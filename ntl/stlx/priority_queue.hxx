/**\file*********************************************************************
 *                                                                     \brief
 *  Class template priority_queue [23.2.5.2 priority.queue]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_PRIORITYQUEUE
#define NTL__STLX_PRIORITYQUEUE

#include "vector.hxx"
#include "functional.hxx"

namespace std {

/**\addtogroup  lib_containers ********* Containers library [23] ************
 *@{*/

/**\addtogroup  lib_sequence *********** Sequences [23.2] *******************
 *@{*/

/**\addtogroup  lib_container_adaptors *** Container adaptors [23.2.5]*********
 *@{*/

  /// Class template priority_queue [23.2.5.2 priority.queue]
  template <class T, class Container = vector<T>,
    class Compare = less<typename Container::value_type> >
  class priority_queue 
  {
  public:
    typedef Container                           container_type;
    typedef typename Container::value_type      value_type;
    typedef typename Container::reference       reference;
    typedef typename Container::const_reference const_reference;
    typedef typename Container::size_type       size_type;
  public:
    
    #ifdef NTL__CXX
    priority_queue(const Compare& x, const Container&);
    explicit priority_queue(const Compare& x = Compare(), Container&& = Container());
    template <class InputIterator>
    priority_queue(InputIterator first, InputIterator last, const Compare& x, const Container&);
    template <class InputIterator>
    priority_queue(InputIterator first, InputIterator last, const Compare& x = Compare(), Container&& = Container());
    priority_queue(priority_queue&&);
    priority_queue& operator=(priority_queue&&);
    #else
    explicit priority_queue(const Compare& x = Compare(), const Container& c = Container());
    template <class InputIterator>
    priority_queue(InputIterator first, InputIterator last, const Compare& x = Compare(), const Container& c = Container());
    priority_queue(const priority_queue&);
    priority_queue& operator= (const priority_queue&);
    #endif

    template <class Alloc>
    explicit priority_queue(const Alloc&);
    template <class Alloc>
    priority_queue(const Compare&, const Alloc&);
    template <class Alloc>
    priority_queue(const Compare&, const Container&, const Alloc&);
    
    #ifdef NTL__CXX
    template <class Alloc>
    priority_queue(const Compare&, Container&&, const Alloc&);
    template <class Alloc>
    priority_queue(priority_queue&&, const Alloc&);
    #endif

    bool empty() const { return c.empty(); }
    size_type size() const { return c.size(); }
    
    const_reference top() const { return c.front(); }
    void push(const value_type& x);
    void pop();
    
    #ifdef NTL__CXX
    void push(value_type&& x);
    template <class... Args>
    void emplace(Args&&... args);
    #endif

    
    #ifdef NTL__CXX
    void swap(priority_queue&&);
    #else
    void swap(priority_queue&);
    #endif

  protected:
    Container c;
    Compare comp;
  };

  // no equality is provided
  template <class T, class Container, class Compare>
  void swap(priority_queue<T, Container, Compare>& x, priority_queue<T, Container, Compare>& y);
  
  #ifdef NTL__CXX
  template <class T, class Container, class Compare>
  void swap(priority_queue<T, Container, Compare>&& x, priority_queue<T, Container, Compare>& y);
  template <class T, class Container, class Compare>
  void swap(priority_queue<T, Container, Compare>& x, priority_queue<T, Container, Compare>&& y);
  #endif
  
  template <class T, class Container, class Compare, class Alloc>
  struct uses_allocator<priority_queue<T, Container, Compare>, Alloc>
    : uses_allocator<Container, Alloc>::type { };
  
  template <class T, class Container, class Compare>
  struct constructible_with_allocator_suffix<
    priority_queue<T, Container, Compare> >
    : true_type { };

  /**@} lib_container_adaptors */
  /**@} lib_sequence */
  /**@} lib_containers */
}//namespace std

#endif //#ifndef NTL__STLX_PRIORITYQUEUE
