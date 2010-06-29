/**\file*********************************************************************
 *                                                                     \brief
 *  Atomic operations library [29 atomics]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_ATOMIC
#define NTL__STLX_ATOMIC
#pragma once

#include "../atomic.hxx"
#include "cassert.hxx"

namespace std
{
/**\defgroup lib_atomic *** 29 Atomic operations library [atomics]
 *  Components for fine-grained %atomic access. This access is provided via operations on atomic objects.
 * @{
 **/


  /**\defgroup lib_atomic_3 29.3 Order and Consistency [atomics.order]
   *@{
   **/
  enum memory_order
  {
    /** the operation does not order memory */
    memory_order_relaxed,

    /** the operation performs a consume operation on the affected memory loca-
      tion, thus making regular memory writes in other threads released through
      the atomic variable to which it is applied visible to the regular memory
      reads that are dependencies of this consume operation */
    memory_order_consume,
    
    /** the operation performs an acquire operation on the affected memory lo
      tion, thus making regular memory writes in other threads released throu
      the atomic variable to which it is applied visible to the current thread */
    memory_order_acquire,
    
    /** the operation performs a release operation on the affected memory location
      thus making regular memory writes visible to other threads through the
      atomic variable to which it is applied */
    memory_order_release,
    
    /** the operation has both acquire and release semantics */
    memory_order_acq_rel,
    
    /** the operation has both acquire and release semantics, and, in addition, has
      sequentially-consistent operation ordering */
    memory_order_seq_cst
  };

  /**
   *	The argument does not carry a dependency to the return value
   *  @return y
   **/
  template <class T>
  inline T kill_dependency(T y) { return y; }
  /** @} lib_atomic_3 */


  namespace __atomic_v1
  {
    namespace atomic = ntl::atomic;
    namespace intrin = ntl::intrinsic;

#pragma warning(push)
#pragma warning(disable: 4146) // unary minus operator applied to unsigned type, result still unsigned

    /// Base class for %atomic type
    template<typename T, bool fundamental>
    struct base;

    template<typename T, bool small = (sizeof(T) <= sizeof(uint32_t))>
    struct rep
    {
      typedef uint32_t type;
      __forceinline operator T() const volatile { return static_cast<T>(val);  }
      __forceinline T operator=(T v) volatile   { val =  static_cast<type>(v); return v; }
      __forceinline static T cast(type v){ return static_cast<T>(v); }
      uint32_t val;
    };

    template<typename T>
    struct rep<T, false>
    {
      typedef uint64_t type;
      __forceinline operator T() const volatile { return static_cast<T>(val);  }
      __forceinline T operator=(T v) volatile   { val =  static_cast<type>(v); return v; }
      __forceinline static T cast(type v){ return static_cast<T>(v); }
      uint64_t val;
    };

    template<>
    struct rep<bool, true>
    {
      typedef uint32_t type;
      operator bool() const volatile { return val != 0; }
      bool operator=(bool v) volatile{ val = v != 0; return v; }
      __forceinline static bool cast(type v){ return v != 0; }
      uint32_t val;
    };

    template<>
    struct rep<void*,(sizeof(void*) == sizeof(uint32_t))> /*ST: why pointer is not a small type on x64?*/
    {
      typedef uint32_t type;
      __forceinline operator void*() const volatile { return reinterpret_cast<void*>(val); }
      __forceinline void* operator=(void* p)volatile{ val = reinterpret_cast<type>(p); return p; }
      __forceinline static void* cast(type v) { return reinterpret_cast<void*>(v); }
      __forceinline static type  cast(void*v) { return reinterpret_cast<type >(v); }
      uint32_t val;
    };

    /// Base class for %atomic fundamental type
    template<typename T>
    struct base<T, true>
    {
      typedef T type;
    protected:
      typedef typename rep<T>::type reptype;
      static const bool lock_free = true;

    public:
      ///\name 29.6 Operations on Atomic Types [atomics.types.operations]

      /** If the object’s operations are lock-free. */
      bool is_lock_free() const volatile { return lock_free; }

      /** Returns the result of load(). */
      operator type() const volatile { return load(); }

      void swap(volatile base& x) volatile
      {
        exchange(x.val);
      }
    public:
      /** Atomically replaces the value pointed to by object or by this with the \p value. Memory is affected according to the value of \p mo. */
      void store(type value, memory_order mo = memory_order_seq_cst) volatile
      {
        switch(mo)
        {
        case memory_order_seq_cst:
          atomic::exchange(val.val, static_cast<reptype>(value));
          break;
        case memory_order_release:
          intrin::_ReadWriteBarrier();
          val = value;
          break;
        default:
          _assert_msg("invalid memory order specified");
        case memory_order_relaxed:
          val = value;
        }
      }

      /** Atomically returns the value pointed to by \c this.
        \pre The \p mo argument shall not be \c memory_order_release nor \c memory_order_acq_rel. */
      type load(memory_order mo = memory_order_seq_cst) const volatile
      {
        switch(mo)
        {
        case memory_order_seq_cst:
        case memory_order_acquire:
        case memory_order_consume:
          {
            const type r = val;
            intrin::_ReadWriteBarrier();
            return r;
          }
        default:
          _assert_msg("invalid memory order specified");
        case memory_order_relaxed:
          return val;
        }
      }

      /** Atomically replaces the value pointed to by \c this with \p value. Memory is affected according to the value of \p mo. 
        These operations are %atomic read-modify-write operations.
        \return Atomically returns the value pointed to by this immediately before the effects.*/
      type exchange(type value, memory_order mo = memory_order_seq_cst) volatile
      {
        switch(mo)
        {
        case memory_order_release:
          intrin::_ReadWriteBarrier();
          return rep<type>::cast(atomic::exchange(val.val, static_cast<reptype>(value)));
        case memory_order_acquire:
        case memory_order_consume:
          {const type r = rep<type>::cast(atomic::exchange(val.val, static_cast<reptype>(value)));
          intrin::_ReadWriteBarrier();
          return r;}
        default:
          _assert_msg("invalid memory order specified");
        case memory_order_relaxed:
        case memory_order_seq_cst:
        case memory_order_acq_rel:
          return rep<type>::cast(atomic::exchange(val.val, static_cast<reptype>(value)));
        }
      }

      /**
       *	@brief Atomically, compares the value pointed to by \c this for equality with that in \p expected, and if \c true, 
       *  replaces the value pointed to by this with \p desired, and if \c false, updates the value in \p expected with the value pointed to by \c this.
       *
       *  Further, if the comparison is \c true, memory is affected according to the value of \p success, and if the comparison is \c false, 
       *  memory is affected according to the value of \p failure. When only one memory_order argument is supplied, the value of \p success is \p order, 
       *  and the value of \p failure is \p order except that a value of \c memory_order_acq_rel shall be replaced by the value \c memory_order_acquire 
       *  and a value of \c memory_order_release shall be replaced by the value \c memory_order_relaxed. 
       *
       *  These operations are %atomic read-modify-write operations.
       *
       *  \pre The \p failure argument shall not be \c memory_order_release nor \c memory_order_acq_rel. The \p failure argument shall be no stronger than the \p success argument.
       **/
      bool compare_exchange_strong(type& expected, type desired, memory_order /*success*/, memory_order /*failure*/) volatile
      {
        const type old = rep<type>::cast(atomic::compare_exchange(val.val, desired, expected));
        if(old != expected){
          intrin::_ReadWriteBarrier();
          expected = old;
        }
        return old == expected;
      }
      /** \copydoc compare_exchange_strong() */
      bool compare_exchange_strong(type& expected, type desired, memory_order order = memory_order_seq_cst) volatile
      {
        return compare_exchange_strong(expected, desired, order, order);
      }
      /** \copydoc compare_exchange_strong() */
      bool compare_exchange_weak(type& expected, type desired, memory_order success, memory_order failure) volatile
      {
        return compare_exchange_strong(expected, desired, success, failure);
      }
      /** \copydoc compare_exchange_strong() */
      bool compare_exchange_weak(type& expected, type desired, memory_order order = memory_order_seq_cst) volatile
      {
        return compare_exchange_strong(expected, desired, order, order);
      }
      ///\}
    protected:
      type x_add(type operand, memory_order order) volatile
      {
        switch(order)
        {
        case memory_order_release:
          intrin::_ReadWriteBarrier();
          return rep<type>::cast(atomic::exchange_add(val.val, operand));
        case memory_order_acquire:
        case memory_order_consume:
          {const type r = rep<type>::cast(atomic::exchange_add(val.val, operand));
          intrin::_ReadWriteBarrier();
          return r;}
        default:
          _assert_msg("invalid memory order specified");
        case memory_order_seq_cst:
        case memory_order_acq_rel:
        case memory_order_relaxed:
          return rep<type>::cast(atomic::exchange_add(val.val, operand));
        }
      }
    protected:
      rep<T> val;
    };

    template<>
    struct base<void*, true>
    {
      typedef void* type;
    protected:
      typedef rep<type>::type reptype;
      static const bool lock_free = true;

    public:
      ///\name 29.6 Operations on Atomic Types [atomics.types.operations]

      /** If the object’s operations are lock-free. */
      bool is_lock_free() const volatile { return lock_free; }

      /** Returns the result of load(). */
      operator type() const volatile { return load(); }

      void swap(volatile base& x) volatile
      {
        exchange(x.val);
      }
    public:
      /** Atomically replaces the value pointed to by object or by this with the \p value. Memory is affected according to the value of \p mo. */
      void store(type value, memory_order mo = memory_order_seq_cst) volatile
      {
        switch(mo)
        {
        case memory_order_seq_cst:
          intrin::_ReadWriteBarrier();
          atomic::exchange(val.val, reinterpret_cast<reptype>(value));
          intrin::_ReadWriteBarrier();
          break;
        case memory_order_release:
          intrin::_ReadWriteBarrier();
          val = value;
          break;
        default:
          _assert_msg("invalid memory order specified");
        case memory_order_relaxed:
          val = value;
        }
      }

      /** Atomically returns the value pointed to by \c this.
      \pre The \p mo argument shall not be \c memory_order_release nor \c memory_order_acq_rel. */
      type load(memory_order mo = memory_order_seq_cst) const volatile
      {
        switch(mo)
        {
        case memory_order_seq_cst:
        case memory_order_acquire:
        case memory_order_consume:
          {
            const type r = val;
            intrin::_ReadWriteBarrier();
            return r;
          }
        default:
          _assert_msg("invalid memory order specified");
        case memory_order_relaxed:
          return val;
        }
      }

      /** Atomically replaces the value pointed to by \c this with \p value. Memory is affected according to the value of \p mo. 
      These operations are %atomic read-modify-write operations.
      \return Atomically returns the value pointed to by this immediately before the effects.*/
      type exchange(type value, memory_order mo = memory_order_seq_cst) volatile
      {
        switch(mo)
        {
        case memory_order_seq_cst:
        case memory_order_acq_rel:
          {intrin::_ReadWriteBarrier();
          const type r = rep<type>::cast(atomic::exchange(val.val, reinterpret_cast<reptype>(value)));
          intrin::_ReadWriteBarrier();
          return r;}
        case memory_order_release:
          intrin::_ReadWriteBarrier();
          return rep<type>::cast(atomic::exchange(val.val, reinterpret_cast<reptype>(value)));
        case memory_order_acquire:
        case memory_order_consume:
          {const type r = rep<type>::cast(atomic::exchange(val.val, reinterpret_cast<reptype>(value)));
          intrin::_ReadWriteBarrier();
          return r;}
        default:
          _assert_msg("invalid memory order specified");
        case memory_order_relaxed:
          return rep<type>::cast(atomic::exchange(val.val, reinterpret_cast<reptype>(value)));
        }
      }

      /**
      *	@brief Atomically, compares the value pointed to by \c this for equality with that in \p expected, and if \c true, 
      *  replaces the value pointed to by this with \p desired, and if \c false, updates the value in \p expected with the value pointed to by \c this.
      *
      *  Further, if the comparison is \c true, memory is affected according to the value of \p success, and if the comparison is \c false, 
      *  memory is affected according to the value of \p failure. When only one memory_order argument is supplied, the value of \p success is \p order, 
      *  and the value of \p failure is \p order except that a value of \c memory_order_acq_rel shall be replaced by the value \c memory_order_acquire 
      *  and a value of \c memory_order_release shall be replaced by the value \c memory_order_relaxed. 
      *
      *  These operations are %atomic read-modify-write operations.
      *
      *  \pre The \p failure argument shall not be \c memory_order_release nor \c memory_order_acq_rel. The \p failure argument shall be no stronger than the \p success argument.
      **/
      bool compare_exchange_strong(type& expected, type desired, memory_order /*success*/, memory_order /*failure*/) volatile
      {
        const type old = rep<type>::cast(atomic::compare_exchange(val.val, reinterpret_cast<uintptr_t>(desired), reinterpret_cast<uintptr_t>(expected)));
        if(old != expected)
          expected = old;
        return old == expected;
      }
      /** \copydoc compare_exchange_strong() */
      bool compare_exchange_strong(type& expected, type desired, memory_order order = memory_order_seq_cst) volatile
      {
        return compare_exchange_strong(expected, desired, order, order);
      }
      /** \copydoc compare_exchange_strong() */
      bool compare_exchange_weak(type& expected, type desired, memory_order success, memory_order failure) volatile
      {
        return compare_exchange_strong(expected, desired, success, failure);
      }
      /** \copydoc compare_exchange_strong() */
      bool compare_exchange_weak(type& expected, type desired, memory_order order = memory_order_seq_cst) volatile
      {
        return compare_exchange_strong(expected, desired, order, order);
      }
      ///\}
    protected:
      type x_add(type operand, memory_order order) volatile
      {
        switch(order)
        {
        case memory_order_seq_cst:
        case memory_order_acq_rel:
          return reinterpret_cast<type>(atomic::exchange_add(val.val, reinterpret_cast<uintptr_t>(operand)));
        case memory_order_release:
          intrin::_ReadWriteBarrier();
          return reinterpret_cast<type>(atomic::exchange_add(val.val, reinterpret_cast<uintptr_t>(operand)));
        case memory_order_acquire:
        case memory_order_consume:
          {const type r = reinterpret_cast<type>(atomic::exchange_add(val.val, reinterpret_cast<uintptr_t>(operand)));
          intrin::_ReadWriteBarrier();
          return r;}
        default:
          _assert_msg("invalid memory order specified");
        case memory_order_relaxed:
          return reinterpret_cast<type>(atomic::exchange_add(val.val, reinterpret_cast<uintptr_t>(operand)));
        }
      }
    protected:
      rep<type> val;
    };



    /// Base class for %atomic integral type
    template<typename T>
    struct integral:
      public base<T, true>
    {
      using base<T, true>::type;

      ///\name arithmetic computations

      /**
       *	@brief Atomically replaces the value pointed to by \c this with the result of the computation 
       *  applied to the value pointed to by \c this and the given \p operand. 
       *  @details Memory is affected according to the value of \p mo. These operations are %atomic read-modify-write operations.
       *  @return Atomically, the value pointed to by \c this immediately before the effects.
       *  @note For signed integral types, arithmetic is defined to use two's complement representation. There are no undefined results.
       **/
      type fetch_add(type operand, memory_order mo = memory_order_seq_cst) volatile
      {
        return this->x_add(operand, mo);
      }
      /** \copydoc fetch_add() */
      type fetch_sub(type operand, memory_order mo = memory_order_seq_cst) volatile
      {
        return this->x_add(-operand, mo);
      }
      /** \copydoc fetch_add() */
      type fetch_and(type operand, memory_order mo = memory_order_seq_cst) volatile
      {
        switch(mo)
        {
        case memory_order_release:
          intrin::_ReadWriteBarrier();
          return rep<type>::cast(atomic::bit_and(this->val.val, operand));
        case memory_order_acquire:
        case memory_order_consume:
          {const type r = rep<type>::cast(atomic::bit_and(this->val.val, operand));
          intrin::_ReadWriteBarrier();
          return r;}
        default:
          _assert_msg("invalid memory order specified");
        case memory_order_relaxed:
        case memory_order_seq_cst:
        case memory_order_acq_rel:
          return rep<type>::cast(atomic::bit_and(this->val.val, operand));
        }
      }
      /** \copydoc fetch_add() */
      type fetch_or (type operand, memory_order mo = memory_order_seq_cst) volatile
      {
        switch(mo)
        {
        case memory_order_release:
          intrin::_ReadWriteBarrier();
          return rep<type>::cast(atomic::bit_or(this->val.val, operand));
        case memory_order_acquire:
        case memory_order_consume:
          {const type r = rep<type>::cast(atomic::bit_or(this->val.val, operand));
          intrin::_ReadWriteBarrier();
          return r;}
        default:
          _assert_msg("invalid memory order specified");
        case memory_order_seq_cst:
        case memory_order_acq_rel:
        case memory_order_relaxed:
          return rep<type>::cast(atomic::bit_or(this->val.val, operand));
        }
      }
      /** \copydoc fetch_add() */
      type fetch_xor(type operand, memory_order mo = memory_order_seq_cst) volatile
      {
        switch(mo)
        {
        case memory_order_release:
          intrin::_ReadWriteBarrier();
          return rep<type>::cast(atomic::bit_xor(this->val.val, operand));
        case memory_order_acquire:
        case memory_order_consume:
          {const type r = rep<type>::cast(atomic::bit_xor(this->val.val, operand));
          intrin::_ReadWriteBarrier();
          return r;}
        default:
          _assert_msg("invalid memory order specified");
        case memory_order_seq_cst:
        case memory_order_acq_rel:
        case memory_order_relaxed:
          return rep<type>::cast(atomic::bit_xor(this->val.val, operand));
        }
      }

    public:
      ///\name operator arithmetic computations

      /** Stores and returns \p value. */
      type operator=(type value) volatile
      {
        store(value);
        return value;
      }

      /** Returns <tt>fetch_add(1)</tt> */
      type operator++(int) volatile
      {
        return fetch_add(1);
      }
      /** Returns <tt>fetch_sub(1)</tt> */
      type operator--(int) volatile
      {
        return fetch_sub(1);
      }
      /** Returns <tt>fetch_add(1) + 1</tt> */
      type operator++() volatile
      {
        return fetch_add(1)+1;
      }
      /** Returns <tt>fetch_sub(1) - 1</tt> */
      type operator--() volatile
      {
        return fetch_sub(1)-1;
      }

      /** Returns <tt>fetch_key(operand) \e op operand</tt> */
      type operator+=(type operand) volatile
      {
        return fetch_add(operand)+operand;
      }
      /** Returns <tt>fetch_key(operand) \e op operand</tt> */
      type operator-=(type operand) volatile
      {
        return fetch_sub(operand)+operand;
      }
      /** Returns <tt>fetch_key(operand) \e op operand</tt> */
      type operator&=(type operand) volatile
      {
        return fetch_and(operand)& operand;
      }
      /** Returns <tt>fetch_key(operand) \e op operand</tt> */
      type operator|=(type operand) volatile
      {
        return fetch_or(operand)& operand;
      }
      /** Returns <tt>fetch_key(operand) \e op operand</tt> */
      type operator^=(type operand) volatile
      {
        return fetch_xor(operand)^operand;
      }
    };
#pragma warning(pop)
  } // atomic_v1


  /**\defgroup lib_atomic_4 29.4 Lock-free Property [atomics.lockfree]
   *@{
   **/
  // 0 - never lock free, 1 - sometimes lock free, 2 - always lock-free
  /** The macros indicate the general lock-free property of integral %atomic type. */
