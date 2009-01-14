#include "../stlx/cstdlib.hxx"

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
  exit_funcs_t* exit_list;

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
    exit_list->push_back(func);
    return func;
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

namespace ntl
{
  extern "C" void _cdecl __init_crt(bool init)
  {
    if(init){
      exit_list = new exit_funcs_t();
      initterm(__xc_a, __xc_z);
    }else{
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
extern "C" int _cdecl _purecall(void)
{
  assert(!"pure virtual function called!");
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