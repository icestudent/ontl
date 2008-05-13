/**\file*********************************************************************
 *                                                                     \brief
 *  Win32 Application support 
 *
 ****************************************************************************
 */

#ifndef NTL__WIN_WINAPP
#define NTL__WIN_WINAPP

#include "win/application.hxx"
#include "win/console.hxx"
#include "nt/new.hxx"

//#pragma comment(linker, "/subsystem:windows")
#pragma comment(linker, "/entry:ntl::_Winapp_entry")

#pragma comment(linker, "/nodefaultlib:libc")
#pragma comment(linker, "/nodefaultlib:libcmt")

namespace ntl {

/**\addtogroup  application ****** Application support library **************
 *@{*/


class winapp : public win::application<win::tchar_t>
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    //static //?
    __forceinline int main();

    typedef win::console console;

};


int _Winapp_entry()
{
  winapp app;
  return app.main();
}


/**@} application */


}//namespace ntl


#endif//#ifndef NTL__WIN_WINAPP
