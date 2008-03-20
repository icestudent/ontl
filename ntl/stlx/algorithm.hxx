/**\file*********************************************************************
 *                                                                     \brief
 *  Algorithms library [25 lib.algorithms]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_ALGORITHM
#define NTL__STLX_ALGORITHM

#include "iterator.hxx"
#include "utility.hxx"

namespace std {

// 25.1, non-modifying sequence operations:

/// 25.1.1 For each [lib.alg.foreach]
template<class InputIterator, class Function>
__forceinline
Function 
  for_each(InputIterator first, InputIterator last, Function f)
{
  while ( first != last ) f(*first++);
  return f;
};

/// 25.1.2 Find [lib.alg.find]
template<class InputIterator, class T>
__forceinline
InputIterator
  find(InputIterator first, InputIterator last, const T& value)
{
  while ( first != last && !(*first == value) ) ++first;
  return first;
}

/// 25.1.2 Find [lib.alg.find]
template<class InputIterator, class Predicate>
__forceinline
InputIterator
  find_if(InputIterator first, InputIterator last, Predicate pred)
{
  while ( first != last && !(pred(*first) != false) ) ++first;
  return first;
}

/// 25.1.3 Find End [lib.alg.find.end]
template<class ForwardIterator1, class ForwardIterator2>
__forceinline
ForwardIterator1 
  find_end(ForwardIterator1 first1, ForwardIterator1 last1,
           ForwardIterator2 first2, ForwardIterator2 last2);

/// 25.1.3 Find End [lib.alg.find.end]
template<class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
__forceinline
ForwardIterator1
  find_end(ForwardIterator1 first1, ForwardIterator1 last1,
           ForwardIterator2 first2, ForwardIterator2 last2,
           BinaryPredicate pred);

/// 25.1.4 Find First [lib.alg.find.first.of]
template<class ForwardIterator1, class ForwardIterator2>
__forceinline
ForwardIterator1
  find_first_of(ForwardIterator1 first1, ForwardIterator1 last1,
                ForwardIterator2 first2, ForwardIterator2 last2)
{
  for ( ; first1 != last1; ++first1 )
    for ( ForwardIterator2 j = first2; j != last2; ++ j )
      if ( *first1 == *j ) return first1;
  return first1;  // here first1 == last1
}

/// 25.1.4 Find First [lib.alg.find.first.of]
template<class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
__forceinline
ForwardIterator1
  find_first_of(ForwardIterator1 first1, ForwardIterator1 last1,
                ForwardIterator2 first2, ForwardIterator2 last2,
                BinaryPredicate pred)
{
  for ( ; first1 != last1; ++first1 )
    for ( ForwardIterator2 j = first2; j != last2; ++ j )
      if ( pred(*first1, *j) != false ) return first1;
  return first1;  // here first1 == last1
}

/// 25.1.5 Adjacent find [lib.alg.adjacent.find]
template<class ForwardIterator>
__forceinline
ForwardIterator
  adjacent_find(ForwardIterator first, ForwardIterator last)
{
  // the code is a bit messy to gain Named Return Value optimization
  if ( first == last )  return first;
  for ( ; ; )
  {
    ForwardIterator i = first;
    if ( ++i == last )  return ++first;
    if ( *first == *i ) return first;
    first = i;
  }  
}

/// 25.1.5 Adjacent find [lib.alg.adjacent.find]
template<class ForwardIterator, class BinaryPredicate>
__forceinline
ForwardIterator
  adjacent_find(ForwardIterator first, ForwardIterator last,
                BinaryPredicate pred)
{
  if ( first == last )  return first;
  for ( ; ; )
  {
    ForwardIterator i = first;
    if ( ++i == last )  return ++first;
    if ( pred(*first, *i) != false ) return first;
    first = i;
  }  
}

/// 25.1.6 Count [lib.alg.count]
template<class InputIterator, class T>
__forceinline
typename iterator_traits<InputIterator>::difference_type
  count(InputIterator first, InputIterator last, const T& value)
{
  typename iterator_traits<InputIterator>::difference_type count = 0;
  for ( ; first != last; ++first )
    if ( *first == value ) ++count;
  return count;
}

/// 25.1.6 Count [lib.alg.count]
template<class InputIterator, class Predicate>
__forceinline
typename iterator_traits<InputIterator>::difference_type
  count_if(InputIterator first, InputIterator last, Predicate pred)
{
  typename iterator_traits<InputIterator>::difference_type count = 0;
  for ( ; first != last; ++first )
    if ( pred(*first) != false ) ++count;
  return count;
}

/// 25.1.7 Mismatch [lib.mismatch]
template<class InputIterator1, class InputIterator2>
__forceinline
pair<InputIterator1, InputIterator2>
  mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2)
{
  while ( first1 != last1 && *first1 == *first2 )
  {
    ++first1;
    ++first2;
  }
  return pair<InputIterator1, InputIterator2>(first1, first2);
}

/// 25.1.7 Mismatch [lib.mismatch]
template
<class InputIterator1, class InputIterator2, class BinaryPredicate>
__forceinline
pair<InputIterator1, InputIterator2>
  mismatch(InputIterator1 first1, InputIterator1 last1,
           InputIterator2 first2, BinaryPredicate pred)
{
  while ( first1 != last1 && pred(*first1, *first2) == false )
  {
    ++first1;
    ++first2;
  }
  return pair<InputIterator1, InputIterator2>(first1, first2);
}

/// 25.1.8 Equal [lib.alg.equal]
template<class InputIterator1, class InputIterator2>
__forceinline
bool
  equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2)
{
  for ( ; first1 != last1; ++first1, ++first2 )
    if ( !(*first1 == *first2) ) return false;
  return true;
}

/// 25.1.8 Equal [lib.alg.equal]
template
<class InputIterator1, class InputIterator2, class BinaryPredicate>
__forceinline
bool
  equal(InputIterator1 first1, InputIterator1 last1,
        InputIterator2 first2, BinaryPredicate pred)
{
  for ( ; first1 != last1; ++first1, ++first2 )
    if ( pred(*first1, *first2) != false ) return false;
  return true;
}

/// 25.1.9 Search [lib.alg.search]
template<class ForwardIterator1, class ForwardIterator2>
__forceinline
ForwardIterator1
  search(ForwardIterator1 first1, ForwardIterator1 last1,
         ForwardIterator2 first2, ForwardIterator2 last2)
{
  // iterators are of forward category, so do a brute force search
  // @todo: specializations for other iterator categories?
  for ( ; ; )
  {
    ForwardIterator1 i1 = first1;
    ForwardIterator2 i2 = first2;
    for ( ; ; )
    {
      if ( i1 == last1 ) return first1; // not found, first1 == last1
      if ( i2 == last2 ) return first1; // found
      if ( *i1 != *i2 ) break;
      ++i1;
      ++i2;
    }
    ++first1;
  }
}

/// 25.1.9 Search [lib.alg.search]
template<class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
__forceinline
ForwardIterator1
  search(ForwardIterator1 first1, ForwardIterator1 last1,
         ForwardIterator2 first2, ForwardIterator2 last2,
         BinaryPredicate pred)
{
  for ( ; ; )
  {
    ForwardIterator1 i1 = first1;
    ForwardIterator2 i2 = first2;
    for ( ; ; )
    {
      if ( i1 == last1 ) return first1; // not found, first1 == last1
      if ( i2 == last2 ) return first1; // found
      if ( pred(*i1, *i2) != false ) break;
      ++i1;
      ++i2;
    }
    ++first1;
  }
}

/// 25.1.9 Search [lib.alg.search]
template<class ForwardIterator, class Size, class T>
__forceinline
ForwardIterator
  search_n(ForwardIterator first, ForwardIterator last, Size count,
           const T& value)
{
  for ( ; ; )
  {
    ForwardIterator i = first;
    Size n = count;
    for ( ; ; )
    {
      if ( i == last ) return first; // not found, first == last
      if ( n == 0 ) return first; // found
      if ( *i == value ) break;
      ++i;
      --n;
    }
    ++first;
  }
}

/// 25.1.9 Search [lib.alg.search]
template<class ForwardIterator, class Size, class T, class BinaryPredicate>
__forceinline
ForwardIterator
  search_n(ForwardIterator first, ForwardIterator last,
           Size count, const T& value, BinaryPredicate pred)
{
  for ( ; ; )
  {
    ForwardIterator i = first;
    Size n = count;
    for ( ; ; )
    {
      if ( i == last ) return first; // not found, first == last
      if ( n == 0 ) return first; // found
      if ( pred(*i, value) != false ) break;
      ++i;
      --n;
    }
    ++first;
  }
}

// 25.2, modifying sequence operations:

/// 25.2.1 Copy [lib.alg.copy]
template<class InputIterator, class OutputIterator>
__forceinline
OutputIterator
  copy(InputIterator first, InputIterator last, OutputIterator result)
{
  while ( first != last )
  {
    *result = *first;
    ++result;
    ++first;
  }
  return result;
}

template<class BidirectionalIterator1, class BidirectionalIterator2>
__forceinline
BidirectionalIterator2
  copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last,
                BidirectionalIterator2 result)
{
  while ( first != last ) *--result == *--first;
  return result;
}

/// 25.2.2 Swap [lib.alg.swap]
template<class T>
__forceinline
void
  swap(T& a, T& b)
{
  T temp( a );
  a = b;
  b = temp;
}

/// 25.2.2 Swap [lib.alg.swap]
template<class ForwardIterator1, class ForwardIterator2>
__forceinline
ForwardIterator2
  swap_ranges(ForwardIterator1 first1, ForwardIterator1 last1,
              ForwardIterator2 first2)
{
  for ( ; first1 != last1; ++first1, ++first2 )
    swap(*first1, *first2);
  return first2;
}

/// 25.2.2 Swap [lib.alg.swap]
template<class ForwardIterator1, class ForwardIterator2>
__forceinline
void
  iter_swap(ForwardIterator1 a, ForwardIterator2 b)
{
  T temp( *a );
  *a = *b;
  *b = temp;
}

/// 25.2.3 Transform [lib.alg.transform]
template<class InputIterator, class OutputIterator, class UnaryOperation>
__forceinline
OutputIterator
  transform(InputIterator first, InputIterator last,
            OutputIterator result, UnaryOperation op)
{
  for ( ; first != last; ++first, ++result )
    *result = op(*first);
  return result;
}

/// 25.2.3 Transform [lib.alg.transform]
template<class InputIterator1, class InputIterator2, class OutputIterator,
         class BinaryOperation>
__forceinline
OutputIterator
  transform(InputIterator1 first1, InputIterator1 last1,
            InputIterator2 first2, OutputIterator result,
            BinaryOperation binary_op)
{
  for ( ; first1 != last1; ++first1, ++first2, ++result )
    *result = binary_op(*first1, *first2);
  return result;
}

/// 25.2.4 Replace [lib.alg.replace]
template<class ForwardIterator, class T>
__forceinline
void
  replace(ForwardIterator first, ForwardIterator last, 
          const T& old_value, const T& new_value)
{
  for ( ; first != last; ++first )
    if ( *first == old_value ) *first = new_value;
}

/// 25.2.4 Replace [lib.alg.replace]
template<class ForwardIterator, class Predicate, class T>
__forceinline
void
  replace_if(ForwardIterator first, ForwardIterator last,
             Predicate pred, const T& new_value)
{
  for ( ; first != last; ++first )
    if ( pred(*first) != false ) *first = new_value;
}

/// 25.2.4 Replace [lib.alg.replace]
template<class InputIterator, class OutputIterator, class T>
__forceinline
OutputIterator
  replace_copy(InputIterator first, InputIterator last, OutputIterator result,
               const T& old_value, const T& new_value)
{
  for ( ; first != last; ++first, ++result )
    if ( *result == old_value ) *result = new_value;
}

/// 25.2.4 Replace [lib.alg.replace]
template<class Iterator, class OutputIterator, class Predicate, class T>
__forceinline
OutputIterator
  replace_copy_if(Iterator first, Iterator last, OutputIterator result,
                  Predicate pred, const T& new_value)
{
  for ( first != last; ++first, ++result )
    if ( pred(*result) != false ) *result = new_value;
}

/// 25.2.5 Fill [lib.alg.fill]
template<class ForwardIterator, class T>
__forceinline
void
  fill(ForwardIterator first, ForwardIterator last, const T& value)
{
  for ( ; first != last; ++first ) *first = value;
}

/// 25.2.5 Fill [lib.alg.fill]
template<class OutputIterator, class Size, class T>
__forceinline
void
  fill_n(OutputIterator first, Size n, const T& value)
{
  for ( ; n--; ++first ) *first = value;
}

/// 25.2.6 Generate [lib.alg.generate]
template<class ForwardIterator, class Generator>
__forceinline
void
  generate(ForwardIterator first, ForwardIterator last, Generator gen)
{
  for ( ; first != last; ++first ) *first = gen();
}

/// 25.2.6 Generate [lib.alg.generate]
template<class OutputIterator, class Size, class Generator>
__forceinline
void
  generate_n(OutputIterator first, Size n, Generator gen)
{
  for ( ; n--; ++first ) *first = gen();
}

/// 25.2.7 Remove [lib.alg.remove]
template<class InputIterator, class OutputIterator, class T>
__forceinline
OutputIterator
  remove_copy(InputIterator first, InputIterator last, 
              OutputIterator result, const T& value)
{
  for ( ; first != last; ++first )
    if ( !(*first == value) )
    {
      *result == *first;
      ++result;
    }
  return result;
}

/// 25.2.7 Remove [lib.alg.remove]
template<class InputIterator, class OutputIterator, class Predicate>
__forceinline
OutputIterator
  remove_copy_if(InputIterator first, InputIterator last,
                 OutputIterator result, Predicate pred)
{
  for ( ; first != last; ++first )
    if ( !(pred(*first) != false) )
    {
      *result == *first;
      ++result;
    }
  return result;
}

/// 25.2.7 Remove [lib.alg.remove]
template<class ForwardIterator, class T>
__forceinline
ForwardIterator
  remove(ForwardIterator first, ForwardIterator last, const T& value)
{
  first = find(first, last, value);
  ForwardIterator i = first;
  i = ( i == last ) ? i : remove_copy(++i, last, first, value);
  return i;
}

/// 25.2.7 Remove [lib.alg.remove]
template<class ForwardIterator, class Predicate>
__forceinline
ForwardIterator
  remove_if(ForwardIterator first, ForwardIterator last, Predicate pred)
{
  first = find_if(first, last, pred);
  ForwardIterator i = first;
  i = ( i == last ) ? i : remove_copy_if(++i, last, first, pred);
  return i; 
}

/// 25.2.8 Unique [lib.alg.unique]
template<class InputIterator, class OutputIterator>
__forceinline
OutputIterator
  unique_copy(InputIterator first, InputIterator last, OutputIterator result)
{
  while ( first != last )
  {
    typename iterator_traits<OutputIterator>::value_type unique = *result = *first;
    ++result;
    while ( ++first != last && unique == *first ) {/*-*/}
  }
  return result;
}

