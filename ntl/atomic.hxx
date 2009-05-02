/**\file*********************************************************************
 *                                                                     \brief
 *  Atomic operations
 *
 ****************************************************************************
 */

#ifndef NTL__ATOMIC
#define NTL__ATOMIC

#ifndef NTL__BASEDEF
#include "basedef.hxx"
#endif

namespace ntl {

#if defined(_MSC_VER)

namespace intrinsic
{
// MSDN prototypes these as having long parameters, however
// it says: The sign is ignored.
  extern "C" {
    
    // interlocked intrinsics

    int32_t __cdecl _InterlockedAnd(volatile uint32_t *, uint32_t);
    char    __cdecl _InterlockedAnd8(volatile uint8_t *, uint8_t);
    int16_t __cdecl _InterlockedAnd16(volatile unsigned short *, unsigned short);
    int64_t __cdecl _InterlockedAnd64(volatile uint64_t *, uint64_t);

    int32_t __cdecl _InterlockedOr(volatile uint32_t *, uint32_t);
    char    __cdecl _InterlockedOr8(volatile uint8_t *, uint8_t);
    int16_t __cdecl _InterlockedOr16(volatile uint16_t *, uint16_t);
    int64_t __cdecl _InterlockedOr64(volatile uint64_t *, uint64_t);

    int32_t __cdecl _InterlockedXor(volatile uint32_t *, uint32_t);
    char    __cdecl _InterlockedXor8(volatile uint8_t *, uint8_t);
    int16_t __cdecl _InterlockedXor16(volatile uint16_t *, uint16_t);
    int64_t __cdecl _InterlockedXor64(volatile uint64_t *, uint64_t);

    int32_t __cdecl _InterlockedIncrement(volatile uint32_t *);
    int16_t __cdecl _InterlockedIncrement16(volatile uint16_t *);
    int64_t __cdecl _InterlockedIncrement64(volatile uint64_t *);

    int32_t __cdecl _InterlockedDecrement(volatile uint32_t *);
    int16_t __cdecl _InterlockedDecrement16(volatile uint16_t *);
    int64_t __cdecl _InterlockedDecrement64(volatile uint64_t *);

    int32_t __cdecl _InterlockedExchange(volatile uint32_t *, uint32_t);
    int64_t __cdecl _InterlockedExchange64(volatile uint64_t *, uint64_t);

    int32_t __cdecl _InterlockedExchangeAdd(volatile uint32_t *, uint32_t);
    int64_t __cdecl _InterlockedExchangeAdd64(volatile uint64_t *, uint64_t);

    int32_t __cdecl _InterlockedCompareExchange(volatile uint32_t *, uint32_t, uint32_t);
    int16_t __cdecl _InterlockedCompareExchange16(volatile uint16_t *, uint16_t, uint16_t);
    int64_t __cdecl _InterlockedCompareExchange64(volatile uint64_t *, uint64_t, uint64_t);

    void*   __cdecl _InterlockedCompareExchangePointer(void* volatile* Destination, void* Exchange, void* Comperand);
    uint8_t __cdecl _InterlockedCompareExchange128(volatile uint64_t *, uint64_t, uint64_t, uint64_t*);
    int64_t __cdecl _InterlockedCompare64Exchange128(uint64_t volatile* Destination, uint64_t ExchangeHigh, uint64_t ExchangeLow, uint64_t Comperand);
    void*   __cdecl _InterlockedExchangePointer(void* volatile* Target, void* Value);
  }

#ifndef __ICL
#pragma intrinsic(_InterlockedAnd, _InterlockedAnd8, _InterlockedAnd16, _InterlockedOr, _InterlockedOr8, _InterlockedOr16, _InterlockedXor, _InterlockedXor8, _InterlockedXor16)
#pragma intrinsic(_InterlockedIncrement, _InterlockedIncrement16, _InterlockedDecrement, _InterlockedDecrement16, _InterlockedExchange, _InterlockedExchangeAdd)
#pragma intrinsic(_InterlockedCompareExchange, _InterlockedCompareExchange16, _InterlockedCompareExchange64)
#ifdef _M_X64
#pragma intrinsic(_InterlockedAnd64, _InterlockedOr64, _InterlockedXor64, _InterlockedIncrement64, _InterlockedDecrement64, _InterlockedExchange64, _InterlockedExchangeAdd64)
#pragma intrinsic(_InterlockedCompareExchangePointer, _InterlockedExchangePointer)
#if _MSC_VER >= 1600
#pragma intrinsic(_InterlockedCompareExchange128, _InterlockedCompare64Exchange128)
#endif
#endif
#endif // icl

