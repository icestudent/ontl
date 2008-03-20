/**\file*********************************************************************
 *                                                                     \brief
 *  N2315 20.6 Memory [memory]
 *
 *  ///\note C++0x move semantics not supported yet
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_MEMORY
#define NTL__STLX_MEMORY

#include "cstddef.hxx"
#include "iterator.hxx"
#include "type_traits.hxx"
#include "utility.hxx"
#include "new.hxx"

namespace std {

/**\addtogroup  lib_utilities ********** [20] General utilities library *****
 *@{*/
/**\defgroup  lib_memory *************** [20.6] Memory **********************
 *@{*/

/// 20.6.1 The default allocator [default.allocator]
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

    allocator() throw() {}
    allocator(const allocator &) throw() {}
    template<class U> allocator(const allocator<U> &) throw() {}
    ~allocator() throw() {}

    ///\name  20.6.1.1 allocator members [allocator.members]

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

    __declspec(noalias) __declspec(restrict)
    __forceinline
    pointer allocate(size_type n, allocator<void>::const_pointer hint = 0)
      throw(bad_alloc)
    {
      (hint);
      const pointer p = reinterpret_cast<T*>(::operator new(sizeof(T) * n));
      __assume(p);
      return p;
    }

    __declspec(noalias)
    __forceinline
    void deallocate(pointer p, size_type /* n */) 
    {
      ::delete(p);//(address(*p));
    }

    size_type max_size() const throw() { return size_t(-1) / sizeof(T); }

    __forceinline
    void construct(pointer p, const T & val)
    { 
      __assume(p);
      ///\todo ::new((void *)p ) T(std::forward<U >(val ))
      ::new((void *)p) T(val);
    }

    __forceinline
    void destroy(const pointer p)
    {
      p->T::/*< workaround MSVC's weird `scalar deleting destructor'*/ ~T();  
    }

    ///\}

};//class allocator

///\name  20.6.1.2 allocator globals [allocator.globals]

template<class T, class U> 
inline
bool
  operator==(const allocator<T>&, const allocator<U>&) throw()
{ 
  return true;
}

template<class T, class U>
inline
bool
  operator!=(const allocator<T>&, const allocator<U>&) throw()
{ 
  return false;
}

///\}

/// 20.6.2 Raw storage iterator [storage.iterator]
template<class OutputIterator, class T>
class raw_storage_iterator 
: public iterator<output_iterator_tag, void, void, void, void>
{
    typedef raw_storage_iterator<OutputIterator, T> this_type;

  public:
    explicit raw_storage_iterator(OutputIterator x) : i(x) {}
    this_type& operator* ()     { return *this; }
    this_type& operator= (const T& element)
      { allocator().construct(&*i, element); }
    this_type& operator++()     { ++i; return *this; }
    this_type  operator++(int)  { this_type old( i ); ++i; return old; }

  private:
    OutputIterator  i;
};

///\name  20.6.3 Temporary buffers [temporary.buffer]

template <class T>
__forceinline
pair<T*,ptrdiff_t>
  get_temporary_buffer(ptrdiff_t n)
{
  T* p = allocator<T>().allocate(static_cast<allocator<T>::size_type>(n));
  return make_pair(p, p ? n : 0);
}

template <class T>
__forceinline
void
  return_temporary_buffer(T* p)
{ 
  // 20.6.1.1/7  n shall equal the value passed as the first argument
  //             to the invocation of allocate which returned p.
  // but allocator::deallocate() does not use n
  allocator<T>::size_type const n = 0;
  allocator<T>().deallocate(p, n); 
}

///\name  20.6.4 Specialized algorithms [specialized.algorithms]

/// 20.6.4.1 uninitialized_copy [uninitialized.copy]
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

/// 20.6.4.2 uninitialized_fill [uninitialized.fill]
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

/// 20.6.4.3 uninitialized_fill_n [uninitialized.fill.n]
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

///\todo 20.6.5 Class template unique_ptr [unique.ptr]


///\name  20.6.6 Smart pointers [util.smartptr]

