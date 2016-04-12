//  NTL samples library
//  devrandom driver test.
//
//  compile:
//      cl /nologo /DUNICODE /GS- randtest.cpp "%ntl64%/rtl/crt.cpp"
//
#include "../../ntl/consoleapp.hxx"
#include "../../ntl/nt/file.hxx"

#include "../../ntl/cstdio"
#include "../../ntl/nt/system_error.hxx"
#include "../../ntl/rtl/crt.cpp"

using namespace ntl;
using namespace ntl::nt;
using namespace std;


int consoleapp::main()
{
  command_line cmdl;
  if(cmdl.size() < 2){
    console::write("usage: randtest <filename> [size] [destfile]\n");
    return 1;
  }
  console::write("randtest\n");

  char strbuf[128], databuf[1024];
  uint32_t cbread = 5;
  if(cmdl.size() > 2){
    cbread = _wtoi(cmdl[2].data());
    if(cbread > _countof(databuf))
      cbread = _countof(databuf);
    else if(!cbread)
      cbread = 1;
  }
  const wstring_ref fname = cmdl[1];
  const wstring_ref dstname = cmdl.size() > 3 ? cmdl[3] : wstring_ref();

  file_handler f;
  rtl::relative_name relname(fname.data());
  ntstatus st = f.open(relname, file::generic_read, file::share_none, file::creation_options_default);
  if(!success(st)){
    size_t l = _snprintf(strbuf, _countof(strbuf), "open error '%ws': 0x%X %s\n", fname.data(), st, strerror(st).c_str());
    console::write<char>(strbuf, (uint32_t)l);
    return 3;
  }

  st = f.read(databuf, cbread);
  uint32_t cb = (uint32_t)f.get_io_status_block().Information;
  f.close();
  if(!success(st)){
    _snprintf(strbuf, _countof(strbuf), "read error: 0x%X %s\n", st, strerror(st).c_str());
    console::write(string_ref(strbuf));
    return 4;
  }
  char buf[256];
  uint32_t l = (uint32_t)_snprintf(buf, _countof(buf), "readed %u of %u bytes: [", cb, cbread);
  char* p = buf+l;

  if (!dstname.empty()) {
    // save as file
    strcpy(p-3, "\n");
    console::write<char>(buf);

    rtl::relative_name relname(dstname.data());
    ntstatus st = f.create(relname, file::overwrite_if, file::generic_write, file::share_read | file::share_write);
    if(!success(st)) {
      _snprintf(strbuf, _countof(strbuf), "cannot open destination file: 0x%X %s\n", st, strerror(st).c_str());
      console::write<char>(strbuf);
      return 3;
    }

    f.write(databuf, cb);
    f.close();
    return 0;
  }

  const uint32_t data_len = min(cb, cbread);
  const uint32_t buf_tail = (uint32_t)_countof(buf) - l - sizeof("]\n");
  const uint32_t len = min(data_len, buf_tail/2);

  for(uint32_t i = 0; i < len; i++, p+=2)
    _snprintf(p, 2, "%02X", databuf[i]);
  strcpy(p, "]\n");
  console::write<char>(buf);
  return 0;
}
