/**\file*********************************************************************
 *                                                                     \brief
 *  files support
 *
 ****************************************************************************
 */

#ifndef NTL__FILE
#define NTL__FILE

#include "device_traits.hxx"
#include "handle.hxx"


namespace ntl {


template<class FileDevice, class traits = device_traits<FileDevice> >
class basic_file : public traits
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef typename traits::size_type            size_type;
    typedef typename traits::access_mask          access_mask;
    typedef typename traits::creation_disposition creation_disposition;
    typedef typename traits::share_mode           share_mode;
    typedef typename traits::creation_options     creation_options;
    typedef typename traits::attributes           attributes;

    inline basic_file() : f() {}

    operator const void*() { return f.operator const void*(); }

    template<typename objectT>
    explicit __forceinline
      basic_file(
        const objectT &             object, 
        const creation_disposition  cd              = traits::creation_disposition_default,
        const access_mask           desired_access  = traits::access_mask_default,
        const share_mode            share_access    = traits::share_mode_default, 
        const creation_options      co              = traits::creation_options_default,
        const attributes            attr            = traits::attribute_default
        ) throw()
    {
      f.create(object, cd, desired_access, share_access, co, attr);
    }

    template<typename objectT>
    bool
      create(
        const objectT &             object, 
        const creation_disposition  cd              = traits::creation_disposition_default,
        const access_mask           desired_access  = traits::access_mask_default,
        const share_mode            share_access    = traits::share_mode_default, 
        const creation_options      co              = traits::creation_options_default,
        const attributes            attr            = traits::attribute_default
        ) throw()
    {
      return success(f.create(object, cd, desired_access, share_access, co, attr));
    }

    /*
    bool
      operator()(
        ) throw()
    {
    }
    */

    bool read(void * out, const uint32_t out_size) throw()
    {
      return success(f.read(out, out_size));
    }

    bool write(const void * in, const uint32_t in_size) throw()
    {
      return success(f.write(in, in_size));
    }

    size_type size() const throw()
    {
      return f.size();
    }

    bool size(const size_type & new_size) throw()
    {
      return success(f.size(new_size));
    }

    bool erase()
    {
      return success(f.erase());
    }

    template<class NameType>
    bool rename(
      const NameType &  new_name,
      bool              replace_if_exists)
    {
      return success(f.rename(new_name, replace_if_exists));
    }

    __forceinline
    raw_data get_data() throw(std::bad_alloc)
    {
      const size_type & file_size = size();
      raw_data file_content;
      if ( ! (file_size & ~static_cast<size_type>(uint32_t(-1))) )
      {
        const uint32_t & data_size = static_cast<uint32_t>(file_size);
        file_content.resize(data_size);
        if ( ! read(file_content.begin(), data_size) )
          file_content.clear();
      }
      return file_content;
    }

    const FileDevice & handler() const
    { 
      return f;
    }

  ///////////////////////////////////////////////////////////////////////////
  private:

    FileDevice f;

}; // class basic_file


}//namespace ntl

#endif//#ifndef NTL__FILE
