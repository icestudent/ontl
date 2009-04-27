////////////////////////////////////////////////////////////////////////////////
///
///\file
///\brief C++ exception support runtime
///
///\note  The file contains compiler predefined library helpers
///       and must be compiled without /GL to avoid C2268.
///       So it is the separate compilation unit to support /GL for other ones.
///
////////////////////////////////////////////////////////////////////////////////

#define NTL_EH_RUNTIME

#ifdef _M_X64
//# define NTL__OTHEREHMAGICS
#include "../pe/image.hxx"
#endif

#include "../nt/exception.hxx"
#include "../nt/context.hxx"
#include "../nt/status.hxx"

#ifdef _DEBUG
# include "../nt/debug.hxx"
  void dbg_pause()
  {
    ntl::nt::dbg::bp();
  }
#endif

#pragma region std::unexpected
/// RTL poiner to the current unexpected_handler
std::unexpected_handler __ntl_std_unexpected_handler;

std::unexpected_handler std::set_unexpected(std::unexpected_handler f) __ntl_nothrow
{
  assert(f != nullptr);
  unexpected_handler old = f;
  return ntl::atomic::generic_op::exchange(__ntl_std_unexpected_handler, old);
}

void std::unexpected()
{
  if(__ntl_std_unexpected_handler)
    __ntl_std_unexpected_handler();
  else
    std::terminate();
}

#pragma endregion

using namespace ntl;
using namespace ntl::cxxruntime;

#ifdef _M_IX86
static __forceinline
uint32_t get_eax()
{
  __asm xchg eax, eax
}
#endif

#ifdef _M_X64


exception_disposition
ntl::cxxruntime::cxxframehandler(
                exception_record *        const er,       ///< thrown NT exception
                cxxregistration *         const eframe,
                const nt::context *       const ectx,
                dispatcher_context *      const dispatch,
                const ehfuncinfo *              const ehfi,     ///< registered handlers
                int                             const trylevel      /* CatchDepth */,
                const exception_registration *  const nested_eframe /* pMarkerRN */,
                bool                            const destruct      /* recursive */)
{
  if(!
    (er->iscxx()
    || (er->isconsolidate() && er->NumberParameters == exception_record::maximum_parameters && er->ExceptionInformation[8] == ehmagic1200)
    || er->islongjump() || ehfi->magic < _EH_MAGIC || !ehfi->synchronous)
    )
  {
    return ExceptionContinueSearch;
  }

  // check a call by RtlUnwind
  if(er->ExceptionFlags & nt::exception::unwind){
    if(ehfi->unwindtable_size && trylevel == 0) {
      bool unwinded = false;
      if(er->ExceptionFlags & nt::exception::target_unwind){
        if(er->islongjump()){
          ehstate_t state = ehfi->state_from_ip(dispatch, (void*)ectx->Rip);
          if(state < -1 || state >= ehfi->unwindtable_size)
            std::terminate();
          // __FrameUnwindToState
          eframe->unwind(dispatch, ehfi, state);
          unwinded = true;
        }else if(er->isconsolidate()){
          ehstate_t state = static_cast<ehstate_t>(er->ExceptionInformation[3]);
          if(state < -1 || state >= ehfi->unwindtable_size)
            __debugbreak();
          cxxregistration* frame = reinterpret_cast<cxxregistration*>(er->ExceptionInformation[1]);
          frame->unwind(dispatch, ehfi, state);
          unwinded = true;
        }
      }
      if(!unwinded){
        // __FrameUnwindToEmptyState
        eframe->unwind(dispatch, ehfi);
      }
    }
    // async exceptions are to be unwinded by __except_handler3
    return ExceptionContinueSearch;
  }

  if(!ehfi->tryblocktable_size){
    if(ehfi->magic < ehmagic1300 || !ehfi->unwindhelp)
      return ExceptionContinueSearch;
  }

  cxxrecord* cxxer = static_cast<cxxrecord*>(er);
  if(cxxer->iscxx() && er->NumberParameters >= 3 && cxxer->get_ehmagic() > ehmagic1400){
    throwinfo::forwardcompathandler_t* fch = NULL;
    if(cxxer->get_throwinfo()->forwardcompathandler){
      fch = (throwinfo::forwardcompathandler_t*)(cxxer->get_throwinfo()->forwardcompathandler + cxxer->get_throwimagebase());
      return fch(er, eframe, ectx, dispatch, ehfi, trylevel, nested_eframe, destruct);
    }
  }

  // not cxx ?
  cxxer->find_handler(eframe, ectx, dispatch, ehfi, destruct, trylevel, nested_eframe);

  return ExceptionContinueSearch;
}

