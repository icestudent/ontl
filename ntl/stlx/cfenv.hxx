/**\file*********************************************************************
 *                                                                     \brief
 *  The floating-point environment [26.3 cfenv]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_CFENV
#define NTL__STLX_CFENV
#pragma once

namespace std
{
  /**\addtogroup  lib_numeric ************ 26 Numerics library [numerics]
   *@{
  /**\addtogroup  lib_cfenv   ************ 26.3 The floating-point environment [cfenv]
  *@{*/

  // macros
  #define FE_DIVBYZERO
  #define FE_INEXACT
  #define FE_INVALID
  #define FE_OVERFLOW
  #define FE_UNDERFLOW

  #define FE_DOWNWARD
  #define FE_TONEAREST
  #define FE_TOWARDZERO
  #define FE_UPWARD

  #define FE_DFL_ENV
  #define FE_ALL_EXCEPT

  // types
  typedef object type fenv_t;
  typedef integer type fexcept_t;

  // functions
  int feclearexcept(int except);
  int fegetexceptflag(fexcept_t *pflag, int except);
  int feraiseexcept(int except);
  int fesetexceptflag(const fexcept_t *pflag, int except);
  int fetestexcept(int except);
  int fegetround(void);
  int fesetround(int mode);
  int fegetenv(fenv_t *penv);
  int feholdexcept(fenv_t *penv);
  int fesetenv(const fenv_t *penv);
  int feupdateenv(const fenv_t *penv);

  /**@} lib_cfenv */
  /**@} lib_numeric */
}
#endif // NTL__STLX_CFENV
