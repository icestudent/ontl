/**\file*********************************************************************
 *                                                                     \brief
 *  20.5 Function objects [lib.function.objects]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_FUNCTIONAL
#define NTL__STLX_FUNCTIONAL

#include "type_traits.hxx"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4820) // X bytes padding added...
#endif

namespace std {

/**\addtogroup  lib_utilities ********* General utilities library [20] ******
 *@{*/

/**\defgroup  lib_function_objects ***** Function objects [20.5] ************
 *
 *    Function objects are objects with an operator() defined
 *@{
 */

#pragma region lib_base
/**\defgroup  lib_base ***************** Base [20.5.3] **********************
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

#pragma region lib_refwrap
/**\defgroup  lib_refwrap ***************** reference_wrapper [20.5.5] *******
 *
 *    reference_wrapper<T> is a CopyConstructible and Assignable wrapper
 *    around a reference to an object of type T.
 *@{
 */

template <class T>
class reference_wrapper
//: public unary_function<T1, R> // see below
//: public binary_function<T1, T2, R> // see below
{
  ///////////////////////////////////////////////////////////////////////////
  public :

    // types
    typedef T type;
    //typedef -- result_type; // Not always defined

    // construct/copy/destroy
    explicit reference_wrapper(T&) __ntl_nothrow;
    reference_wrapper(const reference_wrapper<T>& x) __ntl_nothrow;
#if defined(NTL__CXX_RV) && defined(NTL__CXX_EF)
    explicit reference_wrapper(T&&) = delete; // do not bind to temporary objects
#endif

    // assignment
    reference_wrapper& operator=(const reference_wrapper<T>& x) __ntl_nothrow;

    // access
    operator T& () const __ntl_nothrow;
    T& get() const __ntl_nothrow;

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

};


template <class T>
inline
reference_wrapper<T> ref(T& t) __ntl_nothrow
{
  return reference_wrapper<T>(t);
}

template <class T> reference_wrapper<const T> cref(const T&) __ntl_nothrow;
template <class T> reference_wrapper<T> ref(reference_wrapper<T>) __ntl_nothrow;
template <class T> reference_wrapper<const T> cref(reference_wrapper<T>) __ntl_nothrow;
/**@} lib_refwrap
 */
#pragma endregion

#pragma region lib_arithmetic_operations
/**\defgroup  lib_arithmetic_operations * Arithmetic operations [20.5.6] ****
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
/**\defgroup  lib_comparisons ********** Comparisons [20.5.7] ***************
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
/**\defgroup  lib_logical_operations *** Logical operations [20.5.8] ********
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
/**\defgroup  lib_bitwise_operations *** Bitwise operations [20.5.9] *********
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
/**\defgroup  lib_negators ************* Negators [20.5.10] ******************
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
/**\defgroup  lib_bind ***************** bind [20.5.11] *****************
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
/**\defgroup  lib_binders ************** Binders [D8] ******************
 *@{
 */

/// D8.1 Class template binder1st [lib.binder.1st]
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

/// D8.2 bind1st [lib.bind.1st]
template <class Operation, class T>
inline
binder1st<Operation>
  bind1st(const Operation& op, const T& x)
{
  return binder1st<Operation>(op, typename Operation::first_argument_type(x));
}

/// D8.3 Class template binder2nd [lib.binder.2nd]
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

/// D8.4 bind2nd [lib.bind.2nd]
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
/**\defgroup  lib_function_pointer_adaptors Adaptors for pointers to functions [20.5.12]
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
/**\defgroup  lib_member_pointer_adaptors Adaptors for pointers to members [20.5.13]
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

/**@} lib_member_pointer_adaptors
 */
#pragma endregion

#pragma region unord.hash
// 20.5.16 Class template hash [unord.hash]
/**\defgroup  lib_hash Class template hash [20.5.16]
 *
 * The unordered associative containers defied in clause 23.4 use specializations of hash as the default
 * hash function.
 *
 *@{
 */
// 20.5.16, hash function base template:
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
  size_t operator()(T val) const;
};

#pragma endregion

/**@} lib_function_objects */

/**@} lib_utilities */

}//namespace std

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif//#ifndef NTL__STLX_FUNCTIONAL
