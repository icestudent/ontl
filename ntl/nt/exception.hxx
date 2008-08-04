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

#include "basedef.hxx"
#include "teb.hxx"
#include "../stlx/array.hxx"
#include "../stlx/typeinfo.hxx"

#include "../pe/image.hxx"

#undef exception_code
#undef exception_info
#undef abnormal_termination

#define exception_code              ::_exception_code()
#define exception_info()            reinterpret_cast<struct ntl::nt::exception::pointers*>(::_exception_info())
#define abnormal_termination        (::_abnormal_termination() != 0)

extern "C" 
{
  unsigned long __cdecl _exception_code(void);
  void *        __cdecl _exception_info(void);
  int           __cdecl _abnormal_termination(void);
}

#ifdef _DEBUG
  void dbg_pause();
# define dbgpause() dbg_pause()
#else
# define dbg_pause()
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

    /**
     *	@brief Unwind data information structure 
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

    union frame_pointers
    {
      struct 
      {
        rva_t MemoryStackFp;
        rva_t MemoryStoreFp;
      };
      uintptr_t FramePointers;
    };

    
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
      uint64_t          ScopeIndex;
      uint64_t          Fill0;

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
        // same cast
        registration * const top = reinterpret_cast<registration*>(teb::get(&teb::ExceptionList));
        unwind(ehrec);
        ehrec->ExceptionFlags &= ~unwinding;
        top->next = reinterpret_cast<registration*>(teb::get(&teb::ExceptionList));
        teb::set(&teb::ExceptionList, reinterpret_cast<uintptr_t>(top));
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
__declspec(noreturn)
void __stdcall
RtlRaiseException(
    const exception::record * ExceptionRecord
    );

void exception::record::raise() const
{
  RtlRaiseException(this);
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
  ntl::unwind(this, er);
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

extern "C" int __cdecl  _abnormal_termination();
extern "C" unsigned long __cdecl  _exception_code();
extern "C" exception_pointers * __cdecl _exception_info();

#define _exception_status() static_cast<ntstatus>(_exception_code())

extern "C" void * _ReturnAddress();
#pragma intrinsic(_ReturnAddress)

extern "C"//NTL__EXTERNAPI is not an option because of __declspec(dllimport)
__declspec(noreturn)
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
__declspec(noreturn)
void inline
  raise_exception(
    const ntstatus           code,
    const exception_flags    flags,
    const std::array<T, N> & args)
{
  static_assert(N <= exception_record::maximum_parameters,
                __name__" : invalid number of arguments");
  static_assert(sizeof(T) == sizeof(uintptr_t),
                __name__" : sizeof shall be equal to sizeof(uintptr_t)");
  RaiseException(code, flags, N, reinterpret_cast<const uintptr_t*>(&args));
}

__declspec(noreturn)
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
        ptr += *reinterpret_cast<mdiff_t*>(vdisp_offset +
          *reinterpret_cast<mdiff_t*>(tp + vbtable_offset))
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
    type_info *       typeinfo;
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
      eobject::ctor_ptr copyctor; 
      eobject::ctor_ptr2 copyctor2; 
#else
      rva_t           copyctor;
      rva_t           copyctor2;
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

#ifdef _M_X64
uint32_t              :27;
    uint32_t ishz         : 1;
#endif

#ifdef _M_IX86
    /** pointer to the type descriptor of this catch object */
    const type_info *     typeinfo;
    ptrdiff_t             eobject_bpoffset; // 0 = no object (catch by type)
    /** pointer to the catch funclet */
    generic_function_t *  handler;
#endif
#ifdef _M_X64
    /** offset to the type descriptor of this catch object */
    rva_t                 typeinfo;
    int                   eobject_bpoffset;
    /** offset to the catch clause funclet */
    rva_t                 handler;
    uint32_t              reserved;
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
      dispatch;
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

  typedef int ehstate_t; ///< unwind map index

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

  /// 15.4 Exception specifications - internal representation
  struct estypeinfo
  {
    /* 0x00 */  size_t      size;
    /* 0x04 */  ehandler *  ptr;
  };

