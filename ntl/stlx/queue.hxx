/**\file*********************************************************************
 *                                                                     \brief
 *  Class template queue [23.2.5.1 queue]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_QUEUE
#define NTL__STLX_QUEUE

#include "deque.hxx"

namespace std {

/**\addtogroup  lib_containers ********* Containers library [23] ************
 *@{*/

/**\addtogroup  lib_sequence *********** Sequences [23.2] *******************
 *@{*/

/**\addtogroup  lib_container_adaptors *** Container adaptors [23.2.5]*********
 *@{*/

  /// class template queue [23.2.5.1 queue]
  template <class T, class Container = deque<T> >
  class queue 
  {
  public:
    typedef Container                           container_type;
    typedef typename Container::value_type      value_type;
    typedef typename Container::reference       reference;
    typedef typename Container::const_reference const_reference;
    typedef typename Container::size_type       size_type;
  public:
    explicit queue(const Container&);
    
    #ifdef NTL__CXX_RV
    explicit queue(Container&& = Container());
    queue(queue&& q) : c(std::move(q.c)) {}
    #endif

    template <class Alloc>
    explicit queue(const Alloc&);
    template <class Alloc>
    queue(const Container&, const Alloc&);
    
    #ifdef NTL__CXX_RV
    template <class Alloc>
    queue(Container&&, const Alloc&);
    template <class Alloc>
    queue(queue&&, const Alloc&);
    #endif

    
    #ifdef NTL__CXX_RV
    queue& operator=(queue&& q) { c = std::move(q.c); return *this; }
    #else
    queue& operator=(const queue& q);
    #endif

    bool empty() const { return c.empty(); }
    size_type size() const { return c.size(); }
    reference front() { return c.front(); }

    const_reference front() const { return c.front(); }
    reference back() { return c.back(); }
    const_reference back() const { return c.back(); }

    
    #ifdef NTL__CXX_RV
    void push(value_type&& x) { c.push_back(std::move(x)); }
    #endif
    #ifdef NTL__CXX_VT
    template <class... Args>
    void emplace(Args&&... args);
    #endif

    void push(const value_type& x) { c.push_back(x); }
    void pop() { c.pop_front(); }
    
    #ifdef NTL__CXX_RV
    void swap(queue&& q) { c.swap(q.c); }
    #else
    void swap(queue& q)  { c.swap(q.c); }
    #endif

  protected:
    Container c;
  };

  template <class T, class Container>
  bool operator==(const queue<T, Container>& x, const queue<T, Container>& y);
  template <class T, class Container>
  bool operator< (const queue<T, Container>& x, const queue<T, Container>& y);
  template <class T, class Container>
  bool operator!=(const queue<T, Container>& x, const queue<T, Container>& y);
  template <class T, class Container>
  bool operator> (const queue<T, Container>& x, const queue<T, Container>& y);
  template <class T, class Container>
  bool operator>=(const queue<T, Container>& x, const queue<T, Container>& y);
  template <class T, class Container>
  bool operator<=(const queue<T, Container>& x, const queue<T, Container>& y);

  template <class T, class Container>
  void swap(queue<T, Container>& x, queue<T, Container>& y);
  
  #ifdef NTL__CXX_RV
  template <class T, class Container>
  void swap(queue<T, Container>&& x, queue<T, Container>& y);
  template <class T, class Container>
  void swap(queue<T, Container>& x, queue<T, Container>&& y);
  #endif
  
  
  template <class T, class Container, class Alloc>
  struct uses_allocator<queue<T, Container>, Alloc>
    : uses_allocator<Container, Alloc>::type { };

  template <class T, class Container>
  struct constructible_with_allocator_suffix<queue<T, Container> >
    : true_type { };

  /**@} lib_container_adaptors */
  /**@} lib_sequence */
  /**@} lib_containers */
}//namespace std

#endif //#ifndef NTL__STLX_QUEUE
