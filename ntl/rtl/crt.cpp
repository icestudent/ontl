//#pragma warning(disable:4820 4061)
#include "../stlx/cstdio.hxx"
#include "../stlx/cstdlib.hxx"
#include "../nt/exceptions.hxx"
#include "../atomic.hxx"


#pragma region section_attributes
#pragma comment(linker, "/merge:.CRT=.rdata")
#define sec(x) __declspec(allocate(x))

#ifdef __ICL
#pragma warning(disable: 14)
#endif
#pragma data_seg(push)

/* c++ initializers */
#pragma section(".CRT$XCA",long,read)
#pragma section(".CRT$XCZ",long,read)

/* c++ terminators */
#pragma section(".CRT$XPA",long,read)
#pragma section(".CRT$XPZ",long,read)

/* termination */
#pragma section(".CRT$XTA",long,read)
#pragma section(".CRT$XTZ",long,read)

sec(".CRT$XCA") vfv_t* __xc_a[]= {0};
sec(".CRT$XCZ") vfv_t* __xc_z[]= {0};

sec(".CRT$XPA") vfv_t* __xp_a[]= {0};
sec(".CRT$XPZ") vfv_t* __xp_z[]= {0};

sec(".CRT$XTA") vfv_t* __xt_a[]= {0};
sec(".CRT$XTZ") vfv_t* __xt_z[]= {0};
#undef sec
#pragma data_seg(pop)
#ifdef __ICL
#pragma warning(disable: 14)
#endif
#pragma endregion

#include "../stlx/vector.hxx"

#ifdef NTL__SUBSYSTEM_KM
# include "../km/new.hxx"
#else
# include "../nt/new.hxx"
# include "../nt/thread.hxx"
#endif

namespace
{
  static const size_t exit_list_max = 1024*4;
  static vfv_t* exit_list[exit_list_max];
  static volatile uint32_t exit_list_count = 0;

  static vfv_t* quick_exit_list[exit_list_max];
  static volatile uint32_t quick_exit_count = 0;

  void initterm(vfv_t** from, vfv_t** to)
  {
    while(from < to){
      if(*from)
        (*from)();
      ++from;
    }
  }

  vfv_t* onexit(vfv_t* func)
  {
    if(func){
      uint32_t idx = ntl::atomic::exchange_add(exit_list_count,1);
      if(idx < _countof(exit_list))
        return exit_list[idx] = func;
    }
    return 0;
  }

  void doexit(int /*code*/, int quick, int /*retcaller*/)
  {
    if(!quick){
      for(vfv_t** f = exit_list+exit_list_count-1; f >= exit_list; --f){
        __ntl_try {
          assert(*f != 0);
          (*f)();
        }
        __ntl_catch(...){
          std::terminate();
        }
      }
    }else{
      for(vfv_t** f = quick_exit_list+quick_exit_count-1; f >= quick_exit_list; --f){
        __ntl_try{
          assert(*f != 0); // we checked it at `at_quick_exit`
          (*f)();
        }
        __ntl_catch(...){
          std::terminate();
        }
      }
    }
  }
} // namespace

#ifdef _MSC_VER
  #ifdef _M_X64
  # pragma comment(linker, "/alternatename:__init_iostream_objects=__init_iostream_objects_stub")
  #else
  # pragma comment(linker, "/alternatename:___init_iostream_objects=___init_iostream_objects_stub")
  #endif
#endif
extern "C" void _cdecl __init_iostream_objects_stub(){}


namespace ntl
{

  extern "C" void _cdecl __init_crt(bool init)
  {
    if(init){
      // init exception support on current thread
      #if STLX__USE_EXCEPTIONS
      cxxruntime::_initptd();
      #endif

      // init static objects
      initterm(__xc_a, __xc_z);
      initterm(__xp_a, __xp_z);
      initterm(__xt_a, __xt_z);

    }else{
      // free static objects
      doexit(0,false,true);
      // explicitly destroy iostreams
      __init_iostream_objects(false);
      (void)&__init_iostream_objects_stub;
    }
  }
}

/************************************************************************/
/* `exit` and `quick_exit`                                              */
/************************************************************************/
extern "C" int _cdecl atexit(vfv_t func)
{
  return onexit(func) ? 0 : -1;
}

extern "C" int _cdecl at_quick_exit(vfv_t* f)
{
  const uint32_t idx = ntl::atomic::exchange_add(quick_exit_count, 1);
  if(f && idx < _countof(quick_exit_list)){
    // find, if `f` was already registered
    for(vfv_t** p = quick_exit_list; p < quick_exit_list+quick_exit_count; ++p)
      if(*p == f)
        return 0;
    // add to list
    quick_exit_list[idx] = f;
    return 0;
  }
  return -1;
}

void NTL__CRTCALL exit(int status)
{
  doexit(status, false, true);
  _Exit(status);
}

void NTL__CRTCALL quick_exit(int status)
{
  doexit(status, true,true);
  _Exit(status);
}

#ifndef NTL__SUBSYSTEM_KM
void NTL__CRTCALL _Exit(int code)
{
  using ntl::nt::status;
  ntl::nt::user_thread::exit_process(code == EXIT_SUCCESS ? status::success : (code == EXIT_FAILURE ? status::unsuccessful : static_cast<ntl::nt::ntstatus>(code)) );
}
#endif

/************************************************************************/
/* `abort`                                                              */
/************************************************************************/
#ifndef NTL__SUBSYSTEM_KM
extern "C" void _cdecl abort()
{
  ntl::nt::user_thread::exit_process(ntl::nt::status::unsuccessful);
}
#endif
/************************************************************************/
/* `new` handler                                                        */
/************************************************************************/
namespace ntl
{
  extern "C" volatile std::new_handler __new_handler = 0;
}

std::new_handler std::set_new_handler(std::new_handler new_p) __ntl_nothrow
{
  std::new_handler prev = new_p;
  return ntl::atomic::generic_op::exchange(ntl::__new_handler, prev);
}

/************************************************************************/
/* `terminate` handler                                                  */
/************************************************************************/
std::terminate_handler __ntl_std_terminate_handler = 0;

namespace std
{
  terminate_handler set_terminate(terminate_handler f) __ntl_nothrow
  {
    assert(f != nullptr);
    terminate_handler old = f;
    return ntl::atomic::generic_op::exchange(__ntl_std_terminate_handler, old);
  }

  void terminate()
  {
    if(__ntl_std_terminate_handler)
      __ntl_std_terminate_handler();
    else
      abort();
  }
} // std


//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* `purecall` handler                                                   */
/************************************************************************/
#ifdef _MSC_VER
# include "../nt/debug.hxx" // for abort implementation
# pragma warning(disable:4702) // unreachable code

extern "C" int _cdecl _purecall(void)
{
  _assert_msg("pure virtual function called!");
  debug_abort();
  abort();
  return 0;
}
#endif
