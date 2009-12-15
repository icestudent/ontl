/**\file*********************************************************************
 *                                                                     \brief
 *  N3000 20.8 Memory [memory]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_MEMORY
#define NTL__STLX_MEMORY
#pragma once

#ifndef NTL__STLX_CSTDDEF
#include "cstddef.hxx"
#endif

#ifndef NTL__STLX_ITERATOR
#include "iterator.hxx"         // for raw_storage_iterator<>
#endif

#ifndef NTL__STLX_INITLIST
#include "initializer_list.hxx"
#endif

#include "new.hxx"

#ifndef NTL__STLX_TYPE_TRAITS
#include "type_traits.hxx"
#endif

#ifndef NTL_SPP_ARGS_HXX
#include "../spp/args.hxx"
#endif

namespace std
{

/**\addtogroup  lib_utilities ********** 20 General utilities library [utilities]
 *@{*/
/**\addtogroup  lib_memory ************* 20.8 Memory [memory]
 *@{*/

///\name 20.8.3 Allocator-related traits [allocator.traits]
/**
 *	Automatically detects whether \c T has a nested \c allocator_type that is convertible from \c Alloc.
 **/
template<class T, class Alloc> struct uses_allocator
: public integral_constant<
    bool, is_convertible<Alloc, typename T::allocator_type>::value == true
    > {};


  /**
   *	@brief 20.8.6 Pointer traits [pointer.traits]
   *  @details The template class pointer_traits supplies a uniform interface to certain attributes of pointer-like types.
   **/
  template<class Ptr>
  struct pointer_traits
  {
    typedef Ptr pointer;
    typedef typename Ptr::element_type element_type;
    typedef typename Ptr::difference_type difference_type;

    template<class U> struct rebind { typedef U* other; }; //typedef Ptr::template rebind<U> other; };

    static pointer pointer_to(typename Ptr::element_type& r)
    {
      return Ptr::pointer_to(r);
    }
  };

  template<class T>
  struct pointer_traits<T*>
  {
    typedef T element_type;
    typedef T* pointer;
    typedef ptrdiff_t difference_type;

    template<class U> struct rebind { typedef U* other; };

    static pointer pointer_to(T& r)
    {
      return std::addressof(r);
    }
  };


  /**
   *	@brief 20.8.7 Allocator traits [allocator.traits]
   *  @details The template class allocator_traits supplies a uniform interface to all allocator types.
   **/
  template <class Alloc>
  struct allocator_traits
  {
    ///\name 20.8.7.1 Allocator traits member types [allocator.traits.types]
    typedef Alloc                           allocator_type;
    typedef typename Alloc::value_type      value_type;
    
    typedef typename Alloc::reference       reference;
    typedef typename Alloc::const_reference const_reference;

    typedef typename Alloc::pointer         pointer;
    typedef typename Alloc::const_pointer   const_pointer;

    typedef typename Alloc::difference_type difference_type;
    typedef typename Alloc::size_type       size_type;

    typedef typename pointer_traits<pointer>::rebind<void>::other void_pointer;
    typedef typename pointer_traits<pointer>::rebind<const void>::other const_void_pointer;
    
    typedef false_type propagate_on_container_copy_assignment;
    typedef false_type propagate_on_container_move_assignment;
    typedef false_type propagate_on_container_swap;

    template <class T> struct rebind_alloc { typedef typename Alloc::template rebind<T>::other type; };
    template <class T> struct rebind_traits { typedef allocator_traits<rebind_alloc<T> > type; };
    
    ///\name 20.8.7.2 Allocator traits static member functions [allocator.traits.members]
    static pointer allocate(Alloc& a, size_type n) { return a.allocate(n); }
    static pointer allocate(Alloc& a, size_type n, const_void_pointer hint) { return a.allocate(n, hint); }

    static void deallocate(Alloc& a, pointer p, size_type n) { a.deallocate(p, n); }
    
    static size_type max_size(const Alloc& a) { return a.max_size(); }
    static Alloc select_on_container_copy_construction(const Alloc& rhs) { return rhs; }

    template <class T>
    static void destroy(Alloc& a, T* p) { a.destroy(p); }

    //template <class T, class... Args>
    //static void construct(Alloc& a, T* p, Args&&... args);
#ifdef NTL__CXX_RV
    #define NTL_X(n,p) NTL_SPP_COMMA forward<NTL_SPP_CAT(A,n)>(NTL_SPP_CAT(p,n))
    #define NTL_DEFINE_CONSTRUCT(n,aux) \
    template <class T NTL_SPP_COMMA_IF(n) NTL_SPP_ARGS(1,n,class A)> \
    static void construct(Alloc& a, T* p NTL_SPP_COMMA_IF(n) NTL_SPP_AARGS(1,n,&& a)) { return a.construct(p NTL_SPP_LOOP(1,n,NTL_X,a)); }
#else
    #define NTL_X
    #define NTL_DEFINE_CONSTRUCT(n,aux) \
    template <class T NTL_SPP_COMMA_IF(n) NTL_SPP_ARGS(1,n,class A)> \
    static void construct(Alloc& a, T* p NTL_SPP_COMMA_IF(n) NTL_SPP_AARGS(1,n,const& a)) { return a.construct(p NTL_SPP_COMMA_IF(n) NTL_SPP_ARGS(1,n,a)); }
#endif
    NTL_DEFINE_CONSTRUCT(0,)
    NTL_DEFINE_CONSTRUCT(1,)
    NTL_DEFINE_CONSTRUCT(2,)
    NTL_DEFINE_CONSTRUCT(3,)
    NTL_DEFINE_CONSTRUCT(4,)
    NTL_DEFINE_CONSTRUCT(5,)
#undef NTL_X
#undef NTL_DEFINE_CONSTRUCT
    ///\}
  };

