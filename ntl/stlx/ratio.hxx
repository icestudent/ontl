/**\file*********************************************************************
 *                                                                     \brief
 *  Compile-time rational arithmetic [20.3 ratio]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_RATIO
#define NTL__STLX_RATIO

#include "type_traits.hxx"

namespace std 
{
  template <intmax_t N, intmax_t D = 1> class ratio;

  // ratio arithmetic
  template <class R1, class R2> struct ratio_add;
  template <class R1, class R2> struct ratio_subtract;
  template <class R1, class R2> struct ratio_multiply;
  template <class R1, class R2> struct ratio_divide;

  // ratio comparison
  template <class R1, class R2> struct ratio_equal;
  template <class R1, class R2> struct ratio_not_equal;
  template <class R1, class R2> struct ratio_less;
  template <class R1, class R2> struct ratio_less_equal;
  template <class R1, class R2> struct ratio_greater;
  template <class R1, class R2> struct ratio_greater_equal;

  // convenience SI typedefs
  //typedef ratio<1, 1000000000000000000000000> yocto; // see 20.3.4
  //typedef ratio<1,    1000000000000000000000> zepto; // see 20.3.4
  typedef ratio<1,       1000000000000000000> atto;
  typedef ratio<1,          1000000000000000> femto;
  typedef ratio<1,             1000000000000> pico;
  typedef ratio<1,                1000000000> nano;
  typedef ratio<1,                   1000000> micro;
  typedef ratio<1,                      1000> milli;
  typedef ratio<1,                       100> centi;
  typedef ratio<1,                        10> deci;
  typedef ratio<                       10, 1> deca;
  typedef ratio<                      100, 1> hecto;
  typedef ratio<                     1000, 1> kilo;
  typedef ratio<                  1000000, 1> mega;
  typedef ratio<               1000000000, 1> giga;
  typedef ratio<            1000000000000, 1> tera;
  typedef ratio<         1000000000000000, 1> peta;
  typedef ratio<      1000000000000000000, 1> exa;
  //typedef ratio<   1000000000000000000000, 1> zetta; // see 20.3.4
  //typedef ratio<1000000000000000000000000, 1> yotta; // see 20.3.4


  /// Class template ratio [20.3.1 ratio.ratio]
  template <intmax_t N, intmax_t D>
  class ratio 
  {
    // gcd by euclidean algorithm
    template <intmax_t a, intmax_t b>
    struct gcd      { static const intmax_t value = gcd<b, a % b>::value; };

    template <intmax_t a>
    struct gcd<a, 0>{ static const intmax_t value = a; };

    // sign functions
    template<intmax_t a>
    struct sign     { static const intmax_t value = a == 0 ? 0 : (a > 0 ? 1 : -1); };
    template<intmax_t a>
    struct abs      { static const bool value = a >= 0 ? a : 0-a; };

  public:
    static_assert(D != 0, "template argument D shall not be zero");

    static const intmax_t num = sign<N>::value * sign<D>::value * abs<N>::value / gcd<N, D>::value;
    static const intmax_t den = sign<N>::value * sign<D>::value * abs<D>::value / gcd<N, D>::value;
  };

  /// 20.3.2 Arithmetic on ratio types [ratio.arithmetic]
  template <class R1, class R2>
  struct ratio_add 
  {
    typedef ratio<R1::num*R2::den + R2::num*R1::den, R1::den * R2::den> type;
  };

  template <class R1, class R2>
  struct ratio_subtract 
  {
    typedef ratio<R1::num*R2::den - R2::num*R1::den, R1::den * R2::den> type;
  };

  template <class R1, class R2>
  struct ratio_multiply
  {
    typedef ratio<R1::num*R2::num, R1::den * R2::den> type;
  };

  template <class R1, class R2>
  struct ratio_divide
  {
    typedef ratio<R1::num*R2::den, R1::den * R2::num> type;
  };



  /// 20.3.3 Comparison of ratio types [ratio.comparison]
  template <class R1, class R2> 
  struct ratio_equal: 
    integral_constant<bool, R1::num == R2::num && R1::den == R2::den>
  { };

  template <class R1, class R2> 
  struct ratio_not_equal:
    integral_constant<bool, !ratio_equal<R1, R2>::value> 
  { };

  namespace detail
  {
    template <class R1, class R2> 
    struct ratio_less_helper
    {
      static const bool value = R1::num * R2::den < R2::num * R1::den;
    };

  }
  template <class R1, class R2> 
  struct ratio_less: 
    integral_constant<bool, detail::ratio_less_helper<R1, R2>::value>
  { };

  template <class R1, class R2> 
  struct ratio_less_equal:
    integral_constant<bool, !ratio_less<R2, R1>::value> 
  { };

  template <class R1, class R2> 
  struct ratio_greater:
    integral_constant<bool, ratio_less<R2, R1>::value> 
  { };

  template <class R1, class R2> 
  struct ratio_greater_equal:
    integral_constant<bool, !ratio_less<R1, R2>::value> 
  { };

} // namespace std
#endif // NTL__STLX_RATIO
