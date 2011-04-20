/**\file*********************************************************************
 *                                                                     \brief
 *  26.5 Random number generation [rand]
 *  This subclause defines a facility for generating (pseudo-)random numbers.
 ****************************************************************************
 */
#ifndef NTL__STLX_RANDOM
#error internal <random> file
#endif
#pragma once

namespace std
{
 /**\addtogroup  lib_numeric ************ 26 Numerics library [numerics]
  *@{
 /**\addtogroup  lib_numeric_rand ******* 26.5 Random number generation [rand]
  *@{
  **/

  /**\addtogroup  lib_numeric_rand_dist ************ 26.5.8 Random number distribution class templates [rand.dist]
  *@{
  **/

  namespace __
  {
    template<class E, typename R>
    struct uniform2real
    {
      uniform2real(E& e)
        :e(e)
      {}

      static R min() { return R(0); }
      static R max() { return R(1); }

      R operator()()
      {
        return std::generate_canonical<R, std::numeric_limits<R>::digits, E>(e);
      }
    private:
      E& e;
      uniform2real& operator=(const uniform2real&) __deleted;
    };
  }

  ///\name 26.5.8.2 Uniform distributions [rand.dist.uni]

  /**
   *	@brief 26.5.8.2.1 Class template uniform_int_distribution [rand.dist.uni.int]
   **/
  template<class IntType>
  class uniform_int_distribution
  {
    static_assert(is_integral<IntType>::value, "IntType must be arithmetic type.");
  public:
    ///\name types
    typedef IntType result_type;
    typedef pair<IntType, IntType> param_type;

    ///\name constructors and reset functions
    explicit uniform_int_distribution(IntType a = 0, IntType b = numeric_limits<IntType>::max())
      :p(a,b)
    {
      assert(a <= b);
    }
    explicit uniform_int_distribution(const param_type& parm)
      :p(parm)
    {}

    void reset()
    {}

    ///\name generating functions
    template<class URNG>
    result_type operator()(URNG& g)
    {
      this->operator()(g, param_type(0, numeric_limits<IntType>::max()));
    }
    template<class URNG>
    result_type operator()(URNG& g, const param_type& parm)
    {
      typedef typename make_unsigned<result_type>::type R;
      typedef typename make_unsigned<typename URNG::result_type>::type UR;
      typedef typename conditional<( sizeof(R) > sizeof(UR) ), R, UR>::type T;

      const T min = g.min(), max = g.max(), uspace = max-min, space = max()-min();
      T re;
      if(uspace > space){
        // zoom out
        const T scale = uspace / (space+1),
          up = uspace * scale;
        do {
          re = static_cast<T>(g()) - min;
        } while(re >= up);
        re /= scale;
      }else if(uspace < space){
        // zoom in
        T x, us = uspace+1;
        do{
          x = us * this->operator()(g, param_type(0, space / us));
          re = x + (static_cast<T>(g()) - min);
        } while(re > space || re < x);

      }else{
        re = static_cast<T>(g()) - min;
      }
      return re + p.first;
    }

    ///\name property functions
    result_type a()     const { return p.first;  }
    result_type b()     const { return p.second; }
    param_type  param() const { return p;        }

    void param(const param_type& parm) { p = parm; }

    result_type min() const { return p.first;  }
    result_type max() const { return p.second; }

    ///\name comparsion
    friend bool operator==(const uniform_int_distribution& x, const uniform_int_distribution& y) { return x.p == y.p; }
    friend bool operator!=(const uniform_int_distribution& x, const uniform_int_distribution& y) { return rel_ops::operator !=(x,y); }

