/**\file*********************************************************************
 *                                                                     \brief
 *  Semaphores
 *
 ****************************************************************************
 */
#ifndef NTL__NT_SEMAPHORE
#define NTL__NT_SEMAPHORE
#pragma once

#include "basedef.hxx"
#include "../device_traits.hxx"
#include "object.hxx"

#include "../stlx/chrono.hxx"
#include "time.hxx"

namespace ntl {
  namespace nt {

    enum semaphore_information_class
    {
      SemaphoreBasicInformation
    };

    struct semaphore_basic_information
    {
      int32_t CurrentCount;
      int32_t MaximumCount;
    };


    NTL_EXTERNAPI
    ntstatus __stdcall
    NtCreateSemaphore(
      handle*             SemaphoreHandle,
      uint32_t            DesiredAccess,
      const object_attributes*  ObjectAttributes,
      int32_t             InitialCount,
      int32_t             MaximumCount
      );

    NTL_EXTERNAPI
    ntstatus __stdcall
    NtOpenSemaphore(
      handle*             SemaphoreHandle,
      uint32_t            DesiredAccess,
      const object_attributes*  ObjectAttributes
      );

    NTL_EXTERNAPI
    ntstatus __stdcall NtReleaseSemaphore(legacy_handle SemaphoreHandle, int32_t ReleaseCount, int32_t* PreviousCount);

    NTL_EXTERNAPI
    ntstatus __stdcall NtQuerySemaphore(legacy_handle SemaphoreHandle, semaphore_information_class SemaphoreInformationClass, void* Information, uint32_t InformationLength, uint32_t* ReturnLength);

    class semaphore;
  } // namespace nt

  template<>
  struct device_traits<nt::semaphore>:
    private device_traits<>
  {
    enum access_mask {
      query_state   = 1,
      modify_state  = 2,
      synchronize   = device_traits<>::synchronize,
      query         = synchronize | query_state,
      all_access    = standard_rights_required | synchronize | query_state | modify_state
    };

    friend access_mask operator | (access_mask m, access_mask m2)
    {
      return bitwise_or(m, m2);
    }

    friend access_mask operator | (access_mask m, nt::access_mask m2)
    {
      return m | static_cast<access_mask>(m2);
    }

    friend access_mask operator | (nt::access_mask m, access_mask m2)
    {
      return m2 | m;
    }
  };

  namespace nt {

