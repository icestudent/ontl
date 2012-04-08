#ifndef NTL__STLX_TR2_FILES
# error internal header
#endif

#include "../quoted.hxx"

namespace std
{
  namespace tr2
  {
    namespace files
    {
      struct path::iterator:
        std::iterator<bidirectional_iterator_tag, const path>
      {
        iterator()
          :path(), pos(), element()
        {}

#ifdef NTL_CXX_RV
        iterator(iterator&& r)
          :name(move(r.name)), path(r.path), pos(r.pos), element(r.element)
        {
          r.path = nullptr;
          r.pos = 0;
        }
        iterator& operator=(iterator&& r)
        {
          path = move(r.path);
          pos = r.pos;
          element = r.element;
          r.pos = 0;
          return *this;
        }
#endif

        static_assert((is_same<const value_type&, reference>::value), "something wrong with iterator traits");

        reference operator*() const { return name;  }
        pointer operator->()  const { return &name; }

        inline iterator& operator++();
        inline iterator& operator--();

        iterator  operator++(int)
        {
          iterator tmp(*this);
          operator++();
          return tmp;
        }
        iterator  operator--(int)
        {
          iterator tmp(*this);
          operator--();
          return tmp;
        }

        friend bool operator==(const iterator& x, const iterator& y) { return x.path == y.path && x.pos == y.pos; }
        friend bool operator!=(const iterator& x, const iterator& y) { return !(x == y); }

      protected:
        friend class path;
        //friend iterator basic_path::begin() const;
        //friend iterator basic_path::end() const;

        iterator(const string_type& p, bool start)
          :path(&p), pos(0), element(begin)
        {
          if(!start)
            pos = value_type::npos, element = end;
          else
            operator++();
        }

        enum elements {
          begin, rootname, rootdir, filename, dot, end
        };

        static bool is_unc(reference p) { return p.s.size() > 2 && p.s[0] == backslash && p.s[1] == backslash; }
        static bool has_drive(reference p) { return p.s.size() >= 2 && p.s[1] == colonval; }
      private:
        path name;
        const string_type* path;
        pos_type pos;
        elements element;
      };

      path::iterator path::begin() const { return iterator(s, true ); }
      path::iterator path::end()   const { return iterator(s, false); }
      path::const_iterator path::cbegin() const { return const_iterator(s, true ); }
      path::const_iterator path::cend()   const { return const_iterator(s, false); }



      template <class charT, class traits>
      inline std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& os, const path& p)
      {
        return os << quoted(p.string<std::basic_string<charT,traits> >(), static_cast<charT>('&'));
      }

      template <class charT, class traits>
      inline std::basic_istream<charT, traits>& operator>>(std::basic_istream<charT, traits>& is, path& p)
      {
        std::basic_string<charT,traits> str;
        is >> quoted(str, '&');
        if(is)
          p = str;
        else
          p.clear();
        return is;
      }

      const path::codecvt_type& path::codecvt()
      {
        return std::use_facet<path::codecvt_type>(std::locale::classic());
      }

      bool path::is_absolute() const
      {
        return has_root_name() && has_root_directory();
      }

      bool path::has_unc_name() const
      {
        return s.size() > 2 && s[0] == backslash && s[1] == backslash;
      }
      bool path::has_root_name() const
      {
        return has_unc_name() || has_drive();
      }
      bool path::has_drive() const
      {
        return s.size() >= 2 && s[1] == colonval;
      }
      bool path::has_root_directory() const
      {
        const pos_type pos = s.find(backslash, has_unc_name() ? 2 : 0);
        return pos != npos;
      }
      
      bool path::has_root_path() const
      {
        return has_root_name() || (!s.empty() && s[0] == backslash);
      }
      
      bool path::has_relative_path() const
      {
        return !relative_path().empty();
      }

      bool path::has_filename() const
      {
        return !filename().empty();
      }