/// 20.8.8 The default allocator [default.allocator]
template<class T> class allocator;

/// specialize for void
template<>
class allocator<void>
{
  public:
    typedef void        * pointer;
    typedef const void  * const_pointer;
    typedef void          value_type;
    template<class U> struct rebind { typedef allocator<U> other; };
};

template<class T>
class allocator
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef       T   * pointer;
    typedef const T   * const_pointer;
    typedef       T   & reference;
    typedef const T   & const_reference;
    typedef       T     value_type;
    template<class U> struct rebind { typedef allocator<U> other; };

    allocator() __ntl_nothrow {}
    allocator(const allocator &) __ntl_nothrow {}
    template<class U> allocator(const allocator<U> &) __ntl_nothrow {}
    ~allocator() __ntl_nothrow {}

    ///\name  20.8.8.1 allocator members [allocator.members]

#ifndef NTL_STLEXT
    pointer address(typename add_reference<typename remove_const<typename remove_reference<reference>::type>::type>::type x) const
    {
      return &x;
    }
    const_pointer address(const_reference x) const { return &x; }
#else
    template<typename value_type>
    value_type * address(value_type& x) const
    {
      const_pointer check_type = &x; (check_type);
      return &x;
    }
#endif

    __noalias
    __forceinline
    T* __restrict allocate(size_type n, allocator<void>::const_pointer = 0)
      __ntl_throws(bad_alloc)
    {
      const pointer p = reinterpret_cast<T*>(::operator new(sizeof(T) * n));
      __assume(p);
      return p;
    }

    __noalias
    __forceinline
    void deallocate(pointer p, size_type /* n */)
    {
      ::operator delete(const_cast<typename remove_const<T>::type*>(p));//(address(*p));
    }

    size_type max_size() const __ntl_nothrow { return size_t(-1) / sizeof(T); }

    //    __forceinline
    void destroy(const pointer p)
    {
      p->T::/*< workaround MSVC's weird `scalar deleting destructor'*/ ~T();
    }

#if defined(NTL__CXX_VT)

    template<class... Args>
    void construct(pointer p, Args&&... args)
    {
      __assume(p);
      ::new((void*)p) T(forward<Args>(args)...);
    }
#else

#ifdef NTL__CXX_RV
    #define NTL_X(n,p) forward<NTL_SPP_CAT(A,n)>(NTL_SPP_CAT(p,n))
    #define NTL_DEFINE_CONSTRUCT(n,aux) \
    template <class U NTL_SPP_COMMA_IF(n) NTL_SPP_ARGS(1,n,class A)> \
    void construct(U* p NTL_SPP_COMMA_IF(n) NTL_SPP_AARGS(1,n,&& a)) { __assume(p); ::new( (void*)p ) T(NTL_SPP_LOOP(1,n,NTL_X,a)); }
#else
    #define NTL_X
    #define NTL_DEFINE_CONSTRUCT(n,aux) \
    template <class U NTL_SPP_COMMA_IF(n) NTL_SPP_ARGS(1,n,class A)> \
    void construct(U* p NTL_SPP_COMMA_IF(n) NTL_SPP_AARGS(1,n,const& a)) { __assume(p); ::new( (void*)p ) T(NTL_SPP_ARGS(1,n,a)); }
