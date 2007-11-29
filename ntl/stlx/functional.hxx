/**\file*********************************************************************
 *                                                                     \brief
 *  20.3 Function objects [lib.function.objects]
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

/**\defgroup  lib_function_objects ***** Function objects [20.3] ************
 *
 *    Function objects are objects with an operator() defined
 *@{
 */

/**\defgroup  lib_base ***************** Base [20.3.1] **********************
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

/**\defgroup  lib_arithmetic_operations * Arithmetic operations [20.3.2] ****
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

/**\defgroup  lib_comparisons ********** Comparisons [20.3.3] ***************
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

/**\defgroup  lib_logical_operations *** Logical operations [20.3.4] ********
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

/**\defgroup  lib_negators ************* Negators [20.3.5] ******************
 *
 *   negators take a predicate and return its complement
 *@{
 */

template <class Predicate>
class unary_negate
: public unary_function<typename Predicate::argument_type, bool>
{
    const Predicate& pred;
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

/**\defgroup  lib_binders ************** Binders [20.3.6] *******************
 *@{
 */

/// 20.3.6.1 Class template binder1st [lib.binder.1st]
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

/// 20.3.6.2 bind1st [lib.bind.1st]
template <class Operation, class T>
inline
binder1st<Operation>
  bind1st(const Operation& op, const T& x)
{
  return binder1st<Operation>(op, typename Operation::first_argument_type(x));
}

/// 20.3.6.3 Class template binder2nd [lib.binder.2nd]
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

/// 20.3.6.4 bind2nd [lib.bind.2nd]
template <class Operation, class T>
inline
binder2nd<Operation>
  bind2nd(const Operation& op, const T& x)
{
  return binder2nd<Operation>(op, typename Operation::second_argument_type(x));
}

/**@} lib_binders
 */

/**\defgroup  lib_function_pointer_adaptors Adaptors for pointers to functions [20.3.7]
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
pointer_to_unary_function<Arg,Result> ptr_fun(Result (*f)(Arg))
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

/**\defgroup  lib_member_pointer_adaptors Adaptors for pointers to members [20.3.8]
 *@{
 */

template <class S, class T>
class mem_fun_t : public unary_function<T*, S>
{
    S (T::*m)();
  public:
    explicit mem_fun_t(S (T::*p)()) : m(p) {}
    S operator()(T* p) const { return (p->*m)(); }
};

template <class S, class T>
class const_mem_fun_t : public unary_function<T*, S>
{
    S (T::*m)() const;
  public:
    explicit const_mem_fun_t(S (T::*p)() const) : m(p) {}
    S operator()(const T* p) const { return (p->*m)(); }
};

template <class S, class T, class A>
class mem_fun1_t : public binary_function<T*, A, S>
{
    S (T::*m)(A);
  public:
    explicit mem_fun1_t(S (T::*p)(A)) : m(p) {}
    S operator()(T* p, A x) const { return (p->*m)(x); }
};

template <class S, class T, class A>
class const_mem_fun1_t : public binary_function<T*, A, S>
{
    S (T::*m)(A) const;
  public:
    explicit const_mem_fun1_t(S (T::*p)(A) const) : m(p) {}
    S operator()(const T* p, A x) const { return (p->*m)(x); }
};

template<class S, class T>
inline
mem_fun_t<S, T>
  mem_fun(S (T::*f)())
{
  return mem_fun_t<S, T>(f);
}

template <class S, class T>
inline
const_mem_fun_t<S, T>
  mem_fun(S (T::*f)() const)
{
  return const_mem_fun_t<S, T>(f);
}

template<class S, class T, class A>
inline
mem_fun1_t<S, T, A>
  mem_fun(S (T::*f)(A))
{
  return mem_fun1_t<S, T, A>(f);
}

template <class S, class T, class A>
const_mem_fun1_t<S, T, A>
  mem_fun(S (T::*f)(A) const)
{
  return const_mem_fun1_t<S, T, A>(f);
}

template <class S, class T>
class mem_fun_ref_t : public unary_function<T, S>
{
    S (T::*m)();
  public:
    explicit mem_fun_ref_t(S (T::*p)()) : m(p) {}
    S operator()(T& p) const { return (p.*m)(); }
};

template <class S, class T>
class const_mem_fun_ref_t : public unary_function<T, S>
{
    S (T::*m)() const;
  public:
    explicit const_mem_fun_ref_t(S (T::*p)() const) : m(p) {}
    S operator()(const T& p) const { return (p.*m)(); }
};

template <class S, class T, class A>
class mem_fun1_ref_t : public binary_function<T, A, S>
{
    S (T::*m)(A);
  public:
    explicit mem_fun1_ref_t(S (T::*p)(A)) : m(p) {}
    S operator()(T& p, A x) const { return (p.*m)(x); }
};

template <class S, class T, class A>
class const_mem_fun1_ref_t : public binary_function<T, A, S>
{
    S (T::*m)(A) const;
  public:
    explicit const_mem_fun1_ref_t(S (T::*p)(A) const) : m(p) {}
    S operator()(const T& p, A x) const { return (p.*m)(x); }
};

template<class S, class T>
inline
mem_fun_ref_t<S, T>
  mem_fun_ref(S (T::*f)())
{
  return mem_fun_ref_t<S, T>(f);
}

template <class S, class T>
inline
const_mem_fun_ref_t<S, T>
  mem_fun_ref(S (T::*f)() const)
{
  return const_mem_fun_ref_t<S, T>(f);
}

template<class S, class T, class A>
inline
mem_fun1_ref_t<S, T, A>
  mem_fun_ref(S (T::*f)(A))
{
  return mem_fun1_ref_t<S, T, A>(f);
}

template <class S, class T, class A>
inline
const_mem_fun1_ref_t<S, T, A>
  mem_fun_ref(S (T::*f)(A) const)
{
  return const_mem_fun1_ref_t<S, T, A>(f);
}

/**@} lib_member_pointer_adaptors
 */

/**@} lib_function_objects */

/**@} lib_utilities */

}//namespace std

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif//#ifndef NTL__STLX_FUNCTIONAL