      bool path::has_stem() const
      {
        return !stem().empty();
      }

      bool path::has_extension() const
      {
        return !extension().empty();
      }

      bool path::has_parent_path() const
      {
        return !parent_path().empty();
      }

      path::pos_type path::filename_pos() const
      {
        if(s.empty())
          return string::npos;

        // '//C:/path/to/file.txt
        pos_type slashpos = s.rfind(backslash);
        if(slashpos == npos) // path is filename
          return 0;
        else if(slashpos == static_cast<pos_type>(s.size()-1))  // path ends with slash
          return npos;
        return slashpos + 1;
      }

      path path::root_name() const
      {
        if(!has_root_name())
          return string_type();

        // '//unc[/path]
        const pos_type pos = s.find(backslash, has_unc_name() ? 2 : 0);
        return s.substr(0, pos);
      }

      path path::root_directory() const
      {
        // root dir always '/'?
        return has_root_directory() ? string_type(1, backslash) : string_type();
      }

      path path::root_path() const
      {
        // if '/root[...]' return '/'
        if(!has_root_name() && has_root_path())
          return string_type(1, backslash);

        // root name with trailing slash, if any
        path root = root_name();
        if(!root.empty() && s.size() > root.s.size())
          root.s += backslash; // += root_directory()
        return move(root);
      }

      path path::relative_path() const
      {
        // right part from root path if any
        if(s.empty())
          return path();

        pos_type from = root_path().string().size();
        return s.substr(from);
      }

      path path::parent_path() const
      {
        // path - filename
        if(s.empty())
          return path();

        pos_type fpos = filename_pos();
        return s.substr(0, fpos == 0 ? npos : fpos-1);
      }

      path path::filename() const
      {
        // filename
        if(s.empty())
          return string_type();

        pos_type pos = filename_pos();
        if(pos == npos)
          return string_type();
        else if(pos == 0 && s.size() <= 2 && s[0] == dotval) // '.' or '..'
          return string_type();
        else
          return s.substr(pos);
      }

      path path::stem() const
      {
        // filename without extension, if any
        path fname = filename();
        pos_type dotpos = fname.s.rfind(dotval);
        return dotpos == npos ? move(fname) : fname.s.substr(0, dotpos);
      }

      path path::extension() const
      {
        // filename's extension, if any
        path fname = filename();
        pos_type dotpos = fname.s.rfind(dotval);
        return dotpos == npos ? path() : fname.s.substr(dotpos+1, npos);
      }

      path path::unc_name() const
      {
        return has_unc_name() ? root_name() : string_type();
      }

      void path::to_native(path::string_type& s)
      {
        for(path::string_type::iterator c = s.begin(), e = s.end(); c != e; ++c){
          if(*c == '/')
            *c = '\\';
        }
      }
      void path::to_generic(path::string_type& s)
      {
        for(path::string_type::iterator c = s.begin(), e = s.end(); c != e; ++c){
          if(*c == '\\')
            *c = '/';
        }
      }

      const std::string path::string() const
      {
        const path p = wstring();
        wstring_convert<codecvt_type> conv;
        return conv.to_bytes(p.s);
      }
      const std::string path::generic_string() const
      {
        const path p = generic_wstring();
        wstring_convert<codecvt_type> conv;
        return conv.to_bytes(p.s);
      }

      template<> inline std::string    path::string<std::string>(const codecvt_type& cvt)  const { return string(); }
      template<> inline std::wstring   path::string<std::wstring>(const codecvt_type& cvt) const { return wstring(); }
      template<> inline std::u16string path::string<std::u16string>(const codecvt_type& cvt) const { return u16string(); }
      template<> inline std::u32string path::string<std::u32string>(const codecvt_type& cvt) const { return u32string(); }

