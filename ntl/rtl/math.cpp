/**
 *	Mathemathics
 **/

#include "../stlx/cmath.hxx"

#pragma region FPU intrinsics
extern "C" 
{
  double __cdecl acos(double x);
  double __cdecl asin(double x);
  double __cdecl atan(double x);
  double __cdecl atan2(double y, double x);
  double __cdecl cos(double x);
  double __cdecl sin(double x);
  double __cdecl tan(double x);
  double __cdecl cosh(double x);
  double __cdecl sinh(double x);
  double __cdecl tanh(double x);
  double __cdecl exp(double x);
  double __cdecl log(double x);
  double __cdecl log10(double x);
  double __cdecl pow(double x, double y);
  double __cdecl sqrt(double x);
  double __cdecl ceil(double x);
  double __cdecl floor(double x);
  double __cdecl fmod(double x, double y);
};


#ifdef _MSC_VER
#pragma intrinsic(atan2,floor,fmod,log,pow,sin,sqrt)
//#pragma intrinsic(acos,cos,asin,atan,exp,log10,sinh,tan,tanh)
# ifdef _M_X64
  #pragma intrinsic(ceil, tan, atan)
# else
double __cdecl ceil(double x) { return floor(x+1.0); }
# endif
#endif

#pragma endregion

#pragma region SSE intrinsics
union __declspec(intrin_type) alignas(8) __m64
{
  unsigned __int64    m64_u64;
  float               m64_f32[2];
  __int8              m64_i8[8];
  __int16             m64_i16[4];
  __int32             m64_i32[2];    
  __int64             m64_i64;
  unsigned __int8     m64_u8[8];
  unsigned __int16    m64_u16[4];
  unsigned __int32    m64_u32[2];
};

union __declspec(intrin_type) alignas(16) __m128 {
  float               m128_f32[4];
  unsigned __int64    m128_u64[2];
  __int8              m128_i8[16];
  __int16             m128_i16[8];
  __int32             m128_i32[4];
  __int64             m128_i64[2];
  unsigned __int8     m128_u8[16];
  unsigned __int16    m128_u16[8];
  unsigned __int32    m128_u32[4];
};

union __declspec(intrin_type) alignas(16) __m128i {
  __int8              m128i_i8[16];
  __int16             m128i_i16[8];
  __int32             m128i_i32[4];    
  __int64             m128i_i64[2];
  unsigned __int8     m128i_u8[16];
  unsigned __int16    m128i_u16[8];
  unsigned __int32    m128i_u32[4];
  unsigned __int64    m128i_u64[2];
};

struct __declspec(intrin_type) alignas(16) __m128d {
  double              m128d_f64[2];
};



extern "C"
{
  // single precision
  __m128 _mm_load_ss(float * p );
  __m128 _mm_set_ss(float w );
  __m128 _mm_move_ss( __m128 a , __m128 b );
  void _mm_store_ss(float * p , __m128 a );

  __m128 _mm_min_ss(__m128 a , __m128 b );
  __m128 _mm_max_ss(__m128 a , __m128 b );

  int _mm_comieq_ss(__m128 a,__m128 b );
  int _mm_comilt_ss(__m128 a,__m128 b );
  int _mm_comile_ss(__m128 a,__m128 b );
  int _mm_comigt_ss(__m128 a,__m128 b );
  int _mm_comige_ss(__m128 a,__m128 b );
  int _mm_comineq_ss(__m128 a,__m128 b );
  int _mm_ucomieq_ss(__m128 a,__m128 b );
  int _mm_ucomilt_ss(__m128 a,__m128 b );
  int _mm_ucomile_ss(__m128 a,__m128 b );
  int _mm_ucomigt_ss(__m128 a,__m128 b );
  int _mm_ucomige_ss(__m128 a,__m128 b );
  int _mm_ucomineq_ss(__m128 a,__m128 b );
  __m128 _mm_cmpunord_ss(__m128 a , __m128 b );


  // double precision
  __m128d _mm_load_sd (double *p);
  __m128d _mm_set_sd (double w);
  __m128d _mm_move_sd (__m128d a, __m128d b);
  void _mm_store_sd (double *p, __m128d a);

  __m128d _mm_max_sd(__m128d a, __m128d b);
  __m128d _mm_min_sd(__m128d a, __m128d b);

  int _mm_comieq_sd(__m128d a,__m128d b );
  int _mm_comilt_sd(__m128d a,__m128d b );
  int _mm_comile_sd(__m128d a,__m128d b );
  int _mm_comigt_sd(__m128d a,__m128d b );
  int _mm_comige_sd(__m128d a,__m128d b );
  int _mm_comineq_sd(__m128d a,__m128d b );
  int _mm_ucomieq_sd(__m128d a,__m128d b );
  int _mm_ucomilt_sd(__m128d a,__m128d b );
  int _mm_ucomile_sd(__m128d a,__m128d b );
  int _mm_ucomigt_sd(__m128d a,__m128d b );
  int _mm_ucomige_sd(__m128d a,__m128d b );
  int _mm_ucomineq_sd(__m128d a,__m128d b );
  __m128d _mm_cmpunord_sd(__m128d a , __m128d b );


#ifdef _MSC_VER
#pragma intrinsic(_mm_load_ss,_mm_set_ss,_mm_move_ss,_mm_store_ss,_mm_min_ss,_mm_max_ss)
#pragma intrinsic(_mm_load_sd,_mm_set_sd,_mm_move_sd,_mm_store_sd,_mm_max_sd,_mm_min_sd)
#endif
};

