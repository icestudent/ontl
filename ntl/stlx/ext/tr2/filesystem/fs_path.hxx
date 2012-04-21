/**\file*********************************************************************
  *                                                                     \brief
  *  Filesystem V3 library for TR2 (N3365)
  *
  ****************************************************************************
  */

#ifndef NTL__STLX_TR2_FILES
# error internal header
#endif

namespace std
{
  namespace tr2
  {
    namespace files
    {
      namespace __
      {
        template<typename charT>
        struct path_char_accept:
          std::integral_constant<bool, 
          is_same<charT, char>::value||
          is_same<charT, wchar_t>::value||
          is_same<charT, char16_t>::value||
          is_same<charT, char32_t>::value>
        {};


        template<typename Source> struct path_accept: false_type{};

        template<typename T> 
        struct path_accept<T*>: path_char_accept<T>{};

        template<typename T> 
        struct path_accept<const T*>: path_char_accept<T>{};

        template<typename T, size_t N>
        struct path_accept<T[N]>: path_char_accept<T>{};

        template<typename T, size_t N>
        struct path_accept<const T[N]>: path_char_accept<T>{};

        template<typename charT, class traits, class alloc>
        struct path_accept<std::basic_string<charT,traits,alloc> >: path_char_accept<charT>{};

        template<typename charT, class traits, class alloc>
        struct path_accept<ntl::nt::native_string<charT,traits,alloc> >: path_char_accept<charT>{};
        template<typename charT, class traits, class alloc>
        struct path_accept<ntl::nt::native_string<const charT,traits,alloc> >: path_char_accept<charT>{};

      }

      /**
       *  \addtogroup tr2
       *  @{
       *  \defgroup tr2_files Filesystem V3 Library
       *  @{
       *  \defgroup tr2_files_path Filesystem V3 Path
       *  @{
       **/



      class path
      {
      public:
        typedef wchar_t                                      value_type;
        typedef std::basic_string<value_type>                string_type;
        typedef std::codecvt<wchar_t, char, std::mbstate_t>  codecvt_type;
      private:
        typedef string_type::difference_type pos_type;
      public:

        ///\name encoding conversion
        inline static std::locale imbue( const std::locale& loc ) { return std::locale::classic(); }
        inline static const codecvt_type & codecvt();

        ///\name constructors and destructor
        path()
        {}

        path(const path& p)
          :s(p.s)
        {}

    #ifdef NTL_CXX_RV
        path(path&& p) __ntl_nothrow
          :s(std::move(p.s))
        {}
        path& operator=(path&& p) __ntl_nothrow
        {
          s.clear();
          swap(p);
          return *this;
        }
    #endif

        template <class Source>
        path(Source const& source, const codecvt_type& cvt, typename enable_if<__::path_accept<Source>::value>::type* =0)
        {
          append_conv(source, cvt);
        }

        template <class Source>
        path(const Source& source, typename enable_if<__::path_accept<Source>::value>::type* =0)
        {
          append_conv(source);
        }

        template <class InputIterator>
        path(InputIterator begin, InputIterator end, const codecvt_type& cvt=codecvt())
        {
          append(begin, end, cvt);
        }

        ~path()
        {}

        ///\name assignments
        path& operator=(const path& p)
        {
          s.clear();
          return append(p.s);
        }

        template <class Source>
        typename enable_if<__::path_accept<Source>::value, path&>::type operator=(Source const& source)
        {
          s.clear();
          return append_conv(source);
        }

        template <class Source>
        path& assign(Source const& source, const codecvt_type& cvt, typename enable_if<__::path_accept<Source>::value>::type* =0)
        {
          s.clear();
          return append_conv(source, cvt);
        }

        template <class InputIterator>
        path& assign(InputIterator begin, InputIterator end, const codecvt_type& cvt=codecvt())
        {
          s.clear();
          return append(begin, end, cvt);
        }

        ///\name appends
        path& operator/=(const path& p)
        {
          return append(p.s);
        }

        template <class Source>
        typename enable_if<__::path_accept<Source>::value, path&>::type operator/=(Source const& source)
        {
          return append_conv(source);
        }

