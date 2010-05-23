/**\file*********************************************************************
 *                                                                     \brief
 *  NT Exceptions support
 *
 *  \warning  the file contains the cxxrecord::catchguardhandler function which
 *            has to be registered as .SAFESEH (by linking with rtl/safeseh.asm)
 *            nevertheless /SAFESEH is still unuseful without /GS support.
 *
 ****************************************************************************
 */
#ifndef NTL__NT_EXCEPTION
#define NTL__NT_EXCEPTION
#pragma once

#include "basedef.hxx"
#include "teb.hxx"
#include "../stlx/array.hxx"
#include "../stlx/typeinfo.hxx"

#include "../pe/image.hxx"

#ifdef _NTL_EH_TRACE
  #include <iostream>
  // may not use RAII helper as it is incompatible with __try
  #define _EH_TRACE_ENTER() std::cerr<<" enter "__FUNCTION__<<std::endl;
  #ifdef _M_X64
  # define _EH_TRACE_LEAVE() std::cerr<<" leave "__FUNCTION__<<std::endl;
  #else
  # define _EH_TRACE_LEAVE() __asm{push eax}std::cerr<<" leave "__FUNCTION__<<std::endl;__asm{pop eax}
  #endif // x64
#else
  #define _EH_TRACE_ENTER()
  #define _EH_TRACE_LEAVE()
#endif

#undef exception_code
#undef exception_info
#undef abnormal_termination

#define exception_code              ::_exception_code()
#define exception_info()            reinterpret_cast<struct ntl::nt::exception::pointers*>(ntl::_exception_info())
#define abnormal_termination        (ntl::_abnormal_termination() != 0)

extern "C"
{
  unsigned long __cdecl _exception_code(void);
#ifndef __ICL
  void *        __cdecl _exception_info(void);
  int           __cdecl _abnormal_termination(void);
#endif
}

#include "../nt/debug.hxx"

#ifdef NTL__DEBUG
  void dbg_pause();
# define dbgpause() dbg_pause()
#else
# define dbgpause()
#endif

#ifdef __ICL
#pragma warning(disable:2259) // [remark] non-pointer conversion from
#endif


namespace ntl {
namespace nt {

struct context;

class exception
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    /// Exception filter return values
    enum filter
    {
      continue_execution  = -1,
      continue_search     = 0,
      execute_handler     = 1
    };

    /// Exception disposition return values.
    enum disposition
    {
      ContinueExecution,
      ContinueSearch,
      NestedException,
      CollidedUnwind
    };

    enum flags
    {
      noncontinuable  = 0x01,
      unwinding       = 0x02,
      exit_unwind     = 0x04,
      stack_invalid   = 0x08,
      nested_call     = 0x10,
      target_unwind   = 0x20,
      collied_unwind  = 0x40,
      unwind          = 0x66
    };
    __ntl_bitmask_type(flags, friend);

    typedef uint32_t rva_t;

    struct record;
    struct registration;
    struct dispatcher_context;

    /// Exception handler prototype
    typedef
      disposition __cdecl
      handler_t(
        record              * exception_record,
        registration        * establisher_frame,
        context             * context_record,
        dispatcher_context  * dispatcher_context
        );

#ifdef _M_IX86
    struct dispatcher_context
    {
      registration *          RegistrationPointer;
    };
#endif
#ifdef _M_X64
    /**
     *	@brief Function table entry
     *
     *  Table-based exception handling requires a function table entry for all functions that allocate stack space
     *  or call another function (that is, nonleaf functions).
     *  @note All addresses are image relative (that is, they are 32-bit offsets from the starting address of the image that contains the function table entry).
     **/
    struct __declspec(align(4)) runtime_function
    {
      rva_t BeginAddress;
      rva_t EndAddress;
      rva_t UnwindData;
    };

    /**
     *	@brief Unwind code array entry
     *
     *  It used to record the sequence of operations in the prolog that affect the nonvolatile registers and RSP.
     **/
    union unwind_code
    {
      /** unwind operations */
      enum opcodes {
        /** push a nonvolatile integer register, which decrements RSP by 8, unwind_code::OpInfo member specifies the number of the register. */
        push_nonvol,
        /** allocate a large-sized area on the stack */
        alloc_large,
        /** allocate a small-sized area on the stack */
        alloc_small,
        /** establish the frame pointer register by setting the register to some offset of the current RSP */
        set_fpreg,
        /** save a nonvolatile integer register on the stack using an MOV instead of a PUSH */
        save_nonvol,
        /** save a nonvolatile integer register on the stack with a long offset, using an MOV instead of a PUSH */
        save_nonvol_far,
        /** save the low 64 bits of a nonvolatile XMM register on the stack */
        save_xmm,
        /** save the low 64 bits of a nonvolatile XMM register on the stack with a long offset */
        save_xmm_far,
        /** save all 128 bits of a nonvolatile XMM register on the stack */
        save_xmm128,
        /** save all 128 bits of a nonvolatile XMM register on the stack with a long offset */
        save_xmm128_far,
        /** Push a "machine frame" */
        push_machframe
      };

      /** general-purpose registers encoding */
      enum registers {
        RAX, RCX, RDX, RBX, RSP, RBP, RSI, RDI, R8, R9, R10, R11, R12, R13, R14, R15
      };

#pragma warning(disable:4201)
      struct {
        /** offset from the beginning of the prolog to the end of the instruction that performs this operation,
            plus 1 (that is, the offset of the start of the next instruction) */
        uint8_t CodeOffset;
        /** unwind operation, see \c opcodes */
        uint8_t UnwindOp:4;
        /** information for the operation, often is register-number, see \c registers */
        uint8_t OpInfo:4;
      };
      /** specifies the scaled offset */
      uint16_t  FrameOffset;
    };
#pragma warning(default:4201)

    /**
     *	@brief Unwind data information structure
     *
     *  The unwind data information structure is used to record the effects that a function has on the stack pointer
     *  and where the nonvolatile registers are saved on the stack.
     **/
    struct unwind_info
    {
      /** flags for the unwind data */
      enum flags {
        nhandler    = 0x00,
        /** function has an exception handler that should be called when looking for functions that need to examine exceptions */
        ehandler    = 0x01,
        /** function has a termination handler that should be called when unwinding an exception */
        uhandler    = 0x02,
        /** this \c unwind_info structure is not the primary one for the procedure. Instead, the chained-unwind-info entry
            (\c FunctionEntry member) is the contents of a previous runtime_function entry.
            If this flag is set, then the \c ehandler and \c uhandler flags must be cleared.
            Also, the frame register and fixed-stack allocation fields must have the same values as in the primary unwind information. */
        chaininfo   = 0x04
      };

      /** version number of the unwind data, currently 1 */
      uint8_t Version:3;
      /** flags for unwind data */
      uint8_t Flags:5;
      /** length of the function prolog in bytes */
      uint8_t SizeOfProlog;
      /** number of slots in the unwind codes array */
      uint8_t CountOfCodes;
      /** number of the nonvolatile register used as the frame pointer, if nonzero; specifies that the function does not use a frame pointer otherwise */
      uint8_t FrameRegister:4;
      /** the scaled offset from RSP that is applied to the FP register when it is established, if the FrameRegister member is nonzero;
        The actual FP register is set to RSP plus 16 times this number, allowing offsets from 0 to 240.
        This permits pointing the FP register into the middle of the local stack allocation for dynamic stack frames,
        allowing better code density through shorter instructions (more instructions can use the 8-bit signed offset form). */
      uint8_t FrameOffset:4;
      /** array of items that explains the effect of the prolog on the nonvolatile registers and RSP; for alignment purposes, this array always has an even number of entries. */
      unwind_code UnwindCode[1];

      /** @note following fields are optional */
      union {
        /** image-relative pointer to the function's language-specific exception/termination handler
          (if flag \c chaininfo is clear and one of the flags \c ehandler or \c uhandler is set) */
        rva_t ExceptionHandler;
        /** contents of the chained unwind information (if flag chaininfo is set). In this case, the \c unwind_info structure ends with three dwords,
            which represent the \c runtime_function information for the function of the chained unwind.  */
        rva_t FunctionEntry;
      };
      /** function's language-specific exception handler data */
      rva_t   ExceptionData;

      handler_t* exception_handler(const void* Base) const
      {
        return reinterpret_cast<handler_t*>((char*)Base + get_data(0));
      }

      runtime_function* chained_entry(const void* Base) const
      {
        return reinterpret_cast<runtime_function*>((char*)Base + get_data(0));
      }

      void* exception_data(const void* Base) const
      {
        return (char*)Base + get_data(4);
      }

    private:
      rva_t get_data(ptrdiff_t offset) const
      {
        return *reinterpret_cast<rva_t*>((char*)this + offsetof(unwind_info, UnwindCode) + ((CountOfCodes + 1) & ~1)*sizeof(unwind_code) + offset);
      }
    };

    struct unwind_history_table_entry
    {
      void*             ImageBase;
      runtime_function* FunctionEntry;
    };

    struct unwind_history_table
    {
      static const size_t unwind_history_table_size = 12;

      uint32_t Count;
      uint8_t  Search;
      void*    LowAddress;
      void*    HighAddress;
      unwind_history_table_entry Entry[unwind_history_table_size];
    };

#pragma warning(disable:4201)
    union frame_pointers
    {
      struct
      {
        // ehstate_t
        /*0x00*/ int MemoryStackFp;
        /*0x04*/ int MemoryStoreFp;
      };
        /*0x00*/ uintptr_t FramePointers;
    };
#pragma warning(default:4201)

