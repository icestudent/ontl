/**\file*********************************************************************
 *                                                                     \brief
 *  Windows SDK support
 *
 ****************************************************************************
 */
#ifdef NTL__WIN_WINSDKAPP
#error This file must be included in project only once
#endif
#define NTL__WIN_WINSDKAPP

#pragma once

#include "win/sdk.hxx"
#include "rtl/crt.cpp"
#include "nt/new.hxx"
#include "cstdlib"
#include <tchar.h>

#include "win/application.hxx"

int wWinMainCRTStartup()
{
  int exitcode = 0;
  {
    using namespace ntl;
    crt_initializer __crt;

    typedef win::application<win::tchar_t> application;

    auto* hInstance = pe::image::this_module();

    auto* cmdl = application::command_line::get();

    auto sw = application::startupinfo::get();
    int show = sw.useshowwindow ? sw.wShowWindow : SW_NORMAL;


    // int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR    lpCmdLine, int       nCmdShow)
    exitcode = _tWinMain((HINSTANCE)hInstance, NULL, cmdl, show);
  }
  ExitProcess(exitcode);
  return exitcode;
}

int WinMainCRTStartup() { return wWinMainCRTStartup(); }
