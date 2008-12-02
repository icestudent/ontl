/**\file*********************************************************************
 *                                                                     \brief
 *  20.6 Function objects [function.objects]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_FUNCTIONAL
#define NTL__STLX_FUNCTIONAL

#include "type_traits.hxx"
#include "result_of.hxx"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4820) // X bytes padding added...
#endif

namespace std {

/**\addtogroup  lib_utilities ********* 20 General utilities library [utilities] ******
 *@{*/

/**\defgroup  lib_function_objects ***** 20.6 Function objects [function.objects] ************
 *
 *    Function objects are objects with an operator() defined
 *@{
 */

#pragma region lib_base
/**\defgroup  lib_base ***************** 20.6.3 Base [base] **********************
 *
 *    provided to simplify the typedefs of the argument and result types
 *@{
 */

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

/**@} lib_base
 */
#pragma endregion

#pragma region lib_ret

template <class> class result_of; // undefined

#pragma endregion

#pragma region lib_refwrap
/**\defgroup  lib_refwrap ***************** 20.6.5 Class template reference_wrapper [refwrap] *******
 *
 *    reference_wrapper<T> is a CopyConstructible and Assignable wrapper
 *    around a reference to an object of type T.
 *@{
 */

namespace __
{
  struct empty_base
  {};

  template<typename T>
  struct type2type
  {};

  namespace refwrap_from
  {
    enum type {
      none,
      unary,
      binary,
      other
    };
  }

  template<typename T>
  class rfw_derives_from
  {
    typedef char yes;
    typedef struct { char _[2]; } no;

    template<typename T1, typename R>
    static yes probe_unary(const volatile unary_function<T1,R>*);
    static no  probe_unary(...);

    template<typename T1, typename T2, typename R>
    static yes probe_binary(const volatile binary_function<T1,T2,R>*);
    static no  probe_binary(...);

    template<typename U>
    static yes probe_result(type2type<typename U::result_type>*);
    //template<typename U>
    static no  probe_result(...);
  public:
    static const bool  unary = sizeof( probe_unary ((T*)0) ) == sizeof(yes);
    static const bool binary = sizeof( probe_binary((T*)0) ) == sizeof(yes);
    static const bool has_result_type = sizeof( probe_result((T*)0) ) == sizeof(yes);

    static const refwrap_from::type value = 
      unary ? refwrap_from::unary :
      binary ? refwrap_from::binary :
      has_result_type ? refwrap_from::other :
      refwrap_from::none;
  };

  template<class T, refwrap_from::type derives_from>
  struct refwrap_derived;

  template<class T>
  struct refwrap_derived<T, refwrap_from::none>: empty_base
  {};

  template<class T>
  struct refwrap_derived<T, refwrap_from::unary>: 
    unary_function<typename T::argument_type, typename T::result_type>
  {};

  template<class T>
  struct refwrap_derived<T, refwrap_from::binary>: 
    binary_function<typename T::first_argument_type, typename T::second_argument_type, typename T::result_type>
  {};

  template<class T>
  struct refwrap_derived<T, refwrap_from::other>
  {
    typedef typename T::result_type result_type;
  };


  template<class T>
  struct refwrap_base:
    refwrap_derived<T, rfw_derives_from<T>::value>
  {};

  // functions
  template<typename R, typename T1>
  struct refwrap_base<R(T1)>:
    unary_function<T1,R>
  {};

  template<typename R, typename T1, typename T2>
  struct refwrap_base<R(T1,T2)>:
    binary_function<T1,T2,R>
  {};

  // function pointers
  template<typename R, typename T1>
  struct refwrap_base<R(*)(T1)>:
    unary_function<T1,R>
  {};

  template<typename R, typename T1, typename T2>
  struct refwrap_base<R(*)(T1,T2)>:
    binary_function<T1,T2,R>
  {};

  // member functions
#define NTL_RF_MAKEBASE(_cv) \
  template<typename R, class T> \
  struct refwrap_base<R(T::*)() _cv>: \
    unary_function<_cv T*,R> \
  {}; \
  template<typename R, class T, typename T1> \
  struct refwrap_base<R(T::*)(T1) _cv>: \
    binary_function<_cv T*,T1,R> \
  {};

#define _NEST(x) x
#define _EMPTY
  NTL_RF_MAKEBASE(_NEST(_EMPTY));
  NTL_RF_MAKEBASE(const);
  NTL_RF_MAKEBASE(volatile);
  NTL_RF_MAKEBASE(const volatile);
#undef _EMPTY
#undef _NEST
#undef NTL_RF_MAKEBASE
}

