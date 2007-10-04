/**\file*********************************************************************
 *                                                                     \brief
 *  Class template stack [23.2.3.3 lib.stack]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_STACK
#define NTL__STLX_STACK

#if 0//def - what macro???
#include "deque.hxx"
#define STLX__STACK_CONTAINER deque<T>  // standard behavior
#else
#include "vector.hxx"
#define STLX__STACK_CONTAINER vector<T> // faster
#endif

namespace std {

/**\addtogroup  lib_containers ********* Containers library [23] ************
 *@{*/

/**\addtogroup  lib_sequence *********** Sequences [23.2] *******************
 *@{*/

/**\defgroup  lib_container_adaptors *** Container adaptors [23.2.3]*********
 *@{*/

/// Class template stack 23.2.3.3
template <class T, class Container = STLX__STACK_CONTAINER>
class stack
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef typename Container::value_type  value_type;
    typedef typename Container::size_type   size_type;
    typedef          Container              container_type;

    explicit stack(const Container& c = Container()) : c(c) {}

    bool              empty() const             { return c.empty(); }
    size_type         size()  const             { return c.size(); }
    value_type&       top()                     { return c.back(); }
    const value_type& top()   const             { return c.back(); }
    void              push(const value_type& x) { c.push_back(x); }
    void              pop()                     { c.pop_back(); }

  ///////////////////////////////////////////////////////////////////////////
  protected:

    Container c;

  ///////////////////////////////////////////////////////////////////////////

  ///\name  Stack operators
  ///@{

  friend
    bool operator==(const stack<T, Container>& x, const stack<T, Container>& y)
      { return x.c == y.c; }

  friend
    bool operator< (const stack<T, Container>& x, const stack<T, Container>& y)
      { return x.c < y.c; }

  friend
    bool operator!=(const stack<T, Container>& x, const stack<T, Container>& y)
      { return x.c != y.c }

  friend
    bool operator> (const stack<T, Container>& x, const stack<T, Container>& y)
      { return x.c > y.c; }

  friend
    bool operator>=(const stack<T, Container>& x, const stack<T, Container>& y)
      { return x.c >= y.c; }

  friend
    bool operator<=(const stack<T, Container>& x, const stack<T, Container>& y)
      { return x.c <= y.c; }

  ///@}

};//class stack

/**@} lib_container_adaptors */

/**@} lib_sequence */

/**@} lib_containers */

}//namespace std

#endif//#ifndef NTL__STLX_STACK
