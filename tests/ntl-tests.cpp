#include <consoleapp.hxx>
#include <iostream>

#pragma warning(disable:4127) // conditional expression is constant
#include <tut/tut.hpp>
#include <tut/tut_reporter.hpp>

tut::test_runner_singleton runner;

// 1 - single test; 2 - single group test; else - all tests
#define RUN_TESTS_METHOD 0

int ntl::consoleapp::main()
{
  tut::reporter writer;
  runner.get().set_callback(&writer);

#if RUN_TESTS_METHOD == 1
  tut::test_result tr;

  runner.get().run_test ("std::exception_ptr", 4, tr);
#elif RUN_TESTS_METHOD == 2
  runner.get().run_tests("std::exception_ptr");
#else
  runner.get().run_tests();
#endif

  bool ok = writer.all_ok();

  std::cout << (ok ? "well done" : "something wrong :-(") << '.' << std::endl;
  std::cout << "press enter to exit.";
  std::cin.get();

  return !ok;
}
