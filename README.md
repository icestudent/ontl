Open NT Native Template Library
====

## Why NTL?
This FAQ does not intend to provide a complete answer to the question: Could one write windows kernel mode drivers in C++? The short answer is No (see  [C++ for Kernel Mode Drivers: Pros and Cons](http://www.microsoft.com/whdc/driver/kernel/KMcode.mspx) on MSDN). The longer answer is Yes, but there are a lot of restrictions (see [C++ in an NT Driver](http://www.osronline.com/article.cfm?article=490) on OSR Online, registration required). We believe we have ways around (most, if not all of) those restrictions. The C++ Standard library is greatly appreciated by programmers who code in C++, but as of now, not available in the (Windows) kernel. We are sure things would get better, there would be standard library implementations those would work in windows kernel someday, but we live and code today, and we need a working implementation NOW.


More importantly, NTL is to enable an easier way to write portable code between kernel/user modes. Though this could not be easily done in general, but it would be helpful for testing kernel code in userland. Moreover (and better) - undocumented structures, PE format support and cryptographic functions are being added.

## How to use?
First of all, you should wait for a stable release. If you are on the bleeding edge, you could hack around our [current code base](https://github.com/icestudent/ontl). You are going to need MSVC 2005 with SP1 or newer and windows DDK/WDK library files (as NTL does not depend on _any_ header). Have a look at the [samples](https://github.com/icestudent/ontl/tree/master/samples) to get started. Also there is a [test suites](https://github.com/icestudent/ontl/tree/master/tests) based on [TUT framework](http://tut-framework.sourceforge.net/).

Now NTL comes with [VisualStudio Wizard](https://github.com/icestudent/ontl/tree/master/samples/vstudio/AppWiz).

The project is published under [zlib/libgng license](http://www.opensource.org/licenses/zlib-license.php). In short, the library may be used in a closed-source project without any restrictions or copyright notices.

### How about documentation?
Since many parts of NTL are just C++ wrappers for documented Windows APIs, we could copy-paste from MSDN but that's really not a good option.

NTL does not follow the waterfall model but is in early and active development stage, documentation will be available once its interface is stable. For now, you could run [doxygen](http://doxygen.org) on [NTL](https://github.com/icestudent/ontl/tree/master/doc).