/// 25.2.8 Unique [lib.alg.unique]
template<class InputIterator, class OutputIterator, class BinaryPredicate>
__forceinline
OutputIterator
  unique_copy(InputIterator first, InputIterator last, 
              OutputIterator result, BinaryPredicate pred)
{
  while ( first != last )
  {
    typename iterator_traits<OutputIterator>::value_type unique = *result = *first;
    ++result;
    while ( ++first != last && pred(unique, *first) != false ) {/*-*/}
  }
  return result;
}

/// 25.2.8 Unique [lib.alg.unique]
template<class ForwardIterator>
__forceinline
ForwardIterator
  unique(ForwardIterator first, ForwardIterator last)
{
  first = adjacent_find(first, last);
  return unique_copy(first, last, first);
}

/// 25.2.8 Unique [lib.alg.unique]
template<class ForwardIterator, class BinaryPredicate>
__forceinline
ForwardIterator
  unique(ForwardIterator first, ForwardIterator last, BinaryPredicate pred)
{
  first = adjacent_find(first, last, pred);
  return unique_copy(first, last, first, pred);
}

/// 25.2.9 Reverse [lib.alg.reverse]
template<class BidirectionalIterator>
__forceinline
void
  reverse(BidirectionalIterator first, BidirectionalIterator last)
{
  if ( first != last )
    for ( ; first != --last; ++first )
      iter_swap(first, last);
}

