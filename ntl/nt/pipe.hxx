/**\file*********************************************************************
 *                                                                     \brief
 *  Pipes
 *
 ****************************************************************************
 */
#ifndef NTL__NT_PIPE
#define NTL__NT_PIPE
#pragma once

#include "basedef.hxx"
#include "file.hxx"
#include "../stlx/cstdio.hxx"

namespace ntl {
  namespace nt {

    enum pipe_type {
      pipe_type_stream,
      pipe_type_message
    };

    enum pipe_mode {
      pipe_mode_stream,
      pipe_mode_message
    };

    enum pipe_operation {
      pipe_operation_queue,
      pipe_operation_complete
    };

    NTL_EXTERNAPI
      ntstatus __stdcall
      NtCreateNamedPipeFile(
        handle* FileHandle,
        uint32_t DesiredAccess,
        const object_attributes* ObjectAttributes,
        io_status_block* IoStatusBlock,
        uint32_t ShareAccess,
        uint32_t CreateDisposition,
        uint32_t CreateOptions,
        uint32_t NamedPipeType,
        uint32_t ReadMode,
        uint32_t CompletionMode,
        uint32_t MaximumInstances,
        uint32_t InboundQuota,
        uint32_t OutboundQuota,
        const systime_t& DefaultTimeout
      );

    ntstatus __stdcall create_pipe(handle& ReadPipe, handle& WritePipe, size_t BufferSize = 4096)
    {
      static int pipe_serial_no = 0;

      char aname[64];
      std::snprintf(aname, _countof(aname), "\\Device\\NamedPipe\\Win32Pipes.%08x.%08x.NTL", teb::instance().ClientId.UniqueProcess, pipe_serial_no++);
      
      unicode_string pipe_name;
      nt::RtlAnsiStringToUnicodeString(pipe_name, const_ansi_string(aname), true);

      io_status_block iosb = {};
      object_attributes oa(pipe_name, object_attributes::inherit | object_attributes::case_insensitive);

      const std::chrono::seconds default_timeout(120);
      const systime_t timeout = -1i64*std::chrono::duration_cast<system_duration>(default_timeout).count();

      typedef ntl::device_traits<nt::file_handler> file_traits;

      ntstatus st = NtCreateNamedPipeFile(&ReadPipe, generic_read | synchronize | file_traits::write_attributes, 
        &oa, &iosb, 
        file_traits::share_read | file_traits::share_write, file_traits::create, file_traits::synchronous_io_nonalert,
        pipe_type_stream, pipe_mode_stream, pipe_operation_queue, 
        1, BufferSize, BufferSize, timeout);

      if(!success(st))
        return st;

      st = NtOpenFile(&WritePipe, generic_write | synchronize, &oa, &iosb, 
        file_traits::share_read | file_traits::share_write,
        file_traits::synchronous_io_nonalert | file_traits::non_directory_file);

      if(!success(st)) {
        ReadPipe.reset();
      }

      return st;
    }

  }// namespace nt
}// namespace ntl

#endif //NTL__NT_PIPE
