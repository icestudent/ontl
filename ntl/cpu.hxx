/**\file*********************************************************************
 *                                                                     \brief
 *  CPU specific stuff
 *
 ****************************************************************************
 */


#ifndef NTL__CPU
#define NTL__CPU

#include "base.hxx"

namespace ntl {
namespace cpu {

#ifdef _MSC_VER
  #ifdef _M_IX86

  static inline void pause() { __asm { rep nop } }


  #else // ! _M_IX86
  #error unsupported CPU type
  #endif
#else // ! _MSC_VER
#error unsupported compiler
#endif

}//namespace cpu
}//namespace ntl

#endif//#ifndef NTL__CPU
