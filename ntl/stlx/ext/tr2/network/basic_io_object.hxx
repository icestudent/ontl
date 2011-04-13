/**\file*********************************************************************
 *                                                                     \brief
 *  Basic I/O Service object
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_BASIC_IO_OBJECT
#define NTL__STLX_TR2_BASIC_IO_OBJECT
#pragma once

#include "io_service_fwd.hxx"

namespace std { namespace tr2 { namespace sys {

  /**
   *	@brief 5.3.9. Class template basic_io_object
   *  @details Base class for I/O objects
   **/
  template<class IoObjectService>
  class basic_io_object:
    noncopyable
  {
  public:
    typedef IoObjectService             service_type;
    typedef typename 
      service_type::implementation_type implementation_type;

  public:
    tr2::sys::io_service& get_io_service() { return service.get_io_service(); }

  protected:
    explicit basic_io_object(tr2::sys::io_service& ios)
      :service(tr2::sys::use_service<service_type>(ios))
    {
      service.construct(impl);
    }
    ~basic_io_object()
    {
      service.destroy(impl);
    }

  protected:
    service_type& service;
    implementation_type impl;

  private:
    basic_io_object(const basic_io_object&) __deleted;
    void operator=(const basic_io_object&)  __deleted;
  };


} // sys
} // tr2
} // std
#endif // NTL__STLX_TR2_BASIC_IO_OBJECT
