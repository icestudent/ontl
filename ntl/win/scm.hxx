/**\file*********************************************************************
 *                                                                     \brief
 *  Service Control Manager API
 *
 ****************************************************************************
 */
#ifndef NTL__WIN_SCM
#define NTL__WIN_SCM
#pragma once

#include "windef.hxx"
#include "../nt/service.hxx"

#ifndef NTL__NO_AUTOLINK
# pragma comment(lib, "advapi32") // from PSDK
#endif

//#pragma warning(disable:4061)// enumerator in switch of enum is not explicitly handled by a case label
//#pragma warning(disable:4127)// conditional expression is constant

namespace ntl {
namespace win {
/**\addtogroup  winapi_types_support *** Win32 API support library **********
 *@{*/


typedef const struct _opaque { } * legacy_sc_handle;
enum sc_status_type { sc_status_process_info };

NTL_EXTERNAPI
legacy_sc_handle __stdcall
  OpenSCManagerW(
    const wchar_t * lpMachineName,
    const wchar_t * lpDatabaseName,
    uint32_t        dwDesiredAccess
    );

NTL_EXTERNAPI
legacy_sc_handle __stdcall
  OpenServiceW(
    legacy_sc_handle  hSCManager,
    const wchar_t *   lpServiceName,
    uint32_t          dwDesiredAccess
    );

NTL_EXTERNAPI
int __stdcall
  CloseServiceHandle(
    legacy_sc_handle hSCObject
    );

NTL_EXTERNAPI
legacy_sc_handle __stdcall
  CreateServiceW(
    legacy_sc_handle  hSCManager,
    const wchar_t *   lpServiceName,
    const wchar_t *   lpDisplayName,
    uint32_t          dwDesiredAccess,
    uint32_t          dwServiceType,
    uint32_t          dwStartType,
    uint32_t          dwErrorControl,
    const wchar_t *   lpBinaryPathName,
    const wchar_t *   lpLoadOrderGroup,
    uint32_t *        lpdwTagId,
    const wchar_t *   lpDependencies,
    const wchar_t *   lpServiceStartName,
    const wchar_t *   lpPassword
    );

NTL_EXTERNAPI
int __stdcall
  DeleteService(
    legacy_sc_handle hService
    );

NTL_EXTERNAPI
int __stdcall
  StartServiceW(
    legacy_sc_handle  hService,
    uint32_t          dwNumServiceArgs,
    const wchar_t *   lpServiceArgVectors
    );

NTL_EXTERNAPI
int __stdcall
  ControlService(
    legacy_sc_handle      hService,
    uint32_t              dwControl,
    nt::service::status * lpServiceStatus
    );

NTL_EXTERNAPI
int __stdcall
  QueryServiceStatusEx(
    legacy_sc_handle      hService,
    sc_status_type        InfoLevel,
    void*                 lpBuffer,
    uint32_t              dwBufSize,
    uint32_t*             lpdwBytesNeeded
    );

/// Service Control Manager
class sc_manager
{
    sc_manager(const sc_manager &);
    const sc_manager & operator=(const sc_manager &);

  ///////////////////////////////////////////////////////////////////////////
  public:

    enum access_mask
    {
      connect             = 0x0001,
      create_service      = 0x0002,
      enumerate_service   = 0x0004,
      lock                = 0x0008,
      query_lock_status   = 0x0010,
      modify_boot_config  = 0x0020,
      all_acess = standard_rights_required | connect | create_service
                | enumerate_service | lock | query_lock_status | modify_boot_config
    };

  friend
    access_mask operator | (access_mask m, access_mask m2)
    {
      return bitwise_or(m, m2);
    }

    explicit
    sc_manager(
      access_mask     desired_acess = connect,
      const wchar_t * database_name = 0,  // active_database(),
      const wchar_t * machine_name  = 0   // local machine
      )
    : handle(OpenSCManagerW(machine_name, database_name, desired_acess))
    {/**/}

    ~sc_manager()
    {
      if ( handle ) CloseServiceHandle(handle);
    }

    operator legacy_sc_handle () const { return handle; }

    static const wchar_t * active_database() { return L"ServicesActive"; }
    static const wchar_t * failed_database() { return L"ServicesFailed"; }

  ///////////////////////////////////////////////////////////////////////////
  private:

    const legacy_sc_handle handle;

};//template class sc_manager



/// Service Controller class
class service_control
{
    service_control();
    service_control(const service_control &);
    const service_control & operator=(const service_control &);

  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef nt::service::status service_status;

    service_control(
      const sc_manager        & scm,
      const wchar_t           * service_name,
      nt::service::access_mask  desired_acess)
      : handle(_open(scm, service_name, desired_acess))
    {/**/}

    ~service_control() { close_handle(); }

    static const size_t max_display_name = 256;

    operator const void * () const { return handle; }

    static
    legacy_sc_handle
      _open(
      const sc_manager        & manager,
      const wchar_t           * service_name,
      nt::service::access_mask  desired_acess)
    {
      return OpenServiceW(manager, service_name, desired_acess);
    }

    bool start(uint32_t argc = 0, const wchar_t * argv = 0) const
    {
      return 0 != StartServiceW(handle, argc, argv);
    }

    const service_status * operator->() const __ntl_nothrow
    {
      return handle ? &status : reinterpret_cast<const service_status*>(handle);
    }

    bool operator()(nt::service::control_code command)
    {
      return 0 != ControlService(handle, command, &status);
    }

    bool stop()     { return (*this)(nt::service::control_stop); }
    bool pause()    { return (*this)(nt::service::control_pause); }
    bool resume()   { return (*this)(nt::service::control_continue); }
    bool shutdown() { return (*this)(nt::service::control_shutdown); }

    bool remove()
    {
      const bool r = 0 != DeleteService(handle);
      close_handle();
      return r;
    }

    bool query_status(nt::service::status_process& service_status)
    {
      uint32_t cb;
      return 0 != QueryServiceStatusEx(handle, sc_status_process_info, &service_status, sizeof(nt::service::status_process), &cb);
    }

  ///////////////////////////////////////////////////////////////////////////
  private:

    legacy_sc_handle  handle;
    service_status    status;

    void close_handle()
    {
      if ( handle && CloseServiceHandle(handle) ) handle = 0;
    }

};//template class service_control

/**@} winapi_types_support */
}//namespace win
}//namespace ntl

#endif//#ifndef NTL__WIN_SCM