#endif

    NTL_DEFINE_CONSTRUCT(0,)
    NTL_DEFINE_CONSTRUCT(1,)
    NTL_DEFINE_CONSTRUCT(2,)
    NTL_DEFINE_CONSTRUCT(3,)
    NTL_DEFINE_CONSTRUCT(4,)
    NTL_DEFINE_CONSTRUCT(5,)
#undef NTL_X
#undef NTL_DEFINE_CONSTRUCT
#endif // CXX VT

    ///\}

};//class allocator


///\name  20.8.8.2 allocator globals [allocator.globals]

template<class T, class U>
inline bool operator==(const allocator<T>&, const allocator<U>&) __ntl_nothrow
{
  return true;
}

template<class T, class U>
inline bool operator!=(const allocator<T>&, const allocator<U>&) __ntl_nothrow
{
  return false;
}

///\}

///\ 20.8.6 Scoped allocator adaptor [allocator.adaptor]
template <class OuterA, class InnerA = void>
class scoped_allocator_adaptor;

template <class Alloc>
class scoped_allocator_adaptor<Alloc, void>:
  public Alloc
{
public:
  // outer and inner allocator types are the same:
  typedef Alloc outer_allocator_type;
  typedef Alloc inner_allocator_type;

  typedef typename outer_allocator_type::size_type size_type;
  typedef typename outer_allocator_type::difference_type difference_type;
  typedef typename outer_allocator_type::pointer pointer;
  typedef typename outer_allocator_type::const_pointer const_pointer;
  typedef typename outer_allocator_type::reference reference;
  typedef typename outer_allocator_type::const_reference const_reference;
  typedef typename outer_allocator_type::value_type value_type;

  template <typename _Tp>
  struct rebind
  {
    typedef scoped_allocator_adaptor<
      typename Alloc::template rebind<_Tp>::other, void> other;
  };

  scoped_allocator_adaptor();
  #ifdef NTL__CXX_RV
  scoped_allocator_adaptor(scoped_allocator_adaptor&&);
  #endif
  scoped_allocator_adaptor(const scoped_allocator_adaptor&);
  #ifdef NTL__CXX_RV
  scoped_allocator_adaptor(Alloc&& outerAlloc);
  #endif
  scoped_allocator_adaptor(const Alloc& outerAlloc);
  #ifdef NTL__CXX_RV
  template <typename OuterA2>
  scoped_allocator_adaptor(scoped_allocator_adaptor<OuterA2, void>&&);
  #endif
  template <typename OuterA2>
  scoped_allocator_adaptor(const scoped_allocator_adaptor<OuterA2, void>&);
  ~scoped_allocator_adaptor();

  pointer address(reference x) const;
  const_pointer address(const_reference x) const;

  pointer allocate(size_type n);
  template <typename _HintP>
  pointer allocate(size_type n, _HintP u);
  void deallocate(pointer p, size_type n);

  size_type max_size() const;

  #ifdef NTL__CXX_VT
  template <class... Args>
  void construct(pointer p, Args&&... args);
  #else
  void construct(pointer p, const value_type& val);
  #endif

  void destroy(pointer p);

  const outer_allocator_type& outer_allocator();
  const inner_allocator_type& inner_allocator();
};


template<typename OuterA, typename InnerA>
class scoped_allocator_adaptor : public OuterA
{
public:
  typedef OuterA outer_allocator_type;
  typedef InnerA inner_allocator_type;
  typedef typename outer_allocator_type::size_type size_type;
  typedef typename outer_allocator_type::difference_type difference_type;
  typedef typename outer_allocator_type::pointer pointer;
  typedef typename outer_allocator_type::const_pointer const_pointer;
  typedef typename outer_allocator_type::reference reference;
  typedef typename outer_allocator_type::const_reference const_reference;
  typedef typename outer_allocator_type::value_type value_type;

  template <typename _Tp>
  struct rebind
  {
    typedef scoped_allocator_adaptor<
      typename OuterA::template rebind<_Tp>::other,
      InnerA> other;
  };

