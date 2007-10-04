/**\file*********************************************************************
 *                                                                     \brief
 *  Kernel Mode files support
 *
 ****************************************************************************
 */

#ifndef NTL__KM_FILE
#define NTL__KM_FILE

#include "../nt/file.hxx"
#include "basedef.hxx"
#include "object.hxx"
#include "file_information.hxx"

namespace ntl {
namespace km {

/**\addtogroup  io ********************* I/O API ****************************
 *@{*/

///\name  Legacy API


#pragma warning(push)
//#pragma warning(disable:4190)// C-linkage specified, but returns UDT 'identifier2' which is incompatible with C

NTL__EXTERNAPI
ntstatus __stdcall
  ZwCreateFile(
    handle *                  FileHandle,
    uint32_t                  DesiredAccess,
    const object_attributes * ObjectAttributes,
    io_status_block *         IoStatusBlock,
    const uint64_t *          AllocationSize    __optional,
    uint32_t                  FileAttributes,
    uint32_t                  ShareAccess,
    uint32_t                  CreateDisposition,
    uint32_t                  CreateOptions,
    const void *              EaBuffer          __optional,
    uint32_t                  EaLength
    );

NTL__EXTERNAPI
ntstatus __stdcall
  ZwOpenFile(
    handle *                  FileHandle,
    uint32_t                  DesiredAccess,
    const object_attributes * ObjectAttributes,
    io_status_block *         IoStatusBlock,
    uint32_t                  ShareAccess,
    uint32_t                  OpenOptions
    );


NTL__EXTERNAPI
ntstatus __stdcall
  ZwReadFile(
    legacy_handle     FileHandle,
    legacy_handle     Event         __optional,
    io_apc_routine *  ApcRoutine    __optional,
    const void *      ApcContext    __optional,
    io_status_block * IoStatusBlock,
    void *            Buffer,
    uint32_t          Length,
    const uint64_t *  ByteOffset    __optional,
    const uint32_t *  Key           __optional
    );

NTL__EXTERNAPI
ntstatus __stdcall
  ZwWriteFile(
    legacy_handle     FileHandle,
    legacy_handle     Event         __optional,
    io_apc_routine *  ApcRoutine    __optional,
    const void *      ApcContext    __optional,
    io_status_block * IoStatusBlock,
    const void *      Buffer,
    uint32_t          Length,
    const uint64_t *  ByteOffset    __optional,
    const uint32_t *  Key           __optional
    );

#pragma warning(pop)

///@}

class file_handler;

/**@} io */

}//namespace km


template<>
struct device_traits<km::file_handler>
: public device_traits<nt::file_handler> {/**/};


namespace km {

class file_handler : public handle, public device_traits<file_handler>
{
  ////////////////////////////////////////////////////////////////////////////
  public:

    __forceinline
    bool
      create(
        const object_attributes &   oa, 
        const creation_disposition  cd              = creation_disposition_default,
        const access_mask           desired_access  = access_mask_default,
        const share_mode            share           = share_mode_default, 
        const creation_options      co              = creation_options_default,
        const attributes            attr            = attribute_default,
        const uint64_t *            allocation_size = 0,
        const void *                ea_buffer       = 0,
        uint32_t                    ea_length       = 0
        ) throw()
    {
      reset();
      const ntstatus s = ZwCreateFile(this, desired_access, &oa, &iosb,
                    allocation_size, attr, share, cd, co, ea_buffer, ea_length);
      return nt::success(s);
    }

    __forceinline
    bool
      create(
        const std::wstring &        file_name, 
        const creation_disposition  cd              = creation_disposition_default,
        const access_mask           desired_access  = access_mask_default,
        const share_mode            share           = share_mode_default, 
        const creation_options      co              = creation_options_default,
        const attributes            attr            = attribute_default,
        const uint64_t *            allocation_size = 0,
        const void *                ea_buffer       = 0,
        uint32_t                    ea_length       = 0
        ) throw()
    {
      reset();
      const const_unicode_string uname(file_name);
      const object_attributes oa(uname);
      const ntstatus s = ZwCreateFile(this, desired_access, &oa, &iosb,
                    allocation_size, attr, share, cd, co, ea_buffer, ea_length);
      return nt::success(s);
    }

    __forceinline
    bool
      open(
        const object_attributes &   oa, 
        const access_mask           desired_access,
        const share_mode            share,
        const creation_options      co
        ) throw()
    {
      reset();
      const ntstatus s = ZwOpenFile(this, desired_access, &oa, &iosb, share, co);
      return nt::success(s);
    }

    operator const void*() { return get(); } 

    void close() { reset(); }

    bool erase()
    {
      file_disposition_information<> del;
      file_information<file_disposition_information<> > file_info(get(), del);
      return file_info;
    }

    bool
      read(
        void *            out_buf,
        const uint32_t    out_size,
        const uint64_t *  offset            = 0,
        legacy_handle     completion_event  = legacy_handle(),
        io_apc_routine *  apc_routine       = 0,
        const void *      apc_context       = 0,
        const uint32_t *  key               = 0
        ) throw()
    {
      const ntstatus s = ZwReadFile(get(), completion_event, apc_routine, apc_context,
                          &iosb, out_buf, out_size, offset, key);
      return nt::success(s);
    }


    bool
      write(
        const void *      in_buf,
        const uint32_t    in_size,
        const uint64_t *  offset            = 0,
        legacy_handle     completion_event  = legacy_handle(),
        io_apc_routine *  apc_routine       = 0,
        const void *      apc_context       = 0,
        const uint32_t *  key               = 0
        ) throw()
    {
      const ntstatus s = ZwWriteFile(get(), completion_event, apc_routine, apc_context,
                          &iosb, in_buf, in_size, offset, key);
      return nt::success(s);
    }


    int64_t 
      size() const
    {
      file_information<file_standard_information> file_info(get());
      return file_info ? file_info.data()->size() : 0;
    }
  
  ////////////////////////////////////////////////////////////////////////////
  private:

    io_status_block iosb;

}; // class file_handler

typedef basic_file<file_handler> file;

}//namespace km
}//namespace ntl

#endif//#ifndef NTL__KM_FILE
