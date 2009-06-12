/**\file*********************************************************************
 *                                                                     \brief
 *  20.6 Function objects [function.objects]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_FUNCTIONAL
#define NTL__STLX_FUNCTIONAL

#ifndef NTL__STLX_TYPE_TRAITS
#include "type_traits.hxx"
#endif

#ifndef NTL__STLX_CUCHAR
#include "cuchar.hxx"   // for hash<>
#endif
#ifndef NTL__STLX_ITERATOR
#include "iterator.hxx" // for iterator_traits which used by fnv hash
#endif
#include "typeinfo.hxx" // for hash<type_index>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4820) // X bytes padding added...
#endif

namespace std 
{
/**\addtogroup  lib_utilities ********** 20 General utilities library [utilities]
 *@{*/

/**\addtogroup  lib_function_objects *** 20.6 Function objects [function.objects]
 *  Function objects are objects with an \c operator() defined
 *@{*/

/**\defgroup  lib_func_def ************* 20.6.01 Definitions [func.def]
 *
 *  The following definitions apply to this Clause:
 *  - A <em>call signature</em> is the name of a return type followed by a parenthesized comma-separated %list of zero or
 *  more argument types.
 *  - A <em>callable type</em> is a pointer to function, a pointer to member function, a pointer to member data, or a class
 *  type whose objects can appear immediately to the left of a function call operator.
 *  - A <em>callable object</em> is an object of a callable type.
 *  - A <em>call wrapper type</em> is a type that holds a callable object and supports a call operation that forwards to that
 *  object.
 *  - A <em>call wrapper</em> is an object of a call wrapper type.
 *  - A <em>target object</em> is the callable object held by a call wrapper.
 **/

/**\defgroup  lib_func_require ************* 20.6.02 Requirements [func.require]

  Define <tt>INVOKE(f, t1, t2, ..., tN)</tt> as follows:
  - <tt>(t1.*f)(t2, ..., tN)</tt> when \p f is a pointer to a member function of a class \c T and \p t1 is an object of
 type \c T or a reference to an object of type \c T or a reference to an object of a type derived from \c T;
  - <tt>((*t1).*f)(t2, ..., tN)</tt> when \p f is a pointer to a member function of a class \c T and \p t1 is not one of
 the types described in the previous item;
  - <tt>t1.*f</tt> when \p f is a pointer to member data of a class \c T and \p t1 is an object of type \c T or a reference to
 an object of type \c T or a reference to an object of a type derived from \c T;
  - <tt>(*t1).*f</tt> when \p f is a pointer to member data of a class \c T and \p t1 is not one of the types described in
 the previous item;
  - <tt>f(t1, t2, ..., tN)</tt> in all other cases.
 
 Define <tt>INVOKE(f, t1, t2, ..., tN, R)</tt> as <tt>INVOKE(f, t1, t2, ..., tN)</tt> implicitly converted to \c R.
 
 If a call wrapper (20.6.1) has a <em>weak result type</em> the type of its member type \c result_type is based on the
 type \c T of the wrapper's target object (20.6.1):
  - if \c T is a function, reference to function, or pointer to function type, \c result_type shall be a synonym
 for the return type of \c T;
  - if \c T is a pointer to member function, \c result_type shall be a synonym for the return type of \c T;
  - if \c T is a class type with a member type \c result_type, then result_type shall be a synonym for
 <tt>T::result_type</tt>;
  - otherwise \c result_type shall not be defined.
 
 Every call wrapper (20.6.1) shall be CopyConstructible. A <em>simple call wrapper</em> is a call wrapper that is
 CopyAssignable and whose copy constructor and assignment operator do not throw exceptions. A <em>forwarding
 call wrapper</em> is a call wrapper that can be called with an argument %list.

 @note in a typical implementation forwarding call wrappers have an overloaded function call operator of the form
 \code
 template<class... ArgTypes>
 R operator()(ArgTypes&&... args) cv-qual;
 \endcode
 **/

#pragma region lib_base
/**\defgroup  lib_base ***************** 20.6.03 Base [base]
 *  provided to simplify the typedefs of the argument and result types
 *@{*/

template <class Arg, class Result>
struct unary_function
{
  typedef Arg     argument_type;
  typedef Result  result_type;
};

template <class Arg1, class Arg2, class Result>
struct binary_function
{
  typedef Arg1    first_argument_type;
  typedef Arg2    second_argument_type;
  typedef Result  result_type;
};

/**@} lib_base */
#pragma endregion

#pragma region lib_arithmetic_operations