    /**
     *	A semaphore object is a synchronization object that maintains a count between zero and a specified maximum value.
     **/
    class semaphore:
      public handle,
      public device_traits<semaphore>,
      public last_status_t
    {
    public:
      
      /**
       *	@brief Creates a named semaphore object
       *
       *	@param[in] MutexName      name of the semaphore object
       *	@param[in] DesiredAccess  desired types of access for the semaphore object
       **/
      explicit semaphore(
        const const_unicode_string& MutexName,
        int32_t             InitialCount,
        int32_t             MaximumCount,
        access_mask         DesiredAccess = all_access
        )
      {
        const object_attributes oa(MutexName);
        last_status_ = NtCreateSemaphore(this, DesiredAccess, &oa, InitialCount, MaximumCount);
      }
      explicit semaphore(
        const object_attributes& MutexName,
        int32_t             InitialCount,
        int32_t             MaximumCount,
        access_mask         DesiredAccess = all_access
        )
      {
        last_status_ = NtCreateSemaphore(this, DesiredAccess, &MutexName, InitialCount, MaximumCount);
      }

      /**
       *	@brief Creates an unnamed semaphore object
       *
       *	@param[in] DesiredAccess  desired types of access for the semaphore object
       **/
      explicit semaphore(
        int32_t             InitialCount,
        int32_t             MaximumCount,
        access_mask         DesiredAccess = all_access
        )
      {
        last_status_ = NtCreateSemaphore(this, DesiredAccess, nullptr, InitialCount, MaximumCount);
      }


      /**
       *	@brief Opens a named semaphore object
       *
       *	@param[in] MutexName      name of the semaphore object
       *	@param[in] DesiredAccess  desired types of access for the semaphore object
       **/
      explicit semaphore(
        const const_unicode_string& MutexName,
        access_mask                 DesiredAccess = modify_state
        )
      {
        const object_attributes oa(MutexName);
        last_status_ = NtOpenSemaphore(this, DesiredAccess, &oa);
      }

      /**
       *	@brief Opens a named semaphore object
       *
       *	@param[in] ObjectAttributes provides a name in the object attributes
       *	@param[in] DesiredAccess    desired types of access for the semaphore object
       **/
      explicit semaphore(
        const object_attributes&  MutexName,
        access_mask               DesiredAccess = modify_state
        )
      {
        last_status_ = NtOpenSemaphore(this, DesiredAccess, &MutexName);
      }


      /**
       *	@brief Creates an semaphore object
       *
       *	@param[out] MutexHandle   pointer to handle that will receive semaphore object handle
       *	@param[in] DesiredAccess  desired types of access for the semaphore object
       *	@param[in] ObjectAttributes provides a name in the object attributes, optional
       *	@param[in] InitialState   initial state of semaphore, signaled or nonsignaled
       *  @return result of operation
       **/
      static ntstatus
        create(
          handle*           MutexHandle,
          access_mask       DesiredAccess,
          const object_attributes*  ObjectAttributes,
          int32_t             InitialCount,
          int32_t             MaximumCount
          )
      {
        return NtCreateSemaphore(MutexHandle, DesiredAccess, ObjectAttributes, InitialCount, MaximumCount);
      }

      /**
       *	@brief Opens a named semaphore object
       *
       *	@param[out] MutexHandle   pointer to handle that will receive semaphore object handle
       *	@param[in] ObjectAttributes provides a name in the object attributes
       *	@param[in] DesiredAccess  desired types of access for the semaphore object
       *  @return result of operation
       **/
      static ntstatus
        open(
          handle*           MutexHandle,
          const object_attributes* ObjectAttributes,
          access_mask       DesiredAccess
          )
      {
        return NtOpenSemaphore(MutexHandle, DesiredAccess, ObjectAttributes);
      }

      ntstatus release(int32_t count = 1)
      {
        return last_status_ = NtReleaseSemaphore(get(), count, nullptr);
      }

      ntstatus release(int32_t count, int32_t& prevcount)
      {
        return last_status_ = NtReleaseSemaphore(get(), count, &prevcount);
      }

      ntstatus query(semaphore_basic_information& info) const
      {
        return last_status_ = NtQuerySemaphore(get(), SemaphoreBasicInformation, &info, sizeof(semaphore_basic_information), nullptr);
      }

      ntstatus count(int32_t& current, int32_t& maximum) const
      {
        semaphore_basic_information info = {};
        last_status_ = query(info);
        current = info.CurrentCount,
          maximum = info.MaximumCount;
        return last_status_;
      }

      /**
       *	@brief Waites when the semaphore object is in the signaled state or the specified time was riched
       *
       *	@param[in] abs_time specifies the absolute time at which the wait is to be completed
       *	@param[in] alertable specifies if the wait is alertable 
       *	@return Status of operation
       **/
      template <class Clock, class Duration>
      ntstatus wait_until(const std::chrono::time_point<Clock, Duration>& abs_time, bool alertable = true) const volatile
      {
        return last_status_ = NtWaitForSingleObject(get(), alertable, std::chrono::duration_cast<system_duration>(abs_time.time_since_epoch()).count());
      }

      /**
       *	@brief Waites when the semaphore object is in the signaled state or the specified time-out interval elapses
       *
       *	@param[in] rel_time specifies the time-out interval at which the wait is to be completed
       *	@param[in] alertable specifies if the wait is alertable 
       *	@return Status of operation
       **/
      template <class Rep, class Period>
      ntstatus wait_for(const std::chrono::duration<Rep, Period>& rel_time, bool alertable = true) const volatile
      {
        return last_status_ = NtWaitForSingleObject(get(), alertable, -1i64*std::chrono::duration_cast<system_duration>(rel_time).count());
      }

      /**
       *	@brief Waites when the semaphore object is in the signaled state
       *	@param[in] alertable specifies if the wait is alertable 
       *	@return Status of operation
       **/
      ntstatus wait(bool alertable = true) const volatile
      {
        return last_status_ = NtWaitForSingleObject(get(), alertable, system_time::infinite());
      }
   }; // semaphore

  }// namespace nt
}// namespace ntl

#endif //NTL__NT_SEMAPHORE
