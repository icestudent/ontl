/**\file*********************************************************************
 *                                                                     \brief
 *  N2588 20.7 Memory [memory]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_MEMORY
#define NTL__STLX_MEMORY

#include "cstddef.hxx"
#include "exception.hxx"
#include "iterator.hxx"
#include "type_traits.hxx"

#include "initializer_list.hxx"

#include "new.hxx"

namespace std {

/**\addtogroup  lib_utilities ********** 20 General utilities library [utilities] *****
 *@{*/
/**\defgroup  lib_memory *************** 20.7 Memory [memory] **********************
 *@{*/

///\name 20.7.2 Allocator-related traits [allocator.traits]
/**
 *	Automatically detects whether \c T has a nested \c allocator_type that is convertible from \c Alloc.
 **/
template<class T, class Alloc> struct uses_allocator
: public integral_constant<
    bool, is_convertible<Alloc, typename T::allocator_type>::value == true
    > {};
/**
 *	If a specialization \c is_scoped_allocator<Alloc> is derived from \c true_type, it indicates that
 *  \c Alloc is a \c scoped allocator. A scoped allocator specifies the memory resource to be used by a container
 *  (as all allocators do) and also specifies an inner allocator resource to be used by every element of the container.
 *
 *  @note \e Requires: If a specialization \c is_scoped_allocator<Alloc> is derived from \c true_type,
 *  \c Alloc shall have a nested type \c inner_allocator_type and a member function \c inner_allocator() which is
 *  callable with no arguments and which returns an object of a type that is convertible to \c inner_allocator_type.
 **/
template<class Alloc> struct is_scoped_allocator : false_type {};

/**
 *	If a specialization \c constructible_with_allocator_suffix<T> is derived from \c true_type, it
 *  indicates that \c T can be constructed with an allocator as its last constructor argument. Ideally, all
 *  constructors of \c T (including copy and move constructors) should have a variant that accepts a final
 *  argument of \c allocator_type.
 *
 *  @note \e Requires: If a specialization \c constructible_with_allocator_suffix<T> is derived from \c true_type,
 *  \c T shall have a nested type \c allocator_type and at least one constructor for which \c allocator_type is
 *  the last parameter. If not all constructors of \c T can be called with a final \c allocator_type argument
 *  and \c T is used in a context where a container must call such a constructor, the program is ill-formed.
 **/
template<class T> struct constructible_with_allocator_suffix : false_type {};

/**
 *	If a specialization \c constructible_with_allocator_prefix<T> is derived from \c true_type,
 *  it indicates that \c T can be constructed with allocator_arg and \c T::allocator_type as its first two
 *  constructor arguments. Ideally, all constructors of \c T (including copy and move constructors) should
 *  have a variant that accepts these two initial arguments.
 *
 *  @note \e Requires: If a specialization \c constructible_with_allocator_suffix<T> is derived from \c true_type,
 *  \c T shall have a nested type \c allocator_type and at least one constructor for which \c allocator_arg_t
 *  is the first parameter and \c allocator_type is the second parameter. If not all constructors of \c T can
 *  be called with these initial arguments and \c T is used in a context where a container must call such a
 *  constructor, the program is ill-formed.
 **/
template<class T> struct constructible_with_allocator_prefix : false_type {};
///}


///\name 20.7.3 Allocator propagation traits [allocator.propagation]
/**
 *	If specialized to derive from \c true_type for a specific allocator type, indicates that a container
 *  using the specified \c Alloc should not copy or move the allocator when the container is copy-constructed,
 *  move-constructed, copy-assigned, moved-assigned, or swapped.
 *  @note \e Requires: Alloc shall be an Allocator (20.1.2).
 **/
template<class Alloc> struct allocator_propagate_never
: false_type {};

/**
 *	If specialized to derive from \c true_type for specific allocator type, indicates that a container
 *  using the specified \c Alloc should copy or move the allocator when the container is copy constructed,
 *  move constructed, move assigned, or swapped but not when the container is copy assigned.
 *  @note \e Requires: Alloc shall be an Allocator (20.1.2).
 **/
template<class Alloc> struct allocator_propagate_on_move_assignment
: false_type {};

/**
 *	If specialized to derive from \c true_type for a specific allocator type, indicates that a container
 *  using the specified \c Alloc should copy or move the allocator when the container is copy constructed,
 *  move constructed, move assigned, swapped or copy assigned.
 *  @note \e Requires: Alloc shall be an Allocator (20.1.2).
 **/
