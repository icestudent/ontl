/**\file*********************************************************************
  *                                                                     \brief
  *  Debugger support
  *
  ****************************************************************************
  */
#ifndef NTL__KM_DEBUG
#define NTL__KM_DEBUG
#pragma once

#include "basedef.hxx"
#include "string.hxx"
#include "../nt/debug.hxx"

namespace ntl {
  namespace km {

    using nt::dpfltr;
    using nt::dbgprint;
    using nt::DbgPrint;
    using nt::DbgPrintEx;
    namespace dbg = nt::dbg;

    extern "C" bool* KdDebuggerNotPresent;
    extern "C" bool* KdDebuggerEnabled;

    NTL_EXTERNAPI
      void __stdcall
      DbgBreakPointWithStatus(
      ntstatus Status
      );

    NTL_EXTERNAPI
      bool __stdcall
      KdRefreshDebuggerNotPresent();

    //warning C4100: 'XXX' : unreferenced formal parameter
#pragma warning(push)
#pragma warning(disable:4100)

#ifdef NTL_DEBUG
#	define KdBreakPointWithStatus() DbgBreakPointWithStatus(X)
#else
#	define KdBreakPointWithStatus()
#endif

#pragma warning(pop)

  }//namespace km
}//namespace ntl

#endif//#ifndef NTL__KM_DEBUG
