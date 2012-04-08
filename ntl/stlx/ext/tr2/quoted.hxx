/**\file*********************************************************************
  *                                                                     \brief
  *  Boost.Io.Quoted manipulator
  *
  ****************************************************************************
  */
#ifndef NTL__STLX_TR2_QUOTED
#define NTL__STLX_TR2_QUOTED
#pragma once

#include "../../istream.hxx"

namespace std
{
  namespace __
  {
    template<typename string, typename charT>
    struct quoted_proxy
    {
      string s;
      charT esc, sep;
      const ssize_t len;
    };

    template <class charT, class traits>
    inline std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& os, const quoted_proxy<std::basic_string<charT,traits>&, charT>& q)
    {
      const quoted_proxy<const charT*, charT> qp = {q.s.c_str(), q.esc, q.sep, q.s.length()};
      return os << qp;
    }

    template <class charT, class traits>
    inline std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& os, const quoted_proxy<const charT*, charT>& q)
    {
      os << q.sep;
      const charT* s = q.s;
      if(q.len == -1){
        for(charT c = *s; c != 0; c = *++s){
          if(c == q.esc || c == q.sep)
            os << q.esc;
          os << c;
        }
      }else{
        ssize_t len = q.len;
        for(charT c = *s; len--; c = *++s){
          if(c == q.esc || c == q.sep)
            os << q.esc;
          os << c;
        }
      }
      os << q.sep;
      return os;
    }


    template <class charT, class traits>
    inline std::basic_istream<charT, traits>& operator>>(std::basic_istream<charT, traits>& is, const quoted_proxy<std::basic_string<charT,traits>&, charT>& q)
    {
      charT c;
      if(is.get(c), c != q.sep){
        is.unget();
        is >> q.s;
      }else{
        const std::ios_base::fmtflags skipws = is.setf(0, is.skipws);
        q.s.clear();
        for(is.get(c); c != q.sep && is.good(); is.get(c)){
          if(c != q.esc)
            q.s.push_back(c);
        }
        is.setf(skipws);
      }
      return is;
    }

  } // __ ns


  //////////////////////////////////////////////////////////////////////////
  namespace tr2
  {
    template <class charT>
    inline const __::quoted_proxy<const charT*, charT> quoted(const charT* string, charT escape='\\', charT delim='\"')
    {
      const __::quoted_proxy<const charT*, charT> qp = {string, escape, delim, -1};
      return qp;
    }

    template <class charT, class traits, class alloc>
    inline const __::quoted_proxy<const charT*, charT> quoted(const std::basic_string<charT, traits, alloc>& string, charT escape='\\', charT delim='\"')
    {
      const __::quoted_proxy<const charT*, charT> qp = {string.c_str(), escape, delim, string.length()};
      return qp;
    }

    template <class charT, class traits, class alloc>
    inline const __::quoted_proxy<std::basic_string<charT,traits,alloc>&, charT> quoted(std::basic_string<charT, traits, alloc>& string, charT escape='\\', charT delim='\"')
    {
      const __::quoted_proxy<std::basic_string<charT,traits,alloc>&, charT> qp = {string, escape, delim, 0};
      return qp;
    }

  } // tr2
} // std
#endif // NTL__STLX_TR2_QUOTED
