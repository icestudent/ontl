/**\file*********************************************************************
 *                                                                     \brief
 *  I/O Service Futures
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_USEFUTURE
#define NTL__STLX_TR2_USEFUTURE
#pragma once

#include "../../../memory.hxx"

namespace std { namespace tr2 { namespace sys {


  template<class Allocator = std::allocator<void>>
  class use_future_t
  {
  public:
    typedef Allocator allocator_type;

  public:
    explicit use_future_t()
    {}
    explicit use_future_t(const Allocator& allocator)
      : alloc(allocator)
    {}

    template<class A>
    use_future_t<A> operator[](const A& allocator) const
    {
      return use_future_t<A>(allocator);
    }

    allocator_type get_allocator() const { return alloc; }

  private:
    allocator_type alloc;
  };


  __declspec(selectany)
    use_future_t<> use_future;

}}}

#endif // NTL__STLX_TR2_USEFUTURE
