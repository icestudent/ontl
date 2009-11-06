/**\file*********************************************************************
 *                                                                     \brief
 *  Linked list template
 *
 ****************************************************************************
 */
#ifndef NTL__LINKED_LIST
#define NTL__LINKED_LIST
#pragma once

#ifndef NTL__STLX_ITERATOR
#include "stlx/iterator.hxx"
#endif
#ifndef NTL__STLX_TYPE_TRAITS
#include "stlx/type_traits.hxx"
#endif
#include "stlx/algorithm.hxx"

namespace ntl {

/// The linked data structures (simple graphs) base.
///\note  These structruses represent also native NT lists.
template<size_t Rank = 0>
struct linked /*{}*/;

template<>
struct linked<1>
{
  static const size_t   rank = 1;
  typedef linked<rank>  this_type;

  this_type * next;

  // PushEntryList(ListHead, this);
  void link(this_type * prev)
  {
    this->next = prev->next;
    prev->next = this;
  }

  // PopEntryList(ListHead); however may pop more than one entry
  void unlink(this_type * prev)
  {
    prev->next = this->next;
  }
};

typedef linked<1> single_linked;

template<>
struct linked<2>
{
  static const size_t   rank = 2;
  typedef linked<rank>  this_type;

  ///\warning do not change the order, since this is native OS' type
  this_type * prev, * next;

  void link(this_type * prev, this_type * next)
  {
    this->prev = prev; this->next = next;
    prev->next = this; next->prev = this;
  }

  void link(this_type * prev)
  {
    link(prev, prev->next);
  }

  static void unlink(this_type * prev, this_type * next)
  {
    prev->next = next; next->prev = prev;
  }

  void unlink(this_type * prev)
  {
    unlink(prev, this->next);
  }

  void unlink()
  {
    unlink(this->prev, this->next);
  }

  ///\todo may be better...
  void swap(this_type * other)
  {
    this_type * const this_next = this->next; // the next line may change this
    this->prev->next = other;
    this_next->prev = other;
    this_type * const other_next = other->next; // the next line may change this
    other->prev->next = this;
    other_next->prev = this;
    std::swap(this->prev, other->prev);
    std::swap(this->next, other->next);
  }
};

typedef linked<2> double_linked;


template<template<size_t> class Node, size_t Rank>
struct linked_iterator;

template<template<size_t> class Node>
struct linked_iterator<Node, 1>
: public std::iterator<std::forward_iterator_tag, Node<1> >
{
    typedef Node<1>         node_type;
    typedef linked_iterator this_type;

    linked_iterator() {}
    linked_iterator(node_type * const p) : p(p) {}
    linked_iterator(const linked_iterator& i) : p(i.p) {}

    node_type & operator* () const { return *p; }
    node_type * operator->() const { return &operator*(); }
    this_type & operator++() { p = p->next; return *this; }
    this_type operator++(int) { this_type tmp( *this ); ++*this; return tmp; }

  friend bool operator==(const this_type& x, const this_type& y) { return x.p == y.p; }
  friend bool operator!=(const this_type& x, const this_type& y) { return x.p != y.p; }

  private: node_type  * p;
};

typedef linked_iterator<linked, 1>        single_linked_iterator;
#if !defined(__ICL) && !defined(__GNUC__)
typedef linked_iterator<const linked, 1>  const_single_linked_iterator;
#else
typedef linked_iterator<linked, 1>        const_single_linked_iterator;
#endif

template<template<size_t> class Node>
struct linked_iterator<Node, 2>
: public std::iterator<std::bidirectional_iterator_tag, Node<2> >
{
    typedef Node<2>         node_type;
    typedef linked_iterator this_type;

    linked_iterator() /*: p(0)*/ {}
    linked_iterator(node_type * const p) : p(p) {}
    linked_iterator(const linked_iterator& i) : p(i.p) {}

    node_type & operator* () const { return *p; }
    node_type * operator->() const { return &operator*(); }
    this_type & operator++() { p = p->next; return *this; }
    this_type & operator--() { p = p->prev; return *this; }
    this_type operator++(int) { this_type tmp( *this ); ++*this; return tmp; }
    this_type operator--(int) { this_type tmp( *this ); --*this; return tmp; }

  friend bool operator==(const this_type& x, const this_type& y) { return x.p == y.p; }
  friend bool operator!=(const this_type& x, const this_type& y) { return x.p != y.p; }

  private: node_type  * p;
};

typedef linked_iterator<linked, 2>        double_linked_iterator;
#if !defined(__ICL) && !defined(__GNUC__)
typedef linked_iterator<const linked, 2>  const_double_linked_iterator;
#else
typedef linked_iterator<linked, 2>        const_double_linked_iterator;
#endif

template< typename ContainingType,
          typename MemberType,
          MemberType ContainingType:: * Member>
struct containing_adaptor
{
    typedef containing_adaptor  this_type;
    typedef ContainingType      containing_type;
    typedef MemberType          member_type;

    containing_adaptor(const member_type * m) : m(m) {}

    template<typename Iterator>
    containing_adaptor(const Iterator & it) : m(&*it) {}

    containing_type * operator->() const
    {
      containing_type * const pc = 0;
      member_type * const pf = &(pc->*Member);
      return reinterpret_cast<containing_type*>(uintptr_t(pc)+uintptr_t(m)-uintptr_t(pf));
    }

  ///\todo  unless this is declared in the containing namecpace
  ///       the containing_adaptor tempalte should be instantiated
  template<member_type containing_type:: * M>
  friend
    containing_adaptor<containing_type, member_type, M>
      containing(const member_type* mt) { return containing_adaptor(mt); }

  private:

    const member_type * m;
};

}//namespace ntl

#endif//#ifndef NTL__LINKED_LIST