        /**
         *	Template parameters named Source are required to be one of:
         * * A container with a value type of char,  wchar_t, char16_t, or char32_t.
         * * An iterator for a null terminated byte-string. The value type is required to be char, wchar_t, char16_t, or  char32_t.
         * * A C-array. The value type is required to be char,  wchar_t, char16_t, or char32_t.
         * * A std::filesystem::directory_entry.
         **/
        template <class Source>
        typename enable_if<__::path_accept<Source>::value, path&>::type append(Source const& source, const codecvt_type& cvt) { return append_conv(source, cvt); }

        template <class InputIterator>
        path& append(InputIterator begin, InputIterator end, const codecvt_type& cvt=codecvt());

      protected:
        path& append(const string_type& s, const codecvt_type&) { return append(s.c_str(), s.size()); }
        path& append(const string_type& s) { return append(s.c_str(), s.size()); }
        inline path& append(const value_type* s, size_t len);
      
        path& append_conv(const value_type* s, size_t len) { return append(s, len); }
        path& append_conv(const value_type* s) { return append(s, std::wcslen(s)); }
        path& append_conv(const ntl::nt::unicode_string& s) { return append(s.data(), s.length()); }
        path& append_conv(const ntl::nt::const_unicode_string& s) { return append(s.data(), s.length()); }
        path& append_conv(const string_type& s) { return append(s.c_str(), s.size()); }
        path& append_conv(const std::string& s, const codecvt_type&)
        {
          wstring_convert<codecvt_type> cvt;
          return append_conv(cvt.from_bytes(s));
        }
        path& append_conv(const std::string::value_type* s, const codecvt_type&)
        {
          wstring_convert<codecvt_type> cvt;
          return append_conv(cvt.from_bytes(s));
        }
        path& append_conv(const std::string& s)
        {
          wstring_convert<codecvt_type> cvt;
          return append_conv(cvt.from_bytes(s));
        }
        path& append_conv(const char* s)
        {
          wstring_convert<codecvt_type> cvt;
          return append_conv(cvt.from_bytes(s));
        }

      public:
        ///\name modifiers
        void  clear()
        {
          s.clear();
        }

        inline path& make_absolute(const path& base);
        inline path& make_absolute()
        {
          if(!s.empty())
            to_absolute(s);
          return *this;
        }
        
        inline path& make_preferred()
        {
          // already
          return *this;
        }
        
        inline path& remove_filename()
        {
          s.erase(filename_pos());
          return *this;
        }
        
        inline path& replace_extension(const path& new_extension = path());

        void swap(path& rhs)
        {
          std::swap(s, rhs.s);
        }

        ///\name native format observers

        /** returns native path, but not absolute */
        inline const string_type&  native()const __ntl_nothrow { return s; };
        inline const value_type*   c_str() const __ntl_nothrow { return s.c_str(); }

        template <class String>
        inline String string(const codecvt_type& cvt=codecvt()) const;

        inline const std::string    string    () const;
        inline const std::string    string    (const codecvt_type&) const { return string(); }
        inline const std::wstring&  wstring   (const codecvt_type&) const { return s; }
        inline const std::u16string u16string () const;
        inline const std::u32string u32string () const;
        inline const std::wstring&  wstring   () const { return s; }

        ///\name generic format observers
        template <class String>
        inline String generic_string(const codecvt_type& cvt=codecvt()) const;

        inline const std::string        generic_string(const codecvt_type&) const { return generic_string(); }
        inline const std::wstring       generic_wstring(const codecvt_type&) const { return generic_wstring(); }
        inline const std::u16string     generic_u16string() const;                                 // ditto
        inline const std::u32string     generic_u32string() const;                                 // ditto

        inline const std::string        generic_string () const;
        inline const std::wstring       generic_wstring() const
        {
          std::wstring es = s;
          to_generic(es);
          return std::move(es);
        }

      private:
        
        static inline void to_native(std::wstring& s);
        static inline void to_generic(std::wstring& s);
        static inline bool to_absolute(string_type& s);

        static bool is_sep(value_type c) { return c == slashval || c == backslash; }


