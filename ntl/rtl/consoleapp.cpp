#include "../consoleapp.hxx"

extern "C" int main(int, const char* []); // The C Standard allows a command line modifications, but NTL do not.

int ntl::consoleapp::main()
{
  consoleapp::command_line cmdl;
  return ::main(cmdl.size(), cmdl.begin());
}
