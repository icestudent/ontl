/**\file*********************************************************************
 *                                                                     \brief
 *  Events
 *
 ****************************************************************************
 */
#ifndef NTL__NT_EVENT
#define NTL__NT_EVENT
#pragma once

#include "basedef.hxx"
#include "../device_traits.hxx"
#include "object.hxx"

#include "../stlx/chrono.hxx"
#include "time.hxx"

namespace ntl {
  namespace nt {

    /// event
    enum event_type {
      /** manual-reset */
      NotificationEvent,
      /** auto-reset */
      SynchronizationEvent
    };

    enum event_information_class
    {
      EventBasicInformation
    };

    struct event_basic_information
    {
      event_type EventType;
      int32_t    EventState;
    };


    NTL_EXTERNAPI
      ntstatus __stdcall
      NtClearEvent(
        legacy_handle       EventHandle
        );

    typedef ntstatus __stdcall control_event_t(
      legacy_handle       EventHandle,
      uint32_t*           PreviousState
      );

    NTL_EXTERNAPI control_event_t NtSetEvent, NtResetEvent, NtPulseEvent;

    NTL_EXTERNAPI
      ntstatus __stdcall
      NtQueryEvent(
        legacy_handle       EventHandle,
        event_information_class EventInformationClass,
        void*               EventInformation,
        uint32_t            EventInformationLength,
        uint32_t*           ReturnLength
        );

    class user_event;
  } // namespace nt

  template<>
  struct device_traits<nt::user_event>:
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

    NTL_EXTERNAPI
      ntstatus __stdcall
      NtCreateEvent(
        handle*             EventHandle,
        uint32_t            DesiredAccess,
        const object_attributes*  ObjectAttributes,
        event_type          EventType,
        bool                InitialState
        );

    NTL_EXTERNAPI
      ntstatus __stdcall
      NtOpenEvent(
        handle*             EventHandle,
        uint32_t            DesiredAccess,
        const object_attributes*  ObjectAttributes
        );

