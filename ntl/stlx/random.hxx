/**\file*********************************************************************
 *                                                                     \brief
 *  26.5 Random number generation [rand]
 *  This subclause defines a facility for generating (pseudo-)random numbers.
 ****************************************************************************
 */
#ifndef NTL__STLX_RANDOM
#define NTL__STLX_RANDOM
#pragma once

#include "type_traits.hxx"
#include "vector.hxx"
#include "iomanip.hxx"
#include "ratio.hxx"
#include "cmath.hxx"
#include "ext/numeric_conversions.hxx"

#ifndef NTL_CXX_CONSTEXPR
#pragma push_macro("constexpr")
#undef constexpr
#define constexpr const
#endif


namespace std
{
 /**\addtogroup  lib_numeric ************ 26 Numerics library [numerics]
  *@{
  **/
 /**\addtogroup  lib_numeric_rand ******* 26.5 Random number generation [rand]
  * This subclause defines a facility for generating (pseudo-)random numbers.
  *
  * In addition to a few utilities, four categories of entities are described: <i>uniform random number generators</i>,
  * <i>random number engines</i>, <i>random number engine adaptors</i>, and <i>random number distributions</i>. 
  * These categorizations are applicable to types that satisfy the corresponding requirements, to objects instantiated from
  * such types, and to templates producing such types when instantiated.
  *
  * Each of the entities specified via this subclause has an associated arithmetic type (3.9.1) identified as \c result_type. 
  * With \c T as \c the result_type thus associated with such an entity, that entity is characterized
  * -# as \e boolean or equivalently as <i>boolean-valued</i>, if \c T is \c bool;
  * -# otherwise as \e integral or equivalently as <i>integer-valued</i>, if <tt>numeric_limits<T>::is_integer</tt> is \c true;
  * -# otherwise as \e floating or equivalently as <i>real-valued</i>.
  * .
  * If integer-valued, an entity may optionally be further characterized as \e signed or \e unsigned, according to \c T.
  *
  * Unless otherwise specified, all descriptions of calculations in this subclause use mathematical real numbers.
  *
  * Throughout this subclause, the operators \c bitand, \c bitor, and \c xor denote the respective conventional bitwise operations. Further,
  * -# the operator \c rshift denotes a bitwise right shift with zero-valued bits appearing in the high bits of the result, and
  * -# the operator \c lshift<sub>w</sub> denotes a bitwise left shift with zero-valued bits appearing in the low bits of the
  * result, and whose result is always taken modulo 2<sup>w</sup>.
  *
  * @note These entities are specified in such a way as to permit the binding of any uniform random number generator object \c e as the argument to
  * any random number distribution object \c d, thus producing a zero-argument function object such as given by <tt>bind(d,e)</tt>.
  *
  *@{
  **/


  // 26.5.3.1 Class template linear_congruential_engine
  template <class UIntType, UIntType a, UIntType c, UIntType m>
  class linear_congruential_engine;

  // 26.5.3.2 Class template mersenne_twister_engine
  template <class UIntType, size_t w, size_t n, size_t m, size_t r,
    UIntType a, size_t u, UIntType d, size_t s,
    UIntType b, size_t t,
    UIntType c, size_t l, UIntType f>
  class mersenne_twister_engine;

  // 26.5.3.3 Class template subtract_with_carry_engine
  template <class UIntType, size_t w, size_t s, size_t r>
  class subtract_with_carry_engine;

  // 26.5.4.2 Class template discard_block_engine
  template <class Engine, size_t p, size_t r>
  class discard_block_engine;

  // 26.5.4.3 Class template independent_bits_engine
  template <class Engine, size_t w, class UIntType>
  class independent_bits_engine;

  // 26.5.4.4 Class template shuffle_order_engine
  template <class Engine, size_t k>
  class shuffle_order_engine;

  /**\defgroup lib_numeric_rand_predef 26.5.5 Engines and engine adaptors with predefined parameters
   @{
   **/
  typedef linear_congruential_engine<uint_fast32_t, 16807, 0, 2147483647> minstd_rand0;
  typedef linear_congruential_engine<uint_fast32_t, 48271, 0, 2147483647> minstd_rand;

  typedef mersenne_twister_engine<uint_fast32_t, 32,624,397,31,0x9908b0df,11,
                                  0xffffffff,7,
                                  0x9d2c5680,15,
                                  0xefc60000,18,
                                  1812433253> mt19937;
  typedef mersenne_twister_engine<uint_fast64_t, 64,312,156,31,0xb5026f5aa96619e9,29,
                                  0x5555555555555555,17,
                                  0x71d67fffeda60000,37,
                                  0xfff7eee000000000,43,
                                  6364136223846793005> mt19937_64;
  
  typedef subtract_with_carry_engine<uint_fast32_t, 24, 10, 24> ranlux24_base;
  typedef subtract_with_carry_engine<uint_fast64_t, 48, 5, 12> ranlux48_base;
  