template<class Alloc> struct allocator_propagate_on_copy_assignment
: false_type {};

/**
 *	If specialized to derive from \c true_type for specific allocator type, indicates that a container
 *  using the specified \c Alloc should copy or move the allocator when the container is copy constructed or
 *  move constructed, but not when the container is copy assigned, moved assigned, or swapped.
 *  @note \e Requires: Alloc shall be an Allocator (20.1.2).
 **/
template<class Alloc> struct allocator_propagate_on_copy_construction
: integral_constant<bool, !
   (allocator_propagate_never<Alloc>::value ||
    allocator_propagate_on_move_assignment<Alloc>::value ||
    allocator_propagate_on_copy_assignment<Alloc>::value)>
  {};

///}

/// 20.7.3 20.7.4 Allocator propogation map [allocator.propagation.map]
/**
 *	The \c allocator_propagation_map provides functions to be used by containers for manipulating
 *  allocators during construction, assignment, and swap operations. The implementations of the functions
 *  above are dependent on the allocator propagation traits of the specific \c Alloc.
 *  @note \e Requires: Exactly one propagation trait shall derive from \c true_type for \c Alloc.
 **/
template<class Alloc> struct allocator_propagation_map
{
  static Alloc select_for_copy_construction(const Alloc& x)
  {
    return allocator_propagate_never<Alloc>::value ? Alloc() : x;
  }
#ifdef NTL__CXX_RV
  static void move_assign(Alloc& to, Alloc&& from)
  {
    if ( allocator_propagate_on_move_assignment<Alloc>::value
      || allocator_propagate_on_copy_assignment<Alloc>::value )
      to = forward(from);
  }
#endif
  static void copy_assign(Alloc& to, Alloc& from)
  {
    if ( allocator_propagate_on_copy_assignment<Alloc>::value )
      to = from;
  }
  static void swap(Alloc& a, Alloc& b)
  {
    // anywhy UB
    if ( allocator_propagate_on_move_assignment<Alloc>::value
      || allocator_propagate_on_copy_assignment<Alloc>::value )
    {
      if ( !(a == b ) )
        swap(a, b);
    }else{
      // UB
    }
  }
};

/// 20.7.5 The default allocator [default.allocator]
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

    ///\name  20.7.5.1 allocator members [allocator.members]

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

#if defined(NTL__CXX_VT)
    template<class... Args>
    void construct(pointer p, Args&&... args)
    {
      __assume(p);
      ::new((void*)p) T(forward<Args>(args)...);
    }
#elif defined(NTL__CXX_RV) //&& 0
    template<class Arg>
    __forceinline
    void construct(pointer p, Arg&& val)
    {
      __assume(p);
      ::new((void *)p) T(std::forward<Arg>(val));
    }
    template<class Arg, class Arg2>
    __forceinline
    void construct(pointer p, Arg&& val, Arg2&& val2)
    {
      __assume(p);
      ::new((void *)p) T(std::forward<Arg>(val), std::forward<Arg2>(val2));
    }
    template<class Arg, class Arg2, class Arg3>
    __forceinline
    void construct(pointer p, Arg&& val, Arg2&& val2, Arg3&& val3)
    {
      __assume(p);
      ::new((void *)p) T(std::forward<Arg>(val), std::forward<Arg2>(val2), std::forward<Arg3>(val3));
    }
    template<class Arg, class Arg2, class Arg3, class Arg4>
    __forceinline
    void construct(pointer p, Arg&& val, Arg2&& val2, Arg3&& val3, Arg4&& val4)
    {
      __assume(p);
      ::new((void *)p) T(std::forward<Arg>(val), std::forward<Arg2>(val2), std::forward<Arg3>(val3), std::forward<Arg4>(val4));
    }
    template<class Arg, class Arg2, class Arg3, class Arg4, class Arg5>
    __forceinline
    void construct(pointer p, Arg&& val, Arg2&& val2, Arg3&& val3, Arg4&& val4, Arg5&& val5)
    {
      __assume(p);
      ::new((void *)p) T(std::forward<Arg>(val), std::forward<Arg2>(val2), std::forward<Arg3>(val3), std::forward<Arg4>(val4), std::forward<Arg5>(val5));
    }
