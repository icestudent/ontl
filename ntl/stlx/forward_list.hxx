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

namespace std {

/**\addtogroup  lib_containers ********* Containers library [23] ************
 *@{*/

/**\addtogroup  lib_sequence *********** Sequences [23.2] *******************
 *@{*/

  /// Class template forward_list [23.2.3 forwardlist]
  template <class T, class Allocator = allocator<T> >
  class forward_list
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

  public:
    // 23.2.3.1 construct/copy/destroy:
    explicit forward_list(const Allocator& = Allocator());
    explicit forward_list(size_type n, const T& value = T(), const Allocator& = Allocator());
    template <class InputIterator>
    forward_list(InputIterator first, InputIterator last, const Allocator& = Allocator());
    forward_list(const forward_list<T,Allocator>& x);
    #ifdef NTL__CXX
    forward_list(forward_list<T,Allocator>&& x);
    forward_list(initializer_list<T>, const Allocator& = Allocator());
    #endif
    ~forward_list();

    forward_list<T,Allocator>& operator=(const forward_list<T,Allocator>& x);
    #ifdef NTL__CXX
    forward_list<T,Allocator>& operator=(forward_list<T,Allocator>&& x);
    forward_list& operator=(initializer_list<T>);
    #endif
    
    template <class InputIterator>
    void assign(InputIterator first, InputIterator last);
    void assign(size_type n, const T& t);
    #ifdef NTL__CXX
    void assign(initializer_list<T>);
    #endif
    
    allocator_type get_allocator() const;
    
    // 23.2.3.2 iterators:
    iterator before_begin();
    const_iterator before_begin() const;
    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cbefore_begin() const;
    const_iterator cend() const;
    
    // capacity:
    bool empty() const;
    size_type max_size() const;

    // 23.2.3.3 element access:
    reference front();
    const_reference front() const;
    
    // 23.2.3.4 modifiers:
    
    #ifdef NTL__CXX
    template <class... Args> void emplace_front(Args&&... args);
    void push_front(T&& x);
    #endif
    void push_front(const T& x);
    void pop_front();
    
    
    #ifdef NTL__CXX
    template <class... Args> iterator emplace_after(const_iterator position, Args&&... args);
    iterator insert_after(const_iterator position, T&& x);
    #endif
    iterator insert_after(const_iterator position, const T& x);
    void insert_after(const_iterator position, size_type n, const T& x);
    template <class InputIterator>
    void insert_after(const_iterator position, InputIterator first, InputIterator last);
    #ifdef NTL__CXX
    void insert_after(const_iterator position, initializer_list<T> il);
    #endif
    iterator erase_after(const_iterator position);
    iterator erase_after(const_iterator position, iterator last);
    
    
    #ifdef NTL__CXX
    void swap(forward_list<T,Allocator>&&);
    #else
    void swap(forward_list<T,Allocator>& x);
    #endif
    
    void resize(size_type sz);
    void resize(size_type sz, value_type c);
    
    void clear();
    
    // 23.2.3.5 forward_list operations:
    
    #ifdef NTL__CXX
    void splice_after(const_iterator position, forward_list<T,Allocator>&& x);
    void splice_after(const_iterator position, forward_list<T,Allocator>&& x,
      const_iterator i);
    void splice_after(const_iterator position, forward_list<T,Allocator>&& x,
      const_iterator first, const_iterator last);
    #else
    void splice_after(const_iterator position, forward_list<T,Allocator>& x);
    void splice_after(const_iterator position, forward_list<T,Allocator>& x, const_iterator i);
    void splice_after(const_iterator position, forward_list<T,Allocator>& x, const_iterator first, const_iterator last);
    #endif

    void remove(const T& value);
    template <class Predicate> 
    void remove_if(Predicate pred);
    
    void unique();
    template <class BinaryPredicate>
    void unique(BinaryPredicate binary_pred);
    
    #ifdef NTL__CXX
    void merge(forward_list<T,Allocator>&& x);
    template <class Compare> 
    void merge(forward_list<T,Allocator>&& x, Compare comp);
    #else
    void merge(forward_list<T,Allocator>& x);
    template <class Compare> 
    void merge(forward_list<T,Allocator>& x, Compare comp);
    #endif
    
    void sort();
    template <class Compare>
    void sort(Compare comp);
    
    void reverse();
  };


  // Comparison operators
  template <class T, class Allocator>
  bool operator==(const forward_list<T,Allocator>& x, const forward_list<T,Allocator>& y);
  template <class T, class Allocator>
  bool operator< (const forward_list<T,Allocator>& x, const forward_list<T,Allocator>& y);
  template <class T, class Allocator>
  bool operator!=(const forward_list<T,Allocator>& x, const forward_list<T,Allocator>& y);
  template <class T, class Allocator>
  bool operator> (const forward_list<T,Allocator>& x, const forward_list<T,Allocator>& y);
  template <class T, class Allocator>
  bool operator>=(const forward_list<T,Allocator>& x, const forward_list<T,Allocator>& y);
  template <class T, class Allocator>
  bool operator<=(const forward_list<T,Allocator>& x, const forward_list<T,Allocator>& y);


  // 23.2.3.6 specialized algorithms:
  template <class T, class Allocator>
  void swap(forward_list<T,Allocator>& x, forward_list<T,Allocator>& y);
  
  #ifdef NTL__CXX
  template <class T, class Allocator>
  void swap(forward_list<T,Allocator>&& x, forward_list<T,Allocator>& y);
  template <class T, class Allocator>
  void swap(forward_list<T,Allocator>& x, forward_list<T,Allocator>&& y);
  #endif

  /**@} lib_sequence */
  /**@} lib_containers */
}//namespace std

#endif //#ifndef NTL__STLX_FORWARDLIST
