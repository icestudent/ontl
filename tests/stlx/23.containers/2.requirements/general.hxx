/**\file*********************************************************************
 *                                                                     \brief
 *  23.2.1 General container requirements [container.requirements.general]
 *
 ****************************************************************************
 */
#pragma once

template<> template<> void ::tut::to::test<01>()
{
  X u;
  tut::ensure_equals(u.size(), 0);
}

template<> template<> void ::tut::to::test<02>()
{
  tut::ensure_equals(X().size(), 0);
}

template<> template<> void ::tut::to::test<03>()
{
  X a;
  quick_ensure(X(a) == a);
}

template<> template<> void ::tut::to::test<04>()
{
  X a;
  X u(a);
  quick_ensure(u == a);
}

template<> template<> void ::tut::to::test<05>()
{
  X a;
  X u = a;
  quick_ensure(u == a);
}

#ifdef NTL__CXX_RV
// rv ctors
#endif

template<> template<> void ::tut::to::test<06>()
{
  X a;
  (&a)->~X();
}
