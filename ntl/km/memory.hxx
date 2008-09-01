/**\file*********************************************************************
*                                                                     \brief
*  kernel-mode allocator
*
****************************************************************************
*/

#ifndef NTL__KM_MEMORY
#define NTL__KM_MEMORY

#include "new.hxx"

namespace ntl {
  namespace km {

    /**
     *	default kernel-mode allocator
     *  uses paged pool to memory allocates
     **/
    template<class T>
    class allocator: public std::allocator<T>
    {
    public:
    };

    /**
     *	nonpaged kernel-mode allocator
     *  uses nonpaged pool to memory allocates
     **/
    template<class T>
    class npallocator: public std::allocator<T>
    {
    public:

      __declspec(noalias) __declspec(restrict)
        __forceinline
        pointer allocate(size_type n, std::allocator<void>::const_pointer hint = 0)
        __ntl_nothrow
      {
        (hint);
        const pointer p = reinterpret_cast<T*>(::operator new(sizeof(T) * n, nonpaged));
        __assume(p);
        return p;
      }

      __declspec(noalias)
        __forceinline
        void deallocate(pointer p, size_type /* n */)
      {
        ::operator delete(p);//(address(*p));
      }

    };//class npallocator


  } // namespace km
} // namespace ntl

#endif // #ifndef NTL__KM_MEMORY