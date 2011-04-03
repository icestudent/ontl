/**\file*********************************************************************
 *                                                                     \brief
 *  Class template list [23.2.4 list]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_LIST
#define NTL__STLX_LIST
#pragma once

#include "algorithm.hxx"
#include "memory.hxx"

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

      void unlink()
      { 
        prev->next = next;
        next->prev = prev;
        #ifdef NTL__DEBUG
        prev = next = nullptr;
        #endif
      }
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
    typedef       value_type&                     reference;
    typedef const value_type&                     const_reference;
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
        const_iterator__impl(double_linked * const p) : p(p) {}
        double_linked * p;
    };

  public:

    typedef iterator__impl                        iterator;
    typedef const_iterator__impl                  const_iterator;
    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    ///\name construct/copy/destroy [23.2.4.1]

    explicit list(const Allocator& a = Allocator())
    : node_allocator(a)
    {
      init_head();
    }

    __forceinline
    explicit list(size_type n)
    {
      init_head();
      while(n--)
        insert(end(), T());
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

    list(const list& x, const Allocator& a)
    : node_allocator(a)
    {
      init_head();
      insert(begin(), x.begin(), x.end());
    }
    
    #ifdef NTL__CXX_RV
    list(list&& x)
    : node_allocator()
    {
      init_head();
      swap(x);
    }

    list(list&& x, const Allocator& a)
    : node_allocator(a)
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
    
    list(initializer_list<T> il, const Allocator& a = Allocator())
      :node_allocator(a)
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
      size_type s = size();// I guess it's faster than reallocs - ST
      for ( ; n < s; --s ) pop_back();
      iterator it = begin();
      for ( size_type i = s; i; --i, ++it ) replace(it, t);
      if ( n > s ) insert(it, n - s, t);
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

    bool      empty()     const { return begin() == end(); }
    size_type size()      const { return distance(begin(), end()); }
    size_type max_size()  const { return node_allocator.max_size(); }

    __forceinline
    void resize(size_type sz, const T& c)
    {
      const_iterator odd = cbegin();
      for ( size_type i = 0; i < sz; ++i )
      {
        // if current size is less than required, add new elements
        if ( cend() == odd )
          push_back(c);
        else
          ++odd;
      }
      erase(odd, cend());
    }

    void resize(size_type sz)
    {
      resize(sz, T());
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
      node_type * const p = node_allocator.allocate(1);
      //get_allocator().construct(&p->elem, x);
      node_allocator.construct(p, x);
      double_linked* const np = position.p;
      p->link(np->prev, np);
      return p;
    }

    #ifdef NTL__CXX_RV
    __forceinline
    iterator insert(const_iterator position, T&& x)
    {
      node_type * const p = node_allocator.allocate(1);
      node_allocator.construct(p, forward<T>(x));
      double_linked* const np = position.p;
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
      node_type* const np = static_cast<node_type*>(position.p);
      position.p->unlink();
      node_allocator.destroy(np);
      node_allocator.deallocate(np, 1);
      return res;
    }

    __forceinline
    iterator erase(const_iterator position, const_iterator last)
    {
      while ( position != last ) position = erase(position);
      return last.p;
    }

    #ifdef NTL__CXX_RV
    void swap(list<T,Allocator>&& x)
    {
      using std::swap;
      swap(head.next, x.head.next);
      swap(head.prev, x.head.prev);
      swap(node_allocator, x.node_allocator);
    }
    #endif
    #if !defined(NTL__CXX_RV) || defined(NTL__CXX_RVFIX)
    void swap(list<T, Allocator>& x)
    {
      using std::swap;
      swap(head.next, x.head.next);
      swap(head.prev, x.head.prev);
      swap(node_allocator, x.node_allocator);
    }
    #endif

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
    void splice(const_iterator position, list<T,Allocator>&& x, const_iterator first, const_iterator last);
    #endif

    void splice(const_iterator position, list<T, Allocator>& x);//  assert(&x != this);

    void splice(const_iterator position, list<T, Allocator>& x, const_iterator i)
    {
      (void)&x;
      assert(get_allocator() == x.get_allocator());
     // if ( position == i || position == next(i) ) return;
     // insert(position, *i);
     // x.erase(i);
      if ( position == i /*|| position == next(i)*/ ) return;
      i.p->unlink();
      i.p->link(position.p->prev, position.p);
    }

    void splice(const_iterator position, list<T, Allocator>& x, const_iterator first, const_iterator last);

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
      //if ( size() < 2 ) return;
      iterator i = end();
      if ( i == begin() || --i == begin() ) return;
      do
      { 
        const iterator next = i--;
        if ( *next == *i ) 
          erase(next);
      }
      while ( i != begin() );
    }

    template <class BinaryPredicate>
    void unique(BinaryPredicate binary_pred)
    {
      iterator i = end();
      if ( i == begin() || --i == begin() ) return;
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

  ///////////////////////////////////////////////////////////////////////////
  private:

    mutable double_linked head;
    //size_type     size_;

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
  return equal(x.begin(), x.end(), y.begin());
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

///@}
/**@} lib_sequence */
/**@} lib_containers */

}//namespace std

#ifdef _MSC_VER
#pragma warning(pop)//disable:4820
#endif

#endif//#ifndef NTL__STLX_LIST
