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
ntstatus __stdcall NtFlushBuffersFile(legacy_handle FileHandle, io_status_block* IoStatusBlock);

NTL__EXTERNAPI
ntstatus __stdcall NtQueryAttributesFile(const object_attributes& ObjectAttributes, file_basic_information& FileInformation);

NTL__EXTERNAPI
ntstatus __stdcall NtQueryFullAttributesFile(const object_attributes& ObjectAttributes, file_network_open_information& FileInformation);

NTL__EXTERNAPI
ntstatus __stdcall ZwQueryFullAttributesFile(const object_attributes& ObjectAttributes, file_network_open_information& FileInformation);

NTL__EXTERNAPI
ntstatus __stdcall NtDeleteFile(const object_attributes& ObjectAttributes);

//////////////////////////////////////////////////////////////////////////
NTL__EXTERNAPI
uint32_t __stdcall RtlGetCurrentDirectory_U(uint32_t nBufferLength, wchar_t* lpBuffer);

NTL__EXTERNAPI
ntstatus __stdcall RtlSetCurrentDirectory_U(const const_unicode_string& PathName);

//////////////////////////////////////////////////////////////////////////
NTL__EXTERNAPI
ntstatus __stdcall
  NtQueryDirectoryFile(
    legacy_handle  FileHandle,
    legacy_handle  Event,
    io_apc_routine*  ApcRoutine,
    void* ApcContext,
    io_status_block* IoStatusBlock,
    void* FileInformation,
    uint32_t Length,
    file_information_class FileInformationClass,
    bool ReturnSingleEntry,
    const const_unicode_string* FileName,
    bool RestartScan
    );

NTL__EXTERNAPI
ntstatus __stdcall
  NtCreateSection(
    handle*                     SectionHandle,
    uint32_t                    DesiredAccess,
    const object_attributes*    ObjectAttributes __optional,
    const int64_t*              MaximumSize   __optional,
    page_protection::type       SectionPageProtection,
    allocation_attributes::type AllocationAttributes,
    legacy_handle               FileHandle    __optional
    );

NTL__EXTERNAPI
ntstatus __stdcall
  NtOpenSection(
    handle*                   SectionHandle,
    uint32_t                  DesiredAccess,
    const object_attributes*  ObjectAttributes
    );

NTL__EXTERNAPI
ntstatus __stdcall
  NtMapViewOfSection(
    legacy_handle               SectionHandle,
    legacy_handle               ProcessHandle,
    void**                      BaseAddress,
    uintptr_t                   ZeroBits,
    size_t                      CommitSize,
    int64_t*                    SectionOffset,
    size_t*                     ViewSize,
    section_inherit::type       InheritDisposition,
    allocation_attributes::type AllocationType,
    page_protection::type       Win32Protect
    );

NTL__EXTERNAPI
ntstatus __stdcall
  NtUnmapViewOfSection(
    legacy_handle   ProcessHandle,
    void*           BaseAddress
    );


//////////////////////////////////////////////////////////////////////////
struct rtl_curdir_ref
{
  volatile uint32_t RefCount;
  legacy_handle     Handle;
};

struct rtl_relative_name
{
  unicode_string    RelativeName;
  union
  {
    legacy_handle   ContainingDirectory;
    uint64_t        wow64Handle;
  };

  // 2k3sp1+
  rtl_curdir_ref* CurDirRef;

  rtl_relative_name()
    :wow64Handle(), CurDirRef()
  {}
};

NTL__EXTERNAPI
bool __stdcall RtlDosPathNameToNtPathName_U(
    const wchar_t*  DosFileName,
    unicode_string* NtFileName,
    wchar_t**       FilePart,
    rtl_relative_name* RelativeName // reserved at 2k3sp1
    );

// 2k3sp1+
NTL__EXTERNAPI
ntstatus __stdcall RtlDosPathNameToNtPathName_U_WithStatus(
    const wchar_t*  DosFileName,
    unicode_string* NtFileName,
    wchar_t**       FilePart,
    rtl_relative_name* // reserved
    );

NTL__EXTERNAPI
bool __stdcall RtlDosPathNameToRelativeNtPathName_U(
    const wchar_t*  DosFileName,
    unicode_string* NtFileName,
    wchar_t**       FilePart,
    rtl_relative_name* RelativeName
    );

NTL__EXTERNAPI
ntstatus __stdcall RtlDosPathNameToRelativeNtPathName_U_WithStatus(
    const wchar_t*  DosFileName,
    unicode_string* NtFileName,
    wchar_t**       FilePart,
    rtl_relative_name* RelativeName
    );

