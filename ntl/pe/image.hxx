/**\file*********************************************************************
*                                                                     \brief
*  Portable Executable images support
*
****************************************************************************
*/

#ifndef NTL__PE_IMAGE
#define NTL__PE_IMAGE

#include "../stdlib.hxx"
#include "../basedef.hxx"
#include "../nt/basedef.hxx"
#include "../cstring"

namespace ntl {
  namespace nt {
#ifdef _M_X64
    NTL__EXTERNAPI
      void *__stdcall 
        RtlPcToFileHeader(const void *PcValue, void **pBaseOfImage);
#endif
  }
  namespace pe {

#pragma warning(push)
#pragma warning(disable:4820) // 'X' bytes padding added after data member
#pragma warning(disable:4347) // behavior change: va<>() is called instead of va()

    /**\addtogroup  pe_images_support *** Portable Executable images support*****
    *@{*/

    class image
    {
      ///////////////////////////////////////////////////////////////////////////
    public:

      static __forceinline image * this_module();

      static image * bind(uintptr_t image_base)
      {
        return reinterpret_cast<image*>(image_base);
      }

      static image * bind(void * image_base)
      {
        return reinterpret_cast<image*>(image_base);
      }

      static const image * bind(const void * image_base)
      {
        return reinterpret_cast<const image*>(image_base);
      }

#ifdef _M_X64
      static const image* base_from(const void* address)
      {
        void* base;
        return reinterpret_cast<const image*>(nt::RtlPcToFileHeader(address, &base));
      }
#endif

      template<typename T>
      static __forceinline
        bool in_range(uintptr_t first, uintptr_t last, T p)
      {
        return first <= brute_cast<uintptr_t>(p) && brute_cast<uintptr_t>(p) < last;
      }

      ///\ name   Headers

      struct dos_header
      {
        bool is_valid() const { return e_magic == signature; }

        static const uint16_t signature = 0x5A4D; //'MZ'
        uint16_t  e_magic;
        uint16_t  e_cblp;
        uint16_t  e_cp;
        uint16_t  e_crlc;
        uint16_t  e_cparhdr;
        uint16_t  e_minalloc;
        uint16_t  e_maxalloc;
        uint16_t  e_ss;
        uint16_t  e_sp;
        uint16_t  e_csum;
        uint16_t  e_ip;
        uint16_t  e_cs;
        uint16_t  e_lfarlc;
        uint16_t  e_ovno;
        uint16_t  e_res[4];
        uint16_t  e_oemid;
        uint16_t  e_oeminfo;
        uint16_t  e_res2[10];
        long      e_lfanew;
      }; // struct dos_header
      STATIC_ASSERT(sizeof(dos_header)==0x40);


      struct file_header
      {
        enum Machines {
          unknown,
          i386 = 0x014C,
          amd64= 0x8664
        };
        struct characteristics
        {
          enum values {
            relocs_stripped           = 0x0001,  // relocation info stripped from file.
            executable_image          = 0x0002,  // file is executable  (i.e. no unresolved externel references).
            line_nums_stripped        = 0x0004,  // line nunbers stripped from file.
            local_syms_stripped       = 0x0008,  // local symbols stripped from file.
            aggresive_ws_trim         = 0x0010,  // agressively trim working set
            large_address_aware       = 0x0020,  // app can handle >2gb addresses
            bytes_reversed_lo         = 0x0080,  // bytes of machine word are reversed.
            machine_32bit             = 0x0100,  // 32 bit word machine.
            debug_stripped            = 0x0200,  // debugging info stripped from file in .dbg file
            removable_run_from_swap   = 0x0400,  // if image is on removable media, copy and run from the swap file.
            net_run_from_swap         = 0x0800,  // if image is on net, copy and run from the swap file.
            system                    = 0x1000,  // system file.
            dll                       = 0x2000,  // file is a dll.
            up_system_only            = 0x4000,  // file should only be run on a up machine
            bytes_reversed_hi         = 0x8000  // bytes of machine word are reversed.
          };
        };
        uint16_t  Machine;
        uint16_t  NumberOfSections;
        uint32_t  TimeDateStamp;
        uint32_t  PointerToSymbolTable;
        uint32_t  NumberOfSymbols;
        uint16_t  SizeOfOptionalHeader;
        uint16_t  Characteristics;
      };


      struct data_directory
      {
        uint32_t  VirtualAddress;
        uint32_t  Size;

        enum entry
        {
          export_table,
          import_table,
          resource_table,
          exception_table,
          security_table,
          basereloc_table,
          debug_directory,
          /** architecture specific (or copyright on x86) */
          architecture,
          globalptr,
          tls_table,
          load_config_table,
          bound_import_table,
          iat_table,
          delay_import_table,
          /** COM runtime descriptor */
          com_descriptor,

          number_of_directory_entries = 16
        };
      };