#pragma endregion

#pragma region Miscellaneous intrinsics
extern "C"
{
  unsigned char _bittest(long *a,long b);
  unsigned char _bittest64(__int64 *a,__int64 b);
#pragma intrinsic(_bittest)
# ifdef _M_X64
#pragma intrinsic(_bittest64)
# endif
};
#pragma endregion

#define float2int4(x) (*(uint32_t*)&x)
#define float2int8(x) (*(uint64_t*)&x)

namespace std
{
  namespace __ 
  {
    static const __m128  zerof = {0};
    static const __m128d zerod = {0};
    static const __m128i zeroi = {0};
  }

#define FPFUNC(func) \
  float func ## f(float x) { return static_cast<float>(::func(x)); } \
  float func(float x) { return static_cast<float>(::func(x)); } \
  double func(double x) { return ::func(x); }

  //FPFUNC(acos)
  //FPFUNC(asin)
  FPFUNC(ceil)
  //FPFUNC(cosh)
  FPFUNC(cos)
  //FPFUNC(exp)
  FPFUNC(floor)
  FPFUNC(log)
  //FPFUNC(log10)
  FPFUNC(sin)
  //FPFUNC(sinh)
  FPFUNC(sqrt)
  //FPFUNC(tanh)
  //FPFUNC(tan)
  //FPFUNC(atan)
#ifdef _M_X64
#endif
#undef FPFUNC

#define FPFUNC(func) \
  float func ## f(float x, float y) { return static_cast<float>(::func(x,y)); } \
  float func(float x, float y) { return static_cast<float>(::func(x,y)); } \
  double func(double x, double y) { return ::func(x,y); }

  //FPFUNC(atan2)
  FPFUNC(pow)
  //FPFUNC(fmod)
#undef FPFUNC

  double fmax(double x, double y)
  {
    _mm_store_sd(&x, _mm_max_sd(_mm_load_sd(&x),_mm_load_sd(&y)));
    return x;
  }

  float fmax(float x, float y)
  {
    _mm_store_ss(&x, _mm_max_ss(_mm_load_ss(&x),_mm_load_ss(&y)));
    return x;
  }

  double fmin(double x, double y)
  {
    _mm_store_sd(&x, _mm_min_sd(_mm_load_sd(&x),_mm_load_sd(&y)));
    return x;
  }

  float fmin(float x, float y)
  {
    _mm_store_ss(&x, _mm_min_ss(_mm_load_ss(&x),_mm_load_ss(&y)));
    return x;
  }


  /*
    00..51 mantissa (52 bits)
    52..62 exponent (11 bits)
        63 sign

    hexadecimal representation:
    00000000 00000000                   zero
    00000000 00000001 000FFFFF FFFFFFFF denormalized
    00100000 00000000 7FEFFFFF FFFFFFFF normalized
    7FF00000 00000000                   infinity
    7FF00000 00000001 7FF7FFFF FFFFFFFF SNaN
    7FF80000 00000000 7FFFFFFF FFFFFFFF QNaN
  */

  fpclass_t fpclassify(double d)
  {
    uint64_t x = float2int8(d);
    uint32_t* h = ((uint32_t*)&x + 1); // high dword
    const bool negative = (*h & 0x80000000) != 0;
    *h &= ~0x80000000;
    fpclass_t r;
    if(x == 0)
      r = fpclass::zero;
    else if(x < 0x0010000000000000)
      r = fpclass::denormal;
    else if(x < 0x7FF0000000000000)
      r = fpclass::normal;
    else if(x ==0x7FF0000000000000)
      r = fpclass::infinity;
    else if(x < 0x7FF8000000000000)
      r = fpclass::snan;
    else
      r = fpclass::qnan;
    if(negative)
      r = static_cast<fpclass_t>( r | fpclass::sign );
    return r;
  }
  bool isnan(double d)
  {
    uint64_t x = float2int8(d);
    x &= 0x7FFFFFFFFFFFFFFF;
    return x > 0x7FF0000000000000;
  }
  bool isfinite(double d)
  {
    uint64_t x = float2int8(d);
    x &= 0x7FFFFFFFFFFFFFFF;
    return x < 0x7FF0000000000000;
  }
  bool isinf(double d)
  {
    uint64_t x = float2int8(d);
    x &= 0x7FFFFFFFFFFFFFFF;
    return x == 0x7FF0000000000000; // infinite
  }
  bool isnormal(double d)
  {
    uint64_t x = float2int8(d);
    x &= 0x7FFFFFFFFFFFFFFF;
    return x >= 0x0010000000000000 && x < 0x7FF0000000000000; // normalized
  }
  bool signbit(double d)
  {
    uint32_t* x = ((uint32_t*)&d)+1;
    return (*x & 0x80000000) != 0;
  }