NTL__EXTERNAPI
void __stdcall RtlReleaseRelativeName(rtl_relative_name*);

namespace rtl
{
  struct path_type
  {
    enum type {
      Unknown,
      UncAbsolute,
      DriveAbsolute,
      DriveRelative,
      Rooted,
      Relative,
      LocalDevice,
      RootLocalDevice
    };
  };

  struct relative_name: rtl_relative_name
  {
    unicode_string  path;
    wchar_t*        file;


    relative_name(const wchar_t* dospath)
    {
      convert(dospath);
    }
    relative_name(const std::wstring& dospath)
    {
      convert(dospath.c_str());
    }
    ~relative_name()
    {
      #ifdef _M_X64
      RtlReleaseRelativeName(this);
      #else
      if(CurDirRef){
        if(!--CurDirRef->RefCount){
          close(CurDirRef->Handle);
          delete CurDirRef; // heap::free(process_heap(),0,CurDirRef)
        }
      }
      #endif

      if(!path.empty())
        RtlFreeUnicodeString(path);
    }

    operator const object_attributes() const
    {
      return object_attributes(/*ContainingDirectory, ContainingDirectory ? RelativeName : */path);
    }

    operator explicit_bool_type() const
    {
      return explicit_bool(ok_);
    }

  protected:
    bool ok_;

    void convert(const wchar_t* dospath)
    {
      ok_ = RtlDosPathNameToNtPathName_U(dospath, &path, &file, this);
    }
  };
}

typedef rtl::path_type::type rtl_path_type;

NTL__EXTERNAPI rtl_path_type __stdcall RtlDetermineDosPathNameType_U(const wchar_t* DosFileName);

//#pragma warning(pop)

///@}

class file_handler;
class section;

/**@} io */

}//namespace nt



