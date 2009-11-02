/**\file*********************************************************************
 *                                                                     \brief
 *  CPU specific stuff
 *
 ****************************************************************************
 */
#ifndef NTL__CPU
#define NTL__CPU
#pragma once

namespace ntl {

  /// Compiler intrinsics \internal
  namespace intrinsic {
    extern "C" void __cdecl _mm_pause();
    #pragma intrinsic(_mm_pause)
  }

  /// CPU functions
  namespace cpu
  {
    /** The execution of the next instruction is delayed an implementation specific amount of time. */
    static inline void pause() { intrinsic::_mm_pause(); }

    /** Delay execution for \p delay units */
    static inline void pause(unsigned int delay)
    {
      while(delay--)
        intrinsic::_mm_pause();
    }

#ifdef NTL__NT_BASEDEF
    static inline void yield() { ntl::nt::ZwYieldExecution(); }
#endif
  } // cpu
} // ntl
#endif // NTL__CPU
