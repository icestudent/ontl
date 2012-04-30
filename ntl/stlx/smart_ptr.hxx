/**\file*********************************************************************
 *                                                                     \brief
 *  /// 20.8.11 Class template unique_ptr for legacy compilers [unique.ptr]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_SMARTPTR
#define NTL__STLX_SMARTPTR
#pragma once

#include "memory.hxx"
#include "utility.hxx"

#include "../basedef.hxx"
#include "../linked_ptr.hxx"
#include "reference_wrapper.hxx"

#ifdef NTL_CXX_RV
#pragma error use smart_ptr_rv.hxx
#endif

namespace std
{
  /**\addtogroup  lib_utilities ********** 20 General utilities library [utilities]
   *@{*/
  /**\addtogroup  lib_memory ************* 20.8 Memory [memory]
   *@{*/

  /**\addtogroup  lib_uniqueptr ********** 20.8.11 Class template unique_ptr (legacy) [unique.ptr]
   *@{*/


  // 20.8.11 Class template unique_ptr [unique.ptr]

  ///\name 20.8.11.1 Default deleters [unique.ptr.dltr]

  /// 20.8.11.1.1 default_delete [unique.ptr.dltr.dflt]
  template <class T> struct default_delete
  {
    default_delete() {}
    template <class U> default_delete(const default_delete<U>&) {}
    void operator()(T* ptr) const
    {
      static_assert(sizeof(T) > 0, "incomplete type of T is not allowed");
      ::delete ptr;
    }
  private:
    // forbid incompatible ::delete[]
    template <class U> default_delete(const default_delete<U[]>&);
    template <class U, size_t S> default_delete(const default_delete<U[S]>&);
  };

  /// 20.8.11.1.2 default_delete<T[]> [unique.ptr.dltr.dflt1]
  template <class T> struct default_delete<T[]>
  {
    void operator()(T* ptr) const
    {
      static_assert(sizeof(T) > 0, "incomplete type of T is not allowed");
      ::delete[] ptr;
    }
  private:
    template <class U> void operator()(U*) const __deleted;
  };

#ifdef STLX__SMARTPTR_WITH_N
  /// 20.8.11.1.3 default_delete<T[N]> [unique.ptr.dltr.dflt2]
  template <class T, size_t N> struct default_delete<T[N]>
  {
    void operator()(T* ptr, size_t) const { ::delete[] ptr; }
  };