#define ATOMIC_INTEGRAL_LOCK_FREE 2
  /** The macros indicate the general lock-free property of address %atomic type. */
#define ATOMIC_ADDRESS_LOCK_FREE 2
/** @} lib_atomic_4 */

  /**\defgroup lib_atomic_5 29.5 Atomic Types [atomics.types]
   *@{
   **/
  /**\defgroup lib_atomic_51 29.5.1 Integral Types [atomics.types.integral]
   *  The %atomic integral types shall have standard layout. They shall each have a trivial default constructor, a
   *  constexpr value constructor, a deleted copy constructor, a deleted copy assignment operator, and a trivial
   *  destructor. They shall each support aggregate initialization syntax.
   *
   *  @note Due to limitations of compiler (if it isn't support the \e constexpr and <i>default/deleted functions</i>),
   *  %atomic integral types doesn't have a trivial default constructor and aggregate initialization syntax.
   *@{
   **/

  /**
   *	@brief The atomic_bool type provides an %atomic boolean
   **/
  struct atomic_bool:
    public __atomic_v1::base<bool, true>
  {
  #ifdef NTL__CXX_EF
    atomic_bool() = default;
  #else
    /** Initializes the object with the default value. */
    constexpr atomic_bool()
    {
      val = false;
    }
  #endif
    /** Initializes the object with the \p value.
      \note explicit because there is an ambigous here: <tt>atomic_bool f; f = !f;</tt> */
    explicit constexpr atomic_bool(bool value)
    {
      val = value;
    }

    /** Stores and returns \p value. */
    bool operator=(bool value) volatile
    {
      store(value);
      return value;
    }
  private:
    atomic_bool(const atomic_bool&) __deleted;
    atomic_bool& operator=(const atomic_bool&) __deleted;
  };

  inline bool atomic_is_lock_free(const volatile atomic_bool* object) { return object->is_lock_free(); }
  inline void atomic_store(volatile atomic_bool* object, bool value) { object->store(value); }
  inline void atomic_store_explicit(volatile atomic_bool* object, bool value, memory_order mo) { object->store(value,mo); }
  inline bool atomic_load(const volatile atomic_bool* object) { return object->load(); }
  inline bool atomic_load_explicit(const volatile atomic_bool* object, memory_order mo) { return object->load(mo); }
  inline bool atomic_exchange(volatile atomic_bool* object, bool value) { return object->exchange(value); }
  inline bool atomic_exchange_explicit(volatile atomic_bool* object, bool value, memory_order mo) { return object->exchange(value,mo); }
  inline bool atomic_compare_exchange_weak(volatile atomic_bool* object, bool* expected, bool desired) { return object->compare_exchange_weak(*expected,desired); }
  inline bool atomic_compare_exchange_strong(volatile atomic_bool* object, bool* expected, bool desired) { return object->compare_exchange_weak(*expected,desired); }
  inline bool atomic_compare_exchange_weak_explicit(volatile atomic_bool* object, bool* expected, bool desired, memory_order smo, memory_order fmo) { return object->compare_exchange_weak(*expected,desired,smo,fmo); }
  inline bool atomic_compare_exchange_strong_explicit(volatile atomic_bool* object, bool* expected, bool desired, memory_order smo, memory_order fmo) { return object->compare_exchange_weak(*expected,desired,smo,fmo); }