void cxxregistration::unwindnestedframes(const exception_record* ehrec, const nt::context* ctx, uintptr_t establishedframe, const void* handler, int state, const ehfuncinfo* ehfi, dispatcher_context* const dispatch, bool recursive)
{
  static const exception_record ehtemplate = {ntl::nt::status::unwind_consolidate, exception_noncontinuable, NULL, NULL, exception_record::maximum_parameters, {ehmagic1200}};

  exception_record er = ehtemplate;
  er.ExceptionInformation[0] = (uintptr_t)__CxxCallCatchBlock,
    er.ExceptionInformation[1] = establishedframe,
    er.ExceptionInformation[2] = (uintptr_t)handler,
    er.ExceptionInformation[3] = state,
    er.ExceptionInformation[4] = (uintptr_t)ctx,
    er.ExceptionInformation[5] = (uintptr_t)ehfi,
    er.ExceptionInformation[6] = (uintptr_t)ehrec,
    er.ExceptionInformation[7] = recursive,
    er.ExceptionInformation[8] = ehmagic1200;

  nt::context octx;
  STATIC_ASSERT(sizeof(octx) == 0x4d0);
  RtlUnwindEx(fp, dispatch->ControlPc, &er, NULL, &octx, dispatch->HistoryTable);
}

//////////////////////////////////////////////////////////////////////////
cxxregistration::frame_info* cxxregistration::frame_info::info_ = NULL;

exception_filter ExFilterRethrow(exception_pointers* ep)
{
  cxxrecord* ehrec = static_cast<cxxrecord*>(ep->ExceptionRecord);
  exception_record* ehrec2 = 0;
  bool arg10 = false;
  if(ehrec->is_msvc(true)){
    if(ehrec->ExceptionInformation[1] == ehrec2->ExceptionInformation[1])
      arg10 = true;
    if(ehrec->ExceptionInformation[1]){
      // ptd->2d0 = 1
      arg10 = true;
    }
  }
  return arg10 ? exception_execute_handler : exception_continue_search;
}

void RethrowException(exception_record* ehrec)
{
  cxxrecord* cxxer = static_cast<cxxrecord*>(ehrec);
  cxxer->raise();
}

extern "C" generic_function_t* __CxxCallCatchBlock(exception_record* ehrec)
{
  // save the current exception
  exception_pointers* ep = &_getptd()->curexception;
  exception_pointers saved_exception = *ep;
  ep->ExceptionRecord = reinterpret_cast<exception_record*>(ehrec->ExceptionInformation[6]);
  ep->ContextRecord = reinterpret_cast<nt::context*>(ehrec->ExceptionInformation[4]);

  // original record
  cxxrecord* cxxer = reinterpret_cast<cxxrecord*>(ehrec->ExceptionInformation[6]);
  cxxregistration* eframe = reinterpret_cast<cxxregistration*>(ehrec->ExceptionInformation[1]);
  generic_function_t* ret;
  bool fail = false;
  cxxregistration::frame_info frame(cxxer->get_object());

  exception_record* tmpER = 0;
  generic_function_t *handler = reinterpret_cast<generic_function_t*>(ehrec->ExceptionInformation[2]);
  bool recursive = ehrec->ExceptionInformation[7] != 0;
  if(recursive){
    tmpER = _getptd()->prevER;
    _getptd()->curexception.ExceptionRecord = tmpER;

  }
  __try{
    __try{
      ret = _CallSettingFrame(handler, eframe, 0x100);
    }
    __except(ExFilterRethrow(ntl::_exception_info())){
      fail = true;
      // ptd->2d0 = 0
      if(recursive){
        cxxer->destruct_eobject(true);
        RethrowException(_getptd()->prevER);
      }else{
        RethrowException(cxxer);
      }
    }

  }
  __finally{
    frame.unlink();
    fail |= _abnormal_termination() != 0;
    if(!fail && cxxer->is_msvc(true)){
      if(cxxregistration::frame_info::find(cxxer->get_object()))
        cxxer->destruct_eobject();
    }
  }
  //frame.unlink();
  //if(!fail && cxxer->is_msvc(true)){
  //  if(!cxxregistration::frame_info::find(cxxer->get_object()))
  //    cxxer->destruct_eobject();
  //}

  // restore saved exception
  ep = &_getptd()->curexception;
  *ep = saved_exception;

  *reinterpret_cast<uintptr_t*>(eframe->fp.FramePointers + reinterpret_cast<ehfuncinfo*>(ehrec->ExceptionInformation[5])->unwindhelp) = -2;
  return ret;
}