  /*
    00..22 mantissa (23 bits)
    23..30 exponent  (8 bits)
        31 sign

    hexadecimal representation:
             00000000 zero
    00000001 007FFFFF denormalized
    00800000 7F7FFFFF normalized
             7F800000 infinity
    7F800001 7FBFFFFF SNaN
    7FC00000 7FFFFFFF QNaN
  */
  fpclass_t fpclassify(float f)
  {
    uint32_t x = float2int4(f);
    const bool negative = (x & 0x80000000) != 0;
    x &= ~0x80000000;
    fpclass_t r;
    if(x == 0)
      r = fpclass::zero;
    else if(x <= 0x007FFFFF)
      r = fpclass::denormal;
    else if(x <= 0x7F7FFFFF)
      r = fpclass::normal;
    else if(x == 0x7F800000)
      r = fpclass::infinity;
    else if(x <= 0x7FBFFFFF)
      r = fpclass::snan;
    else
      r = fpclass::qnan;
    if(negative)
      r = static_cast<fpclass_t>( r | fpclass::sign );
    return r;
  }

  // NaN
  bool isnan(float f)
  {
    uint32_t x = float2int4(f);
    x &= 0x7FFFFFFF;
    return x > 0x7F800000;
  }
  // not infinite or NaN
  bool isfinite(float f)
  {
    uint32_t x = float2int4(f);
    x &= 0x7FFFFFFF;
    return x < 0x7F800000;
  }
  // +- infinite
  bool isinf(float f)
  {
    uint32_t x = float2int4(f);
    x &= 0x7FFFFFFF;
    return x == 0x7F800000;
  }

  // neither zero, subnormal, infinite, nor NaN
  bool isnormal(float f)
  {
    uint32_t x = float2int4(f);
    x &= 0x7FFFFFFF;
    return x >= 0x00800000 && x < 0x7F800000;
  }
  bool signbit(float f)
  {
    uint32_t x = float2int4(f);
    return (x & 0x80000000) != 0;
  }


  namespace __{
  bool isgreaterequal(float x, float y)
  {
    return _mm_ucomige_ss(_mm_load_ss(&x),_mm_load_ss(&y)) != 0;
  }
  bool islessequal(float x, float y)
  {
    return _mm_ucomile_ss(_mm_load_ss(&x),_mm_load_ss(&y)) != 0;
  }
  bool isgreater(float x, float y)
  {
    return _mm_ucomigt_ss(_mm_load_ss(&x),_mm_load_ss(&y)) != 0;
  }
  bool isless(float x, float y)
  {
    return _mm_ucomilt_ss(_mm_load_ss(&x),_mm_load_ss(&y)) != 0;
  }
  bool islessgreater(float x, float y)
  {
    return _mm_ucomineq_ss(_mm_load_ss(&x),_mm_load_ss(&y)) != 0;
  }
  bool isunordered(float x, float y)
  {
    // result = (isunord(x,y) cmp 0) == 0
    return _mm_ucomieq_ss(_mm_cmpunord_ss(_mm_load_ss(&x),_mm_load_ss(&y)), __::zerof) == 0;
  }


  bool isgreaterequal(double x, double y)
  {
    return _mm_ucomige_sd(_mm_load_sd(&x),_mm_load_sd(&y)) != 0;
  }
  bool islesdequal(double x, double y)
  {
    return _mm_ucomile_sd(_mm_load_sd(&x),_mm_load_sd(&y)) != 0;
  }
  bool isgreater(double x, double y)
  {
    return _mm_ucomigt_sd(_mm_load_sd(&x),_mm_load_sd(&y)) != 0;
  }
  bool islesd(double x, double y)
  {
    return _mm_ucomilt_sd(_mm_load_sd(&x),_mm_load_sd(&y)) != 0;
  }
  bool islesdgreater(double x, double y)
  {
    return _mm_ucomineq_sd(_mm_load_sd(&x),_mm_load_sd(&y)) != 0;
  }
  bool isunordered(double x, double y)
  {
    // result = (isunord(x,y) cmp 0) == 0
    return _mm_ucomieq_sd(_mm_cmpunord_sd(_mm_load_sd(&x),_mm_load_sd(&y)), __::zerod) == 0;
  }
  } // __


} // std