  extern "C" {
  // compiler intrinsics
  void __cdecl _ReadBarrier(void);
  void __cdecl _WriteBarrier(void);
  void __cdecl _ReadWriteBarrier(void);

  // cache intrinsics

  /** Guarantees that every load instruction that precedes, in program order, the load fence instruction is globally visible 
    before any load instruction that follows the fence in program order. */
  void __cdecl _mm_lfence(void);

  /** Guarantees that every memory access that precedes, in program order, the memory fence instruction is globally visible 
    before any memory instruction that follows the fence in program order. */
  void __cdecl _mm_mfence(void);
  
  /** Guarantees that every preceding store is globally visible before any subsequent store. */
  void __cdecl _mm_sfence(void);
  
  /** The execution of the next instruction is delayed an implementation specific amount of time (spin-lock hint). */
  void __cdecl _mm_pause(void);

#ifdef _M_X64
  /** Guarantees that every preceding store is globally visible before any subsequent store. */
  void __cdecl __faststorefence();
#endif
  }

#ifndef __ICL
#pragma intrinsic(_ReadBarrier, _WriteBarrier, _ReadWriteBarrier, _mm_lfence, _mm_mfence, _mm_sfence, _mm_pause)

#ifdef _M_X64
# pragma intrinsic(__faststorefence)
#endif
#endif

}//namespace intrinsic


namespace atomic {

#pragma warning(push)
#pragma warning(disable:4365)// signed/unsigned mismatch

  static inline
    uint16_t increment(volatile uint16_t & val)
  {
    return intrinsic::_InterlockedIncrement16(&val);
  }

  static inline
    uint32_t increment(volatile uint32_t & val)
  {
    return intrinsic::_InterlockedIncrement(&val);
  }

  static inline
    uint64_t increment(volatile uint64_t & val)
  {
    return intrinsic::_InterlockedIncrement64(&val);
  }

  static inline
    uint16_t
    decrement(volatile uint16_t & val)
  {
    return intrinsic::_InterlockedDecrement16(&val);
  }
  static inline
    uint32_t
    decrement(volatile uint32_t & val)
  {
    return intrinsic::_InterlockedDecrement(&val);
  }

  static inline
    uint64_t
    decrement(volatile uint64_t & val)
  {
    return intrinsic::_InterlockedDecrement64(&val);
  }

  static inline
    bool and(volatile bool & val, bool mask)
  {
    return intrinsic::_InterlockedAnd8(reinterpret_cast<volatile uint8_t*>(&val), mask) == 1;
  }

  static inline
    uint8_t and(volatile uint8_t & val, uint8_t mask)
  {
    return intrinsic::_InterlockedAnd8(&val, mask);
  }

  static inline
    uint16_t and(volatile uint16_t & val, uint16_t mask)
  {
    return intrinsic::_InterlockedAnd16(&val, mask);
  }

  static inline
    uint32_t and(volatile uint32_t & val, uint32_t mask)
  {
    return intrinsic::_InterlockedAnd(&val, mask);
  }

  static inline
    uint64_t and(volatile uint64_t & val, uint64_t mask)
  {
    return intrinsic::_InterlockedAnd64(&val, mask);
  }

  static inline
    bool or(volatile bool & val, bool mask)
  {
    return intrinsic::_InterlockedOr8(reinterpret_cast<volatile uint8_t*>(&val), mask) == 1;
  }

  static inline
    uint8_t or(volatile uint8_t & val, uint8_t mask)
  {
    return intrinsic::_InterlockedOr8(&val, mask);
  }

  static inline
    uint16_t or(volatile uint16_t & val, uint16_t mask)
  {
    return intrinsic::_InterlockedOr16(&val, mask);
  }

  static inline
    uint32_t or(volatile uint32_t & val, uint32_t mask)
  {
    return intrinsic::_InterlockedOr(&val, mask);
  }