  typedef discard_block_engine<ranlux24_base, 223, 23> ranlux24;
  typedef discard_block_engine<ranlux48_base, 389, 11> ranlux48;
  
  typedef shuffle_order_engine<minstd_rand0, 256> knuth_b;

  typedef minstd_rand default_random_engine;
  /** @} lib_numeric_rand_predef */


  // 26.5.6 Class random_device
  class random_device;

  // 26.5.7.1 Class seed_seq
  class seed_seq;

  // 26.5.7.2 Function template generate_canonical
  template<class RealType, size_t bits, class UniformRandomNumberGenerator>
  RealType generate_canonical(UniformRandomNumberGenerator& g);

  // 26.5.8.1.1 Class template uniform_int_distribution
  template <class IntType = int>
  class uniform_int_distribution;

  // 26.5.8.1.2 Class template uniform_real_distribution
  template <class RealType = double>
  class uniform_real_distribution;

  // 26.5.8.2.1 Class bernoulli_distribution
  class bernoulli_distribution;
  
  // 26.5.8.2.2 Class template binomial_distribution
  template <class IntType = int>
  class binomial_distribution;
  
  // 26.5.8.2.3 Class template geometric_distribution
  template <class IntType = int>
  class geometric_distribution;
  
  // 26.5.8.2.4 Class template negative_binomial_distribution
  template <class IntType = int>
  class negative_binomial_distribution;
  
  // 26.5.8.3.1 Class template poisson_distribution
  template <class IntType = int>
  class poisson_distribution;
  
  // 26.5.8.3.2 Class template exponential_distribution
  template <class RealType = double>
  class exponential_distribution;
  
  // 26.5.8.3.3 Class template gamma_distribution
  template <class RealType = double>
  class gamma_distribution;
  
  // 26.5.8.3.4 Class template weibull_distribution
  template <class RealType = double>
  class weibull_distribution;
  
  // 26.5.8.3.5 Class template extreme_value_distribution
  template <class RealType = double>
  class extreme_value_distribution;
  
  // 26.5.8.4.1 Class template normal_distribution
  template <class RealType = double>
  class normal_distribution;
  
  // 26.5.8.4.2 Class template lognormal_distribution
  template <class RealType = double>
  class lognormal_distribution;
  
  // 26.5.8.4.3 Class template chi_squared_distribution
  template <class RealType = double>
  class chi_squared_distribution;
  
  // 26.5.8.4.4 Class template cauchy_distribution
  template <class RealType = double>
  class cauchy_distribution;

  // 26.5.8.4.5 Class template fisher_f_distribution
  template <class RealType = double>
  class fisher_f_distribution;
  
  // 26.5.8.4.6 Class template student_t_distribution
  template <class RealType = double>
  class student_t_distribution;
  
  // 26.5.8.5.1 Class template discrete_distribution
  template <class IntType = int>
  class discrete_distribution;
  
  // 26.5.8.5.2 Class template piecewise_constant_distribution
  template <class RealType = double>
  class piecewise_constant_distribution;
  
  // 26.5.8.5.3 Class template general_pdf_distribution
  template <class RealType = double>
  class general_pdf_distribution;


  namespace __ {
    template<typename T, T m, T a = 1, T c = 0, bool zero = (m == 0)>
    struct safe_mod: false_type
    {
      static T eval(T x)
      {
        //return (a*x + c) % m;
        if(a == 1){
          x %= m;
        }else{
          static const T q = m / a,
            r = m % a;

          const T t1 = a*(x%q),
            t2 = r*(x/q);
          if(t1 >= t2)
            x = t1 - t2;
          else
            x = (m-t2)+t1;
        }
        if(c != 0){
          const T d = m - x;
          if(d > c)
            x += c;
          else
            x = c - d;
        }
        return x;
      }
    };
    template<typename T, T m, T a, T c>
    struct safe_mod<T,m,a,c,true>: true_type
    {
      static T eval(T x) { return a*x + c; }
    };
  }


 /**\addtogroup  lib_numeric_rand_eng ************ 26.5.3 Random number engine class templates [rand.eng]
  *@{
  **/
  /**
   *	@brief 26.5.3.1 Class template linear_congruential_engine [rand.eng.lcong]
   *  @details A linear_congruential engine produces random numbers using a linear function <tt>x(i+1) := (a * x(i) + c) mod m</tt>.
   **/
  template <class UIntType, UIntType a, UIntType c, UIntType m>
  class linear_congruential_engine
  {
    static_assert(is_integral<UIntType>::value && is_unsigned<UIntType>::value, "Template parameter UIntType shall denote an unsigned integral type.");
    typedef __::safe_mod<UIntType, m, a, c> zerom;
    typedef __::safe_mod<UIntType, m> safe_mod;

