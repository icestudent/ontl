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

/**\addtogroup  lib_containers ********* 23 Containers library [containers]
 *@{*/

/**\addtogroup  lib_sequence *********** 23.2 Sequence containers [sequences]
 *@{*/

/**\addtogroup lib_container_adaptors  23.2.5 Container adaptors [container.adaptors]
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
    explicit queue(const Container& c)
      :c(c)
    {}
    
    #ifdef NTL__CXX_RV
    explicit queue(Container&& c = Container())
      :c(move(c))
    {}
    queue(queue&& q)
      :c(move(q.c))
    {}
    #endif

    template <class Alloc>
    explicit queue(const Alloc& a)
      :c(a)
    {}

    template <class Alloc>
    queue(const Container& c, const Alloc& a)
      :c(c, a)
    {}
    
    #ifdef NTL__CXX_RV
    template <class Alloc>
    queue(Container&& c, const Alloc& a)
      :c(move(c),a)
    {}
    template <class Alloc>
    queue(queue&& c, const Alloc& a)
      :c(move(c.c),a)
    {}
    #endif

    
    #ifdef NTL__CXX_RV
    queue& operator=(queue&& q) { c = move(q.c); return *this; }
    #else
    queue& operator=(queue& q) { c.swap(q.c); return *this; }
    #endif

    bool empty() const { return c.empty(); }
    size_type size() const { return c.size(); }
    reference front() { return c.front(); }
    reference back() { return c.back(); }
    const_reference front() const { return c.front(); }
    const_reference back() const { return c.back(); }

    
    #ifdef NTL__CXX_RV
    void push(value_type&& x) { c.push_back(move(x)); }
    #endif

    #ifdef NTL__CXX_VT
    template <class... Args>
    void emplace(Args&&... args) { c.emplace_back(forward<Args>(args)...); }
    #endif

    void push(const value_type& x) { c.push_back(x); }
    void pop() { c.pop_front(); }
    
    #ifdef NTL__CXX_RV
    void swap(queue&& q) { c.swap(q.c); }
    #else
    void swap(queue& q)  { c.swap(q.c); }
    #endif

    friend inline bool operator==(const queue<T, Container>& x, const queue<T, Container>& y) { return x.c == y.c; }
    friend inline bool operator< (const queue<T, Container>& x, const queue<T, Container>& y) { return x.c < y.c; }

  protected:
    Container c;
  };

  template <class T, class Container>
  inline bool operator!=(const queue<T, Container>& x, const queue<T, Container>& y)
  {
    return rel_ops::operator !=(x, y);
  }

  template <class T, class Container>
  inline bool operator> (const queue<T, Container>& x, const queue<T, Container>& y)
  {
    return rel_ops::operator >(x, y);
  }

  template <class T, class Container>
  inline bool operator>=(const queue<T, Container>& x, const queue<T, Container>& y)
  {
    return rel_ops::operator >=(x, y);
  }

  template <class T, class Container>
  inline bool operator<=(const queue<T, Container>& x, const queue<T, Container>& y)
  {
    return rel_ops::operator <=(x, y);
  }

  template <class T, class Container>
  inline void swap(queue<T, Container>& x, queue<T, Container>& y)  { x.swap(y); }
  
  #ifdef NTL__CXX_RV
  template <class T, class Container>
  inline void swap(queue<T, Container>&& x, queue<T, Container>& y) { x.swap(y); }
  template <class T, class Container>
  inline void swap(queue<T, Container>& x, queue<T, Container>&& y) { x.swap(y); }
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
