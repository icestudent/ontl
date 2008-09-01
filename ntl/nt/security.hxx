/**\file*********************************************************************
 *                                                                     \brief
 *  NT Objects
 *
 ****************************************************************************
 */

#ifndef NTL__NT_SECURITY
#define NTL__NT_SECURITY

#include "basedef.hxx"

namespace ntl {
namespace nt {

  typedef const struct { }* access_token;

#pragma warning(push,4)
#pragma warning(disable:4201)
  struct access_mask_t
  {
    union
    {
      struct
      {
        uint16_t  SpecificRights;
        uint8_t   StandardRights;
        uint8_t   AccessSystemAcl: 1;
        uint8_t   Reserved: 3;
        uint8_t   GenericAll: 1;
        uint8_t   GenericExecute: 1;
        uint8_t   GenericWrite: 1;
        uint8_t   GenericRead: 1;
      };
      access_mask DesiredAccess;
    };
  };
#pragma warning(pop)

#pragma pack(push, 4)

  typedef int64_t luid;

  struct luid_and_attributes
  {
    luid      Luid;
    uint32_t  Attributes;
  };
#pragma pack(pop)

  struct sid_identifier_authority
  {
    uint8_t   Value[6];
  };

  struct sid
  {
    uint8_t   Revision;
    uint8_t   SubAuthorityCount;
    sid_identifier_authority IdentifierAuthority;
    uint32_t  SubAuthority[1];
  };

  struct sid_and_attributes
  {
    sid*      Sid;
    uint32_t  Attributes;
  };

  // ...

  struct acl
  {
    uint8_t   AclRevision;
    uint8_t   Sbz1;
    uint16_t  AclSize;
    uint16_t  AceCount;
    uint16_t  Sbz2;
  };

  struct security_descriptor
  {
    uint8_t   Revision;
    uint8_t   Sbz1;
    uint16_t  Control;
    sid*      Owner;
    sid*      Group;
    acl*      Sacl;
    acl*      Dacl;
  };

  namespace privilege
  {
    enum type
    {
      min_well_known       = 2,
      create_token         = 2,
      assignprimarytoken,
      lock_memory,
      increase_quota,
      unsolicited_input,

      machine_account      = 6,
      tcb,
      security,
      take_ownership,
      load_driver,
      system_profile,
      systemtime,
      prof_single_process,
      inc_base_priority,
      create_pagefile,
      create_permanent,
      backup,
      restore,
      shutdown,
      debug,
      audit,
      system_environment,
      change_notify,
      remote_shutdown,
      max_well_known        = remote_shutdown
    };
  } // privilege
}//namespace nt
}//namespace ntl


#endif//#ifndef NTL__NT_SECURITY
