/**\file*********************************************************************
 *                                                                     \brief
 *  Synchronization support
 *
 ****************************************************************************
 */
#ifndef NTL__NT_MUTEX
#define NTL__NT_MUTEX
#pragma once

#include "resource.hxx"
#include "object.hxx"

namespace ntl {
  namespace nt {
    enum mutant_information_class
    {
      MutantBasicInformation
    };

    struct mutant_basic_information
    {
      int32_t CurrentCount;
      bool    OwnedByCaller;
      bool    AbandonedState;
    };


    NTL_EXTERNAPI
    ntstatus __stdcall
    NtCreateMutant(
      handle*             MutantHandle,
      uint32_t            DesiredAccess,
      const object_attributes*  ObjectAttributes,
      bool                InitialState
      );

    NTL_EXTERNAPI
    ntstatus __stdcall
    NtOpenMutant(
      handle*             MutantHandle,
      uint32_t            DesiredAccess,
      const object_attributes*  ObjectAttributes
      );

    NTL_EXTERNAPI
    ntstatus __stdcall NtReleaseMutant(legacy_handle MutantHandle, int32_t* PreviousCount);

    NTL_EXTERNAPI
    ntstatus __stdcall NtQueryMutant(legacy_handle MutantHandle, mutant_information_class MutantInformationClass, void* Information, uint32_t InformationLength, uint32_t* ReturnLength);


    class mutex;
  } // namespace nt