      struct optional_header32
      {
        static const uint16_t signature = 0x010B;
        struct characteristics 
        {
          enum values {
            /**  DLL can move (ASLR) */
            dynamic_base          = 0x0040,
            /**  Code Integrity Image */
            force_integrity       = 0x0080,
            /** Image is NX compatible */
            nx_compat             = 0x0100,
            /** Image understands isolation and doesn't want it */
            no_isolation          = 0x0200,
            /** Image does not use SEH.  No SE handler may reside in this image */
            no_seh                = 0x0400,
            /** Do not bind this image. */
            no_bind               = 0x0800,
            /** Driver uses WDM model */
            wdm_driver            = 0x2000,
            terminal_server_aware = 0x8000
          };
        };
        struct subsystem
        {
          enum values {
            /**  // image doesn't require a subsystem. */
            native                = 1,
            /**                // image runs in the windows gui subsystem. */
            windows_gui,
            /**                // image runs in the windows character subsystem. */
            windows_cui,
            /**  // image runs in the os/2 character subsystem. */
            os2_cui               = 5,
            /**  // image runs in the posix character subsystem. */
            posix_cui             = 7,
            /**             // image is a native win9x driver. */
            native_windows,
            /**             // image runs in the windows ce subsystem. */
            windows_ce_gui,
            efi_application,
            efi_boot_service_driver,
            efi_runtime_driver,
            efi_rom,
            xbox,
            windows_boot_application = 16
          };
        };
        bool is_valid() const { return Magic == signature; }

        uint16_t        Magic;
        uint8_t         MajorLinkerVersion;
        uint8_t         MinorLinkerVersion;
        uint32_t        SizeOfCode;
        uint32_t        SizeOfInitializedData;
        uint32_t        SizeOfUninitializedData;
        uint32_t        AddressOfEntryPoint;
        uint32_t        BaseOfCode;
        uint32_t        BaseOfData;
        uint32_t        ImageBase;
        uint32_t        SectionAlignment;
        uint32_t        FileAlignment;
        uint16_t        MajorOperatingSystemVersion;
        uint16_t        MinorOperatingSystemVersion;
        uint16_t        MajorImageVersion;
        uint16_t        MinorImageVersion;
        uint16_t        MajorSubsystemVersion;
        uint16_t        MinorSubsystemVersion;
        uint32_t        Win32VersionValue;
        uint32_t        SizeOfImage;
        uint32_t        SizeOfHeaders;
        uint32_t        CheckSum;
        uint16_t        Subsystem;
        uint16_t        DllCharacteristics;
        uint32_t        SizeOfStackReserve;
        uint32_t        SizeOfStackCommit;
        uint32_t        SizeOfHeapReserve;
        uint32_t        SizeOfHeapCommit;
        uint32_t        LoaderFlags;
        uint32_t        NumberOfRvaAndSizes;
        data_directory  DataDirectory[data_directory::number_of_directory_entries];
      }; // struct optional_header32

      STATIC_ASSERT(sizeof(optional_header32) == 224);

      struct optional_header64
      {
        static const uint16_t signature = 0x020B;
        struct characteristics 
        {
          enum values {
            /**  DLL can move (ASLR) */
            dynamic_base          = 0x0040,
            /**  Code Integrity Image */
            force_integrity       = 0x0080,
            /** Image is NX compatible */
            nx_compat             = 0x0100,
            /** Image understands isolation and doesn't want it */
            no_isolation          = 0x0200,
            /** Image does not use SEH.  No SE handler may reside in this image */
            no_seh                = 0x0400,
            /** Do not bind this image. */
            no_bind               = 0x0800,
            /** Driver uses WDM model */
            wdm_driver            = 0x2000,
            terminal_server_aware = 0x8000
          };
        };
        struct subsystem
        {
          enum values {
            /**  // image doesn't require a subsystem. */
            native                = 1,
            /**                // image runs in the windows gui subsystem. */
            windows_gui,
            /**                // image runs in the windows character subsystem. */
            windows_cui,
            /**  // image runs in the os/2 character subsystem. */
            os2_cui               = 5,
            /**  // image runs in the posix character subsystem. */
            posix_cui             = 7,
            /**             // image is a native win9x driver. */
            native_windows,
            /**             // image runs in the windows ce subsystem. */
            windows_ce_gui,
            efi_application,
            efi_boot_service_driver,
            efi_runtime_driver,
            efi_rom,
            xbox,
            windows_boot_application = 16
          };
        };
        bool is_valid() const { return Magic == signature; }

        uint16_t        Magic;
        uint8_t         MajorLinkerVersion;
        uint8_t         MinorLinkerVersion;
        uint32_t        SizeOfCode;
        uint32_t        SizeOfInitializedData;
        uint32_t        SizeOfUninitializedData;
        uint32_t        AddressOfEntryPoint;
        uint32_t        BaseOfCode;
        uint64_t        ImageBase;
        uint32_t        SectionAlignment;
        uint32_t        FileAlignment;
        uint16_t        MajorOperatingSystemVersion;
        uint16_t        MinorOperatingSystemVersion;
        uint16_t        MajorImageVersion;
        uint16_t        MinorImageVersion;
        uint16_t        MajorSubsystemVersion;
        uint16_t        MinorSubsystemVersion;
        uint32_t        Win32VersionValue;
        uint32_t        SizeOfImage;
        uint32_t        SizeOfHeaders;
        uint32_t        CheckSum;
        uint16_t        Subsystem;
        uint16_t        DllCharacteristics;
        uint64_t        SizeOfStackReserve;
        uint64_t        SizeOfStackCommit;
        uint64_t        SizeOfHeapReserve;
        uint64_t        SizeOfHeapCommit;
        uint32_t        LoaderFlags;
        uint32_t        NumberOfRvaAndSizes;
        data_directory  DataDirectory[data_directory::number_of_directory_entries];
      }; // struct optional_header64

