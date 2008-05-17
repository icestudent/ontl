#pragma once

#include <iterator>
#include <memory>
#include <functional>

namespace tree
{
  namespace rbtree
  {

    template<class T, class Compare = std::less<T>, class Allocator = std::allocator<T> >
    class rbtree
    {
    public:
      typedef           T                           value_type;
      typedef           Compare                     value_compare;
      typedef           Allocator                   allocator_type;
      typedef typename  Allocator::pointer          pointer;
      typedef typename  Allocator::const_pointer    const_pointer;
      typedef typename  Allocator::reference        reference;
      typedef typename  Allocator::const_reference  const_reference;
      typedef typename  Allocator::size_type        size_type;
      typedef typename  Allocator::difference_type  difference_type;

    protected:
      struct node
      {
        enum color_type { black, red };

        union 
        {
          struct 
          {
            node *left, *right;
          };
          node* link[2];
        };
        node* parent;
        T elem;
        int8_t color;

        explicit node(const T elem, node* parent, node* left = NULL, node* right = NULL)
          :elem(elem), parent(parent), left(left), right(right), 
          color(black)
        {}
        node(const T elem)
          :elem(elem), 
          parent(NULL), left(NULL), right(NULL),
          color(red)
        {}
        node(const node& n)
          :elem(n.elem)
        {}
      };

      typedef typename rbtree<T, Compare, Allocator>::node node_type;

      struct iterator_impl:
        std::iterator<std::bidirectional_iterator_tag, value_type, difference_type, pointer, reference>
      {
#ifdef _DEBUG
        iterator_impl()
          :p(), tree_()
        {}
#else
        iterator_impl(){}
#endif
        //iterator_impl(const iterator_impl& i)
        //  :p(i.p), tree_(i.tree_) {}

        reference operator* () const { return p->elem; }
        pointer   operator->() const { return &operator*(); }
        iterator_impl& operator++()  { p = tree_->next(p, right ); return *this; }
        iterator_impl& operator--()  { p = tree_->next(p, left  ); return *this; }
        iterator_impl operator++(int){ iterator_impl tmp( *this ); ++*this; return tmp; }
        iterator_impl operator--(int){ iterator_impl tmp( *this ); --*this; return tmp; }

        friend bool operator==(const iterator_impl& x, const iterator_impl& y)
        { return x.p == y.p; }
        friend bool operator!=(const iterator_impl& x, const iterator_impl& y)
        { return x.p != y.p; }

      private:
        node_type* p;
        rbtree<T, Compare, Allocator>* tree_;

        friend struct const_iterator_impl;
        friend class rbtree<T,Compare, Allocator>;
        iterator_impl(node_type* /*const*/ p, rbtree<T, Compare, Allocator>* tree)
          :p(p), tree_(tree)
        {}

      };

      struct const_iterator_impl:
        std::iterator<std::bidirectional_iterator_tag, value_type, difference_type, const_pointer, const_reference>
      {
#ifdef _DEBUG
        const_iterator_impl()
          :p(), tree_()
        {}
#else
        const_iterator_impl(){}
#endif
        const_iterator_impl(const iterator_impl& i)
          :p(i.p)
        {}
        const_iterator_impl(const const_iterator_impl& i)
          :p(i.p) {}

        reference operator* () const { return p->elem; }
        pointer   operator->() const { return &operator*(); }

        const_iterator_impl& operator++()  { p = tree_->next(p, right); return *this; }
        const_iterator_impl& operator--()  { p = tree_->next(p, left ); return *this; }
        const_iterator_impl operator++(int){ const_iterator_impl tmp( *this ); ++*this; return tmp; }
        const_iterator_impl operator--(int){ const_iterator_impl tmp( *this ); --*this; return tmp; }

        friend bool operator==(const const_iterator_impl& x, const const_iterator_impl& y)
        { return x.p == y.p; }
        friend bool operator!=(const const_iterator_impl& x, const const_iterator_impl& y)
        { return x.p != y.p; }

      private:
        const node_type* p;
        const rbtree<T, Compare, Allocator>* tree_;

        const_iterator_impl(const node_type* const p, const rbtree<T, Compare, Allocator>* tree)
          :p(p), tree_(tree)
        {}

        friend class rbtree;
      };