    /**
     *	@brief Exception dispatch context
     **/
    struct dispatcher_context
    {
      /** exception address or the address at which control left the establishing function */
      void*             ControlPc;
      /** module image base to which control left function belongs */
      class pe::image * ImageBase;
      /** function table entry holding the function and the unwind information image-base relative addresses for this function */
      runtime_function* FunctionEntry;
      /** address of the base of the fixed-stack allocation for this function */
      frame_pointers*   EstablisherFrame;
      /** instruction address that specifies the continuation address of the unwind */
      void*             TargetIp;
      /** exception context */
      context *         ContextRecord;
      /** language-specific language handler routine being called */
      handler_t *       LanguageHandler;
      /** indicates the location of the PC: if 0, the PC is in the prologue, epilogue, or a null frame region of the function; in the body of the function otherwise */
      rva_t *           HandlerData;
      /** unwind history table */
      unwind_history_table * HistoryTable;
      uint32_t          ScopeIndex;
      uint32_t          Fill0;

      void* va(rva_t rva) const
      {
        return (void*)ImageBase->va(rva);
      }

      template<typename T>
      T va(void* rva) const
      {
        return ImageBase->va<T>(uintptr_t(rva));
      }

      template<typename T>
      T va(rva_t rva) const
      {
        return ImageBase->va<T>(rva);
      }
    };
#endif // _M_X64

    struct record
    {
      static const int maximum_parameters = 15;

      static const ntstatus cxxmagic = status::cxx_exception;//0xE0000000|'msc';
      static const ntstatus commagic = status::com_exception;//0xE0000000|'MOC';
      static const ntstatus longjumpmagic = status::longjump;

      /* 0x00 */  ntstatus  ExceptionCode;
      /* 0x04 */  flags     ExceptionFlags;
      /* 0x08 */  record *  ExceptionRecord;
      /* 0x0C */  void *    ExceptionAddress;
      /* 0x10 */  uint32_t  NumberParameters;
      /* 0x14 */  uintptr_t ExceptionInformation[maximum_parameters];

      bool iscxx() const { return ExceptionCode == cxxmagic; }
      bool islongjump() const { return ExceptionCode == longjumpmagic; }
      bool isconsolidate() const { return ExceptionCode == status::unwind_consolidate; }

      inline void raise() const;
    };

    static void inconsistency();

#ifdef _M_IX86
    /// SEH record
    struct registration
    {
      registration *  next;
      handler_t *     handler;

      static registration * end_of_chain()
      {
        return reinterpret_cast<registration*>(-1);
      }

      uintptr_t stackptr() const
      {
        return reinterpret_cast<const uintptr_t*>(this)[-1];
      }

      uintptr_t & stackptr()
      {
        return reinterpret_cast<uintptr_t*>(this)[-1];
      }

      inline void unwind(const record * = 0) const;

      //_UnwindNestedFrames
      #pragma warning(push)
      // SE handlers already registered should be SAFESEH
      #pragma warning(disable:4733)//Inline asm assigning to 'FS:0' : handler not registered as safe handler
      __declspec(noinline)
      void unwindnestedframes(record * ehrec) const
      {
        _EH_TRACE_ENTER();
        // same cast
        registration * const top = reinterpret_cast<registration*>(teb::get(&teb::ExceptionList));
        unwind(ehrec);
        ehrec->ExceptionFlags &= ~unwinding;
        top->next = reinterpret_cast<registration*>(teb::get(&teb::ExceptionList));
        teb::set(&teb::ExceptionList, reinterpret_cast<uintptr_t>(top));
        _EH_TRACE_LEAVE();
      }
      #pragma warning(pop)
    };
#endif // _M_IX86

    /// exception_info result
    struct pointers
    {
      record*   ExceptionRecord;
      context*  ContextRecord;
    };

}; //class exception

#ifdef _M_X64

NTL__EXTERNAPI
void *__stdcall
  RtlPcToFileHeader(const void *PcValue, void **pBaseOfImage);

#endif

NTL__EXTERNAPI
__noreturn
void __stdcall
RtlRaiseException(
    const exception::record * ExceptionRecord
    );

void exception::record::raise() const
{
  _EH_TRACE_ENTER();
  RtlRaiseException(this);
  _EH_TRACE_LEAVE();
}

}//namespace nt

typedef nt::exception::rva_t              rva_t;
typedef nt::exception::filter             exception_filter;
typedef nt::exception::disposition        exception_disposition;
typedef nt::exception::dispatcher_context dispatcher_context;
typedef nt::exception::flags              exception_flags;
typedef nt::exception::record             exception_record;
typedef nt::exception::handler_t          exception_handler;
typedef nt::exception::pointers           exception_pointers;

//typedef nt::exception::registration exception_registration; see nt::tib::ExceptionList
struct exception_registration
#ifdef _M_IX86
  : public nt::exception::registration
#endif
  {};

/// the functopn is exported by kernel32.dll ntdll.dll & nt.exe
///\warning not to be called from C code since it trashes all registers
NTL__EXTERNAPI
void __stdcall
RtlUnwind(
    const exception_registration *  TargetFrame,    __optional
    const void *                    TargetIp,       __optional ///< not used on x86
    const exception_record *        ExceptionRecord,__optional
    const void *                    ReturnValue
    );

///\todo may be implemented by the __global_unwind2 call
#ifdef _M_IX86
void inline
  unwind(const nt::exception::registration * volatile frame, const exception_record * volatile er)
{
//__asm pusha
  __asm push ebx
  __asm push esi
  __asm push edi
  RtlUnwind(static_cast<const exception_registration*>(frame), 0, er, 0);
  __asm pop edi
  __asm pop esi
  __asm pop ebx
//__asm popa // BHATYPE POPA
}

void nt::exception::registration::unwind(const exception_record * er)
const
{
  //_EH_TRACE_ENTER();
  ntl::unwind(this, er);
  //_EH_TRACE_LEAVE();
}

#endif // _M_IX86


static const exception_flags
  exception_noncontinuable  = nt::exception::noncontinuable,
  exception_unwinding       = nt::exception::unwinding,
  exception_exit_unwind     = nt::exception::exit_unwind,
  exception_stack_invalid   = nt::exception::stack_invalid,
  exception_nested_call     = nt::exception::nested_call,
  exception_target_unwind   = nt::exception::target_unwind,
  exception_collied_unwind  = nt::exception::collied_unwind,
  exception_unwind  = nt::exception::unwind;

/// Exception filter return values
static const exception_filter
  exception_continue_execution  = nt::exception::continue_execution,
  exception_continue_search     = nt::exception::continue_search,
  exception_execute_handler     = nt::exception::execute_handler;

static const exception_disposition
      ExceptionContinueExecution  = nt::exception::ContinueExecution,
      ExceptionContinueSearch     = nt::exception::ContinueSearch,
      ExceptionNestedException    = nt::exception::NestedException,
      ExceptionCollidedUnwind     = nt::exception::CollidedUnwind;

//extern "C" int __cdecl  _abnormal_termination();
//extern "C" ntl::nt::ntstatus __cdecl  _exception_code();
extern "C" exception_pointers * __cdecl _exception_info();

#define _exception_status() static_cast<ntstatus>(_exception_code())

extern "C" void * _ReturnAddress();
#pragma intrinsic(_ReturnAddress)

extern "C"//NTL__EXTERNAPI is not an option because of __declspec(dllimport)
__noreturn
void __stdcall
RaiseException(ntstatus, exception_flags, uint32_t, const uintptr_t *);

#ifndef NTL__WIN_NEW
/// Constructs the exception_record object and sends this to exception_handler
/// through the OS's exception dispatcher
__forceinline
void __stdcall
RaiseException(
    ntstatus          ExceptionCode,
    exception_flags   ExceptionFlags,
    uint32_t          NumberOfArguments,
    const uintptr_t * Arguments)
{
  exception_record er =
  {
    ExceptionCode, ExceptionFlags, 0, _ReturnAddress(),
    !Arguments ? 0 :
      NumberOfArguments > exception_record::maximum_parameters ?
        exception_record::maximum_parameters : NumberOfArguments
  };
  for ( uint32_t i = 0; i != er.NumberParameters; ++i )
    er.ExceptionInformation[i] = Arguments[i];
  er.raise();
}
#endif // NTL__WIN_NEW

/// Constructs the exception_record object and sends this to exception_handler
/// through the OS's exception dispatcher
///\todo use std::tuple
template <typename T, size_t N>
__noreturn
void inline
  raise_exception(
    const ntstatus           code,
    const exception_flags    flags,
    const std::array<T, N> & args)
{
  static_assert(N <= exception_record::maximum_parameters,
                "raise_exception: invalid number of arguments");
  static_assert(sizeof(T) == sizeof(uintptr_t),
                "raise_exception: sizeof shall be equal to sizeof(uintptr_t)");
  RaiseException(code, flags, N, reinterpret_cast<const uintptr_t*>(&args));
}

__noreturn
void inline
  raise_exception(
    const ntstatus        code,
    const exception_flags flags = exception_noncontinuable)
{
  RaiseException(code, flags, 0, 0);
}



namespace cxxruntime {
#ifdef _MSC_VER
  // C++ exception compiler support
  // see some details at http://www.openrce.org/articles/full_view/21

  // wierd names! do not abuse =)

  static const uint32_t ehmagic1994 = 0x01994000;
  static const uint32_t ehmagic1400 = 0x19930522;
  static const uint32_t ehmagic1300 = 0x19930521;
  static const uint32_t ehmagic1200 = 0x19930520;

