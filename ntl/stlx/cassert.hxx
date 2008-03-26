/**\file*********************************************************************
 *                                                                     \brief
 *  Assertions [lib.assertions 19.2]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_CASSERT
#define NTL__STLX_CASSERT

//namespace std {

#ifdef _DEBUG
#define NTL__DEBUG
#endif


#ifndef _BUGCHECK 
#ifdef _M_IX86
#	define _BUGCHECK(msg, line)      \
{                                 \
  static const char m[] = msg;    \
  __asm mov ecx, offset m         \
  __asm mov edx, line             \
  __asm _emit 0xCC  /* ICE BP */  \
}
#elif defined _M_X64
#	define _BUGCHECK(msg, line)	__debugbreak();
#else
# error unsupported CPU type
#endif// CPU type
#endif// #ifndef _BUGCHECK 


#ifdef NTL__DEBUG
#define _Assert(expr) \
  if ( !!(expr) ); else \
  _BUGCHECK("Assertion ("#expr") failed in "__FUNCSIG__" //"__FILE__,__LINE__);\
  ((void)0)
#else
#define _Assert(expr)
#endif


#undef assert

#ifdef NDEBUG
#define assert(expr) ((void)0)
#else
#define assert(expr) (not implemented)
#endif

//}//namespace std

#endif//#ifndef NTL__STLX_CASSERT
