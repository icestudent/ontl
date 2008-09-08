/**\file*********************************************************************
 *                                                                     \brief
 *  Time utilities [20.8 time]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_CHRONO
#define NTL__STLX_CHRONO

#include "ctime.hxx"
#include "type_traits.hxx"
#include "limits.hxx"
#include "ratio.hxx"

namespace std
{
/**\defgroup  lib_utilities *** 20 General utilities library [utilities] *****
 *
 *    Components used by other elements of the Standard C + + library.
 *@{
 **/

  namespace chrono
  {
    template <class Rep, class Period = ratio<1>> class duration;
    template <class Clock, class Duration = typename Clock::duration> class time_point;

    // Clocks
    class system_clock;

    /// convenience typedefs
    typedef duration<int64_t, nano>       nanoseconds;
    typedef duration<int64_t, micro>      microseconds;
    typedef duration<int64_t, milli>      milliseconds;
    typedef duration<int64_t>             seconds;
    typedef duration<int32_t, ratio< 60>> minutes;
    typedef duration<int32_t, ratio<3600>>hours;
  }

  // common_type traits
  /// Specializations of common_type [20.8.2.3 time.traits.specializations]
  template <class Rep1, class Period1, class Rep2, class Period2>
  struct common_type<chrono::duration<Rep1, Period1>, chrono::duration<Rep2, Period2>>
  {
    typedef chrono::duration<
      typename common_type<Rep1, Rep2>::type,
      ratio<
        detail::static_evaluation::gcd<Period1::num, Period2::num>::value,
        (Period1::den / detail::static_evaluation::gcd<Period1::den, Period2::den>::value) * Period2::den
           >
                            > type;
  };

  template <class Clock, class Duration1, class Duration2>
  struct common_type<chrono::time_point<Clock, Duration1>, chrono::time_point<Clock, Duration2>>
  {
    typedef chrono::time_point<Clock, typename common_type<Duration1, Duration2>::type> type;
  };


  namespace chrono
  {
    // customization traits
    /// is_floating_point [20.8.2.1 time.traits.is_fp]
    template <class Rep>
    struct treat_as_floating_point:
      is_floating_point<Rep>
    {};

    /// duration_values [20.8.2.2 time.traits.duration_values]
    template <class Rep>
    struct duration_values
    {
      static const Rep zero() { return Rep(0); }
      static const Rep min()  { return numeric_limits<Rep>::lowest(); }
      static const Rep max()  { return numeric_limits<Rep>::max(); }
    };


    /**
     *	@brief Class template duration [20.8.3 time.duration]
     *
     *  A \c duration type measures time between two points in time (\c time_points). A \c duration has a representation which
     *  holds a count of ticks and a tick period. The tick period is the amount of time which occurs from one tick to the next, in
     *  units of seconds. It is expressed as a rational constant using the template \ñ ratio.
     *
     *  @note \e Requires: \c Rep shall be an arithmetic type or a class emulating an arithmetic type.
     *  If a program instantiates duration with a duration type for the template argument Rep a diagnostic is required.
     *  \par \e Requires: Period shall be a specialization of ratio, diagnostic required.
     *  \par \e Requires: Period::num shall be positive, diagnostic required.
     *  \par \e Requires: Members of duration shall not throw exceptions other than those thrown by the indicated operations
     *  on their representations.
     *  @todo constexpr
     **/
    template <class Rep, class Period>
    class duration
    {
      template<class NotRatio>
      struct is_ratio: false_type
      {};
      template <ratio_t Num, ratio_t Den>
      struct is_ratio<ratio<Num, Den> >: true_type
      {};
    public:
      static_assert(is_arithmetic<Rep>::value, "Rep shall be an arithmetic type or a class emulating an arithmetic type");
      static_assert(is_ratio<Period>::value,   "Period shall be a specialization of ratio");
      static_assert(Period::num >= 0,          "Period::num shall be positive");

    public:
      typedef Rep rep;
      typedef Period period;

    public:
      // 20.8.3.1 construct/copy/destroy
      duration()
        :rep_(duration_values<rep>::zero())
      {}

      explicit duration(const Rep& r)
        :rep_(r)
      {}

      template <class Rep2>
      explicit duration(const Rep2& r)
        :rep_(static_cast<rep>(r))
      {
        static_assert((is_convertible<Rep2, rep>::value && (treat_as_floating_point<rep>::value || !treat_as_floating_point<Rep2>::value)), "20.8.3.1/6");
      }

      template <class Rep2, class Period2>
      duration(const duration<Rep2, Period2>& d)
        :rep_(duration_cast<duration>(d).count())
      {}

#ifdef NTL__CXX
      ~duration() = default;
#else
      // using the default destructor
      //~duration()
      //{}
#endif

      duration(const duration& d)
        :rep_(d.rep_)
      {}

      duration& operator=(const duration& d)
      {
        rep_ = d.rep_; return *this;
      }

      // 20.8.3.2 observer
      rep count() const { return rep_; }


      // 20.8.3.3 arithmetic
      duration operator+() const
      {
        return *this;
      }

      duration operator-() const
      {
        return duration(-rep_);
      }

      duration& operator++()
      {
        ++rep_;
        return *this;
      }

      duration operator++(int)
      {
        return duration(rep_++);
      }

      duration& operator--()
      {
        --rep_;
        return *this;
      }

      duration operator--(int)
      {
        return duration(rep_--);
      }

      duration& operator+=(const duration& d)
      {
        rep_ += d.rep_; return *this;
      }

      duration& operator-=(const duration& d)
      {
        rep_ -= d.rep_; return *this;
      }

      duration& operator*=(const rep& rhs)
      {
        rep_ *= rhs; return *this;
      }

      duration& operator/=(const rep& rhs)
      {
        rep_ /= rhs; return *this;
      }


      // 20.8.3.4 special values
      static const duration zero()
      {
        return duration(duration_values<rep>::zero());
      }

      static const duration min()
      {
        return duration(duration_values<rep>::min());
      }

      static const duration max()
      {
        return duration(duration_values<rep>::max());
      }
    private:
      rep rep_;
    };


    /// duration arithmetic
    template <class Rep, class Period>
    duration<Rep, Period>
      operator+ (const duration<Rep, Period>& lhs, const duration<Rep, Period>& rhs)
    {
      return duration<Rep, Period>(lhs) += rhs;
    }

    template <class Rep, class Period>
    duration<Rep, Period>
      operator- (const duration<Rep, Period>& lhs, const duration<Rep, Period>& rhs)
    {
      return duration<Rep, Period>(lhs) -= rhs;
    }

    template <class Rep1, class Period1, class Rep2, class Period2>
    typename common_type<duration<Rep1, Period1>, duration<Rep2, Period2>>::type
      operator+ (const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
    {
      typedef typename common_type<duration<Rep1, Period1>, duration<Rep2, Period2>>::type CD;
      return CD(lhs) += rhs;
    }

    template <class Rep1, class Period1, class Rep2, class Period2>
    typename common_type<duration<Rep1, Period1>, duration<Rep2, Period2>>::type
      operator- (const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
    {
      typedef typename common_type<duration<Rep1, Period1>, duration<Rep2, Period2>>::type CD;
      return CD(lhs) -= rhs;
    }

    template <class Rep1, class Period, class Rep2>
    duration<typename common_type<Rep1, Rep2>::type, Period>
      operator* (const duration<Rep1, Period>& d, const Rep2& s)
    {
      // TODO: CR existence diagnostics
      typedef typename common_type<Rep1, Rep2>::type CR;
      return duration<CR, Period>(d) *= s;
    }

    template <class Rep1, class Period, class Rep2>
    duration<typename common_type<Rep1, Rep2>::type, Period>
      operator* (const Rep1& s, const duration<Rep2, Period>& d)
    {
      // TODO: CR existence diagnostics
      return d * s;
    }

    template <class Rep1, class Period, class Rep2>
    duration<typename common_type<Rep1, Rep2>::type, Period>
      operator/ (const duration<Rep1, Period>& d, const Rep2& s)
    {
      return duration<typename common_type<Rep1, Rep2>::type, Period>(d) /= s;
    }

    template <class Rep1, class Period1, class Rep2, class Period2>
    typename common_type<Rep1, Rep2>::type
      operator/ (const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
    {
      typedef typename common_type<duration<Rep1, Period1>, duration<Rep2, Period2>>::type CD;
      return CD(lhs).count() / CD(rhs).count();
    }


    /// duration comparisons
    template <class Rep1, class Period1, class Rep2, class Period2>
    bool operator==(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
    {
      typedef typename common_type<const duration<Rep1, Period1>, const duration<Rep2, Period2>>::type CT;
      return CT(lhs).count() == CT(rhs).count();
    }

    template <class Rep1, class Period1, class Rep2, class Period2>
    bool operator!=(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
    {
      return !(lhs == rhs);
    }

    template <class Rep1, class Period1, class Rep2, class Period2>
    bool operator< (const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
    {
      typedef typename common_type<const duration<Rep1, Period1>, const duration<Rep2, Period2>>::type CT;
      return CT(lhs).count() < CT(rhs).count();
    }

    template <class Rep1, class Period1, class Rep2, class Period2>
    bool operator<=(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
    {
      return !(rhs < lhs);
    }

    template <class Rep1, class Period1, class Rep2, class Period2>
    bool operator> (const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
    {
      return rhs < lhs;
    }

    template <class Rep1, class Period1, class Rep2, class Period2>
    bool operator>=(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
    {
      return !(lhs < rhs);
    }

    namespace detail {

      template<bool num_is_one, bool den_is_one>
      struct duration_cast_impl
      {
        template<class ToDuration, typename CF, typename CR, class Rep, class Period>
        static ToDuration cast(const duration<Rep, Period>& d)
        {
          return ToDuration(static_cast<typename ToDuration::rep>(static_cast<CR>(d.count()) * static_cast<CR>(CF::num) / static_cast<CR>(CF::den)));
        }
      };

      template<>
      struct duration_cast_impl<true, true>
      {
        template<class ToDuration, typename CF, typename CR, class Rep, class Period>
        static ToDuration cast(const duration<Rep, Period>& d)
        {
          return ToDuration(static_cast<typename ToDuration::rep>(d.count()));
        }
      };

      template<>
      struct duration_cast_impl<false, true>
      {
        template<class ToDuration, typename CF, typename CR, class Rep, class Period>
        static ToDuration cast(const duration<Rep, Period>& d)
        {
          return ToDuration(static_cast<typename ToDuration::rep>(static_cast<CR>(d.count()) * static_cast<CR>(CF::num)));
        }
      };

      template<>
      struct duration_cast_impl<true, false>
      {
        template<class ToDuration, typename CF, typename CR, class Rep, class Period>
        static ToDuration cast(const duration<Rep, Period>& d)
        {
          return ToDuration(static_cast<typename ToDuration::rep>(static_cast<CR>(d.count()) / static_cast<CR>(CF::den)));
        }
      };
    }

    /**
     *	@brief duration_cast [20.8.3.7 time.duration.cast]
     *
     *  @note \e Requires: ToDuration shall be an instantiation of duration. Diagnostic required.
     *
     *  @remarks  This function shall not rely on any implicit conversions. All conversions shall be accomplished through static_cast.
     *  The implementation shall avoid all multiplications or divisions when it is known at compile time
     *  that they can be avoided because one or more arguments are 1. All intermediate computations shall be carried out
     *  in the widest possible representation and only converted to the destination representation at the final step.
     **/
    template <class ToDuration, class Rep, class Period>
    ToDuration duration_cast(const duration<Rep, Period>& d)
    {
      typedef typename ratio_divide<Period, typename ToDuration::period>::type    CF;
      typedef typename common_type<typename ToDuration::rep, Rep, intmax_t>::type CR;

      return detail::duration_cast_impl<CF::num == 1, CF::den == 1>::cast<ToDuration, CF, CR>(d);
    }



    /**
     *	@brief Class template time_point [20.8.4 time.point]
     *  A time_point represents a point in time, as opposed to a duration of time.
     *  Another way of saying the same thing, is that a time_point represents an epoch plus or minus a duration.

     *  @tparam Clock shall meet the Clock requirements
     *  @tparam Duration shall be an instance of duration.
     **/
    template <class Clock, class Duration>
    class time_point
    {
      template<class NotDuration>
      struct is_duration: false_type
      {};
      template <class Rep, class Period>
      struct is_duration<duration<Rep, Period> >: true_type
      {};
    public:
      static_assert(is_duration<Duration>::value, "Duration shall be an instance of duration");

    public:
      typedef Clock clock;
      typedef Duration duration;
      typedef typename duration::rep rep;
      typedef typename duration::period period;
    public:
      // 20.8.4.1 Construct
      time_point()
      {}

      explicit time_point(const typename time_point<Clock, Duration>::duration& d)
      :d(d)
      {}

      template <class Duration2>
      time_point(const time_point<clock, Duration2>& t)
        :d(static_cast<duration>(t.time_since_epoch()))
      {}

      // 20.8.4.2 observer
      duration time_since_epoch() const { return d; }

      // 20.8.4.3 arithmetic
      time_point& operator+=(const duration& d)
      {
        this->d += d; return *this;
      }

      time_point& operator-=(const duration& d)
      {
        this->d -= d; return *this;
      }

      // 20.8.4.4 special values
      static constexpr time_point min() { return time_point(duration::min()); }
      static constexpr time_point max() { return time_point(duration::max()); }
    private:
      duration d;
    };


    /// time_point arithmetic
    template <class Clock, class Duration1, class Rep2, class Period2>
    time_point<Clock, typename common_type<Duration1, duration<Rep2, Period2>>::type>
      operator+ (const time_point<Clock, Duration1>& lhs, const duration<Rep2, Period2>& rhs)
    {
      typedef typename time_point<Clock, typename common_type<Duration1, duration<Rep2, Period2>>::type> CT;
      return CT(lhs) += rhs;
    }

    template <class Rep1, class Period1, class Clock, class Duration2>
    time_point<Clock, typename common_type<duration<Rep1, Period1>, Duration2>::type>
      operator+ (const duration<Rep1, Period1>& lhs, const time_point<Clock, Duration2>& rhs)
    {
      return rhs + lhs;
    }

    template <class Clock, class Duration1, class Rep2, class Period2>
    time_point<Clock, typename common_type<Duration1, duration<Rep2, Period2>>::type>
      operator- (const time_point<Clock, Duration1>& lhs, const duration<Rep2, Period2>& rhs)
    {
      return lhs + (-rhs);
    }

    template <class Clock, class Duration1, class Duration2>
    typename common_type<Duration1, Duration2>::type
      operator- (const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs)
    {
      return lhs.time_since_epoch() - rhs.time_since_epoch();
    }


    /// time_point comparisons
    template <class Clock, class Duration1, class Duration2>
    bool operator==(const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs)
    {
      return lhs.time_since_epoch() == rhs.time_since_epoch();
    }

    template <class Clock, class Duration1, class Duration2>
    bool operator!=(const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs)
    {
      return !(lhs == rhs);
    }

    template <class Clock, class Duration1, class Duration2>
    bool operator< (const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs)
    {
      return lhs.time_since_epoch() < rhs.time_since_epoch();
    }

    template <class Clock, class Duration1, class Duration2>
    bool operator<=(const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs)
    {
      return !(rhs < lhs);
    }

    template <class Clock, class Duration1, class Duration2>
    bool operator> (const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs)
    {
      return rhs < lhs;
    }

    template <class Clock, class Duration1, class Duration2>
    bool operator>=(const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs)
    {
      return !(lhs < rhs);
    }


    /**
     *	@brief time_point_cast [20.8.4.7 time.point.cast]
     *  @note \e Requires: ToDuration shall be an instance of duration.
     **/
    template <class ToDuration, class Clock, class Duration>
    time_point<Clock, ToDuration> time_point_cast(const time_point<Clock, Duration>& t)
    {
      return time_point<Clock, ToDuration>(duration_cast<ToDuration>(t.time_since_epoch()));
    }


    /************************************************************************/
    /* Clocks                                                               */
    /************************************************************************/

    /**
     *	@brief Class system_clock [20.8.5.1 time.clock.system]
     *  Objects of class system_clock represent wall clock time from the system-wide realtime clock.
     **/
    class system_clock
    {
    public:
      typedef int64_t rep;

      // The system clock time stored as 100 nanoseconds units.
      // it customizable from 0,001 to 0,015625 sec (1 - 15,6 ms),
      // usualy 976,5us (microseconds), so an approximate value (10ms) was choosen.
      typedef ratio_multiply<ratio<10>, milli>::type  period;

      typedef chrono::duration<rep, period>           duration;
      typedef chrono::time_point<system_clock>        time_point;

      // NOTE: see the Table 54 (20.8.1), system time can be adjusted back.
      static const bool is_monotonic = false;
    public:
#ifdef NTL__CXX
      static_assert(duration::min() < duration::zero(), "20.8.5.1.2");
#else
      static_assert(numeric_limits<duration::rep>::is_signed, "20.8.5.1.2");
#endif

      /** \c return the time_point representing a current date and time */
      static time_point now()
      {
        typedef ratio_multiply<ratio<100>, nano>::type systime_unit;
        typedef chrono::duration<ntl::nt::systime_t, systime_unit> systime_duration;
        STATIC_ASSERT(systime_unit::den == 10000000); // 100ns is a 10^-7 of second
        STATIC_ASSERT(period::den == 100); // 10ms is a 10^-2 (1/100) of second
        STATIC_ASSERT(( ratio_divide<period, systime_unit>::type::num == 100000 )); // 10ms is in 100 000 times greater than 100ns

        const ntl::nt::systime_t ntime = ntl::nt::query_system_time();
        //const systime_duration sd(ntime);
        //const duration d = duration_cast<duration>(sd);
        //const time_point tp(d);
        //return tp;
        return time_point( duration_cast<duration>(systime_duration(ntime)) );
      }

      /**
       *  A \c time_t object that represents the same point in time as \c t when both values are truncated to the coarser
       *  of the precisions of \c time_t and time_point.
       *  @note Precision of \c time_t less than precision of time_point.
       **/
      static time_t to_time_t (const time_point& t)
      {
        // Number of seconds from 1/1/1601 to 1/1/1970 in 10ms units (see system_clock::period) (10ms = 1 centisecond)
        typedef ratio_divide<ratio_multiply<ratio<116444736>, hecto>::type, centi>::type epochdiff_t;
        STATIC_ASSERT(epochdiff_t::num == 1164447360000i64);

        const system_clock::duration diff(epochdiff_t::num);
        return duration_cast<chrono::seconds>(t.time_since_epoch() - diff).count();
      }

      /**
       *  A \c time_point object that represents the same point in time as \c t when both values are truncated to the coarser
       *  of the precisions of \c time_t and time_point.
       *  @note Precision of \c time_t less than precision of time_point.
       **/
      static time_point from_time_t(time_t t)
      {
        // Number of seconds from 1/1/1601 to 1/1/1970 in 10ms units (see system_clock::period) (10ms = 1 centisecond)
        typedef ratio_divide<ratio_multiply<ratio<116444736>, hecto>::type, centi>::type epochdiff_t;
        STATIC_ASSERT(epochdiff_t::num == 1164447360000i64);
        typedef chrono::time_point<system_clock, chrono::seconds> seconds_time_point;
        const system_clock::duration diff(epochdiff_t::num);

        // NOTE: c++ recognizes `seconds_time_point from_tp(chrono::seconds(t))` as function declaration, so we need an alternate way:
        //  const seconds_time_point from_tp((chrono::seconds(t))) OR
        const seconds_time_point from_tp(static_cast<chrono::seconds>(t));
        return time_point(duration_cast<duration>(from_tp.time_since_epoch()) + diff);
      }
    };



    /**
     *	@brief Class monotonic_clock [20.8.5.2 time.clock.monotonic]
     *  Objects of class monotonic_clock represent clocks for which values of time_point never decrease as physical time
     *  advances.
     *
     *  @note It is monotonic non-decreasing clock (see the http://www.rtsj.org/specjavadoc/timers_overview-summary.html for information)
     **/
    class monotonic_clock
    {
    public:
      typedef int64_t rep;

      // period is 10 ms
      typedef ratio_multiply<ratio<10>, milli>::type  period;
      typedef chrono::duration<rep, period>           duration;

      typedef chrono::time_point<monotonic_clock>     time_point;

      static const bool is_monotonic = true;
    public:
#ifdef NTL__CXX
      static_assert(duration::min() < duration::zero(), "20.8.5.1.2");
#else
      static_assert(numeric_limits<duration::rep>::is_signed, "20.8.5.1.2");
#endif

      /** \c return the time_point representing a current monotonic time */
      static time_point now()
      {
        typedef ratio_multiply<ratio<100>, nano>::type systime_unit;
        typedef chrono::duration<ntl::nt::systime_t, systime_unit> systime_duration;
        STATIC_ASSERT(systime_unit::den == 10000000); // 100ns is a 10^-7 of second
        STATIC_ASSERT(period::den == 100); // 10ms is a 10^-2 (1/100) of second
        STATIC_ASSERT(( ratio_divide<period, systime_unit>::type::num == 100000 )); // 10ms is in 100 000 times greater than 100ns

        const ntl::nt::systime_t ntime = ntl::user_shared_data::instance().InterruptTime.get();
        return time_point( duration_cast<duration>(systime_duration(ntime)) );
      }
    };

#if 0
    /**
     *	@brief Class high_resolution_clock [20.8.5.3 time.clock.hires]
     *  Objects of class high_resolution_clock represent clocks with the shortest tick period.
     **/
    class high_resolution_clock
    {
    public:
      typedef uint64_t rep;

      typedef ratio<unspecified, unspecified> period;
      typedef chrono::duration<rep, period> duration;
      typedef chrono::time_point<unspecified, duration> time_point;

      static const bool is_monotonic = true;
    public:
      /** \c return the time_point representing a current date and time */
      static time_point now();
    };
#else
    class monotonic_clock;
    class high_resolution_clock;
#endif
    } // namespace chrono

  /**@} lib_utilities */
} // namespace std

#endif // NTL__STLX_CHRONO
