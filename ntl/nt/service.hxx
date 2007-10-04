/**\file*********************************************************************
 *                                                                     \brief
 *  NT Services
 *
 ****************************************************************************
 */

#ifndef NTL__NT_SERVICE
#define NTL__NT_SERVICE

#include "string.hxx"

namespace ntl {
namespace nt {


/**\addtogroup  NT_services_support *** NT Services support library ***********
 *@{*/


NTL__EXTERNAPI
ntstatus __stdcall
  NtLoadDriver(
    const const_unicode_string *  DriverServiceName
    );

NTL__EXTERNAPI
ntstatus __stdcall
  NtUnloadDriver(
    const const_unicode_string *  DriverServiceName
    );


static inline
ntstatus
  load_driver(const const_unicode_string & service_name)
{
  return NtLoadDriver(&service_name);
}

static inline
ntstatus
  unload_driver(const const_unicode_string & service_name)
{
  return NtUnloadDriver(&service_name);
}

static inline
ntstatus
  load_driver(const std::wstring & service_name)
{
  return load_driver(const_unicode_string(service_name));
}

static inline
ntstatus
  unload_driver(const std::wstring & service_name)
{
  return unload_driver(const_unicode_string(service_name));
}


struct service
{
  enum type
  {
    kernel_driver       = 0x001,
    file_system_driver  = 0x002,
    adapter             = 0x004,
    recognizer_driver   = 0x008,
    driver              = kernel_driver | file_system_driver | recognizer_driver,
    win32_own_process   = 0x010,
    win32_share_process = 0x020,
    win32               = win32_own_process | win32_share_process,
    interactive_process = 0x100,
    all     = win32 | adapter | driver | interactive_process
  };


  enum access_mask
  {
    qeury_config          = 0x0001,   change_config         = 0x0002,
    qeury_status          = 0x0004,   enumerate_dependents  = 0x0008,
    start                 = 0x0010,   stop                  = 0x0020,
    pause_continue        = 0x0040,   interrogate           = 0x0080,
    user_defined_control  = 0x0100,
    all_acess = standard_rights_required | qeury_config | change_config 
              | qeury_status | enumerate_dependents | start | stop | interrogate
              | user_defined_control
  };

  friend access_mask operator | (access_mask m, access_mask m2) 
  { 
    return bitwise_or(m, m2);
  }


  enum start_type
  {
    boot_start    = 0,  ///< Device driver started by the system loader.
    system_start  = 1,  ///< Device driver started by the IoInitSystem function.
    auto_start    = 2,  ///< Service started automatically during system startup. 
    demand_start  = 3,  ///< Service started by the service control manager. 
    disabled      = 4   ///< Service cannot be started.
  };


  enum control_code
  {
    control_stop                  = 0x1,
    control_pause                 = 0x2,
    control_continue              = 0x3,
    control_interrogate           = 0x4,
    control_shutdown              = 0x5,
    control_paramchange           = 0x6,
    control_netbindadd            = 0x7,
    control_netbindremove         = 0x8,
    control_netbindenable         = 0x9,
    control_netbinddisable        = 0xA,
    control_deviceevent           = 0xB,
    control_hardwareprofilechange = 0xC,
    control_powerevent            = 0xD,
    control_sessionchange         = 0xE
  };

  enum error_control
  {
    error_ignore    = 0,
    error_normal    = 1,
    error_severe    = 2,
    error_critical  = 3
  };

  enum state
  {
    indeterminable    = 0,  ///< Error during status request
    stopped           = 1,
    start_pending     = 2,
    stop_pending      = 3,
    running           = 4,
    continue_pending  = 5,
    pause_pending     = 6,
    paused            = 7
  };


  struct status
  {
    uint32_t  dwServiceType;
    uint32_t  dwCurrentState;
    uint32_t  dwControlsAccepted;
    uint32_t  dwWin32ExitCode;
    uint32_t  dwServiceSpecificExitCode;
    uint32_t  dwCheckPoint;
    uint32_t  dwWaitHint;

    type          service_ype() const   { return static_cast<type>(dwServiceType); }
    state         current_state() const { return static_cast<state>(dwCurrentState); }
    control_code  controls_accepted() const { return static_cast<control_code>(dwControlsAccepted); }
    uint32_t      wait_hint() const { return dwWaitHint; }
  };

  struct status_process : public service::status
  {
    uint32_t  dwProcessId;
    uint32_t  dwServiceFlags;
  };
  


};


/**@} NT_services_support */

}//namespace nt
}//namespace ntl

#endif//#ifndef NTL__NT_SERVICE
