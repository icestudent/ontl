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


#if !defined _BUGCHECK && defined NTL__DEBUG && defined _M_IX86
#	define _BUGCHECK(msg, line)      \
{                                 \
  static const char m[] = msg;    \
  __asm mov ecx, offset m         \
  __asm mov edx, line             \
  __asm _emit 0xf1  /* ICE BP */  \
}
#else
#	define _BUGCHECK(msg, line)	\
	__debugbreak();
#endif


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