      STATIC_ASSERT(sizeof(optional_header64) == 240);

      struct nt_headers
      {
        static const uint32_t signature = 0x00004550; // PE\x0\x0
        bool is_valid() const { return Signature == signature; }

        uint32_t            Signature;
        file_header         FileHeader;
        union 
        {
          optional_header32 OptionalHeader32;
          optional_header64 OptionalHeader64;
        };

        image::optional_header32 * optional_header32()
        {
          return OptionalHeader32.is_valid() ? &OptionalHeader32 : 0;
        }

        const image::optional_header32 * optional_header32() const
        {
          return OptionalHeader32.is_valid() ? &OptionalHeader32 : 0;
        }

        image::optional_header64 * optional_header64()
        {
          return OptionalHeader64.is_valid() ? &OptionalHeader64 : 0;
        }

        const image::optional_header64 * optional_header64() const
        {
          return OptionalHeader64.is_valid() ? &OptionalHeader64 : 0;
        }

        const image::data_directory * data_directory(data_directory::entry entry) const
        {
          if ( optional_header32()
            && optional_header32()->NumberOfRvaAndSizes > uint32_t(entry) )
            return &optional_header32()->DataDirectory[entry];
          else if ( optional_header64()
            && optional_header64()->NumberOfRvaAndSizes > uint32_t(entry) )
            return &optional_header64()->DataDirectory[entry];
          return 0;
        }

        image::data_directory * data_directory(data_directory::entry entry)
        {
          return const_cast<image::data_directory*>(
            const_cast<const nt_headers*>(this)->data_directory(entry));
        }

      }; // struct nt_headers

      image::data_directory * get_data_directory(data_directory::entry entry)
      {
        nt_headers * const nth = get_nt_headers();
        return nth->is_valid() ? nth->data_directory(entry) : 0;
      }

      const image::data_directory * 
        get_data_directory(data_directory::entry entry) const
      {
        const nt_headers * const nth = get_nt_headers();
        return nth->is_valid() ? nth->data_directory(entry) : 0;
      }

      uintptr_t base() const
      {
        return reinterpret_cast<uintptr_t>(this); 
      }

      /// converts RVA to VA
      template<typename T>
      T va(uintptr_t rva) const
      {
        return reinterpret_cast<T>(base() + rva);
      }

      uintptr_t va(uintptr_t rva) const
      {
        return base() + rva;
      }

      /// converts VA to RVA
      uintptr_t rva(uintptr_t va) const
      {
        return va - base();
      }

      dos_header * get_dos_header()
      { 
        return va<dos_header*>(0); 
      }

      const dos_header * get_dos_header() const
      { 
        return va<const dos_header*>(0); 
      }

      nt_headers * get_nt_headers() 
      { 
        return va<nt_headers*>(static_cast<uintptr_t>(get_dos_header()->e_lfanew)); 
      }

      const nt_headers * get_nt_headers() const
      { 
        return va<const nt_headers*>(static_cast<uintptr_t>(get_dos_header()->e_lfanew)); 
      }

      uint32_t checksum() const
      {
        const dos_header * const dh = get_dos_header();
        if ( !dh->is_valid() ) return 0;
        const nt_headers * const nth = get_nt_headers();
        uint32_t sum32 = 0;
        for ( const uint16_t * p = va<uint16_t*>(0);
          p < va<uint16_t*>(nth->OptionalHeader32.SizeOfImage); // < care about odd SizeOfImage
          ++p )
        {
          sum32 += *p;
          sum32 = (sum32 >> 16) + static_cast<uint16_t>(sum32);
        }
        uint16_t sum = static_cast<uint16_t>((sum32 >> 16) + sum32);
        sum = sum - ( sum < static_cast<uint16_t>(nth->OptionalHeader32.CheckSum) );
        sum = sum - static_cast<uint16_t>(nth->OptionalHeader32.CheckSum);
        sum = sum - ( sum < static_cast<uint16_t>(nth->OptionalHeader32.CheckSum >> 16) );
        sum = sum - static_cast<uint16_t>(nth->OptionalHeader32.CheckSum >> 16);
        return nth->OptionalHeader32.SizeOfImage + sum;
      }

      uint32_t checksum(bool update)
      {
        const uint32_t sum = checksum();
        if ( sum && update ) get_nt_headers()->OptionalHeader32.CheckSum = sum;
        return sum;
      }


      struct section_header  
      {
        struct characteristics
        {
          enum values {
            type_reg                   = 0x00000000,
            type_dsect                 = 0x00000001,
            type_noload                = 0x00000002,
            type_group                 = 0x00000004,
            type_no_pad                = 0x00000008,
            type_copy                  = 0x00000010,
            /** section contains code. */
            cnt_code                   = 0x00000020,
            /** section contains initialized data. */
            cnt_initialized_data       = 0x00000040,
            /** section contains uninitialized data. */
            cnt_uninitialized_data     = 0x00000080,
            lnk_other                  = 0x00000100,
            /** section contains comments or some other type of information. */
            lnk_info                   = 0x00000200,
            type_over                  = 0x00000400,
            /** section contents will not become part of image. */
            lnk_remove                 = 0x00000800,
            /** section contents comdat. */
            lnk_comdat                 = 0x00001000,
            /** obsolete */
            mem_protected              = 0x00004000,
            /** reset speculative exceptions handling bits in the tlb entries for this section. */
            no_defer_spec_exc          = 0x00004000,
            /** section content can be accessed relative to gp */
            gprel                      = 0x00008000,

