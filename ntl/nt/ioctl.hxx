/**\file*********************************************************************
 *                                                                     \brief
 *  IO control codes
 *
 ****************************************************************************
 */

#ifndef NTL__NT_IOCTL
#define NTL__NT_IOCTL

#include "basedef.hxx"

namespace ntl {
namespace nt {

struct file_device
{
  enum type
  {
    beep                = 0x001,
    cd_rom              = 0x002,
    cd_rom_file_system  = 0x003,
    controller          = 0x004,
    datalink            = 0x005,
    dfs                 = 0x006,
    disk                = 0x007,
    disk_file_system    = 0x008,
    file_system         = 0x009,
    inport_port         = 0x00a,
    keyboard            = 0x00b,
    mailslot            = 0x00c,
    midi_in             = 0x00d,
    midi_out            = 0x00e,
    mouse               = 0x00f,
    multi_unc_provider  = 0x010,
    named_pipe          = 0x011,
    network             = 0x012,
    network_browser     = 0x013,
    network_file_system = 0x014,
    null                = 0x015,
    parallel_port       = 0x016,
    physical_netcard    = 0x017,
    printer             = 0x018,
    scanner             = 0x019,
    serial_mouse_port   = 0x01a,
    serial_port         = 0x01b,
    screen              = 0x01c,
    sound               = 0x01d,
    streams             = 0x01e,
    tape                = 0x01f,
    tape_file_system    = 0x020,
    transport           = 0x021,
    unknown             = 0x022,
    video               = 0x023,
    virtual_disk        = 0x024,
    wave_in             = 0x025,
    wave_out            = 0x026,
    port_8042           = 0x027,
    network_redirector  = 0x028,
    battery             = 0x029,
    bus_extender        = 0x02a,
    modem               = 0x02b,
    vdm                 = 0x02c,
    mass_storage        = 0x02d,
    smb                 = 0x02e,
    ks                  = 0x02f,
    changer             = 0x030,
    smartcard           = 0x031,
    acpi                = 0x032,
    dvd                 = 0x033,
    fullscreen_video    = 0x034,
    dfs_file_system     = 0x035,
    dfs_volume          = 0x036,
    serenum             = 0x037,
    termsrv             = 0x038,
    ksec                = 0x039,
    fips                = 0x03a,
    infiniband          = 0x03b,

    _max_reserved       = 0x7FFF,
    user_defined        = 0x8000
  };
};//struct file_device

namespace ioctl {

struct method
{
  enum type
  {
    buffered              = 0,
    in_direct             = 1,
    out_direct            = 2,
    neither               = 3,
    direct_to_hardware    = in_direct,
    firect_from_hardware  = out_direct
  };
};


struct file_access
{
  enum type
  {
    any     = 0,
    special = any,
    read    = 1,
    write   = 2
  };
};

static const file_access::type file_any_access      = file_access::any;
static const file_access::type file_special_access  = file_access::special;
static const file_access::type file_read_access     = file_access::read;
static const file_access::type file_write_access    = file_access::write;

template <uint16_t Function>
struct function
{
  enum type
  {
    _max_reserved       = 0x7FF,
    user_defined        = 0x800
  };

  STATIC_ASSERT(Function <= _max_reserved);
  static const uint16_t standard  = Function;
  static const uint16_t custom    = Function + user_defined;
};


enum ctl_code {};


template <file_device::type DeviceType,
          uint16_t          Function,
          method::type      Method,
          file_access::type Access>
struct ioctl
{
  static_assert(DeviceType <= file_device::_max_reserved, __name__" invalid DeviceType");
  static_assert(Function <= function<function<0>::_max_reserved>::custom, __name__" invalid Function");
  static_assert((Method & ~3) == 0, __name__" invalid Method");
  static_assert((Access & ~3) == 0, __name__" invalid Access");

  static const method::type       method    = Method;
  static const uint16_t           function  = Function;
  static const file_access::type  access    = Access;
  static const file_device::type  device_type = DeviceType;

  static const ctl_code code = static_cast<ctl_code>((device_type << 16)
                                  | (access << 14) | (function << 2) | method);

};//struct ioctl

} // ioctl

typedef 
ntstatus __stdcall
  control_file_t(
    legacy_handle FileHandle,
    legacy_handle Event,
    io_apc_routine* ApcRoutine,
    void* ApcContext,
    io_status_block* IoStatusBlock,
    uint32_t IoControlCode,
    void* InputBuffer,
    uint32_t InputBufferLength,
    void* OutputBuffer,
    uint32_t OutputBufferLength
    );

NTL__EXTERNAPI control_file_t NtDeviceIoControlFile, NtFsControlFile;


}//namespace nt
}//namespace ntl

#endif//#ifndef NTL__NT_IOCTL
