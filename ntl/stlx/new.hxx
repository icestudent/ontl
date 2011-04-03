/**\file*********************************************************************
 *                                                                     \brief
 *  Dynamic memory management [18.6 support.dynamic]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_NEW
#define NTL__STLX_NEW
#pragma once

#ifndef NTL__STLX_CSTDDEF
#include "cstddef.hxx"
#endif

#include "stdexception.hxx"

/// calculates the new size of a reallocating heap block.
/// Win32 heaps have granularity of 8 and store 8 byte control structure
/// into allocated blocks. So the good sizes are: 8, 24, 56, ...
///\todo find the apropriate place for this
template<typename SizeType>
__forceinline
SizeType __ntl_grow_heap_block_size(const SizeType  size)
{
  return (((size + 8) * 2) & ~(8-1)) - 8;
}

namespace std {

  /**\addtogroup  lib_language_support *** 18 Language support library [language.support]
 *@{*/

/**\defgroup  lib_support_dynamic ****** 18.6 Dynamic memory management [support.dynamic]
 *@{*/

/**\defgroup  lib_alloc_errors ********* 18.6.2 Storage allocation errors [alloc.errors]
 *@{*/

/**
 *	@brief Class bad_alloc [18.6.2.1 bad.alloc]
 *  @details The class bad_alloc defines the type of objects thrown as exceptions by the implementation to report
 *  a failure to allocate storage.
 **/
class bad_alloc : public exception
{
  public:
    bad_alloc() __ntl_nothrow {}
    bad_alloc(const bad_alloc&) __ntl_nothrow {}
    bad_alloc& operator=(const bad_alloc&) __ntl_nothrow { return *this; }
    virtual ~bad_alloc() __ntl_nothrow {}
    virtual const char* what() const __ntl_nothrow { return "bad_alloc"; }
};

/**
 *	@brief Class bad_array_new_length [18.6.2.2 new.badlength]
 *  @details The class bad_array_new_length defines the type of objects thrown as exceptions by the implementation
 *  to report an attempt to allocate an array of size greater than an implementation-defined limit (5.3.4).
 **/
class bad_array_new_length:
  public bad_alloc
{
public:
  bad_array_new_length() __ntl_nothrow {}
  ~bad_array_new_length() __ntl_nothrow{}
  virtual const char* what() const __ntl_nothrow { return "bad_array_new_length"; }
};

struct  nothrow_t {};

#if defined(_MSC_VER)
extern __declspec(selectany) const nothrow_t nothrow = {};
#elif defined(__BCPLUSPLUS__)
__declspec(selectany) extern const nothrow_t nothrow;
#elif defined(__GNUC__)
 extern const nothrow_t nothrow;
#endif

/// Type new_handler [18.6.2.3 new.handler]
typedef void (*new_handler)();

/// set_new_handler [18.6.2.4 set.new.handler]
new_handler set_new_handler(new_handler new_p) __ntl_nothrow;

/// set_new_handler [18.6.2.5 get.new.handler]
new_handler get_new_handler() __ntl_nothrow;


/**@} lib_alloc_errors */
/**@} lib_support_dynamic */
/**@} lib_language_support */

}//namespace std

#ifdef _MSC_VER
#define operator  __cdecl operator
#else
//#define operator  operator
#endif

/**\addtogroup  lib_language_support
 *@{*/

/**\addtogroup  lib_support_dynamic
 *@{*/

// Storage allocation and deallocation [18.6.1 lib.new.delete]

///\name  Single-object forms [18.6.1.1 lib.new.delete.single]

void* operator new      (std::size_t size) __ntl_throws(std::bad_alloc);
void  operator delete   (void* ptr) __ntl_nothrow;

void* operator new      (std::size_t size, const std::nothrow_t&) __ntl_nothrow;
void  operator delete   (void* ptr, const std::nothrow_t&) __ntl_nothrow;

///\name  Array forms [18.6.1.2 lib.new.delete.array]

void* operator new[]    (std::size_t size) __ntl_throws(std::bad_alloc);
void  operator delete[] (void* ptr) __ntl_nothrow;

void* operator new[]    (std::size_t size, const std::nothrow_t&) __ntl_nothrow;
void  operator delete[] (void* ptr, const std::nothrow_t&) __ntl_nothrow;

///\name  Placement forms [18.6.1.3 lib.new.delete.placement]
///\note  Standard says nothing about inline nor static, but we're avoiding LNK2005

__forceinline
void* operator new      (std::size_t, void* ptr) __ntl_nothrow  { return ptr; }

__forceinline
void  operator delete   (void*, void*) __ntl_nothrow            {}

__forceinline
void* operator new[]    (std::size_t, void* ptr) __ntl_nothrow  { return ptr; }

__forceinline
void  operator delete[] (void*, void*) __ntl_nothrow            {}

#undef operator


///\name Variable-size structures support
struct varsize_tag {};

#if defined(_MSC_VER)
extern __declspec(selectany) const varsize_tag varsize = {};
#elif defined(__BCPLUSPLUS__)
__declspec(selectany) extern const varsize_tag varsize;
#elif defined(__GNUC__)
 extern const varsize_tag varsize;
#endif

__forceinline
void* operator new(std::size_t size, const varsize_tag&, std::size_t aux_size)
{
  return ::operator new(size+aux_size);
}

__forceinline
void operator delete(void* ptr, const varsize_tag&, std::size_t)
{
  return ::operator delete(ptr);
}
///@}

/**@} lib_support_dynamic */
/**@} lib_language_support */

#endif//#ifndef NTL__STLX_NEW
