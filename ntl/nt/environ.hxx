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

    NTL_EXTERNAPI
    ntstatus __stdcall RtlCreateEnvironment(bool CloneCurrentEnvironment, void** Environment);
    NTL_EXTERNAPI
    ntstatus __stdcall RtlDestroyEnvironment(void* Environment);

    NTL_EXTERNAPI
    ntstatus __stdcall RtlSetEnvironmentVariable(void** Environment, const const_unicode_string& Name, const const_unicode_string& Value);


    NTL_EXTERNAPI
    ntstatus __stdcall RtlQueryEnvironmentVariable_U(void* Environment, const const_unicode_string& Name, unicode_string& Value);



    class environment
    {
      void* env;
    public:
      static environment assign(void* env)
      {
        environment re; re.env = env;
        return re;
      }
      static environment create(bool clone_current = true)
      {
        void* env = nullptr;
        /*ntstatus st =*/ RtlCreateEnvironment(clone_current, &env);
        return assign(env);
      }

      environment()
      {
        env = peb::instance().process_parameters()->Environment;
      }

      ntstatus destroy()
      {
        const ntstatus st = RtlDestroyEnvironment(env);
        env = nullptr;
        return st;
      }

      bool exists(const const_unicode_string& name) const
      {
        unicode_string us;
        return RtlQueryEnvironmentVariable_U(env, name, us) != status::variable_not_found;
      }

      ntstatus query(const const_unicode_string& name, unicode_string& value) const
      {
        return RtlQueryEnvironmentVariable_U(env, name, value);
      }

      ntstatus set(const const_unicode_string& name, const const_unicode_string& value)
      {
        return RtlSetEnvironmentVariable(&env, name, value);
      }
    };


  }
}
#endif //NTL__NT_ENVIRON