/// 25.2.9 Reverse [lib.alg.reverse]
template<class BidirectionalIterator, class OutputIterator>
__forceinline
OutputIterator
  reverse_copy(BidirectionalIterator first, BidirectionalIterator last,
               OutputIterator result)
{
  for ( ; first != last; ++result )
    *result = *--last;
  return result;
}

/// 25.2.10 Rotate [lib.alg.rotate]
template<class ForwardIterator>
inline
void
  rotate(ForwardIterator first, ForwardIterator middle, ForwardIterator last);

template<class ForwardIterator, class OutputIterator>
inline
OutputIterator
  rotate_copy(ForwardIterator first, ForwardIterator middle,
              ForwardIterator last, OutputIterator result);

template<class RandomAccessIterator>
inline
void
  random_shuffle(RandomAccessIterator first, RandomAccessIterator last);

template<class RandomAccessIterator, class RandomNumberGenerator>
inline
void
  random_shuffle(RandomAccessIterator first, RandomAccessIterator last,
                 RandomNumberGenerator& rand);

// 25.2.12, partitions:
template<class BidirectionalIterator, class Predicate>
inline
BidirectionalIterator
  partition(BidirectionalIterator first, BidirectionalIterator last,
            Predicate pred);

template<class BidirectionalIterator, class Predicate>
inline
BidirectionalIterator
  stable_partition(BidirectionalIterator first, BidirectionalIterator last,
                   Predicate pred);