#ifdef __DOXYGEN__
  /**
   *	@brief %atomic integral type
   *
   *  The name atomic_integral and the functions operating on it in the preceding synopsis are placeholders for
   *  a %set of classes and functions. Throughout the preceding synopsis, atomic_integral should be replaced by
   *  each of the class names in table 118 (Atomics for built-in types) and table 119 (Atomics for standard typedef types), 
   *  and integral should be replaced by the integral type corresponding to the class name.
   *
   *  The %atomic integral types shall have standard layout. They shall each have a trivial default constructor, a
   *  constexpr value constructor, a deleted copy constructor, a deleted copy assignment operator, and a trivial
   *  destructor. They shall each support aggregate initialization syntax.
   **/
  template<typename integral>
  struct atomic_integral:
    public __atomic_v1::integral<integral>
  {
    /** Initializes the object with the default value. */
    constexpr atomic_integral()
    {}
    /** Initializes the object with the \p value. */
    constexpr atomic_integral(type value)
      :integral(value)
    {}
  private:
    atomic_integral(const atomic_integral&) __deleted;
    atomic_integral& operator=(const atomic_integral&) __deleted;
  };
 
  inline bool atomic_is_lock_free(const volatile atomic_integral* object) { return object->is_lock_free(); }
  inline void atomic_store(volatile atomic_integral* object, integral value) { object->store(value); }
  inline void atomic_store_explicit(volatile atomic_integral* object, integral value, memory_order mo) { object->store(value, mo); }
 
  inline integral atomic_load(const volatile atomic_integral* object) { return object->load(); }
  inline integral atomic_load_explicit(const volatile atomic_integral* object, memory_order mo) { return object->load(mo); }
 
  inline integral atomic_exchange(volatile atomic_integral* object, integral value) { return object->exchange(value); }
  inline integral atomic_exchange_explicit(volatile atomic_integral* object, integral value, memory_order mo) { return object->exchange(value,mo); }
 
  inline bool atomic_compare_exchange_strong(volatile atomic_integral* object, integral* expected, integral desired) { return object->compare_exchange_strong(*expected, desired); }
  inline bool atomic_compare_exchange_strong_explicit(volatile atomic_integral* object, integral* expected, integral desired, memory_order smo, memory_order fmo) { return object->compare_exchange_strong(*expected,desired,smo,fmo); }
  inline bool atomic_compare_exchange_weak(volatile atomic_integral* object, integral* expected, integral desired) { return object->compare_exchange_weak(*expected, desired); }
  inline bool atomic_compare_exchange_weak_explicit(volatile atomic_integral* object, integral* expected, integral desired, memory_order smo, memory_order fmo) { return object->compare_exchange_weak(*expected,desired,smo,fmo); }
 
  inline integral atomic_fetch_add(volatile atomic_integral* object, integral value) { return object->fetch_add(value); }
  inline integral atomic_fetch_add_explicit(volatile atomic_integral* object, integral value, memory_order mo) { return object->fetch_add(value,mo); }
  inline integral atomic_fetch_sub(volatile atomic_integral* object, integral value) { return object->fetch_sub(value); }
  inline integral atomic_fetch_sub_explicit(volatile atomic_integral* object, integral value, memory_order mo) { return object->fetch_sub(value,mo); }
  inline integral atomic_fetch_and(volatile atomic_integral* object, integral value) { return object->fetch_and(value); }
  inline integral atomic_fetch_and_explicit(volatile atomic_integral* object, integral value, memory_order mo) { return object->fetch_and(value,mo); }
  inline integral atomic_fetch_or(volatile atomic_integral* object, integral value) { return object->fetch_or(value); }
  inline integral atomic_fetch_or_explicit(volatile atomic_integral* object, integral value, memory_order mo) { return object->fetch_or(value,mo); }
  inline integral atomic_fetch_xor(volatile atomic_integral* object, integral value) { return object->fetch_xor(value); }
  inline integral atomic_fetch_xor_explicit(volatile atomic_integral* object, integral value, memory_order mo) { return object->fetch_xor(value,mo); }
