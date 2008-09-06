/**\file*********************************************************************
 *                                                                     \brief
 *  Events
 *
 ****************************************************************************
 */

#ifndef NTL__NT_EVENT
#define NTL__NT_EVENT

#include "basedef.hxx"
#include "../device_traits.hxx"
#include "object.hxx"

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


    NTL__EXTERNAPI
      ntstatus __stdcall
      NtClearEvent(
      legacy_handle       EventHandle
      );

    typedef ntstatus __stdcall control_event_t(
      legacy_handle       EventHandle,
      uint32_t*           PreviousState
      );

    NTL__EXTERNAPI control_event_t NtSetEvent, NtResetEvent, NtPulseEvent;

    NTL__EXTERNAPI
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
      all_access    = standard_rights_required | synchronize | 0x3
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

    NTL__EXTERNAPI
      ntstatus __stdcall
      NtCreateEvent(
        handle*             EventHandle,
        uint32_t            DesiredAccess,
        const object_attributes*  ObjectAttributes,
        event_type          EventType,
        bool                InitialState
        );

    NTL__EXTERNAPI
      ntstatus __stdcall
      NtOpenEvent(
        handle*             EventHandle,
        uint32_t            DesiredAccess,
        const object_attributes*  ObjectAttributes
        );

    /// user_event
    class user_event:
      public handle,
      public device_traits<user_event>
    {
    public:
      // create
      explicit user_event(
        const const_unicode_string& event_name,
        event_type          EventType,
        bool                InitialState = false,
        access_mask         DesiredAccess = all_access
        )
      {
        const object_attributes oa(event_name);
        create(this, DesiredAccess, &oa, EventType, InitialState);
      }

      explicit user_event(
        const const_unicode_string& event_name,
        event_type          EventType,
        bool&               IsOpened,
        bool                InitialState = false,
        access_mask         DesiredAccess = all_access
        )
      {
        const object_attributes oa(event_name, object_attributes::case_insensitive | object_attributes::openif);
        IsOpened = create(this, DesiredAccess, &oa, EventType, InitialState) == status::object_name_exists;
      }

      explicit user_event(
        event_type          EventType,
        bool                InitialState = false,
        access_mask         DesiredAccess = all_access
        )
      {
        create(this, DesiredAccess, NULL, EventType, InitialState);
      }


      // open
      user_event(
        const const_unicode_string& event_name,
        access_mask         DesiredAccess = modify_state
        )
      {
        const object_attributes oa(event_name);
        open(this, &oa, DesiredAccess);
      }


      user_event(
        const object_attributes&  ObjectAttributes,
        access_mask         DesiredAccess = modify_state
        )
      {
        open(this, &ObjectAttributes, DesiredAccess);
      }
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

      static ntstatus
        open(
          handle*           EventHandle,
          const object_attributes* ObjectAttributes,
          access_mask       DesiredAccess
          )
      {
        return NtOpenEvent(EventHandle, DesiredAccess, ObjectAttributes);
      }

      bool set()
      {
        return success(NtSetEvent(get(), 0));
      }

      bool reset()
      {
        return success(NtClearEvent(get()));
      }

      bool pulse()
      {
        return success(NtPulseEvent(get(), 0));
      }

      // old-style wait, deprecated
      __declspec(deprecated("use 'wait_for' function"))
      ntstatus wait(uint32_t timeout = -1, bool alertable = true) const
      {
        const int64_t interval = int64_t(-1) * times::milliseconds * timeout;
        return NtWaitForSingleObject(get(), alertable, interval);
      }

      // old-style wait, deprecated
      template<ntl::times::type TimeResolution>
      __declspec(deprecated("use 'wait_for' function"))
      ntstatus wait(uint32_t timeout_time_resolution, bool alertable = true) const
      {
        const int64_t interval = int64_t(-1) * TimeResolution * timeout_time_resolution;
        return NtWaitForSingleObject(get(), alertable, interval);
      }

      template <class Clock, class Duration>
      ntstatus wait_until(const std::chrono::time_point<Clock, Duration>& abs_time, bool alertable = true) const
      {
        typedef ratio_multiply<ratio<100>, nano>::type systime_unit;
        typedef chrono::duration<systime_t, systime_unit> system_duration;

        return NtWaitForSingleObject(get(), alertable, std::chrono::duration_cast<system_duration>(abs_time.time_since_epoch()).count());
      }

      template <class Rep, class Period>
      ntstatus wait_for(const std::chrono::duration<Rep, Period>& rel_time, bool alertable = true) const
      {
        typedef ratio_multiply<ratio<100>, nano>::type systime_unit;
        typedef chrono::duration<systime_t, systime_unit> system_duration;

        return NtWaitForSingleObject(get(), alertable, -1i64 * std::chrono::duration_cast<system_duration>(rel_time).count());
      }

      bool is_ready() const
      {
        typedef std::ratio_multiply<std::ratio<100>, std::nano>::type systime_unit;
        typedef std::chrono::duration<systime_t, systime_unit> system_duration;

        return success(wait_for(system_duration(0), false));
      }
    };

  }// namespace nt
}// namespace ntl

#endif //