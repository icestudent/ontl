/**\file*********************************************************************
 *                                                                     \brief
 *  Memory [20.4 lib.memory]
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

/**\addtogroup  lib_utilities ********** General utilities library [20] *****
 *@{*/
/**\defgroup  lib_memory *************** Memory [20.4] **********************
 *@{*/

/// [20.4.1 lib.default.allocator]
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

/// The default allocator [20.4.1 lib.default.allocator]
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

    ///\name  allocator members [20.4.1.1 lib.allocator.members]

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
      ::operator delete(p);//(address(*p));
    }

    size_type max_size() const throw() { return size_t(-1) / sizeof(T); }

    __forceinline
    void construct(pointer p, const T & val)
    { 
      __assume(p);
      new((void *)p) T(val);
    }

    __forceinline
    void destroy(const pointer p)
    {
      p->T::/*< workaround MSVC's weird `scalar deleting destructor'*/ ~T();  
    }

    ///@}

};//class allocator

///\name  allocator comparisons

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

/// Raw storage iterator [20.4.2 lib.storage.iterator]
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

///\name  Temporary buffers [20.4.3 lib.temporary.buffer]

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
  // 20.4.1.1/8  n shall equal the value passed as the first argument
  //             to the invocation of allocate which returned p.
  // but allocator::deallocate() does not use n
  allocator<T>::size_type const n = 0;
  allocator<T>().deallocate(p, n); 
}

///@}

/// uninitialized_copy [20.4.4.1 lib.uninitialized.copy]
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

/// uninitialized_fill [20.4.4.2 lib.uninitialized.fill]
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

/// uninitialized_fill_n [20.4.4.3 lib.uninitialized.fill.n]
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


template<class Y> struct auto_ptr_ref;

/// Class template auto_ptr [20.4.5 lib.auto.ptr]
template<class X>
class auto_ptr
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef X element_type;

    ///\name  construct/copy/destroy [20.4.5.1]

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

    ///\name  members [20.4.5.2]

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

    ///\name  conversions [20.4.5.3]

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

// Additions to header <memory> synopsis [2.2.1 tr.util.smartptr.synopsis]

/// Class bad_weak_ptr [2.2.2]
class bad_weak_ptr;

/// Class template shared_ptr [2.2.3]
template<class T> class shared_ptr;

/// Class template weak_ptr [2.2.4]
template<class T> class weak_ptr;


/// Class template shared_ptr [2.2.3 tr.util.smartptr.shared]
template<class T>
class shared_ptr
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef T element_type;

    ///\name  constructors [2.2.3.1]

    shared_ptr();
    template<class Y> explicit shared_ptr(Y* p);
    template<class Y, class D> shared_ptr(Y* p, D d);
    shared_ptr(shared_ptr const& r);
    template<class Y> shared_ptr(shared_ptr<Y> const& r);
    template<class Y> explicit shared_ptr(weak_ptr<Y> const& r);
    template<class Y> explicit shared_ptr(auto_ptr<Y>& r);

    ///\name  destructor [2.2.3.2]
    ~shared_ptr();

    ///\name  assignment [2.2.3.3]
    shared_ptr& operator=(shared_ptr const& r);
    template<class Y> shared_ptr& operator=(shared_ptr<Y> const& r);
    template<class Y> shared_ptr& operator=(auto_ptr<Y>& r);

    ///\name  modifiers [2.2.3.4]

    void swap(shared_ptr& r);
    void reset();
    template<class Y> void reset(Y* p);
    template<class Y, class D> void reset(Y* p, D d);

    ///\name  observers [2.2.3.5]

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

///\name  shared_ptr comparisons [2.2.3.6]

template<class T, class U>
bool
  operator==(shared_ptr<T> const& a, shared_ptr<U> const& b);

template<class T, class U>
bool
  operator!=(shared_ptr<T> const& a, shared_ptr<U> const& b);

template<class T, class U>
bool
  operator<(shared_ptr<T> const& a, shared_ptr<U> const& b);

///\name  shared_ptr I/O [2.2.3.7]
template<class E, class T, class Y>
basic_ostream<E, T>& 
  operator<< (basic_ostream<E, T>& os, shared_ptr<Y> const& p)
{
  os << p.get();
}

///\name  shared_ptr specialized algorithms [2.2.3.8]
template<class T> 
void
  swap(shared_ptr<T>& a, shared_ptr<T>& b)
{
  a.swap(b);
}

///\name  shared_ptr casts [2.2.3.9]

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

/// shared_ptr get_deleter [2.2.3.10]
template<class D, class T> D* get_deleter(shared_ptr<T> const& p);

/// Class template weak_ptr [2.2.4 tr.util.smartptr.weak]
template<class T>
class weak_ptr
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef T element_type;

    ///\name  constructors/destructor

    weak_ptr();
    template<class Y> weak_ptr(shared_ptr<Y> const& r);
    weak_ptr(weak_ptr const& r);
    template<class Y> weak_ptr(weak_ptr<Y> const& r);
    ~weak_ptr();

    ///\name  assignment

    weak_ptr& operator=(weak_ptr const& r);
    template<class Y> weak_ptr& operator=(weak_ptr<Y> const& r);
    template<class Y> weak_ptr& operator=(shared_ptr<Y> const& r);

    ///\name  modifiers
    
    void swap(weak_ptr& r);
    void reset();

    ///\name  observers
    
    long use_count() const;
    bool expired() const;
    shared_ptr<T> lock() const;

    ///@}

  ///////////////////////////////////////////////////////////////////////////
  private:

};

///\name  weak_ptr comparison
template<class T, class U>
bool
  operator<(weak_ptr<T> const& a, weak_ptr<U> const& b);

///\name  weak_ptr specialized algorithms
template<class T>
void
  swap(weak_ptr<T>& a, weak_ptr<T>& b)
{
  a.swap(b);
}

///@}
/**@} lib_memory */
/**@} lib_utilities */

}//namespace std

#endif//#ifndef NTL__STLX_MEMORY