template<>
struct device_traits<nt::file_handler>:
  public device_traits<>,
  public nt::file_attribute
{
  bool success(ntstatus s) { return nt::success(s); }

  typedef int64_t size_type;

  enum access_mask
  {
      read_data             = 0x0001,
      list_directory        = 0x0001,
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
    create_if  = open_if,
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

  typedef file_attribute::type attributes;
  static const attributes attribute_default = static_cast<attributes>(0);

  //friend attributes operator | (attributes m, attributes m2)
  //{
  //  return bitwise_or(m, m2);
  //}

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
        ) __ntl_nothrow
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
        ) __ntl_nothrow
    {
      reset();
      const const_unicode_string uname(file_name);
      const object_attributes oa(uname);
      return NtCreateFile(this, desired_access, &oa, &iosb,
                    allocation_size, attr, share, cd, co, ea_buffer, ea_length);
    }

    __forceinline
    ntstatus
      open(
        const object_attributes &   oa,
        const access_mask           desired_access  = access_mask_default,
        const share_mode            share           = share_mode_default,
        const creation_options      co              = creation_options_default
        ) __ntl_nothrow
    {
      reset();
      return NtOpenFile(this, desired_access, &oa, &iosb, share, co);
    }

    ntstatus
      open(
        const const_unicode_string& file_name,
        const access_mask           desired_access,
        const share_mode            share,
        const creation_options      co
        ) __ntl_nothrow
    {
      reset();
      const object_attributes oa(file_name);
      return NtOpenFile(this, desired_access, &oa, &iosb, share, co);
    }

    //operator explicit_bool_type() const { return explicit_bool(get()); } 

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
        ) const __ntl_nothrow
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
        ) __ntl_nothrow
    {
      return NtWriteFile(get(), completion_event, apc_routine, apc_context,
                          &iosb, in_buf, in_size, offset, blocking_key);
    }

    ntstatus flush()
    {
      return NtFlushBuffersFile(get(), &iosb);
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

    ntstatus rename(const const_unicode_string& new_name, bool replace_if_exists = true)
    {
      file_information<file_rename_information> file_info(get(), new_name, replace_if_exists);
      return file_info;
    }

    ntstatus link(const const_unicode_string& name, bool replace_if_exists = false)
    {
      file_information<file_link_information> file_info(get(), name, replace_if_exists);
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
  // create unnamed section
  explicit section(
    int64_t               MaximumSize,
    page_protection       SectionPageProtection = page_protection::page_readwrite,
    allocation_attributes AllocationAttributes = allocation_attributes::sec_commit,
    access_mask           DesiredAccess = all_access
    ):base_()
  {
    last_status_ = create(this, DesiredAccess, NULL, &MaximumSize, SectionPageProtection, AllocationAttributes, NULL);
  }

  explicit section(
    int64_t*              MaximumSize,
    page_protection       SectionPageProtection = page_protection::page_readwrite,
    allocation_attributes AllocationAttributes = allocation_attributes::sec_commit,
    access_mask           DesiredAccess = all_access
    ):base_()
  {
    last_status_ = create(this, DesiredAccess, NULL, MaximumSize, SectionPageProtection, AllocationAttributes, NULL);
  }

  explicit section(
    legacy_handle FileHandle,
    page_protection       SectionPageProtection = page_protection::page_readwrite,
    allocation_attributes AllocationAttributes = allocation_attributes::sec_commit,
    access_mask           DesiredAccess = all_access
    ):base_()
  {
    last_status_ = create(this, DesiredAccess, NULL, NULL, SectionPageProtection, AllocationAttributes, FileHandle);
  }

  // create named section
  explicit section(
    const const_unicode_string& SectionName,
    int64_t               MaximumSize,
    page_protection       SectionPageProtection = page_protection::page_readwrite,
    allocation_attributes AllocationAttributes = allocation_attributes::sec_commit,
    access_mask           DesiredAccess = all_access
    ):base_()
  {
    const object_attributes oa(SectionName);
    last_status_ = create(this, DesiredAccess, &oa, &MaximumSize, SectionPageProtection, AllocationAttributes, NULL);
  }

  explicit section(
    const const_unicode_string& SectionName,
    int64_t*              MaximumSize,
    page_protection       SectionPageProtection = page_protection::page_readwrite,
    allocation_attributes AllocationAttributes = allocation_attributes::sec_commit,
    access_mask           DesiredAccess = all_access
    ):base_()
  {
    const object_attributes oa(SectionName);
    last_status_ = create(this, DesiredAccess, &oa, MaximumSize, SectionPageProtection, AllocationAttributes, NULL);
  }

  // open
  explicit section(
    const const_unicode_string& SectionName,
    access_mask           DesiredAccess
    ):base_()
  {
    const object_attributes oa(SectionName);
    last_status_ = open(this, DesiredAccess, &oa);
  }

  explicit section(
    const object_attributes& ObjectAtributes,
    access_mask  DesiredAccess
    ):base_()
  {
    last_status_ = open(this, DesiredAccess, &ObjectAtributes);
  }

  ~section()
  {
    munmap();
  }

  // map
  void* mmap(
    size_t          ViewSize,
    page_protection RegionProtection = page_protection::page_readwrite
    )
  {
    last_status_ = NtMapViewOfSection(get(), current_process(), &base_, 0, 0, NULL, &ViewSize, section_inherit::ViewUnmap, allocation_attributes::none, RegionProtection);
    return base_;
  }

  void* mmap(
    int64_t&  SectionOffset,
    size_t    CommitSize,
    size_t    ViewSize,
    page_protection       RegionProtection = page_protection::page_readwrite,
    allocation_attributes AllocationType = allocation_attributes::none
    )
  {
    last_status_ = NtMapViewOfSection(get(), current_process(), &base_, 0, CommitSize, &SectionOffset, &ViewSize, section_inherit::ViewUnmap, AllocationType, RegionProtection);
    return base_;
  }

  bool munmap()
  {
    last_status_ = NtUnmapViewOfSection(current_process(), base_);
    base_ = NULL;
    return success(last_status_);
  }

  bool flush()
  {
    void* base = base_;
    io_status_block iosb;
    size_t size = 0;
    last_status_ = NtFlushVirtualMemory(current_process(), &base, &size, &iosb);
    return success(last_status_);
  }

  bool flush(size_t size)
  {
    void* base = base_;
    io_status_block iosb;
    last_status_ = NtFlushVirtualMemory(current_process(), &base, &size, &iosb);
    return success(last_status_);
  }


  // static helpers
  __forceinline
    static ntstatus
      create(
        handle*             SectionHandle,
        access_mask         DesiredAccess,
        const object_attributes*  ObjectAttributes,
        int64_t*            MaximumSize,
        page_protection     SectionPageProtection,
        allocation_attributes AllocationAttributes,
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

  ntstatus last_status() const { return last_status_; }

private:
  void* base_;
  ntstatus last_status_;
};

}//namespace nt
}//namespace ntl

#endif//#ifndef NTL__NT_FILE
