#include <exception>
#include <ntl-tests-common.hxx>


STLX_DEFAULT_TESTGROUP_NAME("std::exception");

template<> template<> void ::tut::to::test<01>()
{
  tut::ensure_equals(extmsg("uncaught exceptions"), std::uncaught_exception(), false);
}
template<> template<> void ::tut::to::test<02>()
{
  try{
    tut::ensure_equals(extmsg("uncaught exceptions"), std::uncaught_exception(), false);
    throw 0;
  }catch(...){
    tut::ensure_equals(extmsg("uncaught exceptions"), std::uncaught_exception(), true);
  }
}