  scoped_allocator_adaptor();
  #ifdef NTL__CXX_RV
  scoped_allocator_adaptor(outer_allocator_type&& outerAlloc,
    inner_allocator_type&& innerAlloc);
  #endif
  scoped_allocator_adaptor(const outer_allocator_type& outerAlloc,
    const inner_allocator_type& innerAlloc);
  #ifdef NTL__CXX_RV
  scoped_allocator_adaptor(scoped_allocator_adaptor&& other);
  #endif
  scoped_allocator_adaptor(const scoped_allocator_adaptor& other);
  #ifdef NTL__CXX_RV
  template <typename OuterAlloc2>
  scoped_allocator_adaptor(
    scoped_allocator_adaptor<OuterAlloc2&,InnerA>&&);
  #endif
  template <typename OuterAlloc2>
  scoped_allocator_adaptor(
    const scoped_allocator_adaptor<OuterAlloc2&,InnerA>&);
  ~scoped_allocator_adaptor();

  pointer address(reference x) const;
  const_pointer address(const_reference x) const;

  pointer allocate(size_type n);
  template <typename _HintP>
  pointer allocate(size_type n, _HintP u);
  void deallocate(pointer p, size_type n);

  size_type max_size() const;


  #ifdef NTL__CXX_VT
  template <class... Args>
  void construct(pointer p, Args&&... args);
  #else
  void construct(pointer p, const value_type& val);
  #endif

  void destroy(pointer p);

  const outer_allocator_type& outer_allocator() const;
  const inner_allocator_type& inner_allocator() const;
};


template<typename OuterA1, typename OuterA2, typename InnerA>
inline bool operator==(const scoped_allocator_adaptor<OuterA1,InnerA>& a, const scoped_allocator_adaptor<OuterA2,InnerA>& b)
{
  return a.outer_allocator() == b.outer_allocator() && a.inner_allocator() == b.inner_allocator();
}

template<typename OuterA1, typename OuterA2, typename InnerA>
inline bool operator!=(const scoped_allocator_adaptor<OuterA1,InnerA>& a, const scoped_allocator_adaptor<OuterA2,InnerA>& b)
{
  return !(a == b);
}




/// 20.8.7 Raw storage iterator [storage.iterator]
template<class OutputIterator, class T>
class raw_storage_iterator
: public iterator<output_iterator_tag, void, void, void, void>
{
    typedef raw_storage_iterator<OutputIterator, T> this_type;

  public:
    explicit raw_storage_iterator(OutputIterator x) : i(x) {}
    this_type& operator* ()     { return *this; }
    this_type& operator++()     { ++i; return *this; }
    this_type  operator++(int)  { this_type old( i ); ++i; return old; }
    this_type& operator= (const T& element)
    {
      allocator<T>().construct(&*i, element);
      return *this;
    }

  private:
    OutputIterator  i;
};


///\name  20.8.8 Temporary buffers [temporary.buffer]
template<class T1, class T2> struct pair;

template <class T>
__forceinline
pair<T*,ptrdiff_t>
  get_temporary_buffer(ptrdiff_t n)
{
  T* p = allocator<T>().allocate(static_cast<typename allocator<T>::size_type>(n));
  return make_pair(p, p ? n : 0);
}

template <class T>
__forceinline
void
  return_temporary_buffer(T* p)
{
  // 20.8.1.1/7  n shall equal the value passed as the first argument
  //             to the invocation of allocate which returned p.
  // but allocator::deallocate() does not use n
  typename allocator<T>::size_type const n = 0;
  allocator<T>().deallocate(p, n);
}

///\name  20.8.10 Specialized algorithms [specialized.algorithms]

/// 20.8.13.1 addressof [specialized.addressof ]
template <class T>
inline T* addressof(T& r) __ntl_nothrow { return &r; }

/// 20.8.10.1 uninitialized_copy [uninitialized.copy]
template <class InputIterator, class ForwardIterator>
__forceinline
ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result)
{
  typedef typename iterator_traits<ForwardIterator>::value_type value_type;
  for ( ; first != last; ++result, ++first )
  {
    __assume(&*result);
    new (static_cast<void*>(&*result)) value_type(*first);
    ///@todo delete on exceptions
  }
  ///@todo separate function for scalar types ?
  return result;
}

template <class InputIterator, class Size, class ForwardIterator>
__forceinline
ForwardIterator uninitialized_copy_n(InputIterator first, Size n,
                                     ForwardIterator result)
{
  typedef typename iterator_traits<ForwardIterator>::value_type value_type;
  for(; n--; ++result, ++first){
    __assume(&*result);
    new (static_cast<void*>(&*result)) value_type(*first);
  }
  return result;
}