    ///\name I/O support
    template<typename charT, typename traits>
    friend basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, const uniform_int_distribution& x)
    {
      saveiostate s(os); os.flags(ios_base::dec|ios_base::left|ios_base::fixed);
      return os << setfill(' ') << x.a() << ' ' << x.b();
    }
    template<typename charT, typename traits>
    friend basic_istream<charT, traits>& operator>>(basic_istream<charT, traits>& is, uniform_int_distribution& x)
    {
      saveiostate s(is); is.flags(ios_base::dec|ios_base::skipws);
      return is >> x.p.first >> x.p.second;
    }
    ///\}
  private:
    param_type p;
  };



  /**
   *	@brief 26.5.8.2.2 Class template uniform_real_distribution [rand.dist.uni.real]
   **/
  template<class RealType>
  class uniform_real_distribution
  {
    static_assert(is_floating_point<RealType>::value, "RealType must be float type.");
  public:
    ///\name types
    typedef RealType result_type;
    typedef pair<RealType, RealType> param_type;

    ///\name constructors and reset functions
    explicit uniform_real_distribution(RealType a = 0.0, RealType b = 1.0)
      :p(a,b)
    {
      assert(a <= b);
    }
    explicit uniform_real_distribution(const param_type& parm)
      :p(parm)
    {}
    void reset()
    {}

    ///\name generating functions
    template<class URNG>
    result_type operator()(URNG& g)
    {
      this->operator()(g, param_type(0.0,1.0));
    }
    template<class URNG>
    result_type operator()(URNG& g, const param_type& parm);

    ///\name property functions
    result_type a()     const { return p.first;  }
    result_type b()     const { return p.second; }
    param_type  param() const { return p;        }

    void param(const param_type& parm) { p = parm; }
    
    result_type min() const { return p.first;  }
    result_type max() const { return p.second; }

    ///\name comparsion
    friend bool operator==(const uniform_real_distribution& x, const uniform_real_distribution& y) { return x.p == y.p; }
    friend bool operator!=(const uniform_real_distribution& x, const uniform_real_distribution& y) { return rel_ops::operator !=(x,y); }

    ///\name I/O support
    template<typename charT, typename traits>
    friend basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, const uniform_real_distribution& x)
    {
      saveiostate s(os); os.flags(ios_base::left|ios_base::scientific);
      return os << setfill(' ') << setprecision(numeric_limits<RealType>::digits10)
        << x.a() << ' ' << x.b();
    }
    template<typename charT, typename traits>
    friend basic_istream<charT, traits>& operator>>(basic_istream<charT, traits>& is, uniform_real_distribution& x)
    {
      saveiostate s(is); is.flags(ios_base::dec|ios_base::skipws);
      return is >> x.p.first >> x.p.second;
    }
    ///\}
  private:
    param_type p;
  };



  ///\name 26.5.8.3 Bernoulli distributions [rand.dist.bern]

  ///\name 26.5.8.4 Poisson distributions [rand.dist.pois]

  ///\name 26.5.8.5 Normal distributions [rand.dist.norm]

  /**
   *	@brief 26.5.8.5.1 Class template normal_distribution [rand.dist.norm.normal]
   **/
  template<class RealType>
  class normal_distribution
  {
    static_assert(is_floating_point<RealType>::value, "RealType must be float type.");
  public:
    ///\name types
    typedef RealType result_type;
    typedef pair<RealType, RealType> param_type;

    ///\name constructors and reset functions
    explicit normal_distribution(RealType mean = 0.0, RealType stddev = 1.0)
      :p(mean, stddev)
    {
      assert(stddev > RealType(0));
    }
    explicit normal_distribution(const param_type& parm)
      :p(parm)
    {}
    void reset()
    {
      res.second = false;
    }

    ///\name generating functions
    template<class URNG>
    result_type operator()(URNG& g)
    {
      return this->operator ()(g, param_type(0,1));
    }
    template<class URNG>
    result_type operator()(URNG& g, const param_type& parm)
    {
      result_type re;
      typedef result_type T;
      if(res.second){
        re = res.first;
        res.second = false;
      }else{
        __::uniform2real<URNG, T> rg(g);
        T x,y,r2;
        do{
          x = T(2) * rg() - 1;
          y = T(2) * rg() - 1;
          r2 = x*x + y*y;
        }while(r2 > 1 || r2 == 0);
        
        const T m = std::sqrt(-2 * std::log(r2) / r2);
        res.first = m * x,
          res.second = true;
        re = m * y;
      }
      re = re * parm.second + parm.first;
      return re;
    }

    ///\name property functions
    RealType   mean()   const { return p.first;  }
    RealType   stddev() const { return p.second; }
    param_type param()  const { return p;        }
    void param(const param_type& parm) { p = parm; }

    result_type min() const { return numeric_limits<result_type>::min(); }
    result_type max() const { return numeric_limits<result_type>::max(); }

    ///\name comparsion
    friend bool operator==(const normal_distribution& x, const normal_distribution& y)
    {
      if(x.p != y.p || x.res.second != y.res.second)
        return false;
      return x.res.second == false || x.res.first == y.res.first;
    }
    friend bool operator!=(const normal_distribution& x, const normal_distribution& y) { return rel_ops::operator !=(x,y); }

    ///\name I/O support
    template<typename charT, typename traits>
    friend basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, const normal_distribution& x)
    {
      saveiostate s(os); os.flags(ios_base::scientific|ios_base::left);
      const charT w = os.widen(' ');
      os<< setfill(' ') << setprecision(numeric_limits<RealType>::digits10) 
        << x.mean() << w
        << x.stddev() << w
        << x.res.second;
      if(x.res.second)
        os << w << x.res.first;
      return os;
    }
    template<typename charT, typename traits>
    friend basic_istream<charT, traits>& operator>>(basic_istream<charT, traits>& is, normal_distribution& x)
    {
      saveiostate s(is); is.flags(ios_base::dec|ios_base::skipws);
      is >> x.p.first >> x.p.second;
      is >> x.res.second;
      if(x.res.second)
        is >> x.res.first;
      return is;
    }
    ///\}
  private:
    param_type p;
    pair<result_type, bool> res;
  };


  ///\name 26.5.8.6 Sampling distributions [rand.dist.samp]

  /**@} lib_numeric_rand_dist */

  /**@} lib_numeric_rand */
  /**@} lib_numeric */
}
