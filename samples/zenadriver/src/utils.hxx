//
#include <pe/image.hxx>
#include <nt/registry.hxx>

template<typename String, typename String2>
static inline
bool equal_filenames(const String & s1, const String2 & s2)
{
  String2::const_reverse_iterator it2 = s2.rbegin();
  if ( s1.empty() || s2.empty() || *it2 == '\\' ) return false;
  if ( *it2 == '\0' ) ++it2;
  for ( String::const_reverse_iterator it = s1.rbegin() + (*s1.rbegin() == '\0');
        it != s1.rend();
        ++it, ++it2 )
  { 
    if ( it2 == s2.rend() ) return *it == '\\';
    if ( *it - (*it >= 'a' && *it <= 'z' ? 'a' - 'A' : 0)
      != *it2 - (*it2 >= 'a' && *it2 <= 'z' ? 'a' - 'A' : 0) )
      return false;
    if ( *it == '\\' ) break;
  }
  return it2 == s2.rend() || *it2 == '\\';
}


template<class wstringContainer>
static inline
bool get_active_drivers_list(wstringContainer & drvs)
{
  const system_information<system_modules_information> active_drivers;
  if ( !active_drivers ) return false;
  for ( system_modules_information::const_iterator it = active_drivers->begin();
        it != active_drivers->end();
        ++it )
  {
    drvs.push_back(string());
    string(it->FullPathName).swap(drvs.back());
  }
  return true;
}


static inline
void build_full_driver_path(wstring & filename)
{
  using namespace ntl::nt;
  if ( wstring::npos == filename.find('\\') )
    filename.insert(0, L"\\SystemRoot\\system32\\DRIVERS\\");
  else
  {
    wstring system_directory;
    key cv(L"\\Registry\\Machine\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
            key::query_value);
    if ( cv ) cv.query(L"SystemRoot", system_directory);
    if ( system_directory.size() > sizeof("C:") - sizeof('\0')
      && std::equal(system_directory.begin() + sizeof("C:") - sizeof('\0'),
                    system_directory.end() - (system_directory.end()[-1] == L'\0'),
                    filename.begin()) )
    {
      filename.insert(filename.begin(),
                      system_directory.begin(),
                      system_directory.begin() + sizeof("C:") - sizeof('\0'));
      filename.insert(0, L"\\??\\");     
    }
  }
}


template<class wstringContainer>
static
bool get_runnable_drivers_list(wstringContainer & drvs)
{
  const key srv_base(L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services\\");
  for ( key::subkey_iterator subkey = srv_base.subkey_begin();
        subkey != srv_base.subkey_end();
        ++subkey )
  {
    key srv( srv_base, *subkey, key::read | key::write );

    uint32_t type = service::driver;
    if ( srv.query(L"Type", type)
      && !(type & service::driver) )
      continue;

    uint32_t start;
    if ( srv.query(L"Start", start)
      && type != service::file_system_driver
      && start == service::disabled )
      continue;

    // default image path
    wstring  image_path(L"\\SystemRoot\\System32\\Drivers\\");
    image_path += *subkey;
    image_path += L".SYS";

    if ( srv.query(L"ImagePath", image_path) 
      && image_path[0] != L'\\' )
    {
      image_path.insert(0, L"\\SystemRoot\\");
    }

    drvs.push_back(image_path);
  }
  return true;
}


template<class wstringContainer>
static
bool get_video_drivers_list(wstringContainer & drvs)
{
  const key vddb(L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Control\\Video");
  for ( key::subkey_iterator vdi = vddb.subkey_begin();
        vdi != vddb.subkey_end();
        ++vdi )
  {
    key dev( vddb, *vdi );
    for ( key::subkey_iterator d_it = dev.subkey_begin();
        d_it != dev.subkey_end();
        ++d_it )
    {
      if ( *d_it == L"Video" ) continue;
      wstring d_drvs;
      if ( key(dev, *d_it).query(L"InstalledDisplayDrivers", d_drvs) )
      {
        const void * volatile const ensure_there_is_0 = d_drvs.c_str();(ensure_there_is_0);
        for ( wstring::iterator c = d_drvs.begin(); c != d_drvs.end(); )
        {
          wstring name(c);
          c += name.size() + 1;
          if ( !name.empty() )
          {
            name += L".dll";
            name.insert(0, L"\\SystemRoot\\system32\\");
            drvs.push_back(name);
          }
        }
      }
    }
  }
  return true;
}

template<class String>
static inline
bool load_image(
  const String & filename,
  raw_data & img)
{
  using namespace pe;
  file f(filename);
  ///\note image will be autoaligned due to the heap algo
  img.resize(sizeof(image::dos_header));

  // check DOS header
  if ( !f.read(img.begin(), img.size()) 
    || !image::bind(img.begin())->get_dos_header()->is_valid() )
    return false;

  // load file header and stuff before
  const raw_data::size_type fh_size =
                        sizeof(image::nt_headers::signature)
                      + sizeof(image::file_header)
                      + image::bind(img.begin())->get_dos_header()->e_lfanew
                      - img.size(); // == sizeof(image::dos_header)
  img.resize(img.size() + fh_size);
  if ( !f.read(&img[sizeof(image::dos_header)], fh_size) )
    return false;
  const image::file_header & fh =
                      image::bind(img.begin())->get_nt_headers()->FileHeader;
  const unsigned n_sec = fh.NumberOfSections;

  // load optional & section headers
  const raw_data::size_type oh_sh_size =
                fh.SizeOfOptionalHeader + n_sec * sizeof(image::section_header);
  raw_data::size_type pos = img.size();
  img.resize(img.size() + oh_sh_size);
  if ( !f.read(&img[pos], oh_sh_size)
    || !image::bind(img.begin())->get_nt_headers()->is_valid() )
    return false;

  // load stuff after headers
  pos = img.size();
  img.resize(image::bind(img.begin())->get_nt_headers()->OptionalHeader32.SizeOfImage);
  const image::optional_header32 & oh =
              image::bind(img.begin())->get_nt_headers()->OptionalHeader32;
  if ( !f.read(&img[pos], oh.SizeOfHeaders - pos) )
    return false;

  // load sections
  for ( uint32_t sec = 0; sec != n_sec; ++sec )
  {
    const image::section_header & section =
                        *image::bind(img.begin())->get_section_header(sec);
    pos = section.VirtualAddress;
    if ( !f.read(&img[pos], align_up(section.SizeOfRawData, oh.FileAlignment)) )
      return false;
  }
  return true;
}