            mem_fardata                = 0x00008000,
            /** obsolete */
            mem_sysheap                = 0x00010000,
            mem_purgeable              = 0x00020000,
            mem_16bit                  = 0x00020000,
            mem_locked                 = 0x00040000,
            mem_preload                = 0x00080000,

            align_1bytes               = 0x00100000,
            align_2bytes               = 0x00200000,
            align_4bytes               = 0x00300000,
            align_8bytes               = 0x00400000,
            /** default alignment if no others are specified. */
            align_16bytes              = 0x00500000,
            align_32bytes              = 0x00600000,
            align_64bytes              = 0x00700000,
            align_128bytes             = 0x00800000,
            align_256bytes             = 0x00900000,
            align_512bytes             = 0x00a00000,
            align_1024bytes            = 0x00b00000,
            align_2048bytes            = 0x00c00000,
            align_4096bytes            = 0x00d00000,
            align_8192bytes            = 0x00e00000,
            align_mask                 = 0x00f00000,

            /** section contains extended relocations. */
            lnk_nreloc_ovfl            = 0x01000000,
            /** section can be discarded. */
            mem_discardable            = 0x02000000,
            /** section is not cachable. */
            mem_not_cached             = 0x04000000,
            /** section is not pageable. */
            mem_not_paged              = 0x08000000,
            /** section is shareable. */
            mem_shared                 = 0x10000000,
            /** section is executable. */
            mem_execute                = 0x20000000,
            /** section is readable. */
            mem_read                   = 0x40000000,
            /** section is writeable. */
            mem_write                  = 0x80000000
          };
        };
        static const size_t sizeof_short_name = 8;
        char        Name[sizeof_short_name];
        union
        { 
          uint32_t  PhysicalAddress;
          uint32_t  VirtualSize;
        };
        uint32_t    VirtualAddress;
        uint32_t    SizeOfRawData;
        uint32_t    PointerToRawData;
        uint32_t    PointerToRelocations;
        uint32_t    PointerToLinenumbers;
        uint16_t    NumberOfRelocations;
        uint16_t    NumberOfLinenumbers;
        uint32_t    Characteristics;
      };

      STATIC_ASSERT(sizeof(section_header) == 40);

      section_header * get_section_header(size_t n = 0)
      {
        nt_headers * const nth = get_nt_headers();
        section_header * sh = (section_header*)(uintptr_t(&nth->OptionalHeader32)
          + nth->FileHeader.SizeOfOptionalHeader);
        return n < nth->FileHeader.NumberOfSections ? &sh[n] : 0;
      }

      const section_header * get_section_header(size_t n = 0) const
      {
        const nt_headers * const nth = get_nt_headers();
        const section_header * sh = (section_header*)(uintptr_t(&nth->OptionalHeader32)
          + nth->FileHeader.SizeOfOptionalHeader);
        return n < nth->FileHeader.NumberOfSections ? &sh[n] : 0;
      }

      section_header * get_section_header(const char name[])
      {
        nt_headers * const nth = get_nt_headers();
        section_header * sh = (section_header*)(uintptr_t(&nth->OptionalHeader32)
          + nth->FileHeader.SizeOfOptionalHeader);
        size_t n = nth->FileHeader.NumberOfSections;
        while ( n )
          if ( !std::strncmp(name, &sh[--n].Name[0], sizeof(sh->Name)) )
            return &sh[n];
        return 0;
      }

      const section_header * get_section_header(const char name[]) const
      {
        const nt_headers * const nth = get_nt_headers();
        const section_header * sh = (section_header*)(uintptr_t(&nth->OptionalHeader32)
          + nth->FileHeader.SizeOfOptionalHeader);
        size_t n = nth->FileHeader.NumberOfSections;
        while ( n )
          if ( !std::strncmp(name, &sh[--n].Name[0], sizeof(sh->Name)) )
            return &sh[n];
        return 0;
      }

      struct import_by_name
      {
        uint16_t  Hint;
        char      Name[1];
      };

      union thunk_data32
      {
        uint32_t  ForwarderString;  // char*
        uint32_t  Function;         // uint32_t*
        uint32_t  Ordinal;
        uint32_t  AddressOfData;    // import_by_name*
      };

      union thunk_data64
      {
        uint64_t  ForwarderString;  // char*
        uint64_t  Function;         // uint32_t*
        uint64_t  Ordinal;
        uint64_t  AddressOfData;    // import_by_name*
      };

      ///\name  Export support

      struct export_directory 
      {
        uint32_t  Characteristics;
        uint32_t  TimeDateStamp;
        uint16_t  MajorVersion;
        uint16_t  MinorVersion;
        uint32_t  Name;
        uint32_t  Base;                   // Ordinal Base
        uint32_t  NumberOfFunctions;      // Address Table Entries
        uint32_t  NumberOfNames;          // Number of Name Pointers
        uint32_t  AddressOfFunctions;     // Export Address Table RVA
        uint32_t  AddressOfNames;         // Name Pointer RVA
        uint32_t  AddressOfNameOrdinals;  // Ordinal Table RVA

