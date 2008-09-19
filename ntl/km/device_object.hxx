/**\file*********************************************************************
 *                                                                     \brief
 *  Device Object
 *
 ****************************************************************************
 */

#ifndef NTL__KM_DEVICE_OBJECT
#define NTL__KM_DEVICE_OBJECT

#include "object.hxx"
#include "irp.hxx"
#include "event.hxx"

namespace ntl {
namespace km {

struct devobj_extension
{
  uint16_t        Type;
  uint16_t        Size;
  device_object * DeviceObject;
  uint32_t        PowerFlags;
  struct device_object_power_extension* Dope;
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
  enum type {
    beep = 1,
    cd_rom,
    cd_rom_file_system,
    controller,
    datalink,
    dfs,
    disk,
    disk_file_system,
    file_system,
    inport_port,
    keyboard,
    mailslot,
    midi_in,
    midi_out,
    mouse,
    multi_unc_provider,
    named_pipe,
    network,
    network_browser,
    network_file_system,
    null,
    parallel_port,
    physical_netcard,
    printer,
    scanner,
    serial_mouse_port,
    serial_port,
    screen,
    sound,
    streams,
    tape,
    tape_file_system,
    transport,
    unknown,
    video,
    virtual_disk,
    wave_in,
    wave_out,
    _8042_port,
    network_redirector,
    battery,
    bus_extender,
    modem,
    vdm,
    mass_storage
  };
  STATIC_ASSERT(mass_storage == 0x2D);

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
  type                  DeviceType;
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

  typedef ntstatus __stdcall dispatch_t(device_object *, irp *);
  typedef dispatch_t *  dispatch_ptr;

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
    device_object * pdo = nullptr;
    file_object *   pfo = nullptr;
    IoGetDeviceObjectPointer(&object_name, desired_access, &pfo, &pdo);
    ObfDereferenceObject(pfo);
    return pdo;
  }

  ntstatus call(irp * pirp);

}; // struct device_object

typedef device_object::type device_type;


}//namspace km
}//namespace ntl


#endif//#ifndef NTL__KM_DEVICE_OBJECT
