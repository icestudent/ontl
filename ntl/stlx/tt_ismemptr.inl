#undef NTL_TT_TCV_

#ifndef NTL_TT_TCV
#define NTL_TT_TCV_

template <class T, class RT>
struct is_member_object_pointer<RT T::* NTL_TT_PCV> : public true_type {};

#else 
#define NTL_TT_TCV_ NTL_TT_TCV
#endif

template <class T, class RT>
struct is_member_function_pointer<RT (T::* NTL_TT_PCV)()NTL_TT_TCV_> : public true_type {};

template <class T, class RT>
struct is_member_function_pointer<RT (T::* NTL_TT_PCV)(...)NTL_TT_TCV_> : public true_type {};

template <class T, class RT, class A1>
struct is_member_function_pointer<RT (T::* NTL_TT_PCV)(A1)NTL_TT_TCV_> : public true_type {};

template <class T, class RT, class A1>
struct is_member_function_pointer<RT (T::* NTL_TT_PCV)(A1 ...)NTL_TT_TCV_> : public true_type {};

template <class T, class RT, class A1, class A2>
struct is_member_function_pointer<RT (T::* NTL_TT_PCV)(A1, A2)NTL_TT_TCV_> : public true_type {};

template <class T, class RT, class A1, class A2>
struct is_member_function_pointer<RT (T::* NTL_TT_PCV)(A1, A2 ...)NTL_TT_TCV_> : public true_type {};

template <class T, class RT, class A1, class A2, class A3>
struct is_member_function_pointer<RT (T::* NTL_TT_PCV)(A1, A2, A3)NTL_TT_TCV_> : public true_type {};

template <class T, class RT, class A1, class A2, class A3>
struct is_member_function_pointer<RT (T::* NTL_TT_PCV)(A1, A2, A3 ...)NTL_TT_TCV_> : public true_type {};

template <class T, class RT, class A1, class A2, class A3, class A4>
struct is_member_function_pointer<RT (T::* NTL_TT_PCV)(A1, A2, A3, A4)NTL_TT_TCV_> : public true_type {};

template <class T, class RT, class A1, class A2, class A3, class A4>
struct is_member_function_pointer<RT (T::* NTL_TT_PCV)(A1, A2, A3, A4 ...)NTL_TT_TCV_> : public true_type {};

template <class T, class RT, class A1, class A2, class A3, class A4, class A5>
struct is_member_function_pointer<RT (T::* NTL_TT_PCV)(A1, A2, A3, A4, A5)NTL_TT_TCV_> : public true_type {};

template <class T, class RT, class A1, class A2, class A3, class A4, class A5>
struct is_member_function_pointer<RT (T::* NTL_TT_PCV)(A1, A2, A3, A4, A5 ...)NTL_TT_TCV_> : public true_type {};

template <class T, class RT, class A1, class A2, class A3, class A4, class A5, class A6>
struct is_member_function_pointer<RT (T::* NTL_TT_PCV)(A1, A2, A3, A4, A5, A6)NTL_TT_TCV_> : public true_type {};

template <class T, class RT, class A1, class A2, class A3, class A4, class A5, class A6>
struct is_member_function_pointer<RT (T::* NTL_TT_PCV)(A1, A2, A3, A4, A5, A6 ...)NTL_TT_TCV_> : public true_type {};

template <class T, class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
struct is_member_function_pointer<RT (T::* NTL_TT_PCV)(A1, A2, A3, A4, A5, A6, A7)NTL_TT_TCV_> : public true_type {};

template <class T, class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
struct is_member_function_pointer<RT (T::* NTL_TT_PCV)(A1, A2, A3, A4, A5, A6, A7 ...)NTL_TT_TCV_> : public true_type {};

template <class T, class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
struct is_member_function_pointer<RT (T::* NTL_TT_PCV)(A1, A2, A3, A4, A5, A6, A7, A8)NTL_TT_TCV_> : public true_type {};

template <class T, class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
struct is_member_function_pointer<RT (T::* NTL_TT_PCV)(A1, A2, A3, A4, A5, A6, A7, A8 ...)NTL_TT_TCV_> : public true_type {};

template <class T, class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
struct is_member_function_pointer<RT (T::* NTL_TT_PCV)(A1, A2, A3, A4, A5, A6, A7, A8, A9)NTL_TT_TCV_> : public true_type {};

template <class T, class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
struct is_member_function_pointer<RT (T::* NTL_TT_PCV)(A1, A2, A3, A4, A5, A6, A7, A8, A9 ...)NTL_TT_TCV_> : public true_type {};

template <class T, class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
struct is_member_function_pointer<RT (T::* NTL_TT_PCV)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)NTL_TT_TCV_> : public true_type {};

template <class T, class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
struct is_member_function_pointer<RT (T::* NTL_TT_PCV)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10 ...)NTL_TT_TCV_> : public true_type {};
