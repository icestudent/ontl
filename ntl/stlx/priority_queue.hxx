/**\file*********************************************************************
 *                                                                     \brief
 *  Class template priority_queue [23.2.5.2 priority.queue]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_PRIORITYQUEUE
#define NTL__STLX_PRIORITYQUEUE
#pragma once

#include "vector.hxx"
#include "functional.hxx"
#include "algorithm.hxx"

namespace std {

/**\addtogroup  lib_containers ********* 23 Containers library [containers]
 *@{*/

/**\addtogroup  lib_sequence *********** 23.2 Sequence containers [sequences]
 *@{*/

/**\addtogroup lib_container_adaptors  23.2.5 Container adaptors [container.adaptors]
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
  #ifdef NTL_CXX_RV
    explicit priority_queue(const Compare& x, const Container& c);
    explicit priority_queue(const Compare& x, Container&& = Container());
    
    template <class InputIterator>
    priority_queue(InputIterator first, InputIterator last, const Compare& x, Container&& = Container());

    template <class InputIterator>
    priority_queue(InputIterator first, InputIterator last, const Compare& x, const Container& c);

    priority_queue(priority_queue&& r)
      : c(move(r.c))
      , comp(move(r.comp))
    {}

    priority_queue& operator=(priority_queue&& r)
    {
      c = move(r.c);
      comp = move(comp);
      return *this;
    }

  #else
    explicit priority_queue(const Compare& x, const Container& c = Container());


    priority_queue(const priority_queue& r)
      : c(r.c)
      , comp(r.comp)
    {}

    priority_queue& operator= (const priority_queue& r)
    {
      c = r.c;
      comp = r.comp;
      return *this;
    }
  #endif

    explicit priority_queue()
      : c()
      , comp()
    {}

    template <class InputIterator>
    priority_queue(InputIterator first, InputIterator last)
      : c()
      , comp()
    {
      c.insert(c.end(), first, last);
      make_heap(c.begin(), c.end(), comp);
    }

    template <class Alloc>
    explicit priority_queue(const Alloc&);

    template <class Alloc>
    priority_queue(const Compare&, const Alloc&);

    template <class Alloc>
    priority_queue(const Compare&, const Container&, const Alloc&);
    

  #ifdef NTL_CXX_RV
    template <class Alloc>
    priority_queue(const Compare&, Container&&, const Alloc&);
    template <class Alloc>
    priority_queue(priority_queue&&, const Alloc&);
  #endif


    ///\name extensions
    typedef typename Container::iterator iterator;
    typedef typename Container::const_iterator const_iterator;

    iterator begin()        { return c.begin(); }
    iterator end()          { return c.end(); }

    iterator begin()  const { return c.begin(); }
    iterator end()    const { return c.end(); }

    iterator cbegin() const { return c.begin(); }
    iterator cend()   const { return c.end(); }
    


    ///\name comtainer access
    bool empty() const { return c.empty(); }

    size_type size() const { return c.size(); }
    
    const_reference top() const { return c.front(); }


    ///\name modifying members
    void push(const value_type& x)
    {
      c.push_back(x);
      push_heap(c.begin(), c.end(), comp);
    }

    void pop()
    {
      pop_heap(c.begin(), c.end(), comp);
      c.pop_back();
    }
    
  #ifdef NTL_CXX_RV
    void push(value_type&& x)
    {
      c.push_back(std::move(x));
      push_heap(c.begin(), c.end(), comp);
    }
  #endif
  #ifdef NTL_CXX_VT
    template <class... Args>
    void emplace(Args&&... args)
    {
      c.emplace_back(std::forward<Args>(args)...);
      push_heap(c.begin(), c.end(), comp);
    }
  #endif
    ///\}

    void swap(priority_queue& x)
    {
      using std::swap;
      swap(c, x.c);
      swap(comp, x.comp);
    }

  protected:
    Container c;
    Compare comp;
  };

  // no equality is provided
  template <class T, class Container, class Compare>
  inline void swap(priority_queue<T, Container, Compare>& x, priority_queue<T, Container, Compare>& y)
  {
    x.swap(y);
  }
  
  template <class T, class Container, class Compare, class Alloc>
  struct uses_allocator<priority_queue<T, Container, Compare>, Alloc>
    : uses_allocator<Container, Alloc>::type { };
  

  /**@} lib_container_adaptors */
  /**@} lib_sequence */
  /**@} lib_containers */
}//namespace std

#endif //#ifndef NTL__STLX_PRIORITYQUEUE