  static const uint32_t CxxNumberOfParameters =
#ifdef _M_IX86
    3;
#else
    4;
#endif

#ifndef _EH_MAGIC
#if _MSC_VER >= 1400
#define _EH_MAGIC ehmagic1400
#elif _MSC_VER>= 1300
#define _EH_MAGIC ehmagic1300
#else
#define _EH_MAGIC ehmagic1200
#endif
#endif

  /// pointer to member descriptor
  struct ptrtomember // _PMD
  {
    typedef __w64 int32_t  mdiff_t;
    mdiff_t member_offset;
    mdiff_t vbtable_offset; // -1 if not a virtual base
    mdiff_t vdisp_offset;   // offset to the displacement value inside the vbtable

    template<typename T>
    T * operator()(T * const thisptr) const
    {
      uintptr_t tp = reinterpret_cast<uintptr_t>(thisptr);
      uintptr_t ptr = tp + member_offset;
      if ( vbtable_offset != -1 ) // !(vbtable_offset < 0)
      {
        ptr += *reinterpret_cast<mdiff_t*>( static_cast<intptr_t>(vdisp_offset + *reinterpret_cast<mdiff_t*>(tp + vbtable_offset)) )
          + vbtable_offset;
      }
      return reinterpret_cast<T*>(ptr);
    }
  };

  /// The generic UDT to avoid ugly __asm for thiscall
  struct eobject
  {
#ifndef _M_X64
    typedef void (__thiscall eobject::* dtor_ptr )();
    typedef void (__thiscall eobject::* ctor_ptr )(eobject&);
    typedef void (__thiscall eobject::* ctor_ptr2)(eobject&, int);
#else
    typedef void (* dtor_ptr )(eobject*);
    typedef void (* ctor_ptr )(eobject*, eobject*);
    typedef void (* ctor_ptr2)(eobject*, eobject*, int);
#endif
  };
  STATIC_ASSERT(sizeof(eobject::dtor_ptr) == sizeof(void*));

  ///\note the calling convention should not matter since this has no arguments
  typedef void generic_function_t();

  struct catchabletype
  {
    /** is simple type */
    uint32_t    memmoveable : 1;
    /** catchable as reference only */
    uint32_t    refonly     : 1;
    /** class with virtual base */
    uint32_t    hasvirtbase : 1;
#ifndef _M_X64
    /** pointer to the type descriptor */
    type_info * typeinfo;
#else
    /** offset to the type descriptor */
    rva_t       typeinfo;
#endif

    /** catch type instance location within a thrown object  */
    ptrtomember thiscast;
    /** size of the simple type or offset into buffer of \c this pointer for catch object */
    uint32_t    object_size;

    union
    {
#ifndef _M_X64
      eobject::ctor_ptr   copyctor;
      eobject::ctor_ptr2  copyctor2;
#else
      rva_t               copyctor;
      rva_t               copyctor2;
#endif

    };
  };

#pragma pack(push, 4)
  struct catchabletypearray
  {
    uint32_t        size;
#ifndef _M_X64
    catchabletype * type[1];
#else
    rva_t           type[1];
#endif
  };
#pragma pack(pop)

  /// the MSVC's internal _s__ThrowInfo structure representation
#ifdef _M_IX86
  struct throwinfo
  {
    /* 0x00 */  uint32_t  econst    : 1;
    /* 0x00 */  uint32_t  evolatile : 1;
    /* 0x04 */  eobject::dtor_ptr exception_dtor;
    /* 0x08 */  exception_disposition (__cdecl * forwardcompathandler)(...);///\todo safe proto
    /* 0x0C */  ntl::cxxruntime::catchabletypearray *  catchabletypearray; ///< types able to catch the exception.
  };
  STATIC_ASSERT(sizeof(throwinfo) == 0x10);
#endif // _M_IX86

#ifdef _M_X64
#pragma pack(push, 4)
  struct throwinfo
  {
    typedef exception_disposition __cdecl forwardcompathandler_t(...);

    /* 0x00 */  uint32_t  econst    : 1;
    /* 0x00 */  uint32_t  evolatile : 1;
    /* 0x00 */  uint32_t            : 1;
    /* 0x00 */  uint32_t  e8        : 1;
    /* 0x04 */  rva_t     exception_dtor;
    /* 0x08 */  rva_t     forwardcompathandler;
    /* 0x0C */  rva_t     catchabletypearray; ///< types able to catch the exception.
  };
#pragma pack(pop)

#endif // _M_X64

  /// This type represents the catch clause
  struct ehandler
  {
    //  union { uint32_t  adjectives; void * ptr; };
    uint32_t isconst      : 1;
    uint32_t isvolatile   : 1;
    uint32_t isunaligned  : 1;// guess it is not used on x86
    uint32_t isreference  : 1;
    uint32_t              : 2;
    uint32_t unknown7     : 1;

#ifdef _M_X64
    uint32_t              :24;
    uint32_t ishz         : 1;
#endif

#ifndef _M_X64
    /** pointer to the type descriptor of this catch object */
    const type_info *     typeinfo;
    ptrdiff_t             eobject_bpoffset; // 0 = no object (catch by type)
    /** pointer to the catch funclet */
    generic_function_t *  handler;
#else
    /** offset to the type descriptor of this catch object */
    /*0x04*/ rva_t        typeinfo;         // dispType
    /*0x08*/ int          eobject_bpoffset; // dispCatchObj
    /** offset to the catch clause funclet */
    /*0x0C*/ rva_t        handler;          // dispOfHandler
    /*0x10*/ uint32_t     frame;            // dispFrame
#endif

    /// 15.1/3  A throw-expression initializes a temporary object, called
    ///         the exception object, the type of which is determined by
    ///         removing any top-level cv-qualifiers from the static type
    ///         of the operand of throw and adjusting the type from “array of T”
    ///         or “function returning T” to “pointer to T” or “pointer to function returning T”, respectively.
    bool type_match(const catchabletype * ct, const throwinfo * ti, const dispatcher_context* const dispatch) const
    {
      // catch(...) handles them all
#ifdef _M_IX86
      (void)dispatch;
      const type_info *ti1 = this->typeinfo, *ti2 = ct->typeinfo;
#endif
#ifdef _M_X64
      const type_info *ti1 = this->typeinfo ? dispatch->va<type_info*>(this->typeinfo) : NULL,
        *ti2 = ct->typeinfo ? dispatch->va<type_info*>(ct->typeinfo) : NULL;
#endif

      if(is_ellipsis(ti1))
        return true;

      // different TI record with different name?
      if(ti1 != ti2 && std::strcmp(ti1->name(), ti2->name()))
        return false;

      // reference?
      if(ct->refonly && !isreference) return false;
      // check CV cvalifiers?.. Hmmm
      if (ti->econst && !isconst) return false;
      if (ti->evolatile && !isvolatile) return false;
      return true;
    }

    // catch(...) ?
#ifndef _M_X64
    bool is_ellipsis() const
    {
      return !typeinfo || !*typeinfo->name();
    }
#else
    bool is_ellipsis(const dispatcher_context* const dispatch) const
    {
      return !typeinfo || !*dispatch->va<type_info*>(typeinfo)->name();
    }
#endif
    static bool is_ellipsis(const type_info* ti)
    {
      return !ti || !*ti->name();
    }
  };

  typedef int ehstate_t; ///< unwind map index: -2 means `invalid`, -1 means `none`.

  struct unwindtable
  {
    ehstate_t            state;
#ifndef _M_X64
    generic_function_t * unwindfunclet;
#else
    rva_t                unwindfunclet;
#endif
  };


  struct tryblock
  {
    ehstate_t   trylow;
    ehstate_t   tryhigh;
    ehstate_t   catchhigh;
    int         ncatches;
#ifdef _M_IX86
    ehandler *  catchsym;
#else
    rva_t       catchsym;
#endif

    typedef std::pair<const tryblock*, const tryblock*> ranges;
  };

  // have you seen /d1ESrt in TFM? so do I...


#ifdef _M_IX86
  /// 15.4 Exception specifications - internal representation
  struct estypeinfo
  {
    /* 0x00 */  size_t      size;
    /* 0x04 */  ehandler *  ptr;
  };

  struct ehfuncinfo1200 //_s_ESTypeList
  {
    /* 0x00 */  uint32_t        magic : 30;
    /* 0x04 */  ehstate_t       unwindtable_size;
    /* 0x08 */  unwindtable *   unwindtable;
    /* 0x0C */  size_t          tryblocktable_size;
    /* 0x10 */  tryblock *      tryblocktable;
    /* 0x14 */  size_t          _size;
    /* 0x18 */  void *          _;

    tryblock::ranges
      get_try_ranges(int depth, ehstate_t state) const
    {
      const tryblock * end = &tryblocktable[tryblocktable_size];
      tryblock::ranges r(end, end);
      while ( depth >= 0 )
      {
        if ( --r.first < &tryblocktable[0]
        || r.first->tryhigh < state && state <= r.first->catchhigh )
        {
          --depth;
          r.second = end;
          end = r.first;
        }
      }
      ++r.first;
      return r;
    }
  };

  struct ehfuncinfo1300 : public ehfuncinfo1200
  {
    /* 0x1C */  estypeinfo  *   estypeinfo;
  };

  struct ehfuncinfo1400 : public ehfuncinfo1300
  {
    /* 0x20 */  uint32_t        synchronous : 1; // EHFlags
  };

