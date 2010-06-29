/**\file*********************************************************************
 *                                                                     \brief
 *  Console support
 *
 ****************************************************************************
 */
#ifndef NTL__WIN_CONSOLE
#define NTL__WIN_CONSOLE
#pragma once

#include "windef.hxx"
#include "handle.hxx"
#include "../nt/peb.hxx"
#include "application.hxx"
#include "stlx/stdstring.hxx"

#include "stlx/streambuf.hxx"
#include "../nt/file.hxx"

namespace ntl {
namespace win {


NTL__EXTERNAPI
boolean __stdcall AllocConsole();

NTL__EXTERNAPI
boolean __stdcall AttachConsole(uint32_t);

NTL__EXTERNAPI
boolean __stdcall FreeConsole();


NTL__EXTERNAPI
boolean __stdcall
  WriteConsoleA(
    legacy_handle ConsoleOutput,
    const void *  Buffer,
    uint32_t      NumberOfCharsToWrite,
    uint32_t *    NumberOfCharsWritten,
    void *        Reserved
    );

NTL__EXTERNAPI
boolean __stdcall
  WriteConsoleW(
    legacy_handle ConsoleOutput,
    const void *  Buffer,
    uint32_t      NumberOfCharsToWrite,
    uint32_t *    NumberOfCharsWritten,
    void *        Reserved
    );

NTL__EXTERNAPI
boolean __stdcall
  ReadConsoleA(
    legacy_handle ConsoleInput,
    void*         Buffer,
    uint32_t      NumberOfCharsToRead,
    uint32_t*     NumberOfCharsReaded,
    void*         InputControl
    );

NTL__EXTERNAPI
boolean __stdcall
  ReadConsoleW(
    legacy_handle ConsoleInput,
    void*         Buffer,
    uint32_t      NumberOfCharsToRead,
    uint32_t*     NumberOfCharsReaded,
    void*         InputControl
    );

NTL__EXTERNAPI
boolean __stdcall GetConsoleMode(legacy_handle ConsoleHandle, uint32_t* Mode);

NTL__EXTERNAPI
boolean __stdcall SetConsoleMode(legacy_handle ConsoleHandle, uint32_t  Mode);

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

    enum mode_type {
      DefaultMode,
      EnableProcessedInput  = 0x01,
      EnableLineInput       = 0x02,
      EnableEchoInput       = 0x04,
      EnableWindowInput     = 0x08,
      EnableMouseInput      = 0x10,
      EnableInsertMode      = 0x20,
      EnableQuickEditMode   = 0x40
    };
    __ntl_bitmask_type(mode_type, friend);

  public:
    static legacy_handle handle(type t)
    {
      return *(&nt::peb::instance().ProcessParameters->StandardInput + t);
    }

    static bool is_console_handle(legacy_handle h)
    {
      return (uintptr_t(h) & 0x03) == 0x03;
    }

    static bool is_console_handle(type t)
    {
      return is_console_handle(handle(t));
    }

    static mode_type mode(legacy_handle h)
    {
      uint32_t m = 0;
      GetConsoleMode(h, &m);
      return static_cast<mode_type>(m);
    }

    static mode_type mode(legacy_handle h, mode_type m)
    {
      uint32_t prev_mode = 0;
      GetConsoleMode(h, &prev_mode);
      SetConsoleMode(h, m);
      return static_cast<mode_type>(prev_mode);
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
  static const size_t buffer_size = 1024;

public:
  using typename std::basic_streambuf<charT, traits>::char_type;
  using typename std::basic_streambuf<charT, traits>::int_type;

  explicit console_buffer(legacy_handle outh, bool is_output = true)
    :outh(outh), buffer(), is_console_handle(console::is_console_handle(outh)),
    write_only(is_output)
  {
    if(outh)
      init();
  }

  explicit console_buffer(console::type type, bool is_output = true)
    :outh(console::handle(type)), buffer(), is_console_handle(console::is_console_handle(type)),
    write_only(is_output)
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
    if(!s && !n)
      // unbuffered io
      s = buffer, n = 1;

    if(write_only)
      this->setp(s, s+n);
    else
      this->setg(s, s, s);
    return this;
  }

