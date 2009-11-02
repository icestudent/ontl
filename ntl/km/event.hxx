/**\file*********************************************************************
 *                                                                     \brief
 *  Events
 *
 ****************************************************************************
 */
#ifndef NTL__KM_EVENT
#define NTL__KM_EVENT
#pragma once

#include "basedef.hxx"
#include "../nt/event.hxx"

namespace ntl {
  namespace km {

    using nt::event_type;
    using nt::SynchronizationEvent; // auto-reset
    using nt::NotificationEvent;    // manual-reset

    struct kevent { dispatcher_header Header; };


    NTL__EXTERNAPI
    void __stdcall
      KeInitializeEvent(
        kevent *    Event,
        event_type  Type,
        bool        State
        );

    NTL__EXTERNAPI
    long __stdcall
      KeSetEvent(
        kevent *  Event,
        kpriority Increment,
        bool      Wait
        );

    NTL__EXTERNAPI void __stdcall KeClearEvent(kevent* Event);
    NTL__EXTERNAPI long __stdcall KeResetEvent(kevent* Event);
    NTL__EXTERNAPI long __stdcall KeReadStateEvent(kevent* Event);




    template <event_type Type>
    struct event : kevent
    {
      event(bool Signaled = false)
      {
        KeInitializeEvent(this, Type, Signaled);
      }

      long set(kpriority Increment = 0, bool Wait = false)
      {
        return KeSetEvent(this, Increment, Wait);
      }
      long reset() { return KeResetEvent(this); }
      void clear() { return KeClearEvent(this); }

      long read_state() const { return KeReadStateEvent(this); }
    };

    typedef event<NotificationEvent> notification_event;
    typedef event<SynchronizationEvent> synchronization_event;

  }// namespace km
}// namespace ntl

#endif //