#endif

  ///\}

  template <class T, class D = default_delete<T> >
  class unique_ptr;

  /// 20.8.11.2 unique_ptr for single objects [unique.ptr.single]
  template <class T, class D>
  class unique_ptr
  {
    typedef const T* unique_ptr<T, default_delete<T> >::* unspecified_pointer_type;
    ///////////////////////////////////////////////////////////////////////////
  public:

    typedef T* pointer;
    typedef T  element_type;
    typedef D  deleter_type;

    ///\name 20.8.11.2.1 unique_ptr constructors [unique.ptr.single.ctor]
    unique_ptr() __ntl_nothrow
      : ptr(0), deleter()
    {
      static_assert(!(is_reference<D>::value || is_pointer<D>::value), "D shall not be a reference type or pointer type");
    }

    explicit unique_ptr(pointer p) __ntl_nothrow
      : ptr(p), deleter()
    {
      static_assert(!(is_reference<D>::value || is_pointer<D>::value), "D shall not be a reference type or pointer type");
    }

    unique_ptr(nullptr_t) __ntl_nothrow
      :ptr(0), deleter()
    {}

    unique_ptr(pointer p, typename conditional<is_reference<deleter_type>::value, deleter_type, typename add_lvalue_reference<const typename remove_reference<deleter_type>::type>::type>::type d) __ntl_nothrow
      : ptr(p), deleter(d)
    {}

    unique_ptr(const unique_ptr& u) __ntl_nothrow
      : ptr(u.get()), deleter(forward<D>(u.deleter))
    {
      u.release();
    }

    template <class U, class E>
    unique_ptr(const unique_ptr<U, E>& u) __ntl_nothrow
      : ptr(u.get()), deleter(forward<D>(u.get_deleter()))
    {
      static_assert(is_array<U>::value == false, "conversion among array and non-array forms are disallowed");
      static_assert((!is_reference<deleter_type>::value || is_same<deleter_type, E>::value), "If D is a reference type, then E shall be the same type as D");
      u.release();
    }

    ///\name 20.8.11.2.2 unique_ptr destructor [unique.ptr.single.dtor]
    ~unique_ptr() __ntl_nothrow
    {
      if ( get() )
        get_deleter()(get());
    }

    ///\name 20.8.11.2.3 unique_ptr assignment [unique.ptr.single.asgn]
    __forceinline
      unique_ptr& operator=(const unique_ptr& u) __ntl_nothrow
    {
      reset(u.release());
      deleter = move(u.deleter);
      return *this;
    }

    template <class U, class E>
    __forceinline
      unique_ptr& operator=(const unique_ptr<U, E>& u) __ntl_nothrow
    {
      static_assert(is_array<U>::value == false, "conversion among array and non-array forms are disallowed");
      reset(u.release());
      deleter = move(u.get_deleter());
      return *this;
    }

    unique_ptr& operator=(unspecified_pointer_type *)
    {
      reset();
      return *this;
    }

    ///\name 20.8.11.2.4 unique_ptr observers [unique.ptr.single.observers]
    typename add_lvalue_reference<T>::type operator*() const __ntl_nothrow { return *ptr; }
    pointer operator->() const __ntl_nothrow { return ptr; }
    pointer get() const __ntl_nothrow { return ptr; }

    typename add_lvalue_reference<deleter_type>::type get_deleter() __ntl_nothrow { return deleter; }
    typename add_lvalue_reference<const typename remove_reference<deleter_type>::type>::type get_deleter() const __ntl_nothrow { return deleter; }

    operator __::explicit_bool_type() const __ntl_nothrow { return __::explicit_bool(ptr); }

    ///\name 20.8.11.2.5 unique_ptr modifiers [unique.ptr.single.modifiers]
    pointer release() const
    {
      pointer tmp = nullptr;
      std::swap(ptr, tmp);
      return tmp;
    }

    __forceinline
    void reset(pointer p = 0) __ntl_nothrow
    {
      if ( get() && get() != p ) get_deleter()(get());
      set(p);
    }

    void swap(unique_ptr& u) __ntl_nothrow
    {
      using std::swap;
      swap(ptr, u.ptr);
      swap(deleter, u.deleter);
    }

    ///\}

    ///////////////////////////////////////////////////////////////////////////
  private:
    mutable pointer ptr;
#ifndef __GNUC__
#ifdef __ICL
#pragma warning(disable:80) // a storage class may not be specified here
#endif
    typename conditional<is_const<deleter_type>::value, deleter_type, mutable deleter_type>::type deleter;
#ifdef __ICL
#pragma warning(default:80)
#endif
#else
    mutable deleter_type deleter;
#endif

    void set(T * p) { ptr = p; }

    template<class U> void reset(U) __deleted;

  };//template class unique_ptr



  template <class T>
  class unique_ptr<T, default_delete<T> >
  {
    typedef const T* unique_ptr<T, default_delete<T> >::* unspecified_pointer_type;

    ///////////////////////////////////////////////////////////////////////////
  public:

    typedef T* pointer;
    typedef T  element_type;
    typedef default_delete<T> deleter_type;

    ///\name 20.8.11.2.1 unique_ptr constructors [unique.ptr.single.ctor]
    unique_ptr() __ntl_nothrow : ptr(0) {}

    unique_ptr(nullptr_t) __ntl_nothrow
      :ptr(0)
    {}

    explicit unique_ptr(pointer p) __ntl_nothrow : ptr(p) {}

    unique_ptr(pointer p, const deleter_type &) __ntl_nothrow : ptr(p) {}

    unique_ptr(const unique_ptr& u) __ntl_nothrow : ptr(u.get())
    {
      u.release(); // it's here to help MSVC optimizing container operations
    }

    template <class U/*, class E*/>
    unique_ptr(const unique_ptr<U, default_delete<U> >& u) __ntl_nothrow : ptr(u.get())
    {
      static_assert(is_array<U>::value == false, "conversion among array and non-array forms are disallowed");
      u.release();
    }

    ///\name 20.8.11.2.2 unique_ptr destructor [unique.ptr.single.dtor]
    ~unique_ptr() __ntl_nothrow
    {
      if ( get() )
        get_deleter()(get());
    }


    ///\name 20.8.11.2.3 unique_ptr assignment [unique.ptr.single.asgn]
    __forceinline
      unique_ptr& operator=(const unique_ptr& u) __ntl_nothrow
    {
      reset(u.release());
      return *this;
    }

    template <class U/*, class E*/>
    __forceinline
      unique_ptr& operator=(const unique_ptr<U, default_delete<U> >& u) __ntl_nothrow
    {
      reset(u.release());
      return *this;
    }

    unique_ptr& operator=(unspecified_pointer_type *)
    {
      reset();
      return *this;
    }

    ///\name 20.8.11.2.4 unique_ptr observers [unique.ptr.single.observers]
    typename add_lvalue_reference<T>::type operator*() const __ntl_nothrow { return *ptr; }
    pointer operator->() const __ntl_nothrow { return ptr; }
    pointer get() const __ntl_nothrow { return ptr; }

    // local statics produce code bloat, shoud we replace the UD with a global static?
    deleter_type& get_deleter() __ntl_nothrow { return *(deleter_type*)1; }
    const deleter_type& get_deleter() const __ntl_nothrow { return *(deleter_type*)1; }

    operator __::explicit_bool_type() const __ntl_nothrow { return __::explicit_bool(ptr); }

    ///\name 20.8.11.2.5 unique_ptr modifiers [unique.ptr.single.modifiers]
    pointer release() const
    {
      pointer tmp = nullptr;
      std::swap(ptr, tmp);
      return tmp;
    }

    __forceinline
    void reset(pointer p = 0) const __ntl_nothrow
    {
      if ( get() && get() != p ) get_deleter()(get());
      set(p);
    }

    void swap(const unique_ptr& u) const  __ntl_nothrow
    {
      std::swap(ptr, u.ptr);
    }
    ///\}

    ///////////////////////////////////////////////////////////////////////////
  private:
    mutable pointer ptr;
    void set(T * p) const  { ptr = p; }

  };//template class unique_ptr


  /// 20.8.11.3 unique_ptr for array objects with a runtime length [unique.ptr.runtime]
  template <class T, class D>
  class unique_ptr<T[], D>
  {
    typedef const T* unique_ptr<T, default_delete<T> >::* unspecified_pointer_type;
    ///////////////////////////////////////////////////////////////////////////
  public:
    typedef T* pointer;
    typedef T  element_type;
    typedef D  deleter_type;

    ///\name 20.8.11.3.1 unique_ptr constructors [unique.ptr.runtime.ctor]
    unique_ptr() __ntl_nothrow
      : ptr(0), deleter()
    {
      static_assert(!(is_reference<D>::value || is_pointer<D>::value), "D shall not be a reference type or pointer type");
    }

    explicit unique_ptr(pointer p) __ntl_nothrow
      : ptr(p), deleter()
    {
      static_assert(!(is_reference<D>::value || is_pointer<D>::value), "D shall not be a reference type or pointer type");
    }

    unique_ptr(nullptr_t) __ntl_nothrow
      :ptr(0), deleter()
    {}

    unique_ptr(pointer p, typename conditional<is_reference<deleter_type>::value, deleter_type, typename add_lvalue_reference<const typename remove_reference<deleter_type>::type>::type>::type d) __ntl_nothrow
      : ptr(p), deleter(d)
    {}

    unique_ptr(const unique_ptr& u) __ntl_nothrow
      : ptr(u.get()), deleter(forward<D>(u.deleter))
    {
      u.release();
    }

    template <class U, class E>
    unique_ptr(const unique_ptr<U, E>& u) __ntl_nothrow
      : ptr(u.get()), deleter(forward<D>(u.get_deleter()))
    {
      static_assert(is_array<U>::value == true, "conversion among array and non-array forms are disallowed");
      static_assert((!is_reference<deleter_type>::value || is_same<deleter_type, E>::value), "If D is a reference type, then E shall be the same type as D");
      u.release();
    }

    ///\name 20.8.11.2.2 unique_ptr destructor [unique.ptr.single.dtor]
    ~unique_ptr() __ntl_nothrow
    {
      if ( get() )
        get_deleter()(get());
    }


    ///\name 20.8.11.2.3 unique_ptr assignment [unique.ptr.single.asgn]
    __forceinline
      unique_ptr& operator=(const unique_ptr& u) __ntl_nothrow
    {
      reset(u.release());
      deleter = move(u.deleter);
      return *this;
    }

    unique_ptr& operator=(unspecified_pointer_type *)
    {
      reset();
      return *this;
    }

    ///\name 20.8.11.3.2 unique_ptr observers [unique.ptr.runtime.observers]
    T& operator[](size_t i) const __ntl_nothrow { return ptr[i]; }
    pointer get() const __ntl_nothrow { return ptr; }

    typename add_lvalue_reference<deleter_type>::type get_deleter() __ntl_nothrow { return deleter; }
    typename add_lvalue_reference<const typename remove_reference<deleter_type>::type>::type get_deleter() const __ntl_nothrow { return deleter; }

    operator __::explicit_bool_type() const __ntl_nothrow { return __::explicit_bool(ptr); }

    ///\name 20.8.11.2.5 unique_ptr modifiers [unique.ptr.single.modifiers]
    pointer release() const
    {
      pointer tmp = nullptr;
      std::swap(tmp, ptr);
      return tmp;
    }

    __forceinline
    void reset(pointer p = 0) const  __ntl_nothrow
    {
      if ( get() && get() != p ) get_deleter()(get());
      set(p);
    }

    void swap(unique_ptr& u) const  __ntl_nothrow
    {
      using std::swap;
      swap(ptr, u.ptr);
      swap(deleter, u.deleter);
    }

    ///\}

    ///////////////////////////////////////////////////////////////////////////
  private:
    mutable pointer ptr;
  #ifndef __GNUC__
  #ifdef __ICL
  #pragma warning(disable:80) // a storage class may not be specified here
  #endif
    typename conditional<is_const<deleter_type>::value, deleter_type, mutable deleter_type>::type deleter;
  #ifdef __ICL
  #pragma warning(default:80)
  #endif
  #else
    mutable deleter_type deleter;
  #endif


    void set(T * p) const  { ptr = p; }

    template<class Other> unique_ptr(Other*);
    template<class Other> void reset(Other*) const;

  };//template class unique_ptr


  template <class T>
  class unique_ptr<T[], default_delete<T[]> >
  {
    typedef const T* unique_ptr<T, default_delete<T> >::* unspecified_pointer_type;
    ///////////////////////////////////////////////////////////////////////////
  public:

    typedef T* pointer;
    typedef T  element_type;
    typedef default_delete<T[]> deleter_type;

    ///\name 20.8.11.3.1 unique_ptr constructors [unique.ptr.runtime.ctor]
    unique_ptr() __ntl_nothrow : ptr(0) {}

    explicit unique_ptr(pointer p) __ntl_nothrow : ptr(p) {}

    unique_ptr(nullptr_t) __ntl_nothrow
      :ptr(0)
    {}

    unique_ptr(pointer p, const deleter_type &) __ntl_nothrow : ptr(p) {}

    unique_ptr(const unique_ptr& u) __ntl_nothrow : ptr(u.get())
    {
      u.release();
    }

    template <class U/*, class E*/>
    unique_ptr(const unique_ptr<U, default_delete<U[]> >& u) __ntl_nothrow
      : ptr(u.get())
    {
      static_assert(is_array<U>::value == true, "conversion among array and non-array forms are disallowed");
      u.release();
    }

    ///\name 20.8.11.2.2 unique_ptr destructor [unique.ptr.single.dtor]
    ~unique_ptr() __ntl_nothrow
    {
      if ( get() )
        get_deleter()(get());
    }

    ///\name 20.8.11.2.3 unique_ptr assignment [unique.ptr.single.asgn]
    __forceinline
      unique_ptr& operator=(const unique_ptr& u) __ntl_nothrow
    {
      reset(u.release());
      return *this;
    }

    unique_ptr& operator=(unspecified_pointer_type *)
    {
      reset();
      return *this;
    }

    ///\name 20.8.11.3.2 unique_ptr observers [unique.ptr.runtime.observers]
    T& operator[](size_t i) const __ntl_nothrow { return ptr[i]; }
    pointer get() const __ntl_nothrow { return ptr; }

    deleter_type& get_deleter() __ntl_nothrow { return *(deleter_type*)1; }

    const deleter_type& get_deleter() const __ntl_nothrow { return *(deleter_type*)1; }

    operator __::explicit_bool_type() const __ntl_nothrow { return __::explicit_bool(ptr); }

    ///\name 20.8.11.2.5 unique_ptr modifiers [unique.ptr.single.modifiers]
    pointer release() const
    {
      pointer tmp = nullptr;
      std::swap(tmp, ptr);
      return tmp;
    }

    __forceinline
      void reset(pointer p = 0) const  __ntl_nothrow
    {
      if ( get() && get() != p ) get_deleter()(get());
      set(p);
    }

    void swap(unique_ptr& u) const  __ntl_nothrow
    {
      std::swap(ptr, u.ptr);
    }
    ///\}

    ///////////////////////////////////////////////////////////////////////////
  private:
    mutable pointer ptr;

    void set(T * p) const  { ptr = p; }

    template<class Other> unique_ptr(Other*);
    template<class Other> void reset(Other*) const;

  };//template class unique_ptr


