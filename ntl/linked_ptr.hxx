/**\file*********************************************************************
 *                                                                     \brief
 *  Linked pointer  
 *
 ****************************************************************************
 */

#ifndef NTL__LINKED_PTR
#define NTL__LINKED_PTR
 
#include "linked_list.hxx"

namespace ntl {

template<typename T>
struct linked_ptr
{
    typedef linked_ptr  this_type;
    typedef T(this_type::*unspecified_bool_type)();

    /// construct from the raw pointer
    explicit this_type(T * ptr = 0)
    : ptr(ptr)
    {
      //links.link(&links, &links);
      links.prev = links.next = &links;
    }

    /// copyconstruct to link to the existing ptr list
    this_type(const this_type & other)
    {
      links.link(&other.links);
    }

    T * get() const { return ptr; }
    void set(T * ptr) { this->ptr = ptr; }

    /// unlink
    ~linked_ptr()
    { 
      /// unless this is not the last one
      //if ( links.prev != links.next )
        links.unlink(links.prev, links.next);
    }

    __forceinline
    long use_count() const
    {
      long c = 0;
      const_double_linked_iterator i = &links;
      do if ( containing_adaptor<this_type, double_linked, &this_type::links>(i)->ptr )
          ++c; 
      while ( --i != &links );
      return c;
    }

    bool unique() const
    {
      // optimize use_count() != 0
      return links.prev == links.next && ptr;
    }

    operator unspecified_bool_type() const
    { 
      // ptr != 0 forces a few bloat instructions
      return brute_cast<unspecified_bool_type>(ptr);
    }

  private:
            T *           ptr;
    mutable double_linked links;
};


}//namespace ntl {

#endif//#ifndef NTL__LINKED_PTR
