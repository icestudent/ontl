#include "../stlx/cstdio.hxx"
#include "../stlx/cstdlib.hxx"

#include "../nt/peb.hxx"
#include "../pe/image.hxx"

#include "../stlx/cmath.hxx"

namespace
{
  // CRT stubs
  vfv_t *imp_strtod, *imp_vsnprintf, *imp_acos, *imp_atan, *imp_tan;
}

// CRT forwards
double NTL_CRTCALL strtod(const char * __restrict nptr, char ** __restrict endptr)
{
  typedef double __cdecl f_t(const char*, char**);
  return reinterpret_cast<f_t*>(imp_strtod)(nptr, endptr);
}

float NTL_CRTCALL strtof(const char * __restrict nptr, char ** __restrict endptr)
{
  return (float)strtod(nptr, endptr);
}

long double NTL_CRTCALL strtold(const char * __restrict nptr, char ** __restrict endptr)
{
  return strtod(nptr, endptr);
}

double NTL_CRTCALL atof(const char *nptr)
{
  char* end;
  return strtod(nptr, &end);
}

#define FPFUNC(func, type) \
  type NTL_CRTCALL std::func(type d)\
  {\
    typedef type __cdecl f_t(type);\
    return reinterpret_cast<f_t*>(imp_ ## func)(d);\
  }

  FPFUNC(acos, float)
  FPFUNC(atan, float)
  FPFUNC(tan,  float)
  FPFUNC(acos, double)
  FPFUNC(atan, double)
  FPFUNC(tan,  double)


namespace ntl { namespace msvcrt
{
  size_t NTL_CRTCALL snprintf(char *buffer, size_t count, const char *format, ...)
  {
    va_list va;
    va_start(va, format);
    return vsnprintf(buffer, count, format, va);
  }
  size_t NTL_CRTCALL vsnprintf(char *buffer, size_t count, const char *format, va_list argptr)
  {
    typedef size_t __cdecl f_t(char*, size_t, const char*, va_list);
    return reinterpret_cast<f_t*>(imp_vsnprintf)(buffer, count, format, argptr);
  }

  //int NTL_CRTCALL sscanf(const char* string, const char* format, ...)
  //{
  //  va_list va;
  //  va_start(va, format);
  //  return vsscanf(string, format, va);
  //}
  //int NTL_CRTCALL vsscanf(const char* string, const char* format, va_list argptr)
  //{
  //  typedef int __cdecl f_t(const char*, const char*, va_list);
  //  return reinterpret_cast<f_t*>(imp_vsscanf)(string, format, argptr);
  //}
}}

namespace
{
  const ntl::pe::image* bind_msvcrt();

  static const ntl::pe::image* msvcrt = bind_msvcrt();

  const ntl::pe::image* bind_msvcrt()
  {
    using namespace ntl;
    using namespace ntl::nt;
    if(msvcrt)
      return msvcrt;

    // load dll
    static const char* const dllname = "msvcrt.dll";
    peb::find_dll fd(&peb::instance());
    msvcrt = fd(dllname);
    if(!msvcrt){
      const pe::image* kernel = fd("kernel32.dll");
      if(!kernel)
        return nullptr;
      typedef ntl::pe::image* __stdcall load_library_t(const char* dll);
      load_library_t* dlopen = kernel->find_export<load_library_t*>("LoadLibraryA");
      msvcrt = dlopen("msvcrt.dll");
    }
    if(!msvcrt)
      return nullptr;

    // bind imports
#define NTL_IMP(func) imp_ ## func = msvcrt->find_export<vfv_t*>(#func)
    NTL_IMP(strtod);
    NTL_IMP(vsnprintf);
    NTL_IMP(acos);
    NTL_IMP(atan);
    NTL_IMP(tan);
#undef NTL_IMP

    return msvcrt;
  }
}
