#include "../stlx/cstddef.hxx"
#include "../stlx/typeinfo.hxx"

#include "../pe/image.hxx"
#include "../nt/exception.hxx"
#include "../nt/status.hxx"
#include "../nt/peb.hxx"

#pragma region typeinfo::name implementation

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

namespace __
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
} // namespace __

namespace {
  const char* undname(const char* signame)
  {
    if(!__::undname)
      if(!__::init_undname())
        return signame;

    static const uint16_t flags = 0x2000; /* no_arguments */

#ifndef CRT_UNDNAME
    char buf[4096];
    size_t len = __::undname(signame+1, buf, sizeof(buf), flags);
    if(!len)
      return signame;
    return std::strdup(buf);
#else
    return __::undname(0, signame+1, 0, x::malloc, x::free, flags);
#endif
  }
} // namespace

//////////////////////////////////////////////////////////////////////////
const char* std::type_info::name() const
{
  if(!data)
    data = const_cast<char*>(undname(mname));
  return reinterpret_cast<const char*>(data);
}

#pragma endregion

#pragma region dynamic_cast implementation

namespace ptr
{
  template<typename T, typename U>
  inline T round_to_size(T size, U alignment)
  {
    return (T)( (uintptr_t(size) + uintptr_t(alignment) - 1) & ~(uintptr_t(alignment)-1) );
  }

  template<typename T, typename U>
  inline T padd(T p, U offset)
  {
    return (T)( uintptr_t(p) + uintptr_t(offset) );
  }

  template<typename R, typename T, typename U>
  inline R padd(T p, U offset)
  {
    return (R)( uintptr_t(p) + uintptr_t(offset) );
  }
}

namespace ntl { namespace cxxruntime
{
  #pragma pack(push, ehdata, 4)

  typedef ntl::nt::exception::rva_t rva_t;

  /// pointer to member descriptor
  struct pmd // _PMD
  {
    rva_t   member_offset;
    rva_t   vbtable_offset; // -1 if not a virtual base
    rva_t   vdisp_offset;   // offset to the displacement value inside the vbtable

    template<class T>
    T* operator()(T* obj) const
    {
      ptrdiff_t off = 0;
      if(vbtable_offset >= 0){
        // *(ptrdiff_t*)((char*)*(ptrdiff_t*)((char*)pThis + RetOff) + pmd.vdisp);
        off = vbtable_offset + *reinterpret_cast<const ptrdiff_t*>(*reinterpret_cast<const ptrdiff_t*>(ptr::padd(obj, vbtable_offset)) + vdisp_offset);
      }
      off += member_offset;
      return ptr::padd(obj, off);
    }
  };

  typedef void generic_function_t();

  #pragma pack(push, _TypeDescriptor, 8)
  struct typeinfo
  {
    const void* vptr;
    mutable void* spare;
    char name[1];
  };
  #pragma pack(pop, _TypeDescriptor)
  #pragma pack(pop, ehdata)

  #pragma pack(push, rttidata, 4)

  struct class_hierarchy;

  union typeinfos
  {
    const typeinfo& rawtype;
    const std::type_info& type;
  };

  inline bool operator==(const typeinfo& l, const typeinfo& r)
  {
    return &l == &r || reinterpret_cast<const std::type_info&>(l) == reinterpret_cast<const std::type_info&>(r);
  }

  struct base_class
  {
#ifndef _M_X64
    typeinfos   type;
#else
    rva_t       type;
#endif
    uint32_t    bases;
    pmd         thiscast;
    //uint32_t  attributes;
    uint32_t    notvisible:1;
    uint32_t    ambiguous:1;
    uint32_t    privcomp:1;
    uint32_t    privbase:1;
    uint32_t    vbofcontobj:1;
    uint32_t    nonpolymorphic:1;
  };

  struct base_class2: base_class
  {
    const class_hierarchy* hierarchy;
  };