#else
    __forceinline
    void construct(pointer p)
    {
      __assume(p);
      ::new((void *)p) T();
    }
    template<class Arg>
    __forceinline
    void construct(pointer p, const Arg& val)
    {
      __assume(p);
      ::new((void *)p) T(val);
    }
    template<class A1, class A2>
    __forceinline
    void construct(pointer p, const A1& a1, const A2& a2)
    {
      __assume(p);
      ::new((void *)p) T(a1,a2);
    }
    template<class A1, class A2, class A3>
    __forceinline
    void construct(pointer p, const A1& a1, const A2& a2, const A3& a3)
    {
      __assume(p);
      ::new((void *)p) T(a1,a2,a3);
    }
    template<class A1, class A2, class A3, class A4>
    __forceinline
      void construct(pointer p, const A1& a1, const A2& a2, const A3& a3, const A4& a4)
    {
      __assume(p);
      ::new((void *)p) T(a1,a2,a3,a4);
    }
    template<class A1, class A2, class A3, class A4, class A5>
    __forceinline
      void construct(pointer p, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
    {
      __assume(p);
      ::new((void *)p) T(a1,a2,a3,a4,a5);
    }
#endif

//    __forceinline
    void destroy(const pointer p)
    {
      p->T::/*< workaround MSVC's weird `scalar deleting destructor'*/ ~T();
    }

    ///\}

};//class allocator


template<class T> struct allocator_propagate_never<allocator<T> >
: true_type {};


///\name  20.7.5.2 allocator globals [allocator.globals]

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

///\ 20.7.6 Scoped allocator adaptor [allocator.adaptor]
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

template <class OuterA, class InnerA>
struct is_scoped_allocator<scoped_allocator_adaptor<OuterA, InnerA> >
  : true_type { };

template <class OuterA, class InnerA>
struct allocator_propagate_never<scoped_allocator_adaptor<OuterA, InnerA> >
  : true_type { };

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




/// 20.7.7 Raw storage iterator [storage.iterator]
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


///\name  20.7.8 Temporary buffers [temporary.buffer]
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
  // 20.7.1.1/7  n shall equal the value passed as the first argument
  //             to the invocation of allocate which returned p.
  // but allocator::deallocate() does not use n
  typename allocator<T>::size_type const n = 0;
  allocator<T>().deallocate(p, n);
}

/// 20.7.9 construct_element [construct.element]
#ifdef NTL__CXX_VT
namespace __
{
  template<bool prefix, bool suffix>
  struct construct_element_policy
  {
    template <class Alloc, class T, class... Args>
    static void construct_element(Alloc& alloc, T& r, Args&&... args)
    {
      alloc.construct(alloc.address(r), args...);
    }
  };

  template<bool suffix>
  struct construct_element_policy<true, suffix>
  {
    template <class Alloc, class T, class... Args>
    static void construct_element(Alloc& alloc, T& r, Args&&... args)
    {
      alloc.construct(alloc.address(r), allocator_arg_t, alloc.inner_allocator(), args...);
    }
  };

  template<bool prefix>
  struct construct_element_policy<prefix, true>
  {
    template <class Alloc, class T, class... Args>
    static void construct_element(Alloc& alloc, T& r, Args&&... args)
    {
      alloc.construct(alloc.address(r), args..., alloc.inner_allocator());
    }
  };
}


template <class Alloc, class T, class... Args>
void construct_element(Alloc& alloc, T& r, Args&&... args)
{
  typedef typename
    __select_or<is_scoped_allocator<Alloc>::value == false, uses_allocator<T, Alloc::inner_allocator_type>::value == false,
      __::construct_element_policy<false, false>,
      __::construct_element_policy<
          constructible_with_allocator_prefix<T, Alloc::inner_allocator_type, Args...>::value,
          constructible_with_allocator_suffix<T, Alloc::inner_allocator_type, Args...>::value> policy;
  policy::construct_element(alloc, r, args...);
}
#endif

///\name  20.7.10 Specialized algorithms [specialized.algorithms]

/// 20.7.10.1 uninitialized_copy [uninitialized.copy]
template <class InputIterator, class ForwardIterator>
__forceinline
ForwardIterator
  uninitialized_copy(InputIterator    first,
                     InputIterator    last,
                     ForwardIterator  result)
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

/// 20.7.10.2 uninitialized_fill [uninitialized.fill]
template <class ForwardIterator, class T>
__forceinline
void
  uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x)
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

/// 20.7.10.3 uninitialized_fill_n [uninitialized.fill.n]
template <class ForwardIterator, class Size, class T>
__forceinline
void
  uninitialized_fill_n(ForwardIterator first, Size n, const T& x)
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
    static void schedule_destruction(T* p) { atexit(dtor); }

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
      destroy_object(get_buffer());
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
