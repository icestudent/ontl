/**\file*********************************************************************
*                                                                     \brief
*  Thread Context
*
****************************************************************************
*/
#ifndef NTL__NT_CONTEXT
#define NTL__NT_CONTEXT
#pragma once

#include "basedef.hxx"

namespace ntl {
  namespace nt {
#pragma warning(push)
#pragma warning(disable:4201) // nameless struct or union

#if defined(_M_IX86)

struct floating_save_area {
  uint32_t   ControlWord;
  uint32_t   StatusWord;
  uint32_t   TagWord;
  uint32_t   ErrorOffset;
  uint32_t   ErrorSelector;
  uint32_t   DataOffset;
  uint32_t   DataSelector;
  uint8_t    RegisterArea[80];
  uint32_t   Cr0NpxState;
};

struct context {
  uint32_t ContextFlags;

  uint32_t   Dr0;
  uint32_t   Dr1;
  uint32_t   Dr2;
  uint32_t   Dr3;
  uint32_t   Dr6;
  uint32_t   Dr7;

  floating_save_area FloatSave;

  uint32_t   SegGs;
  uint32_t   SegFs;
  uint32_t   SegEs;
  uint32_t   SegDs;

  uint32_t   Edi;
  uint32_t   Esi;
  uint32_t   Ebx;
  uint32_t   Edx;
  uint32_t   Ecx;
  uint32_t   Eax;

  uint32_t   Ebp;
  uint32_t   Eip;
  uint32_t   SegCs;
  uint32_t   EFlags;
  uint32_t   Esp;
  uint32_t   SegSs;

  uint8_t   ExtendedRegisters[512];

};

#elif defined(_M_X64)

//
// Define 128-bit 16-byte aligned xmm register type.
//

struct __declspec(align(16)) m128_t
{
  uint64_t  Low;
  int64_t   High;
};

//
// Format of data for 32-bit fxsave/fxrstor instructions.
//

struct xmm_save_area32
{
  uint16_t ControlWord;
  uint16_t StatusWord;
  uint8_t TagWord;
  uint8_t Reserved1;
  uint16_t ErrorOpcode;
  uint32_t ErrorOffset;
  uint16_t ErrorSelector;
  uint16_t Reserved2;
  uint32_t DataOffset;
  uint16_t DataSelector;
  uint16_t Reserved3;
  uint32_t MxCsr;
  uint32_t MxCsr_Mask;
  m128_t FloatRegisters[8];
  m128_t XmmRegisters[16];
  uint8_t Reserved4[96];
};

struct __declspec(align(16)) context {

  //
  // Register parameter home addresses.
  //
  // N.B. These fields are for convience - they could be used to extend the
  //      context record in the future.
  //

  uint64_t P1Home;
  uint64_t P2Home;
  uint64_t P3Home;
  uint64_t P4Home;
  uint64_t P5Home;
  uint64_t P6Home;

  //
  // Control flags.
  //

  uint32_t ContextFlags;
  uint32_t MxCsr;

  //
  // Segment Registers and processor flags.
  //

  uint16_t SegCs;
  uint16_t SegDs;
  uint16_t SegEs;
  uint16_t SegFs;
  uint16_t SegGs;
  uint16_t SegSs;
  uint32_t EFlags;

  //
  // Debug registers
  //

  uint64_t Dr0;
  uint64_t Dr1;
  uint64_t Dr2;
  uint64_t Dr3;
  uint64_t Dr6;
  uint64_t Dr7;

  //
  // Integer registers.
  //

  uint64_t Rax;
  uint64_t Rcx;
  uint64_t Rdx;
  uint64_t Rbx;
  uint64_t Rsp;
  uint64_t Rbp;
  uint64_t Rsi;
  uint64_t Rdi;
  uint64_t R8;
  uint64_t R9;
  uint64_t R10;
  uint64_t R11;
  uint64_t R12;
  uint64_t R13;
  uint64_t R14;
  uint64_t R15;

  //
  // Program counter.
  //

  uint64_t Rip;

  //
  // Floating point state.
  //

  union {
    xmm_save_area32 FltSave;
    struct {
      m128_t Header[2];
      m128_t Legacy[8];
      m128_t Xmm0;
      m128_t Xmm1;
      m128_t Xmm2;
      m128_t Xmm3;
      m128_t Xmm4;
      m128_t Xmm5;
      m128_t Xmm6;
      m128_t Xmm7;
      m128_t Xmm8;
      m128_t Xmm9;
      m128_t Xmm10;
      m128_t Xmm11;
      m128_t Xmm12;
      m128_t Xmm13;
      m128_t Xmm14;
      m128_t Xmm15;
    };
  };

  //
  // Vector registers.
  //

  m128_t VectorRegister[26];
  uint64_t VectorControl;

  //
  // Special debug control registers.
  //

  uint64_t DebugControl;
  uint64_t LastBranchToRip;
  uint64_t LastBranchFromRip;
  uint64_t LastExceptionToRip;
  uint64_t LastExceptionFromRip;
};
#endif // _M_X64

#pragma warning(pop)
  } // namespace nt
} // namespace ntl

#endif // NTL__NT_CONTEXT
