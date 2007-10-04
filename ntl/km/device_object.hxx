/**\file*********************************************************************
 *                                                                     \brief
 *  Device Object
 *
 ****************************************************************************
 */

#ifndef NTL__KM_DEVICE_OBJECT
#define NTL__KM_DEVICE_OBJECT

#include "object.hxx"
#include "driver_object.hxx"

namespace ntl {
namespace km {

struct driver_object;


enum device_type {};


struct devobj_extension
{
  uint16_t        Type;
  uint16_t        Size;
  device_object * DeviceObject;
  uint32_t        PowerFlags;
  struct _DEVICE_OBJECT_POWER_EXTENSION* Dope;
  uint32_t        ExtensionFlags;
  void *          DeviceNode;
  device_object * AttachedTo;
  int32_t         StartIoCount;
  int32_t         StartIoKey;
  uint32_t        StartIoFlags;
  vpb *           Vpb;
};


struct device_object
{
  uint16_t              Type;
  uint16_t              Size;
  int32_t               ReferenceCount;
  driver_object *       DriverObject;
  device_object *       NextDevice;
  device_object *       AttachedDevice;
  struct irp    *       CurrentIrp;
  io_timer *            Timer;
  uint32_t              Flags;                                // See above:  DO_...
  uint32_t              Characteristics;                      // See ntioapi:  FILE_...
  vpb *                 Vpb;
  void *                DeviceExtension;
  device_type           DeviceType;
  uint8_t               StackSize;
  union { list_entry ListEntry; wait_context_block  Wcb; } Queue;
  uint32_t              AlignmentRequirement;
  kdevice_queue         DeviceQueue;
  kdpc                  Dpc;
  uint32_t              ActiveThreadCount;
  security_descriptor * SecurityDescriptor;
  kevent                DeviceLock;
  uint16_t              SectorSize;
  uint16_t              Spare1;
  devobj_extension *    DeviceObjectExtension;
  void *                Reserved;

  device_object * get_attached_device()
  {
    device_object * pdo = this;
    while ( pdo->AttachedDevice ) pdo = pdo->AttachedDevice;
    return pdo;
  }

  device_object * get_root_device()
  {
    device_object * pdo = this;
    while ( pdo->DeviceObjectExtension && pdo->DeviceObjectExtension->AttachedTo )
      pdo = pdo->DeviceObjectExtension->AttachedTo;
    return pdo;
  }

  static __forceinline
  device_object *
    get_by_name(
      const const_unicode_string & object_name,
      const access_mask            desired_access)
  {
    device_object * pdo = std::nullptr;
    file_object *   pfo = std::nullptr;
    IoGetDeviceObjectPointer(&object_name, desired_access, &pfo, &pdo);
    ObfDereferenceObject(pfo);
    return pdo;
  }

  ntstatus call(irp * pirp)
  {
//    pirp->CurrentLocation--;
//    io_stack_location * const stack = pirp->get_next_stack_location();
//    pirp->Tail.Overlay.CurrentStackLocation = stack;
    pirp->set_next_stack_location();
#if 0//defined(_DEBUG)
    if( pirp->CurrentLocation <= 0 )
        KeBugCheckEx(NO_MORE_IRP_STACK_LOCATIONS, pirp, 0, 0, 0);
#endif
    io_stack_location * const stack = pirp->get_current_stack_location();
    stack->DeviceObject = this;
    return DriverObject->MajorFunction[stack->MajorFunction](this, pirp);
  }
  
}; // struct device_object


}//namspace km
}//namespace ntl


#endif//#ifndef NTL__KM_DEVICE_OBJECT
