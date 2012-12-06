//#pragma warning(disable:4820 4061)
#include "../stlx/cstdio.hxx"
#include "../stlx/cstdlib.hxx"
#include "../nt/exception.hxx"
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

#ifdef NTL_SUBSYSTEM_KM
# include "../km/new.hxx"
#else
# include "../nt/new.hxx"
# include "../nt/thread.hxx"
#endif
using namespace std;
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
extern "C" void __cdecl __init_iostream_objects_stub(){}


namespace ntl
{

  extern "C" void __cdecl __init_crt(bool init)
  {
    if(init){
      // init exception support on current thread
      #if STLX_USE_EXCEPTIONS
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
extern "C" int __cdecl atexit(vfv_t func)
{
  return onexit(func) ? 0 : -1;
}

extern "C" int __cdecl at_quick_exit(vfv_t* f)
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

void NTL_CRTCALL exit(int status)
{
  doexit(status, false, true);
  _Exit(status);
}

void NTL_CRTCALL quick_exit(int status)
{
  doexit(status, true,true);
  _Exit(status);
}

#ifndef NTL_SUBSYSTEM_KM
void NTL_CRTCALL _Exit(int code)
{
  using ntl::nt::status;
  ntl::nt::this_thread::exit_process(code == EXIT_SUCCESS ? status::success : (code == EXIT_FAILURE ? status::unsuccessful : static_cast<ntl::nt::ntstatus>(code)) );
}
#endif

/************************************************************************/
/* `abort`                                                              */
/************************************************************************/
//#ifndef NTL_SUBSYSTEM_KM
extern "C" void __cdecl abort()
{
#pragma warning(disable:4127)
  assert(false);
#pragma warning(default:4127)
  //raise(SIGABRT);
  //ntl::nt::user_thread::exit_process(ntl::nt::status::unsuccessful);
}
//#endif
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

std::new_handler std::get_new_handler() __ntl_nothrow
{
  return ntl::__new_handler;
}

/************************************************************************/
/* `terminate` handler                                                  */
/************************************************************************/
std::terminate_handler __ntl_std_terminate_handler = 0;

namespace std
{
  terminate_handler get_terminate() __ntl_nothrow
  {
    return __ntl_std_terminate_handler;
  }

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

extern "C" int __cdecl _purecall(void)
{
  _assert_msg("pure virtual function called!");
  debug_abort();
  abort();
  return 0;
}
#endif



#if _MSC_VER >= 1600

#include <pe/image.hxx>
extern "C" void __fastcall __security_check_cookie(std::uintptr_t cookie)
{
  (void)cookie;
  assert(cookie != __security_cookie);
}

extern "C" const ntl::pe::image::section_header* __cdecl _FindPESection(ntl::pe::image* pe, std::uintptr_t rva)
{
  using ntl::pe::image;
  const image::nt_headers* nth = pe->get_nt_headers();
  for(uint32_t i = 0; i < nth->FileHeader.NumberOfSections; i++){
    const image::section_header* sec = pe->get_section_header(i);
    if(rva >= sec->VirtualAddress && rva < sec->VirtualAddress+sec->VirtualSize)
      return sec;
  }
  return nullptr;
}

extern "C" unsigned __cdecl _ValidateImageBase(std::uintptr_t base)
{
  using ntl::pe::image;
  const image* pe = image::bind(base);
  return pe->get_dos_header()->is_valid() && pe->get_nt_headers()->is_valid() 
    && (pe->get_nt_headers()->optional_header32()->is_valid() || pe->get_nt_headers()->optional_header64()->is_valid());
}

// prevent removing functions written above by VC11's optimizer.
volatile bool cookie_used = &__security_check_cookie && &_FindPESection && &_ValidateImageBase;

#endif // _MSC_VER >= 1600
