/**\file*********************************************************************
 *                                                                     \brief
 *  Windows SDK support
 *
 ****************************************************************************
 */
#ifdef NTL__WIN_WINSDKDLL
#error This file must be included in project only once
#endif
#define NTL__WIN_WINSDKDLL

#pragma once

#include "win/sdk.hxx"
#include "rtl/crt.cpp"
#include "nt/new.hxx"
#include "cstdlib"
#include <tchar.h>

BOOL WINAPI DllMain(
  __in  HINSTANCE hinstDLL,
  __in  DWORD fdwReason,
  __in  LPVOID lpvReserved
);

extern "C" BOOL APIENTRY _DllMainCRTStartup(HMODULE Module, DWORD Reason, LPVOID Reserved)
{
	if(Reason == DLL_PROCESS_ATTACH) {
		ntl::__init_crt(true);
	}

	BOOL re = DllMain(Module, Reason, Reserved);

	if(Reason == DLL_PROCESS_DETACH) {
		ntl::__init_crt(false);
	}

	return re;
}