    public:
      typedef iterator_impl                         iterator;
      typedef const_iterator_impl                   const_iterator;
      typedef std::reverse_iterator<iterator>       reverse_iterator;
      typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    public:
      explicit rbtree(const Compare& comp = Compare(), const Allocator& a = Allocator())
        :comparator_(comp), node_allocator(a),
        root_(), count_(0)
      {}

      template<class InputIterator>
      rbtree(InputIterator first, InputIterator last, 
        const Compare& comp = Compare(), const Allocator& a = Allocator())
        :comparator_(comp), node_allocator(a),
        root_(), count_()
      {
        insert(first, last);
      }

      rbtree(const Allocator& a);
      
      rbtree(const rbtree<T, Compare, Allocator>& x)
        :root_(x.root_), count_(x.count_), node_allocator(x.node_allocator), comparator_(x.comparator_)
      {}
      
      rbtree(const rbtree& x, const Allocator& a)
        :root_(x.root_), count_(x.count_), node_allocator(a), comparator_(x.comparator_)
      {}

      rbtree& operator=(const rbtree<T, Compare, Allocator>& x)
      {
        if(this == &x)
          return *this;

        clear();
        insert(x.cbegin(), x.cend());
        return *this;
      }

      ~rbtree() throw()
      {
        clear();
      }

      allocator_type get_allocator() const { return node_allocator; }

    public:
      // capacity
      bool      empty() const     { return count_ == 0; }
      size_type size()  const     { return count_; }
      size_type max_size()  const { return node_allocator.max_size(); }

      // iterators
      iterator                begin()        { return iterator(first(), this); }
      const_iterator          begin()  const { return const_iterator(first(), this); }
      iterator                end()          { return iterator(NULL, this);  }
      const_iterator          end()    const { return const_iterator(NULL, this);  }

      reverse_iterator        rbegin()       { return reverse_iterator(iterator(NULL, this));  }
      const_reverse_iterator  rbegin() const { return const_reverse_iterator(const_iterator(NULL, this));  }
      reverse_iterator        rend()         { return reverse_iterator(iterator(first(), this)); }
      const_reverse_iterator  rend()   const { return const_reverse_iterator(const_iterator(first(), this)); }

      const_iterator          cbegin() const { return begin(); }
      const_iterator          cend()   const { return end();   }
      const_reverse_iterator  crbegin()const { return rbegin();}
      const_reverse_iterator  crend()  const { return rend();  }

      // access
      iterator find(const value_type& x)
      {
        node* p = root_;
        while(p){
          if(elem_less(x, p->elem))
            p = p->left;
          else if(elem_greater(x, p->elem))
            p = p->right;
          else
            return iterator(p, this);
        }
        return end();
      }

      const_iterator find(const value_type& x) const { return find(x); }

      // modifiers
      pair<iterator, bool> insert(const value_type& x)
      {
        if(count_ == max_size()-1)
          __ntl_throw(std::length_error("rbtree<T> too long"));

        if(empty()){
          // insert x as the root node
          root_ = node_allocator.allocate(2);
          root_->parent = NULL;
          root_->left = root_->right = root_+1; root_->color = node::red;
          node_allocator.construct(++root_, x);
          root_->color = node::black;
          ++count_;
          return make_pair(iterator(root_, this), true);
        }

        bool greater;
        node *q = NULL;
        for(node* p = root_; p != NULL; q = p, p = p->link[ greater ]){
          greater = comparator_(p->elem, x);
          if(!greater && !elem_less(x, p->elem))
            // equal
            return make_pair(iterator(p, this), false);
        }

        // create node
        node_type* const p = node_allocator.allocate(1);
        node_allocator.construct(p, x);
        p->parent = q;

        // link
        q->link[greater] = p;
        if(q == first() && !greater)
          head()->left = p;
        else if(q == last() && greater)
          head()->right= p;

        ++count_;

        // balance tree
        fixup_insert(p);
        return make_pair(iterator(p, this), true);
      }