// 25.3, sorting and related operations:
// 25.3.1, sorting:
template<class RandomAccessIterator>
inline
void
  sort(RandomAccessIterator first, RandomAccessIterator last);

template<class RandomAccessIterator, class Compare>
inline
void
  sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp);

template<class RandomAccessIterator>
inline
void
  stable_sort(RandomAccessIterator first, RandomAccessIterator last);

template<class RandomAccessIterator, class Compare>
inline
void
  stable_sort(RandomAccessIterator first, RandomAccessIterator last,
              Compare comp);

template<class RandomAccessIterator>
inline
void
  partial_sort(RandomAccessIterator first, RandomAccessIterator middle,
               RandomAccessIterator last);

template<class RandomAccessIterator, class Compare>
inline
void
  partial_sort(RandomAccessIterator first, RandomAccessIterator middle,
               RandomAccessIterator last, Compare comp);

template<class InputIterator, class RandomAccessIterator>
inline
RandomAccessIterator
  partial_sort_copy(InputIterator first, InputIterator last,
                    RandomAccessIterator result_first, 
                    RandomAccessIterator result_last);

template<class InputIterator, class RandomAccessIterator, class Compare>
inline
RandomAccessIterator
  partial_sort_copy(InputIterator first, InputIterator last,
                    RandomAccessIterator result_first,
                    RandomAccessIterator result_last,
                    Compare comp);

