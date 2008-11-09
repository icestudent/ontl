/**\file*********************************************************************
 *                                                                     \brief
 *  Class template forward_list [23.2.3 forwardlist]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_FORWARDLIST
#define NTL__STLX_FORWARDLIST

#include "iterator.hxx"
#include "memory.hxx"
#include "type_traits.hxx"

#include "../linked_list.hxx"

namespace std {

/**\addtogroup  lib_containers ********* Containers library [23] ************
 *@{*/

/**\addtogroup  lib_sequence *********** Sequences [23.2] *******************
 *@{*/

  /// Class template forward_list [23.2.3 forwardlist]
  template <class T, class Allocator = allocator<T> >
  class forward_list
  {
    typedef ntl::single_linked single_linked;
    struct node: 
      public single_linked
    {
      T elem;

      node(const T& elem) : elem(elem)   {}
      #ifdef NTL__CXX_RV
      node(T&& elem) : elem(forward<T>(elem))  {}
      node(node&& x): elem(move(x.elem))
      {}
      #endif

    };

    typedef typename forward_list<T, Allocator>::node node_type;
    typedef typename  
      Allocator::template rebind<node_type>::other    node_allocator_type;
  public:
    // types:
    typedef T                                       value_type;
    typedef Allocator                               allocator_type;
    typedef typename  
      Allocator::template rebind<T>::other          allocator;
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
    explicit forward_list(const Allocator& a = Allocator())
      :node_allocator(a)
    {
      init_head();
    }

    explicit forward_list(size_type n)
      :node_allocator()
    {
      init_head();
      while(n--)
        insert_after(end(), forward<value_type>(T()));
    }

    forward_list(size_type n, const T& value, const Allocator& a = Allocator())
      : node_allocator(a)
    {
      init_head();
      insert_after(begin(), n, value);
    }

    template <class InputIterator>
    forward_list(InputIterator first, InputIterator last, const Allocator& = Allocator())
      : node_allocator(a)
    {
      init_head();
      insert_after(begin(), first, last);
    }

    forward_list(const forward_list<T,Allocator>& x)
      : node_allocator(x.node_allocator)
    {
      init_head();
      insert_after(begin(), x.begin(), x.end());
    }

    #ifdef NTL__CXX_RV
    forward_list(forward_list<T,Allocator>&& x)
      :node_allocator()
    {
      init_head();
      swap(x);
    }

    forward_list(initializer_list<T> il, const Allocator& a = Allocator())
      :node_allocator(a)
    {
      init_head();
      insert_after(begin(), il);
    }

    #endif

    ~forward_list()
    {
      clear();
    }

    forward_list<T,Allocator>& operator=(const forward_list<T,Allocator>& x)
    {
      assign(x.begin(), x.end());
      return *this;
    }

    #ifdef NTL__CXX_RV
    forward_list<T,Allocator>& operator=(forward_list<T,Allocator>&& x)
    {
      if(this != &x){
        clear();
        swap(x);
      }
      return *this;
    }
    forward_list& operator=(initializer_list<T> il)
    {
      assign(il);
      return *this;
    }
    #endif
    
    template <class InputIterator>
    void assign(InputIterator first, InputIterator last)
    {
      assign__disp(first, last, is_integral<InputIterator>::type());
    }

    void assign(size_type n, const T& t)
    {
      clear();
      insert_after(begin(), n, t);
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
      insert_after(begin(), first, last);
    }

    template <class IntegralType>
    void assign__disp(IntegralType n, IntegralType x, const true_type&)
    {
      assign(static_cast<size_type>(n), static_cast<value_type>(x));
    }

  public:
    // 23.2.3.2 iterators:
    iterator        before_begin()        { return &head_;     }
    const_iterator  before_begin() const  { return &head_;     }
    iterator        begin()               { return head_.next; }
    const_iterator  begin()        const  { return head_.next; }
    iterator        end()                 { return nullptr;   }
    const_iterator  end()          const  { return nullptr;   }

    const_iterator  cbegin()       const  { return begin();   }
    const_iterator  cbefore_begin()const  { return before_begin(); }
    const_iterator  cend()         const  { return end();     }
    
    // capacity:
    bool empty() const { return head_.next == nullptr; }

    size_type max_size() const { return node_allocator.max_size(); }

    // 23.2.3.3 element access:
    reference front()                     { return *begin();  }
    const_reference front() const         { return *begin();  }
    
    // 23.2.3.4 modifiers:
    
    #ifdef NTL__CXX_VT
    template <class... Args> void emplace_front(Args&&... args)
    {
      insert_after(cbefore_begin(), forward<Args>(args)...);
    }
    #endif

    #ifdef NTL__CXX_RV
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
      erase_after(begin());
    }
    
    #ifdef NTL__CXX_VT
    template <class... Args> iterator emplace_after(const_iterator position, Args&&... args);
    #endif

    #ifdef NTL__CXX_RV
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
      single_linked* node = const_cast<single_linked*>(position.p);
      node_type* np = static_cast<node_type*>(node->next);
      np->unlink(node); // node->next = np->next
      node_allocator.destroy(np);
      node_allocator.deallocate(np, 1);
      return node->next;
    }

    iterator erase_after(const_iterator position, iterator last)
    {
      while ( position != last ) position = erase_after(position);
      return const_cast<single_linked*>(last.p);
    };
    
    #ifdef NTL__CXX_RV
    void swap(forward_list<T,Allocator>&&)
    #else
    void swap(forward_list<T,Allocator>& x)
    #endif
    {
      std::swap(node_allocator, x.node_allocator);
      std::swap(head_.next, x.head_.next);
    }
    
    void resize(size_type sz)
    {
      size_type size_ = size();
      if(sz < size_){
        const_iterator from = cbegin();
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
      size_type size_ = size();
      if(sz < size_){
        const_iterator from = cbegin();
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
      erase_after(begin(), end());
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
    
    #ifdef NTL__CXX_RV
    void splice_after(const_iterator position, forward_list<T,Allocator>&& x);
    void splice_after(const_iterator position, forward_list<T,Allocator>&& x, const_iterator i)
    {
      splice_after(position, x, i, i.p->next);
    }
    void splice_after(const_iterator position, forward_list<T,Allocator>&& x, const_iterator first, const_iterator last);
    #else
    void splice_after(const_iterator position, forward_list<T,Allocator>& x);
    void splice_after(const_iterator position, forward_list<T,Allocator>& x, const_iterator i);
    void splice_after(const_iterator position, forward_list<T,Allocator>& x, const_iterator first, const_iterator last);
    #endif

    template <class Predicate> 
    void remove_if(Predicate pred)
    {
      iterator i = begin();
      while(i.p){
        if(pred(*i))
          i = erase_after(i);
      }
    }

    void remove(const T& value)
    {
      iterator i = begin();
      while(i.p){
        if(*i == value)
          i = erase_after(i);
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
    
#ifdef NTL__CXX_RV
    void merge(forward_list<T,Allocator>&& x)
    {
      merge(less<T>());
    }

    template <class Compare> 
    void merge(forward_list<T,Allocator>&& x, Compare comp)
#else
    void merge(forward_list<T,Allocator>& x)
    {
      merge(x, less<T>());
    }

    template <class Compare> 
    void merge(forward_list<T,Allocator>& x, Compare comp)
#endif
    {
      single_linked* node = &head_;
      while(node->next && x.head_.next){
        if(comp(static_cast<node_type*>(x.head_.next)->elem, static_cast<node_type*>(node->next)->elem)){
          // node->transfer_after(&x.head_, x.head_.next)
          single_linked* tmp = x.head_.next;
          if(x.head_.next){
            x.head_.next = x.head_.next->next;
            x.head_.next->next = node->next;
          }else{
            x.head_.next = nullptr;
          }
          node->next = tmp;
        }
        node = node->next;
      }
      if(x.head_.next){
        node->next = x.head_.next;
        x.head_.next = nullptr;
      }
    }
    
    template <class Compare>
    void sort(Compare comp);

    void sort()
    {
      sort(less<T>());
    }
    
    void reverse()
    {
      single_linked *head = head_.next, *tail = nullptr;
      while(head){
        single_linked* p = head;
        head = head->next;
        p->next = tail;
        tail = p;
      }
      head_.next = tail;
    }

  protected:
    void init_head()
    {
      head_.next = nullptr;
    }

    size_type size() const
    {
      size_type n = 0;
      single_linked* p = head_.next;
      while(p)
        n++, p = p->next;
      return n;
    }
  private:
    single_linked head_;

    node_allocator_type node_allocator;
  };


  // Comparison operators
  template <class T, class Allocator>
  bool operator==(const forward_list<T,Allocator>& x, const forward_list<T,Allocator>& y);

  template <class T, class Allocator>
  bool operator< (const forward_list<T,Allocator>& x, const forward_list<T,Allocator>& y)
  {
    return lexicographical_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  template <class T, class Allocator>
  bool operator!=(const forward_list<T,Allocator>& x, const forward_list<T,Allocator>& y)
  {
    return rel_ops::operator !=(x, y);
  }

  template <class T, class Allocator>
  bool operator> (const forward_list<T,Allocator>& x, const forward_list<T,Allocator>& y)
  {
    return rel_ops::operator >(x, y);
  }

  template <class T, class Allocator>
  bool operator>=(const forward_list<T,Allocator>& x, const forward_list<T,Allocator>& y)
  {
    return rel_ops::operator >=(x, y);
  }

  template <class T, class Allocator>
  bool operator<=(const forward_list<T,Allocator>& x, const forward_list<T,Allocator>& y)
  {
    return rel_ops::operator <=(x, y);
  }


  // 23.2.3.6 specialized algorithms:
  template <class T, class Allocator>
  void swap(forward_list<T,Allocator>& x, forward_list<T,Allocator>& y)
  {
    x.swap(y);
  }
  
  #ifdef NTL__CXX_RV
  template <class T, class Allocator>
  void swap(forward_list<T,Allocator>&& x, forward_list<T,Allocator>& y)
  {
    x.swap(y);
  }

  template <class T, class Allocator>
  void swap(forward_list<T,Allocator>& x, forward_list<T,Allocator>&& y)
  {
    x.swap(y);
  }
  #endif

  /**@} lib_sequence */
  /**@} lib_containers */
}//namespace std

#endif //#ifndef NTL__STLX_FORWARDLIST