  typedef ehfuncinfo1400 ehfuncinfo;

#endif // _M_IX86
#ifdef _M_X64
  struct estypeinfo
  {
    /* 0x00 */  size_t      size;
    /* 0x04 */  rva_t       typeArray;
  };
  
  struct ehfuncinfo_packed
  {
    /* 0x00 */  uint32_t        magic : 29;
    /* 0x00 */  uint32_t        bbtFlags:3;
    /* 0x04 */  ehstate_t       unwindtable_size;
    /* 0x08 */  rva_t           unwindtable;
    /* 0x0C */  uint32_t        tryblocktable_size;
    /* 0x10 */  rva_t           tryblocktable;
    /* 0x14 */  uint32_t        ipmap_count;
    /* 0x18 */  rva_t           ipmap;
    /* 0x1C */  rva_t           unwindhelp;
    /* 0x20 */  rva_t           estypeinfo;
    /* 0x24 */  uint32_t        synchronous : 1; // EHFlags
  };
  STATIC_ASSERT(sizeof(ehfuncinfo_packed) == 0x28);

  struct ipstateentry
  {
    rva_t       ip;
    ehstate_t   state;
  };

  struct ehfuncinfo
  {
    // compiler version
    uint32_t        magic : 29;
    uint32_t        bbtFlags:3;

    // max_state, unwind map size
    ehstate_t       unwindtable_size;
    // unwind map
    unwindtable*    unwindtable;
    // try block count in this function
    ehstate_t       tryblocktable_size;
    // try block table
    tryblock*       tryblocktable;
    // IP-to-state map size
    uint32_t        ipmap_count;
    // IP-to-state map
    ipstateentry*   ipmap;
    rva_t           unwindhelp;
    // ES type list
    rva_t           estypeinfo;
    // synchronous exception (SEH)
    uint32_t        synchronous : 1; // EHFlags

    ehfuncinfo(const ehfuncinfo_packed& fi, uintptr_t base)
    {
      magic = fi.magic;
      bbtFlags = fi.bbtFlags;
      unwindtable_size = fi.unwindtable_size;
      tryblocktable_size = fi.tryblocktable_size;
      ipmap_count = fi.ipmap_count;
      unwindhelp = fi.unwindhelp;
      estypeinfo = fi.estypeinfo;
      synchronous = fi.synchronous;

      // fix ptrs
      const uintptr_t p = (const uintptr_t)base;
      unwindtable = (ntl::cxxruntime::unwindtable*)(fi.unwindtable + p);
      tryblocktable = (tryblock*)(fi.tryblocktable + p);
      ipmap = (ipstateentry*)(fi.ipmap + p);
    }

    tryblock::ranges
      get_try_ranges(int depth, ehstate_t state) const
    {
      const tryblock * end = &tryblocktable[tryblocktable_size];
      tryblock::ranges r(end, end);
      while ( depth >= 0 )
      {
        if ( --r.first < &tryblocktable[0]
        || r.first->tryhigh < state && state <= r.first->catchhigh )
        {
          --depth;
          r.second = end;
          end = r.first;
        }
      }
      ++r.first;
      return r;
    }

    tryblock::ranges get_try_ranges2(int depth, ehstate_t state) const
    {
      // find block which contain the state
      const tryblock* tb = tryblocktable;
      uint32_t start, end = tryblocktable_size - 1;
      for(start = 0; start <= end; start++){
        if(state >= tb[start].trylow && state <= tb[start].tryhigh)
          break;
      }

      while(depth > 0 && start < end){
        if(state > tb[end].tryhigh && state <= tb[end].catchhigh)
          depth--;
        end--;
      }
      tryblock::ranges r(&tryblocktable[start], &tryblocktable[end+1]);
      assert(r.second >= r.first && r.second <= &tryblocktable[tryblocktable_size]);
      return r;
    }

    tryblock::ranges get_try_ranges3(int /*depth*/, ehstate_t state, const dispatcher_context* const dispatch, bool& found) const
    {
      ehstate_t cs = state_from_control(dispatch);
      if(tryblocktable_size == 0)
        nt::exception::inconsistency();
      found = true;
      const tryblock* tbe = 0;
      ehstate_t i, start = -1, end = -1;
      for(i = tryblocktable_size; i > 0; i--){
        const tryblock* tb = &tryblocktable[i-1];
        if(cs <= tb->tryhigh)
          continue;
        if(cs > tb->catchhigh)
          continue;
        break;
      }
      if(i)
        tbe = &tryblocktable[i-1];
      for(i = 0; i < tryblocktable_size; i++){
        const tryblock* tb = &tryblocktable[i];
        if(tbe){
          if(tb->trylow <= tbe->tryhigh || tb->tryhigh > tbe->catchhigh)
            continue;
        }
        if(state < tb->trylow || state > tb->tryhigh)
          continue;
        if(start == -1)
          start = i;
        end = i+1;
      }
      if(start == -1){
        start = end = 0;
        found = 0;
        return tryblock::ranges(&tryblocktable[tryblocktable_size], &tryblocktable[tryblocktable_size]);
      }
      return tryblock::ranges(&tryblocktable[start], &tryblocktable[end]);
    }


    const tryblock* catch_try_block(ehstate_t state) const
    {
      for(uint32_t itb = tryblocktable_size; itb != 0; itb--){
        const tryblock* tbe = &tryblocktable[itb-1];
        if(state > tbe->tryhigh && state <= tbe->catchhigh)
          return tbe;
      }
      return NULL;
    }

    ehstate_t state_from_ip(const dispatcher_context* const dispatch, const void* ip) const
    {
      const rva_t va_ip = static_cast<rva_t>((uintptr_t)ip - (uintptr_t)dispatch->ImageBase);
      uint32_t i = 0;
      while(i < ipmap_count){
        const ipstateentry* entry = &ipmap[i];
        if(va_ip < entry->ip)
          break;
        i++;
      }
      return i == 0 ? -1 : ipmap[i-1].state;
    }

    ehstate_t state_from_control(const dispatcher_context* const dispatch) const
    {
      return state_from_ip(dispatch, dispatch->ControlPc);
    }
  };

  STATIC_ASSERT(sizeof(ehfuncinfo_packed) == 0x28);

  NTL__EXTERNAPI
    nt::exception::runtime_function*  __stdcall
    RtlLookupFunctionEntry(
    const void* ControlPc,
    void**      ImageBase,
    nt::exception::unwind_history_table* History
    );

  NTL__EXTERNAPI
    void __stdcall
    RtlUnwindEx(
    uintptr_t                         TargetFrame,
    void*                             TargetIp,
    exception_record*                 ExceptionRecord,
    void*                             ReturnValue,
    nt::context*                      OriginalContext,
    nt::exception::unwind_history_table*  HistoryTable
    );

  extern "C" generic_function_t* CxxCallCatchBlock(exception_record* ehrec);
  extern "C" generic_function_t* _CallSettingFrame(void (*unwindfunclet)(), void* frame, int notify_param);

#endif // _M_IX86

  ///\name VC++ _tiddata clone
  struct tiddata
  {
    /** Thread ID */
    nt::legacy_handle tid;

    /** Current exception */
    exception_pointers curexception;
    exception_record* prevER;

    /** SEH translator */
    void* setranslator;
    void* unexpected; // is it must be per-thread?

    /** nested exceptions count */
    uint32_t nestedExcount;
    uint32_t processingThrow;
    /** current exception specifier */
    void* curexspec;

  #ifdef _M_X64
    void* frame_info;
    void* foreignException;
  #endif
  };

  inline tiddata* _initptd()
  {
    nt::teb& teb = nt::teb::instance();
    tiddata* tid = new tiddata();
    tid->tid = teb.ClientId.UniqueThread;
    teb.EnvironmentPointer = tid;
    return tid;
  }

  inline tiddata* _getptd()
  {
    nt::teb& teb = nt::teb::instance();
    tiddata* tid = reinterpret_cast<tiddata*>(teb.EnvironmentPointer);
    if(!tid || tid->tid != teb.ClientId.UniqueThread)
      tid = _initptd();
    return tid;
  }

  inline tiddata* _getptd_noinit()
  {
    nt::teb& teb = nt::teb::instance();
    tiddata* td = reinterpret_cast<tiddata*>(teb.EnvironmentPointer);
    if(td && td->tid != teb.ClientId.UniqueThread) td = nullptr;
    return td;
  }

  inline void _freeptd()
  {
    delete _getptd_noinit();
  }