/**\defgroup  lib_arithmetic_operations *** 20.6.06 Arithmetic operations [arithmetic.operations]
 *    functors for all of the arithmetic operators
 *@{*/

template <class T>
struct plus : binary_function<T, T, T>
{
  T operator()(const T& x, const T& y) const { return x + y; }
};

template <class T>
struct minus : binary_function<T, T, T>
{
  T operator()(const T& x, const T& y) const { return x - y; }
};

template <class T>
struct multiplies : binary_function<T, T, T>
{
  T operator()(const T& x, const T& y) const { return x * y; }
};

template <class T>
struct divides : binary_function<T, T, T>
{
  T operator()(const T& x, const T& y) const { return x / y; }
};

template <class T>
struct modulus : binary_function<T, T, T>
{
  T operator()(const T& x, const T& y) const { return x % y; }
};

template <class T>
struct negate : unary_function<T, T>
{
  T operator()(const T& x) const { return - x; }
};

/**@} lib_arithmetic_operations */
#pragma endregion

#pragma region lib_comparisons
/**\defgroup  lib_comparisons ********** 20.6.07 Comparisons [comparisons]
 *   functors for all of the comparison operators
 *@{*/

template <class T>
struct equal_to : binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x == y; }
};

template <class T>
struct not_equal_to : binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x != y; }
};

template <class T>
struct greater : binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x > y; }
};

template <class T>
struct less : binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x < y; }
};

template <class T>
struct greater_equal : binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x >= y; }
};

template <class T>
struct less_equal : binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x <= y; }
};

/**@} lib_comparisons */
#pragma endregion

#pragma region lib_logical_operations
/**\defgroup  lib_logical_operations *** 20.6.08 Logical operations [logical.operations]
 *
 *   functors for all of the logical operators
 *@{*/

template <class T>
struct logical_and : binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x && y; }
};

template <class T>
struct logical_or : binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x || y; }
};

template <class T>
struct logical_not : unary_function<T, bool>
{
  bool operator()(const T& x) const { return ! x; }
};

/**@} lib_logical_operations */
#pragma endregion

#pragma region lib_bitwise_operations
/**\defgroup  lib_bitwise_operations *** 20.6.09 Bitwise operations [bitwise.operations]
 *
 *   functors for all of the bitwise operators in the language
 *@{**/

template <class T> struct bit_and : binary_function<T,T,T>
{
  T operator()(const T& x, const T& y) const
  {
    return x & y;
  }
};

template <class T> struct bit_or : binary_function<T,T,T>
{
  T operator()(const T& x, const T& y) const
  {
    return x | y;
  }
};

template <class T> struct bit_xor : binary_function<T,T,T>
{
  T operator()(const T& x, const T& y) const
  {
    return x ^ y;
  }
};
/**@} lib_bitwise_operations */
#pragma endregion

#pragma region lib_negators
/**\defgroup  lib_negators ************* 20.6.10 Negators [negators]
 *
 *   negators take a predicate and return its complement
 *@{*/

template <class Predicate>
class unary_negate
: public unary_function<typename Predicate::argument_type, bool>
{
    const Predicate& pred;
    void operator=(const unary_negate &);
  public:
    explicit unary_negate(const Predicate& pred) : pred(pred) {}

    bool operator()(const typename Predicate::argument_type& x) const
    {
      return ! pred(x);
    }
};

template <class Predicate>
inline
unary_negate<Predicate> not1(const Predicate& pred)
{
  return unary_negate<Predicate>(pred);
}

template <class Predicate>
class binary_negate
: public binary_function <typename Predicate::first_argument_type,
                          typename Predicate::second_argument_type,
                          bool>
{
    const Predicate& pred;
    void operator=(const binary_negate &);
  public:
    explicit binary_negate(const Predicate& pred) : pred(pred) {}

    bool operator()(const typename Predicate::first_argument_type&  x,
                    const typename Predicate::second_argument_type& y) const
    {
      return ! pred(x, y);
    }
};

template <class Predicate>
inline
binary_negate<Predicate> not2(const Predicate& pred)
{
  return binary_negate<Predicate>(pred);
}

/**@} lib_negators */
#pragma endregion

#pragma region lib_binders
/**\ingroup std_depr
 *@{*/

/**\defgroup  lib_binders ************** D.8 Binders [depr.lib.binders]
 *@{*/