/// 25.3.2 Nth element [lib.alg.nth.element]
template<class RandomAccessIterator>
inline
void
  nth_element(RandomAccessIterator first, RandomAccessIterator nth,
              RandomAccessIterator last);

template<class RandomAccessIterator, class Compare>
inline
void
  nth_element(RandomAccessIterator first, RandomAccessIterator nth,
              RandomAccessIterator last, Compare comp);

// 25.3.3, binary search:
template<class ForwardIterator, class T>
inline
ForwardIterator
  lower_bound(ForwardIterator first, ForwardIterator last, const T& value);

template<class ForwardIterator, class T, class Compare>
inline
ForwardIterator
  lower_bound(ForwardIterator first, ForwardIterator last, 
              const T& value, Compare comp);

template<class ForwardIterator, class T>
inline
ForwardIterator
  upper_bound(ForwardIterator first, ForwardIterator last, const T& value);

template<class ForwardIterator, class T, class Compare>
inline
ForwardIterator
  upper_bound(ForwardIterator first, ForwardIterator last,
              const T& value, Compare comp);

template<class ForwardIterator, class T>
inline
pair<ForwardIterator, ForwardIterator>
  equal_range(ForwardIterator first, ForwardIterator last, const T& value);

template<class ForwardIterator, class T, class Compare>
inline
pair<ForwardIterator, ForwardIterator>
  equal_range(ForwardIterator first, ForwardIterator last,
              const T& value, Compare comp);

