/**\file*********************************************************************
 *                                                                     \brief
 *  /// 20.7.11 Class template unique_ptr [unique.ptr]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_SMARTPTR
#define NTL__STLX_SMARTPTR

#include "memory.hxx"
#include "utility.hxx"
#include "type_traits.hxx"

#include "../basedef.hxx"
#include "../linked_ptr.hxx"
#include "../stdlib.hxx"

namespace std
{
  /**\addtogroup  lib_utilities ********** [20] General utilities library *****
  *@{*/
  /**\addtogroup  lib_memory *************** [20.7] Memory **********************
  *@{*/

  /// 20.7.11 Class template unique_ptr [unique.ptr]

  ///\name 20.7.11.1 Default deleters [unique.ptr.dltr]

  /// 20.7.11.1.1 default_delete [unique.ptr.dltr.dflt]
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

  /// 20.7.11.1.2 default_delete<T[]> [unique.ptr.dltr.dflt1]
  template <class T> struct default_delete<T[]>
  {
    void operator()(T* ptr) const 
    {
      static_assert(sizeof(T) > 0, "incomplete type of T is not allowed");
      ::delete[] ptr; 
    }
  };

  #ifdef STLX__SMARTPTR_WITH_N
  /// 20.7.11.1.3 default_delete<T[N]> [unique.ptr.dltr.dflt2]
  template <class T, size_t N> struct default_delete<T[N]>
  {
    void operator()(T* ptr, size_t) const { ::delete[] ptr; }
  };
  #endif

  ///\}

  template <class T, class D = default_delete<T> >
  class unique_ptr;

  /// 20.7.11.2 unique_ptr for single objects [unique.ptr.single]
  template <class T, class D>
  class unique_ptr
  {
#ifndef __BCPLUSPLUS__
    typedef typename const unique_ptr::T** unspecified_pointer_type;
    typedef typename const unique_ptr::T*  unspecified_bool_type;
#else
    typedef const T** unspecified_pointer_type;
    typedef const T*  unspecified_bool_type;
#endif

    ///////////////////////////////////////////////////////////////////////////
  public:

    typedef T* pointer;
    typedef T  element_type;
    typedef D  deleter_type;

    ///\name 20.7.11.2.1 unique_ptr constructors [unique.ptr.single.ctor]
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

    unique_ptr(pointer p, typename conditional<is_reference<deleter_type>::value, deleter_type, const deleter_type&>::type d) __ntl_nothrow 
      : ptr(p), deleter(d)
    {}

    unique_ptr(pointer p, typename remove_reference<deleter_type>::type&& d) __ntl_nothrow 
      : ptr(p), deleter(move(d))
    {
      static_assert(!is_reference<D>::value, "rvalue deleter object combined with reference deleter type");
    }

    unique_ptr(unique_ptr&& u) __ntl_nothrow
      : ptr(u.get()), deleter(forward<D>(u.deleter))
    {
      u.release();
    }

    template <class U, class E>
    unique_ptr(unique_ptr<U, E>&& u) __ntl_nothrow
      : ptr(u.get()), deleter(forward<D>(u.get_deleter()))
    {
      static_assert(is_array<U>::value == false, "conversion among array and non-array forms are disallowed");
      static_assert(!is_reference<deleter_type>::value || is_same<deleter_type, E>::value, "If D is a reference type, then E shall be the same type as D");
      u.release();
    }

    ///\name 20.7.11.2.2 unique_ptr destructor [unique.ptr.single.dtor]
    ~unique_ptr() __ntl_nothrow 
    {
      if ( get() )
        get_deleter()(get()); 
    }


#ifdef NTL__CXX_EF
    // disable copy from lvalue
    unique_ptr(const unique_ptr&) = delete;
    template <class U, class E> unique_ptr(const unique_ptr<U, E>&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;
    template <class U, class E> unique_ptr& operator=(const unique_ptr<U, E>&) = delete;
#endif

    ///\name 20.7.11.2.3 unique_ptr assignment [unique.ptr.single.asgn]
    __forceinline
      unique_ptr& operator=(unique_ptr&& u) __ntl_nothrow
    {
      reset(u.release());
      deleter = move(u.deleter);
      return *this;
    }

    template <class U, class E>
    __forceinline
      unique_ptr& operator=(unique_ptr<U, E>&& u) __ntl_nothrow
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

    ///\name 20.7.11.2.4 unique_ptr observers [unique.ptr.single.observers]
    typename add_lvalue_reference<T>::type operator*() const __ntl_nothrow { return *get(); }
    pointer operator->() const __ntl_nothrow { return get(); }
    pointer get() const __ntl_nothrow { return ptr; }

    deleter_type& get_deleter() __ntl_nothrow { return deleter; }
    const deleter_type& get_deleter() const __ntl_nothrow { return deleter; }

    operator unspecified_bool_type() const __ntl_nothrow { return ptr; }

    ///\name 20.7.11.2.5 unique_ptr modifiers [unique.ptr.single.modifiers]
    pointer release()
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

    void swap(unique_ptr&& u) __ntl_nothrow 
    {
      std::swap(ptr, u.ptr);
      std::swap(deleter, u.deleter);
    }

    ///\}

    ///////////////////////////////////////////////////////////////////////////
  private:
    pointer ptr;
    deleter_type  deleter;

    void set(T * p) { ptr = p; }

#ifndef NTL__CXX_EF
    // disable copy from lvalue
    unique_ptr(const unique_ptr&);
    template <class U, class E> unique_ptr(const unique_ptr<U, E>&);
    unique_ptr& operator=(const unique_ptr&);
    template <class U, class E> unique_ptr& operator=(const unique_ptr<U, E>&);
#endif

  };//template class unique_ptr



  template <class T>
  class unique_ptr<T, default_delete<T> >
  {
    #ifndef __BCPLUSPLUS__
    typedef typename const unique_ptr::T** unspecified_pointer_type;
    typedef typename const unique_ptr::T*  unspecified_bool_type;
    #else
    typedef const T** unspecified_pointer_type;
    typedef const T*  unspecified_bool_type;
    #endif

    ///////////////////////////////////////////////////////////////////////////
  public:

    typedef T* pointer;
    typedef T  element_type;
    typedef default_delete<T> deleter_type;

    ///\name 20.7.11.2.1 unique_ptr constructors [unique.ptr.single.ctor]
    unique_ptr() __ntl_nothrow : ptr(0) {}

    explicit unique_ptr(pointer p) __ntl_nothrow : ptr(p) {}
    
    unique_ptr(nullptr_t) __ntl_nothrow
      :ptr(0)
    {}

    unique_ptr(pointer p, const deleter_type &) __ntl_nothrow : ptr(p) {}

    unique_ptr(unique_ptr&& u) __ntl_nothrow : ptr(u.get())
    {
      u.release(); // it's here to help MSVC optimizing container operations
    }

    template <class U/*, class E*/>
    unique_ptr(unique_ptr<U, default_delete<U> >&& u) __ntl_nothrow : ptr(u.get())
    {
      static_assert(is_array<U>::value == false, "conversion among array and non-array forms are disallowed");
      u.release();
    }

    ///\name 20.7.11.2.2 unique_ptr destructor [unique.ptr.single.dtor]
    ~unique_ptr() __ntl_nothrow 
    {
      if ( get() )
        get_deleter()(get()); 
    }


    #ifdef NTL__CXX_EF
    // disable copy from lvalue
    unique_ptr(const unique_ptr&) = delete;
    template <class U, class E> unique_ptr(const unique_ptr<U, E>&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;
    template <class U, class E> unique_ptr& operator=(const unique_ptr<U, E>&) = delete;
    #endif

    ///\name 20.7.11.2.3 unique_ptr assignment [unique.ptr.single.asgn]
    __forceinline
    unique_ptr& operator=(unique_ptr&& u) __ntl_nothrow
    {
      reset(u.release());
      return *this;
    }

    template <class U/*, class E*/>
    __forceinline
    unique_ptr& operator=(unique_ptr<U, default_delete<U> >&& u) __ntl_nothrow
    {
      static_assert(is_array<U>::value == false, "conversion among array and non-array forms are disallowed");
      reset(u.release());
      return *this;
    }

    unique_ptr& operator=(unspecified_pointer_type *)
    {
      reset();
      return *this;
    }

    ///\name 20.7.11.2.4 unique_ptr observers [unique.ptr.single.observers]
    typename add_lvalue_reference<T>::type operator*() const __ntl_nothrow { return *get(); }
    pointer operator->() const __ntl_nothrow { return get(); }
    pointer get() const __ntl_nothrow { return ptr; }

    // local statics produce code bloat, shoud we replace the UD with a global static?
    deleter_type& get_deleter() __ntl_nothrow { return *(deleter_type*)0; }
    const deleter_type& get_deleter() const __ntl_nothrow { return *(deleter_type*)0; }

    operator unspecified_bool_type() const __ntl_nothrow { return ptr; }

    ///\name 20.7.11.2.5 unique_ptr modifiers [unique.ptr.single.modifiers]
    pointer release()
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

    void swap(unique_ptr&& u) __ntl_nothrow 
    {
      std::swap(ptr, u.ptr); 
    }
    ///\}

    ///////////////////////////////////////////////////////////////////////////
  private:
    pointer ptr;
    void set(T * p) { ptr = p; }

    #ifndef NTL__CXX_EF
    // disable copy from lvalue
    unique_ptr(const unique_ptr&);
    template <class U, class E> unique_ptr(const unique_ptr<U, E>&);
    unique_ptr& operator=(const unique_ptr&);
    template <class U, class E> unique_ptr& operator=(const unique_ptr<U, E>&);
    #endif

  };//template class unique_ptr


  /// 20.7.11.3 unique_ptr for array objects with a runtime length [unique.ptr.runtime]
  template <class T, class D>
  class unique_ptr<T[], D>
  {
#ifndef __BCPLUSPLUS__
    typedef typename const unique_ptr::T** unspecified_pointer_type;
    typedef typename const unique_ptr::T*  unspecified_bool_type;
#else
    typedef const T** unspecified_pointer_type;
    typedef const T*  unspecified_bool_type;
#endif

    ///////////////////////////////////////////////////////////////////////////
  public:
    typedef T* pointer;
    typedef T  element_type;
    typedef D  deleter_type;

    ///\name 20.7.11.3.1 unique_ptr constructors [unique.ptr.runtime.ctor]
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

    unique_ptr(pointer p, typename conditional<is_reference<deleter_type>::value, deleter_type, const deleter_type&>::type d) __ntl_nothrow 
      : ptr(p), deleter(d)
    {}

    unique_ptr(pointer p, typename remove_reference<deleter_type>::type&& d) __ntl_nothrow 
      : ptr(p), deleter(move(d))
    {
      static_assert(!is_reference<D>::value, "rvalue deleter object combined with reference deleter type");
    }

    unique_ptr(unique_ptr&& u) __ntl_nothrow
      : ptr(u.get()), deleter(forward<D>(u.deleter))
    {
      u.release();
    }

    template <class U, class E>
    unique_ptr(unique_ptr<U, E>&& u) __ntl_nothrow
      : ptr(u.get()), deleter(forward<D>(u.get_deleter()))
    {
      static_assert(is_array<U>::value == true, "conversion among array and non-array forms are disallowed");
      static_assert(!is_reference<deleter_type>::value || is_same<deleter_type, E>::value, "If D is a reference type, then E shall be the same type as D");
      u.release();
    }

    ///\name 20.7.11.2.2 unique_ptr destructor [unique.ptr.single.dtor]
    ~unique_ptr() __ntl_nothrow 
    {
      if ( get() )
        get_deleter()(get()); 
    }


#ifdef NTL__CXX_EF
    // disable copy from lvalue
    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;
#endif

    ///\name 20.7.11.2.3 unique_ptr assignment [unique.ptr.single.asgn]
    __forceinline
      unique_ptr& operator=(unique_ptr&& u) __ntl_nothrow
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

    ///\name 20.7.11.3.2 unique_ptr observers [unique.ptr.runtime.observers]
    T& operator[](size_t i) const __ntl_nothrow { return get()[i]; }
    pointer get() const __ntl_nothrow { return ptr; }

    deleter_type& get_deleter() __ntl_nothrow { return deleter; }
    const deleter_type& get_deleter() const __ntl_nothrow { return deleter; }

    operator unspecified_bool_type() const __ntl_nothrow { return ptr; }

    ///\name 20.7.11.2.5 unique_ptr modifiers [unique.ptr.single.modifiers]
    pointer release()
    {
      pointer tmp = nullptr;
      std::swap(tmp, ptr);
      return tmp;
    }

    __forceinline
    void reset(pointer p = 0) __ntl_nothrow
    {
      if ( get() && get() != p ) get_deleter()(get());
      set(p);
    }

    void swap(unique_ptr&& u) __ntl_nothrow
    {
      std::swap(ptr, u.ptr); 
      std::swap(deleter, u.deleter);
    }

    ///\}

    ///////////////////////////////////////////////////////////////////////////
  private:
    pointer ptr;
    deleter_type deleter;

    void set(T * p) { ptr = p; }

#ifndef NTL__CXX_EF
    // disable copy from lvalue
    unique_ptr(const unique_ptr&);
    unique_ptr& operator=(const unique_ptr&);
#endif

    template<class Other> unique_ptr(Other*);
    template<class Other> void reset(Other*) const;

  };//template class unique_ptr


  template <class T>
  class unique_ptr<T[], default_delete<T[]> >
  {
    #ifndef __BCPLUSPLUS__
    typedef typename const unique_ptr::T** unspecified_pointer_type;
    typedef typename const unique_ptr::T*  unspecified_bool_type;
    #else
    typedef const T** unspecified_pointer_type;
    typedef const T*  unspecified_bool_type;
    #endif

    ///////////////////////////////////////////////////////////////////////////
  public:

    typedef T* pointer;
    typedef T  element_type;
    typedef default_delete<T[]> deleter_type;

    ///\name 20.7.11.3.1 unique_ptr constructors [unique.ptr.runtime.ctor]
    unique_ptr() __ntl_nothrow : ptr(0) {}

    explicit unique_ptr(pointer p) __ntl_nothrow : ptr(p) {}

    unique_ptr(nullptr_t) __ntl_nothrow
      :ptr(0)
    {}

    unique_ptr(pointer p, const deleter_type &) __ntl_nothrow : ptr(p) {}

    unique_ptr(unique_ptr&& u) __ntl_nothrow : ptr(u.get())
    {
      u.release();
    }

    template <class U/*, class E*/>
    unique_ptr(unique_ptr<U, default_delete<U[]> >&& u) __ntl_nothrow
      : ptr(u.get())
    {
      static_assert(is_array<U>::value == true, "conversion among array and non-array forms are disallowed");
      u.release();
    }

    ///\name 20.7.11.2.2 unique_ptr destructor [unique.ptr.single.dtor]
    ~unique_ptr() __ntl_nothrow 
    {
      if ( get() )
        get_deleter()(get()); 
    }

    #ifdef NTL__CXX_EF
    // disable copy from lvalue
    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;
    #endif

    ///\name 20.7.11.2.3 unique_ptr assignment [unique.ptr.single.asgn]
    __forceinline
    unique_ptr& operator=(unique_ptr&& u) __ntl_nothrow
    {
      reset(u.release());
      return *this;
    }

    unique_ptr& operator=(unspecified_pointer_type *)
    {
      reset();
      return *this;
    }

    ///\name 20.7.11.3.2 unique_ptr observers [unique.ptr.runtime.observers]
    T& operator[](size_t i) const __ntl_nothrow { return get()[i]; }
    pointer get() const __ntl_nothrow { return ptr; }

    deleter_type& get_deleter() __ntl_nothrow
    {
      static deleter_type deleter;
      return deleter;
    }

    const deleter_type& get_deleter() const __ntl_nothrow
    {
      static const deleter_type deleter;
      return deleter;
    }

    operator unspecified_bool_type() const __ntl_nothrow { return ptr; }

    ///\name 20.7.11.2.5 unique_ptr modifiers [unique.ptr.single.modifiers]
    pointer release()
    {
      pointer tmp = nullptr;
      std::swap(tmp, ptr);
      return tmp;
    }

    __forceinline
    void reset(pointer p = 0) __ntl_nothrow
    {
      if ( get() && get() != p ) get_deleter()(get());
      set(p);
    }

    void swap(unique_ptr&& u) __ntl_nothrow
    {
      std::swap(ptr, u.ptr); 
    }
    ///\}

    ///////////////////////////////////////////////////////////////////////////
  private:
    pointer ptr;

    void set(T * p) { ptr = p; }

    #ifndef NTL__CXX_EF
    // disable copy from lvalue
    unique_ptr(const unique_ptr&);
    unique_ptr& operator=(const unique_ptr&);
    #endif

    template<class Other> unique_ptr(Other*);
    template<class Other> void reset(Other*) const;

  };//template class unique_ptr


  #ifdef STLX__SMARTPTR_WITH_N
  /// unique_ptr for array objects with a compile time length (removed from N2723)
  template <class T, size_t N>
  class unique_ptr<T[N], default_delete<T[N]> >
  {
    typedef typename const unique_ptr::T** unspecified_pointer_type;
    typedef typename const unique_ptr::T*  unspecified_bool_type;

    ///////////////////////////////////////////////////////////////////////////
  public:
    typedef T* pointer;
    typedef T  element_type;
    typedef default_delete<T[N]> deleter_type;

    static const size_t size = N;

    ///\name 20.7.11.3.1 unique_ptr constructors [unique.ptr.runtime.ctor]
    unique_ptr() __ntl_nothrow : ptr(0) {}

    explicit unique_ptr(pointer p) __ntl_nothrow : ptr(p) {}

    unique_ptr(nullptr_t) __ntl_nothrow
      :ptr(0)
    {}

    unique_ptr(pointer p, const deleter_type &) __ntl_nothrow : ptr(p) {}

    unique_ptr(unique_ptr&& u) __ntl_nothrow : ptr(u.get())
    {
      u.release();
    }

    ///\name 20.7.11.4.1 unique_ptr destructor [unique.ptr.compiletime.dtor]
    ~unique_ptr() __ntl_nothrow 
    {
      if ( get() )
        get_deleter()(get(), N); 
    }

    #ifdef NTL__CXX_EF
    // disable copy from lvalue
    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;
    #endif

    ///\name 20.7.11.2.3 unique_ptr assignment [unique.ptr.single.asgn]
    __forceinline
    unique_ptr& operator=(unique_ptr&& u) __ntl_nothrow
    {
      reset(u.release());
      return *this;
    }

    unique_ptr& operator=(unspecified_pointer_type *)
    {
      reset();
      return *this;
    }

    ///\name 20.7.11.4.1 unique_ptr destructor [unique.ptr.compiletime.dtor]
    T& operator[](size_t i) const __ntl_nothrow { return get()[i]; }
    pointer get() const __ntl_nothrow { return ptr; }

    deleter_type& get_deleter() __ntl_nothrow
    {
      static deleter_type deleter;
      return deleter;
    }

    const deleter_type& get_deleter() const __ntl_nothrow
    {
      static const deleter_type deleter;
      return deleter;
    }

    operator unspecified_bool_type() const __ntl_nothrow { return ptr; }

    ///\name 20.7.11.2.5 unique_ptr modifiers [unique.ptr.single.modifiers]
    pointer release()
    {
      pointer tmp = nullptr;
      std::swap(ptr, tmp);
      return tmp; 
    }

    __forceinline
    void reset(pointer p = 0) __ntl_nothrow
    {
      if ( get() && get() != p ) get_deleter()(get(), N);
      set(p);
    }

    void swap(unique_ptr&& u) __ntl_nothrow 
    { 
      std::swap(ptr, u.ptr); 
    }
    ///\}

    ///////////////////////////////////////////////////////////////////////////
  private:
    enum { __actual_size = N };

    pointer ptr;
    void set(T* p) { ptr = p; }

    #ifndef NTL__CXX_EF
    // disable copy from lvalue
    unique_ptr(const unique_ptr&);
    unique_ptr& operator=(const unique_ptr&);
    #endif

    template<class Other> unique_ptr(Other*);
    template<class Other> void reset(Other*) const;

  };//template class unique_ptr<T[N], default_delete<T[N]> >
  #endif

  ///\name 20.7.11.4 unique_ptr specialized algorithms [unique.ptr.special]
  template <class T, class D> void swap(unique_ptr<T, D>& x, unique_ptr<T, D>& y)
  {
    x.swap(y);
  }

  template <class T, class D> void swap(unique_ptr<T, D>&& x, unique_ptr<T, D>& y)
  {
    x.swap(y);
  }
  template <class T, class D> void swap(unique_ptr<T, D>& x, unique_ptr<T, D>&& y)
  {
    x.swap(y);
  }

  template <class T1, class D1, class T2, class D2>
  bool operator==(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
  {
    return x.get() == y.get();
  }

  template <class T1, class D1, class T2, class D2>
  bool operator!=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
  {
    return x.get() != y.get();
  }

  template <class T1, class D1, class T2, class D2>
  bool operator<(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
  {
    return x.get() < y.get();
  }

  template <class T1, class D1, class T2, class D2>
  bool operator<=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
  {
    return x.get() <= y.get();
  }

  template <class T1, class D1, class T2, class D2>
  bool operator>(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
  {
    return x.get() > y.get();
  }

  template <class T1, class D1, class T2, class D2>
  bool operator>=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
  {
    return x.get() >= y.get();
  }




  ///\name  20.7.12 Smart pointers [util.smartptr]

  template<class X> class auto_ptr;

  /// 20.7.12.1 Class bad_weak_ptr [util.smartptr.weakptr]
  class bad_weak_ptr : public std::exception
  {
  public:
    virtual const char* what() const __ntl_nothrow { return "bad_weak_ptr"; }
  };

  template<class T> class weak_ptr;

  /// 20.7.12.2 Class template shared_ptr [util.smartptr.shared]
  ///
  /// 1 The shared_ptr class template stores a pointer, usually obtained via new.
  ///   shared_ptr implements semantics of shared ownership; the last remaining
  ///   owner of the pointer is responsible for destroying the object, or
  ///   otherwise releasing the resources associated with the stored pointer.
  ///\note no constructor throws bad_alloc, since this needs no heap memory
  template<class T>
  class shared_ptr : ntl::linked_ptr<T>
  {
    typedef ntl::linked_ptr<T> base_type;

    ///////////////////////////////////////////////////////////////////////////
  public:

    typedef T element_type;

    ///\name  20.7.12.2.1 shared_ptr constructors [util.smartptr.shared.const]

    /// 1 Effects: Constructs an empty shared_ptr object.
    /// 2 Postconditions: use_count() == 0 && get() == 0.
    /// 3 Throws: nothing.
    shared_ptr() __ntl_nothrow : base_type() {/**/}

    /// 4 Requires: p shall be convertible to T*. Y shall be a complete type.
    ///   The expression delete p shall be well-formed,
    ///   shall have well defined behavior, and shall not throw exceptions.
    /// 5 Effects: Constructs a shared_ptr object that owns the pointer p.
    /// 6 Postconditions: use_count() == 1 && get() == p
    template<class Y> explicit shared_ptr(Y* p) : base_type(p) {/**/}

    /// Well, should not D & A be moved to the class template? ...
    template<class Y, class D> shared_ptr(Y* p, D d);
    /// this needs no allocator at all
    template<class Y, class D, class A> shared_ptr(Y* p, D d, A a);

    /// 14 Effects: Constructs a shared_ptr instance that stores p and shares
    ///    ownership with r.
    /// 15 Postconditions: get() == p && use_count() == r.use_count()
    /// 16 Throws: nothing.
    template<class Y> shared_ptr(const shared_ptr<Y>& r, T *p);

    /// 20 Effects: If r is empty, constructs an empty shared_ptr object;
    ///   otherwise, constructs a shared_ptr object that shares ownership with r.
    /// 21 Postconditions: get() == r.get() && use_count() == r.use_count().
    /// 22 Throws: nothing.
    shared_ptr(const shared_ptr& r) : base_type(r)
    {
      set(r.get());
    }

    /// see above
    ///\todo:
    /// 19 Requires: The constructor shall not participate in
    ///    the overload resolution unless Y* is implicitly convertible to T*.
    template<class Y> shared_ptr(const shared_ptr<Y>& r) : base_type(r)
    {
      set(r.get());
    }

    /// 23 Requires: For the second constructor Y* shall be convertible to T*.
    /// 24 Effects: Move-constructs a shared_ptr instance from r.
    /// 25 Postconditions: *this shall contain the old value of r. r shall be empty.
    /// 26 Throws: nothing.
    shared_ptr(shared_ptr&& r)
      :base_type(r)
    {
      set(r.get());
      r.set(nullptr);
    }
    template<class Y> shared_ptr(shared_ptr<Y>&& r)
      :base_type(r)
    {
      set(r.get());
      r.set(nullptr);
    }

    /// 27 Requires: Y* shall be convertible to T*.
    /// 28 Effects: Constructs a shared_ptr object that shares ownership with r
    ///    and stores a copy of the pointer stored in r.
    /// 29 Postconditions: use_count() == r.use_count().
    /// 30 Throws: bad_weak_ptr when r.expired().
    /// 31 Exception safety: If an exception is thrown, the constructor has no effect.
    template<class Y> explicit shared_ptr(const weak_ptr<Y>& r);
#if 0
    : base_type(r)
    {
      if ( r.get() ) set(r.get());
      else throw bad_alloc();
    }
#endif

    /// 32 Requires: r.release() shall be convertible to T*. Y shall be a complete
    ///    type. The expression delete r.release() shall be well-formed,
    ///    shall have well defined behavior, and shall not throw exceptions.
    /// 33 Effects: Constructs a shared_ptr object that stores and owns r.release().
    /// 34 Postconditions: use_count() == 1 && r.get() == 0.
    /// 35 Throws: bad_alloc, or an implementation-defined exception when
    ///    a resource other than memory could not be obtained.
    /// 36 Exception safety: If an exception is thrown, the constructor has no effect.
    template<class Y> explicit shared_ptr(auto_ptr<Y>&& r)
      :base_type(r.get())
    {
      set(r.release());
    }

#ifdef NTL__CXX_EF
    /// 37 Effects: Equivalent to shared_ptr(r.release(), r.get_deleter())
    ///    when D is not a reference type,
    ///    otherwise shared_ptr(r.release(), ref(r.get_deleter())).
    /// 38 Exception safety: If an exception is thrown, the constructor has no effect.
    template <class Y, class D> explicit shared_ptr(const unique_ptr<Y, D>& r) = delete;
#endif

    template <class Y, class D> explicit shared_ptr(unique_ptr<Y, D>&& r)
    : base_type(r.release()) {/**/}

    ///\name  20.7.12.2.2 shared_ptr destructor [util.smartptr.shared.dest]
    ///
    /// 1 — If *this is empty or shares ownership with another shared_ptr instance
    ///   (use_count() > 1), there are no side effects.
    ///\todo: (?)
    ///   — Otherwise, if *this owns a pointer p and a deleter d, d(p) is called.
    ///   — Otherwise, *this owns a pointer p, and delete p is called.
    /// 2 Throws: nothing.
    /// 3 [ Note: Since the destruction of *this decreases the number of instances
    ///   that share ownership with *this by one, after *this has been destroyed
    ///   all shared_ptr instances that shared ownership with *this will report
    ///   a use_count() that is one less than its previous value. —end note ]
    __forceinline
      ~shared_ptr()
    {
      if ( unique() )
        free();
    }

    ///\name  20.7.12.2.3 shared_ptr assignment [util.smartptr.shared.assign]
    shared_ptr& operator=(shared_ptr&& r)
    {
      r.swap(*this); 
      return *this; 
    }

    template<class Y>
    shared_ptr& operator=(shared_ptr<Y>&& r)
    { 
      shared_ptr<T>(r).swap(*this);
      return *this;
    }

    template<class Y>
    shared_ptr& operator=(auto_ptr<Y>&& r)
    {
      set(r.release());
      return *this;
    }

    template <class Y, class D>
    shared_ptr& operator=(unique_ptr<Y, D>&& r)
    {
      set(r.release());
      return *this;
    }

#ifdef NTL__CXX_EF
    template <class Y, class D> shared_ptr& operator=(const unique_ptr<Y, D>& r) = delete;
#endif
    ///\name  20.7.12.2.4 shared_ptr modifiers [util.smartptr.shared.mod]

    /// 1 Effects: Exchanges the contents of *this and r.
    /// 2 Throws: nothing.
    void swap(shared_ptr&& r)
    {
      base_type::swap(r);
    }

    /// 3 Effects: Equivalent to shared_ptr().swap(*this).
    __forceinline
    void reset() { shared_ptr().swap(*this); }
    template<class Y> void reset(Y* p) { shared_ptr(p).swap(*this); }
    template<class Y, class D> void reset(Y* p, D d);

    ///\name  20.7.6.2.5 shared_ptr observers [util.smartptr.shared.obs]

    T & operator* ()  const __ntl_nothrow { return *base_type::get(); }
    T * operator->()  const __ntl_nothrow { return base_type::get(); }
    T * get()         const __ntl_nothrow { return base_type::get(); }

    //long use_count() const __ntl_nothrow { return base_type::use_count(); }
    using base_type::use_count;

    //bool unique()    const __ntl_nothrow { return base_type::unique(); }
    using base_type::unique;

    #ifndef NTL__CXX_EXPLICITOP
    //operator base_type::unspecified_bool_type() const __ntl_nothrow
    //{
    //  return base_type::operator base_type::unspecified_bool_type();
    //}
    using base_type::operator base_type::unspecified_bool_type;
    #else
    explicit operator bool() const;
    #endif

    ///@}

    ///////////////////////////////////////////////////////////////////////////
  private:

    void set(T * p) { base_type::set(p); }
    void free() { delete base_type::get(); }

  };

  ///\name  20.7.12.2.6 shared_ptr creation [util.smartptr.shared.create]
#ifdef NTL__CXX_VT
  /// 1 Requires: The expression new (pv) T(std::forward<Args>(args)...), where
  ///   pv has type void* and points to storage suitable to hold an object of type
  ///   T, shall be well-formed. A shall be an allocator (20.1.2).
  ///   The copy constructor and destructor of A shall not throw exceptions.
  /// 2 Effects: Allocates memory suitable for an object of type T and constructs
  ///   an object in that memory via the placement new expression new (pv) T()
  ///   or new (pv) T(std::forward<Args>(args)...). The template allocate_shared
  ///   uses a copy of a to allocate memory.
  ///   If an exception is thrown, the functions have no effect.
  /// 3 Returns: A shared_ptr instance that stores and owns the address of
  ///   the newly constructed object of type T.
  /// 4 Postconditions: get() != 0 && use_count() == 1
  /// 5 Throws: bad_alloc, or an exception thrown from A::allocate
  ///   or from the constructor of T.
  template<class T, class... Args> shared_ptr<T> make_shared(Args&&... args);
  template<class T, class A, class... Args>shared_ptr<T> allocate_shared(const A& a, Args&&... args);
#else

#endif

  ///\name  20.7.12.2.7 shared_ptr comparison [util.smartptr.shared.cmp]

  template<class T, class U>
  inline
    bool
    operator==(shared_ptr<T> const& a, shared_ptr<U> const& b)
  {
    return a.get() == b.get();
  }

  template<class T, class U>
  inline
    bool
    operator!=(shared_ptr<T> const& a, shared_ptr<U> const& b)
  {
    return a.get() != b.get();
  }

  /// 5 Returns: an unspecified value such that
  ///   — operator< is a strict weak ordering as described in 25.3;
  ///   — under the equivalence relation defined by operator<, !(a < b) && !(b < a),
  ///     two shared_ptr instances are equivalent if and only if they share
  ///     ownership or are both empty.
  /// 6 Throws: nothing.
  /// 7 [ Note: Defining a comparison operator allows shared_ptr objects
  ///   to be used as keys in associative containers. —end note ]
  template<class T, class U>
  inline
    bool
    operator<(shared_ptr<T> const& a, shared_ptr<U> const& b)
  {
    return reinterpret_cast<uintptr_t>(a.get())
      < reinterpret_cast<uintptr_t>(b.get());
  }

  ///\name  20.7.12.2.8 shared_ptr I/O [util.smartptr.shared.io]
  template<class E, class T, class Y>
  inline
    basic_ostream<E, T>&
    operator<< (basic_ostream<E, T>& os, shared_ptr<Y> const& p)
  {
    os << p.get();
  }

  ///\name  20.7.12.2.9 shared_ptr specialized algorithms [util.smartptr.shared.spec]
  template<class T>
  inline
    void
    swap(shared_ptr<T>& a, shared_ptr<T>& b) __ntl_nothrow
  {
    a.swap(b);
  }
  template<class T> void swap(shared_ptr<T>&& a, shared_ptr<T>& b)
  {
    a.swap(b);
  }
  template<class T> void swap(shared_ptr<T>& a, shared_ptr<T>&& b)
  {
    a.swap(b);
  }

  ///\name  20.7.12.2.10 shared_ptr casts [util.smartptr.shared.cast]

  template<class T, class U>
  inline
    shared_ptr<T>
    static_pointer_cast(shared_ptr<U> const& r);

  template<class T, class U>
  inline
    shared_ptr<T>
    dynamic_pointer_cast(shared_ptr<U> const& r);

  template<class T, class U>
  inline
    shared_ptr<T>
    const_pointer_cast(shared_ptr<U> const& r);

  ///@}

  /// 20.7.12.2.11 get_deleter [util.smartptr.getdeleter]
  template<class D, class T> D* get_deleter(shared_ptr<T> const& p);


  /// 20.7.12.3 Class template weak_ptr [util.smartptr.weak]
  template<class T>
  class weak_ptr
  {
    ///////////////////////////////////////////////////////////////////////////
  public:

    typedef T element_type;

    ///\name  20.7.12.3.1 weak_ptr constructors [util.smartptr.weak.const]

    weak_ptr();
    template<class Y> weak_ptr(shared_ptr<Y> const& r);
    weak_ptr(weak_ptr const& r);
    template<class Y> weak_ptr(weak_ptr<Y> const& r);

    ///\name  20.7.12.3.2 weak_ptr destructor [util.smartptr.weak.dest]
    ~weak_ptr();

    ///\name  20.7.12.3.3 weak_ptr assignment [util.smartptr.weak.assign]

    weak_ptr& operator=(weak_ptr const& r);
    template<class Y> weak_ptr& operator=(weak_ptr<Y> const& r);
    template<class Y> weak_ptr& operator=(shared_ptr<Y> const& r);

    ///\name  20.7.12.3.4 weak_ptr modifiers [util.smartptr.weak.mod]

    void swap(weak_ptr& r);
    void reset();

    ///\name  20.7.12.3.5 weak_ptr observers [util.smartptr.weak.obs]

    long use_count() const;
    bool expired() const;
    shared_ptr<T> lock() const;

    ///@}

    ///////////////////////////////////////////////////////////////////////////
  private:

  };

  ///\name  20.7.12.3.6 weak_ptr comparison [util.smartptr.weak.cmp]
  template<class T, class U>
  bool
    operator<(weak_ptr<T> const& a, weak_ptr<U> const& b);

  ///\name  20.7.12.3.7 weak_ptr specialized algorithms [util.smartptr.weak.spec]
  template<class T>
  void
    swap(weak_ptr<T>& a, weak_ptr<T>& b)
  {
    a.swap(b);
  }

  ///@}

  /// 20.7.12.4 Class template enable_shared_from_this [util.smartptr.enab]
  template<class T> 
  class enable_shared_from_this
  {
  protected:
    enable_shared_from_this();
    enable_shared_from_this(const enable_shared_from_this&);
    enable_shared_from_this& operator=(const enable_shared_from_this&);
    ~enable_shared_from_this();
  public:
    shared_ptr<T> shared_from_this();
    shared_ptr<T const> shared_from_this() const;
  };

  /// 20.7.12.6 Pointer safety [util.dynamic.safety]
#ifdef NTL__CXX_ENUM
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


  /// 20.7.13 Align [ptr.align]
#ifndef NTL__CXX_ALIGN
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

  ///\name  D.9 auto_ptr [depr.auto.ptr]

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

    X & operator* ()  const __ntl_nothrow { return *get(); }
    X * operator->()  const __ntl_nothrow { return get(); }
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

    ///@}

    ///////////////////////////////////////////////////////////////////////////
  private:

    X * ptr;
    void set(X * p) { ptr = p; }
  };

  /**@} lib_memory */
  /**@} lib_utilities */

}//namespace std

#endif//#ifndef NTL__STLX_SMARTPTR
