/**\file*********************************************************************
 *                                                                     \brief
 *  Atomic operations
 *
 ****************************************************************************
 */

#ifndef NTL__ATOMIC
#define NTL__ATOMIC

namespace ntl {

#ifdef  _MSC_VER

namespace intrinsic {
// MSDN prototypes these as having long parameters, however
// it says: The sign is ignored.
extern "C" long __cdecl _InterlockedIncrement(volatile unsigned long *);
extern "C" long __cdecl _InterlockedDecrement(volatile unsigned long *);
extern "C" long __cdecl _InterlockedExchange(volatile unsigned long *, unsigned long);
extern "C" long __cdecl _InterlockedExchangeAdd(volatile unsigned long *, unsigned long);
extern "C" long __cdecl _InterlockedCompareExchange(volatile unsigned long *, unsigned long, unsigned long);
#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_InterlockedExchangeAdd)
#pragma intrinsic(_InterlockedCompareExchange)
}//namespace intrinsic


namespace atomic {

#pragma warning(push)
#pragma warning(disable:4365)// signed/unsigned mismatch

static inline
unsigned long
  increment(volatile unsigned long & val) 
{ 
  return intrinsic::_InterlockedIncrement(&val);
}

static inline
unsigned long
  decrement(volatile unsigned long & val) 
{ 
  return intrinsic::_InterlockedDecrement(&val);
}

static inline
unsigned long
  exchange(volatile unsigned long & target, unsigned long val) 
{ 
  return intrinsic::_InterlockedExchange(&target, val);
}

static inline
unsigned long
  exchange_add(volatile unsigned long & target, unsigned long val) 
{ 
  return intrinsic::_InterlockedExchangeAdd(&target, val);
}

static inline
unsigned long
  compare_exchange(volatile unsigned long & dest, unsigned long exchange,
                   unsigned long comperand)
{ 
  return intrinsic::_InterlockedCompareExchange(&dest, exchange, comperand);
}

#pragma warning(pop)
}//namespace atomic

#else
#error unsupported compiler
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
    unsigned long exchange_if_equal(unsigned long val, unsigned long comperand) 
      { return atomic::compare_exchange(value, val, comperand); }

  private: unsigned long volatile value;
};//struct atomic_t



template <class Lock, class T>
class atomic_exec
{
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

    atomic_exec();
    atomic_exec(const atomic_exec &);
    atomic_exec & operator=(const atomic_exec &);
}; 
  

template <class Lock>
class atomic_exec<Lock, void>
{
  public:

    explicit
    atomic_exec(Lock & lock)
    : lock(lock)
    { 
      lock.acquire()
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
      lock.acquire()
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

    atomic_exec();
    atomic_exec(const atomic_exec &);
    atomic_exec & operator=(const atomic_exec &);
}; 
  


}//namespace ntl

#endif//#ifndef NTL__ATOMIC