        ///\note  binary search is not for speed here -
        ///       sequential one could give different results on tricky PEs
        ///       and probably is exposed for some exploitation 
        __forceinline
          uint32_t ordinal(const image * pe, const char name[]) const
        {
          const uint32_t * const name_table = pe->va<uint32_t*>(AddressOfNames);
          uint32_t l = 0, h = NumberOfNames - 1;
          while ( h >= l )
          {
            const uint32_t m = (l + h) / 2;
            const int r = std::strcmp(pe->va<char*>(name_table[m]), name);
            if ( ! r ) return (pe->va<const uint16_t*>(AddressOfNameOrdinals))[m];
            else if ( r > 0) h = m - 1;
            else if ( r < 0) l = m + 1;
          }
          return 0xffffffff;
        }

        uint32_t ordinal(const image * /*pe*/, uint16_t ordinal) const
        {
          return ordinal - Base;
        }

        void * function(const image * pe, uint32_t ordinal) const 
        {
          return ordinal < NumberOfFunctions
            ? pe->va<void*>(pe->va<uint32_t*>(AddressOfFunctions)[ordinal]) : 0;
        }

        template<class Functor>
        uint32_t ordinal(const image* pe, Functor finder) const
        {
          const uint32_t * const name_table = pe->va<uint32_t*>(AddressOfNames);
          for(uint32_t n = 0; n < NumberOfNames-1; n++){
            if(finder(pe->va<const char*>(name_table[n])))
              return pe->va<const uint16_t*>(AddressOfNameOrdinals)[n];
          }
          return 0xffffffff;
        }

      }; // struct export_directory

      typedef
        const image * find_dll_t(const char * dll_name);

      template<typename ExportType>
      void * find_export(ExportType exp) const
      {
        const data_directory * const export_table = 
          get_data_directory(data_directory::export_table);
        if ( ! export_table || ! export_table->VirtualAddress ) return 0;
        export_directory * exports = va<export_directory*>(export_table->VirtualAddress);
        void * const f = exports->function(this, exports->ordinal(this, exp));
        const uintptr_t ex = reinterpret_cast<uintptr_t>(exports);
        return in_range(ex, ex + export_table->Size, f) ? 0 : f;
      }

      template<class Functor>
      void* find_export_f(Functor finder) const
      {
        const data_directory * const export_table = 
          get_data_directory(data_directory::export_table);
        if (!export_table || !export_table->VirtualAddress)
          return 0;

        export_directory* exports = va<export_directory*>(export_table->VirtualAddress);
        void * const f = exports->function(this, exports->ordinal(this, finder));
        const uintptr_t ex = reinterpret_cast<uintptr_t>(exports);
        return in_range(ex, ex + export_table->Size, f) ? 0 : f;
      }

      template<typename DllFinder>
      void * find_export(const char * exp, DllFinder find_dll) const
      {
        const data_directory * const export_table = 
          get_data_directory(data_directory::export_table);
        if ( ! export_table || ! export_table->VirtualAddress ) return 0;
        export_directory * exports = va<export_directory*>(export_table->VirtualAddress);
        const uint32_t ordinal = uintptr_t(exp) <= 0xFFFF 
          ? exports->ordinal(this, reinterpret_cast<uint16_t>(exp))
          : exports->ordinal(this, exp);
        void * const f = exports->function(this, ordinal);
        const uintptr_t ex = reinterpret_cast<uintptr_t>(exports);
        if ( !in_range(ex, ex + export_table->Size, f) )
          return f;

        // forward export
        static const size_t dll_name_max = 16;
        char dll_name[dll_name_max + sizeof("dll")];
        const char * forward = reinterpret_cast<char*>(f);
        size_t i = 0;
        for ( ; ; )
        {
          if ( i == dll_name_max ) return 0;
          const char c = *forward++;
          dll_name[i++] = c;
          if ( c == '.' ) break;
        }
        dll_name[i++] = 'd';
        dll_name[i++] = 'l';
        dll_name[i++] = 'l';
        dll_name[i] = '\0';
        const image * forwarded_dll = find_dll(dll_name);
        return forwarded_dll ? forwarded_dll->find_export(forward, find_dll) : 0;
      }

      template<typename PtrType, typename ExportType>
      PtrType find_export(ExportType exp) const
      {
        return brute_cast<PtrType>(find_export(exp));
      }
      template<typename PtrType, class Functor>
      PtrType find_export_f(Functor finder) const
      {
        return brute_cast<PtrType>(find_export(finder));
      }
      template<typename PtrType, typename DllFinder>
      PtrType find_export(const char * exp, DllFinder find_dll) const
      {
        return brute_cast<PtrType>(find_export(exp, find_dll));
      }

      template<typename PtrType, typename DllFinder>
      PtrType find_export(uint16_t exp, DllFinder find_dll) const
      {
        return brute_cast<PtrType>(find_export(reinterpret_cast<const char*>(exp), find_dll));
      }

      ///\name  Imports

      /** Import directory entry */
      struct import_descriptor
      {
        union
        {
          /** obsolete */
          uint32_t  Characteristics;
          /** The RVA of the import lookup table. This table contains a name or ordinal for each import. */
          uint32_t  OriginalFirstThunk;
        };
        /** The stamp that is set to zero until the image is bound. 
          After the image is bound, this field is set to the time/data stamp of the DLL. */
        uint32_t  TimeDateStamp;
        /** The index of the first forwarder reference. */
        uint32_t  ForwarderChain;
        /** RVA. The address of an ASCII string that contains the name of the DLL. */
        uint32_t  Name;
        /** The RVA of the import address table. The contents of this table are identical 
          to the contents of the import lookup table until the image is bound. */
        uint32_t  FirstThunk;

