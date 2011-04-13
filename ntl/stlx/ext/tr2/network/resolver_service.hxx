/**\file*********************************************************************
 *                                                                     \brief
 *  IP name resolver service
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_NETWORK_RESOLVERSERVICE
#define NTL__STLX_TR2_NETWORK_RESOLVERSERVICE
#pragma once

#include "resolver_base.hxx"
#include "io_service.hxx"
#include "winsock/service_resolve.hxx"

namespace std { namespace tr2 { namespace network {

  namespace ip
  {
    /**
     *	@brief 5.9.10. Class template ip::resolver_service
     **/
     template<class InternetProtocol>
     class resolver_service:
       public tr2::sys::io_service::service
     {
       typedef ntl::network::winsock::resolver_service<InternetProtocol> service_implementation_type;
     public:
       static tr2::sys::io_service::id id;

       ///\name types:
       typedef InternetProtocol protocol_type;
       typedef typename InternetProtocol::endpoint endpoint_type;
       typedef basic_resolver_query<InternetProtocol> query_type;
       typedef basic_resolver_iterator<InternetProtocol> iterator_type;
       typedef typename service_implementation_type::implementation_type implementation_type;
       
       ///\name constructors:
       explicit resolver_service(tr2::sys::io_service& ios)
         :service(ios), svc(ios)
       {}

       ///\name members:
       void construct(implementation_type& impl){ return svc.construct(impl); }
       void destroy(implementation_type& impl){ return svc.destroy(impl); }

       error_code cancel(implementation_type& impl, error_code& ec){ return svc.cancel(impl, ec); }
       
       iterator_type resolve(implementation_type& impl, const query_type& q, error_code& ec) { return iterator_type(svc.resolve(impl, q, ec)); }
       iterator_type resolve(implementation_type& impl, const endpoint_type& e, error_code& ec) { return iterator_type(svc.resolve(impl, e, ec)); }

       template<class ResolveHandler>
       void async_resolve(implementation_type& impl, const query_type& q, ResolveHandler handler) { return svc.async_resolve(impl, q, handler); }
       template<class ResolveHandler>
       void async_resolve(implementation_type& impl, const endpoint_type& e, ResolveHandler handler) { return svc.async_resolve(impl, e, handler); }
       ///\}
     private:
       void shutdown_service() { svc.shutdown_service(); }

     private:
       service_implementation_type svc;
     };

  } // ip
}}}
#endif // NTL__STLX_TR2_NETWORK_RESOLVERSERVICE