  static inline
    uint64_t or(volatile uint64_t & val, uint64_t mask)
  {
    return intrinsic::_InterlockedOr64(&val, mask);
  }

  static inline
    bool xor(volatile bool & val, bool mask)
  {
    return intrinsic::_InterlockedXor8(reinterpret_cast<volatile uint8_t*>(&val), mask) == 1;
  }

  static inline
    uint8_t xor(volatile uint8_t & val, uint8_t mask)
  {
    return intrinsic::_InterlockedXor8(&val, mask);
  }

  static inline
    uint16_t xor(volatile uint16_t & val, uint16_t mask)
  {
    return intrinsic::_InterlockedXor16(&val, mask);
  }

  static inline
    uint32_t xor(volatile uint32_t & val, uint32_t mask)
  {
    return intrinsic::_InterlockedXor(&val, mask);
  }

  static inline
    uint64_t xor(volatile uint64_t & val, uint64_t mask)
  {
    return intrinsic::_InterlockedXor64(&val, mask);
  }


  static inline
    uint32_t
    exchange(volatile uint32_t & dest, uint32_t val)
  {
    return intrinsic::_InterlockedExchange(&dest, val);
  }

  static inline
    uint64_t
    exchange(volatile uint64_t & dest, uint64_t val)
  {
    return intrinsic::_InterlockedExchange64(&dest, val);
  }

  static inline
    uint32_t
    exchange_add(volatile uint32_t & dest, uint32_t val)
  {
    return intrinsic::_InterlockedExchangeAdd(&dest, val);
  }

  static inline
    uint64_t
    exchange_add(volatile uint64_t & dest, uint64_t val)
  {
    return intrinsic::_InterlockedExchangeAdd64(&dest, val);
  }

  static inline
    uint16_t
    compare_exchange(volatile uint16_t & dest, uint16_t exchange, uint16_t comparand)
  {
    return intrinsic::_InterlockedCompareExchange16(&dest, exchange, comparand);
  }

  static inline
    uint32_t
    compare_exchange(volatile uint32_t & dest, uint32_t exchange, uint32_t comparand)
  {
    return intrinsic::_InterlockedCompareExchange(&dest, exchange, comparand);
  }

  static inline
    uint64_t
    compare_exchange(volatile uint64_t & dest, uint64_t exchange, uint64_t comparand)
  {
    return intrinsic::_InterlockedCompareExchange64(&dest, exchange, comparand);
  }

  static inline
    uint8_t
    compare_exchange(volatile uint64_t & dest, uint64_t exchange_high, uint64_t exchange_low, uint64_t* comparand)
  {
    return intrinsic::_InterlockedCompareExchange128(&dest, exchange_high, exchange_low, comparand);
  }

  static inline
    uint64_t
    compare_exchange(volatile uint64_t & dest, uint64_t exchange_high, uint64_t exchange_low, uint64_t comparand)
  {
    return intrinsic::_InterlockedCompare64Exchange128(&dest, exchange_high, exchange_low, comparand);
  }

  //////////////////////////////////////////////////////////////////////////

  static inline
    int16_t increment(volatile int16_t & val)
  {
    return intrinsic::_InterlockedIncrement16(reinterpret_cast<volatile uint16_t*>(&val));
  }

  static inline
    int32_t increment(volatile int32_t & val)
  {
    return intrinsic::_InterlockedIncrement(reinterpret_cast<volatile uint32_t*>(&val));
  }

  static inline
    int64_t increment(volatile int64_t & val)
  {
    return intrinsic::_InterlockedIncrement64(reinterpret_cast<volatile uint64_t*>(&val));
  }

  static inline
    int16_t
    decrement(volatile int16_t & val)
  {
    return intrinsic::_InterlockedDecrement16(reinterpret_cast<volatile uint16_t*>(&val));
  }
  static inline
    int32_t
    decrement(volatile int32_t & val)
  {
    return intrinsic::_InterlockedDecrement(reinterpret_cast<volatile uint32_t*>(&val));
  }

  static inline
    int64_t
    decrement(volatile int64_t & val)
  {
    return intrinsic::_InterlockedDecrement64(reinterpret_cast<volatile uint64_t*>(&val));
  }

