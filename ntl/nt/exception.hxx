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



#undef exception_code
#undef exception_info
#undef abnormal_termination

#define exception_code              _exception_code()
#define exception_info()            reinterpret_cast<struct ntl::nt::exception::pointers*>(_exception_info())
#define abnormal_termination        (_abnormal_termination() != 0)

extern "C" 
{
  unsigned long __cdecl _exception_code(void);
  void *        __cdecl _exception_info(void);
  int           __cdecl _abnormal_termination(void);
}


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

#if defined _M_IX86
    struct dispatcher_context
    {
      struct registration * RegistrationPointer;
    };
#elif defined _M_X64
    struct dispatcher_context
    {
      uint64_t    ControlPc;
      pe::image * ImageBase;
      void * /*RUNTIME_FUNCTION* */ FunctionEntry;
      uint64_t    EstablisherFrame;
      uint64_t    TargetIp;
      context *   ContextRecord;
      void * /*EXCEPTION_ROUTINE* */ LanguageHandler;
      void *      HandlerData;
      struct unwind_history_table * HistoryTable;
      uint64_t    ScopeIndex;
      uint64_t    Fill0;
    };
#endif

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

      inline void raise() const;
    };
    
    struct registration;
    
    /// Exception handler prototype
    typedef
      disposition __cdecl
      handler_t(
        exception::record   * exception_record,
        registration        * establisher_frame,
        context             * context_record,
        dispatcher_context  * dispatcher_context
        );

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

    /// exception_info result
    struct pointers
    {
      record*   ExceptionRecord;
      context*  ContextRecord;
    };

};//class exception


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

typedef nt::exception::filter             exception_filter;
typedef nt::exception::disposition        exception_disposition;
typedef nt::exception::dispatcher_context dispatcher_context;
typedef nt::exception::flags              exception_flags;
typedef nt::exception::record             exception_record;
typedef nt::exception::handler_t          exception_handler;
typedef nt::exception::pointers           exception_pointers;

//typedef nt::exception::registration exception_registration; see nt::tib::ExceptionList
struct exception_registration : public nt::exception::registration {};

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
#endif

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

#if defined _MSC_VER && defined _M_IX86

// C++ exception compiler support
// see some details at http://www.openrce.org/articles/full_view/21

// wierd names! do not abuse =)

static const uint32_t ehmagic1400 = 0x19930522;
static const uint32_t ehmagic1300 = 0x19930521;
static const uint32_t ehmagic1200 = 0x19930520;

#ifndef _EH_MAGIC
  #if _MSC_VER >= 1400
    #define _EH_MAGIC ehmagic1400
  #elif _MSC_VER>= 1300
    #define _EH_MAGIC ehmagic1300
  #else
    #define _EH_MAGIC ehmagic1200
  #endif
#endif

struct memptrdescriptor // _PMD
{
  typedef int32_t  mdiff_t;
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
  typedef void (__thiscall eobject::* dtor_ptr)();
  typedef void (__thiscall eobject::* ctor_ptr)();
  typedef void (__thiscall eobject::* ctor_ptr2)(int);
};
STATIC_ASSERT(sizeof(eobject::dtor_ptr) == sizeof(void*));

///\note the calling convention should not matter since this has no arguments
typedef void generic_function_t(); 

struct catchabletype
{
  uint32_t  memmoveable : 1;
  uint32_t  refonly     : 1;
  uint32_t  hasvirtbase : 1;
  type_info *       typeinfo;
  memptrdescriptor  thiscast;
  uint32_t          object_size;
  union { eobject::ctor_ptr copyctor; eobject::ctor_ptr2 copyctor2; };
};

struct catchabletypearray
{
  uint32_t        size; 
  catchabletype * type[1];
};


/// the MSVC's internal _s__ThrowInfo structure representation
struct throwinfo
{
  /* 0x00 */  uint32_t  econst    : 1;
  /* 0x00 */  uint32_t  evolatile : 1;
  /* 0x04 */  eobject::dtor_ptr exception_dtor; 
  /* 0x08 */  exception_disposition (__cdecl * forwardcompathandler)(...);///\todo safe proto
  /* 0x0C */  ntl::catchabletypearray *  catchabletypearray; ///< types able to catch the exception.

};
STATIC_ASSERT(sizeof(throwinfo) == 0x10); // STATIC_ASSERT(sizeof(exception_dtor) == sizeof(void*));