      iterator insert(iterator /*position*/, const value_type& x)
      {
        // TODO: implement fast insert function based on position
        return insert(x)->first;
      }

      template<class InputIterator>
      void insert(InputIterator first, InputIterator last)
      {
        while(first != last){
          insert(*first);
          ++first;
        }
      }
      iterator erase(iterator position)
      {
        node *y, *z = position.p;
        if(!z)
          return position;

        ++position;

        // check limits
        if(z == first()){
          head()->left = next(z, right);
        }else if(z == last()){
          head()->right= next(z, left);
        }

        if(z->left == NULL || z->right == NULL){
          y = z;
        }else{
          // find tree successor with a NULL node as a child
          y = z->right;
          while(y->left)
            y = y->left;
        }
        node* x = y->link[ y->left == NULL ];
        // remove y from the parent chain
        node* prev_root = NULL;
        if(x)
          x->parent = y->parent;
        if(y->parent)
          y->parent->link[ y != y->parent->left ] = x;
        else{
          prev_root = root_;
          root_ = make_root(x);
        }

        if(y != z){
          // copy y to z?
          *z = *y;
        }
        if(y->color == node::black && x){
          if(!prev_root)
            fixup_delete(x);
          else
            x->color = node::black;
        }
        node_allocator.destroy(y);
        if(!prev_root)
          node_allocator.deallocate(y, 1);
        else{
          //if(count_ != 1)
            --prev_root;
          node_allocator.deallocate(prev_root, 2);
        }
        if(count_)
          --count_;
        return position;
      }

      iterator erase(iterator first, iterator last)
      {
        while(first != last)
          first = erase(first);

        return empty() ? end() : last;
      }

      size_type erase(const value_type& x)
      {
        if(empty())
          return 0;
        iterator pos = find(x);
        return pos == end() ? 0 : (erase(pos), 1);
      }

      void swap(rbtree<T, Compare, Allocator>& tree)
      {
        if(this != &tree){
          //if(node_allocator == tree.node_allocator){
          std::swap(root_, tree.root_);
          std::swap(count_, tree.count_);
          std::swap(comparator_, tree.comparator_);
          std::swap(node_allocator, tree.node_allocator);
        }
      }

      void clear()
      {
        if(!empty())
          erase(begin(), end());
      }

      // observes
      value_compare value_comp() const { return comparator_; }

    protected:
      // left: most less, right: most greater
      node* head() const throw() { return root_-1; }

      node* make_root(node* from)
      {
        if(!from)
          return NULL;

        // create head
        node* h = node_allocator.allocate(2);
        *h++ = *head();
        // copy data to the new root
        node_allocator.construct(h, from->elem);
        h->left = from->left; h->right = from->right;
        h->color = from->color;
        return h;
      }

      node* end(bool direction) const throw()
      {
        if(!root_)
          return NULL;
        node* p = root_;
        while(p->link[direction])
          p = p->link[direction];
        return p;
      }

      node* next(node* from, bool direction) const throw()
      {
        // if --begin() || ++end(), do nothing;
        // if --end(), return last.
        if(!direction && from == first())
          return from;
        else if(!from){
          if(direction)
            return from;
          else
            return last();
        }

        const bool indirection = !direction;
        if(from->link[direction] == NULL){
          for(node *p = from, *q = p->parent; ; p = q, q = q->parent)
            if(q == NULL || p == q->link[indirection])
              return q;
        }else{
          from = from->link[direction];
          while(from->link[indirection])
            from = from->link[indirection];
          return from;
        }
      }

      const node* next(const node* from, bool direction) const throw()
      {
        return next(const_cast<node*>(from), direction);
      }

      node* first() const throw()
      {
        return root_ ? head()->left : NULL;
      }

      node* last() const throw()
      {
        return root_ ? head()->right: NULL;
      }

      void rotate(node* x, bool direction) throw()
      {
        const bool right = direction, left = !direction;
        node* y = x->link[right];

        // right x link
        x->link[right] = y->link[left];
        if(y->link[left])
          y->link[left]->parent = x;

        // parent y link
        y->parent = x->parent;
        if(x->parent)
          x->parent->link[ x != x->parent->link[left] ] = y;
        else
          root_ = y;

        // x-y
        y->link[left] = x;
        if(x)
          x->parent = y;
      }