    template<class T, bool small = (sizeof(result_type) <= sizeof(uint32_t))>
    struct shift32 { static const T value = 0; };
    template<class T> struct shift32<T,false>
    { static const T value = T(1) << 32; };
  public:
    // types
    typedef UIntType result_type;

    ///\name engine characteristics
    static const result_type multiplier   = a;
    static const result_type increment    = c;
    static const result_type modulus      = m;  // max+1 == 0
    static const result_type default_seed = 1u;
    static_assert(m == 0 || (a < m && c < m), "following relations shall hold: a < m and c < m");

    static constexpr result_type min() { return increment == 0u ? 1u : 0u; }
    static constexpr result_type max() { return modulus - 1u; }

    
    ///\name constructors and seeding functions
    /** Constructs a linear_congruential_engine object. */
    explicit linear_congruential_engine(result_type s = default_seed)
    {
      seed(s, zerom());
    }

    template<class Sseq>
    explicit linear_congruential_engine(Sseq& q, typename enable_if<!is_same<Sseq, linear_congruential_engine>::value>::type* =0)
    {
      seed(q);
    }

    /** If <tt>c mod m</tt> is 0 and <tt>s mod m</tt> is 0, 
      sets the engine's state to 1, otherwise sets the engine's state to <tt>s mod m</tt>.  */
    void seed(result_type s = default_seed)
    {
      seed(s, zerom());
    }

    template<class Sseq>
    typename enable_if<is_class<Sseq>::value>::type seed(Sseq& q)
    {
      static const result_type k0 = m == 0 ? numeric_limits<result_type>::digits : __::static_evaluation::log2<modulus>::value;
      static const result_type k = (k0+31) / 32;
      
      std::array<uint32_t, k+3> arr;
      q.generate(arr.begin(), arr.end());


      result_type f = 1, s = 0;
      for(size_t j = 0; j < k; j++){
        s += arr[j+3] * f;
        f *= shift32<result_type>::value;
      }
      seed(s);
    }
    
    ///\name generating functions
    result_type operator()()
    {
      return produce();
    }
    void discard(unsigned long long z)
    {
      while(z--)
        produce();
    }

    ///\name comparsion
    friend bool operator==(const linear_congruential_engine& x, const linear_congruential_engine& y) { return x.state_ == y.state_; }
    friend bool operator!=(const linear_congruential_engine& x, const linear_congruential_engine& y) { return x.state_ != y.state_; }
    
    ///\name I/O support
    template<typename charT, typename traits>
    friend basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, const linear_congruential_engine& x)
    {
      saveiostate s(os); os.flags(ios_base::dec|ios_base::left);
      return os << setfill(' ') << x.state_;
    }
    template<typename charT, typename traits>
    friend basic_istream<charT, traits>& operator>>(basic_istream<charT, traits>& is, linear_congruential_engine& x)
    {
      saveiostate s(is); is.flags(ios_base::dec);
      return is >> x.state_;
    }
    ///\}
  private:
    // DivByZero prevention
    result_type produce()
    {
      return state_ = zerom::eval(state_);
    }
    void seed(result_type s, false_type) { state_ = (increment % modulus == 0 && s % modulus == 0) ? 1 : s % modulus; }
    void seed(result_type, true_type) { state_ = 1; }