  struct cxxregistration
#ifdef _M_IX86
    : public nt::exception::registration
#endif
  {
#ifndef _M_X64
    /* 0x08 */  ehstate_t state;
#else
    /* 0x00 */  nt::exception::frame_pointers fp;
#endif

#ifdef _M_IX86
    uintptr_t stackbaseptr() const
    {
      STATIC_ASSERT(sizeof(cxxregistration) == 0xC);
      return reinterpret_cast<uintptr_t>(this + 1);
    }

    ehstate_t current_state(const ehfuncinfo * const  ehfi) const
    {
      // what this stuff is about to do?
      const ehstate_t cs = ehfi->unwindtable_size <= 0x80 ?
        static_cast<int8_t>(this->state) : this->state;
      assert( cs >= -1 );
      assert( cs < ehfi->unwindtable_size );
      return cs;
    }

#endif // _M_IX86
#ifdef _M_X64
    typedef nt::exception::frame_pointers frame_pointers;

    static __forceinline frame_pointers* framepointers(uintptr_t fp, const ehfuncinfo* const ehfi)
    {
      return reinterpret_cast<frame_pointers*>( fp + ehfi->unwindhelp);
    }

    ehstate_t unwind_try_block(uintptr_t /*fp*/, const ehfuncinfo* const ehfi, const dispatcher_context* const dispatch) const
    {
      frame_pointers establisher;
      const_cast<cxxregistration*>(this)->establisherframe(ehfi, dispatch, &establisher);
      //ehstate_t etb1 = *reinterpret_cast<ehstate_t*>( frame + ehfi->unwindhelp + 4 );
      //ehstate_t etb  = framepointers(fp,ehfi)->MemoryStoreFp; // *(eframe+unwindhelp+4)
      frame_pointers* place = framepointers(establisher.FramePointers,ehfi);
      ehstate_t etb = place->MemoryStoreFp;
      return etb;
    }

    void unwind_try_block(ehstate_t state, uintptr_t /*fp*/, const ehfuncinfo* const ehfi, const dispatcher_context* const dispatch)
    {
      frame_pointers establisher;
      establisherframe(ehfi, dispatch, &establisher);
      ehstate_t etb = unwind_try_block(establisher.FramePointers, ehfi, dispatch);
      if(state > etb){
        frame_pointers* place = framepointers(establisher.FramePointers,ehfi);
        place->MemoryStoreFp = state;
      }
    }

    // __GetCurrentState
    ehstate_t current_state(const ehfuncinfo* const ehfi, uintptr_t fp, const dispatcher_context* const dispatch) const
    {
      ehstate_t cs = framepointers(fp,ehfi)->MemoryStackFp;
      if(cs == -2)
        cs = ehfi->state_from_control(dispatch);
      return cs;
    }

    // __SetState
    void current_state(const ehfuncinfo* const ehfi, uintptr_t fp, ehstate_t state)
    {
      //ehstate_t old = framepointers(fp,ehfi)->MemoryStackFp;
      frame_pointers* place = framepointers(fp,ehfi);
      place->MemoryStackFp = state;
    }

    cxxregistration* establisherframe(const ehfuncinfo* const ehfi, const dispatcher_context* const dispatch, frame_pointers* frame) const
    {
      ehstate_t cs = ehfi->state_from_control(dispatch);
      *frame = fp;
      cxxregistration* eframe = reinterpret_cast<cxxregistration*>(frame);

      for(uint32_t i = ehfi->tryblocktable_size; i > 0; --i){
        tryblock* tb = &ehfi->tryblocktable[i-1];
        if(tb->tryhigh >= cs || cs > tb->catchhigh)
          continue;
        void* imagebase;
        nt::exception::runtime_function* rf = RtlLookupFunctionEntry(dispatch->ControlPc, &imagebase, NULL);
        rva_t funcStart = rf->BeginAddress;
        ehandler* catchtable = dispatch->va<ehandler*>(tb->catchsym);
        for(int j = 0; j < tb->ncatches; j++){
          ehandler& eh = catchtable[j];
          if(eh.handler == funcStart){
            uintptr_t f = fp.FramePointers; // [rcx]
            frame->FramePointers = *reinterpret_cast<uintptr_t*>( f + eh.frame );
            return eframe;
          }
        }
      }
      return eframe;
    }

    struct frame_info
    {
      frame_info(void* thrown_object)
      {
        object_ = thrown_object;
        frame_info** info_ = info();
        frame_info* head = *info_ < this ? 0 : *info_;
        next = head;
        *info_ = this;
      }

      void unlink()
      {
        frame_info* head = *info();
        if(this != head)
          nt::exception::inconsistency();
        do{
          if(head == this){
            *info() = next;
            break;
          }
        }while(head = head->next, head);
      }

      static bool find(void* object)
      {
        const frame_info* p = *info();
        while(p){
          if(p->object_ == object)
            return false;
          p = p->next;
        }
        return true;
      }
    protected:
      static frame_info** info() { return reinterpret_cast<frame_info**>(&_getptd()->frame_info); }
    private:
      void* object_;
      frame_info* next;
    };

    void unwindnestedframes(const exception_record* ehrec, const nt::context* ctx, cxxregistration* establishedframe, const void* handler, int state, const ehfuncinfo* ehfi, dispatcher_context* const dispatch, bool recursive);
#endif // _M_X64

    // __CallSettingFrame
    /// thunk to call unwindfunclet
    ///\return the value returned from unwindfunclet
#ifdef _M_X64
    generic_function_t *
      callsettingframe(void (*unwindfunclet)(), cxxregistration* frame, int nlg_notify_param = 0x100)
    {
      return _CallSettingFrame(unwindfunclet, frame, nlg_notify_param);
    }
#else
#pragma warning(push)
#pragma warning(disable:4731)//frame pointer register 'ebp' modified by inline assembly code
    generic_function_t * __stdcall
      callsettingframe(void (*unwindfunclet)(), int /*nlg_notify_param*/ = 0x103)
    {
      _EH_TRACE_ENTER();
      uintptr_t const _ebp = stackbaseptr();
      __asm {
        mov   eax, unwindfunclet
        push  ebp
        mov   ebp, _ebp

        ///\todo These pushs are not necessary (except of ecx) as
        ///     shold be done by compiler. But not sure about all of them.
        push  esi
        push  edi
        push  ebx
        push  ecx

        call  eax;

        pop   ecx
        pop   ebx
        pop   edi
        pop   esi

        pop   ebp
      }
      _EH_TRACE_LEAVE();
#ifdef __ICL
#pragma warning(disable:1011)// missing return statement
#endif
    }
#pragma warning(pop)
#endif // _M_X64

    /// 15.2/3  If a destructor called during stack unwinding exits with an exception,
    ///         nt::exception::inconsistency is called.
    static exception_filter unwindfilter(ntstatus code)
    {
      switch ( code )
      {
      case exception_record::cxxmagic:
        std::unexpected();
      default:
        return exception_continue_search;
      }
    }

    static exception_filter unwindfilter(exception_pointers * ptrs)
    {
      return unwindfilter(ptrs->ExceptionRecord->ExceptionCode);
    }

    // __FrameUnwindToState
    /// 15.2/1  As control passes from a throw-expression to a handler,
    ///         destructors are invoked for all automatic objects constructed
    ///         since the try block was entered. The automatic objects are destroyed
    ///         in the reverse order of the completion of their construction.
    void// __forceinline
      unwind(const dispatcher_context * const dispatch, const ehfuncinfo * const ehfi, ehstate_t to_state = -1) ///< defaults to empty state
    {
      _EH_TRACE_ENTER();
#ifdef _M_IX86
      (void)dispatch;
      _getptd()->processingThrow++;
      for ( ehstate_t cs = current_state(ehfi); cs != to_state; cs = ehfi->unwindtable[cs].state )
      {
        //std::cerr << " cs: " << cs << std::endl;
        assert( cs > -1 );
        __try
        {
          if ( ehfi->unwindtable[cs].unwindfunclet )
          {
            this->state = ehfi->unwindtable[cs].state;
            callsettingframe(ehfi->unwindtable[cs].unwindfunclet);
          }
        }
        __except(unwindfilter(_exception_status()/*_exception_info()*/))
        {
        }
      }
      _getptd()->processingThrow--;

#endif // _M_IX86
#ifdef _M_X64

      // __FrameUnwindToState
      ehstate_t uestate, cs = current_state(ehfi, fp.FramePointers, dispatch);
      assert(cs >= -1 && cs < ehfi->unwindtable_size);
      _getptd()->processingThrow++;
      for(uestate = -1; cs != -1 && cs > to_state; )
      {
        assert( cs > -1 );
        __try {
          unwindtable* ute = &ehfi->unwindtable[cs];
          uestate = ute->state;
          generic_function_t* action = ute->unwindfunclet ? dispatch->va<generic_function_t*>(ute->unwindfunclet) : NULL;
          if(action){
            current_state(ehfi, fp.FramePointers, uestate);
            callsettingframe(action, this);
          }
          //cs &= 0x30; // ??
          cs = uestate;
        }
        __except(unwindfilter(_exception_status())){
          cs = uestate;
        }
      }
      _getptd()->processingThrow--;
      if(cs != -1 && cs > to_state)
        nt::exception::inconsistency();
      current_state(ehfi, fp.FramePointers, cs);

#endif // _M_X64
      _EH_TRACE_LEAVE();
    }

  };//struct cxxregistration

  exception_disposition
    inline
    cxxframehandler(
    exception_record *        const er,
    cxxregistration *         const eframe,
    const nt::context *             const ectx,
    dispatcher_context *      const dispatch,
    const ehfuncinfo *              const ehfi,
    int                             const trylevel      = 0,
    const exception_registration *  const nested_eframe = 0,
    bool                            const destruct      = false);

  //#ifndef NTL_EH_RUNTIME

  struct cxxrecord : public nt::exception::record
  {

    uint32_t get_ehmagic() const
    {
      /* 0x14 */  return (uint32_t)ExceptionInformation[0];
    }

    eobject * get_object() const
    {
      /* 0x18 */  return reinterpret_cast<eobject*>(ExceptionInformation[1]);
    }

    const throwinfo * get_throwinfo() const
    {
      /* 0x1C */  return reinterpret_cast<const throwinfo*>(ExceptionInformation[2]);
    }

#ifdef _M_X64
    uintptr_t get_throwimagebase() const
    {
      /* 0x24 */  return ExceptionInformation[3];
    }

    void* thrown_va(rva_t rva) const
    {
      return (void*)reinterpret_cast<const pe::image*>(get_throwimagebase())->va(rva);
    }

