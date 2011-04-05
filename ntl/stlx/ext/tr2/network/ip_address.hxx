/**\file*********************************************************************
 *                                                                     \brief
 *  Network
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_NETWORK_IPADDRESS
#define NTL__STLX_TR2_NETWORK_IPADDRESS
#pragma once

#include "network_fwd.hxx"
#include "system_network.hxx"
#include "../../../../stdlib.hxx"
#include "../../../array.hxx"
#include "../../../stdstring.hxx"
#include "../../numeric_conversions.hxx"
#include "network_error.hxx"

namespace std { namespace tr2 { namespace network {

  namespace ip
  {
    /**
     *	@brief 5.9.3. Class ip::address_v4
     **/
    class address_v4
    {
    public:
      ///\name types:
      typedef array<unsigned char, 4> bytes_type;

      ///\name constructors:
      address_v4()
      {
        v.addr = 0;
      }
      explicit address_v4(const bytes_type& bytes)
      {
        v.addr = *reinterpret_cast<const uint32_t*>(bytes.data());
      }
      explicit address_v4(unsigned long val)
      {
        v.addr = ntl::big_endian(val);
      }

      ///\name members:
      bool is_class_a() const   { return (to_ulong() & classA_mask) == classA_val; }
      bool is_class_b() const   { return (to_ulong() & classB_mask) == classB_val; }
      bool is_class_c() const   { return (to_ulong() & classC_mask) == classC_val; }
      bool is_multicast()const  { return (to_ulong() & classD_mask) == classD_val; }
      bool is_loopback() const  { return (to_ulong() & loopback_mask) == loopback_mask; }
      bool is_unspecified()const{ return v.addr == 0; }

      bytes_type to_bytes() const
      {
        bytes_type bytes = {v.b1, v.b2, v.b3, v.b4 };
        return bytes;
      }

      unsigned long to_ulong() const { return ntl::big_endian(v.addr); }

      string to_string(error_code& ec = throws()) const
      {
        // aaa.bbb.ccc.ddd
        char buf[16];
        char* dst = buf;
        bytes_type bytes = to_bytes();
        for(unsigned i = 0; i < 4; i++){
          size_t n;
          ntl::numeric::itoa(bytes[i], dst, 16, 10, &n);
          dst += n;
          *dst++ = '.';
        }
        dst[-1] = 0;
        if(&ec != &throws())
          ec.clear();
        return std::string(buf);
      }

      ///\name static members:
      static address_v4 from_string(const string& str, error_code& ec = throws())
      {
        if(&ec != &throws())
          ec.clear();
        if(str.empty())
          return address_v4();
        bool ok = true;
        bytes_type bytes = {0};
        string::const_iterator p = str.c_str(), end = str.cend(), dst;
        unsigned i;
        for(i = 0; i < 4 && p < end; i++){
          unsigned long v;
          if(*p == '0' && (p+1 >= end || p[1] == '.')){
            v = 0, p += 2;
          } else {
            v = ntl::numeric::strtoul(p, &dst);
            if((dst != end && (dst == p || *dst != '.')) || v > 255){
              ok = false;
              break;
            }
            p = dst+1;
          }
          bytes[i] = static_cast<bytes_type::value_type>(v);
        }
        if(ok == false || i < 4){
          throw_system_error(make_error_code(network_error::invalid_argument), ec);
          return address_v4();
        }
        return address_v4(bytes);
      }

      static address_v4 any()       { return address_v4(); }
      static address_v4 loopback()  { return address_v4(loopback_val1); }
      static address_v4 broadcast() { return address_v4(broadcast_val); }
      static address_v4 broadcast(const address_v4& addr, const address_v4& mask)
      {
        return address_v4(addr.to_ulong() | ~mask.to_ulong());
      }
      static address_v4 netmask(const address_v4& addr)
      {
        uint32_t mask;
        if(addr.is_class_a())
          mask = 0xFF000000;
        else if(addr.is_class_b())
          mask = 0xFFFF0000;
        else if(addr.is_class_c())
          mask = 0xFFFFFF00;
        else
          mask = 0xFFFFFFFF; //-V112
        return address_v4(mask);
      }

      ///\name address comparisons:
      friend inline bool operator==(const address_v4& a, const address_v4& b) { return a.to_ulong() == b.to_ulong(); }
      friend inline bool operator!=(const address_v4& a, const address_v4& b) { return a.to_ulong() != b.to_ulong(); }
      friend inline bool operator< (const address_v4& a, const address_v4& b) { return a.to_ulong() <  b.to_ulong(); }
      friend inline bool operator> (const address_v4& a, const address_v4& b) { return a.to_ulong() >  b.to_ulong(); }
      friend inline bool operator<=(const address_v4& a, const address_v4& b) { return a.to_ulong() <= b.to_ulong(); }
      friend inline bool operator>=(const address_v4& a, const address_v4& b) { return a.to_ulong() >= b.to_ulong(); }
      ///\}
    private:
      static const uint32_t 
        classA_mask = 1U << 31, classA_val = 0,
        classB_mask = 3U << 30, classB_val = classA_mask,
        classC_mask = 7U << 29, classC_val = classB_mask,
        classD_mask = 15U<< 28, classD_val = classC_mask,
        loopback_mask = 0x7F000000, loopback_val1 = 0x7f000001,
        broadcast_val = 0xffffffff; //-V112

      ntl::network::in_addr v;  // in network byte order (big-endian)
    };



    /**
     *	@brief 5.9.4. Class ip::address_v6
     **/
    class address_v6
    {
    public:
      ///\name types:
      typedef array<unsigned char, 16> bytes_type;


      ///\name constructors:
      address_v6()
        :scope(0)
      {
        for(unsigned i = 0; i < addr_size / 2; i++)
          v.words[i] = 0;
      }
      explicit address_v6(const bytes_type& bytes, unsigned long scope_id = 0)
        :scope(scope_id)
      {
        for(unsigned i = 0; i < addr_size; i++)
          v.bytes[i] = bytes[i];
      }

      ///\name members:
      void scope_id(unsigned long id) { scope = id;   }
      unsigned long scope_id() const  { return scope; }

      bool is_unspecified() const
      {
        for(unsigned i = 0; i < addr_size; i++)
          if(v.bytes[i] != 0)
            return false;
        return true;
      }
      bool is_loopback() const
      {
        for(unsigned i = 0; i < addr_size - 1; i++)
          if(v.bytes[i] != 0)
            return false;
        return v.bytes[15] == 1;
      }

      bool is_multicast() const { return v.bytes[0] == 0xFF; }
      bool is_link_local() const{ return v.bytes[0] == 0xFE && (v.bytes[1] & 0xC0) == 0x80; }
      bool is_site_local() const{ return v.bytes[0] == 0xFE && (v.bytes[1] & 0xC0) == 0xC0; }
      bool is_v4_mapped() const
      {
        for(unsigned i = 0; i < 10; i++)
          if(v.bytes[i] != 0)
            return false;
        return v.bytes[10] == 0xFF && v.bytes[11] == 0xFF;
      }
      bool is_v4_compatible() const
      {
        for(unsigned i = 0; i < 12; i++)
          if(v.bytes[i] != 0)
            return false;
        return !is_unspecified() && !is_loopback();
      }
      bool is_multicast_node_local() const { return v.bytes[0] == 0xFF && (v.bytes[1] & 0x0F) == 0x01; }
      bool is_multicast_link_local() const { return v.bytes[0] == 0xFF && (v.bytes[1] & 0x0F) == 0x02; }
      bool is_multicast_site_local() const { return v.bytes[0] == 0xFF && (v.bytes[1] & 0x0F) == 0x05; }
      bool is_multicast_org_local() const  { return v.bytes[0] == 0xFF && (v.bytes[1] & 0x0F) == 0x08; }
      bool is_multicast_global() const     { return v.bytes[0] == 0xFF && (v.bytes[1] & 0x0F) == 0x0E; }

      bytes_type to_bytes() const
      {
        bytes_type bytes;
        for(unsigned i = 0; i < addr_size; i++)
          bytes[i] = v.bytes[i];
        return bytes;
      }

      string to_string(error_code& ec = throws()) const
      {
        // http://en.wikipedia.org/wiki/IPv6#Address_Format
        /*
        IPv6 addresses have two logical parts: a 64-bit network prefix, and a 64-bit host address part. 
        An IPv6 address is represented by 8 groups of 16-bit hexadecimal values separated by colons (:) shown as follows:
        2001:0db8:85a3:0000:0000:8a2e:0370:7334
        ::1 -- loopback

        The hexadecimal digits are case-insensitive.

        The 128-bit IPv6 address can be abbreviated with the following rules:
        Rule one: Leading zeroes within a 16-bit value may be omitted. For example, the address fe80:0000:0000:0000:0202:b3ff:fe1e:8329 may be written as fe80:0:0:0:202:b3ff:fe1e:8329
        Rule two: A single occurrence of consecutive groups of zeroes within an address may be replaced by a double colon. For example, fe80:0:0:0:202:b3ff:fe1e:8329 becomes fe80::202:b3ff:fe1e:8329
        Note: compression can be appled only once
        */

        char buf[64 /*8*4 + 8 + 9*/], *dst = buf;

        if(is_v4_mapped()){
          strcpy(dst, "::ffff:"); dst += 7;
          for(unsigned i = 12; i < 16; i++){
            size_t n;
            ntl::numeric::itoa(v.bytes[i], dst, 16, 10, &n);
            dst += n;
            *dst++ = '.';
          }
          dst[-1] = 0;
          return std::string(buf);
        }


        uint16_t w, pw = 1;
        bool compressed = false;
        for(unsigned i = 0; i < 8; i++, pw = w){
          w = ntl::big_endian(v.words[i]);
          if(w == 0/* && i < 7*/){
            if(pw != 0 && ntl::big_endian(v.words[i+1]) == 0)
              // next is zero, skip current
              continue;

            if(pw == 0){
              // two sibling zeroes
              if(!compressed){
                // compress it
                if(dst == buf || dst[-1] != ':')
                  *dst++ = ':';
                *dst++ = ':';
                compressed = true;
              }
              continue;
            }
          }
          size_t c;
          ntl::numeric::itoa(w, dst, _countof(buf), 16, &c);
          dst += c;
          *dst++ = ':';
        }
        if(dst - buf == 2)
          // its just "::"
          dst++;

        if(scope != 0){
          dst[-1] = '%';
          size_t c;
          ntl::numeric::itoa(scope, dst, _countof(buf), 10, &c);
          dst += c+1;
        }
        dst[-1] = 0;
        if(&ec != &throws())
          ec.clear();
        return std::string(buf);
      }

      address_v4 to_v4() const __ntl_throws(bad_cast)
      {
        if(!(is_v4_mapped() || is_v4_compatible())){
          __ntl_throw(bad_cast());
          return address_v4();
        }
        address_v4::bytes_type v4 = { v.bytes[12], v.bytes[13], v.bytes[14], v.bytes[15] };
        return address_v4(v4);
      }

      ///\name static members:
      static address_v6 from_string(const string& str, error_code& ec = throws())
      {
        address_v6 addr;
        if(&ec != &throws())
          ec.clear();
        const size_t strsize = str.size();
        if(strsize < 3){
          // empty string or "::" are mapped to zero address
          bool ok = strsize == 0 || (strsize == 2 && str[0] == ':' && str[1] == ':');
          if(ok == false)
            throw_system_error(make_error_code(network_error::invalid_argument), ec);
          return addr;
        }

        // validate ipv6 address
        bool ok = true, compressed = false;
        unsigned parts = 1, compress_part = 0;
        string::const_iterator p = str.cbegin(), end = str.cend(), dst;

        // check zone index
        {
          // http://tools.ietf.org/html/rfc4007
          // NOTE: supported only '%' sign and positive numeric zone IDs
          size_t pos = str.rfind('%');
          if(pos != string::npos && pos < (strsize - 1)){
            const char* ids = str.c_str() + pos + 1;
            addr.scope = ntl::numeric::strtoul(ids, &dst);
            if(dst == ids)
              ok = false;
            else
              end = str.cbegin() + pos;
          }
        }
        if(str.compare(0, 7, "::ffff:", 7) == 0){
          // check mapped address
          if(str.find('.', 7) != string::npos){
            // its ipv4 mapped
            address_v4 v4 = address_v4::from_string(string(str, 7, end - str.cbegin() - 7), ec);
            if(ec == false){
              const uint32_t id = addr.scope;
              addr = v4_mapped(v4);
              addr.scope = id;
            }
            return addr;
          }
        }
        if(p[0] == ':'){
          if(p[1] == ':'){
            if(p[2] != ':')
              compressed = true, p += 2;
            else
              ok = false;
          }else{
            // :z isn't valid
            ok = false;
          }
        }
        for(;ok && p < end-1; p++){
          if(*p == ':'){
            if(p[-1] == ':'){
              if(compressed){
                ok = false;
                break;
              }
              // skip compressed part
              compressed = true;
              compress_part = parts++;
              continue;
            }
            if(p[1] != ':')
              parts++;
            continue;
          }
          if(!ntl::numeric::detail::mini_ctype::is(ntl::numeric::detail::mini_ctype::xdigit, *p))
            ok = false;
        }
        if(!ntl::numeric::detail::mini_ctype::is(ntl::numeric::detail::mini_ctype::xdigit, *p))
          ok = false;
        if(compressed == false && parts != 8)
          ok = false;

        unsigned i;
        p = str.c_str();
        uint16_t* words = addr.v.words;
        for(i = 0; ok && i < 8 && p < end; i++){
          // ::z, x::y:z, x:y::z
          if(compressed && i == compress_part){
            // this is start of compress, move to next part
            p += 1 + (i == 0);
            unsigned remain = parts - compress_part;
            i = 8 - remain - 1;
            continue;
          }

          unsigned long v;
          if(*p == '0' && (p+1 >= end || p[1] == ':')){
            v = 0, p += 2;
          } else {
            v = ntl::numeric::strtoul(p, &dst, 16);
            if(dst != end && (dst == p || *dst != ':')){
              ok = false;
              break;
            }
            p = dst+1;
          }
          words[i] = ntl::big_endian(static_cast<uint16_t>(v));
        }
        if(!ok || i < 8){
          throw_system_error(make_error_code(network_error::invalid_argument), ec);
          return address_v6();
        }
        return addr;
      }

      static address_v6 any() { return address_v6(); }
      static address_v6 loopback()
      {
        address_v6 x;
        x.v.bytes[15] = 1;
        return x;
      }
      static address_v6 v4_mapped(const address_v4& addr)
      {
        address_v4::bytes_type v4b = addr.to_bytes();
        bytes_type v6b = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0xFF, 0xFF, v4b[0], v4b[1], v4b[2], v4b[3] };
        return address_v6(v6b);
      }
      static address_v6 v4_compatible(const address_v4& addr)
      {
        address_v4::bytes_type v4b = addr.to_bytes();
        bytes_type v6b = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          v4b[0], v4b[1], v4b[2], v4b[3] };
        return address_v6(v6b);
      }

      ///\name address_v6 comparisons:
      friend inline bool operator==(const address_v6& a, const address_v6& b) { return a.scope == b.scope && memcmp(a.v.bytes, b.v.bytes, addr_size) == 0; } //-V106
      friend inline bool operator!=(const address_v6& a, const address_v6& b) { return rel_ops::operator!=(a, b); }
      friend inline bool operator< (const address_v6& a, const address_v6& b) { return a.to_bytes() < b.to_bytes() || (memcmp(a.v.bytes, b.v.bytes, addr_size) == 0 && a.scope < b.scope); } //-V106
      friend inline bool operator> (const address_v6& a, const address_v6& b) { return rel_ops::operator> (a, b); }
      friend inline bool operator<=(const address_v6& a, const address_v6& b) { return rel_ops::operator<=(a, b); }
      friend inline bool operator>=(const address_v6& a, const address_v6& b) { return rel_ops::operator>=(a, b); }
      ///\}
    private:
      enum { addr_size = 16U };
      ntl::network::in_addr6 v;
      unsigned long scope;
    };



    /**
     *	@brief 5.9.2. Class ip::address
     **/
    class address
    {
    public:
      ///\name constructors:
      address()
        :v4addr(true)
      {}
      address(const address_v4& addr)
        :v4addr(true), v4(addr)
      {}
      address(const address_v6& addr)
        :v4addr(false), v6(addr)
      {}

      ///\name assignment:
      address& operator=(const address_v4& addr)
      {
        v4 = addr, v4addr = true; return *this;
      }
      address& operator=(const address_v6& addr)
      {
        v6 = addr, v4addr = true; return *this;
      }

      ///\name members:
      bool is_multicast() const { return v4addr ? v4.is_multicast() : v6.is_multicast(); }
      bool is_v4() const        { return v4addr; }
      bool is_v6() const        { return v4addr == false; }

      address_v4 to_v4(bool mapped = false) const
      {
        if(!v4addr){
          if(mapped){
            return v6.to_v4();
          }
          __ntl_throw(bad_cast());
        }
        return v4; 
      }
      address_v6 to_v6(bool mapped = false) const
      {
        if(v4addr){
          if(mapped)
            return address_v6::v4_mapped(v4);
          __ntl_throw(bad_cast());
        }
        return v6;
      }

      string to_string(error_code& ec = throws()) const
      {
        return v4addr ? v4.to_string(ec) : v6.to_string(ec);
      }

      ///\name static members:
      static address from_string(const string& str, error_code& ec = throws())
      {
        if(&ec != &throws())
          ec.clear();
        error_code e;
        address_v4 v4 = address_v4::from_string(str, e);
        if(e == false)
          return address(v4);
        address_v6 v6 = address_v6::from_string(str, e);
        if(e == false)
          return address(v6);
        throw_system_error(e, ec);
        return address();
      }

      ///\name address comparisons:
      friend inline bool operator==(const address& a, const address& b)
      {
        return (a.is_v4() && b.is_v4() && a.to_v4() == b.to_v4()) ||
          (a.is_v6() && b.is_v6() && a.to_v6() == b.to_v6());
      }
      friend inline bool operator!=(const address& a, const address& b) { return rel_ops::operator!=(a, b); }
      friend inline bool operator< (const address& a, const address& b)
      {
        return (a.is_v4() && b.is_v4() && a.to_v4() < b.to_v4()) ||
          (a.is_v6() && b.is_v6() && a.to_v6() < b.to_v6());
      }
      friend inline bool operator> (const address& a, const address& b) { return rel_ops::operator> (a, b); }
      friend inline bool operator<=(const address& a, const address& b) { return rel_ops::operator<=(a, b); }
      friend inline bool operator>=(const address& a, const address& b) { return rel_ops::operator>=(a, b); }
      ///\}
    private:
      address_v6 v6;
      address_v4 v4;
      bool v4addr;
    };


    ///\name IP Address I/O
    template<class CharT, class Traits>
    inline basic_ostream<CharT, Traits>& operator<<(basic_ostream<CharT, Traits>& os, const address_v4& addr)
    {
      error_code ec;
      string s = addr.to_string(ec);
      if(ec)
        os.setstate(ios_base::failbit);
      else
        os << s;
      return os;
    }

    template<class CharT, class Traits>
    inline basic_ostream<CharT, Traits>& operator<<(basic_ostream<CharT, Traits>& os, const address_v6& addr)
    {
      error_code ec;
      string s = addr.to_string(ec);
      if(ec)
        os.setstate(ios_base::failbit);
      else
        os << s;
      return os;
    }

    template<class CharT, class Traits>
    inline basic_ostream<CharT, Traits>& operator<<(basic_ostream<CharT, Traits>& os, const address& addr)
    {
      error_code ec;
      string s = addr.to_string(ec);
      if(ec)
        os.setstate(ios_base::failbit);
      else
        os << s;
      return os;
    }
    ///\}

  }
}}}
#endif // NTL__STLX_TR2_NETWORK_IPADDRESS
