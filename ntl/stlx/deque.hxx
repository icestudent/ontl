/**\file*********************************************************************
 *                                                                     \brief
 *  Class template deque [23.2.2 deque]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_DEQUE
#define NTL__STLX_DEQUE

#include "iterator.hxx"
#include "memory.hxx"
#include "type_traits.hxx"

namespace std {

/**\addtogroup  lib_containers ********* Containers library [23] ************
 *@{*/

/**\addtogroup  lib_sequence *********** Sequences [23.2] *******************
 *@{*/

  template <class T, class Allocator = allocator<T> >
  class deque 
  {
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

    typedef pointer                                 iterator;
    typedef const_pointer                           const_iterator;
    typedef std::reverse_iterator<iterator>         reverse_iterator;
    typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;

  public:
    // 23.2.2.1 construct/copy/destroy:
    explicit deque(const Allocator& = Allocator());
    explicit deque(size_type n, const T& value = T(), const Allocator& a = Allocator());

    template <class InputIterator>
    deque(InputIterator first, InputIterator last, const Allocator& = Allocator());
    deque(const deque<T,Allocator>& x);
    deque(const deque& x, const Allocator& a);
    
    #ifdef NTL__CXX_RV
    deque(deque&&);
    deque(deque&&, const Allocator&);
    deque(initializer_list<T>, const Allocator& = Allocator());
    #endif

    ~deque();
    
    deque<T,Allocator>& operator=(const deque<T,Allocator>& x);
    
    #ifdef NTL__CXX_RV
    deque<T,Allocator>& operator=(deque<T,Allocator>&& x);
    deque& operator=(initializer_list<T>);
    void assign(initializer_list<T>);
    #endif
    
    template <class InputIterator>
    void assign(InputIterator first, InputIterator last);
    void assign(size_type n, const T& t);
    
    allocator_type get_allocator() const;
    
    // iterators:
    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;
    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    reverse_iterator rend();
    const_reverse_iterator rend() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;
    
    // 23.2.2.2 capacity:
    size_type size() const;
    size_type max_size() const;
    
    void resize(size_type sz);
    void resize(size_type sz, const T& c);
    
    bool empty() const;
    
    // element access:
    reference operator[](size_type n);
    const_reference operator[](size_type n) const;
    
    reference at(size_type n);
    const_reference at(size_type n) const;
    
    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;
    
    // 23.2.2.3 modifiers:
    #ifdef NTL__CXX_VT
    template <class... Args> void emplace_front(Args&&... args);
    template <class... Args> void emplace_back(Args&&... args);
    template <class... Args> iterator emplace(const_iterator position, Args&&... args);
    #endif
    #ifdef NTL__CXX_RV
    void push_front(T&& x);
    void push_back(T&& x);
    #endif

    void push_front(const T& x);
    void push_back(const T& x);
    void pop_front();
    void pop_back();

    #ifdef NTL__CXX_RV
    iterator insert(const_iterator position, T&& x);
    void insert(const_iterator position, initializer_list<T>);
    #endif

    iterator insert(const_iterator position, const T& x);
    void insert(const_iterator position, size_type n, const T& x);

    template <class InputIterator>
    void insert (const_iterator position, InputIterator first, InputIterator last);

    iterator erase(const_iterator position);
    iterator erase(const_iterator first, const_iterator last);

    #ifdef NTL__CXX_RV
    void swap(deque<T,Allocator>&&);
    #else
    void swap(deque<T,Allocator>& x);
    #endif

    void clear();
  };


  template <class T, class Allocator>
  bool operator==(const deque<T,Allocator>& x, const deque<T,Allocator>& y);
  template <class T, class Allocator>
  bool operator< (const deque<T,Allocator>& x, const deque<T,Allocator>& y);
  template <class T, class Allocator>
  bool operator!=(const deque<T,Allocator>& x, const deque<T,Allocator>& y)
  {
    return rel_ops::operator !=(x, y);
  }

  template <class T, class Allocator>
  bool operator> (const deque<T,Allocator>& x, const deque<T,Allocator>& y)
  {
    return rel_ops::operator >(x, y);
  }

  template <class T, class Allocator>
  bool operator>=(const deque<T,Allocator>& x, const deque<T,Allocator>& y)
  {
    return rel_ops::operator >=(x, y);
  }

  template <class T, class Allocator>
  bool operator<=(const deque<T,Allocator>& x, const deque<T,Allocator>& y)
  {
    return rel_ops::operator <=(x, y);
  }
  
  
  // specialized algorithms:
  template <class T, class Allocator>
  void swap(deque<T,Allocator>& x, deque<T,Allocator>& y)
  {
    x.swap(y);
  }
  
  #ifdef NTL__CXX_RV
  template <class T, class Allocator>
  void swap(deque<T,Allocator>&& x, deque<T,Allocator>& y)
  {
    x.swap(y);
  }

  template <class T, class Allocator>
  void swap(deque<T,Allocator>& x, deque<T,Allocator>&& y)
  {
    x.swap(y);
  }
  #endif
  
  
  template <class T, class Alloc>
  struct constructible_with_allocator_suffix<deque<T, Alloc> >
    : true_type { };

  /**@} lib_sequence */
  /**@} lib_containers */
}//namespace std

#endif //#ifndef NTL__STLX_DEQUE
