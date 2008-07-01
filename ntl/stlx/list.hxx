/**\file*********************************************************************
 *                                                                     \brief
 *  Class template list [23.2.4 lib.list]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_LIST
#define NTL__STLX_LIST

#include "algorithm.hxx"
#include "iterator.hxx"
#include "memory.hxx"
//#include "stdexcept.hxx"
#include "type_traits.hxx"


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4820)// X bytes padding added...
#endif

namespace std {

/**\addtogroup  lib_containers ********* Containers library [23] ************
 *@{*/

/**\addtogroup  lib_sequence *********** Sequences [23.2] *******************
 *@{*/

/// Class template list [23.2.4]
///\todo  node caching
template <class T, class Allocator = allocator<T> >
class list
{
    struct double_linked
    {
      double_linked * prev, * next;

      void link(double_linked * prev, double_linked * next)
      {
        this->prev = prev; this->next = next;
        prev->next = this; next->prev = this;
      }

      void unlink() { prev->next = next; next->prev = prev; }
    };

    //#pragma warning(push)
    //#pragma warning(disable:4512)//assignment operator could not be generated for const T
    struct node : public double_linked
    {
        T elem;
        node(const T & elem) : elem(elem)   {}
     // protected:
        // It's Ok not to copy the base (double_linked) while
        // copy constructor is called by list::insert() & list::replace() only.
        node(const node & n) : elem(n.elem) {}
    };
    //#pragma warning(pop)

    typedef typename list<T, Allocator>::node node_type;

  ///////////////////////////////////////////////////////////////////////////
  public:

    // types:

    typedef           T                           value_type;
    typedef           Allocator                   allocator_type;
    typedef typename  Allocator::pointer          pointer;
    typedef typename  Allocator::const_pointer    const_pointer;
    typedef typename  Allocator::reference        reference;
    typedef typename  Allocator::const_reference  const_reference;
    typedef typename  Allocator::size_type        size_type;
    typedef typename  Allocator::difference_type  difference_type;

  private:

    struct iterator__impl
    : public std::iterator<bidirectional_iterator_tag, value_type,
                           difference_type, pointer, reference>
    {
        iterator__impl() /*: p(0)*/ {}
        iterator__impl(const iterator__impl& i) : p(i.p) {}

        reference operator* () const 
          { return static_cast<node_type*>(this->p)->elem; }
        pointer   operator->() const { return &operator*(); }
        iterator__impl & operator++() { p = p->next; return *this; }
        iterator__impl & operator--() { p = p->prev; return *this; }
        iterator__impl operator++(int)
          { iterator__impl tmp( *this ); ++*this; return tmp; }
        iterator__impl operator--(int)
          { iterator__impl tmp( *this ); --*this; return tmp; }

      friend bool operator==(const iterator__impl& x, const iterator__impl& y)
        { return x.p == y.p; }

      friend bool operator!=(const iterator__impl& x, const iterator__impl& y)
        { return x.p != y.p; }

      friend class list;
      friend struct const_iterator__impl;

      private:
        double_linked * p;
        iterator__impl(double_linked * const p) : p(p) {}
    };

    struct const_iterator__impl
    : public std::iterator<bidirectional_iterator_tag, value_type,
                           difference_type, const_pointer, const_reference>
    {
        const_iterator__impl() /*: p(0)*/ {}
        const_iterator__impl(const typename list::iterator__impl& i) : p(i.p) {} //`typename list::' works around MSVC's /Ze
        const_iterator__impl(const const_iterator__impl& i) : p(i.p) {}

        const_reference operator* () const 
          { return static_cast<const node_type*>(this->p)->elem; }
        const_pointer   operator->() const { return &operator*(); }
        const_iterator__impl & operator++() { p = p->next; return *this; }
        const_iterator__impl & operator--() { p = p->prev; return *this; }
        const_iterator__impl operator++(int)
          { const_iterator__impl tmp( *this ); ++*this; return tmp; }
        const_iterator__impl operator--(int) 
          { const_iterator__impl tmp( *this ); --*this; return tmp; }

      friend bool
        operator==(const const_iterator__impl& x, const const_iterator__impl& y)
          { return x.p == y.p; }

      friend bool
        operator!=(const const_iterator__impl& x, const const_iterator__impl& y)
          { return x.p != y.p; }

      friend class list;

      private:
        const_iterator__impl(const double_linked * const p) : p(p) {}
        const double_linked * p;
    };

  public:

    typedef iterator__impl                        iterator;
    typedef const_iterator__impl                  const_iterator;
    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    ///\name construct/copy/destroy [23.2.4.1]

    explicit list(const Allocator& a = Allocator())
    : node_allocator(a), size_(0) 
    { 
      init_head();
    }

