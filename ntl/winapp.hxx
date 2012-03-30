/**\file*********************************************************************
 *                                                                     \brief
 *  Win32 Application support
 *
 ****************************************************************************
 */
#ifndef NTL__WIN_WINAPP
#define NTL__WIN_WINAPP
#pragma once

#include "win/application.hxx"
#include "nt/new.hxx"
#include "cstdlib"

#pragma comment(linker, "/subsystem:windows")
#pragma comment(linker, "/entry:ntl::_Winapp_entry")

#pragma comment(linker, "/nodefaultlib:libc")
#pragma comment(linker, "/nodefaultlib:libcmt")
#pragma comment(linker, "/nodefaultlib:oldnames")

namespace ntl {

/**\addtogroup  winapi_types_support *** Win32 API support library **********
 *@{*/
/**\addtogroup  application ****** Application support library **************
 *@{*/


class winapp : public win::application<win::tchar_t>
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    //static //?
    __forceinline int main();

};

extern"C" void __cdecl __console_abort();

#ifdef _MSC_VER
// this used to prevent linker error if CRT initialization isn't needed
  #ifdef _M_X64
  # pragma comment(linker, "/alternatename:__init_crt=__init_crt_stub")
  # pragma comment(linker, "/alternatename:abort=__console_abort")
  #else
  # pragma comment(linker, "/alternatename:___init_crt=___init_crt_stub")
  # pragma comment(linker, "/alternatename:_abort=___console_abort")
  #endif
extern "C" inline void __cdecl __init_crt_stub(){}
#endif

int _Winapp_entry()
{
#ifdef _MSC_VER
  (void)__security_cookie;
  (void)&__console_abort;
#endif

  winapp app;
  return app.main();

  // for explicit instantiation to eliminate annoyed including "nt/new.hxx"
  new char, new char[0], new (std::nothrow) char, new(std::nothrow) char[0];
  __init_crt_stub();
}

//////////////////////////////////////////////////////////////////////////
namespace win {
  NTL_EXTERNAPI __noreturn void __stdcall ExitProcess(ntl::nt::ntstatus);
}

extern"C" void __cdecl __console_abort()
{
  win::ExitProcess(ntl::nt::status::unsuccessful);
}

/**@} application */
/**@} winapi_types_support */

}//namespace ntl


#endif//#ifndef NTL__WIN_WINAPP
