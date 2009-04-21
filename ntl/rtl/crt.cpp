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
#endif

namespace
{
  typedef std::vector<vfv_t*> exit_funcs_t;
  static exit_funcs_t* exit_list = 0;

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
    }
  }
} // namespace

extern "C" void _cdecl __check_iostream_objects();
extern "C" void _cdecl __check_iostreams_stub(){}
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
      exit_list->reserve(__xc_z - __xc_a);
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

//////////////////////////////////////////////////////////////////////////
int _cdecl atexit(vfv_t func)
{
  return onexit(func) ? 0 : -1;
}

//////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////
#if defined NTL__DEBUG || !defined NDEBUG
extern "C" {
  char __assertion_failure_buf[4096];
  const char* __assertion_failure = __assertion_failure_buf;
  unsigned   __assertion_failure_at_line;
}
#endif