//#pragma warning(disable:4820 4061)

#include "../stlx/cstdlib.hxx"
#include "../nt/exception.hxx"

#pragma region section_attributes
#pragma comment(linker, "/merge:.CRT=.rdata")

#pragma section(".CRT$XCA",long,read)
#pragma section(".CRT$XCZ",long,read)

/* c++ initializers */
#define sec(x) __declspec(allocate(x))
sec(".CRT$XCA") vfv_t* __xc_a[]= {0};
sec(".CRT$XCZ") vfv_t* __xc_z[]= {0};
#undef sec

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
  typedef std::vector<vfv_t*> exit_funcs_t;
  static exit_funcs_t* exit_list = 0;

  static vfv_t* quick_exit_list[128];
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
    if(exit_list){
      if(func)
        exit_list->push_back(func);
      return func;
    }
    return 0;
  }

  void doexit(int /*code*/, int quick, int /*retcaller*/)
  {
    if(!quick){
      for(exit_funcs_t::const_iterator e = exit_list->cbegin(); e != exit_list->cend(); ++e)
        if(*e) (*e)();
      delete exit_list;
    }else{
      for(vfv_t** f = quick_exit_list+quick_exit_count; f >= quick_exit_list; --f){
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

extern "C" void _cdecl __check_iostream_objects();
extern "C" inline void _cdecl __check_iostreams_stub(){}
#ifdef _M_X64
# pragma comment(linker, "/alternatename:__check_iostream_objects=__check_iostreams_stub")
#else
# pragma comment(linker, "/alternatename:___check_iostream_objects=___check_iostreams_stub")
#endif

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
      exit_list = new exit_funcs_t();
      exit_list->reserve(static_cast<size_t>(__xc_z - __xc_a));
      initterm(__xc_a, __xc_z);

      // init io streams
      __check_iostreams_stub();
      __check_iostream_objects();
    }else{
      // free static objects
      doexit(0,false,true);
    }
  }
}

/************************************************************************/
/* `exit` and `quick_exit`                                              */
/************************************************************************/
int _cdecl atexit(vfv_t func)
{
  return onexit(func) ? 0 : -1;
}

int _cdecl std::at_quick_exit(vfv_t* f)
{
  const uint32_t idx = ntl::atomic::increment(quick_exit_count);
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

void _cdecl std::quick_exit(int status)
{
  doexit(status, true,true);
  _Exit(status);
}

#ifndef NTL__SUBSYSTEM_KM
void _cdecl _Exit(int code)
{
  using ntl::nt::status;
  ntl::nt::user_thread::exit_process(code == EXIT_SUCCESS ? status::success : (code == EXIT_FAILURE ? status::unsuccessful : static_cast<ntl::nt::ntstatus>(code)) );
}
#endif

/************************************************************************/
/* `abort`                                                              */
/************************************************************************/
void _cdecl abort()
{
  ntl::nt::user_thread::exit_process(ntl::nt::status::unsuccessful);
}

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
#include "../nt/debug.hxx" // for abort implementation
extern "C" int _cdecl _purecall(void)
{
#pragma warning(disable:4127)
  assert(!"pure virtual function called!");
#pragma warning(default:4127)
  debug_abort();
  abort();
  return 0;
}
#endif