  struct base_class_array
  {
#ifndef _M_X64
    const base_class2* bases[1];
#else
    rva_t              bases[1];
#endif
  };

  struct class_hierarchy
  {
    uint32_t    signature;
    //uint32_t  attributes;
    uint32_t    ismultiple:1;
    uint32_t    isvirtual:1;
    uint32_t    isambiguous:1;
    uint32_t    bases;
#ifndef _M_X64
    const base_class_array* classes;
#else
    rva_t       classes;
#endif
  };

  struct object_locator
  {
    uint32_t  signature;
    int32_t   offset;
    int32_t   cdoffset;
#ifndef _M_X64
    typeinfos type;
    const class_hierarchy* hierarchy;
#else
    rva_t     type;
    rva_t     hierarchy;
#endif
    static const object_locator& instance(const void* object)
    {
      return *reinterpret_cast<const object_locator*>( ((*((void***)object))[-1]) );
    }

    const void* find_object(const void* object) const
    {
      intptr_t complete = ptr::padd<intptr_t>(object, -offset);
      if(cdoffset)
#ifndef _M_X64
        complete +=
#else
        complete -=
#endif
        *reinterpret_cast<const rva_t*>( ptr::padd(object, -cdoffset) );
      return reinterpret_cast<const void*>(complete);
    }

    const base_class2* find_instance(const void* complete, ptrdiff_t ptrdelta, const typeinfo& srctype, const typeinfo& desttype, const pe::image* imagebase = NULL) const
    {
      const class_hierarchy* hierarchy =
#ifndef _M_X64
        this->hierarchy;
#else
        imagebase->va<const class_hierarchy*>(this->hierarchy);
#endif

      if(!hierarchy->ismultiple)
        return find_single_instance(srctype, desttype, imagebase);
      else if(!hierarchy->isvirtual)
        return find_multiple_instance(complete, ptrdelta, srctype, desttype, imagebase);
      else
        return find_virtual_instance(complete, ptrdelta, srctype, desttype, imagebase);
    }

  protected:
    const base_class2* find_single_instance(const typeinfo& srctype, const typeinfo& desttype, const pe::image* imagebase) const
    {
#ifndef _M_X64
      const base_class2* base, * const * ptr = hierarchy->classes->bases;
      for(unsigned i = 0; i < hierarchy->bases; i++, ptr++){
        base = *ptr;
        if(base->type.rawtype == desttype){
          // assume that no more than one base classes are exists,
          // i.e. only one base object exists, which type equals to the target type, and it [base object] can be private,
          // so we can't give access to it.
          if(!base->notvisible)
            return base;
          break;
        }
      }
#else
      const class_hierarchy* hierarchy = imagebase->va<const class_hierarchy*>(this->hierarchy);
      const rva_t* bases = imagebase->va<const rva_t*>( hierarchy->classes );
      for(unsigned i = 0; i < hierarchy->bases; i++){
        const base_class2* base = imagebase->va<const base_class2*>(bases[i]);
        const typeinfo* type = imagebase->va<const typeinfo*>(base->type);
        if(*type == desttype){
          // on x64 behaviour should be such as find_multiple_instance
          for(unsigned j = i + 1; j < hierarchy->bases; j++){
            const base_class2* base2 = imagebase->va<const base_class2*>(bases[i]);
            type = imagebase->va<const typeinfo*>(base2->type);
            if(*type == srctype)
              return base;
          }
        }
      }
#endif
      return NULL;
    }

    const base_class2* find_multiple_instance(const void* complete, ptrdiff_t ptrdelta, const typeinfo& srctype, const typeinfo& desttype, const pe::image* imagebase) const
    {
      __debugbreak();
      return 0;
    }
    const base_class2* find_virtual_instance(const void* complete, ptrdiff_t ptrdelta, const typeinfo& srctype, const typeinfo& desttype, const pe::image* imagebase) const
    {
      __debugbreak();
      return 0;
    }
  };

