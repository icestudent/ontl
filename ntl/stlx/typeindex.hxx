/**\file*********************************************************************
 *                                                                     \brief
 *  Type identification utilities [20.11 type.index]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TYPEINDEX
#define NTL__STLX_TYPEINDEX
#pragma once

#include "typeinfo.hxx"
#include "functional.hxx"

namespace std
{
  /**\addtogroup  lib_utilities *** 20 General utilities library [utilities]
   *@{
   **/
  /**\defgroup  lib_typeindex ***** 20.11 Class typeindex [type.index]
   *@{
   */

  /**
   *	@brief 20.11 Class type_index [type.index] (N2960)
   *  @details The class type_index provides a simple wrapper for type_info which can be used as an index type in
   *  associative containers and in unordered associative containers.
   **/
  class type_index
  {
  public:
    /** Constructs type_index object which holds the type_info object */
    type_index(const type_info& rhs)
      :target(&rhs)
    {}
    ///\name Comparsion operators
    bool operator==(const type_index& rhs) const  { return *target == *rhs.target; }
    bool operator!=(const type_index& rhs) const  { return *target != *rhs.target; }
    bool operator< (const type_index& rhs) const  { return target->before(*rhs.target); }
    bool operator<= (const type_index& rhs)const  { return !rhs.target->before(*target); }
    bool operator> (const type_index& rhs) const  { return rhs.target->before(*rhs.target); }
    bool operator>= (const type_index& rhs)const  { return !target->before(*rhs.target); }
    ///\}

    /** Returns the hash value of the stored type_info object */
    size_t hash_code() const __ntl_nothrow { return target->hash_code(); }
    /** Returns the type name of the stored type_info object */
    const char* name() const __ntl_nothrow { return target->name(); }
  private:
    const type_info* target;
  };

  /**
   *	@brief 20.11.4 Template specialization hash&lt;type_index&gt; [type.index.templ] (N2960)
   **/
  template<>
  struct hash<type_index>:
    unary_function<type_index, size_t>
  {
    size_t operator()(const type_index& index) const
    {
      return index.hash_code();
    }
  };

  /**@} lib_typeindex */
  /**@} lib_utilities */
}
#endif // NTL__STLX_TYPEINDEX
