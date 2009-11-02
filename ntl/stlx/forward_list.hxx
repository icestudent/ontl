/**\file*********************************************************************
 *                                                                     \brief
 *  Class template forward_list [23.2.3 forwardlist]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_FORWARDLIST
#define NTL__STLX_FORWARDLIST
#pragma once

#include "memory.hxx"

#include "../linked_list.hxx"

namespace std {

/**\addtogroup  lib_containers ********* 23 Containers library [containers]
 *@{*/

/**\addtogroup  lib_sequence *********** 23.2 Sequence containers [sequences]
 *@{*/

  /// Class template forward_list [23.2.3 forwardlist]
  template <class T, class Alloc = allocator<T> >
  class forward_list
  {
    typedef ntl::single_linked single_linked;
    struct node: public single_linked
    {
      T elem;

      node(const T& elem) : elem(elem)   {}
      #ifdef NTL__CXX_RV
      node(T&& elem) : elem(forward<T>(elem))  {}
      node(node&& x): elem(move(x.elem))
      {}
      #endif

      static void transfer_after(single_linked* p, single_linked* first)
      {
        single_linked* last = first;
        while(last && last->next)
          last = last->next;
        transfer_after(p, first, last);
      }

      static void transfer_after(single_linked* p, single_linked* first, single_linked* last)
      {
        single_linked* tmp = first->next;
        if(last){
          first->next = last->next;
          last->next = p->next;
        }else{
          first->next = nullptr;
        }
        p->next = tmp;
      }
    };

    typedef typename forward_list<T, Alloc>::node   node_type;
    typedef typename  
      Alloc::template rebind<node_type>::other      node_allocator_type;

  #ifdef NTL__CXX_RV
    typedef forward_list<T, Alloc>&& list_reference;
  #else
    typedef forward_list<T, Alloc>&  list_reference;
  #endif

  public:
    // types:
    typedef T                                       value_type;
    typedef Alloc                                   allocator_type;
    typedef typename  
      Alloc::template rebind<T>::other              allocator;
    typedef typename  allocator::pointer            pointer;
    typedef typename  allocator::const_pointer      const_pointer;
    typedef typename  allocator::reference          reference;
    typedef typename  allocator::const_reference    const_reference;
    typedef typename  allocator::size_type          size_type;
    typedef typename  allocator::difference_type    difference_type;

  private:
    struct iterator__impl
      : public std::iterator<forward_iterator_tag, value_type,
      difference_type, pointer, reference>
    {
      iterator__impl() /*: p(0)*/ {}
      iterator__impl(const iterator__impl& i) : p(i.p) {}

      reference operator* () const
      { return static_cast<node_type*>(this->p)->elem; }
      pointer   operator->() const { return &operator*(); }
      iterator__impl & operator++() { p = p->next; return *this; }
      iterator__impl operator++(int)
      { iterator__impl tmp( *this ); ++*this; return tmp; }

      friend bool operator==(const iterator__impl& x, const iterator__impl& y)
      { return x.p == y.p; }

      friend bool operator!=(const iterator__impl& x, const iterator__impl& y)
      { return x.p != y.p; }

      friend class forward_list;
      friend struct const_iterator__impl;

    private:
      single_linked * p;
      iterator__impl(single_linked * const p) : p(p) {}
      iterator__impl(nullptr_t): p(nullptr){}
    };

    struct const_iterator__impl
      : public std::iterator<forward_iterator_tag, value_type,
      difference_type, const_pointer, const_reference>
    {
      const_iterator__impl() /*: p(0)*/ {}
      const_iterator__impl(const typename forward_list::iterator__impl& i) : p(i.p) {} //`typename list::' works around MSVC's /Ze
      const_iterator__impl(const const_iterator__impl& i) : p(i.p) {}

      const_reference operator* () const
      { return static_cast<const node_type*>(this->p)->elem; }
      const_pointer   operator->() const { return &operator*(); }
      const_iterator__impl & operator++() { p = p->next; return *this; }
      const_iterator__impl operator++(int)
      { const_iterator__impl tmp( *this ); ++*this; return tmp; }

      friend bool
        operator==(const const_iterator__impl& x, const const_iterator__impl& y)
      { return x.p == y.p; }

      friend bool
        operator!=(const const_iterator__impl& x, const const_iterator__impl& y)
      { return x.p != y.p; }

      friend class forward_list;

    private:
      const_iterator__impl(const single_linked * const p) : p(p) {}
      const_iterator__impl(nullptr_t): p(nullptr){}
      const single_linked * p;
    };

  public:
    typedef iterator__impl iterator;
    typedef const_iterator__impl const_iterator;

  public:
    // 23.2.3.1 construct/copy/destroy:
    explicit forward_list(const Alloc& a = Alloc())
      :node_allocator(a)
    {
      init_head();
    }

    explicit forward_list(size_type n)
      :node_allocator()
    {
      init_head();
      while(n--)
        insert_after(cbefore_begin(), forward<value_type>(T()));
    }

    forward_list(size_type n, const T& value, const Alloc& a = Alloc())
      : node_allocator(a)
    {
      init_head();
      insert_after(cbefore_begin(), n, value);
    }

    template <class InputIterator>
    forward_list(InputIterator first, InputIterator last, const Alloc& a = Alloc())
      : node_allocator(a)
    {
      init_head();
      insert_after(cbefore_begin(), first, last);
    }

    forward_list(const forward_list<T,Alloc>& x)
      : node_allocator(x.node_allocator)
    {
      init_head();
      insert_after(cbefore_begin(), x.begin(), x.end());
    }

  #ifdef NTL__CXX_RV
    forward_list(forward_list<T,Alloc>&& x)
      :node_allocator()
    {
      init_head();
      swap(x);
    }
  #endif

    forward_list(initializer_list<T> il, const Alloc& a = Alloc())
      :node_allocator(a)
    {
      init_head();
      insert_after(cbefore_begin(), il);
    }

    ~forward_list()
    {
      if(!empty())
        clear();
    }

    forward_list<T,Alloc>& operator=(const forward_list<T,Alloc>& x)
    {
      assign(x.begin(), x.end());
      return *this;
    }

  #ifdef NTL__CXX_RV
    forward_list<T,Alloc>& operator=(forward_list<T,Alloc>&& x)
    {
      if(this != &x){
        clear();
        swap(x);
      }
      return *this;
    }
  #endif
    forward_list& operator=(initializer_list<T> il)
    {
      assign(il);
      return *this;
    }

    
    template <class InputIterator>
    void assign(InputIterator first, InputIterator last)
    {
      assign__disp(first, last, is_integral<InputIterator>::type());
    }

    void assign(size_type n, const T& t)
    {
      clear();
      insert_after(cbefore_begin(), n, t);
    }

    void assign(initializer_list<T> il)
    {
      assign(il.begin(), il.end());
    }
    
    allocator_type get_allocator() const { return static_cast<allocator_type>(node_allocator); }
    
  private:
    template <class InputIterator>
    void assign__disp(InputIterator first, InputIterator last, const false_type&)
    {
      clear();
      insert_after(cbefore_begin(), first, last);
    }

    template <class IntegralType>
    void assign__disp(IntegralType n, IntegralType x, const true_type&)
    {
      assign(static_cast<size_type>(n), static_cast<value_type>(x));
    }

  public:
    // 23.2.3.2 iterators:
    iterator        before_begin()        { return &head;     }
    const_iterator  before_begin() const  { return &head;     }
    iterator        begin()               { return head.next; }
    const_iterator  begin()        const  { return head.next; }
    iterator        end()                 { return nullptr;   }
    const_iterator  end()          const  { return nullptr;   }

    const_iterator  cbegin()       const  { return begin();   }
    const_iterator  cbefore_begin()const  { return before_begin(); }
    const_iterator  cend()         const  { return end();     }
    
    // capacity:
    bool empty() const { return head.next == nullptr; }

    size_type max_size() const { return node_allocator.max_size(); }

    // 23.2.3.3 element access:
    reference front()                     { return *begin();  }
    const_reference front() const         { return *begin();  }
    
    // 23.2.3.4 modifiers:
    
  #ifdef NTL__CXX_VT
    template <class... Args>
    void emplace_front(Args&&... args)
    {
      insert_after(cbefore_begin(), forward<Args>(args)...);
    }
  #elif defined(NTL__CXX_RV)

    template<class Arg1>
    void emplace_front(Arg1&& arg1)
    {
      insert_after(cbefore_begin(), value_type(forward<Arg1>(arg1)));
    }

    template<class Arg1, class Arg2>
    void emplace_front(Arg1&& arg1, Arg2&& arg2)
    {
      insert_after(cbefore_begin(), value_type(forward<Arg1>(arg1), forward<Arg2>(arg2)));
    }

    void push_front(T&& x)
    {
      insert_after(cbefore_begin(), move(x));
    }
  #endif

    void push_front(const T& x)
    {
      insert_after(cbefore_begin(), x);
    }

    void pop_front()
    {
      erase_after(cbefore_begin());
    }
    
  #ifdef NTL__CXX_VT
    template <class... Args> iterator emplace_after(const_iterator position, Args&&... args);
  #elif defined NTL__CXX_RV
    template <class Arg1>
    iterator emplace_after(const_iterator position, Arg1&& arg1)
    {
      return insert_after(position, value_type(forward<Arg1>(arg1)));
    }
    template <class Arg1, class Arg2>
    iterator emplace_after(const_iterator position, Arg1&& arg1, Arg2&& arg2)
    {
      return insert_after(position, value_type(forward<Arg1>(arg1), forward<Arg2>(arg2)));
    }

    iterator insert_after(const_iterator position, T&& x)
    {
      node* const p = node_allocator.allocate(1);
      node_allocator.construct(p, forward<T>(x));
      single_linked* pp = const_cast<single_linked*>(position.p);
      p->link(pp);
      return p;
    }
  #endif

    iterator insert_after(const_iterator position, const T& x)
    {
      node_type* const p = node_allocator.allocate(1);
      node_allocator.construct(p, x);
      single_linked* pp = const_cast<single_linked*>(position.p);
      p->link(pp);
      return p;
    }

    void insert_after(const_iterator position, size_type n, const T& x)
    {
      while ( n-- ) insert_after(position, x);
    }

    template <class InputIterator>
    void insert_after(const_iterator position, InputIterator first, InputIterator last)
    {
      insert__disp(position, first, last, is_integral<InputIterator>::type());
    }

    void insert_after(const_iterator position, initializer_list<T> il)
    {
      insert_after(position, il.begin(), il.end());
    }

    iterator erase_after(const_iterator position)
    {
  #if 0
      single_linked* node = const_cast<single_linked*>(position.p);
      if(node){
        node_type* np = static_cast<node_type*>(node->next);
        if(np){
          //np->unlink(node); 
          node->next = np->next;
          node_allocator.destroy(np);
          node_allocator.deallocate(np, 1);
        }
      }
      return node ? node->next : node;
  #else
      single_linked* pos = const_cast<single_linked*>(position.p);
      if(pos){
        // return erase(tmp)
        node_type* cur = static_cast<node_type*>(pos->next);
        if(cur){
          single_linked* next = cur->next;
          pos->next = next;
          node_allocator.destroy(cur);
          node_allocator.deallocate(cur, 1);
        }
        return pos;
      }else{
        return end();
      }
  #endif
    }

    iterator erase_after(const_iterator position, iterator last)
    {
  #if 0
      while ( position != last ) position = erase_after(position);
      return const_cast<single_linked*>(last.p);
  #else
      single_linked* pos = const_cast<single_linked*>(position.p);
      single_linked* cur = pos->next;
      while(cur){
        node_type* tmp = static_cast<node_type*>(cur);
        cur = cur->next;
        node_allocator.destroy(tmp);
        node_allocator.deallocate(tmp, 1);
        pos->next = cur;
        if(tmp == last.p)
          break;
      }
      return pos;
  #endif
    };
    
    void swap(list_reference x)
    {
      std::swap(node_allocator, x.node_allocator);
      std::swap(head.next, x.head.next);
    }
    
    void resize(size_type sz)
    {
      size_type size_ = get_size();
      if(sz < size_){
        const_iterator from = cbefore_begin();
        for(size_type i = 0; i < sz; ++i, ++from);
        erase_after(from, end());
      }else if(sz > size_) {
        size_type n = sz - size_;
        while(n--)
          insert_after(end(), forward<value_type>(T()));
      }
    }

    void resize(size_type sz, const value_type& c)
    {
      size_type size_ = get_size();
      if(sz < size_){
        const_iterator from = cbefore_begin();
        for(size_type i = 0; i < sz; ++i, ++from);
        erase_after(from, end());
      }else if(sz > size_) {
        size_type n = sz - size_;
        while(n--)
          insert_after(end(), c);
      }
    }
    
    void clear()
    {
      if(!empty())
        erase_after(cbefore_begin(), end());
    }

  private:
    template <class InputIterator>
    void insert__disp(const_iterator position, InputIterator first, InputIterator last, const false_type&)
    {
      while ( first != last ) position = insert_after(position, *first++);
    }

    template <class IntegralType>
    void insert__disp(const_iterator position, IntegralType n, IntegralType x, const true_type&)
    {
      insert_after(position, static_cast<size_type>(n), static_cast<value_type>(x));
    }

  public:
    // 23.2.3.5 forward_list operations:
    
    void splice_after(const_iterator position, list_reference x)
    {
      if(!x.empty() && &x != this){
        iterator first = x.before_begin();
        node::transfer_after(const_cast<single_linked*>(position.p), first.p); 
        x.clear();
      }
    }

    void splice_after(const_iterator position, list_reference x, const_iterator i)
    {
      splice_after(position, x, i, i.p->next);
    }

    void splice_after(const_iterator position, list_reference /*x*/, const_iterator first, const_iterator last)
    {
      node::transfer_after(const_cast<single_linked*>(position.p), const_cast<single_linked*>(first.p), const_cast<single_linked*>(last.p)); 
    }

    template <class Predicate> 
    void remove_if(Predicate pred)
    {
      single_linked* node = &head;
      while(node->next){
        if(pred(static_cast<node_type*>(node->next)->elem))
          erase_after(node);
        else
          node = node->next;
      }
    }

    void remove(const T& value)
    {
      single_linked* node = &head;
      while(node->next){
        if(static_cast<node_type*>(node->next)->elem == value)
          erase_after(node);
        else
          node = node->next;
      }
    }
    
    template <class BinaryPredicate>
    void unique(BinaryPredicate binary_pred)
    {
      iterator first = begin(), last = end();
      if(first == last)
        return;
      for(iterator next = first; ++next != last; next = first){
        if(binary_pred(*first, *next))
          erase_after(first);
        else
          first = next;
      }
    }

    void unique()
    {
      unique(equal_to<T>());

    }
    
    void merge(list_reference x)
    {
      merge(x, less<T>());
    }

    template <class Compare> 
    void merge(list_reference x, Compare comp)
    {
      single_linked* node = &head;
      while(node->next && x.head.next){
        if(comp(static_cast<node_type*>(x.head.next)->elem, static_cast<node_type*>(node->next)->elem))
          node::transfer_after(node, &x.head, x.head.next);
        node = node->next;
      }
      if(x.head.next){
        node->next = x.head.next;
        x.head.next = nullptr;
      }
    }
    
    template <class Compare>
    void sort(Compare comp);
    void sort();
    
    void reverse()
    {
      single_linked *head = this->head.next, *tail = nullptr;
      while(head){
        single_linked* p = head;
        head = head->next;
        p->next = tail;
        tail = p;
      }
      this->head.next = tail;
    }

  protected:
    void init_head()
    {
      head.next = nullptr;
    }

    size_type get_size() const
    {
      size_type n = 0;
      single_linked* p = head.next;
      while(p)
        n++, p = p->next;
      return n;
    }
  private:
    single_linked head;

    node_allocator_type node_allocator;
  };


  // Comparison operators
  template <class T, class Alloc>
  bool operator==(const forward_list<T,Alloc>& x, const forward_list<T,Alloc>& y)
  {
    for(typename forward_list<T, Alloc>::const_iterator x1 = x.cbegin(), y1 = y.cbegin(), x2 = x.cend(), y2 = y.cend();
        x1 != x2 && y1 != y2;
        ++x1, ++y1)
    {
      if(*x1 != *y1)
        return false;
    }
    return true;
  }

  template <class T, class Alloc>
  bool operator< (const forward_list<T,Alloc>& x, const forward_list<T,Alloc>& y)
  {
    return lexicographical_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  template <class T, class Alloc>
  bool operator!=(const forward_list<T,Alloc>& x, const forward_list<T,Alloc>& y)
  {
    return rel_ops::operator !=(x, y);
  }

  template <class T, class Alloc>
  bool operator> (const forward_list<T,Alloc>& x, const forward_list<T,Alloc>& y)
  {
    return rel_ops::operator >(x, y);
  }

  template <class T, class Alloc>
  bool operator>=(const forward_list<T,Alloc>& x, const forward_list<T,Alloc>& y)
  {
    return rel_ops::operator >=(x, y);
  }

  template <class T, class Alloc>
  bool operator<=(const forward_list<T,Alloc>& x, const forward_list<T,Alloc>& y)
  {
    return rel_ops::operator <=(x, y);
  }


  // 23.2.3.6 specialized algorithms:
  template <class T, class Alloc>
  void swap(forward_list<T,Alloc>& x, forward_list<T,Alloc>& y)
  {
    x.swap(y);
  }
  
#ifdef NTL__CXX_RV
  template <class T, class Alloc>
  void swap(forward_list<T,Alloc>&& x, forward_list<T,Alloc>& y)
  {
    x.swap(y);
  }

  template <class T, class Alloc>
  void swap(forward_list<T,Alloc>& x, forward_list<T,Alloc>&& y)
  {
    x.swap(y);
  }
#endif

  /**@} lib_sequence */
  /**@} lib_containers */
}//namespace std

#endif //#ifndef NTL__STLX_FORWARDLIST
