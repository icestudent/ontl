/**\file*********************************************************************
 *                                                                     \brief
 *  Class template deque [23.2.2 deque]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_DEQUE
#define NTL__STLX_DEQUE

#include "memory.hxx"
#include "algorithm.hxx"

namespace std {

/**\addtogroup  lib_containers ********* 23 Containers library [containers]
 *@{*/

/**\addtogroup  lib_sequence *********** 23.2 Sequence containers [sequences]
 *@{*/

  template <class T, class Allocator = allocator<T> >
  class deque 
  {
    typedef typename  
      Allocator::template rebind<T>::other          allocator;
  public:
    // types:
    typedef T                                       value_type;
    typedef Allocator                               allocator_type;
    typedef typename  allocator::pointer            pointer;
    typedef typename  allocator::const_pointer      const_pointer;
    typedef typename  allocator::reference          reference;
    typedef typename  allocator::const_reference    const_reference;
    typedef typename  allocator::size_type          size_type;
    typedef typename  allocator::difference_type    difference_type;

    typedef pointer                                 iterator;
    typedef const_pointer                           const_iterator;
    typedef std::reverse_iterator<iterator>         reverse_iterator;
    typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;

  public:
    ///\name 23.2.2.1 construct/copy/destroy:
    explicit deque(const Allocator& a = Allocator())
      :alloc(a), left(), right(), capL(), capR()
    {}
    explicit deque(size_type n)
      :alloc(), left(), right(), capL(), capR()
    {
      if(n){
        reserve(n);
        capR -= n;
        while(n--)
          alloc.construct(right++, forward<value_type>(value_type()));
      }
    }

    explicit deque(size_type n, const T& value, const Allocator& a = Allocator())
      :alloc(a), left(), right(), capL(), capR()
    {
      if(n){
        reserve(n);
        capR -= n;
        while(n--)
          alloc.construct(right++, value);
      }
    }

    template <class InputIterator>
    deque(InputIterator first, InputIterator last, const Allocator& a = Allocator())
      :alloc(a), left(), right(), capL(), capR()
    {
      assign(first, last);
    }

    deque(const deque<T,Allocator>& x)
      :alloc(x.alloc), left(), right(), capL(), capR()
    {
      assign(x.cbegin(), x.cend());
    }

    deque(const deque& x, const Allocator& a)
      :alloc(a), left(), right(), capL(), capR()
    {
      assign(x.cbegin(), x.cend());
    }
    
    deque(initializer_list<T> il, const Allocator& a = Allocator())
      :alloc(a), left(), right(), capL(), capR()
    {
      assign(il.begin(), il.end());
    }

    #ifdef NTL__CXX_RV
    deque(deque&& x)
      :alloc(), left(), right(), capL(), capR()
    {
      swap(x);
    }
    deque(deque&& x, const Allocator& a);
    #endif

    ~deque()
    {
      dispose();
    }
    
    deque& operator=(initializer_list<T> il)
    {
      dispose();
      assign(il.begin(), il.end());
      return *this;
    }

    deque<T,Allocator>& operator=(const deque<T,Allocator>& x)
    {
      dispose();
      assign(x.cbegin(), x.cend());
      return *this;
    }
    
    #ifdef NTL__CXX_RV
    deque<T,Allocator>& operator=(deque<T,Allocator>&& x)
    {
      dispose();
      swap(x);
      return *this;
    }
    #endif
    
    template <class InputIterator>
    void assign(InputIterator first, InputIterator last)
    {
      assign(first, last, typename iterator_traits<InputIterator>::iterator_category());
    }

    void assign(size_type n, const T& t)
    {
      const size_type cap = capacity();
      if(n > cap){
        dispose();
        reserve(n);
        capR -= n;
      }else{
        clear();
        // adjust pointers to place elements
        // after clear they are in the middle
        const size_type d = n/2;
        left -= d; capL -= d; capR -= d;
        right = left;
      }
      while(n--)
        alloc.construct(right++, t);
    }

    void assign(initializer_list<T> il)
    {
      assign(il.begin(), il.end());
    }
    
    allocator_type get_allocator() const { return alloc; }

    
    ///\name iterators:
    iterator        begin()                 { return left; }
    const_iterator  begin() const           { return left; }
    const_iterator cbegin() const           { return left; }

    iterator        end()                   { return right; }
    const_iterator  end() const             { return right; }
    const_iterator cend() const             { return right; }

    reverse_iterator        rbegin()        { return reverse_iterator(right); }
    const_reverse_iterator  rbegin() const  { return const_reverse_iterator(right); }
    const_reverse_iterator crbegin() const  { return const_reverse_iterator(right); }

    reverse_iterator        rend()          { return reverse_iterator(left); }
    const_reverse_iterator  rend() const    { return const_reverse_iterator(left); }
    const_reverse_iterator crend() const    { return const_reverse_iterator(left); }
    

    ///\name 23.2.2.2 capacity:
    size_type size() const      { return right - left; }
    size_type max_size() const  { return alloc.max_size(); }
    
    void resize(size_type sz)
    {
    #ifndef NTL__CXX_RV
      resize(sz, value_type());
      return;
    #else
      const size_type len = size();
      if(len){
        if(sz < len)
          erase(left+sz, right);
        else if(sz > size())
          insert_n(right, sz-len, forward<value_type>(value_type()));
      }
    #endif
    }
    void resize(size_type sz, const T& c)
    {
      const size_type len = size();
      if(len){
        if(sz < len)
          erase(left+sz, right);
        else if(sz > size())
          insert(right, sz-len, c);
      }
    }

    void shrink_to_fit()
    {
      if(!empty() && size() > can_shrink)
        reallocate();
    }
    
    bool empty() const { return left == right; }
    
    ///\name element access:
    reference       operator[](size_type n)       { return *(left+n); }
    const_reference operator[](size_type n) const { return *(left+n); }
    
    reference at(size_type n)
    {
      if(n > size()) __ntl_throw(out_of_range());
      return *(left+n);
    }
    const_reference at(size_type n) const
    {
      if(n > size()) __ntl_throw(out_of_range());
      return *(left+n);
    }
    
    reference front()             { return *left; }
    const_reference front() const { return *left; }
    reference back()              { return *(right-1); }
    const_reference back() const  { return *(right-1); }
    
    ///\name 23.2.2.3 modifiers:
    #ifdef NTL__CXX_VT
    template <class... Args> void emplace_front(Args&&... args);
    template <class... Args> void emplace_back(Args&&... args);
    template <class... Args> iterator emplace(const_iterator position, Args&&... args);
    #endif

    #ifdef NTL__CXX_RV
    void push_front(T&& x)
    {
      if(empty())
        reserve();
      else if(!capL)
        relocate(false);
      alloc.construct(--left, forward<value_type>(x)); --capL;
    }
    void push_back(T&& x)
    {
      if(empty())
        reserve();
      else if(!capR)
        relocate(true);
      alloc.construct(right++, forward<value_type>(x)); --capR;
    }
    #endif

    void push_front(const T& x)
    {
      if(empty())
        reserve();
      else if(!capL)
        relocate(false);
      alloc.construct(--left, x); --capL;
    }

    void push_back(const T& x)
    {
      if(empty())
        reserve();
      else if(!capR)
        relocate(true);
      alloc.construct(right++, x); --capR;
    }

    void pop_front()
    {
      if(!empty())
        alloc.destroy(left++), ++capL;
    }
    void pop_back()
    {
      if(!empty())
        alloc.destroy(--right), ++capR;
    }

    #ifdef NTL__CXX_RV
    iterator insert(const_iterator position, T&& x)
    {
      assert(position >= left && position <= right);
      if(position == right){
        push_back(forward<value_type>(x));
        return right-1;
      }
      pointer pos = insert_impl(position);
      alloc.construct(pos, forward<value_type>(x));
      --capR;
      return pos;
    }
    #endif

    iterator insert(const_iterator position, const T& x)
    {
      assert(position >= left && position <= right);
      if(position == right){
        push_back(x);
        return right-1;
      }
      pointer pos = insert_impl(position);
      alloc.construct(pos, x);
      --capR;
      return pos;
    }

    void insert(const_iterator position, size_type n, const T& x)
    {
      assert(position >= left && position <= right);
      if(n > capR)
        reallocate(n);
      pointer pos = const_cast<pointer>(position);
      move_backward(pos, right, pos+n); // shift right
      capR -= n; right += n;
      while(n--)
        alloc.construct(pos++, x);
    }

    void insert(const_iterator position, initializer_list<T> il)
    {
      insert(position, il.begin(), il.end());
    }

    template <class InputIterator>
    void insert(const_iterator position, InputIterator first, InputIterator last)
    {
      assert(position >= left && position <= right);
      insert(position, first, last, typename iterator_traits<InputIterator>::iterator_category());
    }

    iterator erase(const_iterator position)
    {
      assert(!empty() && position >= left && position < right);
      iterator pos;
      if(position == left) // front
        alloc.destroy(left++), ++capL, pos = left;
      else if(position == right-1) // back
        alloc.destroy(--right), ++capR, pos = right;
      else{
        // erasing element inside deque, move elements
        pos = const_cast<iterator>(position);
        alloc.destroy(pos);
        move(pos+1, right, pos);
        --right, ++capR;
      }
      return pos;
    }

    iterator erase(const_iterator first, const_iterator last)
    {
      assert(first >= left && last <= right);
      if(first == left && last == right){
        // clear all
        return erase_all(no_dtor());
      }else{
        while(first != last)
          first = erase(first);
        return const_cast<pointer>(last);
      }
    }

    #ifdef NTL__CXX_RV
    void swap(deque<T,Allocator>&& x)
    #else
    void swap(deque<T,Allocator>& x)
    #endif
    {
      pointer l = left, r = right;
      size_type cl = capL, cr = capR;
      left = x.left, right = x.right;
      capL = x.capL, capR = x.capR;
      x.left = l, x.right = r;
      x.capL = cl, x.capR = cr;
      std::swap(alloc, x.alloc);
    }

    void clear()
    {
      if(!empty())
        erase_all(no_dtor());
    }
    ///\}
  protected:
    // we can fast relocate if type is pod
    static const bool can_fast_relocate = 0 && has_trivial_copy_constructor<T>::value;
    static const size_type can_shrink = 32;
    typedef has_trivial_destructor<T> no_dtor;

    // capacity helpers
    size_type capacity_factor(size_type n) const { return (n + 4) * 2; }
    size_type capacity() const { return (right - left) + capL + capR; }
    bool can_relocate(size_type n) const { return n > capacity_factor(size()); }

    // allocate memory
    void reserve(size_type n = 0)
    { // pre: container must be cleared
      const size_type cap = capacity_factor(n);
      pointer p = alloc.allocate(cap);
      //if(!p) return;
      pointer middle = p + cap/2;
      right = left = middle - n/2;
      capL = left - p; capR = cap - capL;
      // post: left & right points to the new uninitialized element (or in the middle if n == 0)
    }

    pointer base() const { return left - capL; }

    void relocate(bool is_right)
    {
      // param: which side needed a space
      if(can_fast_relocate){
        // TODO: define offset to relocate
        if(is_right && can_relocate(capL)){
          // move the contents left
        }else if(!is_right && can_relocate(capR)){
          // move the contents right
        }
        return;
      }
      reallocate();
    }

    void reallocate(size_type n = 0)
    {
      pointer l = left, r = right, p = left - capL;
      const size_type c = max(capacity(), n);
      reserve(c);
      right = move(l, r, right);
      alloc.deallocate(p, c);
    }

    iterator erase_all(true_type)
    {
      // no dtor, just adjust pointers
      const pointer p = base();
      const size_type cap = capacity();
      right = left = p + cap/2;
      capL = left - p; capR = cap - capL;
      return right;
    }

    iterator erase_all(false_type)
    {
      pointer p = left;
      while(p != right)
        alloc.destroy(p++);
      return erase_all(true_type());
    }

    void dispose()
    {
      clear();
      alloc.deallocate(base(), capacity());
    }

    template <class InputIterator>
    void assign(InputIterator first, InputIterator last, random_access_iterator_tag)
    {
      const size_type n = last - first;
      const size_type cap = capacity();
      if(n > cap){
        dispose();
        reserve(n);
        capR -= n;
      }else{
        clear();
        // adjust pointers to place elements
        // after clear they are in the middle
        const size_type d = n/2;
        left -= d; capL -= d; capR -= d;
        right = left;
      }
      while(first != last)
        alloc.construct(right++, *first++);
    }
    template <class InputIterator>
    void assign(InputIterator first, InputIterator last, input_iterator_tag)
    {
      clear();
      while(first != last)
        push_back(*first++);
    }

    pointer insert_impl(const_iterator position)
    {
      if(empty() || !capR)
        reserve(size());
      pointer pos = const_cast<pointer>(position);
      move_backward(pos, right++, pos+1); // shift right
      --capR;
      return pos;
    }

#if 0
    template <class InputIterator>
    void insert(const_iterator position, InputIterator first, InputIterator last, random_access_iterator_tag)
    {
      // if need realloc, save pointers, allocate new and then move there old & new elements with 3 move operations
      // if not, shift right & insert new elements
      size_type n = last - first;
      if(n > capR)
        reallocate(n-capR);
      move_backward(pos, right, pos+n); // shift right
      capR -= n;

      while(n--)
        alloc.construct(right++, *first++);
    }
#endif

    template <class InputIterator>
    void insert(const_iterator position, InputIterator first, InputIterator last, input_iterator_tag)
    {
      iterator pos = const_cast<iterator>(position);
      while(first != last)
        pos = insert(pos, *first++);
    }


  private:
    pointer left, right;
    size_type capL, capR;
    allocator alloc;
  };


  template <class T, class Allocator>
  inline bool operator==(const deque<T,Allocator>& x, const deque<T,Allocator>& y)
  {
    return x.size() == y.size() && std::equal(x.cbegin(), x.cend(), y.cbegin());
  }
  template <class T, class Allocator>
  inline bool operator< (const deque<T,Allocator>& x, const deque<T,Allocator>& y)
  {
    return lexicographical_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }
  template <class T, class Allocator>
  inline bool operator!=(const deque<T,Allocator>& x, const deque<T,Allocator>& y)
  {
    return rel_ops::operator !=(x, y);
  }
  template <class T, class Allocator>
  inline bool operator> (const deque<T,Allocator>& x, const deque<T,Allocator>& y)
  {
    return rel_ops::operator >(x, y);
  }
  template <class T, class Allocator>
  inline bool operator>=(const deque<T,Allocator>& x, const deque<T,Allocator>& y)
  {
    return rel_ops::operator >=(x, y);
  }
  template <class T, class Allocator>
  inline bool operator<=(const deque<T,Allocator>& x, const deque<T,Allocator>& y)
  {
    return rel_ops::operator <=(x, y);
  }
  
  
  // specialized algorithms:
  template <class T, class Allocator>
  inline void swap(deque<T,Allocator>& x, deque<T,Allocator>& y)  { x.swap(y); }
  
  #ifdef NTL__CXX_RV
  template <class T, class Allocator>
  inline void swap(deque<T,Allocator>&& x, deque<T,Allocator>& y) { x.swap(y); }
  template <class T, class Allocator>
  inline void swap(deque<T,Allocator>& x, deque<T,Allocator>&& y) { x.swap(y); }
  #endif
  
  
  template <class T, class Alloc>
  struct constructible_with_allocator_suffix<deque<T, Alloc> >
    : true_type { };

  /**@} lib_sequence */
  /**@} lib_containers */
}//namespace std
#endif //#ifndef NTL__STLX_DEQUE
