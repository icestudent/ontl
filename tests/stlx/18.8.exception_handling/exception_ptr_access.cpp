
#include <exception>
#include <ntl-tests-common.hxx>

namespace tut
{
  STLX_DEFAULT_TESTGROUP();
  testgroup tg_eptr_access("std::exception_ptr#access");
}

namespace 
{
  template<> template<> void tut::to::test<01>()
  {
    std::exception_ptr ptr;
    // note that default-constructed exception_ptr is null-pointer.
    quick_ensure(ptr->target_type() == typeid(void));
  }

  template<> template<> void tut::to::test<02>()
  {
    try {
      throw 0;
    }
    catch(...){
      std::exception_ptr ptr = std::current_exception();
      quick_ensure(ptr->target_type() == typeid(0));
    }
  }
  template<> template<> void tut::to::test<03>()
  {
    try {
      throw 0;
    }
    catch(...){
      std::exception_ptr ptr = std::current_exception();
      quick_ensure(ptr->target<int>() != nullptr);
    }
  }
  template<> template<> void tut::to::test<04>()
  {
    try {
      throw 1;
    }
    catch(...){
      std::exception_ptr ptr = std::current_exception();
      tut::ensure_equals(*ptr->target<int>(), 1);
    }
  }


}