/// D8.1 Class template binder1st [depr.lib.binder.1st]
template <class Operation>
class binder1st
: public unary_function<typename Operation::second_argument_type,
                        typename Operation::result_type>
{
  protected:
    Operation op;
    typename Operation::first_argument_type value;

  public:
    binder1st(const Operation& x,
              const typename Operation::first_argument_type& y)
    : op(x), value(y) {}

    ///\note extended with remove_reference
    typename Operation::result_type
      operator()(const typename remove_reference<typename Operation::second_argument_type>::type& x) const
    {
      return op(value, x);
    }
};

/// D8.2 bind1st [depr.lib.bind.1st]
template <class Operation, class T>
inline
binder1st<Operation>
  bind1st(const Operation& op, const T& x)
{
  return binder1st<Operation>(op, typename Operation::first_argument_type(x));
}

/// D8.3 Class template binder2nd [depr.lib.binder.2nd]
template <class Operation>
class binder2nd
: public unary_function<typename Operation::first_argument_type,
                        typename Operation::result_type>
{
  protected:
    Operation op;
    typename Operation::second_argument_type value;

  public:
    binder2nd(const Operation& x,
              const typename Operation::second_argument_type& y)
    : op(x), value(y) {}

    typename Operation::result_type
      operator()(const typename Operation::first_argument_type& x) const
    {
      return op(x, value);
    }

    ///\note extension
    typename Operation::result_type
      operator()(typename Operation::first_argument_type& x) const
    {
      return op(x, value);
    }
};

/// D8.4 bind2nd [depr.lib.bind.2nd]
template <class Operation, class T>
inline
binder2nd<Operation>
  bind2nd(const Operation& op, const T& x)
{
  return binder2nd<Operation>(op, typename Operation::second_argument_type(x));
}

/**@} lib_binders */
/**@} std_depr */
#pragma endregion

#pragma region lib_adaptors
/**\defgroup  lib_function_pointer_adaptors 20.6.12 Adaptors for pointers to functions [function.pointer.adaptors]
 *  Allow pointers to (unary and binary) functions to work with function adaptors
 *@{*/

template <class Arg, class Result>
class pointer_to_unary_function : public unary_function<Arg, Result>
{
    Result (*f)(Arg);
  public:
    explicit pointer_to_unary_function(Result (*f)(Arg)) : f(f) {}
    Result operator()(Arg x) const { return f(x); }
};

template <class Arg, class Result>
inline pointer_to_unary_function<Arg,Result>
  ptr_fun(Result (*f)(Arg))
{
  return pointer_to_unary_function<Arg, Result>(f);
}

template <class Arg1, class Arg2, class Result>
class pointer_to_binary_function : public binary_function<Arg1, Arg2, Result>
{
    Result (*f)(Arg1, Arg2);
  public:
    explicit pointer_to_binary_function(Result (*f)(Arg1, Arg2)) : f(f) {}
    Result operator()(Arg1 x, Arg2 y) const { return f(x, y); }
};

template <class Arg1, class Arg2, class Result>
inline
pointer_to_binary_function<Arg1, Arg2, Result>
  ptr_fun(Result (*f)(Arg1,Arg2))
{
  return pointer_to_binary_function<Arg1,Arg2,Result>(f);
}

/**@} lib_function_pointer_adaptors */
#pragma endregion

#pragma region lib_member_pointer_adaptors
/**\defgroup  lib_member_pointer_adaptors 20.6.13 Adaptors for pointers to members [member.pointer.adaptors]
 *  The purpose of the following is to provide the same facilities for pointer to members as those provided for pointers to functions in 20.6.12.
 *@{*/

template <class Result, class T>
class mem_fun_t : public unary_function<T*, Result>
{
    Result (T::*m)();
  public:
    explicit mem_fun_t(Result (T::*p)()) : m(p) {}
    Result operator()(T* p) const { return (p->*m)(); }
};

template <class Result, class T>
class const_mem_fun_t : public unary_function<T*, Result>
{
    Result (T::*m)() const;
  public:
    explicit const_mem_fun_t(Result (T::*p)() const) : m(p) {}
    Result operator()(const T* p) const { return (p->*m)(); }
};

template <class Result, class T, class A>
class mem_fun1_t : public binary_function<T*, A, Result>
{
    Result (T::*m)(A);
  public:
    explicit mem_fun1_t(Result (T::*p)(A)) : m(p) {}
    Result operator()(T* p, A x) const { return (p->*m)(x); }
};

template <class Result, class T, class A>
class const_mem_fun1_t : public binary_function<T*, A, Result>
{
    Result (T::*m)(A) const;
  public:
    explicit const_mem_fun1_t(Result (T::*p)(A) const) : m(p) {}
    Result operator()(const T* p, A x) const { return (p->*m)(x); }
};

