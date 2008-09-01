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



/**@} console */


}//namespace win
}//namespace ntl

#endif//#ifndef NTL__WIN_CONSOLE