#ifdef _M_IX86
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

  struct ehfuncinfo_packed
  {
    /* 0x00 */  uint32_t        magic : 30;
    /* 0x04 */  ehstate_t       unwindtable_size;
    /* 0x08 */  rva_t           unwindtable;
    /* 0x0C */  uint32_t        tryblocktable_size;
    /* 0x10 */  rva_t           tryblocktable;
    /* 0x14 */  uint32_t        ipmap_count;
    /* 0x18 */  rva_t           ipmap;
    /* 0x1C */  rva_t           estypeinfo;
    /* 0x20 */  uint32_t        reserved;
    /* 0x24 */  uint32_t        synchronous : 1; // EHFlags
  };

  struct ipstateentry
  {
    rva_t       ip;
    ehstate_t   state;
  };

  struct ehfuncinfo
  {
    uint32_t        magic : 30;
    ehstate_t       unwindtable_size;
    unwindtable*    unwindtable;
    uint32_t        tryblocktable_size;
    tryblock*       tryblocktable;
    uint32_t        ipmap_count;
    ipstateentry*   ipmap;
    rva_t           estypeinfo;
    uint32_t        reserved;
    uint32_t        synchronous : 1; // EHFlags

    ehfuncinfo(const ehfuncinfo_packed& fi, uintptr_t base)
    {
      magic = fi.magic;
      unwindtable_size = fi.unwindtable_size;
      tryblocktable_size = fi.tryblocktable_size;
      ipmap_count = fi.ipmap_count;
      synchronous = fi.synchronous;
      estypeinfo = fi.estypeinfo;

      // fix ptrs
      const uintptr_t p = (const uintptr_t)base;
      unwindtable = (ntl::cxxruntime::unwindtable*)(fi.unwindtable + p);
      tryblocktable = (tryblock*)(fi.tryblocktable+ p);
      ipmap = (ipstateentry*)(fi.ipmap+p);
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
      for(uint32_t i = 0; i < ipmap_count; i++){
        const ipstateentry* entry = &ipmap[i];
        const void* p = dispatch->va<const void*>(entry->ip);
        if(ip < p)
          return ipmap[i-1].state;
      }
      return -1;
    }

    ehstate_t state_from_control(const dispatcher_context* const dispatch) const
    {
      return state_from_ip(dispatch, dispatch->ControlPc);
    }

  };

  NTL__EXTERNAPI
    nt::exception::runtime_function*  __stdcall
    RtlLookupFunctionEntry(
    const void* ControlPc,
    void**      ImageBase,
    nt::exception::unwind_history_table** History
    );

  NTL__EXTERNAPI
    void __stdcall
    RtlUnwindEx(
    nt::exception::frame_pointers     TargetFrame,
    void*                             TargetIp,
    exception_record*                 ExceptionRecord,
    void*                             ReturnValue,
    nt::context*                      OriginalContext,
    nt::exception::unwind_history_table*  HistoryTable
    );

  extern "C" generic_function_t* __CxxCallCatchBlock(void* unknown);
  extern "C" generic_function_t* _CallSettingFrame(void (*unwindfunclet)(), void* frame, int notify_param);

#endif // _M_IX86

  /* BTW, per-thread data returned by MSVC's _getptd()

  +0x78 - terminate handler
  +0x7C - unexpected handler
  +0x80 - SE translator
  */


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
      //STATIC_ASSERT(sizeof(cxxregistration) == 0xC || sizeof(cxxregistration) == 20);
      return reinterpret_cast<uintptr_t>(this + 1);
    }

    ehstate_t current_state(const ehfuncinfo * const  ehfi) const
    {
      // what this stuff is abaut to do?
      const ehstate_t cs = ehfi->unwindtable_size <= 0x80 ?
        static_cast<int8_t>(this->state) : this->state;
      _Assert( cs > -1 );
      _Assert( cs < ehfi->unwindtable_size );
      return cs;
    }