/// This type represents the catch clause
struct ehandler
{
//  union { uint32_t  adjectives; void * ptr; };
  uint32_t isconst      : 1;
  uint32_t isvolatile   : 1;
  uint32_t isunaligned  : 1;// guess it is not used on x86
  uint32_t isreference  : 1;

  const type_info *     typeinfo;
  ptrdiff_t             eobject_bpoffset; // 0 = no object (catch by type)
  generic_function_t *  handler;

  /// 15.1/3  A throw-expression initializes a temporary object, called
  ///         the exception object, the type of which is determined by
  ///         removing any top-level cv-qualifiers from the static type
  ///         of the operand of throw and adjusting the type from “array of T”
  ///         or “function returning T” to “pointer to T” or “pointer to function returning T”, respectively.
  bool type_match(const catchabletype * ct, const throwinfo * ti) const
  {
    // catch(...) handles them all
    if ( !this->typeinfo || !*this->typeinfo->name() )
      return true;
    // different TI record with different name?
    if ( this->typeinfo != ct->typeinfo
      && std::strcmp(this->typeinfo->name(), ct->typeinfo->name()) )
      return false;
    // reference?
    if ( ct->refonly && !isreference ) return false;
    // check CV cvalifiers?.. Hmmm
    if ( ti->econst && !isconst ) return false;
    if ( ti->evolatile && !isvolatile ) return false;
    return true;
  }
};

struct unwindtable
{
  int     state;
  generic_function_t * unwindfunclet; 
};

typedef int ehstate_t; ///< unwind map index

struct tryblock
{
  ehstate_t   trylow;
  ehstate_t   tryhigh;
  ehstate_t   catchhigh;
  int         ncatches;
  ehandler *  catchsym;

  typedef std::pair<const tryblock*, const tryblock*> ranges;
};

// have you seen /d1ESrt in TFM? so do I...

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


/* BTW, per-thread data returned by MSVC's _getptd()

    +0x78 - terminate handler
    +0x7C - unexpected handler
    +0x80 - SE translator
 */


struct cxxregistration : public nt::exception::registration
{
  /* 0x08 */  ehstate_t state;
#if 0
  /* 0x0C */  uintptr_t stackbaseptr;
#endif
  uintptr_t stackbaseptr() const
  { 
    STATIC_ASSERT(sizeof(cxxregistration) == 0xC);
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

  // __CallSettingFrame
  /// thunk to call unwindfunclet
  ///\return the value returned from unwindfunclet
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
    unwind(dispatcher_context *,
          const ehfuncinfo * const  ehfi,
          ehstate_t const           to_state = -1) ///< defaults to empty state
  {
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
    /* 0x14 */  return ExceptionInformation[0];
  }

  eobject * get_object() const
  { 
    /* 0x18 */  return reinterpret_cast<eobject*>(ExceptionInformation[1]);
  }

  const throwinfo * get_throwinfo() const
  { 
    /* 0x1C */  return reinterpret_cast<const throwinfo*>(ExceptionInformation[2]);
  }

  /// destruct the exception object if it has a destructor
  void destruct_eobject(bool cannotthrow = true) const
  {
    _Assert(ExceptionCode == cxxmagic);
    _Assert(get_throwinfo());
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
    nt::teb::set(&nt::teb::ExceptionList, reinterpret_cast<uintptr_t>(&guard));
    generic_function_t * const continuation = cxxreg->callsettingframe(handler, nlg_code);
    nt::teb::set(&nt::teb::ExceptionList, reinterpret_cast<uintptr_t>(guard.next));
    return continuation;
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
      if (is_msvc() && continuation)
      {
        destruct_eobject(!!_abnormal_termination());
      }
    }
    return continuation;
  }

