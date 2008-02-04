/**\file*********************************************************************
 *                                                                     \brief
 *  Kernel Objects
 *
 ****************************************************************************
 */

#ifndef NTL__KM_OBJECT
#define NTL__KM_OBJECT

#include "basedef.hxx"
#include "string.hxx"
#include "handle.hxx"
#include "../nt/object.hxx"


namespace ntl {
namespace km {

using nt::security_descriptor;
using nt::security_quality_of_service;

using nt::object_information_class;
using nt::object_basic_information;
using nt::object_name_information;
using nt::object_type_information;
using nt::object_types_information;
using nt::object_handle_flag_information;
using nt::object_session_information;

struct device_object;
struct file_object;


struct object_attributes:
  public nt::object_attributes
{
  object_attributes(
    const const_unicode_string &  name,
    const attributes              attr      = case_insensitive | kernel_handle,
    const security_descriptor *   security  = 0)
    : nt::object_attributes(name, attr, security)
  {/**/}

  object_attributes(
    const legacy_handle           root,
    const const_unicode_string &  name,
    const attributes              attr      = case_insensitive | kernel_handle,
    const security_descriptor *   security  = 0)
    : nt::object_attributes(root, name, attr, security)
  {/**/}

private:

  //object_attributes(const object_attributes &);
  const object_attributes & operator=(const object_attributes &);

};


NTL__EXTERNAPI
ntstatus __stdcall
  IoGetDeviceObjectPointer(
    const const_unicode_string *  ObjectName,
    const access_mask             DesiredAccess,
    file_object * *               FileObject,
    device_object * *             DeviceObject
    );

static __forceinline
ntstatus
  get_device_object_pointer(
    const const_unicode_string &  name,
    const access_mask             desired_access,
    file_object * &               pfo,
    device_object * &             pdo)
{
  return IoGetDeviceObjectPointer(&name, desired_access, &pfo, &pdo);
}


struct object_type;
#if 0
{
  // non-static data --------------------------------
  /*<thisrel this+0x0>*/ /*|0x38|*/ struct _ERESOURCE Mutex;
  /*<thisrel this+0x38>*/ /*|0x8|*/ struct _LIST_ENTRY TypeList;
  /*<thisrel this+0x40>*/ /*|0x8|*/ struct _UNICODE_STRING Name;
  /*<thisrel this+0x48>*/ /*|0x4|*/ void* DefaultObject;
  /*<thisrel this+0x4c>*/ /*|0x4|*/ unsigned long Index;
  /*<thisrel this+0x50>*/ /*|0x4|*/ unsigned long TotalNumberOfObjects;
  /*<thisrel this+0x54>*/ /*|0x4|*/ unsigned long TotalNumberOfHandles;
  /*<thisrel this+0x58>*/ /*|0x4|*/ unsigned long HighWaterNumberOfObjects;
  /*<thisrel this+0x5c>*/ /*|0x4|*/ unsigned long HighWaterNumberOfHandles;
  /*<thisrel this+0x60>*/ /*|0x4c|*/ struct _OBJECT_TYPE_INITIALIZER TypeInfo;
  /*<thisrel this+0xac>*/ /*|0x4|*/ unsigned long Key;
  /*<thisrel this+0xb0>*/ /*|0xe0|*/ struct _ERESOURCE ObjectLocks[4];
};
STATIC_ASSERT(sizeof(object_type) ==0x190);
#endif

struct object_handle_information
{
	uint32_t HandleAttributes;
	uint32_t GrantedAccess;
};


NTL__EXTERNAPI
ntstatus __stdcall
  ObReferenceObjectByHandle (
    legacy_handle               Handle,
    access_mask                 DesiredAccess,
    const object_type *         ObjectType      __optional,
    kprocessor_mode             AccessMode,
    void *                      Object,
    object_handle_information * HandleInformation __optional
    );

template<typename ObjectType>
static __forceinline
ObjectType *
  reference_object(
    const handle &      hndl,
    access_mask         desired_access  = generic_read,
    const object_type * obj_type        = 0,
    kprocessor_mode     access_mode     = KernelMode
    )
{
  ObjectType * ptr;
  return nt::success(ObReferenceObjectByHandle(hndl.get(), desired_access,
                                      obj_type, access_mode, &ptr, 0))
          ? ptr
          : 0;
}

template<typename ObjectType>
static __forceinline
ObjectType *
  reference_object(
    const nt::handle &  hndl,
    access_mask         desired_access  = generic_read,
    const object_type * obj_type        = 0,
    kprocessor_mode     access_mode     = KernelMode
    )
{
  return reference_object<ObjectType>(*reinterpret_cast<const km::handle*>(&hndl),
                                        desired_access, obj_type, access_mode);
}


NTL__EXTERNAPI
void __fastcall
  ObfDereferenceObject(void * Object);


static __forceinline
void dereference_object(device_object * pdo)
{
  ObfDereferenceObject(pdo);
}

static __forceinline
void dereference_object(file_object * pfo)
{
  ObfDereferenceObject(pfo);
}


/// Volume Parameter Block
struct vpb
{
  static const size_t maximum_volume_label_length = 32 * sizeof(wchar_t);
  uint16_t        Type;
  uint16_t        Size;
  uint16_t        Flags;
  uint16_t        VolumeLabelLength; // in bytes
  device_object * DeviceObject;
  device_object * RealDevice;
  uint32_t        SerialNumber;
  uint32_t        ReferenceCount;
  wchar_t         VolumeLabel[maximum_volume_label_length / sizeof(wchar_t)];
};


struct io_timer
{
  uint16_t        Type;
  uint16_t        TimerFlag;
  list_entry      TimerList;
  void(__stdcall* TimerRoutine)(device_object*, void*);
  void *          Context;
  device_object * DeviceObject;
};


enum io_allocation_action 
{
  KeepObject                    = 0x1,
  DeallocateObject              = 0x2,
  DeallocateObjectKeepRegisters = 0x3,
};


struct kdevice_queue_entry
{
  list_entry  DeviceListEntry;
  uint32_t    SortKey;
  bool        Inserted;
};


struct kdevice_queue
{
  uint16_t    Type;
  uint16_t    Size;
  list_entry  DeviceListHead;
  kspin_lock  Lock;
  bool        Busy; // differs on AMD64
};


struct irp;

struct wait_context_block
{
  kdevice_queue_entry     WaitQueueEntry;
  io_allocation_action (* DeviceRoutine)(device_object*, irp*, void*, void*);
  void *                  DeviceContext;
  uint32_t                NumberOfMapRegisters;
  void *                  DeviceObject;
  void *                  CurrentIrp;
  kdpc *                  BufferChainingDpc;
};


}//namspace km
}//namespace ntl


#endif//#ifndef NTL__KM_OBJECT