      node* fixup_insert(node* x, bool direction) throw()
      {
        const bool right = direction, left = !direction;

        node* y = x->parent->parent->link[right];
        if(y && y->color == node::red){
          // uncle is red
          x->parent->color = node::black;
          y->color = node::black;
          x->parent->parent->color = node::red;
          x = x->parent->parent;
        }else{
          // uncle is black
          if(x == x->parent->link[right]){
            x = x->parent;
            rotate(x, left);
          }
          // recolor and rotate
          x->parent->color = node::black;
          x->parent->parent->color = node::red;
          rotate(x->parent->parent, right);
        }
        return x;
      }

      void fixup_delete(node* x, bool direction) throw()
      {
        const bool right = direction, left = !direction;

        node* w = x->parent->link[right];
        if(w->color == node::red){
          w->color = node::black;
          x->parent->color = node::red;
          rotate(x->parent, left);
          w = x->parent->link[right];
        }
        if(w->link[left]->color == node::black && w->link[right]->color == node::black){
          w->color = node::red;
          x = x->parent;
        }else{
          if(w->link[right]->color = node::black){
            w->link[left]->color = node::black;
            w->color = node::red;
            rotate(w, right);
            w = x->parent->link[right];
          }
          w->color = x->parent->color;
          x->parent->color = node::black;
          w->link[right]->color = node::black;
          rotate(x->parent, left);
          x = root_;
        }
      }

      void fixup_insert(node* x) throw()
      {
        while(x != root_ && x->parent->color == node::red){
          x = fixup_insert(x, x->parent != x->parent->parent->left);
        }
        root_->color = node::black;
      }

      void fixup_delete(node* x) throw()
      {
        if(!x) return;
        while(x != root_ && x->color == node::black){
          fixup_delete(x, x != x->parent->left);
        }
        x->color = node::black;
      }

      bool elem_less(const T& x, const T& y) const
      {
        return comparator_(x, y);
      }
      bool elem_equal(const T& x, const T& y) const
      {
        return !node_less(x, y) && !node_greater(x, y);
      }
      bool elem_greater(const T& x, const T& y) const
      {
        return comparator_(y, x);
      }
    private:
      typename allocator_type::template rebind<node_type>::other node_allocator;
      value_compare comparator_;

      node* root_;
      size_type count_;

      static const bool left = false, right = true;
    };

    template<class T, class Compare, class Allocator>
    bool operator == (const rbtree<T, Compare, Allocator>& x, const rbtree<T, Compare, Allocator>& y)
    {
      return x.size() == y.size() && equal(x.cbegin(), x.cend(), y.cbegin());
    }

    template<class T, class Compare, class Allocator>
    bool operator != (const rbtree<T, Compare, Allocator>& x, const rbtree<T, Compare, Allocator>& y)
    {
      return !(x == y);
    }

    template<class T, class Compare, class Allocator>
    bool operator < (const rbtree<T, Compare, Allocator>& x, const rbtree<T, Compare, Allocator>& y)
    {
      return lexicographical_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend());
    }

    template<class T, class Compare, class Allocator>
    bool operator > (const rbtree<T, Compare, Allocator>& x, const rbtree<T, Compare, Allocator>& y)
    {
      return y < x;
    }

    template<class T, class Compare, class Allocator>
    bool operator <= (const rbtree<T, Compare, Allocator>& x, const rbtree<T, Compare, Allocator>& y)
    {
      return !(y < x);
    }

    template<class T, class Compare, class Allocator>
    bool operator >= (const rbtree<T, Compare, Allocator>& x, const rbtree<T, Compare, Allocator>& y)
    {
      return !(x < y);
    }

    // specialized algorithms
    template<class T, class Compare, class Allocator>
    void swap(rbtree<T, Compare, Allocator>& x, rbtree<T, Compare, Allocator>& y)
    {
      x.swap(y);
    }

  } // rbtree
} // tree