#ifdef STLX__SMARTPTR_WITH_N
  /// unique_ptr for array objects with a compile time length (removed from N2723)
  template <class T, size_t N>
  class unique_ptr<T[N], default_delete<T[N]> >
  {
    typedef typename const unique_ptr::T** unspecified_pointer_type;

    ///////////////////////////////////////////////////////////////////////////
  public:
    typedef T* pointer;
    typedef T  element_type;
    typedef default_delete<T[N]> deleter_type;

    static const size_t size = N;

    ///\name 20.8.11.3.1 unique_ptr constructors [unique.ptr.runtime.ctor]
    unique_ptr() __ntl_nothrow : ptr(0) {}

    explicit unique_ptr(pointer p) __ntl_nothrow : ptr(p) {}

    unique_ptr(nullptr_t) __ntl_nothrow
      :ptr(0)
    {}

    unique_ptr(pointer p, const deleter_type &) __ntl_nothrow : ptr(p) {}

    unique_ptr(const unique_ptr& u) __ntl_nothrow : ptr(u.get())
    {
      u.release();
    }

    ///\name 20.8.11.4.1 unique_ptr destructor [unique.ptr.compiletime.dtor]
    ~unique_ptr() __ntl_nothrow
    {
      if ( get() )
        get_deleter()(get(), N);
    }

    ///\name 20.8.11.2.3 unique_ptr assignment [unique.ptr.single.asgn]
    __forceinline
      unique_ptr& operator=(const unique_ptr& u) __ntl_nothrow
    {
      reset(u.release());
      return *this;
    }

    unique_ptr& operator=(unspecified_pointer_type *)
    {
      reset();
      return *this;
    }

    ///\name 20.8.11.4.1 unique_ptr destructor [unique.ptr.compiletime.dtor]
    T& operator[](size_t i) const __ntl_nothrow { return ptr[i]; }
    pointer get() const __ntl_nothrow { return ptr; }

    deleter_type& get_deleter() __ntl_nothrow { return *(deleter_type*)1; }
    const deleter_type& get_deleter() const __ntl_nothrow { return *(deleter_type*)1; }

    operator __::explicit_bool_type() const __ntl_nothrow { return __::explicit_bool(ptr); }

    ///\name 20.8.11.2.5 unique_ptr modifiers [unique.ptr.single.modifiers]
    pointer release() const
    {
      pointer tmp = nullptr;
      std::swap(ptr, tmp);
      return tmp;
    }

    __forceinline
    void reset(pointer p = 0) const __ntl_nothrow
    {
      if ( get() && get() != p ) get_deleter()(get(), N);
      set(p);
    }

    __forceinline
    void swap(const unique_ptr& u) const __ntl_nothrow
    {
      pointer tmp(ptr);
      ptr = u.ptr;
      u.ptr = tmp;
    }
    ///\}

    ///////////////////////////////////////////////////////////////////////////
  private:
    enum { __actual_size = N };

    mutable pointer ptr;
    void set(T * p) const { ptr = p; }

    template<class Other> unique_ptr(Other*);
    template<class Other> void reset(Other*) const;

  };//template class unique_ptr<T[N], default_delete<T[N]> >
