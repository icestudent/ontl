/**\file*********************************************************************
 *                                                                     \brief
 *  Basic I/O Services
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_IOSERVICE_FWD
#define NTL__STLX_TR2_IOSERVICE_FWD
#pragma once

#include "../../../atomic.hxx"
#include "network_error.hxx"

namespace std { namespace tr2 {

  /// System namespace.
  namespace sys
  {

    class io_service;
    class invalid_service_owner;
    class service_already_exists;

    template<class Service> Service& use_service(io_service&);
    template<class Service> void add_service(io_service&, Service*);
    template<class Service> bool has_service(io_service&);

    void* io_handler_allocate(size_t s, ...);
    void io_handler_deallocate(void* p, size_t s, ...);
    template<class F> void io_handler_invoke(F f, ...);

    template<class IoObjectService>
    class basic_io_object;

  } // sys
}}
#endif // NTL__STLX_TR2_IOSERVICE_FWD
