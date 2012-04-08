/**\file*********************************************************************
 *                                                                     \brief
 *  Filesystem V3 library for TR2
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_FILES
#error internal header
#endif

#include "../../stack.hxx"

namespace std
{
  namespace tr2
  {
    namespace files
    {
      /**
       *  \addtogroup tr2
       *  @{
       *  \addtogroup tr2_files
       *  @{
       *  \defgroup tr2_files_dir Filesystem v3 directories support
       *  @{
       **/

      //////////////////////////////////////////////////////////////////////////
      /**
       *	@brief Class template directory_entry
       *  @details A directory_entry object stores a basic_path object, a file_status object for non-symbolic link status, 
       *  and a file_status object for symbolic link status. 
       *  The file_status objects act as value caches.
       *  @note Because status() on a pathname may be a very expensive operation, some operating systems provide status information 
       *  as a byproduct of directory iteration. 
       *  Caching such status information can result is significant time savings. Cached and non-cached results may differ in the presence of race conditions.
       **/
      
      class directory_entry
      {
      public:
        typedef files::path path_type;
        typedef path_type::string_type string_type;

        ///\name constructors
        directory_entry()
        {}

        explicit directory_entry(const path_type& p, file_status st = file_status(), file_status symlink_st = file_status())
          :p(p),st(st), lst(symlink_st)
        {}

      #ifdef NTL_CXX_RV
        explicit directory_entry(path_type&& p, file_status st = file_status(), file_status symlink_st = file_status())
          :p(forward<path_type>(p)),st(st), lst(symlink_st)
        {}
        directory_entry(directory_entry&& r)
        {
          swap(r);
        }
        directory_entry& operator=(directory_entry&& r)
        {
          directory_entry(r).swap(*this);
        }

        void assign(path_type&& p, file_status st = file_status(), file_status symlink_st = file_status())
        {
          this->p = move(p), this->st = st, lst = symlink_st;
        }

      #endif
        void swap(directory_entry& r)
        {
          p.swap(r.p);
          std::swap(st,r.st);
          std::swap(lst,r.lst);
        }

        friend void swap(directory_entry& x, directory_entry& y) { x.swap(y); }

        ///\name modifiers
        /** Assigns the stored path and statuses with the new values */
        void assign(const path_type& p, file_status st = file_status(), file_status symlink_st = file_status())
        {
          this->p = p, this->st = st, lst = symlink_st;
        }

        /** Replaces the base filename and statuses of the stored path with the new values */
        void replace_filename(const string_type& s, file_status st = file_status(), file_status symlink_st = file_status())
        {
          p = p.parent_path() / s;
          this->st = st, lst = symlink_st;
        }

        ///\name observers
        /** Returns the stored path */
        const path_type& path() const     { return p; }
        /** Returns the stored path */
        operator const path_type&() const { return p; }

        /** Determines filesystem::status() of path object or returns cached value */
        file_status status(error_code& ec = throws()) const
        {
          if(!status_known(st)){
            if(status_known(lst) && !is_symlink(lst))
              st = lst;
            else
              st = files::status(p, ec);
          }else if(&ec != &throws())
            ec.clear();
          return st;
        }

        /** Determines symlink status of path object or returns cached value. \see filesystem::symlink_status() */
        file_status symlink_status(error_code& ec = throws()) const
        {
          if(!status_known(lst))
            lst = files::symlink_status(p, ec);
          else if(&ec != &throws())
            ec.clear();
          return lst;
        }

        ///\name comparisons
        bool operator<(const directory_entry& rhs);
        bool operator==(const directory_entry& rhs) { return st == rhs.st && lst == rhs.lst && p == rhs.p; }
        bool operator!=(const directory_entry& rhs) { return !(*this == rhs); }
        bool operator>(const directory_entry& rhs);
        bool operator<=(const directory_entry& rhs);
        bool operator>=(const directory_entry& rhs);
        ///\}

      private:
        path_type            p;
        mutable file_status  st;
        mutable file_status  lst;
      };


      //////////////////////////////////////////////////////////////////////////
      /**
       *	@brief Class template directory_iterator
       *  @details directory_iterator satisfies the requirements of an input iterator (C++ Std, 24.1.1, Input iterators [lib.input.iterators]).
       *
       *  A directory_iterator reads successive elements from the directory for which it was constructed, as if by calling POSIX readdir_r(). 
       *  After a directory_iterator is constructed, and every time <tt>operator++</tt> is called, it reads and stores a value of directory_entry 
       *  and possibly stores associated status values. <tt>operator++</tt> is not equality preserving; that is, <tt>i == j</tt> does not imply that <tt>++i == ++j</tt>.
       *
       *  @note The practical consequence of not preserving equality is that directory iterators can be used only for single-pass algorithms.
       *
       *  If the end of the directory elements is reached, the iterator becomes equal to the end iterator value. 
       *  The constructor <tt>directory_iterator()</tt> with no arguments always constructs an end iterator object, which is the only legitimate iterator 
       *  to be used for the end condition. The result of <tt>operator*</tt> on an end iterator is not defined. 
       *  For any other iterator value a <tt>const directory_entry&</tt> is returned. The result of <tt>operator-></tt> on an end iterator is not defined. 
       *  For any other iterator value a <tt>const directory_entry*</tt> is returned.
       *
       *  Two end iterators are always equal. An end iterator is not equal to a non-end iterator.
       *
       *  The result of calling the path() member of the directory_entry object obtained by dereferencing a directory_iterator is a reference 
       *  to a basic_path object composed of the directory argument from which the iterator was constructed with filename of the directory entry appended as if by <tt>operator/=</tt>.
       **/
      class directory_iterator:
        public iterator<input_iterator_tag, directory_entry>
      {
        typedef const ntl::nt::file_directory_information  fdi_t;
      public:
        typedef files::path path_type;

        ///\name Constructors
        directory_iterator() __ntl_nothrow
          :di(), base(), memsize()
        {}
        explicit directory_iterator(const files::path& dp) __ntl_nothrow
          :dp(dp), di(), base(), memsize()
        {
          error_code ec;
          ReadDirectory(dp, ec);
        }
        directory_iterator(const files::path& dp, error_code& ec) __ntl_nothrow
          :dp(dp), di(), base(), memsize()
        {
          error_code e;
          ReadDirectory(dp, e);
          if(&ec != &throws())
            ec = e;
        }
        directory_iterator(const directory_iterator& r) __ntl_nothrow
          :dp(r.dp), di(r.di), base(r.base), bdi(r.bdi), memsize(r.memsize)
        {
        }
        directory_iterator& operator=(const directory_iterator& r) __ntl_nothrow
        {
          dp = r.dp,
          base = r.base,
          di = r.di,
          bdi = r.bdi,
          memsize = r.memsize;
          return *this;
        }
        ~directory_iterator() __ntl_nothrow
        {}
#ifdef NTL_CXX_RV
        explicit directory_iterator(files::path&& dp) __ntl_nothrow
          :dp(forward<files::path>(dp)), di(), base(), memsize()
        {
          error_code ec;
          ReadDirectory(dp, ec);
        }
        directory_iterator(files::path&& dp, error_code& ec) __ntl_nothrow
          :dp(forward<files::path>(dp)), di(), base(), memsize()
        {
          error_code e;
          ReadDirectory(dp, e);
          if(&ec != &throws())
            ec = e;
        }
        directory_iterator(directory_iterator&& r) __ntl_nothrow
          :di()
        {
          swap(r);
        }
        directory_iterator& operator=(directory_iterator&& r) __ntl_nothrow
        {
          dp = move(r.dp),
            base = move(r.base),
            di = r.di,
            bdi = r.bdi,
            memsize = r.memsize;
          r.di = nullptr;
          return *this;
        }
#endif
        void swap(directory_iterator& r)
        {
          dp.swap(r.dp);
          base.swap(r.base);
          std::swap(di,r.di);
          bdi.swap(r.bdi);
          std::swap(memsize,r.memsize);
        }



        ///\name Input iterator operations
        const directory_entry& operator*()  const { assert(di != nullptr); return bdi;  }
        const directory_entry* operator->() const { assert(di != nullptr); return &bdi; }

        directory_iterator& operator++()
        {
          assert(di != nullptr);
          di = step(di);
          sync();
          return *this;
        }
        directory_iterator operator++(int)
        {
          directory_iterator tmp(*this);
          operator++();
          return move(tmp);
        }

        ///\name Comparsions
        friend inline bool operator== (const directory_iterator& x, const directory_iterator& y) { return x.di == y.di; }
        friend inline bool operator!= (const directory_iterator& x, const directory_iterator& y) { return !(x == y); }
        ///\}


      protected:
        bool ReadDirectory(const files::path& dp, error_code& ec) __ntl_nothrow
        {
          ec.clear();
          if(dp.empty())
            return false;

          using namespace ntl::nt;
          file_handler f;
          ntstatus st = f.open(const_unicode_string(native(dp)), file::generic_read, file::share_valid_flags, 
            file::directory_file|file::open_for_backup_intent|file::synchronous_io_nonalert);
          if(success(st)){
            io_status_block iosb;
            char* buf = nullptr;
          #ifdef NTL_SUBSYSTEM_KM
            static const uint32_t multiplier = 2;
            memsize = (sizeof(file_directory_information)+32)*16; // ~32 files by default (~3KB), multiplies by 2
          #else
            static const uint32_t multiplier = 4;
            memsize = (sizeof(file_directory_information)+32)*64; // 256 files by default (26KB), multiplies by 4
          #endif
            do{
              if(buf)
                delete[] buf;
              buf = new (nothrow) char[ memsize *= multiplier ];
              if(!buf){
                ec = make_error_code(status::insufficient_resources);
                return false;
              }
              st = ZwQueryDirectoryFile(f.get(), nullptr, nullptr, nullptr, &iosb, buf, memsize, FileDirectoryInformation, false, nullptr, true);
              if(success(st)){
                file_directory_information tmp;
                io_status_block iosb;
                st = ZwQueryDirectoryFile(f.get(), nullptr, nullptr, nullptr, &iosb, &tmp, sizeof(tmp), FileDirectoryInformation, false, nullptr, false);
                if(st != status::no_more_files)
                  st = status::buffer_overflow;
              }
            } while(st == status::buffer_overflow);

            memsize = static_cast<uint32_t>(iosb.Information);
            base.reset(di = reinterpret_cast<fdi_t*>(buf));
          }
          if(status::is_error(st))
            ec = make_error_code(st);
          else
            sync();
          return !ec;
        }
        #if 0
        void copy(const directory_iterator& bdi)
        {
          if(!bdi.base)
            return;

          memsize = bdi.memsize;
          di = bdi.di;
          
          //difference_type idx = 0;
          //for(fdi_t p = bdi.base.get(); p != bdi.di; p = step(p), idx++);

          //base = reinterpret_cast<fdi_t>( new (nothrow) char[memsize = bdi.memsize] );
          //if(!base)
          //  return;

          //memcpy((void*)base, bdi.base, memsize);

          //for(difference_type n = 0; n < idx; n++)
          //  di = step(di);
        }
        void free() __ntl_nothrow
        {
          if(base){
            delete[] reinterpret_cast<const char*>(base);
            base = di = nullptr;
            memsize = 0;
          }
        }
        #endif
        static fdi_t* step(fdi_t* p) __ntl_nothrow
        {
          return p->NextEntryOffset ? reinterpret_cast<fdi_t*>( uintptr_t(p) + p->NextEntryOffset ) : nullptr;
        }

        void sync()
        {
          // skip the '.' and '..'
          while(di && 
                (
                  (di->FileNameLength == 2 && di->FileName[0] == '.') || 
                  (di->FileNameLength == 4 && di->FileName[0] == '.' && di->FileName[1] == '.')
                )
               )
          {
            di = step(di);
          }
          if(!di){
            base.reset();
            bdi.assign(files::path());
            return;
          }
          using ntl::nt::file_attribute;
          file_type ft = regular_file;
          if(di->FileAttributes & file_attribute::directory)
            ft = directory_file;
          else if(di->FileAttributes & file_attribute::reparse_point)
            ft = symlink_file;
          else if(di->FileAttributes & file_attribute::device)
            ft = device_file;
          const path_type p( ntl::nt::const_unicode_string(di->FileName, di->FileNameLength/sizeof(wchar_t)) );
          bdi.assign(dp / p, file_status(ft));
          //bdi.assign(files::path::traits_type::to_internal(dp, files::path::external_string_type(di->FileName, di->FileNameLength/sizeof(wchar_t))), file_status(ft));
        }

      private:
        shared_ptr<fdi_t> base;
        fdi_t* di;
        uint32_t memsize;
        value_type bdi;
        files::path dp;
      };

      inline void swap(directory_iterator& x, directory_iterator& y) { x.swap(y); }
      //////////////////////////////////////////////////////////////////////////
      /**
       *	@brief Class template recursive_directory_iterator
       *  @details The behavior of a recursive_directory_iterator is the same as a directory_iterator unless otherwise specified.
       *
       *  - When an iterator is constructed, \c level is set to 0;
       *  - When an iterator it is incremented, if <tt>it->is_directory()</tt> is \c true and no_push() had not been called subsequent to the 
       *  most recent increment operation (or construction, if no increment has occurred), then  \c level is incremented, the directory is visited, 
       *  and its contents recursively iterated over.
       *  - When an iterator reaches the end of the directory currently being iterated over, or when pop() is called, \c level is decremented, 
       *  and iteration continues with the parent directory, until the directory specified in the constructor argument is reached.
       *  - level() returns \c level.
       *  - level(), pop(), and no_push() all require that the iterator not be the end iterator.
       *  .
       *  @note One of the uses of no_push() is to prevent unwanted recursion into symlinked directories. This may be necessary to prevent loops on some operating systems.
       **/
      class recursive_directory_iterator:
        public iterator<input_iterator_tag, directory_entry>
      {
      public:
        typedef files::path path_type;

        ///\name Constructors
        recursive_directory_iterator()
          :recursive(false)
        {}
        explicit recursive_directory_iterator(const files::path& dp, error_code& ec = throws())
          :recursive(true)
        {
          error_code e;
          directory_iterator it(dp, e);
          if(e){
            if(&ec != &throws())
              ec = e;
          }else{
            depth.push(move(it));
          }
        }
        recursive_directory_iterator(const recursive_directory_iterator& rhs)
          :recursive(rhs.recursive), depth(rhs.depth)
        {}
        recursive_directory_iterator& operator=(const recursive_directory_iterator& rhs)
        {
          recursive = rhs.recursive;
          depth = rhs.depth;
        }
        ~recursive_directory_iterator()
        {}
#ifdef NTL_CXX_RV
        explicit recursive_directory_iterator(files::path&& dp, error_code& ec = throws())
          :recursive(true)
        {
          error_code e;
          directory_iterator it(forward<files::path>(dp), e);
          if(e){
            if(&ec != &throws())
              ec = e;
          }else{
            depth.push(move(it));
          }
        }
        recursive_directory_iterator(recursive_directory_iterator&& rhs)
          :recursive(rhs.recursive), depth(move(rhs.depth))
        {
          rhs.recursive = false; // reset
        }
#endif
        void swap(recursive_directory_iterator& r)
        {
          using std::swap;
          swap(depth,r.depth);
          swap(recursive,r.recursive);
        }
        friend void swap(recursive_directory_iterator& x, recursive_directory_iterator& y) { x.swap(y); }

        ///\name Observers

        /** Returns the depth level of recursion */
        int level() const
        {
          // returns min(size - 1, 0)
          return min(static_cast<int>(depth.size())-1, 0);
        }

        ///\name Modifiers
        /** Goes back to the parent path  */
        void pop()     { depth.pop(); }
        /** Stops the lowering when directory found */
        void no_push() { recursive = false; }

        ///\name Input iterator operations
        const directory_entry& operator*()  const { return * depth.top(); }
        const directory_entry* operator->() const { return &*depth.top(); }

        recursive_directory_iterator& operator++()
        {
          assert(!depth.empty());

          const directory_iterator end = directory_iterator();

          // check the current object
          {
            directory_iterator& it = depth.top();
            if(recursive && is_directory(it->status())){
              // go to the child, if directory
              directory_iterator dit(it->path());
              if(dit != end){
                // current directory isn't empty
                depth.push(move(dit));
                return *this;
              }
            }
            // if it isn't directory, go to other sibling
            ++it;
          }
          
          directory_iterator* pit = &depth.top();
          while(*pit == end){
            depth.pop();
            if(depth.empty())
              break;
            pit = &depth.top();
            if(*pit != end)
              ++*pit;
          }
          if(depth.empty())
            recursive = false; // reset to default
          return *this;
        }
        recursive_directory_iterator operator++(int)
        {
          recursive_directory_iterator tmp(*this);
          operator++();
          return std::move(tmp);
        }

        ///\name Comparsions
        friend inline bool operator== (const recursive_directory_iterator& x, const recursive_directory_iterator& y) 
        { 
          bool re = x.recursive == y.recursive && x.depth.size() == y.depth.size();
          if(re && !x.depth.empty())
            re = &x.depth.top() == &y.depth.top(); // compare pointers to top()
          return re;
        }
        friend inline bool operator!= (const recursive_directory_iterator& x, const recursive_directory_iterator& y) { return !(x == y); }
        ///\}
      private:
        stack<directory_iterator> depth;
        bool recursive;
      };
      //////////////////////////////////////////////////////////////////////////

      /** @} tr2_files_dir */
      /** @} tr2_files */
      /** @} tr2 */
    }
  }
}