/**
 *	20.6.5 Class template reference_wrapper [refwrap]
 *  reference_wrapper<T> is a CopyConstructible and Assignable wrapper around a reference to an object of type T.
 *
 *  @note implements 20.6.5/3, 20.6.5/4 (N2723); has \c result_type if type \c T has it.
 *  @todo invokation (20.6.5.4 [refwrap.invoke])
 **/
template <class T>
class reference_wrapper:
  public __::refwrap_base<typename remove_cv<T>::type>
{
  public:
    // types
    typedef T type;

    // construct/copy/destroy
    explicit reference_wrapper(T& t) __ntl_nothrow
      : ptr(&t)
    {}

    reference_wrapper(const reference_wrapper<T>& x) __ntl_nothrow
      : ptr(&x.get())
    {}

    // assignment
    reference_wrapper& operator=(const reference_wrapper<T>& x) __ntl_nothrow
    {
      ptr = x.get();
      return *this;
    }

    // access
    operator T& () const __ntl_nothrow
    {
      return this->get();
    }
    T& get() const __ntl_nothrow
    {
      return *ptr;
    }

#if NTL__CXX_VT
    // invocation
    template <class T1, class T2, ..., class TN>
    typename result_of<T(T1, T2, ..., TN)>::type
      operator() (T1&, T2&, ..., TN&) const
    {
    // Returns: INVOKE (get(), a1, a2, ..., aN). ([3.3])
      return get()(t1);
    }
#endif

  ///////////////////////////////////////////////////////////////////////////
  private:
    T* ptr;

    // do not bind to temporary objects
    #if defined(NTL__CXX_EF)
    explicit reference_wrapper(T&&) = delete; 
    #elif defined(NTL__CXX_RV)
    explicit reference_wrapper(T&&);
    #endif
};


template <class T>
inline
reference_wrapper<T> ref(T& t) __ntl_nothrow
{
  return reference_wrapper<T>(t);
}

template <class T>
reference_wrapper<const T> cref(const T& t) __ntl_nothrow
{
  return reference_wrapper<const T>(t);
}

template <class T>
reference_wrapper<T> ref(reference_wrapper<T> x) __ntl_nothrow
{
  return x;
}

template <class T>
reference_wrapper<const T> cref(reference_wrapper<const T> x) __ntl_nothrow
{
  return x;
}

template <class T>
reference_wrapper<const T> cref(reference_wrapper<T> x) __ntl_nothrow
{
  return reference_wrapper<const T>(x.get());
}

/**@} lib_refwrap
 */
#pragma endregion

#pragma region lib_arithmetic_operations

/**\defgroup  lib_arithmetic_operations * 20.6.6 Arithmetic operations [arithmetic.operations] ****
 *
 *    functors for all of the arithmetic operators
 *@{
 */

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

/**@} lib_arithmetic_operations
 */
#pragma endregion

#pragma region lib_comparsions
/**\defgroup  lib_comparisons ********** 20.6.7 Comparisons [comparisons] ***************
 *
 *   functors for all of the comparison operators
 *@{
 */

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

/**@} lib_comparisons
 */
#pragma endregion

#pragma region lib_logical_operations
/**\defgroup  lib_logical_operations *** 20.6.8 Logical operations [logical.operations] ********
 *
 *   functors for all of the logical operators
 *@{
 */

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

/**@} lib_logical_operations
 */
#pragma endregion

#pragma region lib_bitwise_operations
/**\defgroup  lib_bitwise_operations *** 20.6.9 Bitwise operations [bitwise.operations] *********
 *
 *   functors for all of the bitwise operators in the language
 *@{
 **/

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
#pragma endregion

#pragma region lib_negators
/**\defgroup  lib_negators ************* 20.6.10 Negators [negators] ******************
 *
 *   negators take a predicate and return its complement
 *@{
 */

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

/**@} lib_negators
 */
#pragma endregion

#pragma region lib_bind
/**\defgroup  lib_bind ***************** 20.6.11 Function template bind [bind] *****************
 *
 *   The template function bind returns an object that binds a function object passed as an argument to additional arguments.
 *@{
 */
#ifdef NTL__CXX_VT

template<class T> struct is_bind_expression;
template<class T> struct is_placeholder;

