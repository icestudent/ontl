/**
 *	@file rtlcompress.cpp
 *	@brief Sample which shows how to work with console and files with NTL library
 *	@note Compilation command-line: cl /nologo /I../../branches/x64/ntl /DWIN32 /D_UNICODE /DUNICODE /GS- rtlcompress.cpp /link /subsystem:console /libpath:your_lib_path_with_ntdll.lib 
 *	@author icestudent
 *	@version 0.3
 **/

#include <consoleapp.hxx>

#include <nt/file.hxx>

#include <nt/debug.hxx>
#include <cstdio>

/* assume that we do not work with large files */
#pragma warning(disable:4267) // 'argument' : conversion from 'size_t' to 'std::uint32_t', possible loss of data. 

using namespace ntl;
using namespace ntl::nt;
using namespace std;

class app: consoleapp
{
  wstring infile, outfile;
  bool decompress;
public:
  int proceed();
protected:
  void header()
  {
    console::write(" rtlcompress v0.1a\n");
  }
  void usage()
  {
    console::write(" usage: rtlcompress.exe [-d] <infile> <outfile>\n");
  }

  bool parse_args()
  {
    decompress = false;
    command_line cmdl;
    if(cmdl.size() < 3)
      return false;
    for(command_line::const_iterator cmd = cmdl.cbegin()+1; cmd != cmdl.cend(); ++cmd){
      if(!wcscmp(*cmd, L"-d"))
        decompress = true;
      else if(infile.empty())
        infile = *cmd;
      else if(outfile.empty())
        outfile = *cmd;
    }
    return !infile.empty() && !outfile.empty();
  }
};

int app::proceed()
{
  header();

  if(!parse_args()){
    usage();
    return 2;
  }

  rtl::relative_name rel(infile);
  //object_attributes oa(rel);
  //console::write<wchar_t>(oa.ObjectName->data(), oa.ObjectName->length());
  file in((rel));
  if(!in){
    console::write("can't open infile\n");
    return 3;
  }
  file out(rtl::relative_name(outfile), file::supersede, file::generic_write);
  if(!out){
    console::write("can't open outfile\n");
    return 4;
  }

  raw_data indata = in.get_data(), outdata;
  outdata.resize(indata.size()*(decompress ? 5 : 1));

  ntstatus st;
  uint32_t buf_cb;
  if(!decompress){
    uint32_t chunk_cb;
    st = RtlGetCompressionWorkSpaceSize(compression::default_format, &buf_cb, &chunk_cb);
    if(success(st)){
      raw_data tmp(buf_cb);
      st = RtlCompressBuffer(compression::default_format, indata.cbegin(), indata.size(), 
        outdata.begin(), outdata.size(), chunk_cb, &buf_cb, tmp.begin());
    }
  }else{
    st = RtlDecompressBuffer(compression::default_format, outdata.begin(), outdata.size(),
      indata.cbegin(), indata.size(), &buf_cb);
  }
  if(success(st))
    outdata.resize(buf_cb);

  out.write(outdata.cbegin(), outdata.size());

  char buf[128];
  size_t i = indata.size(), o = outdata.size();
  size_t l = _snprintf(buf, sizeof(buf)-1, "\n done. %d -> %d (%d%%)\n", i, o, decompress ? (i*100/o) : (o*100/i));
  console::write<char>(buf, l);
  return 0;
}

int consoleapp::main()
{
  app app_;
  return app_.proceed();
}