        bool is_terminating() const { return OriginalFirstThunk == 0; }
        bool bound() const { return TimeDateStamp != 0; }
        bool no_forwarders() const { return ForwarderChain == -1; }

        uintptr_t * find(const image * const img, const char * const import_name) const
        {
          uintptr_t * iat = img->va<uintptr_t*>(FirstThunk);
          for (intptr_t * hint_name = img->va<intptr_t*>(OriginalFirstThunk);
            *hint_name && *hint_name >= 0;
            ++hint_name, ++iat)
          {
            if(!std::strcmp(img->va<const char*>(*hint_name) + 2, // skip Hint
              import_name))
              return iat;
          }
          return 0;
        }

        uintptr_t * find(const image * const img, uint16_t import_ordinal) const
        {
          uintptr_t * iat = img->va<uintptr_t*>(FirstThunk);
          for (intptr_t * hint_name = img->va<intptr_t*>(OriginalFirstThunk);
            *hint_name && *hint_name < 0;
            ++hint_name, ++iat)
          {
            if(static_cast<uint16_t>(*hint_name) == import_ordinal)
              return iat;
          }
          return 0;
        }

        template<class Functor>
        uintptr_t * find(const image* const img, Functor finder) const
        {
          uintptr_t* iat = img->va<uintptr_t*>(FirstThunk);
          for (intptr_t * hint_name = img->va<intptr_t*>(OriginalFirstThunk);
            *hint_name && *hint_name >= 0;
            ++hint_name, ++iat)
          {
            if(finder(img->va<const char*>(*hint_name) + 2))
              return iat;
          }
          return 0;
        }


      };

      /** Import lookup table (x86) */
      struct import_lookup_table32
      {
        union {
          /** A 16-bit ordinal number. */
          uint16_t    Ordinal;
          struct {
            /** A 31-bit RVA of a hint/name table entry. */
            uint32_t  Name:31;
            /** Ordinal/Name Flag */
            uint32_t  IsOrdinal:1;
          } f;
        } u;
      };

      /**
       *	Import lookup table (x64)
       *  @see import_lookup_table32
       **/
      struct import_lookup_table64
      {
        union {
          uint16_t    Ordinal;
          struct {
            uint64_t  Name:31;
            uint64_t  :32;
            uint64_t  IsOrdinal:1;
          } s;
        } u;
      };

      /** Hint/Name import table */
      struct import_name_table
      {
        /** An index into the export name pointer table. A match is attempted first with this value. 
          If it fails, a binary search is performed on the DLL’s export name pointer table. */
        uint16_t Hint;
        /** An ASCII string that contains the name to import. This is the string that must be matched 
          to the public name in the DLL. This string is case sensitive and terminated by a null byte. */
        char     Name;
      };

      static inline uintptr_t & null_import()
      {
        static uintptr_t _0;
        return _0;
      }

      import_descriptor *
        get_first_import_entry() const
      {
        const data_directory * const import_table = 
          get_data_directory(data_directory::import_table);
        return import_table && import_table->VirtualAddress 
          ? va<import_descriptor*>(import_table->VirtualAddress)
          : 0;
      }

      import_descriptor *
        find_import_entry(const char * const module_name) const
      {
        if ( !module_name ) return 0;
        for ( import_descriptor * import_entry = get_first_import_entry();
          import_entry && !import_entry->is_terminating();
          ++import_entry )
        {
          if ( !import_entry->Name ) continue;
          // compare names case-insensitive (simpified)
          const char * const name = va<const char*>(import_entry->Name);
          for ( unsigned i = 0; module_name[i]; ++i )
            if ( (module_name[i] ^ name[i]) & 0x5F) goto next_entry;
          return import_entry;
next_entry:;
        }
        return 0;
      }

      template<class Functor>
      const import_descriptor *
        find_import_entry_f(Functor finder) const
      {
        for (const import_descriptor * import_entry = get_first_import_entry();
          import_entry && !import_entry->is_terminating();
          ++import_entry)
        {
          if (!import_entry->Name) continue;
          const char * const name = va<const char*>(import_entry->Name);
          if(finder(name))
            return import_entry;
        }
        return 0;
      }

      uintptr_t& find_iat_entry(
        const char*  const import_name,
        const char*  const module = 0) const
      {
        for (import_descriptor * import_entry = get_first_import_entry();
          import_entry && !import_entry->is_terminating();
          ++import_entry)
        {
          if (module){ 
            if (!import_entry->Name) 
              goto next_entry;

            // compare names case-insensitive (simpified)
            const char* const name = va<const char*>(import_entry->Name);
            for (unsigned i = 0; module[i]; ++i)
              if((module[i] ^ name[i]) & 0x5F)
                goto next_entry;
          }
          if(uintptr_t* iat = import_entry->find(this, import_name))
            return *iat;
  next_entry:;
        }
        return null_import();
      }

