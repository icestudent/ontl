#pragma once

#include "sockets.hxx"
#include "../io_service.hxx"
#include "../resolver_base.hxx"
#include "dns_ops.hxx"

namespace ntl { namespace network {
  namespace error = std::tr2::network::network_error;
  namespace stdnet = std::tr2::network;
  namespace ios = std::tr2::sys;
  namespace winsock
  {

    class resolver_service_base:
      public winsock_service_base
    {
    public:
      struct implementation_type
      {
        functions_t* funcs;
      };

    public:
      void construct(implementation_type& impl)
      {
        impl.funcs = winsock_runtime::instance();
      }
      void destroy(implementation_type& /*impl*/)
      {}
    };

    template<class InternetProtocol>
    class resolver_service:
      public resolver_service_base
    {
      typedef resolver_service_base daddy;

    public:
      ios::iocp::iocp_service& iocp;
      typedef InternetProtocol protocol_type;
      typedef typename InternetProtocol::endpoint endpoint_type;
      typedef stdnet::ip::basic_resolver_query<InternetProtocol> query_type;
      typedef stdnet::ip::basic_resolver_iterator<InternetProtocol> iterator_type;

      typedef typename iterator_type::value_type entry_t;
      typedef std::vector<entry_t> entries_t;
      typedef std::shared_ptr<entries_t> resolve_result;

    public:
      resolver_service(ios::io_service& svc)
        : iocp(ios::use_service<ios::iocp::iocp_service>(svc))
      {}

      std::error_code cancel(implementation_type& impl, std::error_code& ec);

      resolve_result resolve(implementation_type& impl, const query_type& q, std::error_code& ec)
      {
        resolve_result entries;
        addrinfo* response;
        {
          const int err = impl.funcs->getaddrinfo(q.host_exists ? q.host.c_str() : nullptr, q.service.c_str(), &q.hint, &response);
          if(err != 0){
            make_error(ec);
            return std::move(entries);
          }
        }

        const InternetProtocol proto = endpoint_type().protocol();
        const int query_af = proto.family(), 
          query_st = proto.type(),
          query_proto = proto.protocol();

        size_t count = 0;
        for(const addrinfo* ai = response; ai != 0; count++, ai = ai->next);

        entries.reset(new entries_t());
        entries->reserve(count);

        bool length_error = false;
        for(const addrinfo* ai = response; ai != 0; ai = ai->next){
          endpoint_type ep;
          assert(ep.capacity() >= ai->addrlen);
          __ntl_try {
            ep.resize(ai->addrlen);
          }
          __ntl_catch(const std::length_error&){
            length_error = true;
            continue;
          }
          std::memcpy(ep.data(), ai->addr, ai->addrlen);
          //ep.port(ai->addr->port);

          entries->push_back(entry_t(ep, ai->canonname ? ai->canonname : q.host, q.service));
        }
        impl.funcs->freeaddrinfo(response);

        if(entries->empty())
          ec = error::make_error_code(error::address_family_not_supported);
        return std::move(entries);
      }

      resolve_result resolve(implementation_type& impl, const endpoint_type& ep, std::error_code& ec)
      {
        // http://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.xhtml
        resolve_result entries;
        char svc[64], host[1025];
        const int err = impl.funcs->getnameinfo(ep.data(), static_cast<int>(ep.size()), host, _countof(host), svc, _countof(svc), 0);
        if(err != 0) {
          make_error(ec);
          return std::move(entries);
        }

        entries.reset(new entries_t());
        entries->push_back(entry_t(ep, host, svc));
        return std::move(entries);
      }

      template<class ResolveHandler>
      void async_resolve(implementation_type& impl, const query_type& q, ResolveHandler handler)
      {
        typedef ios::__::resolve_operation<ResolveHandler, resolver_service<InternetProtocol>, query_type> op;
        typename op::ptr p (handler, this, &impl, q);

        iocp.work_started();
        iocp.post_immediate_completion(p.op);

        //op::ptr::type* op_self = static_cast<op::ptr::type*>(p.op);
        //addrinfoex_a hint = {};
        //std::memcpy(&hint, &q.hint, sizeof(int)* 4);
        //int re = impl.funcs->async.getaddrinfoA(q.host_exists ? q.host.c_str() : nullptr, q.service.c_str(), addrinfoex_a::ns_all, nullptr, 
        //  &hint, &op_self->response, nullptr, p.op, nullptr, nullptr);
        //if(re == 0)
        //  iocp.on_pending(p.op);
        //else
        //  iocp.on_completion(p.op, make_error(static_cast<sockerror::type>(re)));
        
        p.release();
      }

      template<class ResolveHandler>
      void async_resolve(implementation_type& impl, const endpoint_type& e, ResolveHandler handler)
      {
        typedef ios::__::resolve_operation<ResolveHandler, resolver_service<InternetProtocol>, endpoint_type> op;
        typename op::ptr p (handler, this, &impl, e);

        iocp.work_started();
        iocp.post_immediate_completion(p.op);

        p.release();
      }
    };

  }
}}
