#include "../stlx/cstddef.hxx"
#include "../stlx/typeinfo.hxx"

#include "../nt/new.hxx"
#include "../nt/peb.hxx"
#include "../pe/image.hxx"

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

namespace detail
{
  typedef void* __cdecl alloc_t(size_t);
  typedef void  __cdecl free_t(void*);
  typedef char* __cdecl undname_t(char* name, const char* signame, int max_size, alloc_t*, free_t*, uint16_t flags);
  static undname_t* undname = NULL;

  bool init_undname()
  {
    using namespace ntl;
    typedef pe::image* __stdcall dlopen_t(const char*);
    nt::peb::find_dll fd(&nt::peb::instance());
    const pe::image* kernel = fd("kernel32.dll");
    dlopen_t* dlopen = kernel->find_export<dlopen_t*>("LoadLibraryA");
    const pe::image* crt = dlopen("msvcrt.dll");
    if(crt)
      undname = crt->find_export<undname_t*>("__unDName");
    return undname != NULL;
  }
}

const char* undname(const char* signame)
{
  if(!detail::undname)
    if(!detail::init_undname())
      return signame;

  return detail::undname(0, signame+1, 0, x::malloc, x::free, 0x2000 /* no_arguments */
#ifdef _M_IX86
      |0x800 /* 32bit decode */
#endif
      );
}

const char* std::type_info::name() const
{
  if(!data)
    data = const_cast<char*>(undname(mname));
  return reinterpret_cast<const char*>(data);
}