#endif // doxygen

///\hideinitializer
#define NTL_MAKE_ATOMIC_ITYPE(itype, integral_t) \
  struct atomic_ ## itype: \
    public __atomic_v1::integral<integral_t> \
  { \
    constexpr atomic_ ## itype(){ val = 0; } \
    /*explicit*/ constexpr atomic_ ## itype(type value) \
    { val = value; } \
    integral_t operator= (integral_t value) { store(value); return value; } \
  private: \
    atomic_ ## itype(const atomic_ ## itype&) __deleted; \
    atomic_ ## itype& operator=(const atomic_ ## itype&) __deleted; \
  }; \
 \
  inline bool atomic_is_lock_free(const volatile atomic_ ## itype* object) { return object->is_lock_free(); }\
  inline void atomic_store(volatile atomic_ ## itype* object, integral_t value) { object->store(value); }\
  inline void atomic_store_explicit(volatile atomic_ ## itype* object, integral_t value, memory_order mo) { object->store(value, mo); }\
 \
  inline integral_t atomic_load(const volatile atomic_ ## itype* object) { return object->load(); }\
  inline integral_t atomic_load_explicit(const volatile atomic_ ## itype* object, memory_order mo) { return object->load(mo); }\
 \
  inline integral_t atomic_exchange(volatile atomic_ ## itype* object, integral_t value) { return object->exchange(value); }\
  inline integral_t atomic_exchange_explicit(volatile atomic_ ## itype* object, integral_t value, memory_order mo) { return object->exchange(value,mo); }\
 \
  inline bool atomic_compare_exchange_strong(volatile atomic_ ## itype* object, integral_t* expected, integral_t desired) { return object->compare_exchange_strong(*expected, desired); }\
  inline bool atomic_compare_exchange_strong_explicit(volatile atomic_ ## itype* object, integral_t* expected, integral_t desired, memory_order smo, memory_order fmo) { return object->compare_exchange_strong(*expected,desired,smo,fmo); }\
  inline bool atomic_compare_exchange_weak(volatile atomic_ ## itype* object, integral_t* expected, integral_t desired) { return object->compare_exchange_weak(*expected, desired); }\
  inline bool atomic_compare_exchange_weak_explicit(volatile atomic_ ## itype* object, integral_t* expected, integral_t desired, memory_order smo, memory_order fmo) { return object->compare_exchange_weak(*expected,desired,smo,fmo); }\
 \
  inline integral_t atomic_fetch_add(volatile atomic_ ## itype* object, integral_t value) { return object->fetch_add(value); }\
  inline integral_t atomic_fetch_add_explicit(volatile atomic_ ## itype* object, integral_t value, memory_order mo) { return object->fetch_add(value,mo); }\
  inline integral_t atomic_fetch_sub(volatile atomic_ ## itype* object, integral_t value) { return object->fetch_sub(value); }\
  inline integral_t atomic_fetch_sub_explicit(volatile atomic_ ## itype* object, integral_t value, memory_order mo) { return object->fetch_sub(value,mo); }\
  inline integral_t atomic_fetch_and(volatile atomic_ ## itype* object, integral_t value) { return object->fetch_and(value); }\
  inline integral_t atomic_fetch_and_explicit(volatile atomic_ ## itype* object, integral_t value, memory_order mo) { return object->fetch_and(value,mo); }\
  inline integral_t atomic_fetch_or(volatile atomic_ ## itype* object, integral_t value) { return object->fetch_or(value); }\
  inline integral_t atomic_fetch_or_explicit(volatile atomic_ ## itype* object, integral_t value, memory_order mo) { return object->fetch_or(value,mo); }\
  inline integral_t atomic_fetch_xor(volatile atomic_ ## itype* object, integral_t value) { return object->fetch_xor(value); }\
  inline integral_t atomic_fetch_xor_explicit(volatile atomic_ ## itype* object, integral_t value, memory_order mo) { return object->fetch_xor(value,mo); }\
  inline void swap(volatile atomic_##itype& x, volatile atomic_##itype& y) { x.swap(y); }

  ///\name Atomics for built-in types
  NTL_MAKE_ATOMIC_ITYPE(char,char);
  NTL_MAKE_ATOMIC_ITYPE(schar, signed char);
  NTL_MAKE_ATOMIC_ITYPE(uchar, unsigned char);
  NTL_MAKE_ATOMIC_ITYPE(short, signed short int);
  NTL_MAKE_ATOMIC_ITYPE(ushort, unsigned short int);
  NTL_MAKE_ATOMIC_ITYPE(int, signed int);
  NTL_MAKE_ATOMIC_ITYPE(uint, unsigned int);
  NTL_MAKE_ATOMIC_ITYPE(long, signed long int);
  NTL_MAKE_ATOMIC_ITYPE(ulong, unsigned long int);
  NTL_MAKE_ATOMIC_ITYPE(llong,int64_t);
  NTL_MAKE_ATOMIC_ITYPE(ullong, uint64_t);
  NTL_MAKE_ATOMIC_ITYPE(wchar_t, wchar_t);
#ifdef NTL__CXX_CHARS
  NTL_MAKE_ATOMIC_ITYPE(char16_t,char16_t);
  NTL_MAKE_ATOMIC_ITYPE(char32_t,char32_t);
#endif

///\hideinitializer
#define NTL_MAKE_ATOMIC_TYPEDEF(itype) NTL_MAKE_ATOMIC_ITYPE(itype,itype)

  ///\name Atomics for standard typedef types
  typedef atomic_schar atomic_int_least8_t;
  typedef atomic_uchar atomic_uint_least8_t;
  typedef atomic_short atomic_int_least16_t;
  typedef atomic_ushort atomic_uint_least16_t;
  typedef atomic_int atomic_int_least32_t;
  typedef atomic_uint atomic_uint_least32_t;
  typedef atomic_llong atomic_int_least64_t;
  typedef atomic_ullong atomic_uint_least64_t;
  typedef atomic_schar atomic_int_fast8_t;
  typedef atomic_uchar atomic_uint_fast8_t;
  typedef atomic_short atomic_int_fast16_t;
  typedef atomic_ushort atomic_uint_fast16_t;
  typedef atomic_int atomic_int_fast32_t;
  typedef atomic_uint atomic_uint_fast32_t;
  typedef atomic_llong atomic_int_fast64_t;
  typedef atomic_ullong atomic_uint_fast64_t;

  typedef atomic_llong atomic_intmax_t;
  typedef atomic_ullong atomic_uintmax_t;

  NTL_MAKE_ATOMIC_TYPEDEF(intptr_t);
  NTL_MAKE_ATOMIC_TYPEDEF(uintptr_t);
  NTL_MAKE_ATOMIC_TYPEDEF(size_t);
  NTL_MAKE_ATOMIC_TYPEDEF(ssize_t);
  NTL_MAKE_ATOMIC_TYPEDEF(ptrdiff_t);
  ///\}
  /** @} lib_atomic_51 */

  /**\defgroup lib_atomic_52 29.5.2 Address Type [atomics.types.address]
   *@{
   **/
  /**
   *	@brief The atomic_address type provides %atomic \c void* operations.
   *  @note The unit of addition/subtraction is a one byte.
   **/
  struct atomic_address:
    public __atomic_v1::base<void*, true>
  {
  #ifdef NTL__CXX_EF
    atomic_address() = default;
  #else
    /** Initializes the object with the default value. */
    constexpr atomic_address()
    {
      val = 0;
    }
  #endif
    /** Initializes the object with the \p value. */
    constexpr atomic_address(void* value)
    {
      val = value;
    }

    /** Stores and returns \p value. */
    void* operator=(void* value) volatile
    {
      store(value);
      return value;
    }

    /** \copydoc atomic_integral::fetch_add() */
    void* fetch_add(ptrdiff_t offset, memory_order mo = memory_order_seq_cst) volatile
    {
      return x_add(reinterpret_cast<void*>(offset), mo);
    }
    /** \copydoc atomic_integral::fetch_add() */
    void* fetch_sub(ptrdiff_t offset, memory_order mo = memory_order_seq_cst) volatile
    {
      return x_add(reinterpret_cast<void*>(-offset), mo);
    }
    /** Returns <tt>fetch_add(operand) + offset</tt> */
    void* operator+=(ptrdiff_t offset) volatile
    {
      return static_cast<void*>(static_cast<char*>(fetch_add(offset))+offset);
    }
    /** Returns <tt>fetch_sub(operand) - offset</tt> */
    void* operator-=(ptrdiff_t offset) volatile
    {
      return (*this)+=(-offset);
    }
  private:
    atomic_address(const atomic_address&) __deleted;
    atomic_address& operator=(const atomic_address&) __deleted;
  };
  inline bool atomic_is_lock_free(const volatile atomic_address* object) { return object->is_lock_free(); }
  inline void atomic_store(volatile atomic_address* object, void* value) { object->store(value); }
  inline void atomic_store_explicit(volatile atomic_address* object, void* value, memory_order mo) { object->store(value,mo); }
  inline void* atomic_load(const volatile atomic_address* object) { return object->load(); }
  inline void* atomic_load_explicit(const volatile atomic_address* object, memory_order mo) { return object->load(mo); }
  inline void* atomic_exchange(volatile atomic_address* object, void* value) { return object->exchange(value); }
  inline void* atomic_exchange_explicit(volatile atomic_address* object, void* value, memory_order mo) { return object->exchange(value,mo); }
  inline bool atomic_compare_exchange_weak(volatile atomic_address* object, void** expected, void* desired) { return object->compare_exchange_weak(*expected,desired); }
  inline bool atomic_compare_exchange_strong(volatile atomic_address* object, void** expected, void* desired) { return object->compare_exchange_weak(*expected,desired); }
  inline bool atomic_compare_exchange_weak_explicit(volatile atomic_address* object, void** expected, void* desired, memory_order smo, memory_order fmo) { return object->compare_exchange_weak(*expected,desired,smo,fmo); }
  inline bool atomic_compare_exchange_strong_explicit(volatile atomic_address* object, void** expected, void* desired, memory_order smo, memory_order fmo) { return object->compare_exchange_weak(*expected,desired,smo,fmo); }
  inline void* atomic_fetch_add(volatile atomic_address* object, ptrdiff_t offset) { return object->fetch_add(offset); }
  inline void* atomic_fetch_add_explicit(volatile atomic_address* object, ptrdiff_t offset, memory_order mo) { return object->fetch_add(offset,mo); }
  inline void* atomic_fetch_sub(volatile atomic_address* object, ptrdiff_t offset) { return object->fetch_sub(offset); }
  inline void* atomic_fetch_sub_explicit(volatile atomic_address* object, ptrdiff_t offset, memory_order mo) { return object->fetch_sub(offset,mo); }
  /** @} lib_atomic_52 */


 /**\defgroup lib_atomic_53 29.5.3 Generic Types [atomics.types.generic]
  *@{
  **/
  /**
   *	@brief Generic class template atomic
   *
   *  There is a generic class template atomic<T>. The type of the template argument T shall be trivially copy
   *  assignable and bitwise equality comparable. 
   **/
  template<class T> struct atomic;

#if __DOXYGEN__

  /**
   *	@brief There is a generic class template atomic<T>.
   *  @details The type of the template argument T shall be trivially copy assignable and bitwise equality comparable.
   *  
   *  Specializations of the atomic template shall have a deleted copy constructor, a deleted copy assignment operator, and a constexpr value constructor.
   *
   *  There are full specializations over the integral types on the atomic class template. For each integral type
   *  integral in the second column of table 118 or table 119, the specialization atomic<integral> shall be
   *  publicly derived from the corresponding %atomic integral type in the first column of the table. 
   *  These specializations shall have trivial default constructors and trivial destructors.
   **/
  template<class T> 
  struct atomic: public integral<T>
  {
    /** Initializes the object with the default value. */
    constexpr atomic();
    /** Initializes the object with the \p value. */
    constexpr atomic(T value);

    /** Stores and returns \p value. */
    T operator=(T value) volatile;
  private:
    atomic(const atomic&) = delete;
    atomic& operator=(const atomic&) = delete;
  };

#endif // doxygen

  /**
   *	@brief Pointer partial specialization on the atomic class template
   *
   *  These specializations shall be publicly derived from atomic_address. These specializations shall have trivial default constructors and trivial destructors.
   *  @note The unit of addition/subtraction for these specializations shall be the size of the referenced type. 
   **/
  template<class T>
  struct atomic<T*>:
    public atomic_address
  {
  private:
    atomic(const atomic&) __deleted;
    atomic& operator=(const atomic&) __deleted;
  public:
  #ifdef NTL__CXX_EF
    atomic() = default;
  #else
    /** Initializes the object with the default value. */
    constexpr atomic()
    {}
  #endif
    /** Initializes the object with the \p value. */
    constexpr atomic(T* value)
      :atomic_address(value)
    {}

  public:
    ///\name Operations on %atomic types
    /** @copydoc __atomic_v1::base<void*, true>::store() */
    void store(T* value, memory_order mo = memory_order_seq_cst) volatile { atomic_address::store(value, mo); }

    /** @copydoc __atomic_v1::base<void*, true>::load() */
    T* load(memory_order mo = memory_order_seq_cst) const volatile { return reinterpret_cast<T*>(atomic_address::load(mo)); }

    /** Returns load(); */
    operator T*() const volatile { return this->load(mo); }

    /** Stores and returns \p value. */
    T* operator=(T* value) volatile { this->store(value); return value; }
    
    /** @copydoc __atomic_v1::base<void*, true>::exchange() */
    T* exchange(T* value, memory_order mo = memory_order_seq_cst) volatile { return reinterpret_cast<T*>(atomic_address::exchange(value,mo)); }
    /** @copydoc __atomic_v1::base<void*, true>::compare_exchange_strong() */
    bool compare_exchange_weak(T*& expected, T* desired, memory_order success_order, memory_order failure_order) volatile { return atomic_address::compare_exchange_weak(expected, desired, success_order, failure_order); }
    /** @copydoc __atomic_v1::base<void*, true>::compare_exchange_strong() */
    bool compare_exchange_strong(T*& expected, T* desired, memory_order success_order, memory_order failure_order) volatile { return atomic_address::compare_exchange_strong(expected, desired, success_order, failure_order); }
    /** @copydoc __atomic_v1::base<void*, true>::compare_exchange_strong() */
    bool compare_exchange_weak(T*&, T*, memory_order mo = memory_order_seq_cst) volatile { return atomic_address::compare_exchange_weak(expected, desired, mo); }
    /** @copydoc __atomic_v1::base<void*, true>::compare_exchange_strong() */
    bool compare_exchange_strong(T*&, T*, memory_order mo = memory_order_seq_cst) volatile { return atomic_address::compare_exchange_strong(expected, desired, mo); }

    ///\name arithmetic computations
    /** @copydoc __atomic_v1::base<void*, true>::fetch_add() */
    T* fetch_add(ptrdiff_t offset, memory_order mo = memory_order_seq_cst) volatile { return reinterpret_cast<T*>(atomic_address::fetch_add(offset,mo)); }
    /** @copydoc __atomic_v1::base<void*, true>::fetch_sub() */
    T* fetch_sub(ptrdiff_t offset, memory_order mo = memory_order_seq_cst) volatile { return reinterpret_cast<T*>(atomic_address::fetch_sub(offset,mo)); }

    ///\name operator arithmetic computations
    /** Returns <tt>fetch_add(1)</tt> */
    T* operator++(int) volatile { return fetch_add(1); }
    /** Returns <tt>fetch_sub(1)</tt> */
    T* operator--(int) volatile { return fetch_sub(1); }
    /** Returns <tt>fetch_add(1) + 1</tt> */
    T* operator++() volatile { return (*this)+=1; }
    /** Returns <tt>fetch_sub(1) - 1</tt> */
    T* operator--() volatile { return (*this)-=1; }
    /** Returns <tt>fetch_add(offset) + offset</tt> */
    T* operator+=(ptrdiff_t offset) volatile { return fetch_add(offset)+offset; }
    /** Returns <tt>fetch_sub(offset) - offset</tt> */
    T* operator-=(ptrdiff_t offset) volatile { return (*this)+=(-offset); }
  };

///\hideinitializer
#define NTL_MAKE_ATOMIC(itype, integral) \
  template<> struct atomic<integral>: atomic_ ## itype \
  { \
    constexpr atomic() \
    {} \
    explicit constexpr atomic(type value) \
      :atomic_ ## itype(value) \
    {} \
    integral operator= (integral value) { store(value); return value; } \
  };

  NTL_MAKE_ATOMIC(char, char);
  //NTL_MAKE_ATOMIC(schar, signed char);
  //NTL_MAKE_ATOMIC(uchar, unsigned char);
  NTL_MAKE_ATOMIC(short, signed short int);
  NTL_MAKE_ATOMIC(ushort, unsigned short int);
  NTL_MAKE_ATOMIC(int, signed int);
  NTL_MAKE_ATOMIC(uint, unsigned int);
  NTL_MAKE_ATOMIC(long, signed long int);
  NTL_MAKE_ATOMIC(ulong, unsigned long int);
  NTL_MAKE_ATOMIC(llong,int64_t);
  NTL_MAKE_ATOMIC(ullong, uint64_t);
  NTL_MAKE_ATOMIC(wchar_t, wchar_t);
#ifdef NTL__CXX_CHARS
  NTL_MAKE_ATOMIC(char16_t,char16_t);
  NTL_MAKE_ATOMIC(char32_t,char32_t);
#endif

#undef NTL_MAKE_ATOMIC
#undef NTL_MAKE_ATOMIC_ITYPE
#undef NTL_MAKE_ATOMIC_TYPEDEF
  /** @} lib_atomic_53 */
  /** @} lib_atomic_5 */


 /**\defgroup lib_atomic_7 29.7 Flag Type and Operations [atomics.flag]
  *@{
  **/

  /**
   *	@brief The atomic_flag type provides the classic test-and-set functionality. It has two states, %set and clear.
   *  @note The atomic_flag type shall have standard layout. It shall have a trivial default constructor, a deleted copy constructor, a deleted copy assignment operator, and a trivial destructor.
   **/
  struct atomic_flag
  {
    /** Atomically sets the value pointed to by object or by this to \c true. Memory is affected
    according to the value of \p order. These operations are %atomic read-modify-write operations (1.10). */
    bool test_and_set(memory_order order = memory_order_seq_cst) volatile
    {
      static const uint32_t value = true;
      switch(order)
      {
      case memory_order_seq_cst:
      case memory_order_acq_rel:
        {
          ntl::intrinsic::_ReadWriteBarrier();
          const uint32_t r = ntl::atomic::exchange(val_, value);
          ntl::intrinsic::_ReadWriteBarrier();
          return r != false;
        }
      case memory_order_release:
        ntl::intrinsic::_ReadWriteBarrier();
        return ntl::atomic::exchange(val_, value) != false;
      case memory_order_acquire:
      case memory_order_consume:
        {
          const uint32_t r = ntl::atomic::exchange(val_, value);
          ntl::intrinsic::_ReadWriteBarrier();
          return r != false;
        }
      default:
        _assert_msg("invalid memory order specified");
      case memory_order_relaxed:
        return ntl::atomic::exchange(val_, value) != false;
      }
    }

    /** Atomically sets the value pointed to by object or by this to \c false. Memory is affected according to the value of \p order. */
    void clear(memory_order order = memory_order_seq_cst) volatile
    {
      static const uint32_t value = false;
      switch(order)
      {
      case memory_order_seq_cst:
        ntl::intrinsic::_ReadWriteBarrier();
        ntl::atomic::exchange(val_, value);
        ntl::intrinsic::_ReadWriteBarrier();
        break;
      case memory_order_release:
        ntl::intrinsic::_ReadWriteBarrier();
        val_ = value;
        break;
      default:
        _assert_msg("invalid memory order specified");
      case memory_order_relaxed:
        val_ = value;
      }
    }

  #ifdef NTL__CXX_EF
    atomic_flag() = default;
    atomic_flag(const atomic_flag&) __deleted;
    atomic_flag& operator=(const atomic_flag&) __deleted;
  #endif

    friend void swap(volatile atomic_flag& x, volatile atomic_flag& y) { ntl::atomic::exchange(x.val_, y.val_); }

  //private:
  #ifndef __DOXYGEN__
    uint32_t val_;
  #endif
  };
#pragma warning(default: 4127)

  inline bool atomic_flag_test_and_set(volatile atomic_flag* flag) { return flag->test_and_set(); }
  inline bool atomic_flag_test_and_set_explicit(volatile atomic_flag* flag, memory_order mo) { return flag->test_and_set(mo); }
  inline void atomic_flag_clear(volatile atomic_flag* flag) { flag->clear(); }
  inline void atomic_flag_clear_explicit(volatile atomic_flag* flag, memory_order mo) { flag->clear(mo); }

  

/** The macro ATOMIC_FLAG_INIT shall be defined in such a way that it can be used to initialize an object of type atomic_flag to the clear state. 
  For a static-duration object, that initialization shall be static.
  \attention A program that uses an object of type atomic_flag without initializing it with the macro ATOMIC_FLAG_INIT is ill-formed. */
#define ATOMIC_FLAG_INIT {false}
  /** @} lib_atomic_7 */

 /**\defgroup lib_atomic_8 29.8 Fences [atomics.fences]
  * This section introduces synchronization primitives called \e fences. Fences can have acquire semantics, release
  * semantics, or both. A fence with acquire semantics is called an <i>acquire fence</i>. A fence with release semantics is called a <i>release fence</i>.
  *@{
  **/
  inline void atomic_thread_fence(memory_order order)
  {
    switch(order)
    {
    case memory_order_seq_cst:
      {
        static volatile long tmp = 0;
        ntl::intrinsic::_ReadWriteBarrier();
        ntl::atomic::exchange(tmp, 0);
        ntl::intrinsic::_ReadWriteBarrier();
      }
      break;
    case memory_order_acq_rel:
      ntl::intrinsic::_ReadWriteBarrier();
      break;
    case memory_order_acquire:
    case memory_order_consume:
      ntl::intrinsic::_ReadBarrier();
      break;
    case memory_order_release:
      ntl::intrinsic::_WriteBarrier();
      break;
    }
  }

  /**
   *	@brief Equivalent to atomic_thread_fence(order), except that synchronizes with relationships are established only between a %thread and a signal handler executed in the same %thread.
   *  @note  atomic_signal_fence can be used to specify the order in which actions performed by the %thread become visible to the signal handler.
   **/
  inline void atomic_signal_fence(memory_order order)
  {
    if(order != memory_order_relaxed)
      ntl::intrinsic::_ReadWriteBarrier();
  }


  /** @} lib_atomic_8 */
  /** @} lib_atomic */
} // namespace std

#include "../memory"

namespace std 
{
  ///\name 20.8.12.5 shared_ptr %atomic access [util.smartptr.shared.atomic]
  template<class T>
  inline bool atomic_is_lock_free(const shared_ptr<T>* p) __ntl_nothrow
  {
    return *p ? atomic_is_lock_free(p->get()) : false;
  }

  template<class T>
  inline shared_ptr<T> atomic_load(const shared_ptr<T>* p) __ntl_nothrow
  {
    atomic_load_explicit(p, memory_order_seq_cst);
  }
  
  template<class T>
  shared_ptr<T> atomic_load_explicit(const shared_ptr<T>* p, memory_order mo) __ntl_nothrow;
  
  template<class T>
  inline void atomic_store(shared_ptr<T>* p, shared_ptr<T> r) __ntl_nothrow
  {
    atomic_store_explicit(p, r, memory_order_acq_rel);
  }
  
  template<class T>
  void atomic_store_explicit(shared_ptr<T>* p, shared_ptr<T> r, memory_order mo) __ntl_nothrow;
  
  template<class T>
  inline shared_ptr<T> atomic_exchange(shared_ptr<T>* p, shared_ptr<T> r) __ntl_nothrow
  {
    atomic_exchange_explicit(p, r, memory_order_seq_cst);
  }
  
  template<class T>
  shared_ptr<T> atomic_exchange_explicit(shared_ptr<T>* p, shared_ptr<T> r, memory_order mo) __ntl_nothrow;

  template<class T>
  inline bool atomic_compare_exchange(shared_ptr<T>* p, shared_ptr<T> *v, shared_ptr<T> w) __ntl_nothrow
  {
    return atomic_compare_exchange_explicit(p, v, w, memory_order_seq_cst);
  }
  
  template<class T>
  bool atomic_compare_exchange_explicit(shared_ptr<T>* p, shared_ptr<T> *v, shared_ptr<T> w, memory_order success, memory_order failure) __ntl_nothrow;
  ///\}

} // namespace std
#endif // NTL__STLX_ATOMIC