#endif

  ///\name 20.8.11.4 unique_ptr specialized algorithms [unique.shared.special]
  template <class T, class D> inline void swap(unique_ptr<T, D>& x, unique_ptr<T, D>& y)
  {
    x.swap(y);
  }

  template <class T1, class D1, class T2, class D2>
  inline bool operator==(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
  {
    return x.get() == y.get();
  }

  template <class T1, class D1, class T2, class D2>
  inline bool operator!=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
  {
    return x.get() != y.get();
  }

  template <class T1, class D1, class T2, class D2>
  inline bool operator<(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
  {
    return x.get() < y.get();
  }

  template <class T1, class D1, class T2, class D2>
  inline bool operator<=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
  {
    return x.get() <= y.get();
  }

  template <class T1, class D1, class T2, class D2>
  inline bool operator>(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
  {
    return x.get() > y.get();
  }

  template <class T1, class D1, class T2, class D2>
  inline bool operator>=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
  {
    return x.get() >= y.get();
  }

  ///\}
  /*@} lib_uniqueptr */

 /**\addtogroup  lib_smartptr ************* 20.8.12 Smart pointers (legacy) [util.smartptr]
   *@{*/

  template<class X> class auto_ptr;

    /** An exception of type bad_weak_ptr is thrown by the shared_ptr constructor taking a weak_ptr. */
    class bad_weak_ptr:
      public exception
    {
    public:
      bad_weak_ptr()
      {}
      virtual const char* what() const __ntl_nothrow { return "bad_weak_ptr"; }
    };

    template<class T> class weak_ptr;

    namespace __
    {
      struct shared_ptr_base:
        private std::noncopyable
      {
        long use_count, weak_count;

        explicit shared_ptr_base()
          :use_count(0), weak_count(0)
        {}
        virtual ~shared_ptr_base() __ntl_nothrow {}
        virtual void free() __ntl_nothrow = 0;
        virtual const void* get_deleter(const type_info&) const __ntl_nothrow
        {
          return nullptr;
        }
        virtual void dispose() __ntl_nothrow
        {
          delete this;
        }
      };
      template<class T>
      struct shared_ptr_data:
        shared_ptr_base
      {
        T* p;

        explicit shared_ptr_data(T* p)
          :p(p)
        {
          use_count = 1;
        }
        virtual void free() __ntl_nothrow
        {
          if(p){
            T* pp = p; p = nullptr;
            delete pp;
          }
        }
      };

      template<class T, class D>
      struct shared_ptr_deleter
        : shared_ptr_data<T>
      {
        D deleter;

        shared_ptr_deleter(T* p, const D& d)__ntl_nothrow
          :shared_ptr_data(p), deleter(d)
        {}

        void free()__ntl_nothrow
        {
          if(p){
            T* pp = p; p = nullptr;
            deleter(pp);
          }
        }
        const void* get_deleter(const type_info& ti) const __ntl_nothrow
        {
        #if STLX_USE_RTTI
            return typeid(D) == ti ? reinterpret_cast<const void*>(&deleter) : nullptr;
        #else
            return reinterpret_cast<const void*>(&deleter);
        #endif
        }
      };

      template<class T, class D, class A>
      struct shared_ptr_da
        : shared_ptr_deleter<T,D>
      {
        A alloc;

        shared_ptr_da(T* p, const D& d, const A& a)
          :shared_ptr_deleter<T,D>(p,d), alloc(a)
        {}
        void dispose() __ntl_nothrow
        {
          free();
          typename A::template rebind<shared_ptr_da>::other alloc(this->alloc);
          alloc.destroy(this);
          alloc.deallocate(this, 1);
        }
      };

      struct shared_cast_static{};
      struct shared_cast_dynamic{};
      struct shared_cast_const{};
      struct shared_allocator_tag{};
      template<class> struct check_shared;

      template<class T, class U>
      inline shared_ptr_data<T>* shared_data_cast(shared_ptr_data<U>* data)
      {
        return reinterpret_cast<shared_ptr_data<T>*>(data);
      }
    }

    template<class T>
    class shared_ptr
    {
      typedef __::shared_ptr_base* shared_data;

      struct explicit_bool { int _; };
      typedef int explicit_bool::*  explicit_bool_type;

      template<class Y> friend class weak_ptr;
      template<class Y> friend class shared_ptr;
      template<class D, class T> 
      friend D* get_deleter(shared_ptr<T> const& p);

    protected:
      // *shared_ptr<void> protect
      typedef typename add_reference<typename conditional<is_void<T>::value,void*,T>::type>::type reference;

      template<class U>
      inline reference get_reference(false_type) const { return *ptr; }
      template<class U>
      inline reference get_reference(true_type)  const { return const_cast<reference>(ptr);  }

      template<class Y>
      void check_shared(Y* p, const shared_ptr<T>* ptr);
    public:
      typedef T element_type;

      shared_ptr() __ntl_nothrow
        :shared(),ptr()
      {}

      shared_ptr(nullptr_t) __ntl_nothrow
        :shared(),ptr()
      {}

      template<class Y> explicit shared_ptr(Y* p) __ntl_throws(bad_alloc)
        :shared(),ptr()
      {
        __ntl_try {
          shared = new __::shared_ptr_data<Y>(p);
          set(p);
          check_shared(p, this);
        }
        __ntl_catch(bad_alloc){
          delete p;
        }
      }

      template<class Y, class D> shared_ptr(Y* p, D d) __ntl_throws(bad_alloc)
        :shared(),ptr()
      {
        __ntl_try {
          shared = new __::shared_ptr_deleter<Y,D>(p, d);
          set(p);
          check_shared(p, this);
        }
        __ntl_catch(bad_alloc){
          d(p);
        }
      }

      template<class Y, class D, class A> shared_ptr(Y* p, D d, A a)
        :shared(),ptr()
      {
        typedef __::shared_ptr_da<Y,D,A> shared_data3;
        typename A::template rebind<shared_data3>::other alloc(a);

        __ntl_try {
          shared_data3* s = alloc.allocate(1);
          alloc.construct(s, p, d, a);
          shared = s;
          set(p);
          check_shared(p, this);
        }
        __ntl_catch(bad_alloc){
          d(p);
        }
      }

      shared_ptr(const shared_ptr& r) __ntl_nothrow
        :shared(r.shared),ptr(r.get())
      {
        add_ref();
      }
      template<class Y> shared_ptr(const shared_ptr<Y>& r) __ntl_nothrow
        :shared(r.shared),ptr(r.get())
      {
        static_assert((is_convertible<Y*,T*>::value), "Y* shall be convertible to T*");
        add_ref();
      }
      template<class Y> shared_ptr(const shared_ptr<Y>& r, T* p) __ntl_nothrow
        :shared(r.shared), ptr()
      {
        add_ref(p);
      }

      template<class Y> explicit shared_ptr(const weak_ptr<Y>& r) __ntl_throws(bad_weak_ptr)
        :shared(),ptr()
      {
        static_assert((is_convertible<Y*,T*>::value), "Y* shall be convertible to T*");
        if(r.expired())
          __ntl_throw(bad_weak_ptr());

        shared = r.shared;
        add_ref(r.ptr);
      }
      template<class Y> explicit shared_ptr(auto_ptr<Y>& r)__ntl_throws(bad_alloc)
        :shared(),ptr()
      {
        if(r.get()){
          shared = new __::shared_ptr_data<Y>(r.get());
          ptr = r.release();
        }
      }
      template <class Y, class D>
      explicit shared_ptr(const unique_ptr<Y, D>& r)__ntl_throws(bad_alloc)
        :shared(),ptr()
      {
        if(r.get()){
          // currently unique_ptr's deleter always are reference
          shared = new __::shared_ptr_deleter<Y,D>(r.get(), ref(r.get_deleter()));
          ptr = r.release();
        }
      }

      ~shared_ptr()__ntl_nothrow
      {
        reset();
      }
      
      shared_ptr& operator=(const shared_ptr& r)
      {
        if(this != &r){
          reset();
          shared = r.shared;
          ptr = r.ptr;
          add_ref();
        }
        return *this;
      }

      template<class Y> shared_ptr& operator=(const shared_ptr<Y>& r)
      {
        if(shared != r.shared)
          shared_ptr(r).swap(*this);
        return *this;
      }
      template<class Y> shared_ptr& operator=(auto_ptr<Y>& r)
      {
        shared_ptr(r).swap(*this);
        return *this;
      }
      template <class Y, class D>
      shared_ptr& operator=(const unique_ptr<Y, D>& r)
      {
        shared_ptr(r).swap(*this);
        return *this;
      }
      
      void swap(shared_ptr& r)
      {
        //using namespace std;
        std::swap(shared, r.shared);
        std::swap(ptr, r.ptr);
      }

      void reset()
      {
        if(shared){
          if(--shared->use_count == 0)
            free();
          shared = nullptr,
            ptr = nullptr;
        }
      }

      template<class Y> void reset(Y* p)
      {
        reset();
        shared = new __::shared_ptr_data<Y>(p);
        set(p);
        check_shared(p, this);
      }
      template<class Y, class D> void reset(Y* p, D d)
      {
        reset();
        shared = new __::shared_ptr_deleter<Y,D>(p, d);
        set(p);
      }
      template<class Y, class D, class A> void reset(Y* p, D d, A a)
      {
        shared_ptr(p,d,a).swap(*this);
      }
      
      T* get() const __ntl_nothrow
      {
        return ptr;
      }

      reference operator*() const __ntl_nothrow
      {
        return get_reference<T>(is_void<T>());
      }

      T* operator->() const __ntl_nothrow { return ptr; }
      
      long use_count() const __ntl_nothrow
      {
        return !shared || (shared->use_count < 0) ? 0 : shared->use_count;
      }

      bool unique() const __ntl_nothrow
      {
        return shared && shared->use_count == 1;
      }

      operator explicit_bool_type() const __ntl_nothrow { return ptr ? &explicit_bool::_ : 0;  }

    protected:
      template<class T, class U>
      friend bool operator<(shared_ptr<T> const& a, shared_ptr<U> const& b) __ntl_nothrow;

      template<class Y> shared_ptr(const shared_ptr<Y>& r, __::shared_cast_const) __ntl_nothrow
        :shared(r.shared)
      {
        const_cast<T*>((Y*)0);
        add_ref();
      }
      template<class Y> shared_ptr(const shared_ptr<Y>& r, __::shared_cast_static) __ntl_nothrow
        :shared(r.shared)
      {
        static_cast<T*>((Y*)0);
        add_ref();
      }
      template<class Y> shared_ptr(const shared_ptr<Y>& r, __::shared_cast_dynamic) __ntl_nothrow
        :shared()
      {
        if(dynamic_cast<T*>(r.get())){
          shared = r.shared;
          add_ref();
        }
      }
    protected:
      bool empty() const __ntl_nothrow { return !shared; }
      void add_ref()
      {
        if(shared)
          ++shared->use_count;
      }
      void add_ref(T* p)
      {
        if(shared)
          ++shared->use_count;
        ptr = p;
      }
      void set(T* p)
      {
        ptr = p;
      }
      void free()__ntl_nothrow
      {
        if(shared){
          shared->free(); // free object, but not counter
          if(!shared->weak_count)
            shared->dispose(); // free counter
        }
      }
    private:
      shared_data shared;
      T* ptr;
    };

    //////////////////////////////////////////////////////////////////////////
    // 20.8.12.2.6, shared_ptr creation
  #ifdef NTL_CXX_VT
    template<class T, class... Args> shared_ptr<T> make_shared(Args&&... args);
    template<class T, class A, class... Args>
    shared_ptr<T> allocate_shared(const A& a, Args&&... args);
  #else
    namespace __
    {
      template<class T, class Alloc>
      inline shared_ptr<T> shared_ptr_allocate(T* p, const Alloc& a)
      {
        struct deleter
        {
          Alloc a;
          deleter(const Alloc& a)
            :a(a){}
          void operator()(T* p)__ntl_nothrow
          {
            a.destroy(p);
            a.deallocate(p, 1);
          }
        } d(a);
        return shared_ptr<T>(p, d, a);
      }

    }

    template<class T>
    inline shared_ptr<T> make_shared()
    {
      return shared_ptr<T>(new T());
    }
    template<class T, class A1>
    inline shared_ptr<T> make_shared(const A1& a1)
    {
      return shared_ptr<T>(new T(a1));
    }
    template<class T, class A1, class A2>
    inline shared_ptr<T> make_shared(const A1& a1, const A2& a2)
    {
      return shared_ptr<T>(new T(a1,a2));
    }

    template<class T, class Alloc>
    inline shared_ptr<T> allocate_shared(const Alloc& a)
    {
      typename Alloc::template rebind<T>::other at(a);
      T* p = at.allocate(1);
      at.construct(p, T());
      return __::shared_ptr_allocate(p, at);
    }

    template<class T, class Alloc, class A1>
    inline shared_ptr<T> allocate_shared(const Alloc& a, const A1& a1)
    {
      typename Alloc::template rebind<T>::other at(a);
      T* p = at.allocate(1);
      at.construct(p, a1);
      return __::shared_ptr_allocate(p, at);
    }
    template<class T, class Alloc, class A1, class A2>
    inline shared_ptr<T> allocate_shared(const Alloc& a, const A1& a1, const A2& a2)
    {
      typename Alloc::template rebind<T>::other at(a);
      T* p = at.allocate(1);
      at.construct(p, a1,a2);
      return __::shared_ptr_allocate(p, at);
    }
  #endif

    //////////////////////////////////////////////////////////////////////////
    template<class T, class U> 
    inline bool operator==(shared_ptr<T> const& a, shared_ptr<U> const& b) __ntl_nothrow
    {
      return a.get() == b.get();
    }
    template<class T, class U>
    inline bool operator!=(shared_ptr<T> const& a, shared_ptr<U> const& b) __ntl_nothrow
    {
      return a.get() != b.get();
    }
    template<class T, class U>
    inline bool operator<(shared_ptr<T> const& a, shared_ptr<U> const& b) __ntl_nothrow
    {
    #ifdef NTL_CXX_TYPEOF
      typedef decltype(a.get() < b.get()) V;
      return std::less<V>()(a.get(), b.get());
    #else
      return a.get() < b.get();
    #endif
    }

    template<class T> inline void swap(shared_ptr<T>& a, shared_ptr<T>& b) __ntl_nothrow
    {
      a.swap(b);
    }

    //////////////////////////////////////////////////////////////////////////
    template<class T, class U>
    inline shared_ptr<T> static_pointer_cast(shared_ptr<U> const& r) __ntl_nothrow
    {
      static_cast<T*>((U*)0); // check
      return r.empty() ? shared_ptr<T>() : shared_ptr<T>(r, __::shared_cast_static());
    }
    template<class T, class U> 
    inline shared_ptr<T> dynamic_pointer_cast(shared_ptr<U> const& r) __ntl_nothrow
    {
      return r.empty() ? shared_ptr<T>() : shared_ptr<T>(r, __::shared_cast_dynamic());
    }
    template<class T, class U> 
    inline shared_ptr<T> const_pointer_cast(shared_ptr<U> const& r) __ntl_nothrow
    {
      const_cast<T*>((U*)0); // check
      return r.empty() ? shared_ptr<T>() : shared_ptr<T>(r, __::shared_cast_const());
    }

    //////////////////////////////////////////////////////////////////////////
    template<class E, class T, class Y>
    inline basic_ostream<E, T>& operator<< (basic_ostream<E, T>& os, shared_ptr<Y> const& p)
    {
      return os << p.get();
    }

    //////////////////////////////////////////////////////////////////////////
    template<class D, class T> 
    inline D* get_deleter(shared_ptr<T> const& p)
    {
      return !p.empty() ? (D*)p.shared->get_deleter(__ntl_typeid(D)) : nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    
    template<class T>
    class weak_ptr
    {
      typedef __::shared_ptr_base* shared_data;

      template<class Y> friend class shared_ptr;
      friend struct __::check_shared<T>;
    public:
      typedef T element_type;
      // constructors
      weak_ptr() __ntl_nothrow
        :shared(),ptr()
      {}

      weak_ptr(weak_ptr const& r) __ntl_nothrow
        :shared(r.shared),ptr(r.ptr)
      {
        add_ref();
      }

      template<class Y> weak_ptr(weak_ptr<Y> const& r) __ntl_nothrow
        :shared(r.shared),ptr(r.ptr)
      {
        add_ref();
      }

      template<class Y> weak_ptr(shared_ptr<Y> const& r) __ntl_nothrow
        :shared(r.shared),ptr(r.ptr)
      {
        add_ref();
      }

      // destructor
      ~weak_ptr() __ntl_nothrow
      {
        reset();
      }

      // assignment
      weak_ptr& operator=(weak_ptr const& r) __ntl_nothrow
      {
        if(this != &r){
          reset();
          shared = r.shared;
          ptr = r.ptr;
        }
        return *this;
      }
      template<class Y> weak_ptr& operator=(weak_ptr<Y> const& r) __ntl_nothrow
      {
        if(shared != r.shared){
          reset();
          shared = r.shared;
          ptr = r.ptr;
          add_ref();
        }
        return *this;
      }
      template<class Y> weak_ptr& operator=(shared_ptr<Y> const& r) __ntl_nothrow
      {
        if(shared != r.shared){
          reset();
          shared = r.shared;
          ptr = r.ptr;
          add_ref();
        }
        return *this;
      }

      // modifiers
      void swap(weak_ptr& r) __ntl_nothrow
      {
        std::swap(shared, r.shared);
        std::swap(ptr, r.ptr);
      }

      void reset() __ntl_nothrow
      {
        if(shared){
          if(shared->use_count == 0)
            shared->free();
          if(--shared->weak_count == 0)
            shared->dispose();
          shared = nullptr,
          ptr = nullptr;
        }
      }

      // observers
      long use_count() const __ntl_nothrow      { return !shared || (shared->use_count < 0) ? 0 : shared->use_count; }
      bool expired() const __ntl_nothrow        { return !shared || shared->use_count <= 0; }

      shared_ptr<T> lock() const __ntl_nothrow  { return expired() ? shared_ptr<T>() : shared_ptr<T>(*this); }

      template<class U> 
      friend bool operator<(weak_ptr const& a, weak_ptr<U> const& b)
      {
        return ptr < b.ptr;
      }

    protected:
      void add_ref()
      {
        if(shared)
          ++shared->weak_count;
      }
      template<class Y> void assign_shared(shared_ptr<Y> const& r) __ntl_nothrow
      {
        if(shared != r.shared){
          reset();
          shared = r.shared;
          ptr = r.ptr;
          add_ref();
        }
      }
    protected:
      shared_data shared;
      T* ptr;
    };
    
    template<class T> 
    inline void swap(weak_ptr<T>& a, weak_ptr<T>& b) __ntl_nothrow
    {
      return a.swap(b);
    }

    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class enable_shared_from_this
    {
      struct shared_weak:
        weak_ptr<T>
      {
        ~shared_weak()
        {
          shared = nullptr;
          ptr = nullptr;
        }
      };
      //weak_ptr<T> weak_this;
      shared_weak weak_this;
      friend struct __::check_shared<T>;
    protected:
      enable_shared_from_this() __ntl_nothrow
      {}
      enable_shared_from_this(enable_shared_from_this const&) __ntl_nothrow
      {}
      enable_shared_from_this& operator=(enable_shared_from_this const&) __ntl_nothrow
      { return *this; }
      ~enable_shared_from_this() __ntl_nothrow
      {}
    public:
      /** Returns a shared_ptr<T> object that shares ownership with \c this. */
      shared_ptr<T> shared_from_this()
      {
        return shared_ptr<T>(weak_this);
      }
      shared_ptr<T const> shared_from_this() const
      {
        return shared_ptr<T const>(weak_this);
      }
    };

    namespace __
    {
      template<class T>
      struct check_shared
      {
        template<class>
        static inline void check(...) {}

        template<class Y>
        static inline void check(enable_shared_from_this<T>* p, const shared_ptr<T>& ptr)
        {
          p->weak_this.assign_shared(ptr);
        }
      };
    }

    template<class T>
    template<class Y>
    inline void shared_ptr<T>::check_shared(Y* p, const shared_ptr<T>* ptr)
    {
      if(p)
        __::check_shared<T>::check<Y>(p, *ptr); //-V111
    }

    /**@} lib_smartptr */


  /// 20.8.12.6 Pointer safety [util.dynamic.safety]
#ifdef NTL_CXX_ENUM
  enum class pointer_safety { relaxed, preferred, strict };
#else
  /**
  *	An enumeration value indicating the implementation’s treatment of pointers that are not safely derived.
  **/
  struct pointer_safety_class
  {
    enum type
    {
      /** pointers that are not safely derived will be treated the same as pointers that are safely derived for the duration of the program */
      relaxed,
      /** pointers that are not safely derived will be treated the same as pointers that are safely derived for the duration of the program
      but allows the implementation to hint that it could be desirable to avoid dereferencing pointers that are not safely derived as described. */
      preferred,
      /** pointers that are not safely derived might be treated differently than pointers that are safely derived. */
      strict
    };
  };
  typedef ntl::class_enum<pointer_safety_class> pointer_safety;
#endif

  void declare_reachable(void *p);
  template <class T> T *undeclare_reachable(T *p);
  void declare_no_pointers(char *p, size_t n);
  void undeclare_no_pointers(char *p, size_t n);
  pointer_safety get_pointer_safety();


  /// 20.8.13 Align [ptr.align]
#ifndef NTL_CXX_ALIGN
  inline void * align(size_t alignment, size_t size, void* &ptr, size_t& space)
  {
    uintptr_t & uptr = reinterpret_cast<uintptr_t&>(ptr);
    uintptr_t const aligned_ptr = ntl::align_up(uptr, alignment);
    if ( aligned_ptr + size <= uptr + space )
    {
      space -= aligned_ptr - uptr;
      return reinterpret_cast<void*>(uptr = aligned_ptr);
    }
    return 0;
  }
#endif

#ifndef NTL__STLX_EXCLUDE_DEPRECATED
  /**\ingroup std_depr
   *@{*/

  /**\defgroup depr_autoptr D.9 auto_ptr (legacy) [depr.auto.ptr]
   *@*/

  template<class X> class auto_ptr;

  template<class Y>
  struct auto_ptr_ref
  {
    explicit auto_ptr_ref(auto_ptr<Y> & a) __ntl_nothrow : ptr(a) {}
    auto_ptr<Y> & ptr;
  private:
    auto_ptr_ref<Y> & operator=(const auto_ptr_ref<Y> &);
  };

  /// D.9.1 Class template auto_ptr [auto.ptr]
  template<class X>
  class auto_ptr
  {
    ///////////////////////////////////////////////////////////////////////////
  public:

    typedef X element_type;

    ///\name  D.9.1.1 auto_ptr constructors [auto.ptr.cons]

    explicit auto_ptr(X * p = 0)  __ntl_nothrow : ptr(p) {}

    auto_ptr(auto_ptr & a)        __ntl_nothrow : ptr(a.release()) {}

    template<class Y>
    auto_ptr(auto_ptr<Y> & a)     __ntl_nothrow : ptr(a.release()) {}

    __forceinline
      auto_ptr & operator=(auto_ptr & a) __ntl_nothrow
    {
      reset(a.release());
      return *this;
    }

    template<class Y>
    auto_ptr & operator=(auto_ptr<Y> & a) __ntl_nothrow
    {
      reset(a.release());
      return *this;
    }

    __forceinline
      ~auto_ptr() __ntl_nothrow { if ( get() ) delete get(); }

    ///\name  D.9.1.2 auto_ptr members [auto.ptr.members]

    X & operator* ()  const __ntl_nothrow { return *ptr; }
    X * operator->()  const __ntl_nothrow { return ptr; }
    X * get()         const __ntl_nothrow { return ptr; }
    X * release()           __ntl_nothrow { X * tmp = get(); set(0); return tmp; }

    __forceinline
      void reset(X * p = 0)   __ntl_nothrow
    {
      if ( get() && get() != p ) delete get();
      set(p);
    }

    ///\name  D.9.1.3 auto_ptr conversions [auto.ptr.conv]

    auto_ptr(auto_ptr_ref<X> r) __ntl_nothrow : ptr(r.ptr.release()) {}

    __forceinline
      auto_ptr & operator=(auto_ptr_ref<X> r) __ntl_nothrow { return *this = r.ptr; }

    template<class Y>
    operator auto_ptr_ref<Y>()  __ntl_nothrow { return auto_ptr_ref<Y>(*reinterpret_cast<auto_ptr<Y>*>(this)); }

    template<class Y>
    operator auto_ptr<Y>()      __ntl_nothrow { return auto_ptr<Y>(release()); }

    ///////////////////////////////////////////////////////////////////////////
  private:

    X * ptr;
    void set(X * p) { ptr = p; }
  };

  /**@} depr_autoptr */
  /**@} std_depr */
#endif

  /**@} lib_memory */
  /**@} lib_utilities */

}//namespace std

#endif//#ifndef NTL__STLX_SMARTPTR