  struct object_locator2: object_locator
  {
    const object_locator2* self;

    static const object_locator2& instance(const void* object)
    {
      return *reinterpret_cast<const object_locator2*>( ((*((void***)object))[-1]) );
    }

    void validate() const
    {
      int reader = 0;
      for(const char* p = reinterpret_cast<const char*>(this);
        p < reinterpret_cast<const char*>(this) + sizeof(*this);
        p++)
      {
        reader += *p;
      }
    }
  };

  #pragma pack(pop, rttidata)



}} // namespaces

using namespace ntl;

namespace {

  const void* __cdecl __RTDynamicCast(const void* object, int32_t vfdelta, const ntl::cxxruntime::typeinfo& srctype, const ntl::cxxruntime::typeinfo& desttype, bool isreference)
  {
    using namespace ntl::cxxruntime;
    __try {
      const pe::image* imagebase =
#ifdef _M_X64
        pe::image::base_from(object);
#else
        NULL;
#endif
      const object_locator2& locator = object_locator2::instance(object);
      locator.validate();

      const void* complete = locator.find_object(object);

      // adjust object ptr by vptr diplacement
      object = ptr::padd(object, -vfdelta);
      const base_class2* base = locator.find_instance(complete, (const char*)object - (const char*)complete, srctype, desttype, imagebase);

      const void* result;
      if(base)
        result = base->thiscast(complete);
      else {
        result = NULL;
        if(isreference)
          __ntl_throw(std::bad_cast(/*"Bad dynamic_cast<>"*/));
      }
      return result;
    }
    __except(exception_code == nt::status::access_violation ? exception_execute_handler : exception_continue_search){
      __ntl_throw(std::bad_typeid(/*"A typeid of bad pointer attempted"*/));
      return NULL;
    }
  }
} // namespace


// extern "C" void* __cdecl __RTCastToVoid (const void* object)
extern "C" void* __cdecl __RTCastToVoid (void* object) __ntl_throws(...)
{
  if(!object)
    return NULL;

  __try {
    const cxxruntime::object_locator2& locator = cxxruntime::object_locator2::instance(object);
    locator.validate();
    return (void*)locator.find_object(object);
  }
  __except(exception_code == nt::status::access_violation ? exception_execute_handler : exception_continue_search){
    __ntl_throw(std::bad_typeid(/*"A typeid of bad pointer attempted"*/));
    return NULL;
  }
}

// extern "C" const std::type_info* __cdecl __RTtypeid(const void* object)
extern "C" void* __cdecl __RTtypeid(void* object) __ntl_throws(...)
{
  if(!object){
    __ntl_throw(std::bad_typeid(/*"A typeid of NULL pointer attempted"*/));
    return NULL;
  }
  __try{
    const cxxruntime::object_locator2& locator = cxxruntime::object_locator2::instance(object);
    locator.validate();
#ifndef _M_X64
    return (void*)&locator.type.type;
#else
    const pe::image* pe;
    if(!locator.type || !(pe = pe::image::base_from(object))){
      __ntl_throw(std::bad_typeid(/*"A typeid of bad pointer attempted"*/));
      return NULL;
    }
    return pe->va<void*>(locator.type);
#endif
  }
  __except(exception_code == nt::status::access_violation ? exception_execute_handler : exception_continue_search){
    __ntl_throw(std::bad_typeid(/*"A typeid of bad pointer attempted"*/));
    return NULL;
  }
}

extern "C" void* __cdecl __RTDynamicCast(void* object, int32_t vfdelta, void* srctype, void* desttype, int isreference)
{
  if(!object)
    return NULL;

  return (void*)__RTDynamicCast(object, vfdelta, reinterpret_cast<ntl::cxxruntime::typeinfo&>(srctype), reinterpret_cast<ntl::cxxruntime::typeinfo&>(desttype), isreference != 0);
}

#pragma endregion

