/**\file*********************************************************************
 *                                                                     \brief
 *  Network
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_NETWORK_BASICRESOLVER
#define NTL__STLX_TR2_NETWORK_BASICRESOLVER
#pragma once

#include "resolver_base.hxx"
#include "resolver_service.hxx"

namespace std { namespace tr2 { namespace network {

  namespace ip
  {
    /**
     *	@brief 5.9.11. Class template ip::basic_resolver
     **/
    template<class InternetProtocol, class ResolverService>
    class basic_resolver:
      public tr2::sys::basic_io_object<ResolverService>,
      public socket_base
    {
    public:
      ///\name types:
      typedef InternetProtocol protocol_type;
      typedef typename InternetProtocol::endpoint endpoint_type;
      typedef basic_resolver_query<InternetProtocol> query;
      typedef basic_resolver_iterator<InternetProtocol> iterator;

      ///\name constructors:
      explicit basic_resolver(tr2::sys::io_service& ios)
        :basic_io_object(ios)
      {}

      ///\name members:
      error_code cancel(error_code& ec = throws())
      {
        error_code e;
        service.cancel(impl, e);
        return throw_system_error(e, ec);
      }
      iterator resolve(const query& q, error_code& ec = throws())
      {
        error_code e;
        iterator re = service.resolve(impl, q, e);
        throw_system_error(e, ec);
        return std::move(re);
      }
      iterator resolve(const endpoint_type& ep, error_code& ec = throws())
      {
        error_code e;
        iterator re = service.resolve(impl, ep, e);
        throw_system_error(e, ec);
        return std::move(re);
      }

      template <class ResolveHandler>
      void async_resolve(const query& q, ResolveHandler handler)
      {
        service.async_resolve(impl, q, handler);
      }
      template <class ResolveHandler>
      void async_resolve(const endpoint_type& e, ResolveHandler handler)
      {
        service.async_resolve(e, handler);
      }
    };


    /**
     *	@brief 5.9.12. Host name functions
     *
     *  The standard host name for the current machine, determined as if by POSIX gethostname().
     **/
    inline string host_name(error_code& ec = throws())
    {
      string host;
      error_code e;
      ntl::network::winsock::socket_service_base::host_name(host, ec);
      throw_system_error(e, ec);
      return std::move(host);
    }

  }
}}}
#endif // NTL__STLX_TR2_NETWORK_BASICRESOLVER