  template<>
  struct device_traits<nt::mutex>:
    private device_traits<>
  {
    enum access_mask {
      query_state   = 1,
      synchronize   = device_traits<>::synchronize,
      query         = synchronize | query_state,
      all_access    = standard_rights_required | synchronize | query_state
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
     *	Notifies one or more waiting threads that an mutex has occurred.
     **/
    class mutex:
      public handle,
      public device_traits<mutex>,
      public last_status_t
    {
    public:
      
      /**
       *	@brief Creates a named mutex object
       *
       *	@param[in] MutexName      name of the mutex object
       *	@param[in] InitialState   initial state of mutex, signaled or nonsignaled
       *	@param[in] DesiredAccess  desired types of access for the mutex object
       **/
      explicit mutex(
        const const_unicode_string& MutexName,
        bool                InitialState = false,
        access_mask         DesiredAccess = all_access
        )
      {
        const object_attributes oa(MutexName);
        last_status_ = NtCreateMutant(this, DesiredAccess, &oa, InitialState);
      }
      explicit mutex(
        const object_attributes& MutexName,
        bool                InitialState = false,
        access_mask         DesiredAccess = all_access
        )
      {
        last_status_ = NtCreateMutant(this, DesiredAccess, &MutexName, InitialState);
      }

      /**
       *	@brief Creates an unnamed mutex object
       *
       *	@param[in] InitialState   initial state of mutex, signaled or nonsignaled
       *	@param[in] DesiredAccess  desired types of access for the mutex object
       **/
      explicit mutex(
        bool                InitialState = false,
        access_mask         DesiredAccess = all_access
        )
      {
        last_status_ = NtCreateMutant(this, DesiredAccess, nullptr, InitialState);
      }


      /**
       *	@brief Opens a named mutex object
       *
       *	@param[in] MutexName      name of the mutex object
       *	@param[in] DesiredAccess  desired types of access for the mutex object
       **/
      explicit mutex(
        const const_unicode_string& MutexName,
        access_mask                 DesiredAccess = device_traits::query
        )
      {
        const object_attributes oa(MutexName);
        last_status_ = NtOpenMutant(this, DesiredAccess, &oa);
      }

      /**
       *	@brief Opens a named mutex object
       *
       *	@param[in] ObjectAttributes provides a name in the object attributes
       *	@param[in] DesiredAccess    desired types of access for the mutex object
       **/
      explicit mutex(
        const object_attributes&  MutexName,
        access_mask               DesiredAccess = device_traits::query
        )
      {
        last_status_ = NtOpenMutant(this, DesiredAccess, &MutexName);
      }


      /**
       *	@brief Creates an mutex object
       *
       *	@param[out] MutexHandle   pointer to handle that will receive mutex object handle
       *	@param[in] DesiredAccess  desired types of access for the mutex object
       *	@param[in] ObjectAttributes provides a name in the object attributes, optional
       *	@param[in] InitialState   initial state of mutex, signaled or nonsignaled
       *  @return result of operation
       **/
      static ntstatus
        create(
          handle*           MutexHandle,
          access_mask       DesiredAccess,
          const object_attributes*  ObjectAttributes,
          bool              InitialState
          )
      {
        return NtCreateMutant(MutexHandle, DesiredAccess, ObjectAttributes, InitialState);
      }

      /**
       *	@brief Opens a named mutex object
       *
       *	@param[out] MutexHandle   pointer to handle that will receive mutex object handle
       *	@param[in] ObjectAttributes provides a name in the object attributes
       *	@param[in] DesiredAccess  desired types of access for the mutex object
       *  @return result of operation
       **/
      static ntstatus
        open(
          handle*           MutexHandle,
          const object_attributes* ObjectAttributes,
          access_mask       DesiredAccess
          )
      {
        return NtOpenMutant(MutexHandle, DesiredAccess, ObjectAttributes);
      }

      ntstatus release()
      {
        return last_status_ = NtReleaseMutant(get(), nullptr);
      }

      ntstatus release(int32_t& prevcount)
      {
        return last_status_ = NtReleaseMutant(get(), &prevcount);
      }

      ntstatus query(mutant_basic_information& info) const
      {
        return last_status_ = NtQueryMutant(get(), MutantBasicInformation, &info, sizeof(mutant_basic_information), nullptr);
      }

      bool owned() const
      {
        mutant_basic_information info = {};
        query(info);
        return info.OwnedByCaller;
      }

      /**
       *	@brief Waites when the mutex object is in the signaled state or the specified time was riched
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
       *	@brief Waites when the mutex object is in the signaled state or the specified time-out interval elapses
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
       *	@brief Waites when the mutex object is in the signaled state
       *	@param[in] alertable specifies if the wait is alertable 
       *	@return Status of operation
       **/
      ntstatus wait(bool alertable = true) const volatile
      {
        return last_status_ = NtWaitForSingleObject(get(), alertable, system_time::infinite());
      }
   }; // mutex


#pragma warning(disable:4127) // conditional expression is constant

    /// STL default base mutex object
    /// In NT subsystem implemented as futex.
    template<bool recursive>
    class base_mutex:
      protected critical_section
    {
    public:
      /** Constructs mutex object */
      base_mutex()
      {}

      /**
       *	@note The behavior of a program is undefined if it destroys a recursive_mutex object owned by any thread.
       **/
      ~base_mutex()
      {
        // NOTE: see @note
        // detect who locks the mutex
        if(locked()){
          if(recursive && is_owner())
            release();
          else 
            // UB
            _assert_msg("This mutex owned by other thread or deletion of owning mutex is not allowed.");
        }
      }

      /** Blocks the calling thread until ownership of the mutex can be obtained for the calling thread. */
      void lock() __ntl_throws(std::system_error)
      {
        if(!recursive){
          if(is_owner()){ // throw system_error(resource_deadlock_would_occur)
            _assert_msg("Current thread already owns the mutex.");
            return;
          }
        }
        acquire();
      }

      /** Attempts to obtain ownership of the mutex for the calling thread without blocking. */
      bool try_lock() __ntl_nothrow
      {
        if(!recursive){
          if(is_owner()){ // throw system_error(resource_deadlock_would_occur)
            _assert_msg("Current thread already owns the mutex.");
            return false;
          }
        }
        return try_acquire();
      }

      /** Releases the calling thread’s ownership of the mutex. */
      void unlock() __ntl_nothrow
      {
        if(!locked()){
          _assert_msg("Unlocking non-locked mutex."); // throw system_error(operation_not_permitted)
        }else
          release();
      }

      // Provide access to implementation details.
      typedef ntl::nt::rtl::critical_section* native_handle_type;
      native_handle_type native_handle()
      {
        return this;
      }

    private:
      base_mutex(const base_mutex&) __deleted;
      base_mutex& operator=(const base_mutex&) __deleted;
    };

#pragma warning(default:4127)
  }
}
#endif // NTL__NT_MUTEX
