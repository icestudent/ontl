/**\file*********************************************************************
 *                                                                     \brief
 *  Nt File Information
 *
 ****************************************************************************
 */

#ifndef NTL__NT_FILE_INFORMATION
#define NTL__NT_FILE_INFORMATION

#include "basedef.hxx"
#include "handle.hxx"
#include "../memory"


namespace ntl {
namespace nt {

/**\addtogroup  file_information ********* File Information ***************
 *@{*/

  struct file_attribute
  {
    enum type {
      readonly             = 0x00000001,
      hidden               = 0x00000002,
      system               = 0x00000004,
      directory            = 0x00000010,
      archive              = 0x00000020,
      device               = 0x00000040,
      normal               = 0x00000080,
      temporary            = 0x00000100,
      sparse_file          = 0x00000200,
      reparse_point        = 0x00000400,
      compressed           = 0x00000800,
      offline              = 0x00001000,
      not_content_indexed  = 0x00002000,
      encrypted            = 0x00004000,
      /** virtual */
      virtual_file         = 0x00010000,
    };

    __ntl_bitmask_type(type, friend)
  };

enum file_information_class
{
  FileDirectoryInformation    = 1,
  FileFullDirectoryInformation,
  FileBothDirectoryInformation,
  FileBasicInformation,
  FileStandardInformation,
  FileInternalInformation,
  FileEaInformation,
  FileAccessInformation,
  FileNameInformation,
  FileRenameInformation,
  FileLinkInformation,
  FileNamesInformation,
  FileDispositionInformation,
  FilePositionInformation,
  FileFullEaInformation,
  FileModeInformation,
  FileAlignmentInformation,
  FileAllInformation,
  FileAllocationInformation,
  FileEndOfFileInformation,
  FileAlternateNameInformation,
  FileStreamInformation,
  FilePipeInformation,
  FilePipeLocalInformation,
  FilePipeRemoteInformation,
  FileMailslotQueryInformation,
  FileMailslotSetInformation,
  FileCompressionInformation,
  FileObjectIdInformation,
  FileCompletionInformation,
  FileMoveClusterInformation,
  FileQuotaInformation,
  FileReparsePointInformation,
  FileNetworkOpenInformation,
  FileAttributeTagInformation,
  FileTrackingInformation,
  FileIdBothDirectoryInformation,
  FileIdFullDirectoryInformation,
  FileValidDataLengthInformation,
  FileShortNameInformation,
  FileIoCompletionNotificationInformation,
  FileIoStatusBlockRangeInformation,
  FileIoPriorityHintInformation,
  FileSfioReserveInformation,
  FileSfioVolumeInformation,
  FileHardLinkInformation,
  FileProcessIdsUsingFileInformation,
  FileNormalizedNameInformation,
  FileNetworkPhysicalNameInformation,
  FileMaximumInformation
}; // enum file_information_class


typedef
ntstatus __stdcall
  query_information_file_t(
    legacy_handle           FileHandle,
    io_status_block *       IoStatusBlock,
    void *                  FileInformation,
    unsigned long           Length,
    uint32_t                FileInformationClass
    );

NTL__EXTERNAPI query_information_file_t NtQueryInformationFile;

typedef
ntstatus __stdcall
  set_information_file_t(
    legacy_handle           FileHandle,
    io_status_block *       IoStatusBlock,
    const void *            FileInformation,
    unsigned long           Length,
    file_information_class  FileInformationClass
    );

NTL__EXTERNAPI set_information_file_t NtSetInformationFile;


template <class                     InformationClass,
          query_information_file_t  QueryInformation,
          set_information_file_t    SetInformation>
struct file_information_base
{
    typedef InformationClass info_class;

    file_information_base(legacy_handle file_handle) __ntl_nothrow
    : status_(_query(file_handle, &info, sizeof(info)))
    {/**/}

    file_information_base(
      legacy_handle       file_handle,
      const info_class &  info
      ) __ntl_nothrow
    : status_(_set(file_handle, &info, sizeof(info)))
    {/**/}

