/**\file*********************************************************************
 *                                                                     \brief
 *  Driver Object
 *
 ****************************************************************************
 */

#ifndef NTL__KM_DRIVER_OBJECT
#define NTL__KM_DRIVER_OBJECT

#include "basedef.hxx"
#include "string.hxx"
#include "device_object.hxx"
#include "irp.hxx"

namespace ntl {
namespace km {

struct device_object;
struct driver_object;
struct fast_io_dispatch;

struct driver_extension
{
  driver_object *   DriverObject;
  long (__stdcall * AddDevice)(driver_object *, device_object *);
  uint32_t          Count;
  unicode_string    ServiceKeyName;
  struct _IO_CLIENT_EXTENSION* ClientDriverExtension;
  struct _FS_FILTER_CALLBACKS* FsFilterCallbacks;
};


struct driver_object
{
  uint16_t                    Type;
  uint16_t                    Size;
  device_object *             DeviceObject;
  uint32_t                    Flags;
  void *                      DriverStart;
  uint32_t                    DriverSize;
  nt::ldr_data_table_entry *  DriverSection;
  driver_extension *          DriverExtension;
  unicode_string              DriverName;
  unicode_string *            HardwareDatabase;
  fast_io_dispatch *          FastIoDispatch;
  ntstatus(__stdcall *        DriverInit)(driver_object*, unicode_string*);
  void    (__stdcall *        DriverStartIo)(driver_object*, irp*);
  void    (__stdcall *        DriverUnload)(driver_object*);

  device_object::dispatch_ptr MajorFunction[28];

};

STATIC_ASSERT(sizeof(driver_object) == 0xA8 || sizeof(driver_object) == 0x150);

__forceinline
ntstatus device_object::call(irp * pirp)
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


}//namspace km
}//namespace ntl


#endif//#ifndef NTL__KM_DRIVER_OBJECT
