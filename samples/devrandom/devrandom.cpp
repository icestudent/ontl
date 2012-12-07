//  NTL samples library
//  devrandom driver sample.
//  it creates two devices("\\.\random" and "\\.\urandom") which are similar to unix /dev/[u]random, but insecure.
//
//  compile:
//      cl /Ox devrandom.cpp /Fedevrandom.sys /link /driver
//
//  install:
//      sc create devrandom binPath=%cd%\devrandom.sys type=kernel start=demand error=ignore
//
//  manual start:
//      sc start devrandom
//
//  uninstall:
//      1. sc delete devrandom
//
#include <km/basedef.hxx>
#include <km/file.hxx>
#include <km/time.hxx>
#include <km/driver_object.hxx>
#include <km/event.hxx>

#include <km/debug.hxx>

#include <cstdio>
#ifdef _DEBUG
  #define TRACE_STATUS(st, msg) ntl::km::dbg::trace.printf("["DRVNAME"]"__FUNCTION__ ": " msg " returned 0x%X\n", st);
  #define CHECK_STATUS(st, msg) TRACE_STATUS(st, msg); if(!success(st)) return st;
  #define CHECK_STATUSV(st, msg) TRACE_STATUS(st, msg); if(!success(st)) return;
#else
  #define TRACE_STATUS(st, msg)
  #define CHECK_STATUS(st, msg) if(!success(st)) return st;
  #define CHECK_STATUSV(st, msg)if(!success(st)) return;
#endif  

#ifndef DRVNAME
  #define DRVNAME "devrandom"
#endif

#pragma comment(lib, "ntoskrnl.lib")
#pragma comment(lib, "hal.lib")
#pragma comment(linker, "/entry:DrvEntry /subsystem:native /nodefaultlib:libcmt.lib")
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
using namespace ntl;
using namespace ntl::km;

typedef ntl::atomic_exec<fast_mutex, void> fast_synchronize;

device_object *g_devRandom = 0, *g_devUrandom;
volatile bool g_canShutdown = true;

ntstatus __stdcall GenerateRandom(device_object* DeviceObject, irp* Irp);
ntstatus __stdcall GenerateRandomLocked(device_object* DeviceObject, irp* Irp);

//////////////////////////////////////////////////////////////////////////
void GetTime(ntl::km::time_fields* tm)
{
  int64_t st, lt;
  KeQuerySystemTime(&st);
  ExSystemTimeToLocalTime(&st, &lt);
  RtlTimeToTimeFields(&lt, tm);
}

uint32_t efilter(exception::pointers* einfo, const char* function)
{
  dbg::error.printf("*** "DRVNAME" %s: Unhandled exception caught!\n\tcode: 0x%p, flags: 0x%X, at address: 0x%p\n", 
    function, einfo->ExceptionRecord->ExceptionCode, einfo->ExceptionRecord->ExceptionFlags,
    einfo->ExceptionRecord->ExceptionAddress);

  if(einfo->ExceptionRecord->ExceptionCode == status::access_violation){
    dbg::error.printf("\tAttempt to %s address 0x%p.\n", einfo->ExceptionRecord->ExceptionInformation[0] ? "write to" : "read from",
      einfo->ExceptionRecord->ExceptionInformation[1]);
  }
  
  return exception_execute_handler;
}

//////////////////////////////////////////////////////////////////////////
void CleanupDriver()
{
  ntstatus st;
  if(g_devRandom){
    st = IoDeleteSymbolicLink(const_unicode_string(L"\\??\\random"));
    TRACE_STATUS(st, "delete symlink r");
    reinterpret_cast<fast_mutex*>(g_devRandom->DeviceExtension)->~fast_mutex();
    st = IoDeleteDevice(g_devRandom);
    TRACE_STATUS(st, "delete device r");
    if(success(st))
      g_devRandom = nullptr;
  }

  if(g_devUrandom){
    st = IoDeleteSymbolicLink(const_unicode_string(L"\\??\\urandom"));
    TRACE_STATUS(st, "delete symlink u");
    st = IoDeleteDevice(g_devUrandom);
    TRACE_STATUS(st, "delete device u");
    if(success(st))
      g_devUrandom = nullptr;
  }
}

void __stdcall DrvUnload(driver_object*)
{
  CleanupDriver();
  dbg::trace(DRVNAME " driver unloaded\n");
}

ntstatus __stdcall DispatchShutdown(device_object* /*DeviceObject*/, irp* Irp)
{
  return Irp->complete_request(g_canShutdown ? status::success : status::unsuccessful);
}