    info_class * data() { return nt::success(status_) ? &info : 0; }
    const info_class * data() const { return nt::success(status_) ? &info : 0; }
    info_class * operator->() { return data(); }
    const info_class * operator->() const { return data(); }

    operator bool() const { return nt::success(status_); }
    operator ntstatus() const { return status_; }

    static __forceinline
    ntstatus
      _query(
        legacy_handle   file_handle,
        void *          info,
        unsigned long   info_length
        )
    {
      io_status_block iosb;
      return QueryInformation(file_handle, &iosb, info, info_length,
                              info_class::info_class_type);
    }

    static __forceinline
    ntstatus
      _set(
        legacy_handle   file_handle,
        const void *    info,
        unsigned long   info_length
        )
    {
      io_status_block iosb;
      return SetInformation(file_handle, &iosb, info, info_length,
                            info_class::info_class_type);
    }

  ///////////////////////////////////////////////////////////////////////////
  private:

    ntstatus    status_;
    info_class  info;

};//class file_information_base


template<class InformationClass>
struct file_information
: public file_information_base <InformationClass,
                                NtQueryInformationFile,
                                NtSetInformationFile>
{
  explicit file_information(legacy_handle file_handle) __ntl_nothrow
  : file_information_base<InformationClass, NtQueryInformationFile, NtSetInformationFile>(file_handle)
  {/**/}

  file_information(
    legacy_handle             file_handle,
    const InformationClass &  info) __ntl_nothrow
  : file_information_base<InformationClass,
                          NtQueryInformationFile,
                          NtSetInformationFile>(file_handle, info)
  {/**/}
};

///\name FileDirectoryInformation == 1
struct file_directory_information
{
  static const file_information_class info_class_type = FileDirectoryInformation;

  uint32_t  NextEntryOffset;
  uint32_t  FileIndex;
  int64_t   CreationTime;
  int64_t   LastAccessTime;
  int64_t   LastWriteTime;
  int64_t   ChangeTime;
  int64_t   EndOfFile;
  int64_t   AllocationSize;
  uint32_t  FileAttributes;
  uint32_t  FileNameLength;
  wchar_t   FileName[1];
};

///\name   FileBasicInformation == 4
struct file_basic_information
{
  static const file_information_class info_class_type = FileBasicInformation;

  int64_t CreationTime;
  int64_t LastAccessTime;
  int64_t LastWriteTime;
  int64_t ChangeTime;
  uint32_t FileAttributes;
};
STATIC_ASSERT(sizeof(file_basic_information) == 0x28);


///\name  FileStandardInformation == 5
struct file_standard_information
{
  static const file_information_class info_class_type = FileStandardInformation;

  int64_t   AllocationSize;
  int64_t   EndOfFile;
  uint32_t  NumberOfLinks;
  bool      DeletePending;
  bool      Directory;

  int64_t   size() const { return EndOfFile; }
};

///\name  FileInternalInformation == 6
struct file_internal_information
{
  static const file_information_class info_class_type = FileInternalInformation;

  int64_t   IndexNumber;
};

///\name  FileEaInformation == 7
struct file_ea_information
{
  static const file_information_class info_class_type = FileEaInformation;

  uint32_t  EaSize;
};

///\name  FileAccessInformation == 8
struct file_access_information
{
  static const file_information_class info_class_type = FileAccessInformation;

  access_mask AccessFlags;
};

///\name  FileRenameInformation == 10
struct file_rename_information
{
  static const file_information_class info_class_type = FileRenameInformation;

  typedef std::unique_ptr<file_rename_information> ptr;

  static inline ptr alloc(const const_unicode_string& new_name, bool replace_if_exists, legacy_handle root_directory = legacy_handle())
  {
    return ptr(new (varsize, new_name.size()*sizeof(wchar_t)) file_rename_information(new_name, replace_if_exists, root_directory));
  }