      template<typename DllFinder>
      bool bind_import(const DllFinder & find_dll)
      {
        for ( import_descriptor * import_entry = get_first_import_entry();
          import_entry && !import_entry->is_terminating();
          ++import_entry )
        {
          if ( ! import_entry->Name ) return false;
          const image * const dll =
            find_dll(va<const char*>(import_entry->Name));
          if ( ! dll ) return false;
          void ** iat = va<void**>(import_entry->FirstThunk);
          for ( intptr_t * hint_name = va<intptr_t*>(import_entry->OriginalFirstThunk);
            *hint_name;
            ++hint_name, ++iat )
          {
            *iat = *hint_name < 0
              ? dll->find_export(static_cast<uint16_t>(*hint_name))
              : dll->find_export(va<const char*>(*hint_name) + 2, find_dll);
            if ( !*iat ) return false;
          }
        }
        return true;
      }


      ///\name  Relocations

      struct base_relocation 
      {
        uint32_t  VirtualAddress;
        uint32_t  SizeOfBlock;
        struct entry_t
        {
          uint16_t  Offset : 12;
          uint16_t  Type   : 4;
        } entry[1];

        /** base relocation types */
        enum types
        {
          absolute,
          high,
          low,
          highlow,
          highadj,
          mips_jmpaddr,
          mips_jmpaddr16= 9,
          ia64_imm64    = 9,
          dir64
        };

        /** x86 relocations */
        struct type32
        {
          enum values {
            absolute  = 0x00,
            high      = 0x01,
            low       = 0x02,
            highlow   = 0x03,
            dir32     = 0x06,
            dir32nb   = 0x07,
            seg12     = 0x09,
            section   = 0x0A,
            secrel    = 0x0B,
            token     = 0x0C,
            secrel7   = 0x0D,
            rel32     = 0x14,
          };
        };

        /** x64 relocations */
        struct type64
        {
          enum values {
            /** reference is absolute, no relocation is necessary */
            absolute        = 0x0000,
            /** 64-bit address (va). */
            addr64          = 0x0001,
            /** 32-bit address (va). */
            addr32          = 0x0002,
            /** 32-bit address w/o image base (rva). */
            addr32nb        = 0x0003,
            /** 32-bit relative address from byte following reloc */
            rel32           = 0x0004,
            /** 32-bit relative address from byte distance 1 from reloc */
            rel32_1         = 0x0005,
            /** 32-bit relative address from byte distance 2 from reloc */
            rel32_2         = 0x0006,
            /** 32-bit relative address from byte distance 3 from reloc */
            rel32_3         = 0x0007,
            /** 32-bit relative address from byte distance 4 from reloc */
            rel32_4         = 0x0008,
            /** 32-bit relative address from byte distance 5 from reloc */
            rel32_5         = 0x0009,
            /** section index */
            section         = 0x000a,
            /** 32 bit offset from base of section containing target */
            secrel          = 0x000b,
            /** 7 bit unsigned offset from base of section containing target */
            secrel7         = 0x000c,
            /** 32 bit metadata token */
            token           = 0x000d,
            /** 32 bit signed span-dependent value emitted into object */
            srel32          = 0x000e,
            pair            = 0x000f,
            /** 32 bit signed span-dependent value applied at link time */
            sspan32         = 0x0010
          };
        };
      };

      bool relocate(ptrdiff_t delta)
      {
        const data_directory * const reloc_dir = 
          get_data_directory(data_directory::basereloc_table);
        if ( ! reloc_dir || ! reloc_dir->VirtualAddress ) return false;
        const base_relocation * fixups = va<base_relocation*>(reloc_dir->VirtualAddress);
        const uintptr_t end = va(reloc_dir->VirtualAddress + reloc_dir->Size);
        while ( reinterpret_cast<uintptr_t>(fixups) < end )
        {
          const uintptr_t addr = va(fixups->VirtualAddress);
          const uintptr_t end = fixups->SizeOfBlock
            + reinterpret_cast<uintptr_t>(fixups);
          const base_relocation::entry_t * entry = &fixups->entry[0];
          for ( ; reinterpret_cast<uintptr_t>(entry) < end; ++entry )
            switch ( entry->Type )
          {
            case base_relocation::type32::highlow:
              *reinterpret_cast<uint32_t*>(addr + entry->Offset) += static_cast<uint32_t>(delta);
              break;
            default:
              break;
          }
          fixups = reinterpret_cast<const base_relocation*>(entry);
        }
        return true;
      }

      ///\name Resources

      struct resource_directory_entry 
      {
        union 
        {
          struct
          {
            uint32_t  Offset:31;
            uint32_t  IsString:1;
          } Name;
          uint16_t  Id;
        };
        uint32_t  OffsetToDirectory:31;
        uint32_t  DataIsDirectory:1;
      };

      struct resource_directory
      {
        uint32_t  Characteristics;
        uint32_t  TimeDateStamp;
        uint16_t  MajorVersion;
        uint16_t  MinorVersion;
        uint16_t  NumberOfNamedEntries;
        uint16_t  NumberOfIdEntries;
      };

      resource_directory_entry* get_res_named_entry(size_t n = 0)
      {
        const data_directory* const resd = get_data_directory(data_directory::resource_table);
        if(!resd || !resd->VirtualAddress)
          return 0;
        resource_directory* rsrc = va<resource_directory*>(resd->VirtualAddress);
        return n < rsrc->NumberOfNamedEntries
          ? &reinterpret_cast<resource_directory_entry*>(uintptr_t(rsrc) + sizeof(resource_directory)) [n]
          : 0;
      }