  static inline
    int8_t and(volatile int8_t & val, int8_t mask)
  {
    return intrinsic::_InterlockedAnd8(reinterpret_cast<volatile uint8_t*>(&val), mask);
  }

  static inline
    int16_t and(volatile int16_t & val, int16_t mask)
  {
    return intrinsic::_InterlockedAnd16(reinterpret_cast<volatile uint16_t*>(&val), mask);
  }

  static inline
    int32_t and(volatile int32_t & val, int32_t mask)
  {
    return intrinsic::_InterlockedAnd(reinterpret_cast<volatile uint32_t*>(&val), mask);
  }

  static inline
    int64_t and(volatile int64_t & val, int64_t mask)
  {
    return intrinsic::_InterlockedAnd64(reinterpret_cast<volatile uint64_t*>(&val), mask);
  }

  static inline
    int8_t or(volatile int8_t & val, int8_t mask)
  {
    return intrinsic::_InterlockedOr8(reinterpret_cast<volatile uint8_t*>(&val), mask);
  }

  static inline
    int16_t or(volatile int16_t & val, int16_t mask)
  {
    return intrinsic::_InterlockedOr16(reinterpret_cast<volatile uint16_t*>(&val), mask);
  }

  static inline
    int32_t or(volatile int32_t & val, int32_t mask)
  {
    return intrinsic::_InterlockedOr(reinterpret_cast<volatile uint32_t*>(&val), mask);
  }

  static inline
    int64_t or(volatile int64_t & val, int64_t mask)
  {
    return intrinsic::_InterlockedOr64(reinterpret_cast<volatile uint64_t*>(&val), mask);
  }

  static inline
    int8_t xor(volatile int8_t & val, int8_t mask)
  {
    return intrinsic::_InterlockedXor8(reinterpret_cast<volatile uint8_t*>(&val), mask);
  }

  static inline
    int16_t xor(volatile int16_t & val, int16_t mask)
  {
    return intrinsic::_InterlockedXor16(reinterpret_cast<volatile uint16_t*>(&val), mask);
  }

  static inline
    int32_t xor(volatile int32_t & val, int32_t mask)
  {
    return intrinsic::_InterlockedXor(reinterpret_cast<volatile uint32_t*>(&val), mask);
  }

  static inline
    int64_t xor(volatile int64_t & val, int64_t mask)
  {
    return intrinsic::_InterlockedXor64(reinterpret_cast<volatile uint64_t*>(&val), mask);
  }


  static inline
    int32_t
    exchange(volatile int32_t & dest, int32_t val)
  {
    return intrinsic::_InterlockedExchange(reinterpret_cast<volatile uint32_t*>(&dest), val);
  }

  static inline
    int64_t
    exchange(volatile int64_t & dest, int64_t val)
  {
    return intrinsic::_InterlockedExchange64(reinterpret_cast<volatile uint64_t*>(&dest), val);
  }

  static inline
    int32_t
    exchange_add(volatile int32_t & dest, int32_t val)
  {
    return intrinsic::_InterlockedExchangeAdd(reinterpret_cast<volatile uint32_t*>(&dest), val);
  }

  static inline
    int64_t
    exchange_add(volatile int64_t & dest, int64_t val)
  {
    return intrinsic::_InterlockedExchangeAdd64(reinterpret_cast<volatile uint64_t*>(&dest), val);
  }

  static inline
    int16_t
    compare_exchange(volatile int16_t & dest, int16_t exchange, int16_t comparand)
  {
    return intrinsic::_InterlockedCompareExchange16(reinterpret_cast<volatile uint16_t*>(&dest), exchange, comparand);
  }

  static inline
    int32_t
    compare_exchange(volatile int32_t & dest, int32_t exchange, int32_t comparand)
  {
    return intrinsic::_InterlockedCompareExchange(reinterpret_cast<volatile uint32_t*>(&dest), exchange, comparand);
  }

  static inline
    int64_t
    compare_exchange(volatile int64_t & dest, int64_t exchange, int64_t comparand)
  {
    return intrinsic::_InterlockedCompareExchange64(reinterpret_cast<volatile uint64_t*>(&dest), exchange, comparand);
  }