    __forceinline
    explicit list(      size_type   n,
                  const T&          value = T(),
                  const Allocator&  a     = Allocator())
    : node_allocator(a), size_(0)
    { 
      init_head();
      insert(begin(), n, value);
    }

    template <class InputIterator>
    list(InputIterator    first,
         InputIterator    last,
         const Allocator& a     = Allocator())
    : node_allocator(a)
    {
      init_head();
      insert(begin(), first, last);
    }

    list(const list<T, Allocator>& x)
    : node_allocator(x.node_allocator)
    {
      init_head();
      insert(begin(), x.begin(), x.end());
    }

    __forceinline
    ~list() { clear(); }

    list<T, Allocator>& operator=(const list<T, Allocator>& x)
    {
      //node_allocator = x.node_allocator;
      assign(x.begin(), x.end());
      return *this;
    }

    template <class InputIterator>
    void assign(InputIterator first, InputIterator last)
    {
      assign__disp(first, last, is_integral<InputIterator>::type());
    }

    void assign(size_type n, const T& t)
    {
      //clear();
      //insert(begin(), n, t);
      while ( n < size() ) pop_back();
      iterator it = begin();
      for ( size_type i = size(); i; --i, ++it ) replace(it, t);
      if ( n > size() ) insert(it, n - size(), t);
    }

    allocator_type get_allocator() const { return node_allocator; }

    ///@}

  private:

    template <class InputIterator>
    void assign__disp(InputIterator first, InputIterator last, const false_type&)
    {
      //clear();
      //insert(begin(), first, last);
      for ( iterator it = begin(); ; ++it, ++first )
      {
        if ( first == last ) return (void)erase(it, end());
        if ( it == end() )   return (void)insert(it, first, last);
        replace(it, *first);
      }
    }

    template <class IntegralType>
    void assign__disp(IntegralType n, IntegralType x, const true_type&)
    {
      assign(static_cast<size_type>(n), static_cast<value_type>(x));
    }

  public:

    ///\name iterators:

    iterator                begin()       { return head.next; }
    const_iterator          begin() const { return head.next; }
    iterator                end()         { return &head; }
    const_iterator          end()   const { return &head; }

    reverse_iterator        rbegin()      { return reverse_iterator(end()); }
    const_reverse_iterator  rbegin() const
      { return const_reverse_iterator(end()); }
    reverse_iterator        rend()       { return reverse_iterator(begin()); }
    const_reverse_iterator  rend() const
      { return const_reverse_iterator(begin()); }

    const_iterator          cbegin() const { return begin(); }
    const_iterator          cend()   const { return end(); }
    const_reverse_iterator  crbegin()const { return rbegin(); }
    const_reverse_iterator  crend()  const { return rend(); }

    ///\name capacity [23.2.4.2]

    bool      empty()     const { return size_ == 0; }
    size_type size()      const { return size_; }
    size_type max_size()  const { return node_allocator.max_size(); }

    __forceinline
    void resize(size_type sz, T c = T())
    {
      while ( sz < size() ) pop_back();
      if    ( sz > size() ) insert(end(), sz - size(), c);
    }

    ///\name element access:

    reference       front()       { return *begin(); }
    const_reference front() const { return *begin(); }
    reference       back()        { return *(--end()); }
    const_reference back()  const { return *(--end()); }

    ///\name modifiers [23.2.4.3]

    __forceinline
    void push_front(const T& x) { insert(begin(), x); }
    
    __forceinline
    void pop_front()            { erase(begin()); }

    __forceinline
    void push_back(const T& x)  { insert(end(), x); }

    __forceinline
    void pop_back()             { erase(--end()); }

    __forceinline
    iterator insert(iterator position, const T& x)
    {
      ++size_;
      node_type * const p = node_allocator.allocate(1);
      node_allocator.construct(p, x);
      p->link(position.p->prev, position.p);
      return p;
    }

    __forceinline
    void insert(iterator position, size_type n, const T& x)
    { 
      while ( n-- ) insert(position, x);
    }

    template <class InputIterator>
    __forceinline
    void insert(iterator position, InputIterator first, InputIterator last)
    {
      insert__disp(position, first, last, is_integral<InputIterator>::type());
    }

    __forceinline
    iterator erase(iterator position)
    {
      iterator res( position.p->next );
      position.p->unlink();
      --size_;
      node_allocator.destroy(static_cast<node_type*>(position.p));
      node_allocator.deallocate(static_cast<node_type*>(position.p), 1);
      return res;
    }

    __forceinline
    iterator erase(iterator position, iterator last)
    { 
      while ( position != last ) position = erase(position);
      return last;
    }

    void swap(list<T, Allocator>&);

    __forceinline
    void clear() { erase(begin(), end()); }

    ///@}

  private:

    template <class InputIterator>
    void insert__disp(iterator position, InputIterator first, InputIterator last,
                      const false_type&)
    {
      while ( first != last ) position = insert(position, *--last);
    }

