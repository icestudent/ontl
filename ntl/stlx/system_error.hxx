/**\file*********************************************************************
*                                                                     \brief
*  19.4 System error support [syserr]
*
****************************************************************************
*/
#ifndef NTL__STLX_SYSTEM_ERROR
#define NTL__STLX_SYSTEM_ERROR

#include "type_traits.hxx"
#include "stdexcept.hxx"
#include "string.hxx"

namespace std {

  /**
   *	@group syserr System error support
   *  @{
   **/

  class error_category;
  class error_code;
  class error_condition;
  class system_error;

  template<class T>
  struct is_error_code_enum: public false_type {};

  template<class T>
  struct is_error_condition_enum: public false_type {};


  /// 19.4.1 syserr.errcat
  class error_category
  {
  public:
    virtual ~error_category();
#ifdef NTL__CXX
    error_category(const error_category&) = delete;
    error_category& operator=(const error_category&) = delete;
#else
  private:
    error_category(const error_category&);
    error_category& operator=(const error_category&);
  public:
#endif
    virtual const char* name() const = 0;
    virtual error_condition default_error_condition(int ev) const;
    virtual bool equivalent(int code, const error_condition& condition) const;
    virtual bool equivalent(const error_code& code, int condition) const;
    virtual string message(int ev) const = 0;

    bool operator==(const error_category& rhs) const;
    bool operator!=(const error_category& rhs) const;
    bool operator<(const error_category& rhs) const;
  };

  /// 19.4.2 syserr.errcode
  class error_code
  {
  public:
    // 19.4.2.2 constructors:
    error_code();
    error_code(int val, const error_category& cat);

    template <class ErrorCodeEnum>
    error_code(ErrorCodeEnum e, typename enable_if<is_error_code_enum<ErrorCodeEnum>::value>::type * = 0);

    // 19.4.2.3 modifers:
    void assign(int val, const error_category& cat);
    template <class ErrorCodeEnum>
    typename enable_if<is_error_code_enum<ErrorCodeEnum>::value>::type& operator=(ErrorCodeEnum e);
    void clear();

    // 19.4.2.4 observers:
    int value() const;
    const error_category& category() const;
    error_condition default_error_condition() const;
    string message() const;
    /*explicit */operator bool() const;

private:
    //int val_; // exposition only
    //const error_category& cat_; // exposition only
  };


  /// 19.4.3 syserr.errcondition
  class error_condition
  {
  public:
    // 19.4.3.2 constructors:
    error_condition();
    error_condition(int val, const error_category& cat);
    template <class ErrorConditionEnum>
    error_condition(ErrorConditionEnum e, typename enable_if<is_error_condition_enum<ErrorConditionEnum>::value>::type* = 0);

    // 19.4.3.3 modifers:
    void assign(int val, const error_category& cat);
    template<typename ErrorConditionEnum>
    typename enable_if<is_error_condition_enum<ErrorConditionEnum>::value, error_code>::type& operator= (ErrorConditionEnum e);
    void clear();

    // 19.4.3.4 observers:
    int value() const;
    const error_category& category() const;
    string message() const;
    /*explicit */operator bool() const;

  private:
    //int val_; // exposition only
    //const error_category& cat_; // exposition only
  };

  // 19.4.3.5 non-member functions:
  bool operator<(const error_condition& lhs, const error_condition& rhs);


  /// 19.4.5.1 syserr.syserr
  class system_error : public runtime_error
  {
  public:
    system_error(error_code ec, const string& what_arg);
    system_error(error_code ec);
    system_error(int ev, const error_category& ecat,
      const string& what_arg);
    system_error(int ev, const error_category& ecat);
    const error_code& code() const __ntl_nothrow;
    const char* what() const __ntl_nothrow;
  };



  // 19.4.4 Comparison operators:
  bool operator==(const error_code& lhs, const error_code& rhs);
  bool operator==(const error_code& lhs, const error_condition& rhs);
  bool operator==(const error_condition& lhs, const error_code& rhs);
  bool operator==(const error_condition& lhs, const error_condition& rhs);
  bool operator!=(const error_code& lhs, const error_code& rhs);
  bool operator!=(const error_code& lhs, const error_condition& rhs);
  bool operator!=(const error_condition& lhs, const error_code& rhs);
  bool operator!=(const error_condition& lhs, const error_condition& rhs);



  /** @} syserr */
} // std
#endif // NTL__STLX_SYSTEM_ERROR