ntstatus __stdcall DispatchCreateClose(device_object* /*DeviceObject*/, irp* Irp)
{
  Irp->IoStatus.Information = 0;
  return Irp->complete_request(status::success);
}

ntstatus __stdcall DispatchRead(device_object* DeviceObject, irp* Irp)
{
  // check params
  if(!(DeviceObject->Flags & device_object::flags::do_buffered_io)){
    dbg::error(DRVNAME " nonbuffered io\n");
    return Irp->complete_request(status::unsuccessful);
  }

  return (DeviceObject == g_devRandom ? GenerateRandomLocked : GenerateRandom)(DeviceObject, Irp);
}

ntstatus __stdcall DrvEntry(driver_object* DriverObject, unicode_string* /*RegistryPath*/)
{
  const const_unicode_string devname1(L"\\Device\\RandomDev"), devname2(L"\\Device\\RandomUDev");
  
  // register random device (blocking)
  ntstatus st = IoCreateDevice(DriverObject, sizeof(fast_mutex), devname1, device_object::unknown, 0, false, &g_devRandom);
  TRACE_STATUS(st, "create device r");
  if(!success(st))
    return st;

  // create link
  st = IoCreateSymbolicLink(const_unicode_string(L"\\??\\random"), devname1);
  TRACE_STATUS(st, "create symlink r");
  if(!success(st)){
    CleanupDriver();
    return st;
  }

  // register urandom device (non-blocking)
  st = IoCreateDevice(DriverObject, 0, devname2, device_object::unknown, 0, false, &g_devUrandom);
  TRACE_STATUS(st, "create device u");
  if(!success(st)){
    CleanupDriver();
    return st;
  }

  // create link
  st = IoCreateSymbolicLink(const_unicode_string(L"\\??\\urandom"), devname2);
  TRACE_STATUS(st, "create symlink u");
  if(!success(st)){
    CleanupDriver();
    return st;
  }

  // we prefer buffered io
  g_devRandom->Flags |= device_object::flags::do_buffered_io;
  g_devUrandom->Flags |= device_object::flags::do_buffered_io;

  // create lock
  new (g_devRandom->DeviceExtension) fast_mutex();

  // irp features
  DriverObject->MajorFunction[irp::mj_create] = 
    DriverObject->MajorFunction[irp::mj_close] = DispatchCreateClose;
  DriverObject->MajorFunction[irp::mj_read] = DispatchRead;

  DriverObject->DriverUnload = DrvUnload;
  dbg::info(DRVNAME " loaded.\n");
  return status::success;
}

//////////////////////////////////////////////////////////////////////////

ntstatus RandomData(void* Buffer, uint32_t Length)
{
  int64_t seed;
  KeQueryTickCount(&seed);
  __try{
    uint32_t* data = reinterpret_cast<uint32_t*>(Buffer);
    for(uint32_t i = 0; i < Length / 4; i++)
      data[i] = RtlRandom((uint32_t*)&seed);
    if(Length % 4){
      uint32_t r = RtlRandom((uint32_t*)&seed);
      std::memcpy(&data[Length/4], &r, Length % 4);
    }
  }__except(efilter(exception_info(), __FUNCTION__)){
    return status::data_error;
  }
  return status::success;
}

ntstatus __stdcall GenerateRandom(device_object* /*DeviceObject*/, irp* Irp)
{
  io_stack_location* io = Irp->get_current_stack_location();
  if(io->Parameters.Read.Length == 0){
    dbg::error(DRVNAME " empty buffer\n");
    return Irp->complete_request(status::buffer_too_small);
  }
  ntstatus st = RandomData(Irp->AssociatedIrp.SystemBuffer, io->Parameters.Read.Length);
  if(success(st))
    Irp->IoStatus.Information = io->Parameters.Read.Length;
  return Irp->complete_request(st);
}

ntstatus __stdcall GenerateRandomLocked(device_object* DeviceObject, irp* Irp)
{
  io_stack_location* io = Irp->get_current_stack_location();
  if(io->Parameters.Read.Length == 0){
    dbg::error(DRVNAME " empty buffer\n");
    return Irp->complete_request(status::buffer_too_small);
  }

  ntstatus st;
  {
    fast_mutex* guard = reinterpret_cast<fast_mutex*>(DeviceObject->DeviceExtension);
    fast_synchronize lock(*guard);

    st = RandomData(Irp->AssociatedIrp.SystemBuffer, io->Parameters.Read.Length);
    if(success(st))
      Irp->IoStatus.Information = io->Parameters.Read.Length;
  }
  return Irp->complete_request(st);
}
//////////////////////////////////////////////////////////////////////////