template<class ForwardIterator, class T>
inline
bool
  binary_search(ForwardIterator first, ForwardIterator last, const T& value);

template<class ForwardIterator, class T, class Compare>
inline
bool
  binary_search(ForwardIterator first, ForwardIterator last, 
                const T& value, Compare comp);

// 25.3.4, merge:
template<class InputIterator1, class InputIterator2, class OutputIterator>
inline
OutputIterator
  merge(InputIterator1 first1, InputIterator1 last1,
        InputIterator2 first2, InputIterator2 last2,
        OutputIterator result);

template<class InputIterator1, class InputIterator2, class OutputIterator,
         class Compare>
inline
OutputIterator
  merge(InputIterator1 first1, InputIterator1 last1,
        InputIterator2 first2, InputIterator2 last2,
        OutputIterator result, Compare comp);

template<class BidirectionalIterator>
inline
void
  inplace_merge(BidirectionalIterator first, BidirectionalIterator middle,
                BidirectionalIterator last);

template<class BidirectionalIterator, class Compare>
inline
void
  inplace_merge(BidirectionalIterator first, BidirectionalIterator middle,
                BidirectionalIterator last, Compare comp);

// 25.3.5, set operations:
template<class InputIterator1, class InputIterator2>
inline
bool
  includes(InputIterator1 first1, InputIterator1 last1,
           InputIterator2 first2, InputIterator2 last2);

template<class InputIterator1, class InputIterator2, class Compare>
inline
bool
  includes(InputIterator1 first1, InputIterator1 last1,
           InputIterator2 first2, InputIterator2 last2, Compare comp);

template<class InputIterator1, class InputIterator2, class OutputIterator>
inline
OutputIterator
  set_union(InputIterator1 first1, InputIterator1 last1,
            InputIterator2 first2, InputIterator2 last2,
            OutputIterator result);

template<class InputIterator1, class InputIterator2, class OutputIterator,
         class Compare>
inline
OutputIterator
  set_union(InputIterator1 first1, InputIterator1 last1,
            InputIterator2 first2, InputIterator2 last2,
            OutputIterator result, Compare comp);

template<class InputIterator1, class InputIterator2, class OutputIterator>
inline
OutputIterator
  set_intersection(InputIterator1 first1, InputIterator1 last1,
                   InputIterator2 first2, InputIterator2 last2,
                   OutputIterator result);

template<class InputIterator1, class InputIterator2, class OutputIterator,
         class Compare>
inline
OutputIterator
  set_intersection(InputIterator1 first1, InputIterator1 last1,
                   InputIterator2 first2, InputIterator2 last2,
                   OutputIterator result, Compare comp);

template<class InputIterator1, class InputIterator2, class OutputIterator>
inline
OutputIterator
  set_difference(InputIterator1 first1, InputIterator1 last1,
                 InputIterator2 first2, InputIterator2 last2,
                 OutputIterator result);

template<class InputIterator1, class InputIterator2, class OutputIterator,
         class Compare>
inline
OutputIterator
  set_difference(InputIterator1 first1, InputIterator1 last1,
                 InputIterator2 first2, InputIterator2 last2,
                 OutputIterator result, Compare comp);

template<class InputIterator1, class InputIterator2, class OutputIterator>
inline
OutputIterator
  set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
                           InputIterator2 first2, InputIterator2 last2,
                           OutputIterator result);

