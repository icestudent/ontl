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
#include "../km/basedef.hxx"

namespace ntl {
  namespace nt {

    /// event
    using km::event_type;
    using km::SynchronizationEvent;
    using km::NotificationEvent;


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
      event_query_state   = 1, 
      event_modify_state  = 2, 
      event_all_access    = standard_rights_required | synchronize | 0x3
    };
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
        const std::wstring& event_name,
        event_type          EventType,
        bool                InitialState = false,
        access_mask         DesiredAccess = event_all_access
        )
      {
        const const_unicode_string uname(event_name);
        const object_attributes oa(uname);
        create(this, DesiredAccess, &oa, EventType, InitialState);
      }

      explicit user_event(
        event_type          EventType,
        bool                InitialState = false,
        access_mask         DesiredAccess = event_all_access
        )
      {
        create(this, DesiredAccess, NULL, EventType, InitialState);
      }


      // open
      user_event(
        const std::wstring& event_name,
        access_mask         DesiredAccess = event_modify_state
        )
      {
        const const_unicode_string uname(event_name);
        const object_attributes oa(uname);
        open(this, &oa, DesiredAccess);
      }


      user_event(
        const object_attributes&  ObjectAttributes,
        access_mask               DesiredAccess = event_modify_state
        )
      {
        open(this, &ObjectAttributes, DesiredAccess);
      }
      static ntstatus
        create(
          handle*             EventHandle,
          access_mask         DesiredAccess,
          const object_attributes*  ObjectAttributes,
          event_type          EventType,
          bool                InitialState
          )
      {
        return NtCreateEvent(EventHandle, DesiredAccess, ObjectAttributes, EventType, InitialState);
      }

      static ntstatus
        open(
          handle*             EventHandle,
          const object_attributes* ObjectAttributes,
          access_mask         DesiredAccess
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

      ntstatus wait(bool alertable = true, uint32_t timeout = -1)
      {
        const int64_t interval = int64_t(-1) * milliseconds * timeout;
        return NtWaitForSingleObject(get(), alertable, &interval);
      }

      template<ntl::times TimeResolution>
      ntstatus wait(uint32_t timeout_time_resolution, bool alertable = true)
      {
        const int64_t interval = int64_t(-1) * TimeResolution * timeout_time_resolution;
        return NtWaitForSingleObject(get(), alertable, &interval);
      }
    };

  }// namespace nt
}// namespace ntl

#endif //