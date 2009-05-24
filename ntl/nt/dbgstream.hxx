/**\file*********************************************************************
 *                                                                     \brief
 *  Debugger support
 *
 ****************************************************************************
 */
#ifndef NTL__NT_DBGSTREAM
#define NTL__NT_DBGSTREAM

#include "debug.hxx"

#include "../stlx/ostream.hxx"
#include "../stlx/streambuf.hxx"
#include "../stlx/string.hxx"

namespace ntl
{
  namespace nt
  {
    namespace dbg
    {
      namespace __
      {
        template <dpfltr::level DefaultLevel  = dpfltr::__identifier(default),
                  dpfltr::type  Type          = dpfltr::ihvdriver>
        class streambuf:
          public std::basic_streambuf<char>,
          noncopyable
        {
          typedef std::basic_streambuf<char> base;
          typedef std::streamsize streamsize;

          static const size_t buffer_size = 128;
          char buf[buffer_size+sizeof(void*)];  // buffer_size + native alignment

          dbgprint<DefaultLevel, Type>& printer;

        public:
          streambuf(dbgprint<DefaultLevel, Type>& printer)
            :printer(printer)
          {
            setbuf(buf, buffer_size);
          }

        protected:
          virtual base* setbuf(char_type* s, streamsize n)
          {
            setp(s, s+n);
            return this;
          }

          virtual int sync()
          {
            const streamsize size = pptr() - pbase();
            if(size){
              printer.printf("%.*s", size, buf);
              reset();
            }
            return static_cast<int>(size);
          }

          virtual int_type overflow(int_type c)
          {
            if(!pbase())
              return traits_type::eof();

            streamsize pending = pptr() - pbase();
            if(!traits_type::eq_int_type(c, traits_type::eof()))
              *pptr() = traits_type::to_char_type(c), pending++;

            printer.printf("%.*s", pending, buf);
            reset();
            return traits_type::eq_int_type(c, traits_type::eof()) ? traits_type::not_eof(c) : c;
          }

        protected:
          void reset()
          {
            setp(buf, buf+buffer_size);
          }

        };
      }

      template <dpfltr::level DefaultLevel  = dpfltr::__identifier(default),
                dpfltr::type  Type          = dpfltr::ihvdriver>
      class stream:
        public std::basic_ostream<char, std::char_traits<char> >
      {
        typedef std::char_traits<char> char_traits;

        typedef __::streambuf<DefaultLevel, Type> streambuf;
        typedef std::streamsize streamsize;
        streambuf sb;

        dbgprint<DefaultLevel, Type> printer;
      public:
        static const dpfltr::type type = Type;

        stream()
          :basic_ostream(&sb), sb(printer)
        {}

        dbgprint<DefaultLevel, Type>& get_printer() { return printer; }
      };

      extern stream<>   cout;
      extern stream<>   cerror;
      extern stream<>   cwarning;
      extern stream<>   ctrace;
      extern stream<>   cinfo;
    }
  }
}

#define NTL_DBG_DEFINE_STREAMS() namespace ntl { namespace nt{ namespace dbg { \
  stream<>   cout;     \
  stream<>   cerror;   \
  stream<>   cwarning; \
  stream<>   ctrace;   \
  stream<>   cinfo;    \
  }}}

#endif // NTL__NT_DBGSTREAM