  virtual int sync()
  {
    if(!outh) return -1;

    streamsize size = this->pptr() - this->pbase();
    if(size){
      size = write(this->pbase(), size);
      reset();
    }
    return static_cast<int>(size);
  }

  virtual streamsize showmanyc()
  {
    _assert_msg("not implemented yet");
    return 0;
  }

  virtual int_type underflow()
  {
    return do_underflow(1);
  }

  int_type do_underflow(streamsize /*n*/)
  {
    const int_type eof = traits_type::eof();
    if(write_only || !outh)
      return eof;

    // backup:  gptr-eback [begin,pos) (already readed)
    // pending: egptr-gptr [pos,end)

    streamsize avail = this->egptr() - this->gptr();
    if(avail > 0)
      // just return it
      return traits_type::to_int_type(*this->gptr());

    streamsize cb;
    char_type* p = this->eback();

    if(this->egptr() == p){
      // first call
      cb = 128;
    }else{
      avail = this->gptr() - this->eback();
      avail = avail > 32 ? 16 : (avail <= 1 ? 0 : 1); // leave 16 characters for backup seq if avail
      cb = buffer_size - avail;
      if(avail){
        traits_type::move(p, this->gptr()-1, avail);
        p += avail;
        this->setg(buffer, p, p); // temporary set read position to the end of backup
      }
    }
    //if(is_console_handle)
      //cb = std::min(n, cb);

    avail = read(p, cb);
    bool ok = avail > 0;

    if(!ok)
      return eof;

    // [back|pos|readed]
    this->setg(this->eback(), p, p+avail);

    return traits_type::to_int_type(*this->gptr());
  }

  virtual streamsize xsgetn(char_type* s, streamsize n)
  {
    const int_type eof = traits_type::eof();
    for (streamsize copied = 0;;){
      if(n <= 0)
        return copied;
      const streamsize ravail = this->gend - this->gnext;
      if(ravail <= 0)
      {
        if(traits_type::eq_int_type(eof, do_underflow(n)))
          return copied;
        continue;
      }
      //__assume(n >= 0 && ravail >= 0);
      const size_t chunksize = static_cast<size_t>(std::min(n, ravail));
      traits_type::copy(s, this->gnext, chunksize);
      s += chunksize;
      copied += chunksize;
      this->gnext += chunksize;
      n -= chunksize;
    }
  }


  virtual int_type overflow(int_type c)
  {
    const int_type eof = traits_type::eof();
    const bool eofc = traits_type::eq_int_type(c, traits_type::eof());
    if(write_only == false || !outh || (!this->pbase() && eofc))
      return eof;

    bool ok = true;
    if(this->pbase()){
      const streamsize pending = this->pptr() - this->pbase();
      ok = write(this->pbase(), pending) == pending;
      if(ok)
        reset();
    }
    if(ok && !eofc){
      const char_type cc = traits_type::to_char_type(c);
      ok = write(&cc, 1) == 1;
    }
    if(!ok)
      return eof;
    return eofc ? traits_type::not_eof(c) : c;
  }

protected:
  void init()
  {
    buffer = new charT[buffer_size];
    setbuf(buffer, buffer_size);
    //console::mode_type m = console::mode(outh);
    //m |= write_only ? console::EnableQuickEditMode : console::EnableInsertMode;
    //console::mode(outh, m);
  }
  void reset()
  {
    if(write_only)
      this->setp(this->pbase(), this->epptr());
    else{
      char_type* g = this->eback();
      this->setg(g, g, g);
    }
  }