  #pragma warning(push)
  #pragma warning(disable:4731)//frame pointer register 'ebp' modified by inline assembly code
  // SE handlers already registered should be SAFESEH
  #pragma warning(disable:4733)//Inline asm assigning to 'FS:0' : handler not registered as safe handler
  __declspec(noreturn)
  static void jumptocontinuation(generic_function_t * funclet, cxxregistration *cxxreg)
  {
    using namespace nt;
    // unlink SE handler
    exception_registration * const top = teb::get(&teb::ExceptionList);
    teb::set(&teb::ExceptionList, reinterpret_cast<uintptr_t>(top->next));

    const uint32_t _ebp = cxxreg->stackbaseptr();
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
  #pragma warning(pop)

  // ___BuildCatchObject
  /// 15.3/16 When the exception-declaration specifies a class type, a copy
  ///         constructor is used to initialize either the object declared
  ///         in the exception-declaration or,
  ///         if the exception-declaration does not specify a name,
  ///         a temporary object of that type.
  ///\note    This is the question may we optimize out the last case.
  void
   constructcatchobject(
      cxxregistration *       const cxxreg,
      const ehandler *        const catchblock,
      catchabletype *         const convertable)
  const
  {
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
        if ( convertable->hasvirtbase )
          (get_object()->*convertable->copyctor2)(1);
        else
          (get_object()->*convertable->copyctor)();
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
      const exception_registration *  const nested)
  {
    //  15.3/7  A handler is considered active when initialization is complete
    //          for the formal parameter (if any) of the catch clause.
    //          [ Note: the stack will have been unwound at that point]
    if ( convertable )
    {
      constructcatchobject(cxxreg, catchblock, convertable);
    }
    // destroy auto objects in the try scope
    if ( nested ) nested->unwindnestedframes(this);
    else          cxxreg->unwindnestedframes(this);
    cxxreg->unwind(dispatch, funcinfo, tb->trylow);

    cxxreg->state = tb->tryhigh + 1;
    generic_function_t * const continuation =
      callcatchblock(cxxreg, ctx, funcinfo, catchblock->handler, catchdepth);
    if ( continuation )
      jumptocontinuation(continuation, cxxreg);
  }

  bool is_msvc() const
  {
    return ExceptionCode == cxxmagic && NumberParameters == 3 && ExceptionInformation[0] == _EH_MAGIC;
  }

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
      const exception_registration *  const nested_eframe)
  {
    ehstate_t cs = ereg->current_state(ehfi);
    if ( ExceptionCode == cxxmagic )
    {
      const uint32_t   ehmagic = ExceptionInformation[0];
      if ( NumberParameters == 3
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
            for ( int c = 0; c < tb->catchhigh; ++c )
              for ( unsigned i = 0; i < ti->catchabletypearray->size; ++i )
                if ( tb->catchsym[c].type_match(ti->catchabletypearray->type[i], ti) )
                {
                  catchit(ereg, ctx, dispatch, ehfi, &tb->catchsym[c],
                    ti->catchabletypearray->type[i], tb, trylevel, nested_eframe);
                  // rethrown excetion
                  goto next_try;
                }
          next_try: ;
        }
        if ( destruct )
          destruct_eobject();
      }
      //return;
    }else if(ehfi->tryblocktable_size > 0 && !destruct)
    {
      // TODO: FindHandlerForForeignException(EHExceptionRecord *pExcept,EHRegistrationNode *pRN,_CONTEXT *pContext,void *pDC,_s_FuncInfo const *pFuncInfo,intcurState,intCatchDepth,EHRegistrationNode *pMarkerRN)
      //__debugbreak();

      for ( tryblock::ranges tr = ehfi->get_try_ranges(trylevel, cs);
        tr.first < tr.second;
        ++tr.first )
      {
        const tryblock* const tb = tr.first;
        if ( tb->trylow <= cs && cs <= tb->tryhigh)
        {
          for(int n = 0; n < tb->ncatches; n++){
            const ehandler& eh = tb->catchsym[n];
            bool ok = eh.typeinfo != 0;
          }
          if(tb->ncatches){
            // get last catchsym (the last catch in that try is an ellipsis (no other can be))
            const ehandler& eh = tb->catchsym[tb->ncatches-1];
            // check for ellipsis
            if(!eh.typeinfo || *eh.typeinfo->name() == 0){
              // ok, it can catch exception
              catchit(ereg, ctx, dispatch, ehfi, &eh, NULL, tb, trylevel, nested_eframe);
              // rethrown excetion
            }
          }
        }
      }

    }else {
      std::terminate();
    }
  }

};


/// C++ exception_handler implementation.
///\note  This is called by __CxxFrameHandler3 which has to be compiled w/o /GL,
///       so avoid __forceinline here to reduce the size of all this huge stuff
///       (MSVC has wierd heuristics to ignore inline, so hack arround)
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
     && ehfi->synchronous )
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
      && cxxer->NumberParameters >= 3 /**\see _CxxThrowException args*/
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

//#endif #ifndef NTL_EH_RUNTIME

#endif//#ifdef _MSC_VER

}//namespace ntl

#endif//#ifndef NTL__NT_EXCEPTION
