/**\file*********************************************************************
*                                                                     \brief
*  Client-Server Runtime
*
****************************************************************************
*/
#ifndef NTL__NT_CSR
#define NTL__NT_CSR
#pragma once

#include "basedef.hxx"
#include "string.hxx"

namespace ntl {
  namespace nt {

typedef uint32_t csr_api_number;

typedef uint32_t (__stdcall *csr_callback_routine_t)(
    struct csr_api_msg* ReplyMsg
    );

struct port_message {
    union {
        struct {
            int16_t DataLength;
            int16_t TotalLength;
        } s1;
        uint32_t Length;
    } u1;
    union {
        struct {
            int16_t Type;
            int16_t DataInfoOffset;
        } s2;
        uint32_t ZeroInit;
    } u2;
    union {
        client_id ClientId;
        double DoNotUseThisField;
    };
    uint32_t MessageId;
    union {
        uint32_t ClientViewSize;
        uint32_t CallbackId;
    };
//  uint8_t Data[];
};

struct csr_api_connectinfo {
  uint32_t ExpectedVersion;
  uint32_t CurrentVersion;
  legacy_handle ObjectDirectory;
  void* SharedSectionBase;
  void* SharedStaticServerData;
  void* SharedSectionHeap;
  uint32_t DebugFlags;
  uint32_t SizeOfPebData;
  uint32_t SizeOfTebData;
  uint32_t NumberOfServerDllNames;
};

struct csr_capture_header {
  uint32_t Length;
  csr_capture_header *RelatedCaptureBuffer;
  uint32_t CountMessagePointers;
  uint32_t CountCapturePointers;
  uint32_t* MessagePointerOffsets;
  uint32_t* CapturePointerOffsets;
  int8_t* FreeSpace;
};

struct csr_nullapicall_msg {
  int32_t  CountArguments;
  uint32_t FastArguments[ 12 ];
  int8_t* *Arguments;
};

struct csr_clientconnect_msg {
  uint32_t ServerDllIndex;
  void* ConnectionInformation;
  uint32_t ConnectionInformationLength;
};

struct csr_threadconnect_msg {
  legacy_handle SectionHandle;
  legacy_handle EventPairHandle;
  int8_t* MessageStack;
  uint32_t MessageStackSize;
  uint32_t RemoteViewDelta;
};

struct csr_profile_control_msg {
  uint32_t ProfileControlFlag;
};

struct csr_identify_alertable_msg {
  client_id ClientId;
};

struct csr_setpriority_class_msg {
  legacy_handle ProcessHandle;
  uint32_t PriorityClass;
};

struct csr_callback_info
{
    uint32_t ApiNumberBase;
    uint32_t MaxApiNumber;
    csr_callback_routine_t** CallbackDispatchTable;
};

struct csr_api_msg {
  port_message h;
  union {
    csr_api_connectinfo ConnectionRequest;
    struct resuest {
      csr_capture_header* CaptureBuffer;
      csr_api_number ApiNumber;
      uint32_t ReturnValue;
      uint32_t Reserved;
      union {
        csr_nullapicall_msg NullApiCall;
        csr_clientconnect_msg ClientConnect;
        csr_threadconnect_msg ThreadConnect;
        csr_profile_control_msg ProfileControl;
        csr_identify_alertable_msg IndentifyAlertable;
        csr_setpriority_class_msg PriorityClass;
        uint32_t ApiMessageData[30];
      } u;
    };
  };
};


/************************************************************************/
/* Base api msg                                                         */
/************************************************************************/
//
// This structure is filled in by the client prior to connecting to the BASESRV
// DLL in the Windows subsystem server.  The server DLL will fill in the OUT
// fields if prior to accepting the connection.
//

struct basesrv_api_connectinfo
{
  uint32_t ExpectedVersion;
  legacy_handle DefaultObjectDirectory;
  uint32_t WindowsVersion;
  uint32_t CurrentVersion;
  uint32_t DebugFlags;
  wchar_t WindowsDirectory[260];
  wchar_t WindowsSystemDirectory[260];
};

#define BASESRV_VERSION 0x10000

//
// Message format for messages sent from the client to the server
//

enum BasesrvApiNumber {
  BasepCreateProcess,
  BasepCreateThread,
  BasepGetTempFile,
  BasepExitProcess,
  BasepDebugProcess,
  BasepCheckVDM,
  BasepUpdateVDMEntry,
  BasepGetNextVDMCommand,
  BasepExitVDM,
  BasepIsFirstVDM,
  BasepGetVDMExitCode,
  BasepSetReenterCount,
  BasepSetProcessShutdownParam,
  BasepGetProcessShutdownParam,
  BasepNlsSetUserInfo,
  BasepNlsSetMultipleUserInfo,
  BasepNlsCreateSortSection,
  BasepNlsPreserveSection,
  BasepSetVDMCurDirs,
  BasepGetVDMCurDirs,
  BasepBatNotification,
  BasepRegisterWowExec,
  BasepSoundSentryNotification,
  BasepRefreshIniFileMapping,
  BasepDefineDosDevice,
  BasepMaxApiNumber
};

struct base_nls_set_user_info_msg {
  wchar_t* pValue;
  wchar_t* pCacheString;
  wchar_t* pData;
  uint32_t DataLength;
};

struct base_nls_set_multiple_user_info_msg {
  uint32_t Flags;
  uint32_t DataLength;
  wchar_t* pPicture;
  wchar_t* pSeparator;
  wchar_t* pOrder;
  wchar_t* pTLZero;
  wchar_t* pTimeMarkPosn;
};

struct base_nls_create_sort_section_msg {
  raw_unicode_string SectionName;
  legacy_handle hNewSection;
  int64_t SectionSize;
};

struct base_nls_preserve_section_msg {
  legacy_handle hSection;
};

struct base_shutdownparam_msg {
  uint32_t ShutdownLevel;
  uint32_t ShutdownFlags;
};

struct base_createprocess_msg {
  legacy_handle ProcessHandle;
  legacy_handle ThreadHandle;
  client_id ClientId;
  client_id DebuggerClientId;
  uint32_t CreationFlags;
  uint32_t IsVDM;
  legacy_handle hVDM;
};

struct base_createthread_msg {
  legacy_handle ThreadHandle;
  client_id ClientId;
};

struct base_gettempfile_msg {
  uint32_t uUnique;
};

struct base_exitprocess_msg {
  uint32_t uExitCode;
};

struct base_debugprocess_msg {
  uint32_t dwProcessId;
  client_id DebuggerClientId;
  void* AttachCompleteRoutine;
};

struct base_checkvdm_msg {
  uint32_t  iTask;
  legacy_handle ConsoleHandle;
  uint32_t  BinaryType;
  legacy_handle WaitObjectForParent;
  legacy_handle StdIn;
  legacy_handle StdOut;
  legacy_handle StdErr;
  uint32_t  CodePage;
  uint32_t  dwCreationFlags;
  char*  CmdLine;
  char*  AppName;
  char*  PifFile;
  char*  CurDirectory;
  char*  Env;
  uint32_t  EnvLen;
  struct startupinfoa* StartupInfo;
  char*  Desktop;
  uint32_t  DesktopLen;
  char*  Title;
  uint32_t  TitleLen;
  char*  Reserved;
  uint32_t  ReservedLen;
  uint16_t CmdLen;
  uint16_t AppLen;
  uint16_t PifLen;
  uint16_t CurDirectoryLen;
  uint16_t CurDrive;
  uint16_t VDMState;
};

struct base_update_vdm_entry_msg {
  uint32_t  iTask;
  uint32_t  BinaryType;
  legacy_handle ConsoleHandle;
  legacy_handle VDMProcessHandle;
  legacy_handle WaitObjectForParent;
  int16_t   EntryIndex;
  int16_t   VDMCreationState;
} ;

struct base_get_next_vdm_command_msg {
  uint32_t  iTask;
  legacy_handle ConsoleHandle;
  legacy_handle WaitObjectForVDM;
  legacy_handle StdIn;
  legacy_handle StdOut;
  legacy_handle StdErr;
  uint32_t  CodePage;
  uint32_t  dwCreationFlags;
  uint32_t  ExitCode;
  char*  CmdLine;
  char*  AppName;
  char*  PifFile;
  char*  CurDirectory;
  char*  Env;
  uint32_t  EnvLen;
  startupinfoa* StartupInfo;
  char*  Desktop;
  uint32_t  DesktopLen;
  char*  Title;
  uint32_t  TitleLen;
  char*  Reserved;
  uint32_t  ReservedLen;
  uint16_t CurrentDrive;
  uint16_t CmdLen;
  uint16_t AppLen;
  uint16_t PifLen;
  uint16_t CurDirectoryLen;
  uint16_t VDMState;
  uint32_t fComingFromBat;
};

struct base_exit_vdm_msg {
  legacy_handle ConsoleHandle;
  uint32_t  iWowTask;
  legacy_handle WaitObjectForVDM;
};

struct base_set_reenter_count_msg {
  legacy_handle ConsoleHandle;
  uint32_t  fIncDec;
};

struct base_is_first_vdm_msg {
  uint32_t    FirstVDM;
};

struct base_get_vdm_exit_code_msg {
  legacy_handle ConsoleHandle;
  legacy_handle hParent;
  uint32_t  ExitCode;
};

struct base_get_set_vdm_cur_dirs_msg {
  legacy_handle ConsoleHandle;
  char*  lpszzCurDirs;
  uint32_t  cchCurDirs;
};

struct base_bat_notification_msg {
  legacy_handle ConsoleHandle;
  uint32_t  fBeginEnd;
};

struct base_register_wowexec_msg {
  legacy_handle hwndWowExec;
};

struct base_soundsentry_notification_msg {
  uint32_t  VideoMode;
};

struct base_refreshinifilemapping_msg {
  raw_unicode_string IniFileName;
};

struct base_definedosdevice_msg {
  uint32_t Flags;
  raw_unicode_string DeviceName;
  raw_unicode_string TargetPath;
};

struct base_api_msg {
  port_message h;
  csr_capture_header* CaptureBuffer;
  csr_api_number ApiNumber;
  uint32_t ReturnValue;
  uint32_t Reserved;
  union {
    base_nls_set_user_info_msg NlsSetUserInfo;
    base_nls_set_multiple_user_info_msg NlsSetMultipleUserInfo;
    base_nls_create_sort_section_msg NlsCreateSortSection;
    base_nls_preserve_section_msg NlsPreserveSection;
    base_shutdownparam_msg ShutdownParam;
    base_createprocess_msg CreateProcess;
    base_createthread_msg CreateThread;
    base_gettempfile_msg GetTempFile;
    base_exitprocess_msg ExitProcess;
    base_debugprocess_msg DebugProcess;
    base_checkvdm_msg CheckVDM;
    base_update_vdm_entry_msg UpdateVDMEntry;
    base_get_next_vdm_command_msg GetNextVDMCommand;
    base_exit_vdm_msg ExitVDM;
    base_is_first_vdm_msg IsFirstVDM;
    base_get_vdm_exit_code_msg GetVDMExitCode;
    base_set_reenter_count_msg SetReenterCount;
    base_get_set_vdm_cur_dirs_msg GetSetVDMCurDirs;
    base_bat_notification_msg BatNotification;
    base_register_wowexec_msg RegisterWowExec;
    base_soundsentry_notification_msg SoundSentryNotification;
    base_refreshinifilemapping_msg RefreshIniFileMapping;
    base_definedosdevice_msg DefineDosDeviceApi;
  } u;
};

#define MAKE_API_NUMBER(DllIndex, ApiIndex) (csr_api_number)((DllIndex) << 16) | (ApiIndex)

//////////////////////////////////////////////////////////////////////////

NTL__EXTERNAPI
ntstatus __stdcall
CsrClientConnectToServer(
    wchar_t* ObjectDirectory,
    uint32_t ServertDllIndex,
    csr_callback_info* CallbackInformation,
    void* ConnectionInformation,
    uint32_t* ConnectionInformationLength,
    uint32_t* CalledFromServer
    );

NTL__EXTERNAPI
ntstatus __stdcall
CsrClientCallServer(
    csr_api_msg* m,
    csr_capture_header* CaptureBuffer,
    csr_api_number ApiNumber,
    uint32_t ArgLength
    );

NTL__EXTERNAPI
csr_capture_header* __stdcall
CsrAllocateCaptureBuffer(
    uint32_t CountMessagePointers,
    uint32_t CountCapturePointers,
    uint32_t Size
    );

NTL__EXTERNAPI
void __stdcall
CsrFreeCaptureBuffer(
    csr_capture_header* CaptureBuffer
    );

NTL__EXTERNAPI
uint32_t __stdcall
CsrAllocateMessagePointer(
    csr_capture_header* CaptureBuffer,
    uint32_t Length,
    void** Pointer
    );

NTL__EXTERNAPI
uint32_t __stdcall
CsrAllocateCapturePointer(
    csr_capture_header* CaptureBuffer,
    uint32_t Length,
    void** Pointer
    );

NTL__EXTERNAPI
void __stdcall
CsrCaptureMessageBuffer(
    csr_capture_header* CaptureBuffer,
    void* Buffer,
    uint32_t Length,
    void** CapturedBuffer
    );

NTL__EXTERNAPI
void __stdcall
CsrCaptureMessageString(
    csr_capture_header* CaptureBuffer,
    const char* String,
    uint32_t Length,
    uint32_t MaximumLength,
    ansi_string* CapturedString
    );

NTL__EXTERNAPI
int64_t* __stdcall
CsrCaptureTimeout(
    uint32_t Milliseconds,
    int64_t* Timeout
    );

NTL__EXTERNAPI
void __stdcall
CsrProbeForWrite(
    void* Address,
    uint32_t Length,
    uint32_t Alignment
    );

NTL__EXTERNAPI
void __stdcall
CsrProbeForRead(
    void* Address,
    uint32_t Length,
    uint32_t Alignment
    );

NTL__EXTERNAPI
ntstatus __stdcall
CsrNewThread();

NTL__EXTERNAPI
ntstatus __stdcall
CsrIdentifyAlertableThread();

NTL__EXTERNAPI
ntstatus __stdcall
CsrSetPriorityClass(
    legacy_handle ProcessHandle,
    uint32_t* PriorityClass
    );

  void some_nt_func0();
  void some_nt_func(ntstatus);
  }
}

#endif //NTL__NT_CSR