  static inline
    int8_t
    compare_exchange(volatile int64_t & dest, int64_t exchange_high, int64_t exchange_low, int64_t* comparand)
  {
    return intrinsic::_InterlockedCompareExchange128(reinterpret_cast<volatile uint64_t*>(&dest), exchange_high, exchange_low, reinterpret_cast<uint64_t*>(comparand));
  }

  static inline
    int64_t
    compare_exchange(volatile int64_t & dest, int64_t exchange_high, int64_t exchange_low, int64_t comparand)
  {
    return intrinsic::_InterlockedCompare64Exchange128(reinterpret_cast<volatile uint64_t*>(&dest), exchange_high, exchange_low, comparand);
  }


  static inline void* exchange(void* volatile & dest, void* src)
  {
    return reinterpret_cast<void*>(exchange(reinterpret_cast<volatile uintptr_t&>(dest), reinterpret_cast<uintptr_t>(src)));
  }

  struct generic_op
  {
  private:
    template<size_t v> struct int2type { enum { value = v }; };
    template<typename T> 
    struct tmap
    {
    private:
      template<size_t> struct map;
      template<> struct map<1> { typedef uint8_t type; };
      template<> struct map<2> { typedef uint16_t type; };
      template<> struct map<4> { typedef uint32_t type; };
      template<> struct map<8> { typedef uint64_t type; };
    public:
      typedef typename map<sizeof(T)>::type type;
    };
  public:
    template<typename T>
    static inline T increment(volatile T& val)
    {
      typedef typename tmap<T>::type U;
      return reinterpret_cast<T>(atomic::increment(reinterpret_cast<volatile U&>(val)));
    }
    template<typename T>
    static inline T decrement(volatile T& val)
    {
      typedef typename tmap<T>::type U;
      return reinterpret_cast<T>(atomic::decrement(reinterpret_cast<volatile U&>(val)));
    }
    template<typename T>
    static inline T or(volatile T& val, T mask)
    {
      typedef typename tmap<T>::type U;
      return reinterpret_cast<T>(atomic::or(reinterpret_cast<volatile U&>(val), reinterpret_cast<U>(mask)));
    }
    template<typename T>
    static inline T xor(volatile T& val, T mask)
    {
      typedef typename tmap<T>::type U;
      return reinterpret_cast<T>(atomic::xor(reinterpret_cast<volatile U&>(val), reinterpret_cast<U>(mask)));
    }
    template<typename T>
    static inline T and(volatile T& val, T mask)
    {
      typedef typename tmap<T>::type U;
      return reinterpret_cast<T>(atomic::and(reinterpret_cast<volatile U&>(val), reinterpret_cast<U>(mask)));
    }
    template<typename T>
    static inline T exchange(volatile T& dest, T val)
    {
      typedef typename tmap<T>::type U;
      return reinterpret_cast<T>(atomic::exchange(reinterpret_cast<volatile U&>(dest), reinterpret_cast<U>(val)));
    }
    template<typename T>
    static inline T exchange_add(volatile T& dest, T val)
    {
      typedef typename tmap<T>::type U;
      return reinterpret_cast<T>(atomic::exchange_add(reinterpret_cast<volatile U&>(dest), reinterpret_cast<U>(val)));
    }
    template<typename T>
    static inline T compare_exchange(volatile T& dest, T exchange, T comparand)
    {
      typedef typename tmap<T>::type U;
      return reinterpret_cast<T>(atomic::compare_exchange(reinterpret_cast<volatile U&>(dest), reinterpret_cast<U>(exchange), reinterpret_cast<U>(comparand)));
    }
  };

#pragma warning(pop)
}//namespace atomic

#elif defined(__GNUC__)

namespace atomic {

  template<typename T>
  static inline
    T increment(volatile T & val)
  {
    return __sync_fetch_and_add(&val, 1);
  }

  template<typename T>
  static inline
    T decrement(volatile T & val)
  {
    return __sync_fetch_and_sub(&val);
  }

  template<typename T>
  static inline
    T and(volatile T & val, T mask)
  {
    return __sync_fetch_and_and(&val, mask);
  }
  static inline
    bool and(volatile bool & val, bool mask)
  {
    return and(reinterpret_cast<volatile uint8_t&>(val), mask) == 1;
  }

