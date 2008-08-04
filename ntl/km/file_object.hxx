/**\file*********************************************************************
 *                                                                     \brief
 *  File Object
 *
 ****************************************************************************
 */

#ifndef NTL__KM_FILE_OBJECT
#define NTL__KM_FILE_OBJECT

#include "device_object.hxx"

namespace ntl {
namespace km {


struct section_object_pointers
{
  void  * DataSectionObject;
  void  * SharedCacheMap;
  void  * ImageSectionObject;
};

/// Completion message
struct io_completion_context { void * Port, * Key; };


struct file_object
{
  uint16_t                  Type;
  uint16_t                  Size;
  device_object *           DeviceObject;
  vpb *                     Vpb;
  void *                    FsContext;
  void *                    FsContext2;
  section_object_pointers * SectionObjectPointer;
  void *                    PrivateCacheMap;
  ntstatus                  FinalStatus;
  file_object *             RelatedFileObject;
  bool                      LockOperation;
  bool                      DeletePending;
  bool                      ReadAccess;
  bool                      WriteAccess;
  bool                      DeleteAccess;
  bool                      SharedRead;
  bool                      SharedWrite;
  bool                      SharedDelete;
  uint32_t                  Flags;
  unicode_string            FileName;
  uint64_t                  CurrentByteOffset;
  uint32_t                  Waiters;
  uint32_t                  Busy;
  void *                    LastLock;
  kevent                    Lock;
  kevent                    Event;
  io_completion_context *   CompletionContext;

#ifdef _M_IX86
  kspin_lock                IrpListLock;
  list_entry                IrpList;
  void *                    FileObjectExtension;
#endif

  ///@note ignores Vpb
  device_object * get_related_device_object()
  {
    return DeviceObject->get_attached_device();
  }
  
} ; // struct file_object
//STATIC_ASSERT(sizeof(file_object) == 0x70 + sizeof(kspin_lock) + sizeof(list_entry) + sizeof(void *));

}//namspace km
}//namespace ntl


#endif//#ifndef NTL__KM_FILE_OBJECT
