/**\file*********************************************************************
 *                                                                     \brief
 *  Win32 Console Application support 
 *
 ****************************************************************************
 */

#ifndef NTL__WIN_CONSOLEAPP
#define NTL__WIN_CONSOLEAPP

#include "win/application.hxx"
#include "win/console.hxx"
#include "nt/new.hxx"
#include "nt/thread.hxx"
#include <cstdlib>

#pragma comment(linker, "/subsystem:console")
#pragma comment(linker, "/entry:ntl::_Consoleapp_entry")

#pragma comment(linker, "/nodefaultlib:libc")
#pragma comment(linker, "/nodefaultlib:libcmt")
#pragma comment(linker, "/nodefaultlib:oldnames")

namespace ntl {

/**\addtogroup  winapi_types_support *** Win32 API support library **********
 *@{*/
/**\addtogroup  application ****** Application support library **************
 *@{*/

class consoleapp : public win::application<win::tchar_t>
{
  public:
    __forceinline int main();

    typedef win::console console;

};

extern"C" void _cdecl __console_abort();

#ifdef _MSC_VER
  // this used to prevent linker error if CRT initialization isn't needed
  #ifdef _M_X64
  # pragma comment(linker, "/alternatename:__init_crt=__init_crt_stub")
  # pragma comment(linker, "/alternatename:abort=__console_abort")
  #else
  # pragma comment(linker, "/alternatename:___init_crt=___init_crt_stub")
  # pragma comment(linker, "/alternatename:_abort=___console_abort")
  #endif
  extern "C" inline void _cdecl __init_crt_stub(){}
#endif

#pragma warning(push)
#pragma warning(disable:4714)//function 'int __thiscall ntl::consoleapp::main(void)' marked as __forceinline not inlined
int _Consoleapp_entry()
{
  #ifdef _MSC_VER
  __init_crt_stub();
  static void* volatile ref2 = __console_abort;
  #endif
  crt_initializer __crt;
  consoleapp app;
  return app.main();
}
#pragma warning(pop)


//////////////////////////////////////////////////////////////////////////
namespace win {
NTL__EXTERNAPI __declspec(noreturn) void __stdcall ExitProcess(ntl::nt::ntstatus);
}

extern"C" void _cdecl __console_abort()
{
  win::ExitProcess(ntl::nt::status::unsuccessful);
}

/**@} application */
/**@} winapi_types_support */

}//namespace ntl

#endif//#ifndef NTL__WIN_CONSOLEAPP