#endif // _M_IX86
#ifdef _M_X64

    ehstate_t current_state(const ehfuncinfo* const ehfi, const dispatcher_context* const dispatch) const
    {
      ehstate_t cs = *reinterpret_cast<ehstate_t*>(fp.FramePointers + ehfi->estypeinfo);
      if(cs == -2)
        cs = ehfi->state_from_control(dispatch);
      return cs;
    }

    // __SetState
    void current_state(const ehfuncinfo* const ehfi, ehstate_t state)
    {
      int*** ppp = reinterpret_cast<int***>(this);
      ehstate_t old = *reinterpret_cast<ehstate_t*>(fp.FramePointers + ehfi->estypeinfo);
      if(old < 16)
        *reinterpret_cast<ehstate_t*>(fp.FramePointers + ehfi->estypeinfo) = state;
      else
        dbgpause();
    }

    uintptr_t establisherframe(const ehfuncinfo* const ehfi, const dispatcher_context* const dispatch, uintptr_t* frame)
    {
      ehstate_t cs = ehfi->state_from_control(dispatch);
      *frame = fp.FramePointers;

      int*** ppthrown = reinterpret_cast<int***>(this);

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
            *frame = fp.FramePointers + 0x10; // sizeof(ehandler?)
            return reinterpret_cast<uintptr_t>(frame);
          }
        }
      }
      return reinterpret_cast<uintptr_t>(frame);
    }

    struct frame_info
    {
      frame_info(void* thrown_object)
      {
        object_ = thrown_object;
        next = info_;
        info_ = this;
      }

      void unlink()
      {
        frame_info* head = info_;
        if(this != head)
          std::terminate();
        do{
          if(head == this){
            info_ = next;
            break;
          }
        }while(head = head->next, head);
      }

      static bool find(void* object)
      {
        frame_info* p = info_;
        while(p){
          if(p->object_ == object)
            return true;
        }
        return false;
      }
    private:
      void* object_;
      frame_info* next;
      static frame_info* info_;
    };

    void unwindnestedframes(const exception_record* ehrec, const nt::context* ctx, uintptr_t establishedframe, const void* handler, int state, const ehfuncinfo* ehfi, dispatcher_context* const dispatch, bool recursive);
#endif // _M_X64

    // __CallSettingFrame
    /// thunk to call unwindfunclet
    ///\return the value returned from unwindfunclet
#ifdef _M_X64
    generic_function_t *
      callsettingframe(void (*unwindfunclet)(), cxxregistration* frame, int nlg_notify_param = 0x103)
    {
      return _CallSettingFrame(unwindfunclet, frame, nlg_notify_param);
    }
#else
#pragma warning(push)
#pragma warning(disable:4731)//frame pointer register 'ebp' modified by inline assembly code
    generic_function_t *
      callsettingframe(void (*unwindfunclet)(), int /*nlg_notify_param*/ = 0x103)
    {
      uintptr_t const _ebp = stackbaseptr();
      __asm mov   eax, unwindfunclet
        __asm push  ebp
        __asm mov   ebp, _ebp
        __asm call  eax;
      __asm pop   ebp
    }