    /**
     *	Notifies one or more waiting threads that an event has occurred.
     **/
    class user_event:
      public handle,
      public device_traits<user_event>,
      public last_status_t
    {
    public:
      /** Manual-reset event: an event object whose state remains signaled until it is explicitly reset to nonsignaled by the reset() function. */
      static const event_type NotificationEvent    = NotificationEvent;
      /** Auto-reset event: an event object whose state remains signaled until a single waiting thread is released, 
          at which time the system automatically sets the state to nonsignaled.  */
      static const event_type SynchronizationEvent = SynchronizationEvent;

      static const event_type manual_reset = NotificationEvent,
        auto_reset = SynchronizationEvent;
    public:
      
      /**
       *	@brief Creates a named event object
       *
       *	@param[in] EventType      name of the event object
       *	@param[in] ManualReset    type of the event object, can be NotificationEvent or SynchronizationEvent
       *	@param[in] InitialState   initial state of event, signaled or nonsignaled
       *	@param[in] DesiredAccess  desired types of access for the event object
       **/
      explicit user_event(
        const const_unicode_string& EventName,
        event_type          EventType,
        bool                InitialState = false,
        access_mask         DesiredAccess = all_access
        )
      {
        const object_attributes oa(EventName);
        last_status_ = NtCreateEvent(this, DesiredAccess, &oa, EventType, InitialState);
      }
      explicit user_event(
        const object_attributes& EventName,
        event_type          EventType,
        bool                InitialState = false,
        access_mask         DesiredAccess = all_access
        )
      {
        last_status_ = NtCreateEvent(this, DesiredAccess, &EventName, EventType, InitialState);
      }

      /**
       *	@brief Creates an unnamed event object
       *
       *	@param[in] EventType      type of the event object, can be NotificationEvent or SynchronizationEvent
       *	@param[in] InitialState   initial state of event, signaled or nonsignaled
       *	@param[in] DesiredAccess  desired types of access for the event object
       **/
      explicit user_event(
        event_type          EventType,
        bool                InitialState = false,
        access_mask         DesiredAccess = all_access
        )
      {
        last_status_ = NtCreateEvent(this, DesiredAccess, NULL, EventType, InitialState);
      }


      /**
       *	@brief Opens a named event object
       *
       *	@param[in] EventName      name of the event object
       *	@param[in] DesiredAccess  desired types of access for the event object
       **/
      explicit user_event(
        const const_unicode_string& EventName,
        access_mask                 DesiredAccess = modify_state
        )
      {
        const object_attributes oa(EventName);
        last_status_ = NtOpenEvent(this, DesiredAccess, &oa);
      }

      /**
       *	@brief Opens a named event object
       *
       *	@param[in] ObjectAttributes provides a name in the object attributes
       *	@param[in] DesiredAccess    desired types of access for the event object
       **/
      explicit user_event(
        const object_attributes&  EventName,
        access_mask               DesiredAccess = modify_state
        )
      {
        last_status_ = NtOpenEvent(this, DesiredAccess, &EventName);
      }


      /**
       *	@brief Creates an event object
       *
       *	@param[out] EventHandle   pointer to handle that will receive event object handle
       *	@param[in] DesiredAccess  desired types of access for the event object
       *	@param[in] ObjectAttributes provides a name in the object attributes, optional
       *	@param[in] EventType      type of the event object, can be NotificationEvent or SynchronizationEvent
       *	@param[in] InitialState   initial state of event, signaled or nonsignaled
       *  @return result of operation
       **/
      static ntstatus
        create(
          handle*           EventHandle,
          access_mask       DesiredAccess,
          const object_attributes*  ObjectAttributes,
          event_type        EventType,
          bool              InitialState
          )
      {
        return NtCreateEvent(EventHandle, DesiredAccess, ObjectAttributes, EventType, InitialState);
      }

      /**
       *	@brief Opens a named event object
       *
       *	@param[out] EventHandle   pointer to handle that will receive event object handle
       *	@param[in] ObjectAttributes provides a name in the object attributes
       *	@param[in] DesiredAccess  desired types of access for the event object
       *  @return result of operation
       **/
      static ntstatus
        open(
          handle*           EventHandle,
          const object_attributes* ObjectAttributes,
          access_mask       DesiredAccess
          )
      {
        return NtOpenEvent(EventHandle, DesiredAccess, ObjectAttributes);
      }

      /** Sets the state of an event object to signaled. */
      bool set() volatile
      {
        return success(last_status_ = NtSetEvent(get(), 0));
      }

      /** Sets the state of an event object to nonsignaled. */
      bool reset() volatile
      {
        return success(last_status_ = NtClearEvent(get()));
      }

      /** Sets the event object to the signaled state and then resets it to the nonsignaled state after releasing the appropriate number of waiting threads */
      bool pulse() volatile
      {
        return success(last_status_ = NtPulseEvent(get(), 0));
      }

      /** Returns the current event object's state */
      bool is_ready() const volatile
      {
        return (last_status_ = wait_for(system_duration(0), false)) == status::wait_0;
      }

      /** Returns event object's type */
      event_type type() const volatile
      {
        event_basic_information info;
        last_status_ = NtQueryEvent(get(), EventBasicInformation, &info, sizeof(info), NULL);
        return info.EventType;
      }

      /**
       *	@brief Waites when the event object is in the signaled state or the specified time was riched
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
       *	@brief Waites when the event object is in the signaled state or the specified time-out interval elapses
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
       *	@brief Waites when the event object is in the signaled state
       *	@param[in] alertable specifies if the wait is alertable 
       *	@return Status of operation
       **/
      ntstatus wait(bool alertable = true) const volatile
      {
        return last_status_ = NtWaitForSingleObject(get(), alertable, system_time::infinite());
      }
    };

  }// namespace nt
}// namespace ntl

#endif //NTL__NT_EVENT