template<class Result, class T>
inline
mem_fun_t<Result, T>
  mem_fun(Result (T::*f)())
{
  return mem_fun_t<Result, T>(f);
}

template <class Result, class T>
inline
const_mem_fun_t<Result, T>
  mem_fun(Result (T::*f)() const)
{
  return const_mem_fun_t<Result, T>(f);
}

template<class Result, class T, class A>
inline
mem_fun1_t<Result, T, A>
  mem_fun(Result (T::*f)(A))
{
  return mem_fun1_t<Result, T, A>(f);
}

template <class Result, class T, class A>
inline const_mem_fun1_t<Result, T, A>
  mem_fun(Result (T::*f)(A) const)
{
  return const_mem_fun1_t<Result, T, A>(f);
}

template <class Result, class T>
class mem_fun_ref_t : public unary_function<T, Result>
{
    Result (T::*m)();
  public:
    explicit mem_fun_ref_t(Result (T::*p)()) : m(p) {}
    Result operator()(T& p) const { return (p.*m)(); }
};

template <class Result, class T>
class const_mem_fun_ref_t : public unary_function<T, Result>
{
    Result (T::*m)() const;
  public:
    explicit const_mem_fun_ref_t(Result (T::*p)() const) : m(p) {}
    Result operator()(const T& p) const { return (p.*m)(); }
};

template <class Result, class T, class A>
class mem_fun1_ref_t : public binary_function<T, A, Result>
{
    Result (T::*m)(A);
  public:
    explicit mem_fun1_ref_t(Result (T::*p)(A)) : m(p) {}
    Result operator()(T& p, A x) const { return (p.*m)(x); }
};

template <class Result, class T, class A>
class const_mem_fun1_ref_t : public binary_function<T, A, Result>
{
    Result (T::*m)(A) const;
  public:
    explicit const_mem_fun1_ref_t(Result (T::*p)(A) const) : m(p) {}
    Result operator()(const T& p, A x) const { return (p.*m)(x); }
};

template<class Result, class T>
inline
mem_fun_ref_t<Result, T>
  mem_fun_ref(Result (T::*f)())
{
  return mem_fun_ref_t<Result, T>(f);
}

template <class Result, class T>
inline
const_mem_fun_ref_t<Result, T>
  mem_fun_ref(Result (T::*f)() const)
{
  return const_mem_fun_ref_t<Result, T>(f);
}

template<class Result, class T, class A>
inline
mem_fun1_ref_t<Result, T, A>
  mem_fun_ref(Result (T::*f)(A))
{
  return mem_fun1_ref_t<Result, T, A>(f);
}

template <class Result, class T, class A>
inline
const_mem_fun1_ref_t<Result, T, A>
  mem_fun_ref(Result (T::*f)(A) const)
{
  return const_mem_fun1_ref_t<Result, T, A>(f);
}

/**@} lib_member_pointer_adaptors */
#pragma endregion


#pragma region unord.hash
/**\addtogroup  lib_hash 20.6.16 Class template hash [unord.hash]
 *
 * The unordered associative containers defied in clause 23.4 use specializations of hash as the default %hash function.
 *@{*/
// 20.6.16, hash function base template:
template <class T> struct hash;

// Hash function specializations
template <> struct hash<bool>;
template <> struct hash<char>;
template <> struct hash<signed char>;
template <> struct hash<unsigned char>;
template <> struct hash<char16_t>;
template <> struct hash<char32_t>;
template <> struct hash<wchar_t>;
template <> struct hash<short>;
template <> struct hash<unsigned short>;
template <> struct hash<int>;
template <> struct hash<unsigned int>;
template <> struct hash<long>;
template <> struct hash<long long>;
template <> struct hash<unsigned long>;
template <> struct hash<unsigned long long>;
template <> struct hash<float>;
template <> struct hash<double>;
template <> struct hash<long double>;
template<class T> struct hash<T*>;

//template <> struct hash<std::string>; // in <string>
//template <> struct hash<std::u16string>;
//template <> struct hash<std::u32string>;
//template <> struct hash<std::wstring>;

//template <> struct hash<std::error_code>; // in <system_error>
//template <> struct hash<std::thread::id>; // in <thread>

/// generic hash function delaration
template<class T>
struct hash: unary_function<T, size_t>
{
  size_t operator()(T val) const __ntl_nothrow;
};

/// hash function implementation for pointers
template<class T>
struct hash<T*>: unary_function<T*, size_t>
{
  size_t operator()(argument_type val) const __ntl_nothrow
  {
    return reinterpret_cast<size_t>(val);
  }
};


