/**\file*********************************************************************
 *                                                                     \brief
 *  18.8 C++0x Exception handling [support.exception]
 *
 ****************************************************************************
 */
#pragma once

#define NTL_EH_RUNTIME

#include "../nt/exceptions.hxx"
#include "exception.hxx"

#ifdef NTL__CXX_RV
# include "smart_ptr_rv.hxx"
#else
# include "smart_ptr.hxx"
#endif

#if STLX__USE_EXCEPTIONS == 0
# pragma push_macro("__noreturn")
# define __noreturn
#endif

#ifdef NTL__CXX_RV
# define __rvtype(T) T&&
#else
# define __rvtype(T) const T&
#endif

#if STLX__USE_EXCEPTIONS == 1
namespace ntl
{
  namespace intrinsic
  {
    extern "C" void* __cdecl _alloca(size_t size);
  }

  namespace cxxruntime
  {
    struct exception_ptr:
      noncopyable
    {
      exception_record ehrec;

      exception_ptr()
      {
        std::memset(&ehrec, 0, sizeof(exception_record));
      }
      explicit exception_ptr(const exception_record& er)
      {
        std::memset(&ehrec, 0, sizeof(exception_record));
        ehrec.ExceptionCode = er.ExceptionCode;
        ehrec.ExceptionFlags = er.ExceptionFlags;
        ehrec.NumberParameters = er.NumberParameters;
        std::memcpy(ehrec.ExceptionInformation, er.ExceptionInformation, er.NumberParameters*sizeof(uintptr_t));

        const cxxrecord& cxx = static_cast<const cxxrecord&>(er);
        if(cxx.is_msvc()){
          ehrec.ExceptionInformation[1] = 0; // reset throwobject
          const throwinfo* info = cxx.get_throwinfo();
          if(cxx.get_object() && info && info->catchabletypearray){
            const catchabletypearray* types = cxx.thrown_va<catchabletypearray*>(info->catchabletypearray);
            assert(types->size > 0);
            const catchabletype& type = *cxx.thrown_va<catchabletype*>(types->type[0]);
            byte* obj = new byte[type.object_size]; // malloc
            ehrec.ExceptionInformation[1] = copyThrownObject(cxx, type, obj);
          }else{
            nt::exception::inconsistency();
          }
        }
      }

      __noreturn
        void rethrow()
      {
        if(this == nullptr){
          __ntl_throw(std::bad_exception());
        }else{
          cxxrecord& cxx = static_cast<cxxrecord&>(ehrec);
          exception_record rec = ehrec;
          if(!cxx.is_msvc()){
            rec.raise();
          }else{
            assert(cxx.get_object() && cxx.get_throwinfo());
            const throwinfo* info = cxx.get_throwinfo();
            assert(info->catchabletypearray != 0);
            const catchabletypearray* types = cxx.thrown_va<const catchabletypearray*>(info->catchabletypearray);
            assert(types->size != 0);
            const catchabletype* type = cxx.thrown_va<const catchabletype*>(types->type[0]);

            void* objplace = intrinsic::_alloca(type->object_size);
            rec.ExceptionInformation[1] = copyThrownObject(cxx, *type, objplace);
            rec.raise();
          }

        }
      }

    protected:
      uintptr_t copyThrownObject(const cxxrecord& cxx, const catchabletype& type, void* obj)
      {
        __try{
          if(type.memmoveable || !type.copyctor){
            std::memcpy(obj, cxx.get_object(), type.object_size);
          }else{
            eobject *objthis = reinterpret_cast<eobject*>(obj), 
              *copyarg = cxx.adjust_pointer(cxx.get_object(), &type);
#ifdef _M_X64
            void* copyctor = cxx.thrown_va(type.copyctor);
            if(type.hasvirtbase){
              // virtual type
              (eobject::ctor_ptr2(copyctor))(objthis, copyarg, 1);
            }else{
              // nonvirtual
              (eobject::ctor_ptr(copyctor))(objthis, copyarg);
            }
#else
            if(type.hasvirtbase)
              (objthis->*type.copyctor2)(*copyarg, 1);
            else
              (objthis->*type.copyctor)(*copyarg);
#endif
          }
        }
        __except(exception_execute_handler){
          nt::exception::inconsistency();
        }
        return reinterpret_cast<uintptr_t>(obj);
      }

    public:
      static exception_ptr* current_exception()
      {
        exception_ptr* ptr = nullptr;
        tiddata* ptd = _getptd_noinit();
        if(ptd && ptd->curexception.ExceptionRecord && ptd->processingThrow == 0
          && ptd->curexception.ExceptionRecord->ExceptionCode != nt::status::com_exception
          && ptd->curexception.ExceptionRecord->ExceptionCode != nt::status::complus_exception)
        {
          ptr = new exception_ptr(*ptd->curexception.ExceptionRecord);
        }
        return ptr;
      }
      static void delete_exception(exception_ptr* ptr)
      {
        cxxrecord& cxx = static_cast<cxxrecord&>(ptr->ehrec);
        if(cxx.is_msvc()){
          const throwinfo* info = cxx.get_throwinfo();
          assert(cxx.get_throwinfo());
          if(info->exception_dtor && cxx.get_object()){
            cxx.destruct_eobject(false);
          }
          delete[] reinterpret_cast<byte*>(cxx.get_object());
        }
      }
    };
  }
}
#elif STLX__USE_EXCEPTIONS

namespace ntl { namespace cxxruntime {
  struct exception_ptr
  {
    operator explicit_bool_type() const { return explicit_bool(false); }
  };

