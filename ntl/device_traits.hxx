/**\file*********************************************************************
 *                                                                     \brief
 *  Device traits
 *
 ****************************************************************************
 */
#ifndef NTL__DEVICE_TRAITS
#define NTL__DEVICE_TRAITS
#pragma once

#include "basedef.hxx"
#include "stdlib.hxx"

namespace ntl {


template<class T = void>
struct device_traits;

template<>
struct device_traits<void>
{
  /// Masks for the predefined standard access types
  enum access_mask
  {
    any_access = 0,
    delete_access             = 0x00010000L,
    read_control              = 0x00020000L,
    write_dac                 = 0x00040000L,
    write_owner               = 0x00080000L,
    synchronize               = 0x00100000L,
    standard_rights_required  = delete_access | read_control | write_dac | write_owner,
    standard_rights_read      = read_control,
    standard_rights_write     = read_control,
    standard_rights_execute   = read_control,
    standard_rights_all       = standard_rights_required | synchronize,
    specific_rights_all       = 0x0000FFFFL,
    access_system_security    = 0x01000000L,
    maximum_allowed           = 0x02000000L,
    generic_read              = 0x80000000L,
    generic_write             = 0x40000000L,
    generic_execute           = 0x20000000L,
    generic_all               = 0x10000000L
  };

  friend access_mask operator|(access_mask m, access_mask m2)
  {
    return bitwise_or(m, m2);
  }

  friend access_mask operator&(access_mask m, access_mask m2)
  {
    return bitwise_and(m, m2);
  }

};//template<>struct device_traits<void>


struct access_mask_t
{
  uint16_t  specific_rights;
  uint8_t   standard_rights;
  uint8_t   access_aystem_acl : 1;
  uint8_t   reserved          : 3;
  uint8_t   generic_all       : 1;
  uint8_t   generic_execute   : 1;
  uint8_t   generic_write     : 1;
  uint8_t   generic_read      : 1;
  access_mask_t(
    bool      generic_read,
    bool      generic_write,
    bool      generic_execute,
    bool      generic_all,
    bool      access_aystem_acl,
    uint8_t   standard_rights,
    uint16_t  specific_rights = 0
    ) : generic_read(static_cast<uint8_t>(generic_read)),
        generic_write(static_cast<uint8_t>(generic_write)),
        generic_execute(static_cast<uint8_t>(generic_execute)),
        generic_all(static_cast<uint8_t>(generic_all)),
        access_aystem_acl(static_cast<uint8_t>(access_aystem_acl)),
        standard_rights(standard_rights), specific_rights(specific_rights)
        {}
  operator uint32_t() { return  *reinterpret_cast<uint32_t*>(this); }
};



#if 0
template<typename T = void>
struct generic_mapping
{
  typename device_traits<T>::access_mask generic_read;
  typename device_traits<T>::access_mask generic_write;
  typename device_traits<T>::access_mask generic_execute;
  typename device_traits<T>::access_mask generic_all;
};
#endif

}//namespace ntl

#endif//#ifndef NTL__DEVICE_TRAITS
