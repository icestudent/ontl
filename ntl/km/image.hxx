/**\file*********************************************************************
 *                                                                     \brief
 *  Image notify
 *
 ****************************************************************************
 */
#ifndef NTL__KM_IMAGE
#define NTL__KM_IMAGE
#pragma once

#include "../pe/image.hxx"
#include "basedef.hxx"

namespace ntl {
namespace km {

struct image_info
{
  uint32_t    ImageAddressingMode  : 8;
  uint32_t    SystemModeImage      : 1;
  uint32_t    ImageMappedToAllPids : 1;
  uint32_t    Reserved             : 22;
  pe::image * ImageBase;
  uint32_t    ImageSelector;
  uint32_t    ImageSize;
  uint32_t    ImageSectionNumber;
};


typedef
void __stdcall
  load_image_notify_routine_t(
    unicode_string *  FullImageName,
    legacy_handle     ProcessId,
    image_info *      ImageInfo
    );


typedef
ntstatus __stdcall
  set_remove_load_image_notify_routine_t(
    load_image_notify_routine_t *  NotifyRoutine
    );


NTL_EXTERNAPI
set_remove_load_image_notify_routine_t  PsSetLoadImageNotifyRoutine;

NTL_EXTERNAPI
set_remove_load_image_notify_routine_t  PsRemoveLoadImageNotifyRoutine;


}//namespace km
}//namespace ntl

#endif//#ifndef NTL__KM_IMAGE