    template <class IntegralType>
    void insert__disp(iterator position, IntegralType n, IntegralType x,
                      const true_type&)
    {
      insert(position, static_cast<size_type>(n), static_cast<value_type>(x));
    }

  public:

    ///\name list operations [23.2.4.4]

    void splice(iterator position, list<T, Allocator>& x);

    void splice(iterator position, list<T, Allocator>& x, iterator i);

    void splice(iterator position, list<T, Allocator>& x,
                iterator first, iterator last);

    void remove(const T& value)
    {
      for ( iterator i = begin(); i != end(); )
      {
        const iterator current = i++;
        if ( *current == value ) erase(current);
      }
    }

    template <class Predicate>
    void remove_if(Predicate pred)
    {
      for ( iterator i = begin(); i != end(); )
      {
        const iterator current = i++;
        if ( pred(*current) != false ) erase(current);
      }
    }

    void unique()
    {
      if ( size() < 2 ) return;
      iterator i = --end();
      do { const iterator next = i--; if ( *next == *i ) erase(next); }
      while ( i != begin() );
    }

    template <class BinaryPredicate>
    void unique(BinaryPredicate binary_pred)
    {
      if ( size() < 2 ) return;
      iterator i = --end();
      do { const iterator next = i--; if ( binary_pred(*next, *i) ) erase(next); }
      while ( i != begin() );
    }

    void merge(list<T, Allocator>& x);

    template <class Compare>
    void merge(list<T, Allocator>& x, Compare comp);

    void sort();

    template <class Compare>
    void sort(Compare comp);

    void reverse();

    ///@}

    static inline
    const char * test__implementation();

  ///////////////////////////////////////////////////////////////////////////
  private:

    double_linked head;
    size_type     size_;

    typename allocator_type::template rebind<node_type>::other node_allocator;

    void init_head() { head.prev = head.next = &head; }

    void replace(iterator position, const T& x)
    {
      // hack: links to prev & next nodes remains, so
      //       be sure to check node constructor
      node_allocator.destroy(static_cast<node_type*>(position.p));
      node_allocator.construct(static_cast<node_type*>(position.p), x);
    }
    
};//class list

///\name  List comparisons

template <class T, class Allocator>
bool operator==(const list<T,Allocator>& x, const list<T,Allocator>& y) __ntl_nothrow
{
  return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}

template <class T, class Allocator>
bool operator< (const list<T,Allocator>& x, const list<T,Allocator>& y) __ntl_nothrow
{
  return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <class T, class Allocator>
bool operator!=(const list<T,Allocator>& x, const list<T,Allocator>& y) __ntl_nothrow
{
  return ! (x == y);
}

template <class T, class Allocator>
bool operator> (const list<T,Allocator>& x, const list<T,Allocator>& y) __ntl_nothrow
{
  return y < x;
}

template <class T, class Allocator>
bool operator>=(const list<T,Allocator>& x, const list<T,Allocator>& y) __ntl_nothrow
{
  return ! (x < y);
}

template <class T, class Allocator>
bool operator<=(const list<T,Allocator>& x, const list<T,Allocator>& y) __ntl_nothrow
{
  return ! (y < x);
}

///\name  List specialized algorithms [23.2.4.5]

template <class T, class Allocator>
void swap(list<T, Allocator>& x, list<T, Allocator>& y) __ntl_nothrow
{ 
  x.swap(y);
}

///@}
/**@} lib_sequence */
/**@} lib_containers */

///////////////////////////////////////////////////////////////////////////
#if 0

template<>
const char * list<int>::test__implementation()
{
  list<int> l( 0 );
  if ( l.size() != 0 )  return "list<int> ( 0 )";

  l.resize(2);
  if ( l.size() != 2 )  return "list<int>::resize(2)";

  l.clear();
  if ( l.size() != 0 )  return "list<int>::clear()";
  
  l.push_back(3);
  l.push_back(4);
  l.push_back(5);
  l.push_front(2);
  l.push_front(1);
  l.push_front(0);

  list<int>::const_iterator i0 = l.begin();
  //list<int>::iterator i1 = i0;//shall not compile
  
  int j = 5;
  for ( list<int>::const_reverse_iterator i = l.rbegin();
        i != static_cast<const_reverse_iterator>(l.rend()); )
  {
    if ( *i++ != j-- )  return "list<int> [0..5]";
  }

  list<int> l2;

  int * p = 0;
  l2.assign(p, p);
  l2.insert(l2.begin(), p, p);

  l2.insert(l2.begin(), l.begin(), l.end());

 // l2.insert(l2.begin(), 5.5, 5);

  return 0;
}

#endif
///////////////////////////////////////////////////////////////////////////

}//namespace std

#ifdef _MSC_VER
#pragma warning(pop)//disable:4820
#endif

#endif//#ifndef NTL__STLX_LIST