      resource_directory_entry* get_res_entry(size_t n = 0)
      {
        const data_directory* const resd = get_data_directory(data_directory::resource_table);
        if(!resd || !resd->VirtualAddress)
          return 0;
        resource_directory* rsrc = va<resource_directory*>(resd->VirtualAddress);
        if(!(n < rsrc->NumberOfIdEntries))
          return 0;
        resource_directory_entry* ride = reinterpret_cast<resource_directory_entry*>(
          uintptr_t(rsrc) + sizeof(resource_directory) + 
          sizeof(resource_directory_entry)*rsrc->NumberOfNamedEntries);
        return &ride[n];
        //return & reinterpret_cast<resource_directory_entry*>
        //  (uintptr_t(rsrc) + sizeof(resource_directory) + sizeof(resource_directory_entry)*rsrc->NumberOfNamedEntries)
        //  [n];
      }

      struct resource_dir_string
      {
        uint16_t  Length;
        wchar_t   NameString[1];
      };

      struct resource_data_entry 
      {
        uint32_t  OffsetToData;
        uint32_t  Size;
        uint32_t  CodePage;
        uint32_t  Reserved;
      };

      ///\name Security table
      struct security_table
      {
        uint32_t  OffsetToData;
        uint32_t  Size;
      };

      struct security_table_entry 
      {
        uint32_t  dwLength;
        // 0x100 - 1.0, 0x200 - 2.0
        uint16_t  wRevision;
        // 1 - x509, 2 - PCS#7, 3 - reserved, 4 - Terminal Server Protocol Stack certificate  signing
        uint16_t  wCertificateType;
        uint8_t   cert_data[1];
      };


      ///\name TLS directory

      typedef void __stdcall tls_callback_t (image* handle, ntl::nt::DllMainReason reason, const void*);
      struct tls_directory32
      {
        uint32_t StartAddressOfRawData;
        uint32_t EndAddressOfRawData;
        uint32_t AddressOfIndex;             // uint32_t*
        uint32_t AddressOfCallBacks;         // tls_callback_t**
        uint32_t SizeOfZeroFill;
        uint32_t Characteristics;
      };

      struct tls_directory64
      {
        uint64_t StartAddressOfRawData;
        uint64_t EndAddressOfRawData;
        uint64_t AddressOfIndex;             // uint32_t*
        uint64_t AddressOfCallBacks;         // tls_callback_t**
        uint64_t SizeOfZeroFill;
        uint64_t Characteristics;
      };

      ///\name Load configuration
      struct load_config_directory32
      {
        uint32_t   Size;
        uint32_t   TimeDateStamp;
        uint16_t   MajorVersion;
        uint16_t   MinorVersion;
        uint32_t   GlobalFlagsClear;
        uint32_t   GlobalFlagsSet;
        uint32_t   CriticalSectionDefaultTimeout;
        uint32_t   DeCommitFreeBlockThreshold;
        uint32_t   DeCommitTotalFreeThreshold;
        uint32_t   LockPrefixTable;            // VA
        uint32_t   MaximumAllocationSize;
        uint32_t   VirtualMemoryThreshold;
        uint32_t   ProcessHeapFlags;
        uint32_t   ProcessAffinityMask;
        uint16_t   CSDVersion;
        uint16_t   Reserved1;
        uint32_t   EditList;                   // VA
        uint32_t   SecurityCookie;             // VA
        uint32_t   SEHandlerTable;             // VA
        uint32_t   SEHandlerCount;
      };

      struct load_config_directory64
      {
        uint32_t   Size;
        uint32_t   TimeDateStamp;
        uint16_t   MajorVersion;
        uint16_t   MinorVersion;
        uint32_t   GlobalFlagsClear;
        uint32_t   GlobalFlagsSet;
        uint32_t   CriticalSectionDefaultTimeout;
        uint64_t   DeCommitFreeBlockThreshold;
        uint64_t   DeCommitTotalFreeThreshold;
        uint64_t   LockPrefixTable;            // VA
        uint64_t   MaximumAllocationSize;
        uint64_t   VirtualMemoryThreshold;
        uint64_t   ProcessAffinityMask;
        uint32_t   ProcessHeapFlags;
        uint16_t   CSDVersion;
        uint16_t   Reserved1;
        uint64_t   EditList;                   // VA
        uint64_t   SecurityCookie;             // VA
        uint64_t   SEHandlerTable;             // VA
        uint64_t   SEHandlerCount;
      };

      struct image_runtime_function_entry
      {
        uint32_t BeginAddress;
        uint32_t EndAddress;
        uint32_t UnwindInfoAddress;
      };

      struct image_function_entry
      {
        uint32_t StartingAddress;
        uint32_t EndingAddress;
        uint32_t EndOfPrologue;
      };

      struct image_function_entry64
      {
        uint64_t StartingAddress;
        uint64_t EndingAddress;
        uint64_t EndOfPrologue;
      };

      ///}

      ///////////////////////////////////////////////////////////////////////////
    private:

    };// class image

    extern "C" image  __ImageBase;

    __forceinline image * image::this_module()
    {
      return &__ImageBase;
    }


    /**@} pe_images_support */

#pragma warning(pop)

  }//namespace pe
}//namespace ntl

#endif//#ifndef NTL__PE_IMAGE