    template<typename T>
    T thrown_va(rva_t rva) const
    {
      return reinterpret_cast<const pe::image*>(get_throwimagebase())->va<T>(rva);
    }
#else
    template<typename T>
    T thrown_va(T va) const { return va; }
#endif

    /// destruct the exception object if it has a destructor
    ///\warning If the destructor throws an exception during the stack unwinding, std::unexpected would be called
    void destruct_eobject(bool cannotthrow = true) const
    {
      _EH_TRACE_ENTER();
      assert(ExceptionCode == cxxmagic && NumberParameters <= maximum_parameters);
      assert(get_throwinfo());
#ifndef _M_X64
      eobject::dtor_ptr const exception_dtor = get_throwinfo()->exception_dtor;
      if ( exception_dtor )
      {
        __try
        {
          (get_object()->*exception_dtor)();
        }
        __except(cannotthrow ? exception_execute_handler : exception_continue_search)
        {
          std::unexpected();
        }
      }
#else
      if(rva_t dtor = get_throwinfo()->exception_dtor){
        __try {
          eobject::dtor_ptr edtor = thrown_va<eobject::dtor_ptr>(dtor);
          edtor(get_object());
          return;
        }
        __except(cannotthrow ? exception_execute_handler : exception_continue_search){
          std::unexpected();
        }
      }
#endif
      _EH_TRACE_LEAVE();
    }

#ifndef _M_X64
    //ExFilterRethrow
    static
      exception_filter
      callcatchblockfilter(exception_pointers * ptrs)
    {
      const cxxrecord & cxxrec = *static_cast<const cxxrecord *>(ptrs->ExceptionRecord);
      return cxxrec.iscxx() && !cxxrec.get_throwinfo() // re-throw?
        ? (_getptd()->processingThrow = 1, exception_execute_handler) : exception_continue_search;
    }

    ///\note no ___security_cookie support yet
    struct catchguard : public exception_registration
    {
      const ehfuncinfo *  funcinfo;
      cxxregistration *   cxxreg;
      int                 catchdepth;
    };
    STATIC_ASSERT(sizeof(catchguard) == 20);


    static
      exception_disposition __cdecl
      catchguardhandler(
      exception_record  *           er,
      nt::exception::registration * establisher_frame,
      nt::context *                 ctx,
      dispatcher_context *          dispatch/*not used*/)
    {
      const catchguard &cg = *static_cast<const catchguard*>(establisher_frame);
      return cxxframehandler(er, cg.cxxreg, ctx, dispatch, cg.funcinfo,
        cg.catchdepth, &cg, false);
    }

    generic_function_t *
      callcatchblock(
      cxxregistration *     const cxxreg,
      const nt::context *   const ctx,
      const ehfuncinfo *    const funcinfo,
      generic_function_t *  const handler,
      int                   const catchdepth,
      unsigned              const nlg_code = 0x100)
    {
      // assume callcatchblockhelper throws
      generic_function_t * continuation = handler;
      const uintptr_t stackptr = cxxreg->stackptr();

      // save the current exception
      exception_pointers* ep = &_getptd()->curexception;
      exception_pointers saved_exception = *ep;
      ep->ExceptionRecord = this;
      ep->ContextRecord = const_cast<nt::context*>(ctx);
      _getptd()->nestedExcount++;

      __try
      {
        __try
        {
          continuation = callcatchblockhelper(cxxreg, funcinfo, handler, catchdepth, nlg_code);
        }
        __except(callcatchblockfilter(_exception_info()))
        {
          continuation = 0;
          _getptd()->processingThrow = 0;
        }
      }
      __finally
      {
        cxxreg->stackptr() = stackptr;
        if ( continuation )
        {
          destruct_eobject(!!_abnormal_termination());
        }
        _getptd()->nestedExcount--;
        // restore saved exception
        ep = &_getptd()->curexception;
        *ep = saved_exception;
      }

      return continuation;
    }


#pragma warning(push)
#pragma warning(disable:4731)//frame pointer register 'ebp' modified by inline assembly code
#pragma warning(disable:4733)//Inline asm assigning to 'FS:0' : handler not registered as safe handler
    // SE handlers already registered should be SAFESEH
    static __noreturn __declspec(noinline)
      void __stdcall jumptocontinuation(generic_function_t * funclet, cxxregistration *cxxreg)
    {
      using namespace nt;
      // unlink SE handler
      exception_registration * const top = teb::get(&teb::ExceptionList);
      teb::set(&teb::ExceptionList, reinterpret_cast<uintptr_t>(top->next));

      const uintptr_t _ebp = cxxreg->stackbaseptr();
      __asm
      {
        mov   ecx,  cxxreg
        mov   eax,  funclet
        mov   esp,  [ecx-4]
        mov   ebp,  _ebp
        // work arround C4740: flow in or out of inline asm code suppresses global optimization
        //jmp   funclet
        push  eax
        ret
      }
    }

    generic_function_t *
      callcatchblockhelper(
      cxxregistration *     const cxxreg,
      const ehfuncinfo *    const funcinfo,
      generic_function_t *  const handler,
      int                   const catchdepth,
      unsigned              const nlg_code)
    {
      #ifdef __ICL
      #pragma warning(disable:906)
      #endif
      #pragma pack(push,1)
      struct jump_thunk
      {
        byte      jmp;
        int32_t  offset;
      };
      static_assert(sizeof(jump_thunk) == 5, "something wrong..");
      #pragma pack(pop)
      #ifdef __ICL
      #pragma warning(default:906)
      #endif

      intptr_t gha = (uintptr_t)&cxxrecord::catchguardhandler;
      const jump_thunk* thunk = reinterpret_cast<jump_thunk*>(gha);
      if(thunk->jmp == 0xE9){
        gha = gha + thunk->offset + sizeof(jump_thunk);
      }

      volatile catchguard guard;
      guard.next        = nt::teb::get(&nt::teb::ExceptionList);
      guard.handler     = reinterpret_cast<nt::exception::handler_t*>(gha);
      guard.funcinfo    = funcinfo;
      guard.cxxreg      = cxxreg;
      guard.catchdepth  = catchdepth + 1;
      nt::teb::set(&nt::teb::ExceptionList, &guard);
      generic_function_t * const continuation = cxxreg->callsettingframe(handler, static_cast<int>(nlg_code));
      nt::teb::set(&nt::teb::ExceptionList, guard.next);
      return continuation;
    }
#pragma warning(pop)
  public:
    eobject* adjust_pointer(void* p1, const catchabletype* const convertable) const
    {
      return reinterpret_cast<eobject*>(convertable->thiscast(p1));
    }

#else // _M_X64
  public:
    // TODO: replace this with convertable->thiscast
    eobject* adjust_pointer(void* p1, const catchabletype* const convertable) const
    {
      intptr_t obj = reinterpret_cast<intptr_t>(p1);
      //const ptrtomember& pmd = *ntl::padd<const ptrtomember*>(convertable, sizeof(void*));
      const ptrtomember& pmd = convertable->thiscast;
      intptr_t ptr2 = (intptr_t)pmd(p1);
      intptr_t ptr = obj + pmd.member_offset;
      if(pmd.vbtable_offset < 0){
        assert(ptr2 == ptr);
        return reinterpret_cast<eobject*>(ptr);
      }

      ptr += *reinterpret_cast<intptr_t*>(obj+pmd.vbtable_offset);
      ptr += *reinterpret_cast<int32_t*>(ptr + pmd.vdisp_offset);
      assert(ptr2 == ptr);
      return reinterpret_cast<eobject*>(ptr);
    }
  public:
#endif // _M_X64