#pragma warning(pop)
#endif // _M_X64

    /// 15.2/3  If a destructor called during stack unwinding exits with an exception,
    ///         std::terminate is called.
    static exception_filter unwindfilter(ntstatus code)
    {
      switch ( code )
      {
      case exception_record::cxxmagic:
        std::terminate();
      case exception_record::commagic:
        /**/
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
#ifdef _M_IX86
      dispatch;
      for ( ehstate_t cs = current_state(ehfi); cs != to_state; cs = ehfi->unwindtable[cs].state )
      {
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
#endif // _M_IX86
#ifdef _M_X64
      if(to_state == -1){
        // __FrameUnwindToEmptyState
        uintptr_t frame;
        establisherframe(ehfi, dispatch, &frame);
        ehstate_t state = ehfi->state_from_control(dispatch);
        const tryblock* tb = ehfi->catch_try_block(state);
        to_state = tb ? tb->tryhigh : -1;
      }
      // __FrameUnwindToState
      ehstate_t uestate, cs;
      for(uestate = -1, cs = current_state(ehfi, dispatch); cs != -1 && cs > to_state; )
      {
        __try {
          unwindtable* ute = &ehfi->unwindtable[cs];
          uestate = ute->state;
          generic_function_t* action = ute->unwindfunclet ? dispatch->va<generic_function_t*>(ute->unwindfunclet) : NULL;
          if(action){
            current_state(ehfi, uestate);
            callsettingframe(action, this);
          }
          cs &= 0x30; // ??
        }
        __except(unwindfilter(_exception_info())){
          cs = uestate;
        }
      }
      if(cs != -1 && cs > to_state)
        std::terminate();
      current_state(ehfi, cs);

#endif // _M_X64
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
#endif

    /// destruct the exception object if it has a destructor
    void destruct_eobject(bool cannotthrow = true) const
    {
      _Assert(ExceptionCode == cxxmagic);
      _Assert(get_throwinfo());
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
          std::terminate();
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
          std::terminate();
        }
      }
#endif
    }

    //ExFilterRethrow
    static
      exception_filter
      callcatchblockfilter(exception_pointers * ptrs)
    {
      const cxxrecord & cxxrec = *static_cast<const cxxrecord *>(ptrs->ExceptionRecord);
      return cxxrec.iscxx() && !cxxrec.get_throwinfo() // re-throw?
        ? exception_execute_handler : exception_continue_search;
    }

    ///\note no ___security_cookie support yet
    struct catchguard : public exception_registration
    {
      const ehfuncinfo *  funcinfo;
      cxxregistration *   cxxreg;
      int                 catchdepth;
    };
    //STATIC_ASSERT(sizeof(catchguard) == 28);

#ifndef _M_X64

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
      const nt::context *   const /*ctx*/,
      const ehfuncinfo *    const funcinfo,
      generic_function_t *  const handler,
      int                   const catchdepth,
      unsigned              const nlg_code = 0x100) 
    {
      // assume callcatchblockhelper throws
      generic_function_t * continuation = 0;
      const uintptr_t stackptr = cxxreg->stackptr();
      __try
      {
        __try
        {
          continuation = callcatchblockhelper(cxxreg, funcinfo, handler, catchdepth, nlg_code);
        }
        __except(callcatchblockfilter(_exception_info()))
        {
        }
      }
      __finally
      {
        cxxreg->stackptr() = stackptr;
        if ( continuation )
        {
          destruct_eobject(!!_abnormal_termination());
        }
      }
      return continuation;
    }


#pragma warning(push)
#pragma warning(disable:4731)//frame pointer register 'ebp' modified by inline assembly code
#pragma warning(disable:4733)//Inline asm assigning to 'FS:0' : handler not registered as safe handler
    // SE handlers already registered should be SAFESEH
    __declspec(noreturn)
      static void jumptocontinuation(generic_function_t * funclet, cxxregistration *cxxreg)
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
      catchguard guard;
      guard.next        = nt::teb::get(&nt::teb::ExceptionList);
      guard.handler     = catchguardhandler;
      guard.funcinfo    = funcinfo;
      guard.cxxreg      = cxxreg;
      guard.catchdepth  = catchdepth + 1;
      nt::teb::set(&nt::teb::ExceptionList, &guard);
      generic_function_t * const continuation = cxxreg->callsettingframe(handler, nlg_code);
      nt::teb::set(&nt::teb::ExceptionList, guard.next);
      return continuation;
    }
