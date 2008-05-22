/**\file*********************************************************************
 *                                                                     \brief
 *  Native files support
 *
 ****************************************************************************
 */

#ifndef NTL__NT_FILE
#define NTL__NT_FILE

#include "../file.hxx"
#include "basedef.hxx"
#include "object.hxx"
#include "file_information.hxx"
#include "virtualmem.hxx"


namespace ntl {
namespace nt {


/**\addtogroup  io ********************* I/O API ****************************
 *@{*/

///\name  Legacy API


//#pragma warning(push)
//#pragma warning(disable:4190)// C-linkage specified, but returns UDT 'identifier2' which is incompatible with C

NTL__EXTERNAPI
ntstatus __stdcall
  NtCreateFile(
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
  NtOpenFile(
    handle *                  FileHandle,
    uint32_t                  DesiredAccess,
    const object_attributes * ObjectAttributes,
    io_status_block *         IoStatusBlock,
    uint32_t                  ShareAccess,
    uint32_t                  OpenOptions
    );


NTL__EXTERNAPI
ntstatus __stdcall
  NtReadFile(
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
  NtWriteFile(
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

NTL__EXTERNAPI
ntstatus __stdcall
  NtCreateSection(
    handle*           SectionHandle,
    uint32_t          DesiredAccess,
    const object_attributes* ObjectAttributes __optional,
    const int64_t*    MaximumSize   __optional,
    uint32_t          SectionPageProtection,
    uint32_t          AllocationAttributes,
    legacy_handle     FileHandle    __optional
    );

NTL__EXTERNAPI
ntstatus __stdcall
  NtOpenSection(
    handle*           SectionHandle,
    uint32_t          DesiredAccess,
    const object_attributes* ObjectAttributes
    );

NTL__EXTERNAPI
ntstatus __stdcall
  NtMapViewOfSection(
    legacy_handle   SectionHandle,
    legacy_handle   ProcessHandle,
    void**          BaseAddress,
    uintptr_t       ZeroBits,
    size_t          CommitSize,
    int64_t*        SectionOffset,
    size_t*         ViewSize,
    section_inherit InheritDisposition,
    uint32_t        AllocationType,
    uint32_t        Win32Protect
    );

NTL__EXTERNAPI
ntstatus __stdcall
  NtUnmapViewOfSection(
    legacy_handle   ProcessHandle,
    void*           BaseAddress
    );




struct rtl_relative_name
{
  unicode_string    RelativeName;
  union 
  {
    legacy_handle   ContainingDirectory;
    uint64_t        wow64Handle;
  };
  
  rtl_relative_name()
    :wow64Handle()
  {}
};

NTL__EXTERNAPI
bool __stdcall RtlDosPathNameToNtPathName_U(
    const wchar_t*  DosFileName,
    unicode_string* NtFileName,
    wchar_t**       FilePart,
    rtl_relative_name* RelativeName
    );



//#pragma warning(pop)

///@}

class file_handler;
class section;

/**@} io */

}//namespace nt



template<>
struct device_traits<nt::file_handler> : public device_traits<>
{
  bool success(ntstatus s) { return nt::success(s); }
  
  typedef int64_t size_type;

  enum access_mask
  {
      read_data             = 0x0001,
      list_directoy         = 0x0001,
      write_data            = 0x0002,
      add_file              = 0x0002,
      append_data           = 0x0004,
      add_subdirectory      = 0x0004,
      create_pipe_instance  = 0x0004,
      read_ea               = 0x0008,
      write_ea              = 0x0010,
      execute               = 0x0020,
      traverce              = 0x0020,
      detete_child          = 0x0040,
      read_attributes       = 0x0080,
      write_attributes      = 0x0100,
      all_access            = standard_rights_required | synchronize | 0x1FF,
      append                = append_data | synchronize,

      generic_read          = standard_rights_read | read_data | read_attributes | read_ea | synchronize,
      generic_write         = standard_rights_write | write_data | write_attributes | write_ea | append_data | synchronize,
      generic_execute       = standard_rights_execute | execute | read_attributes | synchronize,
      generic_all           = all_access
  };
  static const access_mask access_mask_default = 
                        access_mask(read_attributes | read_data | synchronize);

  friend access_mask operator | (access_mask m, access_mask m2) 
  { 
    return bitwise_or(m, m2);
  }

  friend access_mask operator | (access_mask m, nt::access_mask m2)
  { 
    return m | static_cast<access_mask>(m2);
  }

  friend access_mask operator | (nt::access_mask m, access_mask m2)
  { 
    return m2 | m;
  }

  enum creation_disposition
  {
    supersede,
    open,
    open_existing = open,
    create,
    create_new = create,
    open_if,
    overwrite,
    overwrite_if,
  };
  static const creation_disposition creation_disposition_default = open_existing;

  enum created_disposition
  {
    superseded,
    opened,
    created,
    overvritten,
    exists,
    does_not_exists
  };

  friend creation_disposition operator | (creation_disposition m, creation_disposition m2) 
  { 
    return bitwise_or(m, m2);
  }

  enum share_mode
  {
    share_none    = 0,
    share_read    = 1,
    share_write   = 2,
    share_delete  = 4,
    share_valid_flags = share_read | share_write | share_delete
  };
  static const share_mode share_mode_default = share_read;

  friend share_mode operator | (share_mode m, share_mode m2) 
  { 
    return bitwise_or(m, m2);
  }

  enum creation_options
  {
    directory_file            = 0x00000001,
    write_through             = 0x00000002,
    sequental_only            = 0x00000004,
    no_intermediate_buffering = 0x00000008,

    synchronous_io_alert      = 0x00000010,
    synchronous_io_nonalert   = 0x00000020,
    non_directory_file        = 0x00000040,
    create_tree_connection    = 0x00000080,

    complete_if_oplocked      = 0x00000100,
    no_ae_knowledge           = 0x00000200,
    open_remote_instance      = 0x00000400,    
    random_access             = 0x00000800,

    delete_on_close           = 0x00001000,
    open_by_file_id           = 0x00002000,
    open_for_backup_intent    = 0x00004000,
    no_compression            = 0x00008000,

    reserve_opfilter          = 0x00100000,
    open_reparse_point        = 0x00200000,
    open_no_recall            = 0x00400000,
    open_for_free_space_query = 0x00800000,
  };
  static const creation_options creation_options_default = 
                 creation_options(non_directory_file | synchronous_io_nonalert);

  friend creation_options operator | (creation_options m, creation_options m2) 
  { 
    return bitwise_or(m, m2);
  }

  enum attributes
  {
    readonly            = 0x00000001,
    hidden              = 0x00000002,
    system              = 0x00000004,
    archive             = 0x00000020,
    normal              = 0x00000080,
    temporary           = 0x00000100,
    offline             = 0x00001000,
    not_content_indexed = 0x00002000,
    encrypted           = 0x00004000,
    reparse_point       = 0x00200000,
  };
  static const attributes attribute_default = static_cast<attributes>(0);

  friend attributes operator | (attributes m, attributes m2) 
  { 
    return bitwise_or(m, m2);
  }

};

template<>
struct device_traits<nt::section>:
  public device_traits<>
{
  enum access_mask
  {
    query       = 0x0001,
    map_write   = 0x0002,
    map_read    = 0x0004,
    map_execute = 0x0008,
    extend_size = 0x0010,
    all_access  = standard_rights_required | query | map_write | map_read | map_execute | extend_size
  };

  static const access_mask access_mask_default = map_read;

  friend access_mask operator | (access_mask m, access_mask m2) 
  { 
    return bitwise_or(m, m2);
  }

  friend access_mask operator | (access_mask m, nt::access_mask m2)
  { 
    return m | static_cast<access_mask>(m2);
  }

  friend access_mask operator | (nt::access_mask m, access_mask m2)
  { 
    return m2 | m;
  }


};


namespace nt {

class file_handler : public handle, public device_traits<file_handler>
{
  ////////////////////////////////////////////////////////////////////////////
  public:

    __forceinline
    ntstatus
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
      return NtCreateFile(this, desired_access, &oa, &iosb,
                    allocation_size, attr, share, cd, co, ea_buffer, ea_length);
    }

    __forceinline
    ntstatus
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
      return NtCreateFile(this, desired_access, &oa, &iosb,
                    allocation_size, attr, share, cd, co, ea_buffer, ea_length);
    }

    ntstatus
      open(
        const object_attributes &   oa, 
        const access_mask           desired_access,
        const share_mode            share,
        const creation_options      co
        ) throw()
    {
      reset();
      return NtOpenFile(this, desired_access, &oa, &iosb, share, co);
    }

    operator const void*() { return get(); } 

    void close() { reset(); }

    ntstatus erase()
    {
      file_disposition_information<> del;
      file_information<file_disposition_information<> > file_info(get(), del);
      close();
      return file_info;
    }

    ntstatus
      read(
        void *            out_buf,
        const uint32_t    out_size,
        const uint64_t *  offset            = 0,
        legacy_handle     completion_event  = legacy_handle(),
        io_apc_routine *  apc_routine       = 0,
        const void *      apc_context       = 0,
        const uint32_t *  blocking_key      = 0
        ) const throw()
    {
      return NtReadFile(get(), completion_event, apc_routine, apc_context,
                          &iosb, out_buf, out_size, offset, blocking_key);
    }

    ntstatus
      write(
        const void *      in_buf,
        const uint32_t    in_size,
        const uint64_t *  offset            = 0,
        legacy_handle     completion_event  = legacy_handle(),
        io_apc_routine *  apc_routine       = 0,
        const void *      apc_context       = 0,
        const uint32_t *  blocking_key      = 0
        ) throw()
    {
      return NtWriteFile(get(), completion_event, apc_routine, apc_context,
                          &iosb, in_buf, in_size, offset, blocking_key);
    }

    size_type size() const
    {
      file_information<file_standard_information> file_info(get());
      return file_info ? file_info.data()->size() : 0;
    }

    ntstatus size(const size_type & new_size)
    {
      const file_end_of_file_information & fi = 
        *reinterpret_cast<const file_end_of_file_information*>(&new_size);
      file_information<file_end_of_file_information> file_info(get(), fi);
      return file_info;
    }

    size_type tell() const
    {
      file_information<file_position_information> file_info(get());
      return file_info ? file_info.data()->CurrentByteOffset : 0;
    }

    enum Origin { file_begin, file_current, file_end };
    ntstatus seek(const size_type& offset, Origin origin)
    {
      file_position_information fi = {offset};
      if(origin != file_begin){
        if(origin == file_current){
          file_information<file_position_information> file_info(get());
          if(!file_info)
            return file_info;
          fi.CurrentByteOffset += file_info.data()->CurrentByteOffset;
        }else if(origin == file_end){
          file_information<file_standard_information> file_info(get());
          if(!file_info)
            return file_info;
          fi.CurrentByteOffset += file_info.data()->EndOfFile;
        }
      }
      file_information<file_position_information> file_info(get(), fi);
      return file_info;
    }

    ntstatus rename(
      const const_unicode_string &  new_name,
      bool                          replace_if_exists)
    {
      std::auto_ptr<file_rename_information> fi = 
                    file_rename_information::alloc(new_name, replace_if_exists);
      if ( !fi.get() ) return status::insufficient_resources;
      file_information<file_rename_information> file_info(get(), *fi);
      return file_info;
    }

    const io_status_block & get_io_status_block() { return iosb; }
  
  ////////////////////////////////////////////////////////////////////////////
  private:

    mutable io_status_block iosb;

}; // class file_handler

typedef basic_file<file_handler> file;


class section:
  public handle,
  public device_traits<section>
{
public:
  // create
  explicit section(
    access_mask DesiredAccess,
    uint32_t    SectionPageProtection,
    uint32_t    AllocationAttributes = mem_commit
    ):base_()
  {
    create(this, DesiredAccess, NULL, NULL, SectionPageProtection, AllocationAttributes, NULL);
  }

  explicit section(
    access_mask   DesiredAccess,
    uint32_t      SectionPageProtection,
    int64_t&      MaximumSize,
    uint32_t      AllocationAttributes = mem_commit
    ):base_()
  {
    create(this, DesiredAccess, NULL, &MaximumSize, SectionPageProtection, AllocationAttributes, NULL);
  }

  explicit section(
    legacy_handle FileHandle,
    access_mask   DesiredAccess,
    uint32_t      SectionPageProtection,
    uint32_t      AllocationAttributes = mem_commit
    ):base_()
  {
    create(this, DesiredAccess, NULL, NULL, SectionPageProtection, AllocationAttributes, FileHandle);
  }

  explicit section(
    const const_unicode_string& SectionName,
    access_mask           DesiredAccess,
    uint32_t              SectionPageProtection,
    uint32_t              AllocationAttributes = mem_commit
    ):base_()
  {
    const object_attributes oa(SectionName);
    create(this, DesiredAccess, &oa, NULL, SectionPageProtection, AllocationAttributes, NULL);
  }

  explicit section(
    const const_unicode_string& SectionName,
    access_mask           DesiredAccess,
    uint32_t              SectionPageProtection,
    int64_t&              MaximumSize,
    uint32_t              AllocationAttributes = mem_commit
    ):base_()
  {
    const object_attributes oa(SectionName);
    create(this, DesiredAccess, &oa, &MaximumSize, SectionPageProtection, AllocationAttributes, NULL);
  }

  // open
  section(
    access_mask  DesiredAccess
    ):base_()
  {
    open(this, DesiredAccess, NULL);
  }

  section(
    const const_unicode_string& SectionName,
    access_mask           DesiredAccess
    ):base_()
  {
    const object_attributes oa(SectionName);
    open(this, DesiredAccess, &oa);
  }

  ~section()
  {
    munmap();
  }

  // map
  void* mmap(
    size_t    ViewSize,
    uint32_t  RegionProtection,
    uint32_t  AllocationType = 0
    )
  {
    NtMapViewOfSection(get(), current_process(), &base_, 0, 0, NULL, &ViewSize, ViewUnmap, AllocationType, RegionProtection);
    return base_;
  }

  void* mmap(
    int64_t&  SectionOffset,
    size_t    ViewSize,
    uint32_t  RegionProtection,
    uint32_t  AllocationType = 0
    )
  {
    NtMapViewOfSection(get(), current_process(), &base_, 0, 0, &SectionOffset, &ViewSize, ViewUnmap, AllocationType, RegionProtection);
    return base_;
  }

  bool munmap()
  {
    ntstatus st = NtUnmapViewOfSection(current_process(), base_);
    base_ = NULL;
    return success(st);
  }

  bool flush()
  {
    void* base = base_;
    io_status_block iosb;
    size_t size = 0;
    ntstatus st = NtFlushVirtualMemory(current_process(), &base, &size, &iosb);
    return success(st);
  }

  bool flush(size_t size)
  {
    void* base = base_;
    io_status_block iosb;
    ntstatus st = NtFlushVirtualMemory(current_process(), &base, &size, &iosb);
    return success(st);
  }


  // static helpers
  __forceinline
    static ntstatus 
      create(
        handle*             SectionHandle,
        access_mask         DesiredAccess,
        const object_attributes*  ObjectAttributes,
        int64_t*            MaximumSize,
        uint32_t            SectionPageProtection,
        uint32_t            AllocationAttributes,
        legacy_handle       FileHandle
        )
    {
      return NtCreateSection(SectionHandle, DesiredAccess, ObjectAttributes, MaximumSize, SectionPageProtection, AllocationAttributes, FileHandle);
    }

  __forceinline
    static ntstatus
      open(
        handle*             SectionHandle,
        access_mask         DesiredAccess,
        const object_attributes*  ObjectAttributes
        )
  {
    return NtOpenSection(SectionHandle, DesiredAccess, ObjectAttributes);
  }

private:
  void* base_;
};

}//namespace nt
}//namespace ntl

#endif//#ifndef NTL__NT_FILE