#endif // _M_X64

/// throw T; implementation.
/// 15.1/1  Throwing an exception transfers control to a handler
///         An object is passed and the type of that object determines which
///         handlers can catch it.
///\note    MSVC's throw; statement sets both pointers to nulls.
#ifdef __ICL
typedef ntl::cxxruntime::throwinfo _s__ThrowInfo;
#endif
__declspec(noreturn)
extern "C"
void __stdcall _CxxThrowException(void * object, _s__ThrowInfo const * info)
{
#ifdef _M_IX86
  std::array<uintptr_t, 3> args = { _EH_MAGIC, (uintptr_t)object, (uintptr_t)info };
#endif
#ifdef _M_X64
  void* imagebase;
  ntl::nt::RtlPcToFileHeader(info, &imagebase);
  std::array<uintptr_t, 4> args = { _EH_MAGIC, (uintptr_t)object, (uintptr_t)info, (uintptr_t)imagebase};
  const throwinfo* ti = reinterpret_cast<const throwinfo*>(info);
  if(info && (ti->e8 || !imagebase))
    args[0] = ehmagic1994;
#endif // _M_X64
  raise_exception(exception_record::cxxmagic, nt::exception::noncontinuable, args);
}

template<typename T>
void fixptr(uintptr_t base, const rva_t ptr, T& vptr)
{
  vptr = (T)(base + ptr);
}

///\see exception_handler
/// indirectly called by _CxxThrowException -> RtlDispatchException (RtlpExecuteHandlerForException)
/// OR RtlUnwind (RtlpExecuteHandlerForUnwind)
extern "C"
exception_disposition
__cdecl ///\note actually the first arg is passed in EAX register on x86
__CxxFrameHandler3(
  /** ehfuncinfo * eax on x86 */
  exception_record *    er,       ///< thrown NT exception
  cxxregistration *     frame,    ///< tib::ExceptionList node
  nt::context *         ectx,     ///< not used on x86
  dispatcher_context *  dispatch  ///< not used on x86
  )
{
#ifdef _M_IX86
  const ehfuncinfo* /*const*/ ehfi = reinterpret_cast<const ehfuncinfo*>(get_eax());
  return cxxframehandler(er, frame, ectx, dispatch, ehfi);
#endif
#ifdef _M_X64
  uintptr_t imagebase;
  RtlPcToFileHeader(__CxxFrameHandler3, (void**)&imagebase);

  const ehfuncinfo_packed* ehfip = reinterpret_cast<const ehfuncinfo_packed*>( imagebase + *dispatch->HandlerData );

  struct frame_t
  {
    cxxregistration* frame;
    uintptr_t var30;
    uintptr_t var38;
    uintptr_t var40;
  } lframe = {frame};

  ehfuncinfo efi(*ehfip, imagebase);
  const ehfuncinfo* ehfi = &efi;

  return cxxframehandler(er, reinterpret_cast<cxxregistration*>(&lframe), ectx, dispatch, ehfi);
#endif
}

#ifdef __ICL
extern "C"
exception_disposition
__cdecl ///\note actually the first arg is passed in EAX register on x86
__CxxFrameHandler(
                   /** ehfuncinfo * eax on x86 */
                   exception_record *    er,       ///< thrown NT exception
                   cxxregistration *     frame,    ///< tib::ExceptionList node
                   nt::context *         ectx,     ///< not used on x86
                   dispatcher_context *  dispatch  ///< not used on x86
                   )
{
  const ehfuncinfo* /*const*/ ehfi = reinterpret_cast<const ehfuncinfo*>(get_eax());
  return cxxframehandler(er, frame, ectx, dispatch, ehfi);
}
#endif
///\todo __EH_prolog for /Os