template<class Fn, class... Types>
unspecified bind(Fn, Types...);
template<class R, class Fn, class... Types>
unspecified bind(Fn, Types...);

namespace placeholders {
  // M is the implementation-defined number of placeholders
  extern unspecified _1;
  extern unspecified _2;
  extern unspecified _M;
}

#endif // NTL__CXX_VT

/**@} lib_bind
  **/
#pragma endregion

#pragma region lib_binders
/**\defgroup  lib_binders ************** D.8 Binders [depr.lib.binders] ******************
 *@{
 */

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

/**@} lib_binders
 */
#pragma endregion

#pragma region lib_adaptors
/**\defgroup  lib_function_pointer_adaptors 20.6.12 Adaptors for pointers to functions [function.pointer.adaptors]
 *@{
 */

template <class Arg, class Result>
class pointer_to_unary_function : public unary_function<Arg, Result>
{
    Result (*f)(Arg);
  public:
    explicit pointer_to_unary_function(Result (*f)(Arg)) : f(f) {}
    Result operator()(Arg x) const { return f(x); }
};

template <class Arg, class Result>
pointer_to_unary_function<Arg,Result>
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

/**@} lib_function_pointer_adaptors
 */
#pragma endregion

#pragma region lib_member_pointer_adaptors
/**\defgroup  lib_member_pointer_adaptors 20.6.13 Adaptors for pointers to members [member.pointer.adaptors]
 *@{
 */

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
const_mem_fun1_t<Result, T, A>
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

//template<class R, class T>
//unspecified mem_fn(R T::* pm);

/**@} lib_member_pointer_adaptors
 */
#pragma endregion


#pragma region unord.hash
// 20.6.16 Class template hash [unord.hash]
/**\defgroup  lib_hash 20.6.16 Class template hash [unord.hash]
 *
 * The unordered associative containers defied in clause 23.4 use specializations of hash as the default
 * hash function.
 *
 *@{
 */
// 20.6.16, hash function base template:
template <class T> struct hash;

// Hash function specializations
template <> struct hash<bool>;
template <> struct hash<char>;
template <> struct hash<signed char>;
template <> struct hash<unsigned char>;
//template <> struct hash<char16_t>;
//template <> struct hash<char32_t>;
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
//template <> struct hash<std::string>;
//template <> struct hash<std::u16string>;
//template <> struct hash<std::u32string>;
//template <> struct hash<std::wstring>;
//template <> struct hash<std::error_code>;
//template <> struct hash<std::thread::id>;

template<class T>
struct hash: unary_function<T, size_t>
{
  size_t operator()(T val) const __ntl_nothrow;
};

#pragma endregion

#pragma region func.reference_closure

template<class> class reference_closure;

#ifdef NTL__CXX_VT

template<class R, class... ArgTypes >
class reference_closure<R (ArgTypes...)>
{
public:
  typedef R result_type;
  typedef T1 argument_type; // if sizeof...(ArgTypes) == 1 and ArgTypes contains T1
  typedef T1 first_argument_type; // if sizeof...(ArgTypes) == 2 and ArgTypes contains T1, T2
  typedef T2 second_argument_type; // if sizeof...(ArgTypes) == 2 and ArgTypes contains T1, T2

  // 20.6.17.1, construct/copy/destroy:
  reference_closure() = default;
  reference_closure(const reference_closure&) = default;
  constexpr reference_closure(nullptr_t);
  reference_closure& operator=(const reference_closure&) = delete;
  reference_closure& operator=(nullptr_t);
  ~reference_closure() = default;
  // 20.6.17.2, observer:
  explicit operator bool() const;
  // 20.6.17.3, invocation:
  R operator()(ArgTypes...) const;
};

// 20.6.17.4, comparisons:
template <class R, class... ArgTypes>
bool operator==(const reference_closure<R (ArgTypes...)>&, nullptr_t);

template <class R, class... ArgTypes>
bool operator==(nullptr_t, const reference_closure<R (ArgTypes...)>&);

template <class R, class... ArgTypes>
bool operator!=(const reference_closure<R(ArgTypes...)>&, nullptr_t);

template <class R, class... ArgTypes>
bool operator!=(nullptr_t, const reference_closure<R (ArgTypes...)>&);

#endif
#pragma endregion

/**@} lib_function_objects */

/**@} lib_utilities */

}//namespace std

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif//#ifndef NTL__STLX_FUNCTIONAL
