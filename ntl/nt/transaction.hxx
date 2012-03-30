/**\file*********************************************************************
*                                                                     \brief
*  Kernel Transaction Manager
*
****************************************************************************
*/
#ifndef NTL__NT_TRANSACTION
#define NTL__NT_TRANSACTION
#pragma once

#include "basedef.hxx"
#include "object.hxx"
#include "string.hxx"

namespace ntl {
  namespace nt {
    enum transaction_information_class
    {
      TransactionBasicInformation,
      TransactionPropertiesInformation,
      TransactionEnlistmentInformation,
      TransactionSuperiorEnlistmentInformation
    };

    enum transaction_state
    {
      TransactionStateNormal = 1,
      TransactionStateIndoubt,
      TransactionStateCommittedNotify,
    };

    enum transaction_outcome
    {
      TransactionOutcomeUndetermined = 1,
      TransactionOutcomeCommitted,
      TransactionOutcomeAborted,
    };

    struct transaction_basic_information
    {
      static const transaction_information_class info_class_type = TransactionBasicInformation;
      guid  TransactionId;
      transaction_state  State;
      transaction_outcome  Outcome;
    };

    struct transaction_properties_information
    {
      static const transaction_information_class info_class_type = TransactionPropertiesInformation;
      uint32_t  IsolationLevel;
      uint32_t  IsolationFlags;
      int64_t  Timeout;
      uint32_t  Outcome;
      uint32_t  DescriptionLength;
      wchar_t  Description[1];
    };

    struct transaction_enlistment_pair
    {
      guid  EnlistmentId;
      guid  ResourceManagerId;
    };

    struct transaction_enlistments_information
    {
      static const transaction_information_class info_class_type = TransactionEnlistmentInformation;
      uint32_t  NumberOfEnlistments;
      transaction_enlistment_pair  EnlistmentPair[1];
    };




    NTL_EXTERNAPI
    ntstatus __stdcall NtCreateTransaction(
      legacy_handle* TransactionHandle,
      uint32_t  DesiredAccess,
      const object_attributes* ObjectAttributes,
      const guid*  Uow,
      legacy_handle TmHandle,
      uint32_t  CreateOptions,
      uint32_t  IsolationLevel,
      uint32_t  IsolationFlags,
      const int64_t*  Timeout,
      const const_unicode_string* Description 
      );

    NTL_EXTERNAPI
    ntstatus __stdcall NtOpenTransaction(
      legacy_handle* TransactionHandle,
      uint32_t  DesiredAccess,
      object_attributes*  ObjectAttributes,
      const guid* Uow,
      legacy_handle TmHandle
      );

    NTL_EXTERNAPI
      ntstatus __stdcall NtCommitTransaction(legacy_handle TransactionHandle, bool Wait);
    NTL_EXTERNAPI
      ntstatus __stdcall NtRollbackTransaction(legacy_handle TransactionHandle, bool Wait);

    NTL_EXTERNAPI
      ntstatus __stdcall NtQueryInformationTransaction(legacy_handle TransactionHandle, transaction_information_class InformationClass, void* Information, uint32_t InformationLength, uint32_t* ReturnLength);
    NTL_EXTERNAPI
      ntstatus __stdcall NtSetInformationTransaction(legacy_handle TransactionHandle, transaction_information_class InformationClass, const void* Information, uint32_t InformationLength);

    NTL_EXTERNAPI
      legacy_handle __stdcall RtlGetCurrentTransaction();
    NTL_EXTERNAPI
      void __stdcall RtlSetCurrentTransaction(legacy_handle TransactionHandle);

    NTL_EXTERNAPI
      ntstatus __stdcall NtCreateTransactionManager();
    NTL_EXTERNAPI
      ntstatus __stdcall NtEnumerateTransactionObject();
    NTL_EXTERNAPI
      ntstatus __stdcall NtFreezeTransactions();
    NTL_EXTERNAPI
      ntstatus __stdcall NtOpenTransactionManager();
    NTL_EXTERNAPI
      ntstatus __stdcall NtQueryInformationTransactionManager();
    NTL_EXTERNAPI
      ntstatus __stdcall NtRecoverTransactionManager();
    NTL_EXTERNAPI
      ntstatus __stdcall NtRenameTransactionManager();
    NTL_EXTERNAPI
      ntstatus __stdcall NtRollforwardTransactionManager();
    NTL_EXTERNAPI
      ntstatus __stdcall NtSetInformationTransactionManager();
    NTL_EXTERNAPI
      ntstatus __stdcall NtThawTransactions();

  } 
}
#endif //NTL__NT_TRANSACTION