      public:
        ///\name decomposition

        /** extension: returns UNC server name if any */
        inline path  unc_name() const;
        inline path  root_name() const;
        inline path  root_directory() const;
        inline path  root_path() const;
        inline path  relative_path() const;
        inline path  parent_path() const;
        inline path  filename() const;
        inline path  stem() const;
        inline path  extension() const;

        ///\name query
        bool empty() const { return s.empty(); }

        inline bool has_unc_name() const;
        inline bool has_drive() const;
        inline bool has_root_name() const;
        inline bool has_root_directory() const;
        inline bool has_root_path() const;
        inline bool has_relative_path() const;
        inline bool has_parent_path() const;
        inline bool has_filename() const;
        inline bool has_stem() const;
        inline bool has_extension() const;
        inline bool is_absolute() const;
        inline bool is_relative() const;

        ///\name iterators
        /**
         *	@brief A basic_path::iterator is a constant iterator satisfying all the requirements of a bidirectional iterator
         *  The forward traversal order is as follows:
         *  - The root-name element, if present.
         *  - The root-directory element, if present.
         *  - Each successive filename element, if present.
         *  - Dot, if one or more trailing non-root slash characters are present.
         *  .
         *  The backward traversal order is the reverse of forward traversal.
         **/
        struct iterator;
        typedef iterator const_iterator;

        /** Returns an iterator for the first present element in the traversal list above. If no elements are present, the end iterator. */
        inline iterator begin() const;
        /** Returns the end iterator. */
        inline iterator end()   const;

        /** Returns an iterator for the first present element in the traversal list above. If no elements are present, the end iterator. */
        inline const_iterator cbegin() const;
        /** Returns the end iterator. */
        inline const_iterator cend() const;
        ///\}

        /** Compares a two path objects */
        friend inline bool operator==(const path& a, const path& b) { return a.s == b.s; }

      protected:
        inline pos_type filename_pos() const;
      private:
        string_type s;

        static const value_type dotval = '.';
        static const value_type slashval = '/';
        static const value_type colonval = ':';
        static const value_type backslash = '\\';
        static const value_type qmark = '?';
        static const string_type::size_type npos = string_type::npos;
      };



      /**
       *	@brief Class template basic_filesystem_error
       *  @details The class template basic_filesystem_error defines the type of objects thrown as exceptions 
       *  to report file system errors from functions described in this clause.
       **/
      class filesystem_error:
        public system_error
      {
        path p1,p2;
        mutable string msg;
      public:
        typedef path path_type;

        ///\name Constructors
        explicit filesystem_error(const std::string& what_arg, error_code ec)
          :system_error(ec, what_arg)
        {}
        filesystem_error(const std::string& what_arg, const path_type& p1, error_code ec)
        :system_error(ec, what_arg), p1(p1)
        {}
        filesystem_error(const std::string& what_arg, const path_type& p1, const path_type& p2, error_code ec)
        :system_error(ec, what_arg), p1(p1), p2(p2)
        {}

        ///\name Observers
        /** Returns the stored path #1 */
        const path_type& path1() const { return p1; }
        /** Returns the stored path #2 */
        const path_type& path2() const { return p2; }

        inline const char* what() const
        {
          if(msg.empty()){
            __ntl_try{
              string tmp = system_error::what();
              if(!p1.empty() || !p2.empty()){
                if(!tmp.empty() && tmp != "")
                  tmp += " with ";
                if(!p1.empty()){
                  tmp += "path1 is '";
                  tmp += p1.string();
                  tmp += "'";
                }
                if(!p2.empty()){
                  if(!p1.empty())
                    tmp += " and ";
                  tmp += "path2 is '";
                  tmp += p2.string();
                  tmp += "'";
                }
                tmp += ".";
              }
              msg = move(tmp);
            }
            __ntl_catch(...){
              return system_error::what();
            }
          }
          return msg.c_str();
        }

        ///\}
      };


      /** @} tr2_files_path */
      /** @} tr2_files */
      /** @} tr2 */
    }
  }
}

#ifndef NTL_DOC
# include "fs_path_impl.hxx"
#endif