template<class X> class auto_ptr;

/// 20.6.6.1 Class bad_weak_ptr [util.smartptr.weakptr]
class bad_weak_ptr;

template<class T> class weak_ptr;

/// 20.6.6.2 Class template shared_ptr [util.smartptr.shared]
template<class T>
class shared_ptr
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef T element_type;

    ///\name  20.6.6.2.1 shared_ptr constructors [util.smartptr.shared.const]

    shared_ptr();
    template<class Y> explicit shared_ptr(Y* p);
    template<class Y, class D> shared_ptr(Y* p, D d);
    shared_ptr(shared_ptr const& r);
    template<class Y> shared_ptr(shared_ptr<Y> const& r);
    template<class Y> explicit shared_ptr(weak_ptr<Y> const& r);
    template<class Y> explicit shared_ptr(auto_ptr<Y>& r);

    ///\name  20.6.6.2.2 shared_ptr destructor [util.smartptr.shared.dest]
    ~shared_ptr();

    ///\name  20.6.6.2.3 shared_ptr assignment [util.smartptr.shared.assign]
    shared_ptr& operator=(shared_ptr const& r);
    template<class Y> shared_ptr& operator=(shared_ptr<Y> const& r);
    template<class Y> shared_ptr& operator=(auto_ptr<Y>& r);

    ///\name  20.6.6.2.4 shared_ptr modifiers [util.smartptr.shared.mod]

    void swap(shared_ptr& r);
    void reset();
    template<class Y> void reset(Y* p);
    template<class Y, class D> void reset(Y* p, D d);

    ///\name  20.6.6.2.5 shared_ptr observers [util.smartptr.shared.obs]

    T & operator* ()  const throw() { return *get(); }
    T * operator->()  const throw() { return get(); }
    T * get()         const;//throw()

    long use_count() const;//throw()
    bool unique() const;//throw()
    operator bool () const;//throw()

    ///@}

  ///////////////////////////////////////////////////////////////////////////
  private:
  
    T * ptr;
    void set(T * p) { ptr = p; }
};

///\name  20.6.6.2.6 shared_ptr comparison [util.smartptr.shared.cmp]

template<class T, class U>
bool
  operator==(shared_ptr<T> const& a, shared_ptr<U> const& b);

template<class T, class U>
bool
  operator!=(shared_ptr<T> const& a, shared_ptr<U> const& b);

template<class T, class U>
bool
  operator<(shared_ptr<T> const& a, shared_ptr<U> const& b);

///\name  20.6.6.2.7 shared_ptr I/O [util.smartptr.shared.io]
template<class E, class T, class Y>
basic_ostream<E, T>& 
  operator<< (basic_ostream<E, T>& os, shared_ptr<Y> const& p)
{
  os << p.get();
}

///\name  20.6.6.2.8 shared_ptr specialized algorithms [util.smartptr.shared.spec]
template<class T> 
void
  swap(shared_ptr<T>& a, shared_ptr<T>& b) throw()
{
  a.swap(b);
}

///\name  20.6.6.2.9 shared_ptr casts [util.smartptr.shared.cast]

template<class T, class U>
shared_ptr<T>
  static_pointer_cast(shared_ptr<U> const& r);

template<class T, class U>
shared_ptr<T>
  dynamic_pointer_cast(shared_ptr<U> const& r);

template<class T, class U>
shared_ptr<T> 
  const_pointer_cast(shared_ptr<U> const& r);

///@}

/// 20.6.6.2.10 get_deleter [util.smartptr.getdeleter]
template<class D, class T> D* get_deleter(shared_ptr<T> const& p);


