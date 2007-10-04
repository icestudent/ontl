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


namespace ntl {
namespace nt {

/**\addtogroup  file_information ********* File Information ***************
 *@{*/


enum file_information_class
{
  FileDirectoryInformation                = 1,
  FileFullDirectoryInformation            = 2,
  FileBothDirectoryInformation            = 3,
  FileBasicInformation                    = 4,
  FileStandardInformation                 = 5,
  FileInternalInformation                 = 6,
  FileEaInformation                       = 7,
  FileAccessInformation                   = 8,
  FileNameInformation                     = 9,
  FileRenameInformation                   = 10,
  FileLinkInformation                     = 11,
  FileNamesInformation                    = 12,
  FileDispositionInformation              = 13,
  FilePositionInformation                 = 14,
  FileFullEaInformation                   = 15,
  FileModeInformation                     = 16,
  FileAlignmentInformation                = 17,
  FileAllInformation                      = 18,
  FileAllocationInformation               = 19,
  FileEndOfFileInformation                = 20,
  FileAlternateNameInformation            = 21,
  FileStreamInformation                   = 22,
  FilePipeInformation                     = 23,
  FilePipeLocalInformation                = 24,
  FilePipeRemoteInformation               = 25,
  FileMailslotQueryInformation            = 26,
  FileMailslotSetInformation              = 27,
  FileCompressionInformation              = 28,
  FileObjectIdInformation                 = 29,
  FileCompletionInformation               = 30,
  FileMoveClusterInformation              = 31,
  FileQuotaInformation                    = 32,
  FileReparsePointInformation             = 33,
  FileNetworkOpenInformation              = 34,
  FileAttributeTagInformation             = 35,
  FileTrackingInformation                 = 36,
  FileIdBothDirectoryInformation          = 37,
  FileIdFullDirectoryInformation          = 38,
  FileValidDataLengthInformation          = 39,
  FileShortNameInformation                = 40,
  FileIoCompletionNotificationInformation = 41,
  FileIoStatusBlockRangeInformation       = 42,
  FileIoPriorityHintInformation           = 43,
  FileSfioReserveInformation              = 44,
  FileSfioVolumeInformation               = 45,
  FileHardLinkInformation                 = 46,
  FileProcessIdsUsingFileInformation      = 47,
  FileNormalizedNameInformation           = 48,
  FileNetworkPhysicalNameInformation      = 49,
  FileMaximumInformation
}; // enum file_information_class


typedef
ntstatus __stdcall
  query_information_file_t(
    legacy_handle           FileHandle,
    io_status_block *       IoStatusBlock,
    void *                  FileInformation,
    unsigned long           Length,
    file_information_class  FileInformationClass
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

    file_information_base(legacy_handle file_handle) throw()
    : status_(_query(file_handle, &info, sizeof(info)))
    {/**/}

    file_information_base(
      legacy_handle       file_handle,
      const info_class &  info
      ) throw()
    : status_(_set(file_handle, &info, sizeof(info)))
    {/**/}

    info_class * data() { return nt::success(status_) ? &info : 0; }
    const info_class * data() const { return nt::success(status_) ? &info : 0; }

    operator bool() const { return nt::success(status_); }

    ntstatus status() const { return status_; }

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

};//class file_information


template<class InformationClass>
struct file_information
: public file_information_base <InformationClass,
                                NtQueryInformationFile,
                                NtSetInformationFile>
{
  file_information(legacy_handle file_handle) throw()
  : file_information_base<InformationClass, NtQueryInformationFile, NtSetInformationFile>(file_handle)
  {/**/}

  file_information(
    legacy_handle             file_handle,
    const InformationClass &  info) throw()
  : file_information_base<InformationClass,
                          NtQueryInformationFile,
                          NtSetInformationFile>(file_handle, info)
  {/**/}
};


///\name   FileBasicInformation == 4
struct file_basic_information
{
  static const file_information_class info_class_type = FileBasicInformation;

  int64_t CreationTime;
  int64_t LastAccessTime;
  int64_t LastWriteTime;
  int64_t ChangeTime;
  int32_t FileAttributes;
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


///\name  FileDispositionInformation == 13
template<bool Del = true>
struct file_disposition_information
{
  static const file_information_class info_class_type = FileDispositionInformation;

  file_disposition_information() : Delete(Del) {/**/}
  bool  Delete;
};


///\name  FileEndOfFileInformation == 20
struct file_end_of_file_information
{
  static const file_information_class info_class_type = FileEndOfFileInformation;

  int64_t EndOfFile;
};

///\name FileNetworkOpenInformation == 34
struct file_network_open_information {
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

}//namespace nt
}//namespace ntl

#endif//#ifndef NTL__NT_FILE_INFORMATION
