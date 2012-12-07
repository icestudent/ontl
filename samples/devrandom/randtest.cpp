//  NTL samples library
//  devrandom driver test.
//
//  compile:
//      cl /nologo /DUNICODE /GS- randtest.cpp "%ntl64%/rtl/crt.cpp"
//
#include <consoleapp.hxx>
#include <nt/file.hxx>

#include <cstdio>
#include <nt/system_error.hxx>

using namespace ntl;
using namespace ntl::nt;
using namespace std;

extern "C" void abort()
{

}

int consoleapp::main()
{
  command_line cmdl;
  if(cmdl.size() < 2){
    console::write("usage: randtest <filename> [size]\n");
    return 1;
  }
  console::write("randtest\n");

  char strbuf[128];
  wstring fname = cmdl[1];
  uint32_t cbread = 5;
  if(cmdl.size() > 2){
    cbread = _wtoi(cmdl[2]);
    if(cbread > _countof(strbuf))
      cbread = _countof(strbuf);
    else if(!cbread)
      cbread = 1;
  }

  file_handler f;
  rtl::relative_name relname(fname.c_str());
  ntstatus st = f.open(relname, file::generic_read, file::share_none, file::creation_options_default);
  if(!success(st)){
    uint32_t l = (uint32_t)_snprintf(strbuf, _countof(strbuf), "open error '%ws': 0x%X %s\n", fname.c_str(), st, strerror(st).c_str());
    console::write<char>(strbuf, l);
    return 3;
  }

  st = f.read(strbuf, cbread);
  uint32_t cb = (uint32_t)f.get_io_status_block().Information;
  f.close();
  if(!success(st)){
    uint32_t l = (uint32_t)_snprintf(strbuf, _countof(strbuf), "read error: 0x%X %s\n", st, strerror(st).c_str());
    console::write<char>(strbuf, l);
    return 4;
  }
  char buf[256];
  uint32_t l = (uint32_t)_snprintf(buf, _countof(buf), "readed %u of %u bytes: [", cb, cbread);
  char* p = buf+l;

  uint32_t data_len = min(cb, cbread);
  uint32_t buf_tail = (uint32_t)_countof(buf) - l - sizeof("]\n");
  uint32_t len = min(data_len, buf_tail/2);

  for(uint32_t i = 0; i < len; i++, p+=2)
    _snprintf(p, 2, "%02X", strbuf[i]);

  strcpy(p, "]\n");
  l += len*2 + sizeof("]\n") - 1;

  console::write<char>(buf, l);
  return 0;
}