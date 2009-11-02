/**\file*********************************************************************
 *                                                                     \brief
 *  Native Application support 
 *
 ****************************************************************************
 */
#ifndef NTL__WIN_NATIVEAPP
#define NTL__WIN_NATIVEAPP
#pragma once

#include "nt/basedef.hxx"
#include "nt/new.hxx"
#include "nt/thread.hxx"
#include <cstdlib>

namespace ntl
{
  class nativeapp
  {
  public:
    nt::ntstatus main(const nt::unicode_string&);
  };

#ifdef _MSC_VER
  // this used to prevent linker error if CRT initialization isn't needed
#ifdef _M_X64
# pragma comment(linker, "/alternatename:__init_crt=__init_crt_stub")
#else
# pragma comment(linker, "/alternatename:___init_crt=___init_crt_stub")
#endif
  extern "C" inline void _cdecl __init_crt_stub(){}
#endif

#pragma warning(push)
#pragma warning(disable:4702) // unreachable code

#pragma comment(linker, "/entry:_Nativeapp_entry /subsystem:native")
  void _Nativeapp_entry(nt::peb* Peb)
  {
    using namespace ntl::nt;
    nt::rtl_user_process_parameters* upp = Peb->process_parameters();
    unicode_string us_empty;
    ntl::crt_initializer __crt;
    nativeapp app;
    ntstatus st = app.main(upp ? upp->CommandLine : us_empty);
    NtTerminateProcess(current_process(), st);

    // for explicit instantiation to eliminate annoyed including "nt/new.hxx"
    new char, new char[0], new (std::nothrow) char, new(std::nothrow) char[0];
    __init_crt_stub();
  }
}
#pragma warning(pop)
#endif // NTL__WIN_NATIVEAPP