  ///\name write functions
  streamsize console_write(const char* data, streamsize n)
  {
    uint32_t written = 0;
    WriteConsoleA(outh, data, static_cast<uint32_t>(n), &written, 0);
    return static_cast<streamsize>(written);
  }
  streamsize console_write(const wchar_t* data, streamsize n)
  {
    uint32_t written = 0;
    WriteConsoleW(outh, data, static_cast<uint32_t>(n), &written, 0);
    return static_cast<streamsize>(written);
  }
  streamsize file_write(const char_type* data, streamsize n)
  {
    typedef std::codecvt<char_type,char,traits::state_type> codecvt;
    static const codecvt& cvt = std::use_facet<codecvt>(this->getloc());

    using std::codecvt_base;
    traits::state_type state;

    const char_type *from_next, *to = data+n, *from = data;
    char buf[1024], *to_next;
    const void* p;
    streamsize pending = n;

    NTL__SUBSYSTEM_NS::file_handler f; f.reset(outh);
    __ntl_try {
      do{
        streamsize chunk_size, write_size;
        codecvt_base::result re = cvt.out(state,from,to,from_next,buf,buf+_countof(buf),to_next);
        if(re == codecvt_base::error)
          break;
        else if(re == codecvt_base::noconv)
          p = data, chunk_size = to-from, write_size = chunk_size*sizeof(char_type);
        else // ok | partial
          p = buf, chunk_size = to_next-buf, write_size = chunk_size*sizeof(char);

        assert(write_size > 0);
        if(!NTL__SUBSYSTEM_NS::success(f.write(p, static_cast<uint32_t>(write_size))))
          break;
        const streamsize fwritten = static_cast<streamsize>(f.get_io_status_block().Information);
        assert(fwritten == write_size);
        if(fwritten != write_size)
          break;

        pending -= chunk_size;
        from += chunk_size;
      }while(pending > 0);
    }
    __ntl_catch(...){
      f.release();
      __ntl_rethrow;
    }
    f.release();
    return n - pending;
  }
  streamsize write(const char_type* data, streamsize n)
  {
    return (is_console_handle && known_char_type)
      ? console_write(data,n)
      : file_write(data, n);
  }

  ///\name read functions
  streamsize console_read(char* where, streamsize max_size)
  {
    uint32_t readed = 0;
    //console::mode_type mode = console::mode(outh, console::DefaultMode);
    ReadConsoleA(outh, where, static_cast<uint32_t>(max_size), &readed, nullptr);
    //console::mode(outh, mode);
    return readed;   
  }
  streamsize console_read(wchar_t* where, streamsize max_size)
  {
    uint32_t readed = 0;
    //console::mode_type mode = console::mode(outh, console::DefaultMode);
    ReadConsoleW(outh, where, static_cast<uint32_t>(max_size), &readed, nullptr);
    //console::mode(outh, mode);
    return readed;   
  }
  streamsize file_read(char_type* where, streamsize max_size)
  {
    typedef char fromT;
    typedef std::codecvt<char_type,char,traits::state_type> codecvt;
    static const codecvt& cvt = std::use_facet<codecvt>(this->getloc());

    using std::codecvt_base;

    char_type* to_next;
    fromT buf[1024];
    const fromT *from_next;
    streamsize readed = 0;
    NTL__SUBSYSTEM_NS::file_handler f; f.reset(outh);
    __ntl_try{
      for(;;){
        streamsize cb = std::min(max_size*sizeof(fromT), _countof(buf));
        if(!NTL__SUBSYSTEM_NS::success(f.read(buf, static_cast<uint32_t>(cb))))
          break;
        streamsize freaded = static_cast<streamsize>(f.get_io_status_block().Information);
        assert(freaded > 0);
        freaded /= sizeof(fromT);

        traits::state_type state;
        codecvt_base::result re = cvt.in(state, buf, buf+freaded, from_next, where, where+max_size, to_next);
        if(re == codecvt_base::error)
          break;
        else if(re == codecvt_base::noconv){
          readed = freaded;
          traits_type::copy(where, reinterpret_cast<const char_type*>(buf), readed);
        }else{
          readed = to_next-where;
        }
        break;
      }
    }
    __ntl_catch(...){
      f.release();
      __ntl_rethrow;
    }
    f.release();
    return readed;
  }

  streamsize read(char_type* where, streamsize max_size)
  {
    return (is_console_handle && known_char_type)
      ? console_read(where,max_size)
      : file_read(where, max_size);
  }

  ///\}

private:
  char_type* buffer;
  legacy_handle outh;
  const bool is_console_handle, write_only;
  static const bool known_char_type = std::is_same<char_type, char>::value || std::is_same<char_type, wchar_t>::value;

  console_buffer(const console_buffer&) __deleted;
  console_buffer& operator=(console_buffer&) __deleted;
};


/**@} console_stream */
/**@} console */


}//namespace win
}//namespace ntl

#endif//#ifndef NTL__WIN_CONSOLE