/// integer types hash function implementation
#define NTL_HASH_IMPL(T) \
template<> struct hash<T>: unary_function<T, size_t> \
{ \
  inline size_t operator()(argument_type val) const __ntl_nothrow \
  { \
    return static_cast<size_t>(val); \
  } \
}

NTL_HASH_IMPL(bool);
NTL_HASH_IMPL(char);
NTL_HASH_IMPL(signed char);
NTL_HASH_IMPL(unsigned char);
NTL_HASH_IMPL(wchar_t);
NTL_HASH_IMPL(short);
NTL_HASH_IMPL(unsigned short);
NTL_HASH_IMPL(int);
NTL_HASH_IMPL(unsigned int);
NTL_HASH_IMPL(long);
NTL_HASH_IMPL(unsigned long);

// NOTE: if sizeof(int64_t) > sizeof(size_t) we must use some hashing algorithm (e.g. FNV) to calculate hash value of the 64-bit data, 
// but it can produce collision between hash algorithm result and "hash value" of the fundamental data.
#if __SIZEOF_POINTER__ == 8
NTL_HASH_IMPL(long long);
NTL_HASH_IMPL(unsigned long long);
#endif

#ifdef NTL__CXX_CHARS
NTL_HASH_IMPL(char16_t);
NTL_HASH_IMPL(char32_t);
#endif
#undef NTL_HASH_IMPL

/**
 *	@brief 18.7.2.3 Template specialization hash&lt;type_index&gt; [type.index.templ] (N2857)
 *  @ingroup lib_support_rtti
 **/
template<>
struct hash<type_index>:
  unary_function<type_index, size_t>
{
  typedef size_t result_type;
  typedef type_index argument_type;

  size_t operator()(const type_index& index) const
  {
    return index.hash_code();
  }
};


namespace __
{
  /**
   *	@brief FNV Hash Function
   *
   *  Fowler/Noll/Vo is a non-cryptographic hash function created by Glenn Fowler, Landon Curt Noll, and Phong Vo.
   *
   *  @see http://en.wikipedia.org/wiki/Fowler_Noll_Vo_hash
   **/
  template<typename hash_t, hash_t prime, hash_t seed = 0>
  struct FNVHashT
  {
    static const hash_t prime_value = prime;
    static const hash_t seed_value  = seed;

    /** Data sequence hashing (octets) */
    template<typename Iterator>
    inline typename enable_if<sizeof(typename iterator_traits<Iterator>::value_type) == sizeof(uint8_t), hash_t>::type operator()(Iterator first, Iterator last) const
    {
      hash_t hash = seed;
      while(first != last){
        hash = hash * prime ^ static_cast<hash_t>(*first);
        ++first;
      }
      return hash;
    }

    /** Data sequence hashing (large values) */
    template<typename Iterator>
    inline typename enable_if<(sizeof(typename iterator_traits<Iterator>::value_type) > sizeof(uint8_t)), hash_t>::type operator()(Iterator first, Iterator last) const
    {
      hash_t hash = seed;
      while(first != last){
        size_t size = sizeof(*first);
        for(const uint8_t* p = reinterpret_cast<const uint8_t*>(&*first); size--; p++)
          hash = hash * prime ^ *p;
        ++first;
      }
      return hash;
    }

    /** Data buffer hashing (by octets) */
    inline hash_t operator()(const void* data, size_t size) const
    {
      hash_t hash = seed;
      for(const uint8_t* p = reinterpret_cast<const uint8_t*>(data); size--; p++)
        hash = hash * prime ^ *p;
      return hash;
    }

    static inline hash_t hash_op(const void* data, size_t size, hash_t hash = seed)
    {
      for(const uint8_t* p = reinterpret_cast<const uint8_t*>(data); size--; p++)
        hash = hash * prime ^ *p;
      return hash;
    }

    static inline hash_t hash_combine(hash_t hash1, hash_t hash2 = seed)
    {
      return  hash2 * prime ^ hash1;
    }
  };

#ifndef _M_X64
  /// FNV Hash implementation for 32-bit code.
  typedef FNVHashT<size_t, 0x01000193, 2166136261UL> FNVHash;
#else
  /// FNV Hash implementation for 64-bit code.
  typedef FNVHashT<size_t, 0x100000001B3, 14695981039346656037ULL> FNVHash;
#endif
}

// TODO: floating point hash function implementation

/**@} lib_hash */
#pragma endregion

/**@} lib_function_objects */
/**@} lib_utilities */
}//namespace std

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif//#ifndef NTL__STLX_FUNCTIONAL
