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

    enum event_type { NotificationEvent, SynchronizationEvent };

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
      object_attributes*  ObjectAttributes,
      event_type          EventType,
      bool                InitialState
      );

    NTL__EXTERNAPI
      ntstatus __stdcall
      NtOpenEvent(
      handle*             EventHandle,
      uint32_t            DesiredAccess,
      object_attributes*  ObjectAttributes
      );

    class user_event:
      public handle,
      public device_traits<user_event>
    {
    public:

      static ntstatus
        create(
        handle*             EventHandle,
        access_mask         DesiredAccess,
        object_attributes*  ObjectAttributes,
        event_type          EventType,
        bool                InitialState
        )
      {
        return NtCreateEvent(EventHandle, DesiredAccess, ObjectAttributes, EventType, InitialState);
      }

      static ntstatus
        open(
        handle*             EventHandle,
        access_mask         DesiredAccess,
        object_attributes*  ObjectAttributes
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

      ntstatus wait(bool alertable = true, int64_t timeout = -1)
      {
        return NtWaitForSingleObject(get(), alertable, &timeout);
      }
    };

  }// namespace nt
}// namespace ntl

#endif //