/**\file*********************************************************************
 *                                                                     \brief
 *  IP Name resolver base classes
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_NETWORK_RESOLVERBASE
#define NTL__STLX_TR2_NETWORK_RESOLVERBASE
#pragma once

#include "ip_address.hxx"
#include "system_network.hxx"

#include "../../../iterator.hxx"
#include "../../../vector.hxx"

namespace std { namespace tr2 { namespace network {

  namespace ip
  {
    template<class InternetProtocol>
    class resolver_service;

    /**
     *	@brief 5.9.6. Class ip::resolver_query_base
     **/
    class resolver_query_base
    {
      typedef ntl::network::addrinfo addrinfo;
    public:
      /** resolver bit flags */
      enum flags {
        /** Returned endpoints are intended for use as locally bound socket endpoints.  */
        passive = addrinfo::ai_passive,
        /** Determine the canonical name of the host specified in the query.  */
        canonical_name = addrinfo::ai_canonname,
        /** Host name should be treated as a numeric string defining an IPv4 or IPv6 address and no host name resolution should be attempted. */
        numeric_host = addrinfo::ai_numerichost,
        /** Service name should be treated as a numeric string defining a port number and no service name resolution should be attempted.  */
        numeric_service = addrinfo::ai_numericserv,
        /** If the query protocol is specified as an IPv6 protocol, return IPv4-mapped IPv6 addresses on finding no IPv6 addresses.  */
        v4_mapped = addrinfo::ai_v4mapped,
        /** If used with v4_mapped, return all matching IPv6 and IPv4 addresses.  */
        all_matching = addrinfo::ai_all,
        /** Only return IPv4 addresses if a non-loopback IPv4 address is configured for the system
            or return IPv6 addresses if a non-loopback IPv6 address is configured.  */
        address_configured = addrinfo::ai_addrconfig
      };
      __ntl_bitmask_type(flags, friend);
    protected:
      resolver_query_base()
      {}
      ~resolver_query_base()
      {}
    };



    /**
     *	@brief 5.9.7. Class template ip::basic_resolver_entry
     **/
    template<class InternetProtocol>
    class basic_resolver_entry
    {
    public:
      ///\name types:
      typedef InternetProtocol                    protocol_type;
      typedef typename InternetProtocol::endpoint endpoint_type;

      ///\name constructors:
      basic_resolver_entry()
      {}
      basic_resolver_entry(const endpoint_type& ep, const string& h, const string& s)
        :p(ep), h(h), s(s)
      {}

      ///\name members:
      endpoint_type endpoint() const  { return p; }
      operator endpoint_type() const  { return p; }
      
      string host_name()    const     { return h; }
      string service_name() const     { return s; }
      ///\}
    private:
      endpoint_type p;
      string h, s;
    };



    /**
     *	@brief 5.9.8. Class template ip::basic_resolver_iterator
     **/
    template<class InternetProtocol>
    class basic_resolver_iterator:
      public iterator<forward_iterator_tag, const basic_resolver_entry<InternetProtocol> >
    {
    public:
      ///\name types:
      typedef InternetProtocol protocol_type;
      typedef typename InternetProtocol::endpoint endpoint_type;

      ///\name constructors:
      basic_resolver_iterator()
        :response()
      {}

      ///\name forward iterator members
      reference operator*() const
      {
        if(!result){
          // silent dereference support
          static value_type* empty = std::__::static_storage<value_type>::get_object();
          return *empty;
        }
        return (*result)[response];
      }
      pointer operator->() const
      {
        return &operator*();
      }

      basic_resolver_iterator& operator++()
      {
        if(++response == result->size()){
          result.reset();
          response = 0;
        }
        return *this;
      }
      basic_resolver_iterator  operator++(int)
      {
        basic_resolver_iterator tmp(*this);
        ++*this;
        return tmp;
      }

      friend inline bool operator==(const basic_resolver_iterator<InternetProtocol>& a, const basic_resolver_iterator<InternetProtocol>& b)
      {
        return a.response == b.response && a.result == b.result;
      }
      friend inline bool operator!=(const basic_resolver_iterator<InternetProtocol>& a, const basic_resolver_iterator<InternetProtocol>& b)
      {
        return rel_ops::operator!=(a,b);
      }
      ///\}

    protected:
      friend class resolver_service<InternetProtocol>;
      typedef shared_ptr<vector<value_type> > response_t;
      basic_resolver_iterator(const response_t& res)
        :result(res), response(0)
      {}
    private:
      response_t result;
      size_t response;
    };




    /**
     *	@brief 5.9.9. Class template ip::basic_resolver_query
     *
     *  @note Without \c host_name <tt>basic_resolver::resolve("0")</tt> returns "[::]:0",
     *  but with empty \c host_name <tt>basic_resolver::resolve("", "0")</tt> returns endpoints from all network interfaces ("eth0:0", "eth1:0", etc.)
     **/
    template<class InternetProtocol>
    class basic_resolver_query:
      public resolver_query_base
    {
    public:
      ///\name types:
      typedef InternetProtocol protocol_type;

      ///\name constructors:
      basic_resolver_query()
        :hint(), host_exists(false)
      {}
      basic_resolver_query(const string_ref& service_name, flags f = passive | address_configured)
        :hint(), service(service_name), host_exists(false)
      {
        const typename protocol_type::endpoint p;
        set_proto(p.protocol());
        hint.flags = f;
        hint.family = ntl::network::constants::af_unspec;
      }
      basic_resolver_query(const InternetProtocol& proto, const string_ref& service_name, flags f = passive | address_configured)
        :hint(), service(service_name), host_exists(false)
      {
        set_proto(proto);
        hint.flags = f;
      }
      basic_resolver_query(const string_ref& host_name,  const string_ref& service_name, flags f = address_configured)
        :hint(), host(host_name), service(service_name), host_exists(true)
      {
        const typename protocol_type::endpoint p;
        set_proto(p.protocol());
        hint.flags = f;
        hint.family = ntl::network::constants::af_unspec;
      }
      basic_resolver_query(const InternetProtocol& proto, const string_ref& host_name, const string_ref& service_name, flags f = address_configured)
        :hint(), host(host_name), service(service_name), host_exists(true)
      {
        set_proto(proto);
        hint.flags = f;
      }
      ///\}
    protected:
      void set_proto(const protocol_type& proto)
      {
        hint.family = proto.family();
        hint.socktype = proto.type();
        hint.protocol = proto.protocol();
      }
    public:
      string host, service;
      ntl::network::addrinfo hint;
      bool host_exists;
    };

  } // ip
}}}
#endif // NTL__STLX_TR2_NETWORK_RESOLVERBASE
