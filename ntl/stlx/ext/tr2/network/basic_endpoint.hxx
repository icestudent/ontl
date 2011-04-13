/**\file*********************************************************************
 *                                                                     \brief
 *  IP address endpoint
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_NETWORK_BASICENDPOINT
#define NTL__STLX_TR2_NETWORK_BASICENDPOINT
#pragma once

#include "ip_address.hxx"

namespace std { namespace tr2 {

  /// TR2 Network library
  namespace network {

  /// Internet protocol.
  namespace ip
  {

    /**
     *	@brief 5.9.5. Class template basic_endpoint
     **/
    template<class InternetProtocol>
    class basic_endpoint
    {
      typedef ntl::network::sockaddr addr_type;
    public:
      ///\name types:
      typedef InternetProtocol protocol_type;

      ///\name constructors:
      basic_endpoint()
      {
        memset(&storage, 0, sizeof(storage));
        sa.family = af4;
      }
      basic_endpoint(const InternetProtocol& proto, unsigned short port_num)
      {
        memset(&storage, 0, sizeof(storage));
        sa.family = proto.family();
        sa.port = ntl::big_endian(port_num);
      }
      basic_endpoint(const ip::address& addr, unsigned short port_num)
      {
        memset(&storage, 0, sizeof(storage));
        sa.port = ntl::big_endian(port_num);
        address(addr);
      }

      ///\name members:
      InternetProtocol protocol() const
      {
        return sa.family == af4 ? protocol_type::v4() : protocol_type::v6();
      }
      
      ip::address address() const
      {
        if(sa.family == af4){
          address_v4::bytes_type bytes;
          memcpy(&bytes, v4.addr.bytes, bytes.size());
          const address_v4 addr(bytes);
          return ip::address(addr);
        }else{
          address_v6::bytes_type bytes;
          memcpy(&bytes, v6.addr.bytes, bytes.size());
          const address_v6 addr(bytes);
          return ip::address(addr);
        }
      }

      void address(const ip::address& addr)
      {
        // explicit clear storage
        const uint16_t portnum = sa.port;
        memset(&storage, 0, sizeof(storage));
        sa.port = portnum;
        if(addr.is_v4()){
          const address_v4::bytes_type bytes = addr.to_v4().to_bytes();
          memcpy(v4.addr.bytes, &bytes, bytes.size());
          sa.family = af4;
        }else{
          const address_v6 v6a = addr.to_v6();
          const address_v6::bytes_type bytes = v6a.to_bytes();
          memcpy(v6.addr.bytes, &bytes, bytes.size());
          v6.scope_id = v6a.scope_id();
          sa.family = af6;
        }
      }

      unsigned short port() const { return ntl::big_endian(sa.port); }
      void port(unsigned short port_num) { sa.port = ntl::big_endian(port_num); }

      string to_string(error_code& ec = throws()) const
      {
        error_code e;
        string s = address().to_string(e);
        if(!e){
          s.reserve(s.size() + 2 + 1 + 5);
          if(sa.family == af6){
            s.insert(string::size_type(0), 1, '[');
            s.append(1, ']');
          }
          s.append(1, ':');
          char pb[16];
          ntl::numeric::itoa(ntl::big_endian(sa.port), pb);
          s.append(pb);
        }
        throw_system_error(e, ec);
        return std::move(s);
      }

      static basic_endpoint from_string(const string& str, error_code& ec = throws())
      {
        // [addr6]:port or addr4:port
        if(&ec != &throws())
          ec.clear();
        basic_endpoint empty;
        if(str.empty())
          return empty;
        const error_code inv_code = std::make_error_code(std::tr2::network::error::invalid_argument);

        string::size_type pos = str.rfind(':');
        if(pos == string::npos || pos < 4){   // [::]:1
          // no port
          throw_system_error(inv_code, ec);
          return empty;
        }

        // make copy for modifications
        string str2 = str;
        const char *p = str2.c_str()+pos+1, *dst;
        const uint32_t portnum = ntl::numeric::strtoul(p, &dst, 10);
        if(dst == p || portnum > std::numeric_limits<uint16_t>::max()){
          // invalid port
          throw_system_error(inv_code, ec);
          return empty;
        }
        str2.resize(pos);
        bool v4 = true;
        if(str2[0] == '[' && str2[str2.length()-1] == ']'){
          str2.erase(0, 1);
          str2.erase(str2.length()-1);
          v4 = false;
        }
        error_code e;
        if(v4){
          address_v4 addr = address_v4::from_string(str2, e);
          if(!e)
            empty.address(ip::address(addr));
        }else{
          address_v6 addr = address_v6::from_string(str2, e);
          if(!e)
            empty.address(ip::address(addr));
        }
        if(!e)
          empty.port(static_cast<uint16_t>(portnum));
        else
          throw_system_error(e, ec);
        return empty;
      }

      ///\name extensible
      addr_type*        data()            { return &sa; }
      const addr_type*  data()      const { return &sa; }
      size_t            size()      const { return sa.family == af4 ? sizeof(v4) : sizeof(v6); }
      size_t            capacity()  const { return sizeof(storage); }
      void resize(size_t s) __ntl_throws(std::length_error)
      {
        if(s == sizeof(v4))
          sa.family = af4;
        else if(s == sizeof(v6))
          sa.family = af6;
        else
          __ntl_throw(std::length_error("unsupported endpoint size"));
      }
      ///\}

    private:
      static const ntl::network::constants::family_type 
        af4 = ntl::network::constants::af_inet,
        af6 = ntl::network::constants::af_inet6;

      union {
        ntl::network::sockaddr_storage storage;
        ntl::network::sockaddr sa;
        ntl::network::sockaddr_in v4;
        ntl::network::sockaddr_in6 v6;
      };
    };

    ///\name basic_endpoint comparisons:
    template<class InternetProtocol>
    inline bool operator==(const basic_endpoint<InternetProtocol>& a, const basic_endpoint<InternetProtocol>& b) { return memcmp(a.data(), b.data(), a.capacity()) == 0; }
    template<class InternetProtocol>
    inline bool operator!=(const basic_endpoint<InternetProtocol>& a, const basic_endpoint<InternetProtocol>& b) { return rel_ops::operator!=(a, b); }
    template<class InternetProtocol>
    inline bool operator< (const basic_endpoint<InternetProtocol>& a, const basic_endpoint<InternetProtocol>& b)
    {
      return a.address() < b.address() || ((!b.address() < a.address()) && a.port() < b.port());
    }
    template<class InternetProtocol>
    inline bool operator> (const basic_endpoint<InternetProtocol>& a, const basic_endpoint<InternetProtocol>& b) { return rel_ops::operator> (a, b); }
    template<class InternetProtocol>
    inline bool operator<=(const basic_endpoint<InternetProtocol>& a, const basic_endpoint<InternetProtocol>& b) { return rel_ops::operator<=(a, b); }
    template<class InternetProtocol>
    inline bool operator>=(const basic_endpoint<InternetProtocol>& a, const basic_endpoint<InternetProtocol>& b) { return rel_ops::operator?=(a, b); }


    ///\name basic_endpoint I/O:
    template<class CharT, class Traits, class InternetProtocol>
    inline basic_ostream<CharT, Traits>& operator<<(basic_ostream<CharT, Traits>& os, const basic_endpoint<InternetProtocol>& ep)
    {
      error_code ec;
      string s = ep.to_string(ec);
      if(ec)
        os.setstate(ios_base::failbit);
      else
        os << s;
      return os;
    }
    ///\}

  }
}}}
#endif // NTL__STLX_TR2_NETWORK_BASICENDPOINT