      template<> inline std::string    path::generic_string<std::string>(const codecvt_type& cvt)  const { return generic_string(); }
      template<> inline std::wstring   path::generic_string<std::wstring>(const codecvt_type& cvt) const { return generic_wstring(); }
      template<> inline std::u16string path::generic_string<std::u16string>(const codecvt_type& cvt) const { return generic_u16string(); }
      template<> inline std::u32string path::generic_string<std::u32string>(const codecvt_type& cvt) const { return generic_u32string(); }


      path::iterator& path::iterator::operator ++()
      {
        // root name, root directory, elements, dot
        assert(element != end && "out of range");
        if(element == end)
          return *this;

        value_type& p = *path;
        const size_t size = path->size();
        if(pos == npos || pos == static_cast<pos_type>(size)){
          element = end;
          pos = npos;
          name.clear();
          return *this;
        }

        // skip leading slashes
        if(!pos){
          while(pos < static_cast<pos_type>(size) && p.s[pos] == backslash)
            pos++;
        }

        pos_type cur = pos;
        pos = p.s.find(backslash, cur);
        if(pos == npos)
          pos = size;
        name = p.s.substr(cur, pos-cur);
        if(pos != static_cast<pos_type>(size))
          pos++;
      #ifdef NTL_DEBUG
        name.c_str();
      #endif
        return *this;
      }
      path::iterator& path::iterator::operator --()
      {
        assert(element != begin && "out of range");
        return *this;
      }

      path& path::append(const path::value_type *p, size_t size)
      {
        if(!p || size == 0 || !*p)
          return *this;
        s.reserve(s.capacity()+size);

        // check Native subsystem name '\??\'
        const bool native = s.empty() && size > 4 && p[0] == backslash && p[1] == qmark && p[2] == qmark && p[3] == backslash;
        bool unc = false;
        if(native){
          p += 4, size -= 4;
          // check the UNC name 'UNC\'
          unc = size > 4 && p[0] == 'U' && p[1] == 'N' && p[2] == 'C' && p[3] == backslash;
          if(unc){
            p += 4, size -= 4;
            // prepend the portable UNC prefix '//'
            s += backslash, s += backslash;
          }
          if(!size)
            return *this;
        }

        // explicit native sequence (posix?) '//:'
        // NOTE: can this sequence be inside path?
        // All basic_path string or sequence arguments that describe a path shall accept the portable pathname format, 
        // and shall accept the native format if explicitly identified by a native format escape sequence prefix of slash slash colon ('//:').
        const bool pnative = size > 3 && p[0] == backslash && p[1] == backslash && p[2] == colonval;
        if(pnative){
          p += 3, size -= 3;
          if(!size)
            return *this;
        }

        // check trailing separator
        if(!s.empty() && !is_sep(*p) && s[s.size()-1] != backslash)
          s += backslash;

        // NOTE: should be it?
        //if(pnative){
        //  s.append(p,size);
        //  return *this;
        //}

        // append
        while(size--){
          s += *p == slashval ? backslash : *p;
          p++;
        }
      #ifdef NTL_DEBUG
        s.c_str();
      #endif
        return *this;
      }

      bool path::to_absolute(string_type& s)
      {
        if(s.empty())
          return false;
        ntl::nt::rtl::relative_name rel(s.c_str());
        if(rel)
          s.assign(rel.path.data(), rel.path.length());
        return rel;
      }

      path& path::replace_extension(const path& new_ext)
      {
        const path ext = extension();
        if(!ext.empty())
          s.erase(s.size() - ext.s.size());
        if(!new_ext.empty() && new_ext.s[0] != dotval)
          s += dotval;
        s += new_ext.s;
        return *this;
      }

      void swap(path& lhs, path& rhs)
      {
        lhs.swap(rhs);
      }

      inline bool operator!=(const path& lhs, const path& rhs)
      {
        return !(lhs == rhs);
      }

      path operator/ (const path& lhs, const path& rhs)
      {
        path tmp(lhs);
        tmp /= rhs;
        return std::move(tmp);
      }

    }
  }
}