#pragma warning(pop)
#else // _M_X64
  private:
    eobject* adjust_pointer(void* p1, void* p2) const
    {
      intptr_t up1 = reinterpret_cast<intptr_t>(p1);
      int32_t* up2 = reinterpret_cast<int32_t*>( (char*)p2 + sizeof(void*) );
      intptr_t ptr = up1 + up2[0];
      if(up2[1] < 0)
        return reinterpret_cast<eobject*>(ptr);

      ptr = ptr + *reinterpret_cast<int32_t*>( (up1 + up2[1]) + up2[2] );
      ptr = ptr + up2[1];
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
    void
      constructcatchobject(
      cxxregistration *             cxxreg,
      const ehandler *        const catchblock,
      catchabletype *         const convertable,
      const dispatcher_context* const dispatch
      )
      const
    {
#ifdef _M_X64
      // build helper
      __try {
        struct typeinfo_t { void* vtbl; void* spare; char name[1]; };
        enum catchable_info { cidefault, cicomplex, civirtual } cinfo = cidefault;

        const typeinfo_t* ti = catchblock->typeinfo ? dispatch->va<typeinfo_t*>(catchblock->typeinfo) : NULL;
        if(ti && *ti->name && (ti->spare || catchblock->ishz)){
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
      __except(exception_execute_handler)
      {
        std::terminate();
      }
#else // _M_X64
      dispatch;
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
      __except(exception_execute_handler)
      {
        std::terminate();
      }
#endif
    }

    void
      catchit(
      cxxregistration *               const cxxreg,
      const nt::context *             const ctx,
      dispatcher_context *            const dispatch,
      const ehfuncinfo *              const funcinfo,
      const ehandler *                const catchblock,
      catchabletype *                 const convertable,
      const tryblock *                const tb,
      int                             const catchdepth,
      const exception_registration *  const nested,
      bool                                  recursive
      )
    {
#ifdef _M_X64
      uintptr_t xframe;
      uintptr_t frame = cxxreg->establisherframe(funcinfo, dispatch, &xframe);
      cxxregistration* cxxframe = reinterpret_cast<cxxregistration*>(frame);
#else
      recursive;
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
      if ( continuation )
        jumptocontinuation(continuation, cxxreg);
#else
      cxxreg->unwindnestedframes(this, ctx, frame, dispatch->va(catchblock->handler), tb->trylow, funcinfo, dispatch, recursive);
#endif
    }

    bool is_msvc(bool flexible = false) const
    {
      const uint32_t magic = get_ehmagic();
      return ExceptionCode == cxxmagic && NumberParameters == CxxNumberOfParameters && (flexible ? (magic >= ehmagic1200 && magic <= ehmagic1400) : (magic == _EH_MAGIC));
    }

#ifdef _M_X64
    typedef std::pair<uint32_t, rva_t> f108_t;
    bool is_inexceptionSpec(const f108_t* ptd108, const dispatcher_context* const dispatch) const
    {
      if(!ptd108)
        std::terminate();

      const throwinfo* const ti = get_throwinfo();
      const ehandler* handlers = dispatch->va<const ehandler*>(ptd108->second);
      const catchabletypearray* const catchables = thrown_va<catchabletypearray*>(ti->catchabletypearray);

      for(uint32_t i = 0; i < ptd108->first; i++){
        for(uint32_t j = 0; j < catchables->size; j++){
          const catchabletype* ct = dispatch->va<const catchabletype*>(catchables->type[j]);
          if(handlers[j].type_match(ct, ti, dispatch))
            return true;
        }
      }
      return false;
    }
#endif

    /// 15.1/2  When an exception is thrown, control is transferred 
    ///         to the nearest handler with a matching type (15.3)
    ///\return  does not return on handled exceptions.
    void
      find_handler(
      cxxregistration *       const ereg,
      const nt::context *     const ctx,
      dispatcher_context *    const dispatch,
      const ehfuncinfo *      const ehfi,
      const bool                    destruct,
      const ehstate_t               trylevel,
      const exception_registration *const nested_eframe)
    {
#ifdef _M_IX86
      ehstate_t cs = ereg->current_state(ehfi);
#endif
#ifdef _M_X64
      ehstate_t cs = ehfi->state_from_control(dispatch);
      uintptr_t frame;
      ereg->establisherframe(ehfi, dispatch, &frame);

      // get unwind try block
      ehstate_t etb = *reinterpret_cast<ehstate_t*>(frame + ehfi->estypeinfo + 4);
      if(cs > etb){
        // set state
        // BUG: use &frame instead, but isn't works :((
        ereg->current_state(ehfi, cs);
        //reinterpret_cast<cxxregistration*>(&frame)->current_state(ehfi, cs);
        // set unwind try block
        *reinterpret_cast<ehstate_t*>(frame + ehfi->estypeinfo + 4) = cs;
      }else{
        cs = etb;
      }

      if(cs < -1 || cs >= ehfi->unwindtable_size)
        std::terminate();

      bool catched = false;

      // is_cxx1
      if(is_msvc(true) && !get_throwinfo()){
        // TODO: get previous throw information and rethrow it
        dbg_pause();
        // replasing ctx && erec
        // assert check erec again for cxx

        //if(ptd.field108){
        //  auto f108 = 0;
        //  swap(ptd.field_108, f108);
        //  if(!IsInExceptionSpec(pair<count, ehandlers_rva>* &f108, dispatch)){
        //    if(!Is_bad_exception_allowed)
        //      std::terminate();
        //    __DestructExceptionObject(erec, true);
        //    throw std::bad_exception();
        //  }
        //}
      }

      // not cxx: is_cxx2
      bool is_cxx = is_msvc(true);
      if(is_cxx){
        if(ehfi->tryblocktable_size){
          // _GetRangeOfTrysToCheck
          const tryblock::ranges ranges = ehfi->get_try_ranges(trylevel, cs);
          const tryblock* tb = ranges.first;
          const throwinfo* const ti = get_throwinfo();
          const catchabletypearray* const clist = thrown_va<catchabletypearray*>(ti->catchabletypearray);
          for(; tb < ranges.second; tb++){
            if(tb->trylow <= cs && cs <= tb->tryhigh){
              const ehandler* eh = dispatch->va<ehandler*>(tb->catchsym);
              for(int catches = tb->ncatches; catches != 0; catches--, eh++){
                for(uint32_t catchables = 0; catchables < clist->size; catchables++){
                  rva_t catchable = clist->type[catchables];
                  catchabletype* ct = dispatch->va<catchabletype*>(catchable);
                  if(eh->type_match(ct, ti, dispatch)){
                    // OK.  We finally found a match.  Activate the catch.
                    catched = true;
                    catchit(ereg, ctx, dispatch, ehfi, eh, ct, tb, trylevel, nested_eframe, destruct);
                    goto next_try;
                  }

                }
              }
            }
next_try:;
          }
        }

        // tryEmpty:
        if(catched && ehfi->magic >= ehmagic1300){
          const f108_t* ehtable = ehfi->reserved ? dispatch->va<f108_t*>(ehfi->reserved) : 0;
          if(ehtable){
            if(is_inexceptionSpec(ehtable, dispatch)){
              uintptr_t frame;
              ereg->establisherframe(ehfi, dispatch, &frame);
              ereg->unwindnestedframes(this, ctx, frame, NULL, -1, ehfi, dispatch, destruct);
            }

          }
        }
      }else if(ehfi->tryblocktable_size){
        if(destruct)
          std::terminate();
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
              catchit(ereg, ctx, dispatch, ehfi, &eh, NULL, tb, trylevel, nested_eframe, destruct);
            }
          }
        }
      }
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
          const throwinfo * const ti = get_throwinfo();
          if ( !ti ) // rethrow?
          {
            // MSVC keeps previous thrown object in the per-thread data...
            // we just return to continue search in the containing scope.
            return;
          }
          for ( tryblock::ranges tr = ehfi->get_try_ranges(trylevel, cs);
            tr.first < tr.second;
            ++tr.first )
          {
            const tryblock* const tb = tr.first;
            if ( tb->trylow <= cs && cs <= tb->tryhigh )
              for ( int c = 0; c < tb->catchhigh; ++c ){
                ehandler* eh = &tb->catchsym[c];
                for ( unsigned i = 0; i < ti->catchabletypearray->size; ++i ){
                  if(eh[c].type_match(ti->catchabletypearray->type[i], ti, dispatch))
                  {
                    catchit(ereg, ctx, dispatch, ehfi, eh,
                      ti->catchabletypearray->type[i], tb, trylevel, nested_eframe, destruct);
                    // rethrown excetion
                    goto next_try;
                  }
                }
next_try: ;
              }
          }
          if ( destruct )
            destruct_eobject();
        }
        //return;
      }else if(ehfi->tryblocktable_size > 0 && !destruct) {
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
                catchit(ereg, ctx, dispatch, ehfi, &eh, NULL, tb, trylevel, nested_eframe, destruct);
                // rethrown exception
              }
            }
          }
        }
      }else {
        std::terminate();
      }
#endif
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
        return ExceptionContinueSearch;
    }
    // check a call by RtlUnwind
    if ( er->ExceptionFlags & nt::exception::unwind )
    {
      if ( er->iscxx() && ehfi->unwindtable_size && trylevel == 0 )
      {
        eframe->unwind(dispatch, ehfi);
      }
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
        return cxxer->get_throwinfo()->forwardcompathandler
          (er, eframe, ectx, dispatch, ehfi, trylevel, nested_eframe, destruct);
      }
      else
#endif
      {
        cxxer->find_handler(eframe, ectx, dispatch, ehfi, destruct, trylevel, nested_eframe);
      }
    }
    return ExceptionContinueSearch;
  }

#endif // _M_IX86

  //#endif #ifndef NTL_EH_RUNTIME

#endif//#ifdef _MSC_VER
} // cxxruntime
}//namespace ntl

#endif//#ifndef NTL__NT_EXCEPTION
