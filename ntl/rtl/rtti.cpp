#include "../stlx/cstddef.hxx"
#include "../stlx/typeinfo.hxx"

#include "../nt/peb.hxx"
#include "../pe/image.hxx"

#define CRT_UNDNAME

#ifdef CRT_UNDNAME
# include "../nt/new.hxx"
namespace x
{
  void* __cdecl malloc(size_t size)
  {
    return new char[size];
  }

  void __cdecl free(void* p)
  {
    delete p;
  }
}
#endif

namespace detail
{
#ifndef CRT_UNDNAME
  typedef size_t __stdcall undname_t(const char* signame, char* name, size_t length, uint32_t flags);
#else
  typedef void* __cdecl alloc_t(size_t);
  typedef void  __cdecl free_t(void*);
  typedef char* __cdecl undname_t(char* name, const char* signame, int max_size, alloc_t*, free_t*, uint16_t flags);
#endif

  static undname_t* undname = NULL;

  bool init_undname()
  {
    using namespace ntl;
    typedef pe::image* __stdcall dlopen_t(const char*);
    nt::peb::find_dll fd(&nt::peb::instance());
    const pe::image* kernel = fd("kernel32.dll");
    dlopen_t* dlopen = kernel->find_export<dlopen_t*>("LoadLibraryA");
#ifdef CRT_UNDNAME
    const pe::image* crt = dlopen("msvcrt.dll");
    if(crt)
      undname = crt->find_export<undname_t*>("__unDName");
#else
    const pe::image* imagehlp = dlopen("imagehlp.dll");
    if(imagehlp)
      undname = imagehlp->find_export<undname_t*>("UnDecorateSymbolName");
#endif
    return undname != NULL;
  }
}

const char* undname(const char* signame)
{
  if(!detail::undname)
    if(!detail::init_undname())
      return signame;

  static const uint16_t flags = 0x2000; /* no_arguments */

#ifndef CRT_UNDNAME

  char buf[4096];
  size_t len = detail::undname(signame+1, buf, sizeof(buf), flags);
  if(!len)
    return signame;
  return std::strdup(buf);

#else
  return detail::undname(0, signame+1, 0, x::malloc, x::free, flags);
#endif
}

const char* std::type_info::name() const
{
  if(!data)
    data = const_cast<char*>(undname(mname));
  return reinterpret_cast<const char*>(data);
}