  template<typename T>
  static inline
    T or(volatile T & val, T mask)
  {
    return __sync_fetch_and_or(&val, mask);
  }
  static inline
    bool or(volatile bool & val, bool mask)
  {
    return or(reinterpret_cast<volatile uint8_t&>(val), mask) == 1;
  }

  template<typename T>
  static inline
    T xor(volatile T & val, T mask)
  {
    return __sync_fetch_and_xor(&val, mask);
  }
  static inline
    bool xor(volatile bool & val, bool mask)
  {
    return xor(reinterpret_cast<volatile uint8_t&>(&val), mask) == 1;
  }

  template<typename T>
  static inline
    T exchange(volatile T & dest, T val)
  {
    return __sync_lock_test_and_set(&dest, val);
  }

  template<typename T>
  static inline
    T exchange_add(volatile T & dest, T val)
  {
    return __sync_fetch_and_add(&dest, val);
  }

  template<typename T>
  static inline
    T compare_exchange(volatile T & dest, T exchange, T comparand)
  {
    return __sync_val_compare_and_swap(&dest, comparand, exchange);
  }

}//namespace atomic


#else

namespace atomic {

static inline
unsigned long
  increment(volatile unsigned long & val)
{
  return 0;
}

static inline
unsigned long
  decrement(volatile unsigned long & val)
{
  return 0;
}

static inline
unsigned long
  exchange(volatile unsigned long & dest, unsigned long val)
{
  return 0;
}

static inline
unsigned long
  exchange_add(volatile unsigned long & dest, unsigned long val)
{
  return 0;
}

static inline
unsigned long
  compare_exchange(volatile unsigned long & dest, unsigned long exchange,
                   unsigned long comparand)
{
  return 0;
}

}//namespace atomic


#endif


/// @warning not to be allocated on memory with the PAGE_NOCACHE attribute.
struct atomic_t
{
    operator const volatile unsigned long & () const { return value; }
    unsigned long operator++() { return atomic::increment(value); }
    unsigned long operator--() { return atomic::decrement(value); }
    void operator+=(unsigned long val) { exchange_add(val); }
    void operator-=(unsigned long val) { exchange_add(0 - val); }
    unsigned long exchange(unsigned long val)
      { return atomic::exchange(value, val); }
    unsigned long exchange_add(unsigned long val)
      { return atomic::exchange_add(value, val); }
    unsigned long exchange_if_equal(unsigned long val, unsigned long comparand)
      { return atomic::compare_exchange(value, val, comparand); }

  private: unsigned long volatile value;
};//struct atomic_t


template <class Lock, class T = void>
class atomic_exec;

template <class Lock, class T>
class atomic_exec
{
    atomic_exec();
    atomic_exec(const atomic_exec &);
    atomic_exec & operator=(const atomic_exec &);

  public:

    explicit
    atomic_exec(Lock & lock)
    : lock(lock), r(lock.acquire())
    { }

    explicit
    atomic_exec(Lock * lock)
    : lock(*lock), r(lock->acquire())
    { }

    ~atomic_exec()
    {
      lock.release(r);
    }

  private:

    Lock & lock;
    T      r;
};


template <class Lock>
class atomic_exec<Lock, void>
{
    atomic_exec();
    atomic_exec(const atomic_exec &);
    atomic_exec & operator=(const atomic_exec &);

  public:

    explicit
    atomic_exec(Lock & lock)
    : lock(lock)
    {
      lock.acquire();
    }

    template<class AnyLock>
    explicit
    atomic_exec(AnyLock & lock)
    : lock(*static_cast<Lock*>(&lock))
    {
      this->lock.acquire();
    }

    explicit
    atomic_exec(Lock * lock)
    : lock(*lock)
    {
      lock.acquire();
    }

    template<class AnyLock>
    explicit
    atomic_exec(AnyLock * lock)
    : lock(*static_cast<Lock*>(lock))
    {
      this->lock.acquire();
    }

    ~atomic_exec()
    {
      lock.release();
    }

  private:

    Lock & lock;

};



}//namespace ntl

#endif//#ifndef NTL__ATOMIC