  bool          ReplaceIfExists;
  legacy_handle RootDirectory;
  uint32_t      FileNameLength;
  wchar_t       FileName[1];

protected:
  file_rename_information(
    const const_unicode_string &  new_name,
    bool                          replace_if_exists,
    legacy_handle                 root_directory
  )
    : ReplaceIfExists(replace_if_exists),
    RootDirectory(root_directory),
    FileNameLength(new_name.size()*sizeof(wchar_t))
  {
    std::copy(new_name.begin(), new_name.end(), FileName);
  }
};

namespace __
{
  template<class info_class>
  struct file_rename_information_impl
  {
    operator bool() const { return nt::success(status_); }
    operator ntstatus() const { return status_; }

    file_rename_information_impl(legacy_handle file_handle, const const_unicode_string& new_name, bool replace_if_exists, legacy_handle root_directory)
    {
      info_class::ptr p(static_cast<info_class*>(info_class::alloc(new_name, replace_if_exists, root_directory).release()));
      status_ = p 
        ? _set(file_handle, p.get(), sizeof(info_class)+p->FileNameLength-sizeof(wchar_t))
        : status::insufficient_resources;
    }

    static __forceinline
      ntstatus
      _set(
      legacy_handle   file_handle,
      const void *    info,
      unsigned long   info_length
      )
    {
      io_status_block iosb;
      return NtSetInformationFile(file_handle, &iosb, info, info_length, info_class::info_class_type);
    }

  protected:
    ntstatus    status_;
  };
}

template<>
struct file_information<file_rename_information>:
  __::file_rename_information_impl<file_rename_information>
{
  file_information(legacy_handle file_handle, const const_unicode_string& new_name, bool replace_if_exists, legacy_handle root_directory = legacy_handle())
    :file_rename_information_impl(file_handle, new_name, replace_if_exists, root_directory)
  {}
};

///\name  FileLinkInformation == 11
struct file_link_information: file_rename_information,
  private ntl::noncopyable
{
  static const file_information_class info_class_type = FileLinkInformation;

  typedef std::unique_ptr<file_link_information> ptr;
};

template<>
struct file_information<file_link_information>:
  __::file_rename_information_impl<file_link_information>
{
  file_information(legacy_handle file_handle, const const_unicode_string& new_name, bool replace_if_exists, legacy_handle root_directory = legacy_handle())
    :file_rename_information_impl(file_handle, new_name, replace_if_exists, root_directory)
  {}
};

///\name FileNamesInformation == 12
struct file_names_information
{
  static const file_information_class info_class_type = FileNamesInformation;

  uint32_t NextEntryOffset;
  uint32_t FileIndex;
  uint32_t FileNameLength;
  wchar_t  FileName[1];

  const_unicode_string name() const { return const_unicode_string(FileName, FileNameLength); }
};

template<>
struct file_information<file_names_information>
{
  typedef file_names_information info_class;

  file_information(legacy_handle file_handle) __ntl_nothrow
  {
    for(uint32_t size = sizeof(info_class)+256*sizeof(wchar_t); ptr.reset(new char[size]), ptr; size *= 2) {
      st = _query(file_handle, ptr.get(), size);
      if(st == status::success)
        break;
      else if(st != status::buffer_overflow){
        ptr.reset();
        break;
      }
    }
  }

  const info_class * data() const { return nt::success(st) ? reinterpret_cast<const info_class*>(ptr.get()) : 0; }
  const info_class * operator->() const { return data(); }

  operator bool() const { return nt::success(st); }

  operator ntstatus() const { return st; }

  static __forceinline
    ntstatus
    _query(
    legacy_handle   file_handle,
    void *          info,
    unsigned long   info_length
    )
  {
    io_status_block iosb;
    return NtQueryInformationFile(file_handle, &iosb, info, info_length, info_class::info_class_type);
  }