    // ___BuildCatchObject
    /// 15.3/16 When the exception-declaration specifies a class type, a copy
    ///         constructor is used to initialize either the object declared
    ///         in the exception-declaration or,
    ///         if the exception-declaration does not specify a name,
    ///         a temporary object of that type.
    ///\note    This is the question may we optimize out the last case.
    ///\warning If the copy constructor throws an exception, std::unexpected would be called
    void
      constructcatchobject(
      cxxregistration *             cxxreg,
      const ehandler *        const catchblock,
      catchabletype *         const convertable,
      const dispatcher_context* const dispatch
      )
      const
    {
      _EH_TRACE_ENTER();
#ifdef _M_X64
      // build helper
      __try {
        struct typeinfo_t { void* vtbl; void* spare; char name[1]; };
        enum catchable_info { cidefault, cicomplex, civirtual } cinfo = cidefault;

        const typeinfo_t* ti = catchblock->typeinfo ? dispatch->va<typeinfo_t*>(catchblock->typeinfo) : NULL;
        if(ti && *ti->name && (catchblock->eobject_bpoffset || catchblock->ishz)){
          eobject** objplace = catchblock->ishz
            ? reinterpret_cast<eobject**>(cxxreg)
            : reinterpret_cast<eobject**>(catchblock->eobject_bpoffset + cxxreg->fp.FramePointers);
          if(catchblock->isreference){
            // just ref/pointer
            *objplace = adjust_pointer(get_object(), convertable);
          }else if(convertable->memmoveable){
            // POD
            std::memcpy(objplace, get_object(), convertable->object_size);
            if(convertable->object_size == sizeof(void*) && *objplace)
              *objplace = adjust_pointer((void*)*objplace, convertable);
          }else{
            // if copy ctor exists, call it; binary copy otherwise
            if(convertable->copyctor){
              cinfo = convertable->hasvirtbase ? civirtual : cicomplex;
            }else{
              std::memcpy(objplace, (const void*)adjust_pointer(get_object(), convertable), convertable->object_size);
            }
          }
        }
        // end of build helper
        if(cinfo != cidefault){
          eobject* objthis = catchblock->ishz
            ? reinterpret_cast<eobject*>(cxxreg)
            : reinterpret_cast<eobject*>(catchblock->eobject_bpoffset + cxxreg->fp.FramePointers);
          void* copyctor = thrown_va(convertable->copyctor);
          eobject* copyarg = adjust_pointer(get_object(), convertable);
          if(cinfo == cicomplex)
            (eobject::ctor_ptr (copyctor))(objthis, copyarg);
          else
            (eobject::ctor_ptr2(copyctor))(objthis, copyarg, 1);
        }
      }
      __except(cxxregistration::unwindfilter(static_cast<nt::ntstatus>(_exception_code())))
      {
        nt::exception::inconsistency();
      }
#else // _M_X64
      (void)dispatch;
      //assert(catchblock->typeinfo && catchblock->eobject_bpoffset);
      if ( !catchblock->typeinfo ) return;
      if ( !catchblock->eobject_bpoffset ) return;
      void** catchstackframe = reinterpret_cast<void**>(cxxreg->stackbaseptr()
        + catchblock->eobject_bpoffset);
      __try
      {
        if ( catchblock->isreference )
        {
          *catchstackframe = convertable->thiscast(get_object());
        }
        else if ( convertable->memmoveable ) // simple type
        {
          // if the thrown type is a pointer adjust it and copy
          if ( convertable->object_size == sizeof(void*)
            && convertable->thiscast.member_offset )
          {
            *catchstackframe = convertable->thiscast(get_object());
          }
          else
          {
            std::memmove(catchstackframe, get_object(), convertable->object_size);
          }
        }
        else if ( convertable->copyctor ) // UDT with a copy ctor
        {
          eobject* const copy = reinterpret_cast<eobject*>(catchstackframe);
          if ( convertable->hasvirtbase )
            (copy->*convertable->copyctor2)(*get_object(), 1);
          else
            (copy->*convertable->copyctor)(*get_object());
        }
        else // UDT w/o a copy ctor
        {
          std::memmove(catchstackframe, convertable->thiscast(get_object()),
            convertable->object_size);
        }
      }
      __except(cxxregistration::unwindfilter(static_cast<nt::ntstatus>(_exception_code())))
      {
        nt::exception::inconsistency();
      }
#endif
      _EH_TRACE_LEAVE();
    }

    void catchit (
      cxxregistration *               const cxxreg,
      const nt::context *             const ctx,
      dispatcher_context *            const dispatch,
      const ehfuncinfo *              const funcinfo,
      const ehandler *                const catchblock,
      catchabletype *                 const convertable,
      const tryblock *                const tb,
      int                             const catchdepth,
      const exception_registration *  const nested,
      /* bool                               rethrow, // not used */
      bool                                  recursive
      )
    {
      _EH_TRACE_ENTER();
#ifdef _M_X64
      (void)nested; // unreferenced parameter
      (void)catchdepth;
      cxxregistration::frame_pointers xframe;
      cxxregistration* cxxframe = cxxreg->establisherframe(funcinfo, dispatch, &xframe);
#else
      (void)recursive;
      cxxregistration* const cxxframe = cxxreg;
#endif

      //  15.3/7  A handler is considered active when initialization is complete
      //          for the formal parameter (if any) of the catch clause.
      //          [ Note: the stack will have been unwound at that point]
      if(convertable)
        constructcatchobject(cxxframe, catchblock, convertable, dispatch);

      // destroy auto objects in the try scope
#ifndef _M_X64
      if ( nested ) nested->unwindnestedframes(this);
      else          cxxreg->unwindnestedframes(this);
      cxxreg->unwind(dispatch, funcinfo, tb->trylow);

      cxxreg->state = tb->tryhigh + 1;
      generic_function_t * const continuation =
        callcatchblock(cxxreg, ctx, funcinfo, catchblock->handler, catchdepth);
        //CxxCallCatchBlock(this, cxxreg, ctx, funcinfo, catchblock->handler, catchdepth);
      if ( continuation )
        jumptocontinuation(continuation, cxxreg);
#else
      cxxreg->unwindnestedframes(this, ctx, cxxframe, dispatch->va(catchblock->handler), tb->trylow, funcinfo, dispatch, recursive);
#endif
      _EH_TRACE_LEAVE();
    }

    bool is_msvc(bool flexible = false) const
    {
      const uint32_t magic = get_ehmagic();
      return ExceptionCode == cxxmagic && NumberParameters == CxxNumberOfParameters && (flexible ? (magic >= ehmagic1200 && magic <= ehmagic1400) : (magic == _EH_MAGIC));
    }

#ifdef _M_X64
    bool is_inexceptionSpec(const estypeinfo* pESTypeList, const dispatcher_context* const dispatch) const
    {
      if(!pESTypeList)
        nt::exception::inconsistency();

      const throwinfo* const ti = get_throwinfo();
      const ehandler* handlers = dispatch->va<const ehandler*>(pESTypeList->typeArray);
      const catchabletypearray* const catchables = thrown_va<catchabletypearray*>(ti->catchabletypearray);

      for(uint32_t i = 0; i < pESTypeList->size; i++){
        for(uint32_t j = 0; j < catchables->size; j++){
          const catchabletype* ct = dispatch->va<const catchabletype*>(catchables->type[j]);
          if(handlers[j].type_match(ct, ti, dispatch))
            return true;
        }
      }
      return false;
    }
#endif

    exception_record* current_exception() const
    {
      return _getptd()->curexception.ExceptionRecord;
    }

    void set_current_exception()
    {
      _getptd()->curexception.ExceptionRecord = this;
    }
#ifdef _M_X64
    void find_handler2(
      cxxregistration *       const ereg,
      const nt::context *           ctx,
      dispatcher_context *    const dispatch,
      const ehfuncinfo *      const ehfi,
      const bool                    recursive,
      const ehstate_t               trylevel,
      const exception_registration *const nested_eframe,
      ehstate_t cs
      )
    {
      bool catched = false;

      // not cxx: is_cxx2
      bool is_cxx = is_msvc(true);
      if(is_cxx){
        //__debugbreak();
        if(ehfi->tryblocktable_size){
          // _GetRangeOfTrysToCheck
          ///*const */tryblock::ranges ranges = ehfi->get_try_ranges(trylevel, cs);
          //const tryblock::ranges r2 = ehfi->get_try_ranges2(trylevel, cs);
          bool tryfound;
          const tryblock::ranges ranges = ehfi->get_try_ranges3(trylevel, cs, dispatch, tryfound);
          //assert(ranges == r2 && ranges == r3);
          //ranges = r3;

          const tryblock* tb = ranges.first;
          const throwinfo* const ti = get_throwinfo();
          const catchabletypearray* const clist = thrown_va<catchabletypearray*>(ti->catchabletypearray);
          for(; tb < ranges.second; tb++){
            if(tb->trylow <= cs && cs <= tb->tryhigh){
              const ehandler* eh = dispatch->va<ehandler*>(tb->catchsym);
              for(int catches = tb->ncatches; catches != 0; catches--, eh++){

                //const char* catch_block_name = dispatch->va<type_info*>(eh->typeinfo)->name();

                for(uint32_t catchables = 0; catchables < clist->size; catchables++){
                  rva_t catchable = clist->type[catchables];
                  catchabletype* ct = dispatch->va<catchabletype*>(catchable);

                  //const char* thrown_cast_name = dispatch->va<type_info*>(ct->typeinfo)->name();

                  if(eh->type_match(ct, ti, dispatch)){
                    // OK.  We finally found a match.  Activate the catch.
                    catched = true;
                    catchit(ereg, ctx, dispatch, ehfi, eh, ct, tb, trylevel, nested_eframe, recursive);
                    goto next_try;
                  }

                }
              }
            }
next_try:;
          }
        }

        // tryEmpty:
        if(catched == false && ehfi->magic >= ehmagic1300){
          const estypeinfo* ehtable = ehfi->estypeinfo ? dispatch->va<estypeinfo*>(ehfi->estypeinfo) : 0;
          if(ehtable){
            __debugbreak();
            if(is_inexceptionSpec(ehtable, dispatch)){
              uintptr_t frame;
              cxxregistration* esframe = ereg->establisherframe(ehfi, dispatch, reinterpret_cast<cxxregistration::frame_pointers*>(&frame));
              // TODO: _ptd()->_pExitContext = {exitContext, ereg, 0, frame}
              ereg->unwindnestedframes(this, ctx, esframe, NULL, -1, ehfi, dispatch, recursive);
            }

          }
        }
      }else if(ehfi->tryblocktable_size){
        if(recursive)
          nt::exception::inconsistency();
        // FindHandlerForForeignException
        // _GetRangeOfTrysToCheck
        const tryblock::ranges ranges = ehfi->get_try_ranges(trylevel, cs);
        const tryblock* tb = ranges.first;
        for(; tb < ranges.second; tb++){
          if(tb->trylow <= cs && cs <= tb->tryhigh && tb->ncatches){
            // get last catchsym (the last catch in that try is an ellipsis (no other can be))
            const ehandler& eh = dispatch->va<ehandler*>(tb->catchsym)[tb->ncatches-1];
            if(eh.is_ellipsis(dispatch)){
              // OK.  We finally found a match.  Activate the catch.
              catched = true;
              catchit(ereg, ctx, dispatch, ehfi, &eh, NULL, tb, trylevel, nested_eframe, recursive);
            }
          }
        }
      }
    }
#endif

