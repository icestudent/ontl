/**\file*********************************************************************
*                                                                     \brief
*  Environment variables
*
****************************************************************************
*/
#ifndef NTL__NT_ENVIRON
#define NTL__NT_ENVIRON
#pragma once

#include "basedef.hxx"
#include "string.hxx"

namespace ntl {
  namespace nt {
/*
    NtEnumerateSystemEnvironmentValuesEx
      NtEnumerateSystemEnvironmentValuesEx
      NtQuerySystemEnvironmentValue
      NtQuerySystemEnvironmentValue
      NtQuerySystemEnvironmentValueEx
      NtQuerySystemEnvironmentValueEx
      NtSetSystemEnvironmentValue
      NtSetSystemEnvironmentValue
      NtSetSystemEnvironmentValueEx
      NtSetSystemEnvironmentValueEx
      RtlCreateEnvironment
      RtlCreateEnvironment
      RtlCreateEnvironmentEx
      RtlCreateEnvironmentEx
      RtlDestroyEnvironment
      RtlDestroyEnvironment
      RtlExpandEnvironmentStrings
      RtlExpandEnvironmentStrings
      RtlExpandEnvironmentStrings_U
      RtlExpandEnvironmentStrings_U
      RtlQueryEnvironmentVariable
      RtlQueryEnvironmentVariable
      RtlQueryEnvironmentVariable_U
      RtlQueryEnvironmentVariable_U
      RtlSetCurrentEnvironment
      RtlSetCurrentEnvironment
      RtlSetEnvironmentStrings
      RtlSetEnvironmentStrings
      RtlSetEnvironmentVar
      RtlSetEnvironmentVar
      RtlSetEnvironmentVariable
      RtlSetEnvironmentVariable
      ZwEnumerateSystemEnvironmentValuesEx
      ZwEnumerateSystemEnvironmentValuesEx
      ZwQuerySystemEnvironmentValue
      ZwQuerySystemEnvironmentValue
      ZwQuerySystemEnvironmentValueEx
      ZwQuerySystemEnvironmentValueEx
      ZwSetSystemEnvironmentValue
      ZwSetSystemEnvironmentValue
      ZwSetSystemEnvironmentValueEx
      ZwSetSystemEnvironmentValueEx
**/


NTL__EXTERNAPI
ntstatus __stdcall RtlSetEnvironmentVariable(void** Environment, const const_unicode_string& Name, const const_unicode_string& Value);


NTL__EXTERNAPI
ntstatus __stdcall RtlQueryEnvironmentVariable_U(void* Environment, const const_unicode_string& Name, unicode_string& Value);





  }
}

#endif //NTL__NT_ENVIRON
