/**\file*********************************************************************
 *                                                                     \brief
 *  Console support
 *
 ****************************************************************************
 */

#ifndef NTL__WIN_CONSOLE
#define NTL__WIN_CONSOLE

#include "windef.hxx"
#include "handle.hxx"
#include "../nt/peb.hxx"
#include "application.hxx"
#include "stlx/string.hxx"
#include "stlx/streambuf.hxx"

namespace ntl {
namespace win {


NTL__EXTERNAPI
int __stdcall AllocConsole();

NTL__EXTERNAPI
int __stdcall AttachConsole(uint32_t);

NTL__EXTERNAPI
int __stdcall FreeConsole();


NTL__EXTERNAPI
int __stdcall
  WriteConsoleA(
    legacy_handle ConsoleOutput,
    const void *  Buffer,
    uint32_t      NumberOfCharsToWrite,
    uint32_t *    NumberOfCharsWritten,
    void *        Reserved
    );

NTL__EXTERNAPI
int __stdcall
  WriteConsoleW(
    legacy_handle ConsoleOutput,
    const void *  Buffer,
    uint32_t      NumberOfCharsToWrite,
    uint32_t *    NumberOfCharsWritten,
    void *        Reserved
    );


/**\addtogroup  console *********** Console support *************************
 *@{*/


class console
{
  ///////////////////////////////////////////////////////////////////////////
  public:
    enum type {
      stdin,
      stdout,
      stderr
    };

    static legacy_handle handle(type t)
    {
      return *(&nt::peb::instance().ProcessParameters->StandardInput + t);
    }

    static __forceinline
    bool alloc()
    {
      return 0 != AllocConsole();
    }

    ///\note attaches to parent process' console by default
    static __forceinline
    bool attach(uint32_t process_id = uint32_t(-1))
    {
      return 0 != AttachConsole(process_id);
    }

    static __forceinline
    bool free()
    {
      return 0 != FreeConsole();
    }

    template<typename charT>
    static
    uint32_t write(const void * buf, uint32_t chars);

    template<>
    static
    uint32_t write<char>(const void * ptr, uint32_t chars)
    {
      uint32_t written = 0;
      const legacy_handle out = nt::peb::instance().ProcessParameters->StandardOutput;
      WriteConsoleA(out, ptr, chars, &written, 0);
      return written;
    }

    template<>
    static
    uint32_t write<wchar_t>(const void * ptr, uint32_t chars)
    {
      uint32_t written = 0;
      const legacy_handle out = nt::peb::instance().ProcessParameters->StandardOutput;
      WriteConsoleW(out, ptr, chars, &written, 0);
      return written;
    }

    template<typename charT>
    static
    uint32_t write(const charT msg[])
    {
      return write<charT>(msg, static_cast<uint32_t>(std::char_traits<charT>::length(msg)));
    }

    template<typename charT>
    static
    uint32_t write(const charT c)
    {
      return write<charT>(&c, 1);
    }

    template<typename charT>
    static
    uint32_t write(const std::basic_string<charT> & s)
    {
      return write<charT>(s.begin(), s.size() - (s.end()[-1] == '\0'));
    }



};//class console

/** \defgroup console_stream  Console I/O Streams support 
 *@{*/

template<typename charT, class traits = std::char_traits<charT> >
class console_buffer:
  public std::basic_streambuf<charT, traits>
{
  typedef std::streamsize streamsize;
  static const size_t buffer_size = 128;
public:
  explicit console_buffer(legacy_handle outh)
    :outh(outh), buffer()
  {
    if(outh)
      init();
  }

  explicit console_buffer(console::type type)
    :outh(console::handle(type)), buffer()
  {
    init();
  }

  ~console_buffer()
  {
    delete[] buffer;
    buffer = nullptr;
  }

protected:
  ///\name overloaded members
  virtual std::basic_streambuf<charT, traits>* setbuf(char_type* s, streamsize n)
  {
    setp(s, s+n);
    return this;
  }

  virtual int sync()
  {
    streamsize size = pptr() - pbase();
    if(size){
      size = write(pbase(), size);
      reset();
    }
    return static_cast<int>(size);
  }

  virtual int_type overflow(int_type c)
  {
    const bool eofc = traits_type::eq_int_type(c, traits_type::eof());
    if(!pbase() && eofc)
      return traits_type::eof();
    bool ok = true;
    if(pbase()){
      const streamsize pending = pptr() - pbase();
      ok = write(pbase(), pending) == pending;
      reset();
    }
    if(!eofc){
      const char_type cc = traits_type::to_char_type(c);
      ok &= write(&cc, 1) == 1;
    }
    return ok ? (traits_type::eq_int_type(c, traits_type::eof()) ? traits_type::not_eof(c) : c) : traits_type::eof();
  }

protected:
  void init()
  {
    buffer = new char_type[buffer_size];
    setbuf(buffer, buffer_size);
  }
  void reset()
  {
    setp(pbase(), epptr());
  }
  streamsize write(const char* data, streamsize n)
  {
    uint32_t written = 0;
    WriteConsoleA(outh, data, static_cast<uint32_t>(n), &written, 0);
    return static_cast<streamsize>(written);
  }
  streamsize write(const wchar_t* data, streamsize n)
  {
    uint32_t written = 0;
    WriteConsoleW(outh, data, static_cast<uint32_t>(n), &written, 0);
    return static_cast<streamsize>(written);
  }

private:
  char_type* buffer;
  legacy_handle outh;
};


/**@} console_stream */
/**@} console */


}//namespace win
}//namespace ntl

#endif//#ifndef NTL__WIN_CONSOLE