    /// 15.1/2  When an exception is thrown, control is transferred
    ///         to the nearest handler with a matching type (15.3)
    ///\return  does not return on handled exceptions.
    void find_handler(
      cxxregistration *       const ereg,
      const nt::context *           ctx,
      dispatcher_context *    const dispatch,
      const ehfuncinfo *      const ehfi,
      const bool                    recursive,
      const ehstate_t               trylevel,
      const exception_registration *const nested_eframe)
    {
      _EH_TRACE_ENTER();
#ifdef _M_IX86
      ehstate_t cs = ereg->current_state(ehfi);
#endif
#ifdef _M_X64
      // get the current state
      ehstate_t cs = ehfi->state_from_control(dispatch);

      // get establisher frame
      cxxregistration::frame_pointers establisher;
      ereg->establisherframe(ehfi, dispatch, &establisher);

      // unwind try block
      ehstate_t etb = ereg->unwind_try_block(ereg->fp.FramePointers /*frame.FramePointers*/, ehfi, dispatch);
      if(cs > etb){
        ereg->current_state(ehfi, establisher.FramePointers, cs);
        ereg->unwind_try_block(cs, ereg->fp.FramePointers/*frame.FramePointers*/, ehfi, dispatch);
      }else{
        cs = etb;
      }

      assert(cs >= -1 && cs < ehfi->unwindtable_size);
      if(cs < -1 || cs >= ehfi->unwindtable_size)
        nt::exception::inconsistency();

      bool isRethrow = false;
      cxxrecord* cxxrec = this;

      // is_cxx1
      if(is_msvc(true) && !get_throwinfo()){

        // check the current exception
        if(!current_exception())
          return;
        //return;

        // restore exception information
        isRethrow = true;
        static_assert(sizeof(*this) == sizeof(exception_record), "this types must be equal");
        cxxrec = static_cast<cxxrecord*>(_getptd()->curexception.ExceptionRecord);
        assert(cxxrec->NumberParameters <= exception_record::maximum_parameters);
        ctx = _getptd()->curexception.ContextRecord;

        if(cxxrec->iscxx() && !cxxrec->get_throwinfo())
          nt::exception::inconsistency();

        //f108_t* ptd_108 = 0;
        //if(ptd_108){
        //  f108_t* f108 = 0;
        //  std::swap(ptd_108, f108);
        //  if(!is_inexceptionSpec(f108, dispatch)){

        //  }
        //}

        //if(ptd.field108){
        //  auto f108 = 0;
        //  swap(ptd.field_108, f108);
        //  if(!IsInExceptionSpec(pair<count, ehandlers_rva>* &f108, dispatch)){
        //    if(!Is_bad_exception_allowed)
        //      nt::exception::inconsistency();
        //    __DestructExceptionObject(erec, true);
        //    throw std::bad_exception();
        //  }
        //}
        //return;
      }
      cxxrec->find_handler2(ereg, ctx, dispatch, ehfi, recursive, trylevel, nested_eframe, cs);
      return;
#endif
#ifdef _M_IX86
      if(iscxx()) {
        const uint32_t ehmagic = get_ehmagic();
        if(NumberParameters == CxxNumberOfParameters
#ifdef NTL__OTHEREHMAGICS
          && ehmagic >= ehmagic1200 && ehmagic <= ehmagic1400
#else
          && ehmagic == _EH_MAGIC
#endif
          )
        {
          const throwinfo * ti = get_throwinfo();
          cxxrecord* cxxrec = this;
          //__debugbreak();
          if ( !ti ) // rethrow?
          {
            cxxrec = static_cast<cxxrecord*>(_getptd()->curexception.ExceptionRecord);
            assert(cxxrec->NumberParameters <= exception_record::maximum_parameters);
            ctx = _getptd()->curexception.ContextRecord;
            ti = cxxrec->get_throwinfo();

            if(cxxrec->iscxx() && !cxxrec->get_throwinfo())
              nt::exception::inconsistency();

            // MSVC keeps previous thrown object in the per-thread data...
            // we just return to continue search in the containing scope.
            //return;
          }
          for ( tryblock::ranges tr = ehfi->get_try_ranges(trylevel, cs);
            tr.first < tr.second;
            ++tr.first )
          {
            const tryblock* const tb = tr.first;
            if ( tb->trylow <= cs && cs <= tb->tryhigh )
              for ( int c = 0; c < tb->ncatches; ++c ){ // iterate through catch blocks
                ehandler* eh = &tb->catchsym[c];
                //const char* catch_block_name = eh->typeinfo->name();

                for ( unsigned i = 0; i < ti->catchabletypearray->size; ++i ){  // iterate through list of possibly casts of the thrown object
                  //const char* thrown_cast_name = ti->catchabletypearray->type[i]->typeinfo->name();

                  if(eh->type_match(ti->catchabletypearray->type[i], ti, dispatch)) // catch block <=> cast type of the thrown object
                  {
                    cxxrec->catchit(ereg, ctx, dispatch, ehfi, eh,
                      ti->catchabletypearray->type[i], tb, trylevel, nested_eframe, recursive);
                    // rethrown excetion
                    goto next_try;
                  }
                }
next_try: ;
              }
          }
          if ( recursive )
            destruct_eobject();
        }
        //return;
      }else if(ehfi->tryblocktable_size > 0 && !recursive) {
        // TODO: SEH translation
        for ( tryblock::ranges tr = ehfi->get_try_ranges(trylevel, cs);
          tr.first < tr.second;
          ++tr.first )
        {
          const tryblock* const tb = tr.first;
          if ( tb->trylow <= cs && cs <= tb->tryhigh)
          {
            if(tb->ncatches){
              // get last catchsym (the last catch in that try is an ellipsis (no other can be))
              const ehandler& eh = tb->catchsym[tb->ncatches-1];
              // check for ellipsis
              if(eh.is_ellipsis()){
                // ok, it can catch exception
                catchit(ereg, ctx, dispatch, ehfi, &eh, NULL, tb, trylevel, nested_eframe, recursive);
                // rethrown exception
              }
            }
          }
        }
      }else {
        nt::exception::inconsistency();
      }
#endif
      _EH_TRACE_LEAVE();
    }
  };


  /// C++ exception_handler implementation.
  ///\note  This is called by __CxxFrameHandler3 which has to be compiled w/o /GL,
  ///       so avoid __forceinline here to reduce the size of all this huge stuff
  ///       (MSVC has wierd heuristics to ignore inline, so hack arround)

#ifdef _M_IX86

  exception_disposition
    __declspec(noinline)
    cxxframehandler(
    exception_record *        const er,       ///< thrown NT exception
    cxxregistration *         const eframe,   ///< SEH frame ptr
    const nt::context *             const ectx,
    dispatcher_context *      const dispatch,
    const ehfuncinfo *              const ehfi,     ///< registered handlers
    int                             const trylevel      /*= 0*/,
    const exception_registration *  const nested_eframe /*= 0*/,
    bool                            const destruct      /*= false*/)
  {
    if ( !er->iscxx() )
    {
      // however, /EHa allows to catch(...) an arbitrary exception
      //#if _MSC_VER >= 1400
      if ( true
#ifdef NTL__OTHEREHMAGICS
        && ehfi->magic >= ehmagic1400
#endif
#pragma warning(push)
#pragma warning(disable:4127)//conditional expression is constant
        && sizeof(ehfuncinfo) >= sizeof(ehfuncinfo1400) && ehfi->synchronous )
#pragma warning(pop)
        //#endif//MSVC 14
//        _EH_TRACE_LEAVE();
        return ExceptionContinueSearch;
    }
    _EH_TRACE_ENTER();
    // check a call by RtlUnwind
    if ( er->ExceptionFlags & nt::exception::unwind )
    {
      if ( er->iscxx() && ehfi->unwindtable_size && trylevel == 0 )
      {
        eframe->unwind(dispatch, ehfi);
      }
      _EH_TRACE_LEAVE();
      // async exceptions are to be unwinded by __except_handler3
      return ExceptionContinueSearch;
    }
    // handle the exception if possible
    if ( ehfi->tryblocktable_size )
    {
      // the cast is not always safe since there may be an arbitrary exception.
      // I assume find_handler will check this.
      cxxrecord * const cxxer = static_cast<cxxrecord*>(er);
#ifdef NTL__OTHEREHMAGICS
      if ( cxxer->iscxx()
        && cxxer->get_ehmagic() > ehmagic1400
        && cxxer->NumberParameters >= CxxNumberOfParameters /**\see _CxxThrowException args*/
        && cxxer->get_throwinfo()->forwardcompathandler )
      {
//        _EH_TRACE_LEAVE();
        return cxxer->get_throwinfo()->forwardcompathandler
          (er, eframe, ectx, dispatch, ehfi, trylevel, nested_eframe, destruct);
      }
      else
#endif
      {
        cxxer->find_handler(eframe, ectx, dispatch, ehfi, destruct, trylevel, nested_eframe);
      }
    }
    _EH_TRACE_LEAVE();
    return ExceptionContinueSearch;
  }

#endif // _M_IX86

  //#endif #ifndef NTL_EH_RUNTIME

  #endif//#ifdef _MSC_VER
} // cxxruntime
}//namespace ntl

#ifdef __ICL
#pragma warning(default:2259)
#endif

#endif//#ifndef NTL__NT_EXCEPTION
