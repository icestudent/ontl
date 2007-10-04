/**\file*********************************************************************
*                                                                     \brief
*  Fast I/O procedure prototypes.
*
****************************************************************************
*/
#pragma once

#include "basedef.hxx"
#include "file_information.hxx"

namespace ntl {
	namespace km {

struct eprocess;
struct eresource;

struct compressed_data_info {
	/*<thisrel this+0x0>*/ /*|0x2|*/ uint16_t CompressionFormatAndEngine;
	/*<thisrel this+0x2>*/ /*|0x1|*/ uint8_t CompressionUnitShift;
	/*<thisrel this+0x3>*/ /*|0x1|*/ uint8_t ChunkShift;
	/*<thisrel this+0x4>*/ /*|0x1|*/ uint8_t ClusterShift;
	/*<thisrel this+0x5>*/ /*|0x1|*/ uint8_t Reserved;
	/*<thisrel this+0x6>*/ /*|0x2|*/ int16_t NumberOfChunks;
	/*<thisrel this+0x8>*/ /*|0x4|*/ int32_t CompressedChunkSizes[1];
};
// <size 0xc>

//
// Define fast I/O procedure prototypes.
//
// Fast I/O read and write procedures.
//

typedef
bool __stdcall
fast_io_check_if_possible_t (
						   __in file_object *FileObject,
						   __in int64_t* FileOffset,
						   __in uint32_t Length,
						   __in bool Wait,
						   __in uint32_t LockKey,
						   __in bool CheckForReadOperation,
						   __out io_status_block* IoStatus,
						   __in device_object *DeviceObject
						   );

typedef
bool __stdcall
fast_io_read_t (
			  __in file_object *FileObject,
			  __in int64_t* FileOffset,
			  __in uint32_t Length,
			  __in bool Wait,
			  __in uint32_t LockKey,
			  __out void* Buffer,
			  __out io_status_block* IoStatus,
			  __in device_object *DeviceObject
			  );

typedef
bool __stdcall
fast_io_write_t (
			   __in file_object *FileObject,
			   __in int64_t* FileOffset,
			   __in uint32_t Length,
			   __in bool Wait,
			   __in uint32_t LockKey,
			   __in void* Buffer,
			   __out io_status_block* IoStatus,
			   __in device_object *DeviceObject
			   );

//
// Fast I/O query basic and standard information procedures.
//

typedef
bool __stdcall
fast_io_query_basic_info_t (
						  __in file_object *FileObject,
						  __in bool Wait,
						  __out file_basic_information* Buffer,
						  __out io_status_block* IoStatus,
						  __in device_object *DeviceObject
						  );

typedef
bool __stdcall
fast_io_query_standard_info_t (
							 __in file_object *FileObject,
							 __in bool Wait,
							 __out file_standard_information* Buffer,
							 __out io_status_block* IoStatus,
							 __in device_object *DeviceObject
							 );

//
// Fast I/O lock and unlock procedures.
//

typedef
bool __stdcall
fast_io_lock_t (
			  __in file_object *FileObject,
			  __in int64_t* FileOffset,
			  __in int64_t* Length,
			  __in eprocess* ProcessId,
			  __in uint32_t Key,
			  __in bool FailImmediately,
			  __in bool ExclusiveLock,
			  __out io_status_block* IoStatus,
			  __in device_object *DeviceObject
			  );

typedef
bool __stdcall
fast_io_unlock_single_t (
					   __in file_object *FileObject,
					   __in int64_t* FileOffset,
					   __in int64_t* Length,
					   __in eprocess* ProcessId,
					   __in uint32_t Key,
					   __out io_status_block* IoStatus,
					   __in device_object *DeviceObject
					   );

typedef
bool __stdcall
fast_io_unlock_all_t (
					__in file_object *FileObject,
					__in eprocess* ProcessId,
					__out io_status_block* IoStatus,
					__in device_object *DeviceObject
					);

typedef
bool __stdcall
fast_io_unlock_all_by_key_t (
						   __in file_object *FileObject,
						   __in void* ProcessId,
						   __in uint32_t Key,
						   __out io_status_block* IoStatus,
						   __in device_object *DeviceObject
						   );

//
// Fast I/O device control procedure.
//

typedef
bool __stdcall
fast_io_device_control_t (
						__in file_object *FileObject,
						__in bool Wait,
						__in_opt void* InputBuffer,
						__in uint32_t InputBufferLength,
						__out_opt void* OutputBuffer,
						__in uint32_t OutputBufferLength,
						__in uint32_t IoControlCode,
						__out io_status_block* IoStatus,
						__in device_object *DeviceObject
						);

//
// Define callbacks for NtCreateSection to synchronize correctly with
// the file system.  It pre-acquires the resources that will be needed
// when calling to query and set file/allocation size in the file system.
//

typedef
void __stdcall
fast_io_acquire_file_t (
					  __in file_object *FileObject
					  );

typedef
void __stdcall
fast_io_release_file_t (
					  __in file_object *FileObject
					  );

//
// Define callback for drivers that have device objects attached to lower-
// level drivers' device objects.  This callback is made when the lower-level
// driver is deleting its device object.
//

typedef
void __stdcall
fast_io_detach_device_t (
					   __in device_object *SourceDevice,
					   __in device_object *TargetDevice
					   );

//
// This structure is used by the server to quickly get the information needed
// to service a server open call.  It is takes what would be two fast io calls
// one for basic information and the other for standard information and makes
// it into one call.
//

typedef
bool __stdcall
fast_io_query_network_open_info_t (
								 __in file_object *FileObject,
								 __in bool Wait,
								 __out file_network_open_information *Buffer,
								 __out io_status_block *IoStatus,
								 __in device_object *DeviceObject
								 );

//
//  Define Mdl-based routines for the server to call
//

typedef
bool __stdcall
fast_io_mdl_read_t (
				  __in file_object *FileObject,
				  __in int64_t* FileOffset,
				  __in uint32_t Length,
				  __in uint32_t LockKey,
				  __out mdl* *MdlChain,
				  __out io_status_block* IoStatus,
				  __in device_object *DeviceObject
				  );

typedef
bool __stdcall
fast_io_mdl_read_complete_t (
						   __in file_object *FileObject,
						   __in mdl* MdlChain,
						   __in device_object *DeviceObject
						   );

typedef
bool __stdcall
fast_io_prepare_mdl_write_t (
						   __in file_object *FileObject,
						   __in int64_t* FileOffset,
						   __in uint32_t Length,
						   __in uint32_t LockKey,
						   __out mdl* *MdlChain,
						   __out io_status_block* IoStatus,
						   __in device_object *DeviceObject
						   );

typedef
bool __stdcall
fast_io_mdl_write_complete_t (
							__in file_object *FileObject,
							__in int64_t* FileOffset,
							__in mdl* MdlChain,
							__in device_object *DeviceObject
							);

//
//  If this routine is present, it will be called by FsRtl
//  to acquire the file for the mapped page writer.
//

typedef
ntstatus __stdcall
fast_io_acquire_for_mod_write_t (
							   __in file_object *FileObject,
							   __in int64_t* EndingOffset,
							   __out eresource **ResourceToRelease,
							   __in device_object *DeviceObject
							   );

typedef
ntstatus __stdcall
fast_io_release_for_mod_write_t (
							   __in file_object *FileObject,
							   __in eresource *ResourceToRelease,
							   __in device_object *DeviceObject
							   );

//
//  If this routine is present, it will be called by FsRtl
//  to acquire the file for the mapped page writer.
//

typedef
ntstatus __stdcall
fast_io_acquire_for_ccflush_t (
							 __in file_object *FileObject,
							 __in device_object *DeviceObject
							 );

typedef
ntstatus __stdcall
fast_io_release_for_ccflush_t (
							 __in file_object *FileObject,
							 __in device_object *DeviceObject
							 );

typedef
bool __stdcall
fast_io_read_compressed_t (
						 __in file_object *FileObject,
						 __in int64_t* FileOffset,
						 __in uint32_t Length,
						 __in uint32_t LockKey,
						 __out void* Buffer,
						 __out mdl* *MdlChain,
						 __out io_status_block* IoStatus,
						 __out compressed_data_info *CompressedDataInfo,
						 __in uint32_t CompressedDataInfoLength,
						 __in device_object *DeviceObject
						 );

typedef
bool __stdcall
fast_io_write_compressed_t (
						  __in file_object *FileObject,
						  __in int64_t* FileOffset,
						  __in uint32_t Length,
						  __in uint32_t LockKey,
						  __in void* Buffer,
						  __out mdl* *MdlChain,
						  __out io_status_block* IoStatus,
						  __in compressed_data_info *CompressedDataInfo,
						  __in uint32_t CompressedDataInfoLength,
						  __in device_object *DeviceObject
						  );

typedef
bool __stdcall
fast_io_mdl_read_complete_compressed_t (
									  __in file_object *FileObject,
									  __in mdl* MdlChain,
									  __in device_object *DeviceObject
									  );

typedef
bool __stdcall
fast_io_mdl_write_complete_compressed_t (
									   __in file_object *FileObject,
									   __in int64_t* FileOffset,
									   __in mdl* MdlChain,
									   __in device_object *DeviceObject
									   );

typedef
bool __stdcall
fast_io_query_open_t (
					__in irp *Irp,
					__out file_network_open_information NetworkInformation,
					__in device_object *DeviceObject
					);


//
// Define the structure to describe the Fast I/O dispatch routines.  Any
// additions made to this structure MUST be added monotonically to the end
// of the structure, and fields CANNOT be removed from the middle.
//

struct fast_io_dispatch_t {
	uint32_t SizeOfFastIoDispatch;
	fast_io_check_if_possible_t* FastIoCheckIfPossible;
	fast_io_read_t* FastIoRead;
	fast_io_write_t* FastIoWrite;
	fast_io_query_basic_info_t* FastIoQueryBasicInfo;
	fast_io_query_standard_info_t* FastIoQueryStandardInfo;
	fast_io_lock_t* FastIoLock;
	fast_io_unlock_single_t* FastIoUnlockSingle;
	fast_io_unlock_all_t* FastIoUnlockAll;
	fast_io_unlock_all_by_key_t* FastIoUnlockAllByKey;
	fast_io_device_control_t* FastIoDeviceControl;
	fast_io_acquire_file_t* AcquireFileForNtCreateSection;
	fast_io_release_file_t* ReleaseFileForNtCreateSection;
	fast_io_detach_device_t* FastIoDetachDevice;
	fast_io_query_network_open_info_t* FastIoQueryNetworkOpenInfo;
	fast_io_acquire_for_mod_write_t* AcquireForModWrite;
	fast_io_mdl_read_t* MdlRead;
	fast_io_mdl_read_complete_t* MdlReadComplete;
	fast_io_prepare_mdl_write_t* PrepareMdlWrite;
	fast_io_mdl_write_complete_t* MdlWriteComplete;
	fast_io_read_compressed_t* FastIoReadCompressed;
	fast_io_write_compressed_t* FastIoWriteCompressed;
	fast_io_mdl_read_complete_compressed_t* MdlReadCompleteCompressed;
	fast_io_mdl_write_complete_compressed_t* MdlWriteCompleteCompressed;
	fast_io_query_open_t* FastIoQueryOpen;
	fast_io_release_for_mod_write_t* ReleaseForModWrite;
	fast_io_acquire_for_ccflush_t* AcquireForCcFlush;
	fast_io_release_for_ccflush_t* ReleaseForCcFlush;
};

STATIC_ASSERT(sizeof(fast_io_dispatch_t) == 0x70 || sizeof(fast_io_dispatch_t) == 0xE0);

	}//namespace km
}//namespace ntl