  struct iterator:
    std::iterator<std::forward_iterator_tag, const info_class, uint32_t>
  {
    friend bool operator== (const iterator& x, const iterator& y) { return x.p == y.p; }
    friend bool operator!= (const iterator& x, const iterator& y) { return x.p != y.p; }

    reference operator*()   const { return *p; }
    pointer   operator->()  const { return p;  }

    iterator& operator++()  
    { 
      p = p->NextEntryOffset ? reinterpret_cast<value_type*>(uintptr_t(p) + p->NextEntryOffset) : 0;
      return *this;
    }

    iterator operator++(int)
    {
      iterator tmp(*this);
      ++*this;
      return tmp;
    }

    iterator() :p() {}
  protected:
    friend struct file_information<info_class>;

    explicit iterator(const info_class* p)
      :p(p)
    {}
  private:
    const info_class* p;
  };
  typedef iterator const_iterator;

  const_iterator begin() const
  {
    return const_iterator(data());
  }
  const_iterator end() const
  {
    return const_iterator(nullptr);
  }
  const_iterator cbegin() const { return begin(); }
  const_iterator cend()   const { return end();   }

private:
  std::unique_ptr<char[]> ptr;
  ntstatus st;
};

///\name  FileDispositionInformation == 13
template<bool Del = true>
struct file_disposition_information
{
  static const file_information_class info_class_type = FileDispositionInformation;

  file_disposition_information() : Delete(Del) {/**/}
  bool  Delete;
};

///\name  FilePositionInformation == 14
struct file_position_information
{
  static const file_information_class info_class_type = FilePositionInformation;

  int64_t CurrentByteOffset;
};


///\name  FileEndOfFileInformation == 20
struct file_end_of_file_information
{
  static const file_information_class info_class_type = FileEndOfFileInformation;

  int64_t EndOfFile;
};


///\name FileNetworkOpenInformation == 34
struct file_network_open_information
{
	int64_t CreationTime;
	int64_t LastAccessTime;
	int64_t LastWriteTime;
	int64_t ChangeTime;
	int64_t AllocationSize;
	int64_t EndOfFile;
	uint32_t FileAttributes;
};


///}

/**@} file_information */

enum fs_information_class {
  FileFsVolumeInformation = 1,
  FileFsLabelInformation,
  FileFsSizeInformation,
  FileFsDeviceInformation,
  FileFsAttributeInformation,
  FileFsControlInformation,
  FileFsQuotaQueryInformation,
  FileFsQuotaSetInformation,
  FileFsMaximumInformation
};

typedef ntstatus __stdcall
  nt_volume_information_t(
    legacy_handle         FileHandle,
    io_status_block*      IoStatusBlock,
    void*                 FsInformation,
    uint32_t              Length,
    fs_information_class  FsInformationClass
    );

NTL__EXTERNAPI query_information_file_t NtQueryVolumeInformationFile;
NTL__EXTERNAPI set_information_file_t NtSetVolumeInformationFile;

template<class InformationClass>
struct volume_information:
  public file_information_base <InformationClass,
  NtQueryVolumeInformationFile,
  NtSetVolumeInformationFile>
{
  volume_information(legacy_handle file_handle) __ntl_nothrow
    : file_information_base<InformationClass, NtQueryVolumeInformationFile, NtSetVolumeInformationFile>(file_handle)
  {/**/}

  volume_information(
    legacy_handle             file_handle,
    const InformationClass &  info) __ntl_nothrow
    : file_information_base<InformationClass,
    NtQueryVolumeInformationFile,
    NtSetVolumeInformationFile>(file_handle, info)
  {/**/}
};

///\name   FileFsVolumeInformation == 1
struct file_fs_volume_information
{
  static const fs_information_class info_class_type = FileFsVolumeInformation;

  int64_t  VolumeCreationTime;
  uint32_t VolumeSerialNumber;
  uint32_t VolumeLabelLength;
  bool     SupportsObjects;
  wchar_t  VolumeLabel[1];