/// 20.6.6.3 Class template weak_ptr [util.smartptr.weak]
template<class T>
class weak_ptr
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef T element_type;

    ///\name  20.6.6.3.1 weak_ptr constructors [util.smartptr.weak.const]

    weak_ptr();
    template<class Y> weak_ptr(shared_ptr<Y> const& r);
    weak_ptr(weak_ptr const& r);
    template<class Y> weak_ptr(weak_ptr<Y> const& r);

    ///\name  20.6.6.3.2 weak_ptr destructor [util.smartptr.weak.dest]
    ~weak_ptr();

    ///\name  20.6.6.3.3 weak_ptr assignment [util.smartptr.weak.assign]

    weak_ptr& operator=(weak_ptr const& r);
    template<class Y> weak_ptr& operator=(weak_ptr<Y> const& r);
    template<class Y> weak_ptr& operator=(shared_ptr<Y> const& r);

    ///\name  20.6.6.3.4 weak_ptr modifiers [util.smartptr.weak.mod]
    
    void swap(weak_ptr& r);
    void reset();

    ///\name  20.6.6.3.5 weak_ptr observers [util.smartptr.weak.obs]
    
    long use_count() const;
    bool expired() const;
    shared_ptr<T> lock() const;

    ///@}

  ///////////////////////////////////////////////////////////////////////////
  private:

};

///\name  20.6.6.3.6 weak_ptr comparison [util.smartptr.weak.cmp]
template<class T, class U>
bool
  operator<(weak_ptr<T> const& a, weak_ptr<U> const& b);

///\name  20.6.6.3.7 weak_ptr specialized algorithms [util.smartptr.weak.spec]
template<class T>
void
  swap(weak_ptr<T>& a, weak_ptr<T>& b)
{
  a.swap(b);
}

///@}

/// 20.6.6.4 Class template enable_shared_from_this [util.smartptr.enab]
template<class T> class enable_shared_from_this;


///\name  D.9 auto_ptr [depr.auto.ptr]

template<class Y> struct auto_ptr_ref;

/// D.9.1 Class template auto_ptr [auto.ptr]
template<class X>
class auto_ptr
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef X element_type;

    ///\name  D.9.1.1 auto_ptr constructors [auto.ptr.cons]

    explicit auto_ptr(X * p = 0)  throw() : ptr(p) {}

    auto_ptr(auto_ptr & a)        throw() : ptr(a.release()) {}

    template<class Y>
    auto_ptr(auto_ptr<Y> & a)     throw() : ptr(a.release()) {}

    __forceinline
    auto_ptr & operator=(auto_ptr & a) throw()
    {
      reset(a.release());
      return *this;
    }

    template<class Y>
    auto_ptr & operator=(auto_ptr<Y> & a) throw()
    {
      reset(a.release());
      return *this;
    }

    __forceinline
    ~auto_ptr() throw() { if ( get() ) delete get(); }

    ///\name  D.9.1.2 auto_ptr members [auto.ptr.members]

    X & operator* ()  const throw() { return *get(); }
    X * operator->()  const throw() { return get(); }
    X * get()         const throw() { return ptr; }
    X * release()           throw() { X * tmp = get(); set(0); return tmp; }

    __forceinline
    void reset(X * p = 0)   throw()
    { 
      if ( get() && get() != p ) delete get();
      set(p);
    }

    ///\name  D.9.1.3 auto_ptr conversions [auto.ptr.conv]

    auto_ptr(auto_ptr_ref<X> r) throw() : ptr(r.ptr.release()) {}

    __forceinline
    auto_ptr & operator=(auto_ptr_ref<X> r) throw() { return *this = r.ptr; }

    template<class Y>
    operator auto_ptr_ref<Y>()  throw() { return auto_ptr_ref<Y>(*this); }

    template<class Y>
    operator auto_ptr<Y>()      throw() { return auto_ptr<Y>(release()); }

    ///@}

  ///////////////////////////////////////////////////////////////////////////
  private:
  
    X * ptr;
    void set(X * p) { ptr = p; }
};

template<class Y>
struct auto_ptr_ref
{
    auto_ptr_ref(auto_ptr<Y> & a) throw() : ptr(a) {}
    auto_ptr<Y> & ptr;
  private:
    auto_ptr_ref<Y> & operator=(const auto_ptr_ref<Y> &);
};


/**@} lib_memory */
/**@} lib_utilities */

}//namespace std

#endif//#ifndef NTL__STLX_MEMORY
