#include <exception>
#include <ntl-tests-common.hxx>

STLX_DEFAULT_TESTGROUP_NAME("std::exception_ptr#access");

std::ostream& operator<<(std::ostream& os, const std::type_info& ti)
{
  return os << "type_info{" << ti.name() << '}';
}

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

template<> template<> void tut::to::test<05>()
{
  struct base{};
  struct derived: base{};
  try {
    throw derived();
  }
  catch(...){
    std::exception_ptr ptr = std::current_exception();
    quick_ensure(ptr->target<derived>() != nullptr);
    quick_ensure(ptr->target<base>() != nullptr);
  }
}
template<> template<> void tut::to::test<06>()
{
  struct base{};
  struct derived: base{};
  try {
    throw derived();
  }
  catch(...){
    std::exception_ptr ptr = std::current_exception();

    tut::ensure_equals(extmsg("type count"), ptr->type_count(), 2U);

    quick_ensure(ptr->target<derived>(0) != nullptr);
    quick_ensure(ptr->target<derived>(1) != nullptr);

    tut::ensure_equals(extmsg("type #1"), ptr->target_type(0), typeid(derived));
    tut::ensure_equals(extmsg("type #2"), ptr->target_type(1), typeid(base));
  }
}