  private:
    result_type state_;
  };



  /**
   *	@brief 26.5.3.2 Class template mersenne_twister_engine [rand.eng.mers]
   **/
  template<class UIntType, size_t w, size_t n, size_t m, size_t r,
    UIntType a, size_t u, UIntType d, size_t s,
    UIntType b, size_t t,
    UIntType c, size_t l, UIntType f>
  class mersenne_twister_engine
  {
    template<size_t w, bool ok = (w < numeric_limits<UIntType>::digits)>
    struct shift1 { static const UIntType value = (UIntType(1) << w) - 1; };
    template<size_t w> struct shift1<w, false>
    { static const UIntType value = numeric_limits<UIntType>::__max; };

    template<size_t w, bool ok = (w < numeric_limits<UIntType>::digits)>
    struct shift { static const size_t value = UIntType(1) << w; };
    template<size_t w> struct shift<w, false>
    { static const size_t value = 0; };

    template<class T, bool small = (sizeof(result_type) <= sizeof(uint32_t))>
    struct shift32 { static const T value = 0; };
    template<class T> struct shift32<T,false>
    { static const T value = T(1) << 32; };

    static_assert(is_integral<UIntType>::value && is_unsigned<UIntType>::value, "UIntType shall denote an unsigned integral type.");
    static_assert(0 < m && m < n, "0 < m && m < n");
    static_assert(2*u < w, "2u < w");
    static_assert(r <= w, "r <= w");
    static_assert(u <= w, "u <= w");
    static_assert(s <= w, "s <= w");
    static_assert(t <= w, "t <= w");
    static_assert(l <= w, "l <= w");
    static_assert(w <= numeric_limits<UIntType>::digits, "w <= numeric_limits<UIntType>::digits");
    static_assert(a <= shift1<w>::value, "a <= (1 << w) - 1");
    static_assert(b <= shift1<w>::value, "b <= (1 << w) - 1");
    static_assert(c <= shift1<w>::value, "c <= (1 << w) - 1");
    static_assert(d <= shift1<w>::value, "d <= (1 << w) - 1");
    static_assert(f <= shift1<w>::value, "f <= (1 << w) - 1");

    typedef __::safe_mod<UIntType, shift<w>::value> modw;
  public:
    ///\name types
    typedef UIntType result_type;

    ///\name engine characteristics
    static constexpr size_t      word_size                 =  w;
    static constexpr size_t      state_size                =  n;
    static constexpr size_t      shift_size                =  m;
    static constexpr size_t      mask_bits                 =  r;
    static constexpr UIntType    xor_mask                  =  a;
    static constexpr size_t      tempering_u               =  u;
    static constexpr UIntType    tempering_d               =  d;
    static constexpr size_t      tempering_s               =  s;
    static constexpr UIntType    tempering_b               =  b;
    static constexpr size_t      tempering_t               =  t;
    static constexpr UIntType    tempering_c               =  c;
    static constexpr size_t      tempering_l               =  l;
    static constexpr UIntType    initialization_multiplier =  f;
    static constexpr result_type min()                     {  return 0; }
    static constexpr result_type max()                     {  return shift1<w>::value; }

    static constexpr result_type default_seed              =  5489u;


    ///\name constructors and seeding functions
    explicit mersenne_twister_engine(result_type value = default_seed)
    {
      seed(value);
    }
    
    template<class Sseq>
    explicit mersenne_twister_engine(Sseq& q, typename enable_if<!is_same<Sseq, mersenne_twister_engine>::value>::type* =0)
    {
      seed(q);
    }

    void seed(result_type value = default_seed)
    {
      state[0] = modw::eval(value);
      for(size_t i = 1; i < state_size; i++){
        result_type x = state[i-1];
        x ^= x >> (word_size-2);
        x *= initialization_multiplier;
        const result_type v1 = __::safe_mod<UIntType, state_size>::eval(i); // x += i
        const result_type v2 = i;
        assert(v1 == v2);
        x += v1;
        state[i] = modw::eval(x);
      }
      n = state_size;
    }

    template<class Sseq>
    typename enable_if<is_class<Sseq>::value>::type seed(Sseq& q)
    {
      static const result_type H = ~static_cast<UIntType>(0) << mask_bits;
      static const size_t k = (word_size+31) / 32;
      std::array<uint32_t, state_size*k> arr;
      q.generate(arr.begin(), arr.end());
      bool zero = true;
      for(size_t i = 0; i < state_size; i++){
        result_type f = 1, sum = 0;
        for(size_t j = 0; j < k; j++){
          sum += arr[k*i+j] * f;
          f *= shift32<result_type>::value;
        }
        state[i] = modw::eval(sum);
        if(zero){
          if(i == 0 && state[i] & H)
            zero = false;
          else if(i != 0 && state[i] != 0)
            zero = false;
        }
      }
      if(zero){
        state[0] = result_type(1) << (word_size-1);
      }
    }

    ///\name generating functions
    result_type operator()()
    {
      if(n >= state_size)
        rotate();

      result_type z = state[n++];
      z ^= (z >> tempering_u) & tempering_d;
      z ^= (z << tempering_s) & tempering_b;
      z ^= (z << tempering_t) & tempering_c;
      z ^= (z >> tempering_l);
      return z;
    }

    void discard(unsigned long long z)
    {
      while(z--)
        this->operator ()();
    }

    ///\name comparsion
    friend bool operator==(const mersenne_twister_engine& x, const mersenne_twister_engine& y) { return std::equal(x.state,x.state+state_size, y.state); }
    friend bool operator!=(const mersenne_twister_engine& x, const mersenne_twister_engine& y) { return rel_ops::operator !=(x,y); }

    ///\name I/O support
    template<typename charT, typename traits>
    friend basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, const mersenne_twister_engine& x)
    {
      saveiostate s(os); os.flags(ios_base::dec|ios_base::left|ios_base::fixed);
      os << setfill(' ') << x.state[0];
      const charT w = os.widen(' ');
      for(size_t i = 1; i < state_size; i++)
        os << w << x.state[i];
      return os;
    }
    template<typename charT, typename traits>
    friend basic_istream<charT, traits>& operator>>(basic_istream<charT, traits>& is, mersenne_twister_engine& x)
    {
      saveiostate s(is); is.flags(ios_base::dec|ios_base::skipws);
      for(size_t i = 0; i < state_size; i++)
        is >> x.state[i];
      return is;
    }
    ///\}
  protected:
    void rotate()
    {
      static const result_type H = ~static_cast<UIntType>(0) << mask_bits;
      static const result_type L = ~H;
      
      static const size_t part = state_size - shift_size, end = state_size-1;

      for(size_t k = 0; k < part; k++){
        result_type y = (state[k] & H) | (state[k+1] & L);
        state[k] = state[k+shift_size] ^ (y >> 1) ^ ((y&1) ? xor_mask : 0);
      }
      for(size_t k = part; k < end; k++){
        result_type y = (state[k] & H) | (state[k+1] & L);
        state[k] = state[k-part] ^ (y >> 1) ^ ((y&1) ? xor_mask : 0);
      }
      result_type y = (state[end] & H) | (state[0] & L);
      state[end] = state[shift_size-1] ^ (y >> 1) ^ ((y&1) ? xor_mask : 0);
      n = 0;
    }

  private:
    UIntType state[state_size];
    size_t n;
  };

  /**@} lib_numeric_rand_eng */



  /**\addtogroup  lib_numeric_rand_adapt ************ 26.5.4 Random number engine adaptor class templates [rand.adapt]
  *@{
  **/

  /**
   *	@brief 26.5.4.2 Class template discard_block_engine [rand.adapt.disc]
   *  @details A discard_block engine produces random numbers from some base engine by discarding blocks of data.
   **/
  template <class Engine, size_t p, size_t r>
  class discard_block_engine
  {
  public:
    ///\name types
    typedef Engine base_type;
    typedef typename base_type::result_type result_type;

    ///\name engine characteristics
    static const size_t block_size = p;
    static const size_t used_block = r;

    static constexpr result_type min() { return base_type::min(); }
    static constexpr result_type max() { return base_type::max(); }

    ///\name constructors and seeding functions
    discard_block_engine()
      :n(0)
    {}
    explicit discard_block_engine(const base_type& urng)
      :e(urng), n(0)
    {}
#ifdef NTL__CXX_RV
    explicit discard_block_engine(base_type&& urng)
      :e(std::move(urng)), n(0)
    {}
#endif
    explicit discard_block_engine(result_type s)
      :e(s), n(0)
    {}
    template<class Sseq>
    explicit discard_block_engine(Sseq& q, typename enable_if<!is_same<Sseq, discard_block_engine>::value && !is_same<Sseq, Engine>::value>::type* =0)
      :e(q), n(0)
    {}
    void seed()
    {
      e.seed(), n = 0;
    }
    void seed(result_type s)
    {
      e.seed(s), n = 0;
    }
    template<class Sseq>
    void seed(Sseq& q)
    {
      e.seed(q), n = 0;
    }

    ///\name generating functions
    result_type operator()()
    {
      if(n >= used_block){
        size_t i = block_size - used_block;
        while(i--)
          e();
        n = 0;
      }
      ++n;
      return e();
    }

    void discard(unsigned long long z)
    {
      while(z--)
        this->operator ()();
    }

    ///\name property functions
    const base_type& base() const { return e; }

    ///\name comparsion
    friend bool operator==(const discard_block_engine& x, const discard_block_engine& y) { return x.n == y.n && x.e == y.e; }
    friend bool operator!=(const discard_block_engine& x, const discard_block_engine& y) { return rel_ops::operator !=(x,y); }

    ///\name I/O support
    template<typename charT, typename traits>
    friend basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, const discard_block_engine& x)
    {
      saveiostate s(os); os.flags(ios_base::dec|ios_base::left|ios_base::fixed);
      return os << setfill(' ') << x.base() << ' ' << x.n;
    }
    template<typename charT, typename traits>
    friend basic_istream<charT, traits>& operator>>(basic_istream<charT, traits>& is, discard_block_engine& x)
    {
      saveiostate s(is); is.flags(ios_base::dec|ios_base::skipws);
      return is >> x.e >> x.n;
    }
    ///\}
  private:
    base_type e;
    size_t n;
  };



  /**
   *	@brief 26.5.4.4 Class template shuffle_order_engine [rand.adapt.shuf ]
   *  @details A shuffle_order_engine random number engine adaptor produces the same random numbers that are
   *  produced by some base engine \e e, but delivers them in a different sequence. The state \c x<sub>i</sub> of a 
   *  shuffle_order_engine engine adaptor object \c x consists of the state \c e<sub>i</sub> of its base engine \e e, an additional value \e Y of
   *  the type delivered by \c e, and an additional sequence \e V of \e k values also of the type delivered by \c e. The size
   *  of the state is the size of e's state plus \e k + 1.
   **/
  template<class Engine, size_t k>
  class shuffle_order_engine
  {
  public:
    ///\name types
    typedef typename Engine::result_type result_type;

    ///\name engine characteristics
    static constexpr size_t table_size = k;
    static constexpr result_type min() { return Engine::min(); }
    static constexpr result_type max() { return Engine::max(); }

    static_assert(table_size > 0, "0 < k");

    ///\name constructors and seeding functions
    shuffle_order_engine()
    { init(); }

    explicit shuffle_order_engine(const Engine& e)
      :e(e)
    { init(); }

#ifdef NTL__CXX_RV
    explicit shuffle_order_engine(Engine&& e)
      :e(std::move(e))
    { init(); }
#endif

    explicit shuffle_order_engine(result_type s)
      :e(s)
    { init(); }

    template<class Sseq> 
    explicit shuffle_order_engine(Sseq& q, typename enable_if<!is_same<Sseq, shuffle_order_engine>::value && !is_same<Sseq, Engine>::value>::type* =0)
      :e(q)
    { init(); }


    void seed()
    {
      e.seed();
      init();
    }
    void seed(result_type s)
    {
      e.seed(s);
      init();
    }

    template<class Sseq> 
    void seed(Sseq& q)
    {
      e.seed(q);
      init();
    }

    ///\name generating functions
    result_type operator()()
    {
      const result_type min = e.min();
      const result_type diff = e.max() - min + 1;
      const size_t j = static_cast<size_t>( double(table_size) * (Y - min) / diff );
      Y = V[j];
      V[j] = e();
      return Y;
    }

    void discard(unsigned long long z)
    {
      while(z--)
        this->operator()();
    }

    ///\name property functions
    const Engine& base() const { return e; }

    ///\name comparsion
    friend bool operator==(const shuffle_order_engine& x, const shuffle_order_engine& y) { return x.e == y.e && x.Y == y.Y && std::equal(x.V,x.V+table_size, y.V); }
    friend bool operator!=(const shuffle_order_engine& x, const shuffle_order_engine& y) { return rel_ops::operator !=(x,y); }

    ///\name I/O support
    template<typename charT, typename traits>
    friend basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, const shuffle_order_engine& x)
    {
      saveiostate s(os); os.flags(ios_base::dec|ios_base::left|ios_base::fixed);
      const charT w = os.widen(' ');    // space separator
      os << setfill(' ') << x.base();
      for(size_t i = 0; i < table_size; i++)
        os << w << x.V[i];
      os << w << x.Y;
      return os;
    }
    template<typename charT, typename traits>
    friend basic_istream<charT, traits>& operator>>(basic_istream<charT, traits>& is, shuffle_order_engine& x)
    {
      saveiostate s(is); is.flags(ios_base::dec|ios_base::skipws);
      is >> x.e;
      for(size_t i = 0; i < table_size; i++)
        is >> x.V[i];
      is >> x.Y;
      return is;
    }
    ///\}

  protected:
    void init()
    {
      for(size_t i = 0; i < table_size; i++)
        V[i] = e();
      Y = e();
    }
  private:
    Engine e;
    result_type Y;
    result_type V[table_size];
  };



  /**
   *	@brief Class template xor_combine
   *  @details The class template describes a compound engine whose <tt>operator()</tt> returns <tt>(eng1() << shift1) xor (eng2() << shift2)</tt>.
   *  The engine combines the results of two engines, using left shifts and bitwise exclusive OR. 
   *  Except in unusual circumstances, at least one of the two shift values should be 0. 
   *  For best results, the engine whose value is not shifted should produce values ranging from 0 to 2<sup>n</sup> - 1, 
   *  and the shifted values from the other engine should be somewhere in that range.
   **/
   template<class Eng1, size_t S1, class Eng2, size_t S2>
   class xor_combine
   {
     typedef typename Eng1::result_type R1;
     typedef typename Eng2::result_type R2;
   public:
     ///\name types
     //typedef typename std::common_type<typename Eng1::result_type, typename Eng2::result_type>::type  result_type;
     typedef typename std::conditional<( R1(-1) <= R2(-1) ), R1, R2>::type result_type;
     typedef Eng1 base1_type;
     typedef Eng2 base2_type;

     ///\name engine characteristics
     static constexpr size_t shift1 = S1;
     static constexpr size_t shift2 = S2;

     static constexpr result_type min();
     static constexpr result_type max();

     ///\name constructors and seeding functions
     xor_combine()
     {}
     xor_combine(const base1_type& e1, const base2_type& e2)
       :eng1(e1), eng2(e2)
     {}
  #ifdef NTL__CXX_RV
     xor_combine(base1_type&& e1, base2_type&& e2)
       :eng1(std::move(e1)), eng2(std::move(e2))
     {}
  #endif
     xor_combine(result_type s)
       :eng1(static_cast<R1>(s)),
        eng2(static_cast<R2>(s+1))
     {}

     template <class Sseq>
     xor_combine(Sseq& q, typename enable_if<!is_same<Sseq, xor_combine>::value && !is_same<Sseq, Eng1>::value && !is_same<Sseq, Eng2>::value>::type* =0)
       :eng1(q), eng2(q)
     {}

     void seed()
     { 
       eng1.seed();
       eng2.seed();
     }

     void seed(result_type s)
     {
       eng1.seed(static_cast<R1>(s));
       eng2.seed(static_cast<R2>(s+1));
     }

     template<class Sseq>
     void seed(Sseq& q)
     {
       eng1.seed(q);
       eng2.seed(q);
     }

     ///\name generating functions
     result_type operator()()
     {
       return (eng1() << shift1) ^ (eng2() << shift2);
     }

     void discard(unsigned long long z)
     {
       while(z--)
         this->operator()();
     }

     ///\name property functions
     const base1_type& base1() const { return eng1; }
     const base2_type& base2() const { return eng2; }

     ///\name comparsion
     friend bool operator==(const xor_combine& x, const xor_combine& y) { return x.eng1 == y.eng1 && x.eng2 == y.eng2; }
     friend bool operator!=(const xor_combine& x, const xor_combine& y) { return rel_ops::operator !=(x,y); }

     ///\name I/O support
     template<typename charT, typename traits>
     friend basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, const xor_combine& x)
     {
       return os << x.base1() << ' ' << x.base2();
     }
     template<typename charT, typename traits>
     friend basic_istream<charT, traits>& operator>>(basic_istream<charT, traits>& is, xor_combine& x)
     {
       return is >> x.eng1 >> x.eng2;
     }
     ///\}
   private:
     Eng1 eng1;
     Eng2 eng2;
   };


  /**@} lib_numeric_rand_adapt */



  /**
   *	@brief 26.5.6 Class random_device [rand.device]
   *  @details A random_device uniform random number generator produces non-deterministic random numbers.
   *  @note In NTL mt19937 selected as generator.
   **/
  class random_device
  {
  public:
    ///\name types
    typedef unsigned int result_type;

    ///\name generator characteristics
    static constexpr result_type min() { return numeric_limits<result_type>::min(); }
    static constexpr result_type max() { return numeric_limits<result_type>::max(); }

    ///\name constructors
    random_device()
    {}
    /** Constructs generator with initial state determined by \p token (which must be a number). */
    explicit random_device(const string& token)
    {
      result_type s = ntl::numeric::strtoul(token.c_str());
      if(s)
        gen.seed(s);
    }

    ///\name generating functions
    result_type operator()() { return gen(); }

    ///\name property functions
    double entropy() const __ntl_nothrow { return 0.0; }
    ///\}
  private:
    mt19937 gen;

    random_device(const random_device& ) __deleted;
    void operator=(const random_device& ) __deleted;
  };



  /**\addtogroup  lib_numeric_rand_util ************ 26.5.7 Utilities [rand.util]
  *@{
  **/
  /**
   *	@brief    26.5.7.1 Class seed_seq [rand.util.seedseq]
   *  @details  An object of type seed_seq consumes a sequence of integer-valued data and produces a fixed number of
   *  unsigned integer values, <tt>0 <= i < 2<sup>32</sup></tt>, based on the consumed data.
   *  @note   Such an object provides a mechanism to avoid replication of streams of random variates.
   *  This can be useful in applications requiring large numbers of random number engines.
   **/
  class seed_seq:
    noncopyable
  {
  public:
    ///\name types
    typedef uint_least32_t result_type;

    ///\name constructors
    /** Constructs a seed_seq object */
    seed_seq()
    {}

    /** Constructs a seed_seq object by rearranging some or all of the bits of the supplied sequence <tt>[begin,end)</tt> of w-bit quantities into 32-bit units */
    template<class InputIterator>
    seed_seq(InputIterator begin, InputIterator end)
    {
      construct(begin, end, std::iterator_traits<InputIterator>::iterator_category());
    }

    template<class In>
    seed_seq(const initializer_list<In>& il)
    {
      construct(il.begin(), il.end(), std::iterator_traits<initializer_list<In> >::iterator_category() );
    }

    ///\name generating functions
    template<class RandomAccessIterator>
    void generate(RandomAccessIterator begin, RandomAccessIterator end) const
    {
      if(begin == end)
        return;
      typedef typename iterator_traits<RandomAccessIterator>::value_type value_type;
      std::fill(begin, end, value_type(0x8B8B8B8B));
      const size_t s = v.size(), n = end - begin;
      const size_t t = (n >= 623) ? 11
        : (n >=  68) ? 7
        : (n >=  39) ? 5
        : (n >=   7) ? 3
        : (n - 1) / 2;
      const size_t p = (n - t) / 2;
      const size_t q = p + t;
      const size_t m = std::max(s + 1, n);

      struct safe_begin
      {
        RandomAccessIterator e;
        size_t n;
        value_type& operator[](size_t i) { return e[i%n]; }
      } begun = {begin, n};
      struct 
      {
        value_type operator()(const value_type& x) const { return x ^ (x >> 27); }
      } T;
#if 1
      for(size_t k = 0; k < m; k++){
        const value_type arg = begun[k] ^ begun[k+p] ^ begun[k-1];
        const value_type r1 = static_cast<result_type>(T(arg) * 1664525u);
        value_type r2 = r1;
        if (k == 0)
          r2 += s;
        else if (k <= s)
          r2 += k % n + v[k - 1];
        else
          r2 += k % n;
        r2 = static_cast<result_type>(r2);
        begun[k+p] += r1;
        begun[k+q] += r2;
        begun[k] = r2;
      }
      for(size_t k = m; k < (m+n-1); k++){
        const value_type arg = begun[k] ^ begun[k+p] ^ begun[k-1];
        const value_type r3 = static_cast<result_type>(T(arg) * 1566083941u);
        const value_type r4 = r3 - (k % n);
        begun[k+p] ^= r3;
        begun[k+q] ^= r4;
        begun[k] = r4;
      }
#else
      for (size_t k = 0; k < m; ++k)
      {
        value_type arg = (begin[k % n] ^ begin[(k + p) % n] ^ begin[(k - 1) % n]);
        value_type r1 = arg ^ (arg << 27);
        r1 = detail::mod<_Type, detail::_Shift<_Type, 32>::value, 1664525u, 0u>(r1);
        value_type r2 = r1;
        if (k == 0)
          r2 += s;
        else if (k <= s)
          r2 += k % n + v[k - 1];
        else
          r2 += k % n;
        r2 = detail::mod<_Type,
          detail::_Shift<_Type, 32>::value>(r2);
        begin[(k + p) % n] += r1;
        begin[(k + q) % n] += r2;
        begin[k % n] = r2;
      }

      for (size_t k = m; k < m + n; ++k)
      {
        value_type arg = (begin[k % n] + begin[(k + p) % n] + begin[(k - 1) % n]);
        value_type r3 = arg ^ (arg << 27);
        r3 = detail::mod<_Type, detail::_Shift<_Type, 32>::value, 1566083941u, 0u>(r3);
        value_type r4 = r3 - k % n;
        r4 = detail::mod<_Type, detail::_Shift<_Type, 32>::value>(r4);
        begin[(k + p) % n] ^= r4;
        begin[(k + q) % n] ^= r3;
        begin[k % n] = r4;
      }
#endif
    }

    ///\name property functions
    /** Returns the number of 32-bit units that would be returned by a call to param(). */
    size_t size() const { return v.size(); }

    /** Copies the sequence of prepared 32-bit units to the given destination. */
    template<class OutputIterator>
    void param(OutputIterator dest) const
    {
      copy(v.cbegin(), v.cend(), dest);
    }
    ///\}
  private:
    template<class InputIterator>
    void construct(InputIterator begin, InputIterator end, random_access_iterator_tag)
    {
      v.reserve(std::distance(begin, end));
      while(begin != end){
        v.push_back(static_cast<result_type>(*begin++)); // static_cast<uint32_t>(v) == v & 0xffffFFFF
      }
    }
    template<class InputIterator>
    void construct(InputIterator begin, InputIterator end, input_iterator_tag)
    {
      while(begin != end){
        v.push_back(static_cast<result_type>(*begin++)); // static_cast<uint32_t>(v) == v & 0xffffFFFF
      }
    }

    vector<result_type> v;
  };



  /**
   *	@brief 26.5.7.2 Function template generate_canonical [rand.util.canonical]
   *  @details Each function instantiated from the template described in this section 26.5.7.2 maps the result of one or
   *  more invocations of a supplied uniform random number generator \c g to one member of the specified \c RealType
   *  such that, if the values \e g<sub>i</sub> produced by \c g are uniformly distributed, the instantiation's results t<sub>j</sub>, 0 <= t<sub>j</sub> < 1,
   *  are distributed as uniformly as possible as specified below.
   **/
  template<class RealType, size_t bits, class URNG>
  inline RealType generate_canonical(URNG& g)
  {
    static const size_t rd = numeric_limits<RealType>::digits, 
      b = rd < bits ? rd : bits;
    const double R = 1.0 + g.max() - g.min();
    const size_t log2r = static_cast<size_t>(std::log(R) / std::log(2.0));
    const size_t k = std::max<size_t>(1, b / log2r);
    const double min = g.min();
    double S = 0, ri = 1;
    for(size_t i = 0; i < k; i++){
      S += (g() - min) * ri;
      ri *= R;
    }
    return static_cast<RealType>(S / ri);
  }

  /**@} lib_numeric_rand_util */



  /**@} lib_numeric_rand */
  /**@} lib_numeric */
}

#include "random_dist.hxx"

#ifndef NTL_CXX_CONSTEXPR
#pragma pop_macro("constexpr")
#endif

#endif // NTL__STLX_RANDOM