  const_unicode_string label() const { return const_unicode_string(VolumeLabel, VolumeLabelLength / sizeof(wchar_t)); }
};

template<>
struct volume_information<file_fs_volume_information>
{
  typedef file_fs_volume_information info_class;

  explicit volume_information(legacy_handle volume_handle, bool with_label = true)
  {
    // length of the label is 34 characters long max
    for(uint32_t length = sizeof(info_class)+sizeof(wchar_t)*34; ptr.reset(new char[length]), ptr; length*= 2)
    {
      status_ = query(volume_handle, ptr.get(), length);
      if(status_ == status::success){
        break;
      }else if(status_ != status::buffer_overflow){
        ptr.reset();
        break;
      }else if(!with_label && status_ == status::buffer_overflow){
        break;
      }
    }
  }

  const info_class* operator->() const { return data(); }
  info_class* operator->() { return data(); }

  info_class* data() { return reinterpret_cast<info_class*>(ptr.get()); }
  const info_class* data() const { return reinterpret_cast<const info_class*>(ptr.get()); }
  operator const void*() const { return ptr.get(); }

  operator bool() const { return nt::success(status_); }
  operator ntstatus() const { return status_; }

  static __forceinline
    ntstatus query(
    legacy_handle volume_handle,
    void*     volume_information,
    uint32_t  volume_information_length
    )
  {
    io_status_block iosb;
    return NtQueryVolumeInformationFile(volume_handle, &iosb,
      volume_information, volume_information_length, info_class::info_class_type);
  }
private:
  std::unique_ptr<char[]> ptr;
  ntstatus status_;
};

///\name   FileFsLabelInformation == 2
struct file_fs_label_information
{
  static const fs_information_class info_class_type = FileFsLabelInformation;

  uint32_t VolumeLabelLength;
  wchar_t VolumeLabel[1];
};

///\name   FileFsSizeInformation == 3
struct file_fs_size_information
{
  static const fs_information_class info_class_type = FileFsSizeInformation;

  int64_t TotalAllocationUnits;
  int64_t AvailableAllocationUnits;
  uint32_t SectorsPerAllocationUnit;
  uint32_t BytesPerSector;
};

///\name   FileFsDeviceInformation == 4
struct file_fs_device_information
{
  static const fs_information_class info_class_type = FileFsDeviceInformation;

  uint32_t DeviceType;
  uint32_t Characteristics;
};

///\name   FileFsAttributeInformation == 5
struct file_fs_attribute_information
{
  static const fs_information_class info_class_type = FileFsAttributeInformation;

  uint32_t FileSystemAttributes;
  int32_t  MaximumComponentNameLength;
  uint32_t FileSystemNameLength;
  wchar_t  FileSystemName[1];
};

///\name   FileFsControlInformation == 6
struct file_fs_control_information
{
  static const fs_information_class info_class_type = FileFsControlInformation;

  enum flags {
    vc_quota_none                  = 0x00000000,
    vc_quota_track                 = 0x00000001,
    vc_quota_enforce               = 0x00000002,
    vc_quota_mask                  = 0x00000003,
    vc_quotas_log_violations       = 0x00000004,
    vc_content_index_disabled      = 0x00000008,
    vc_log_quota_threshold         = 0x00000010,
    vc_log_quota_limit             = 0x00000020,
    vc_log_volume_threshold        = 0x00000040,
    vc_log_volume_limit            = 0x00000080,
    vc_quotas_incomplete           = 0x00000100,
    vc_quotas_rebuilding           = 0x00000200,
    vc_valid_mask                  = 0x000003ff
  };

  int64_t FreeSpaceStartFiltering;
  int64_t FreeSpaceThreshold;
  int64_t FreeSpaceStopFiltering;
  int64_t DefaultQuotaThreshold;
  int64_t DefaultQuotaLimit;
  flags   FileSystemControlFlags;
};

}//namespace nt
}//namespace ntl

#endif//#ifndef NTL__NT_FILE_INFORMATION
