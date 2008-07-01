////////////////////////////////////////////////////////////////////////////////
///
///\file  C++ exception support runtime
///
///\note  The file contains compiler predefined library helpers
///       and must be compiled without /GL to avoid C2268.
///       So it is the separate compilation unit to support /GL for other ones.
///
////////////////////////////////////////////////////////////////////////////////

#include "../stlx/exception.hxx"

#define NTL_EH_RUNTIME

//#define NTL__OTHEREHMAGICS

#include "../nt/exception.hxx"

/// RTL poiner to the current terminate_handler
///\todo shall be an atomic type for safe multithreading
std::terminate_handler __ntl_std_terminate_handler;

/// Sets the current handler function for terminating exception processing.
///\warning f shall not be a null pointer.
///\return  The previous terminate_handler.
std::terminate_handler
  std::set_terminate(std::terminate_handler f ///< new handler
                    ) __ntl_nothrow
{
  const terminate_handler old = __ntl_std_terminate_handler;
  __ntl_std_terminate_handler = f;
  return old;
}

void std::terminate()
{
  __ntl_std_terminate_handler();
}


using namespace ntl;

static __forceinline
uint32_t get_eax()
{
  __asm xchg eax, eax
}


/// throw T; implementation.
/// 15.1/1  Throwing an exception transfers control to a handler
///         An object is passed and the type of that object determines which
///         handlers can catch it.
///\note    MSVC's throw; statement sets both pointers to nulls.
__declspec(noreturn)
extern "C"
void __stdcall _CxxThrowException(void * object, _s__ThrowInfo const * info)
{
  std::array<uintptr_t, 3> args = { _EH_MAGIC, (uintptr_t)object, (uintptr_t)info };
  raise_exception(exception_record::cxxmagic, nt::exception::noncontinuable, args);
}

///\see exception_handler
/// indirectly called by _CxxThrowException -> RtlDispatchException (RtlpExecuteHandlerForException)
/// OR RtlUnwind (RtlpExecuteHandlerForUnwind)
extern "C"
exception_disposition
__cdecl ///\note actually the first arg is passed in EAX register
__CxxFrameHandler3(
  /** ehfuncinfo * eax */
  exception_record *    er,       ///< thrown NT exception
  cxxregistration *     frame,    ///< tib::ExceptionList node
  nt::context *         ectx,     ///< not used on x86
  dispatcher_context *  dispatch  ///< not used on x86
  )
{
  const ehfuncinfo* const ehfi = reinterpret_cast<const ehfuncinfo*>(get_eax());
  return cxxframehandler(er, frame, ectx, dispatch, ehfi);
}


///\todo __EH_prolog for /Os

