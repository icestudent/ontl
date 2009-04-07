/**\file*********************************************************************
 *                                                                     \brief
 *  CPU specific stuff
 *
 ****************************************************************************
 */

#ifndef NTL__CPU
#define NTL__CPU


namespace ntl {
namespace cpu {

#if defined(_MSC_VER)
  #ifdef _M_IX86

    static inline void pause() { __asm { pause } }

  #else // ! _M_IX86
} // cpu
namespace intrinsic {
  extern "C" void __cdecl _mm_pause();
  #pragma intrinsic(_mm_pause)
}
namespace cpu {
    static inline void pause() { intrinsic::_mm_pause(); }
  #endif

    static unsigned current();

#elif defined(__GNUC__)
// TODO: pause()
    static inline void pause() {}
#else
# error unsupported compiler
#endif

}//namespace cpu
}//namespace ntl

#endif//#ifndef NTL__CPU
