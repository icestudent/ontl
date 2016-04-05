/**\file*********************************************************************
 *                                                                     \brief
 *  Environment variables
 *
 ****************************************************************************
 */
#ifndef NTL__NT_ENVIRON
#define NTL__NT_ENVIRON
#pragma once

#include "basedef.hxx"
#include "string.hxx"

namespace ntl {
  namespace nt {
    /*
    NtEnumerateSystemEnvironmentValuesEx
    NtEnumerateSystemEnvironmentValuesEx
    NtQuerySystemEnvironmentValue
    NtQuerySystemEnvironmentValue
    NtQuerySystemEnvironmentValueEx
    NtQuerySystemEnvironmentValueEx
    NtSetSystemEnvironmentValue
    NtSetSystemEnvironmentValue
    NtSetSystemEnvironmentValueEx
    NtSetSystemEnvironmentValueEx
    RtlCreateEnvironment
    RtlCreateEnvironment
    RtlCreateEnvironmentEx
    RtlCreateEnvironmentEx
    RtlDestroyEnvironment
    RtlDestroyEnvironment
    RtlExpandEnvironmentStrings
    RtlExpandEnvironmentStrings
    RtlExpandEnvironmentStrings_U
    RtlExpandEnvironmentStrings_U
    RtlQueryEnvironmentVariable
    RtlQueryEnvironmentVariable
    RtlQueryEnvironmentVariable_U
    RtlQueryEnvironmentVariable_U
    RtlSetCurrentEnvironment
    RtlSetCurrentEnvironment
    RtlSetEnvironmentStrings
    RtlSetEnvironmentStrings
    RtlSetEnvironmentVar
    RtlSetEnvironmentVar
    RtlSetEnvironmentVariable
    RtlSetEnvironmentVariable
    ZwEnumerateSystemEnvironmentValuesEx
    ZwEnumerateSystemEnvironmentValuesEx
    ZwQuerySystemEnvironmentValue
    ZwQuerySystemEnvironmentValue
    ZwQuerySystemEnvironmentValueEx
    ZwQuerySystemEnvironmentValueEx
    ZwSetSystemEnvironmentValue
    ZwSetSystemEnvironmentValue
    ZwSetSystemEnvironmentValueEx
    ZwSetSystemEnvironmentValueEx
    **/

    NTL_EXTERNAPI
    ntstatus __stdcall RtlCreateEnvironment(bool CloneCurrentEnvironment, void** Environment);
    NTL_EXTERNAPI
    ntstatus __stdcall RtlDestroyEnvironment(void* Environment);

    NTL_EXTERNAPI
    ntstatus __stdcall RtlSetEnvironmentVariable(void** Environment, const const_unicode_string& Name, const const_unicode_string& Value);


    NTL_EXTERNAPI
    ntstatus __stdcall RtlQueryEnvironmentVariable_U(void* Environment, const const_unicode_string& Name, unicode_string& Value);



    class environment
    {
      void* env;
    public:
      static environment assign(void* env)
      {
        environment re; re.env = env;
        return re;
      }
      static environment create(bool clone_current = true)
      {
        void* env = nullptr;
        /*ntstatus st =*/ RtlCreateEnvironment(clone_current, &env);
        return assign(env);
      }

      environment()
      {
        env = peb::instance().process_parameters()->Environment;
      }

      ntstatus destroy()
      {
        const ntstatus st = RtlDestroyEnvironment(env);
        env = nullptr;
        return st;
      }

      bool exists(const const_unicode_string& name) const
      {
        unicode_string us;
        return RtlQueryEnvironmentVariable_U(env, name, us) != status::variable_not_found;
      }

      ntstatus query(const const_unicode_string& name, unicode_string& value) const
      {
        return RtlQueryEnvironmentVariable_U(env, name, value);
      }

      ntstatus set(const const_unicode_string& name, const const_unicode_string& value)
      {
        return RtlSetEnvironmentVariable(&env, name, value);
      }

    public:
      typedef std::pair<std::wstring_ref, std::wstring_ref> value_type;

      struct iterator;
      typedef iterator const_iterator;

      struct iterator: std::iterator<std::forward_iterator_tag, value_type>
      {
        friend class environment;
        friend bool operator== (const iterator& x, const iterator& y) { return x.cur == y.cur; }
        friend bool operator!= (const iterator& x, const iterator& y) { return x.cur != y.cur; }

        reference operator*()   const { return cur; }
        pointer   operator->()  const { return &cur;  }

        iterator& operator++()  
        {
          capture_next();
          return *this;
        }

        iterator operator++(int)
        {
          iterator tmp(*this);
          ++*this;
          return tmp;
        }

        iterator()
          : cur()
        {}

      protected:
        explicit iterator(const void* env)
          : cur()
        {
          capture(reinterpret_cast<const wchar_t*>(env));
        }

        void capture_next()
        {
          if(const wchar_t* end = cur.second.end())
            capture(end + 1);
        }

        void capture(const wchar_t* env)
        {
          std::wstring_ref s = env;
          const size_t split = s.rfind('=');
          if(s.empty() || split == s.npos) {
            // end
            cur.first.clear();
            cur.second.clear();
            return;
          }

          cur.first = s.substr(0, split);
          cur.second = s.substr(split+1);
        }


      private:
        mutable value_type cur;
      };

      const_iterator begin() const
      {
        return const_iterator(env);
      }

      const_iterator end() const
      {
        return const_iterator();
      }

      const_iterator cbegin() const { return begin(); }
      const_iterator cend() const { return end(); }
    };


  }
}
#endif //NTL__NT_ENVIRON