template <class InputIterator, class ForwardIterator>
__forceinline
ForwardIterator uninitialized_move(InputIterator first, InputIterator last, ForwardIterator result)
{
  typedef typename iterator_traits<ForwardIterator>::value_type value_type;
  for ( ; first != last; ++result, ++first )
  {
    __assume(&*result);
    new (static_cast<void*>(&*result)) value_type(move(*first));
    ///@todo delete on exceptions
  }
  ///@todo separate function for scalar types ?
  return result;
}

template <class InputIterator, class Size, class ForwardIterator>
__forceinline
ForwardIterator uninitialized_move_n(InputIterator first, Size n,
                                     ForwardIterator result)
{
  typedef typename iterator_traits<ForwardIterator>::value_type value_type;
  for(; n--; ++result, ++first){
    __assume(&*result);
    new (static_cast<void*>(&*result)) value_type(move(*first));
  }
  return result;
}

/// 20.8.10.2 uninitialized_fill [uninitialized.fill]
template <class ForwardIterator, class T>
__forceinline
void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x)
{
  typedef typename iterator_traits<ForwardIterator>::value_type value_type;
  for ( ; first != last; ++first )
  {
    __assume(&*first);
    new (static_cast<void*>(&*first)) value_type(x);
    ///@todo delete on exceptions
  }
  ///@todo separate function for scalar types ?
}

/// 20.8.10.3 uninitialized_fill_n [uninitialized.fill.n]
template <class ForwardIterator, class Size, class T>
__forceinline
void uninitialized_fill_n(ForwardIterator first, Size n, const T& x)
{
  typedef typename iterator_traits<ForwardIterator>::value_type value_type;
  for ( ; n--; ++first )
  {
    __assume(&*first);
    new (static_cast<void*>(&*first)) value_type(x);
    ///@todo delete on exceptions
  }
  ///@todo separate function for scalar types ?
}

///@}

///\}

namespace __ {

  /**
   *	Static storage helper
   **/
  template<class T>
  struct static_storage
  {
    /** returns uninitialized storage */
    static T* get_buffer()
    {
      // reserve space for constructed flag
      static aligned_storage<sizeof(T)+sizeof(int), alignof(T)>::type buf;
      return reinterpret_cast<T*>(&buf);
    }

    /** returns default-constructed object */
    static T* get_object()
    {
      T* p = get_buffer();
      if(!check_constructed(p, true))
        allocator<T>().construct(p);
      return p;
    }

    /** returns object which constructed with specified argument(s) */
    template<class A1>
    static T* get_object(const A1& a1)
    {
      T* p = get_buffer();
      if(!check_constructed(p, true))
        allocator<T>().construct(p, a1);
      return p;
    }
    template<class A1, class A2>
    static T* get_object(const A1& a1, const A2& a2)
    {
      T* p = get_buffer();
      if(!check_constructed(p, true))
        allocator<T>().construct(p, a1, a2);
      return p;
    }
    template<class A1, class A2, class A3>
    static T* get_object(const A1& a1, const A2& a2, const A3& a3)
    {
      T* p = get_buffer();
      if(!check_constructed(p, true))
        allocator<T>().construct(p, a1, a2, a3);
      return p;
    }
    /** schedules object destruction at program exit */
    static void schedule_destruction(T*) { atexit(dtor); }

    /** decrements object's reference count and destroys the object if it is zero */
    static bool destroy_object(T*& p)
    {
      bool re = check_constructed(p, false) == 1;
      if(re)
        allocator<T>().destroy(p);
      p = nullptr;
      return re;
    }

    /** returns object's reference count */
    static int refcount()
    {
      const T* const p = get_buffer();
      return *reinterpret_cast<const int*>(p+1);
    }

    /** decrements reference count */
    static int release()
    {
      T* p = get_buffer();
      destroy_object(p);
      return refcount();
    }
  private:
    static void __cdecl dtor()
    {
      T* p = get_buffer();
      destroy_object(p);
    }
    // returns true if object already constructed
    static bool is_constructed(const T* p)
    {
      return *reinterpret_cast<const int*>(p+1) != 0;
    }
    // marks the object and returns previous state
    static int check_constructed(T* p, bool mark)
    {
      int re = *reinterpret_cast<int*>(p+1);
      *reinterpret_cast<int*>(p+1) += mark ? 1 : -1;
      return re;
    }
  };
} // __

/**@} lib_memory */
/**@} lib_utilities */

}//namespace std

#endif//#ifndef NTL__STLX_MEMORY