template<class InputIterator1, class InputIterator2, class OutputIterator,
         class Compare>
inline
OutputIterator
  set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
                           InputIterator2 first2, InputIterator2 last2,
                           OutputIterator result, Compare comp);

// 25.3.6, heap operations:
template<class RandomAccessIterator>
inline
void
  push_heap(RandomAccessIterator first, RandomAccessIterator last);

template<class RandomAccessIterator, class Compare>
inline
void
  push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp);

template<class RandomAccessIterator>
inline
void
  pop_heap(RandomAccessIterator first, RandomAccessIterator last);

template<class RandomAccessIterator, class Compare>
inline
void
  pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp);

template<class RandomAccessIterator>
inline
void
  make_heap(RandomAccessIterator first, RandomAccessIterator last);

template<class RandomAccessIterator, class Compare>
inline
void
  make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp);

template<class RandomAccessIterator>
inline
void
  sort_heap(RandomAccessIterator first, RandomAccessIterator last);

template<class RandomAccessIterator, class Compare>
inline
void
  sort_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp);

/// 25.3.7, minimum and maximum:

template<class T>
__forceinline
const T&
  (min)(const T& a, const T& b)
{
  return ( b < a ) ? b : a;
}

template<class T, class Compare>
__forceinline
const T&
  (min)(const T& a, const T& b, Compare comp)
{
  return comp(b, a) ? b : a;
}

template<class T>
__forceinline
const T&
  max(const T& a, const T& b)
{
  return ( a < b ) ? b : a;
}

template<class T, class Compare>
__forceinline
const T&
  max(const T& a, const T& b, Compare comp)
{
  return comp(a, b) ? b : a;
}

template<class ForwardIterator>
inline
ForwardIterator
  min_element(ForwardIterator first, ForwardIterator last);

template<class ForwardIterator, class Compare>
inline
ForwardIterator
  min_element(ForwardIterator first, ForwardIterator last, Compare comp);

template<class ForwardIterator>
inline
ForwardIterator
  max_element(ForwardIterator first, ForwardIterator last);

template<class ForwardIterator, class Compare>
inline
ForwardIterator
  max_element(ForwardIterator first, ForwardIterator last, Compare comp);

/// 25.3.8 Lexicographical comparison [lib.alg.lex.comparison]
template<class InputIterator1, class InputIterator2>
__forceinline
bool
  lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                          InputIterator2 first2, InputIterator2 last2)
{
  for ( ; first1 != last1; ++first1, ++first2 )
    if ( first2 == last2 )   return false;
    else
    if ( *first1 < *first2 ) return true;
    else
    if ( *first2 < *first1 ) return false;
  return first2 != last2;
}

/// 25.3.8 Lexicographical comparison [lib.alg.lex.comparison]
template<class InputIterator1, class InputIterator2, class Compare>
__forceinline
bool
  lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                          InputIterator2 first2, InputIterator2 last2,
                          Compare comp)
{
  for ( ; first1 != last1; ++first1, ++first2 )
    if ( first2 == last2 )        return false;
    else
    if ( comp(*first1, *first2) ) return true;
    else
    if ( comp(*first2, *first1) ) return false;
  return first2 != last2;
}

// 25.3.9, permutations
template<class BidirectionalIterator>
inline
bool
  next_permutation(BidirectionalIterator first, BidirectionalIterator last);

template<class BidirectionalIterator, class Compare>
inline
bool
  next_permutation(BidirectionalIterator first, BidirectionalIterator last,
                   Compare comp);

template<class BidirectionalIterator>
inline
bool
  prev_permutation(BidirectionalIterator first, BidirectionalIterator last);

template<class BidirectionalIterator, class Compare>
inline
bool
  prev_permutation(BidirectionalIterator first, BidirectionalIterator last,
                   Compare comp);

}//namespace std

#endif//#ifndef NTL__STLX_ALGORITHM

// This file is derived from HP STL and STLPort:
/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Copyright (c) 1997
 * Moscow Center for SPARC Technology
 *
 * Copyright (c) 1999 
 * Boris Fomitchev
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted 
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */
