/**\file*********************************************************************
 *                                                                     \brief
 *  Shared resource support
 *
 ****************************************************************************
 */
#ifndef NTL__NT_RESOURCE
#define NTL__NT_RESOURCE

#include "basedef.hxx"
#include "handle.hxx"
#include "../stlx/chrono.hxx"

namespace ntl {
  namespace nt {

    typedef std::ratio_multiply<std::ratio<100>, std::nano>::type systime_unit;
    typedef std::chrono::duration<systime_t, systime_unit>        system_duration;

    namespace rtl
    {
      // critical section
      struct critical_section_debug
      {
        enum type { CritSect, Resource };
        uint16_t    Type;
        uint16_t    CreatorBackTraceIndex;
        struct critical_section* CriticalSection;
        list_entry  ProcessLocksList;
        uint32_t    EntryCount;
        uint32_t    ContentionCount;
        uint32_t    Spare[2];
      };

      struct critical_section
      {
        critical_section_debug* DebugInfo;
        int32_t       LockCount;
        int32_t       RecursionCount;
        legacy_handle OwningThread;
        legacy_handle LockSemaphore;
        uint32_t      reserved;
      };

      struct resource_debug
      {
        uint32_t      reserved[5];
        uint32_t      ContentionCount;
        uint32_t      Spare[2];
      };

      // shared resource
      struct resource
      {
        critical_section  CriticalSection;
        legacy_handle     SharedSemaphore;
        uint32_t          NumberOfWaitingShared;
        legacy_handle     ExclusiveSemaphore;
        uint32_t          NumberOfWaitingExclusive;

        int32_t           NumberOfActive;
        legacy_handle     ExclusiveOwnerThread;

        enum flags { None, LongTerm };
        flags             Flags;
        resource_debug*   DebugInfo;
      };

      // run once
      union run_once
      {
        void* Ptr;
      };

      static const uint32_t RunOnceCheckOnly = 1U;
      static const uint32_t RunOnceAsync     = 2U;

      typedef uint32_t __stdcall run_once_init_t(
          rtl::run_once* RunOnce,
          void* Parameter,
          void** Context
          );

    } // rtl


    typedef ntstatus __stdcall critical_section_control_t(rtl::critical_section* CriticalSection);

    NTL__EXTERNAPI
      critical_section_control_t
        RtlInitializeCriticalSection,
        RtlDeleteCriticalSection,
        RtlEnterCriticalSection,
        RtlLeaveCriticalSection;

    NTL__EXTERNAPI
      ntstatus __stdcall RtlInitializeCriticalSectionAndSpinCount(
        rtl::critical_section* CriticalSection,
        uint32_t               SpinCount
        );

    NTL__EXTERNAPI
      uint32_t __stdcall RtlTryEnterCriticalSection(rtl::critical_section* CriticalSection);

    NTL__EXTERNAPI
      uint32_t __stdcall RtlIsCriticalSectionLocked(rtl::critical_section* CriticalSection);

    NTL__EXTERNAPI
      uint32_t __stdcall RtlIsCriticalSectionLockedByThread(rtl::critical_section* CriticalSection);

    NTL__EXTERNAPI
      uint32_t __stdcall RtlGetCriticalSectionRecursionCount(const rtl::critical_section* CriticalSection);

    NTL__EXTERNAPI
      uint32_t __stdcall RtlSetCriticalSectionSpinCount(rtl::critical_section* CriticalSection, uint32_t SpinCount);

    NTL__EXTERNAPI
      void __stdcall RtlEnableEarlyCriticalSectionEventCreation();

    NTL__EXTERNAPI
      void __stdcall RtlCheckForOrphanedCriticalSections(legacy_handle Thread);

    NTL__EXTERNAPI
      void __stdcall RtlpWaitForCriticalSection(rtl::critical_section* CriticalSection);

    NTL__EXTERNAPI
      void __stdcall RtlpUnWaitCriticalSection(rtl::critical_section* CriticalSection);

    NTL__EXTERNAPI
      void __stdcall RtlpNotOwnerCriticalSection(rtl::critical_section* CriticalSection);

    // run once
    NTL__EXTERNAPI
      void RtlRunOnceInitialize(rtl::run_once* RunOnce);

    NTL__EXTERNAPI
      uint32_t RtlRunOnceExecuteOnce(
        rtl::run_once*        RunOnce,
        rtl::run_once_init_t  InitFn,
        void*                 Parameter,
        void**                Context
      );

    NTL__EXTERNAPI
      uint32_t RtlRunOnceBeginInitialize(
        rtl::run_once*        RunOnce,
        uint32_t              Flags,
        void**                Context
      );

   NTL__EXTERNAPI
      uint32_t RtlRunOnceComplete(
        rtl::run_once*        RunOnce,
        uint32_t              Flags,
        void**                Context
      );

   //////////////////////////////////////////////////////////////////////////

   typedef void __stdcall resource_control_t(rtl::resource*);
   
   NTL__EXTERNAPI 
     resource_control_t
      RtlInitializeResource,
      RtlReleaseResource,
      RtlDeleteResource,
      RtlConvertSharedToExclusive,
      RtlConvertExclusiveToShared;

   bool __stdcall
     RtlAcquireResourceShared(rtl::resource* Resource, bool Wait);

   bool __stdcall
     RtlAcquireResourceExclusive(rtl::resource* Resource, bool Wait);


   /************************************************************************/
   /* CS RAII                                                              */
   /************************************************************************/

   /**
    *	Critical section RAII wrapper
    **/
   class critical_section:
     protected rtl::critical_section,
     ntl::noncopyable
   {
   public:
     /** Constructs CS object */
     critical_section()
     {
       ntl::nt::RtlInitializeCriticalSection(this);
     }

     /** Constructs CS object with the specified spin count */
     explicit critical_section(uint32_t SpinCount)
     {
       ntl::nt::RtlInitializeCriticalSectionAndSpinCount(this, SpinCount);
     }

     /** Destroys CS object. If CS was owned, it is undefined behaviour. */
     ~critical_section()
     {
       ntl::nt::RtlDeleteCriticalSection(this);
     }

     /** Waits and takes ownership of CS. */
     void acquire()
     {
       ntl::nt::RtlEnterCriticalSection(this);
     }

     /** Releases owlership. */
     void release()
     {
       ntl::nt::RtlLeaveCriticalSection(this);
     }

     /** Attempts to take ownership of this CS object. */
     bool try_acquire()
     {
       return ntl::nt::RtlTryEnterCriticalSection(this) != 0;
     }

     /** Checks owlership of this CS. */
     bool locked()
     {
       return ntl::nt::RtlIsCriticalSectionLocked(this) != 0;
     }

     /** Checks is the current thread is owner of CS. */
     bool thread_locked()
     {
       return ntl::nt::RtlIsCriticalSectionLockedByThread(this) != 0;
     }

     
     /**
      *	@brief Waits the specified time for ownership of the CS
      *
      * This function implements the waiting on CS object. 
      * If the CS is haves a synchronization object, function calls standard wait mechanism,
      * otherwise it waits with a delayed execution.
      *
      *	@param[in] timeout absolute or relative time to wait.
      *	@param[in] explicit_wait If CS doesn't have a synchronization object and CS object is owled by other thread, immediately returns.
      *	@param[in] alertable waiting is alertable.
      *
      *	@return Wait status
      *
      **/
     ntstatus wait(const systime_t& timeout, bool explicit_wait, bool alertable = true)
     {
       if(LockSemaphore){
         DebugInfo->EntryCount++;
         DebugInfo->ContentionCount++;
         return NtWaitForSingleObject(LockSemaphore, alertable, timeout);
       }

       if(!try_acquire() && !explicit_wait)
         return status::invalid_handle;

       // wait
       ntstatus st;
       systime_t period = timeout;
       systime_t const interval = std::chrono::duration_cast<system_duration>( std::chrono::milliseconds(50)).count();
       do{
         st = NtDelayExecution(alertable, interval);
         period -= interval;
       }while(st == status::timeout && period > 0);
       return st;
     }

     /**
      *	@brief Waits to ownership until the specified time is occurs
      *
      *	@param[in] abs_time absolute time to wait
      *	@param[in] explicit_wait force wait if CS doesn't have synchronization object
      *	@param[in] alertable is waiting is alertable
      *
      *	@return Wait status
      *
      **/
     template <class Clock, class Duration>
     ntstatus wait_until(const std::chrono::time_point<Clock, Duration>& abs_time, bool explicit_wait = true, bool alertable = true)
     {
       return wait(std::chrono::duration_cast<system_duration>(abs_time.time_since_epoch()).count(), explicit_wait, alertable);
     }

     /**
      *	@brief Waits to ownership for specified time
      *
      *	@param[in] rel_time relative time to wait
      *	@param[in] explicit_wait force wait if CS doesn't have synchronization object
      *	@param[in] alertable is waiting is alertable
      *
      *	@return Wait status
      *
      **/
     template <class Rep, class Period>
     ntstatus wait_for(const std::chrono::duration<Rep, Period>& rel_time, bool explicit_wait = true, bool alertable = true)
     {
       return wait(-1i64 * std::chrono::duration_cast<system_duration>(rel_time).count(), explicit_wait, alertable);
     }

     /** Sets spin count */
     void spin_count(uint32_t SpinCount)
     {
       ntl::nt::RtlSetCriticalSectionSpinCount(this, SpinCount);
     }
   };


   /************************************************************************/
   /* Resource RAII                                                        */
   /************************************************************************/
   class resource:
     protected rtl::resource
   {
   public:
      resource()
      {
        RtlInitializeResource(this);
      }

      ~resource()
      {
        RtlDeleteResource(this);
      }

      bool acquire(bool exclusive, bool wait)
      {
        (exclusive ? RtlAcquireResourceExclusive : RtlAcquireResourceShared)(this, wait);
      }

      bool acquire_shared(bool wait = true)
      {
        return acquire(false, wait);
      }

      bool acquire_exclusive(bool wait = true)
      {
        return acquire(true, wait);
      }

      void release()
      {
        RtlReleaseResource(this);
      }

      void convert(bool exclusive)
      {
        (exclusive ? RtlConvertSharedToExclusive : RtlConvertExclusiveToShared)(this);
      }
   };

  } //namespace nt
} //namespace ntl

#endif //#ifndef NTL__NT_RESOURCE