  inline bool operator==(const exception_ptr&  , const exception_ptr&  ) { return false; }
  inline bool operator!=(const exception_ptr& x, const exception_ptr& y) { return !(x == y); }
  //bool operator==(const exception_ptr&  , const nullptr_t&  ) { return false; }
  //bool operator!=(const exception_ptr& x, const nullptr_t& y) { return !(x == y); }
  //bool operator==(const nullptr_t&  , const exception_ptr&  ) { return false; }
  //bool operator!=(const nullptr_t& x, const exception_ptr& y) { return !(x == y); }
}}

#endif

namespace std
{
/**\addtogroup  lib_language_support *** 18 Language support library [language.support]
 *@{*/

/**\defgroup  lib_support_exception **** 18.8 Exception handling [support.exception]
 *@{*/

  ///\name 18.8.5 Exception Propagation [propagation]

  /**
   *	@brief Exception Propagation [18.8.5 propagation]
   *  @details An exception_ptr object that refers to the currently handled exception (15.3) 
   *  or a copy of the currently handled exception, or a \c null exception_ptr object if no exception is being handled.
   *  The referenced object shall remain valid at least as long as there is an exception_ptr object that refers to it. 
   **/
  typedef std::shared_ptr<ntl::cxxruntime::exception_ptr> exception_ptr;

  inline exception_ptr current_exception();

  inline void rethrow_exception(exception_ptr e);

  template<class E>
  inline exception_ptr copy_exception(E e)
  {
  #if STLX__USE_EXCEPTIONS == 1
    try {
      throw e;
    }
    catch(...){
      return current_exception();
    }
  #else
    return exception_ptr();
  #endif
  }


  ///\name 18.8.6 nested_exception [except.nested]

  /**
   *	@brief Class nested_exception [18.8.6 except.nested]
   *
   *  The class nested_exception is designed for use as a mixin through multiple inheritance.
   *  It captures the currently handled exception and stores it for later use.
   *
   *  @note nested_exception has a virtual destructor to make it a polymorphic class.
   *  Its presence can be tested for with \c dynamic_cast.
   **/
  class nested_exception
  {
  public:
    nested_exception() throw()
      :e(current_exception())
    {}

  #ifdef NTL__CXX_EF
    nested_exception(const nested_exception&) throw() = default;
    nested_exception& operator=(const nested_exception&) throw() = default;
    virtual ~nested_exception() = default;
  #else
    // default generated
    virtual ~nested_exception()
    {}
  #endif

    /** @throws exception the stored exception captured by this object */
    void rethrow_nested() const
    {
      rethrow_exception(e);
    }

    /** @return the stored exception captured by this object */
    exception_ptr nested_ptr() const { return e; }

  private:
    exception_ptr e;
  };

#if STLX__USE_EXCEPTIONS == 1

  exception_ptr current_exception()
  {
    return exception_ptr(ntl::cxxruntime::exception_ptr::current_exception(), &ntl::cxxruntime::exception_ptr::delete_exception);
  }

  void rethrow_exception(exception_ptr e)
  {
    e->rethrow();
  }

  namespace __
  {
    template<class E>
    inline const nested_exception* get_nested(const E& e, true_type)
    {
      return dynamic_cast<const nested_exception*>(e);
    }
    template<class E>
    inline const nested_exception* get_nested(const E& e, false_type)
    {
      return dynamic_cast<const nested_exception*>(&e);
    }

    template<class T>
    __noreturn
    inline void throw_nested_impl(__rvtype(T) t, const nested_exception*)
    {
      __ntl_throw(t);
    }

    template<class T>
    __noreturn
    inline void throw_nested_impl(__rvtype(T) t, ...)
    {
      struct nested_pimp:
        public T, nested_exception
      {
        explicit nested_pimp(__rvtype(T) t)
          :T(static_cast<__rvtype(T)>(t))
        {}
      };
      __ntl_throw(nested_pimp(static_cast<__rvtype(T)>(t)));
    }
  }

  template<class T>
  __noreturn 
  inline typename enable_if<is_class<T>::value>::type
  throw_with_nested(__rvtype(T) t)
  {
    if(__::get_nested(t, is_pointer<T>()))
      throw t;
    else
      __::throw_nested_impl(static_cast<__rvtype(T)>(t));
  }

  template<class T>
  __noreturn 
  inline typename enable_if<!is_class<T>::value>::type
  throw_with_nested(__rvtype(T) t)
  {
    throw t;
  }


  /** Calls \c e.rethrow_nested() only if \p e is publicly derived from nested_exception */
  template <class E>
  inline void rethrow_if_nested(const E& e)
  {
    if(const nested_exception* nex = __::get_nested(e, is_pointer<E>()))
      nex->rethrow_nested();
  }

  /** Calls \c e.rethrow_nested() */
  inline void rethrow_if_nested(const nested_exception& e)
  {
    e.rethrow_nested();
  }

  ///\}

#elif !STLX__USE_EXCEPTIONS
  inline exception_ptr current_exception() { return exception_ptr(); }

  inline void rethrow_exception(exception_ptr){}

  template<class T>
  __noreturn 
  inline void throw_with_nested(__rvtype(T)){}

  template <class E>
  inline void rethrow_if_nested(const E&){}
#endif

#if STLX__USE_EXCEPTIONS == 0
# pragma pop_macro("__noreturn")
#endif
#undef __rvtype

  /**@} lib_support_exception */
  /**@} lib_language_support */
} // std
