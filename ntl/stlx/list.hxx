/**\file*********************************************************************
 *                                                                     \brief
 *  Class template list [23.2.4 list]
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

/**\addtogroup  lib_containers ********* 23 Containers library [containers]
 *@{*/

/**\addtogroup  lib_sequence *********** 23.2 Sequence containers [sequences]
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
        node(const T& elem) : elem(elem)   {}
        #ifdef NTL__CXX_RV
        node(T&& elem) : elem(forward<T>(elem))  {}
        node(node&& x): elem(move(x.elem))
        {}
        #endif
     // protected:
        // It's Ok not to copy the base (double_linked) while
        // copy constructor is called by list::insert() & list::replace() only.
        node(const node & n) : elem(n.elem) {}
    };
    //#pragma warning(pop)

    typedef typename  
      Allocator::template rebind<T>::other    allocator;
    typedef typename list<T, Allocator>::node node_type;

  ///////////////////////////////////////////////////////////////////////////
  public:

    // types:
    typedef           T                           value_type;
    typedef           Allocator                   allocator_type;
    typedef typename  allocator::pointer          pointer;
    typedef typename  allocator::const_pointer    const_pointer;
    typedef typename  allocator::reference        reference;
    typedef typename  allocator::const_reference  const_reference;
    typedef typename  allocator::size_type        size_type;
    typedef typename  allocator::difference_type  difference_type;

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
    explicit list(size_type n)
    : size_(0)
    {
      init_head();
      while(n--)
        insert(end(), forward<value_type>(T()));
    }

    __forceinline
    explicit list(      size_type   n,
                  const T&          value,
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
    : node_allocator(a), size_(0)
    {
      init_head();
      insert(begin(), first, last);
    }

    list(const list<T, Allocator>& x)
    : node_allocator(x.node_allocator), size_(0)
    {
      init_head();
      insert(begin(), x.begin(), x.end());
    }

    list(const list& x, const Allocator& a)
      : node_allocator(a), size_(0)
    {
      init_head();
      insert(begin(), x.begin(), x.end());
    }
    
    #ifdef NTL__CXX_RV
    list(list&& x)
      :node_allocator(), size_(0)
    {
      init_head();
      swap(x);
    }

    list(list&& x, const Allocator& a)
      :node_allocator(a), size_(0)
    {
      init_head();
      if(x.get_allocator() == a){
        swap(x);
      }else{
        // move elements using the node_allocator
        resize(x.size());
        std::move(x.begin(), x.end(), begin());
        x.clear();
      }
    }
    #endif
    
    list(initializer_list<T> il, Allocator& a = Allocator())
      :node_allocator(a), size_(0)
    {
      init_head();
      insert(begin(), il);
    }

    __forceinline
    ~list() { clear(); }

    list<T, Allocator>& operator=(const list<T, Allocator>& x)
    {
      assign(x.begin(), x.end());
      return *this;
    }
    #ifdef NTL__CXX_RV
    list<T,Allocator>& operator=(list<T,Allocator>&& x)
    {
      if(this != &x){
        clear();
        swap(x);
      }
      return *this;
    }
    #endif
    
    list& operator=(initializer_list<T> il)
    {
      assign(il.begin(), il.end());
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
    
    void assign(initializer_list<T> il)
    {
      assign(il.begin(), il.end());
    }

    allocator_type get_allocator() const { return static_cast<allocator_type>(node_allocator); }

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

    void resize(size_type sz)
    {
      //while ( sz < size() ) pop_back();
      if(sz < size()){
        const_iterator from = cbegin();
        for(size_type i = 0; i < sz; ++i, ++from);
        erase(from, cend());
      }else if(sz > size()) {
        size_type n = sz - size();
        while(n--)
          insert(end(), forward<value_type>(T()));
      }
    }

    __forceinline
    void resize(size_type sz, const T& c)
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

    #ifdef NTL__CXX_VT
    template <class... Args> void emplace_front(Args&&... args);
    template <class... Args> void emplace_back(Args&&... args);
    template <class... Args> iterator emplace(const_iterator position, Args&&... args);
    #endif

    #ifdef NTL__CXX_RV
    __forceinline
    void push_front(T&& x) { insert(begin(), move(x)); }
    __forceinline
    void push_back(T&& x)  { insert(end(), move(x));   }
    #endif

    __forceinline
    void push_front(const T& x) { insert(begin(), x); }

    __forceinline
    void pop_front()            { erase(begin()); }

    __forceinline
    void push_back(const T& x)  { insert(end(), x); }

    __forceinline
    void pop_back()             { erase(--end()); }

    __forceinline
    iterator insert(const_iterator position, const T& x)
    {
      ++size_;
      node_type * const p = node_allocator.allocate(1);
      //get_allocator().construct(&p->elem, x);
      node_allocator.construct(p, x);
      double_linked* const np = const_cast<double_linked*>(position.p);
      p->link(np->prev, np);
      return p;
    }

    #ifdef NTL__CXX_RV
    __forceinline
    iterator insert(const_iterator position, T&& x)
    {
      ++size_;
      node_type * const p = node_allocator.allocate(1);
      node_allocator.construct(p, forward<T>(x));
      double_linked* const np = const_cast<double_linked*>(position.p);
      p->link(np->prev, np);
      return p;
    }
    #endif
    
    __forceinline
    void insert(const_iterator position, size_type n, const T& x)
    {
      while ( n-- ) insert(position, x);
    }

    template <class InputIterator>
    __forceinline
    void insert(const_iterator position, InputIterator first, InputIterator last)
    {
      insert__disp(position, first, last, is_integral<InputIterator>::type());
    }

    void insert(const_iterator position, initializer_list<T> il)
    {
      insert(position, il.begin(), il.end());
    }
    
    __forceinline
    iterator erase(const_iterator position)
    {
      iterator res( position.p->next );
      node_type* const np = const_cast<node_type*>(static_cast<const node_type*>(position.p));
      np->unlink();
      --size_;
      node_allocator.destroy(np);
      node_allocator.deallocate(np, 1);
      return res;
    }

    __forceinline
    iterator erase(const_iterator position, const_iterator last)
    {
      while ( position != last ) position = erase(position);
      return const_cast<double_linked*>(last.p);
    }

    #ifdef NTL__CXX_RV
    void swap(list<T,Allocator>&& x)
    #else
    void swap(list<T, Allocator>& x)
#endif
    {
      std::swap(size_, x.size_);
      std::swap(head.next, x.head.next);
      std::swap(head.prev, x.head.prev);
      std::swap(node_allocator, x.node_allocator);
    }

    __forceinline
    void clear() { erase(begin(), end()); }

    ///@}

  private:

    template <class InputIterator>
    void insert__disp(const_iterator position, InputIterator first, InputIterator last,
                      const false_type&)
    {
      while ( first != last ) position = insert(position, *--last);
    }

    template <class IntegralType>
    void insert__disp(const_iterator position, IntegralType n, IntegralType x,
                      const true_type&)
    {
      insert(position, static_cast<size_type>(n), static_cast<value_type>(x));
    }

  public:

    ///\name list operations [23.2.4.4]

    #ifdef NTL__CXX_RV
    void splice(const_iterator position, list<T,Allocator>&& x);
    void splice(const_iterator position, list<T,Allocator>&& x, const_iterator i);
    void splice(const_iterator position, list<T,Allocator>&& x,
      const_iterator first, const_iterator last);
    #else
    void splice(iterator position, list<T, Allocator>& x);

    void splice(iterator position, list<T, Allocator>& x, iterator i);

    void splice(iterator position, list<T, Allocator>& x,
                iterator first, iterator last);
    #endif

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

    #ifdef NTL__CXX_RV
    void merge(list<T,Allocator>&& x);
    template <class Compare>
    void merge(list<T,Allocator>&& x, Compare comp);
    #else
    void merge(list<T, Allocator>& x);
    template <class Compare>
    void merge(list<T, Allocator>& x, Compare comp);
    #endif

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

#ifdef NTL__CXX_RV
template <class T, class Allocator>
void swap(list<T,Allocator>&& x, list<T,Allocator>& y) { x.swap(y); }
template <class T, class Allocator>
void swap(list<T,Allocator>& x, list<T,Allocator>&& y) { x.swap(y); }
#endif

template <class T, class Alloc>
struct constructible_with_allocator_suffix<list<T, Alloc> >
  : true_type { };

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
