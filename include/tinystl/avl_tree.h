/// AVL Tree实现参考avlmini： https://github.com/skywind3000/avlmini
/// avlmini使用MIT许可证：https://github.com/skywind3000/avlmini/blob/master/LICENSE
///
/// 非侵入式AVL Tree，实现中没有使用任何堆内存分配，仅使用栈内存。
/// 因此需要在插入前给节点手动分配存储空间，删除节点后需要手动释放节点的存储空间。
///
/// 使用方法如下：
///
/// ```cpp
/// class MyClass : public tinystl::avl_node {
///   Implement MyClass here.
/// };
///
/// tinystl::avl_tree<MyClass> tree;
/// ```
///
/// 注意，avl_node不是虚基类，因此不要将类型转换成avl_node后析构，这会导致析构出错。
///

#ifndef TINYSTL_AVL_TREE_H
#define TINYSTL_AVL_TREE_H

#include "compressed_pair.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iterator>
#include <type_traits>

namespace tinystl {

template <class T, class Compare> class avl_tree;

class avl_node {
public:
  using size_type     = size_t;
  using pointer       = avl_node *;
  using const_pointer = const avl_node *;

  constexpr avl_node() noexcept = default;

  bool is_left() const noexcept;
  bool is_right() const noexcept;

  pointer   parent() const noexcept { return mParent; }
  pointer   left() const noexcept { return mLeft; }
  pointer   right() const noexcept { return mRight; }
  size_type height() const noexcept { return mHeight; }

  pointer       next() noexcept;
  pointer       prev() noexcept;
  const_pointer next() const noexcept;
  const_pointer prev() const noexcept;

  template <class T, class Compare> friend class avl_tree;

protected:
  // avl_node is NOT a virtual class.
  // DO NOT cast to avl_node before destructing.
  ~avl_node() = default;

private:
  void update_height() noexcept {
    mHeight = std::max(left() ? left()->height() : size_type(0),
                       right() ? right()->height() : size_type(0)) +
              1;
  }

  template <class Impl, class Compare>
  void replace_as_child(pointer                  node,
                        pointer                  parent,
                        avl_tree<Impl, Compare> &tree) noexcept;

  template <class Impl, class Compare>
  void replace(pointer node, avl_tree<Impl, Compare> &tree) noexcept;

  template <class Impl, class Compare>
  pointer rotate_left(avl_tree<Impl, Compare> &tree) noexcept;

  template <class Impl, class Compare>
  pointer rotate_right(avl_tree<Impl, Compare> &tree) noexcept;

  template <class Impl, class Compare>
  pointer fix_left(avl_tree<Impl, Compare> &tree) noexcept;

  template <class Impl, class Compare>
  pointer fix_right(avl_tree<Impl, Compare> &tree) noexcept;

  template <class Impl, class Compare>
  void rebalance(avl_tree<Impl, Compare> &tree) noexcept;

  template <class Impl, class Compare>
  void fix_insert(avl_tree<Impl, Compare> &tree) noexcept;

private:
  avl_node *mParent = nullptr;
  avl_node *mLeft   = nullptr;
  avl_node *mRight  = nullptr;
  size_type mHeight = 0;
};

template <class T, class Compare> class avl_tree_iterator {
public:
  using value_type        = T;
  using reference         = value_type &;
  using const_reference   = const value_type &;
  using pointer           = T *;
  using const_pointer     = const T *;
  using difference_type   = std::ptrdiff_t;
  using iterator_category = std::bidirectional_iterator_tag;

  constexpr avl_tree_iterator(avl_tree<T, Compare> *tree = nullptr,
                              avl_node             *node = nullptr) noexcept
      : mTree(tree), mPtr(node) {}

  constexpr avl_tree_iterator(const avl_tree_iterator &) noexcept = default;

  avl_tree_iterator &operator++() noexcept {
    if (mPtr != nullptr) {
      mPtr = mPtr->next();
    }
    return (*this);
  }

  avl_tree_iterator operator++(int) noexcept {
    avl_tree_iterator ret = (*this);
    ++(*this);
    return ret;
  }

  avl_tree_iterator &operator--() noexcept {
    if (mPtr != nullptr) {
      mPtr = mPtr->prev();
    } else {
      mPtr = static_cast<avl_node *>(mTree->root());

      if (mPtr == nullptr)
        return (*this);

      while (mPtr->right() != nullptr)
        mPtr = mPtr->right();
    }
    return (*this);
  }

  avl_tree_iterator operator--(int) noexcept {
    avl_tree_iterator ret = (*this);
    --(*this);
    return ret;
  }

  reference       operator*() noexcept { return *static_cast<pointer>(mPtr); }
  const_reference operator*() const noexcept {
    return *static_cast<const_pointer>(mPtr);
  }

  reference       operator->() noexcept { return *static_cast<pointer>(mPtr); }
  const_reference operator->() const noexcept {
    return *static_cast<const_pointer>(mPtr);
  }

  constexpr bool operator==(const avl_tree_iterator rhs) const noexcept {
    return (mTree == rhs.mTree && mPtr == rhs.mPtr);
  }

  constexpr bool operator!=(const avl_tree_iterator rhs) const noexcept {
    return !((*this) == rhs);
  }

  pointer get() const noexcept { return static_cast<pointer>(mPtr); }

  friend class avl_tree<T, Compare>;

private:
  avl_tree<T, Compare> *mTree = nullptr;
  avl_node             *mPtr  = nullptr;
};

template <class T, class Compare> class avl_tree_const_iterator {
public:
  using value_type        = const T;
  using reference         = const value_type &;
  using const_reference   = const value_type &;
  using pointer           = const T *;
  using const_pointer     = const T *;
  using difference_type   = std::ptrdiff_t;
  using iterator_category = std::bidirectional_iterator_tag;

  constexpr avl_tree_const_iterator(const avl_tree<T, Compare> *tree = nullptr,
                                    const avl_node *node = nullptr) noexcept
      : mTree(tree), mPtr(node) {}

  constexpr avl_tree_const_iterator(const avl_tree_const_iterator &) noexcept =
      default;

  avl_tree_const_iterator &operator++() noexcept {
    if (mPtr != nullptr) {
      mPtr = mPtr->next();
    }
    return (*this);
  }

  avl_tree_const_iterator operator++(int) noexcept {
    avl_tree_const_iterator ret = (*this);
    ++(*this);
    return ret;
  }

  avl_tree_const_iterator &operator--() noexcept {
    if (mPtr != nullptr) {
      mPtr = mPtr->prev();
    } else {
      mPtr = static_cast<avl_node *>(mTree->root());

      if (mPtr == nullptr)
        return (*this);

      while (mPtr->right() != nullptr)
        mPtr = mPtr->right();
    }
    return (*this);
  }

  avl_tree_const_iterator operator--(int) noexcept {
    avl_tree_const_iterator ret = (*this);
    --(*this);
    return ret;
  }

  reference       operator*() noexcept { return *static_cast<pointer>(mPtr); }
  const_reference operator*() const noexcept {
    return *static_cast<const_pointer>(mPtr);
  }

  reference       operator->() noexcept { return *static_cast<pointer>(mPtr); }
  const_reference operator->() const noexcept {
    return *static_cast<const_pointer>(mPtr);
  }

  constexpr bool operator==(const avl_tree_const_iterator rhs) const noexcept {
    return (mTree == rhs.mTree && mPtr == rhs.mPtr);
  }

  constexpr bool operator!=(const avl_tree_const_iterator rhs) const noexcept {
    return !((*this) == rhs);
  }

  const_pointer get() const noexcept {
    return static_cast<const_pointer>(mPtr);
  }

  friend class avl_tree<T, Compare>;

private:
  const avl_tree<T, Compare> *mTree = nullptr;
  const avl_node             *mPtr  = nullptr;
};

template <class T, class Compare = std::less<T>> class avl_tree {
public:
  using key_type        = T;
  using value_type      = T;
  using reference       = value_type &;
  using const_reference = const value_type &;
  using size_type       = size_t;
  using difference_type = ptrdiff_t;
  using key_compare     = Compare;
  using value_compare   = Compare;
  using pointer         = value_type *;
  using const_pointer   = const value_type *;
  using iterator        = avl_tree_iterator<T, Compare>;
  using const_iterator  = avl_tree_const_iterator<T, Compare>;

  static_assert(std::is_base_of<avl_node, T>::value,
                "T should inhert from avl_node_meta.");

  avl_tree() noexcept(std::is_nothrow_default_constructible<Compare>::value)
      : mValue(nullptr, Compare()) {}

  explicit avl_tree(const Compare &cmp) noexcept(
      std::is_nothrow_copy_constructible<Compare>::value)
      : mValue(nullptr, cmp) {}

  avl_tree(const avl_tree &other) = default;
  avl_tree &operator=(const avl_tree &other) = default;

  bool      empty() const noexcept { return mSize; }
  size_type size() const noexcept { return mSize; }

  pointer root() noexcept { return static_cast<pointer>(mValue.first()); }

  const_pointer root() const noexcept {
    return static_cast<const_pointer>(mValue.first());
  }

  iterator begin() noexcept;
  iterator end() noexcept;

  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;

  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

  reference front() noexcept;
  reference back() noexcept;

  const_reference front() const noexcept;
  const_reference back() const noexcept;

  /// Return false if there is already a node equal to current one.
  bool insert_unique(pointer node) noexcept;

  /// Return pointer to the victim if replace happend. Otherwise, return
  /// nullptr.
  pointer insert_or_replace(pointer node) noexcept;

  void insert_multi(pointer node) noexcept;

  /// Make sure that node belongs to current tree.
  void erase(pointer node) noexcept;

  void erase(iterator node) noexcept {
    assert(node.mTree == this);
    assert(node.mPtr != nullptr);
    erase(node.mPtr);
  }

  template <class Func> void clear(Func &&handler);

  pointer       find(const_reference value) noexcept;
  const_pointer find(const_reference value) const noexcept;

  /// Find a node according to custom cmp function and a custom value.
  /// cmp should match the following sign:
  /// - cmp(const Value &, const_reference) -> int
  /// - cmp return value:
  ///   - negative integer: value is smaller than current node.
  ///   - positive integer: value is greater than current node.
  ///   - 0: value is equal to current node.
  template <class Value, class Fn>
  pointer find(Fn &&cmp, Value &&value) noexcept;

  /// Find a node according to custom cmp function and a custom value.
  /// cmp should match the following sign:
  /// - cmp(const Value &, const_reference) -> int
  /// - return value:
  ///   - negative integer: value is smaller than current node.
  ///   - positive integer: value is greater than current node.
  ///   - 0: value is equal to current node.
  template <class Value, class Fn>
  const_pointer find(Fn &&cmp, Value &&value) const noexcept;

  key_compare   key_comp() const noexcept { return mValue.second(); }
  value_compare value_comp() const noexcept { return mValue.second(); }

  friend class avl_node;

private:
  template <class Func> void clear_impl(avl_node *node, Func &handler);

private:
  size_type                            mSize = 0;
  compressed_pair<avl_node *, Compare> mValue;
};

inline bool avl_node::is_left() const noexcept {
  if (parent() == nullptr)
    return false;
  return (parent()->left() == this);
}

inline bool avl_node::is_right() const noexcept {
  if (parent() == nullptr)
    return false;
  return (parent()->right() == this);
}

inline auto avl_node::next() const noexcept -> const_pointer {
  if (right() != nullptr) {
    pointer node = right();

    while (node->left() != nullptr)
      node = node->left();

    return node;
  } else {
    const_pointer node = this;
    for (;;) {
      const_pointer last = node;
      node               = node->parent();

      if (node == nullptr)
        break;

      if (node->left() == last)
        break;
    }
    return node;
  }
}

inline auto avl_node::next() noexcept -> pointer {
  if (right() != nullptr) {
    pointer node = right();

    while (node->left() != nullptr)
      node = node->left();

    return node;
  } else {
    pointer node = this;
    for (;;) {
      pointer last = node;
      node         = node->parent();

      if (node == nullptr)
        break;

      if (node->left() == last)
        break;
    }
    return node;
  }
}

inline auto avl_node::prev() const noexcept -> const_pointer {
  if (left() != nullptr) {
    pointer node = left();

    while (node->right() != nullptr)
      node = node->right();

    return node;
  } else {
    const_pointer node = this;

    for (;;) {
      const_pointer last = node;
      node               = node->parent();

      if (node == nullptr)
        break;

      if (node->right() == last)
        break;
    }
    return node;
  }
}

inline auto avl_node::prev() noexcept -> pointer {
  if (left() != nullptr) {
    pointer node = left();

    while (node->right() != nullptr)
      node = node->right();

    return node;
  } else {
    pointer node = this;

    for (;;) {
      pointer last = node;
      node         = node->parent();

      if (node == nullptr)
        break;

      if (node->right() == last)
        break;
    }
    return node;
  }
}

template <class T, class Compare>
void avl_node::replace_as_child(pointer               node,
                                pointer               parent,
                                avl_tree<T, Compare> &tree) noexcept {
  if (parent != nullptr) {
    if (parent->left() == this)
      parent->mLeft = node;
    else
      parent->mRight = node;
  } else {
    tree.mValue.first() = node;
  }
}

template <class T, class Compare>
void avl_node::replace(pointer node, avl_tree<T, Compare> &tree) noexcept {
  replace_as_child(node, parent(), tree);

  if (left() != nullptr)
    left()->mParent = node;

  if (right() != nullptr)
    right()->mParent = node;

  node->mLeft   = left();
  node->mRight  = right();
  node->mParent = parent();
  node->mHeight = height();
}

template <class T, class Compare>
auto avl_node::rotate_left(avl_tree<T, Compare> &tree) noexcept -> pointer {
  assert(right() != nullptr);

  pointer r   = right();
  pointer par = parent();

  mRight = r->left();
  if (right() != nullptr) {
    right()->mParent = this;
  }

  r->mLeft   = this;
  r->mParent = par;

  replace_as_child(r, par, tree);

  mParent = r;
  return r;
}

template <class T, class Compare>
auto avl_node::rotate_right(avl_tree<T, Compare> &tree) noexcept -> pointer {
  assert(left() != nullptr);

  pointer l   = left();
  pointer par = parent();

  mLeft = l->right();
  if (l->right() != nullptr)
    l->right()->mParent = this;

  l->mRight  = this;
  l->mParent = par;

  replace_as_child(l, par, tree);

  mParent = l;
  return l;
}

template <class T, class Compare>
auto avl_node::fix_left(avl_tree<T, Compare> &tree) noexcept -> pointer {
  pointer r = right();
  assert(r);
  size_type rh0 = (r->left() ? r->left()->height() : 0);
  size_type rh1 = (r->right() ? r->right()->height() : 0);

  if (rh0 > rh1) {
    r = r->rotate_right(tree);
    r->right()->update_height();
    r->update_height();
  }
  pointer node = this->rotate_left(tree);
  node->left()->update_height();
  node->update_height();
  return node;
}

template <class T, class Compare>
auto avl_node::fix_right(avl_tree<T, Compare> &tree) noexcept -> pointer {
  pointer l = left();
  assert(l);
  size_type rh0 = (l->left() ? l->left()->height() : 0);
  size_type rh1 = (l->right() ? l->right()->height() : 0);

  if (rh0 < rh1) {
    l = l->rotate_left(tree);
    l->left()->update_height();
    l->update_height();
  }
  pointer node = this->rotate_right(tree);
  node->right()->update_height();
  node->update_height();
  return node;
}

template <class T, class Compare>
void avl_node::rebalance(avl_tree<T, Compare> &tree) noexcept {
  for (pointer node = this; node != nullptr; node = node->parent()) {
    pointer   l      = node->left();
    pointer   r      = node->right();
    size_type hl     = (l == nullptr) ? 0 : l->height();
    size_type hr     = (r == nullptr) ? 0 : r->height();
    size_type height = std::max(hl, hr) + 1;

    if (node->height() == height)
      break;

    node->mHeight = height;
    auto diff     = static_cast<int32_t>(hl) - static_cast<int32_t>(hr);

    if (diff <= -2) {
      node = node->fix_left(tree);
    } else if (diff >= 2) {
      node = node->fix_right(tree);
    }
  }
}

template <class T, class Compare>
void avl_node::fix_insert(avl_tree<T, Compare> &tree) noexcept {
  mLeft = mRight = nullptr;
  mHeight        = 1;
  if (parent())
    parent()->rebalance(tree);
}

template <class T, class Compare>
auto avl_tree<T, Compare>::begin() noexcept -> iterator {
  avl_node *node = mValue.first();

  if (node == nullptr)
    return nullptr;

  while (node->left() != nullptr)
    node = node->left();

  return iterator(this, node);
}

template <class T, class Compare>
auto avl_tree<T, Compare>::begin() const noexcept -> const_iterator {
  avl_node *node = mValue.first();

  if (node == nullptr)
    return nullptr;

  while (node->left() != nullptr)
    node = node->left();

  return const_iterator(this, node);
}

template <class T, class Compare>
auto avl_tree<T, Compare>::end() noexcept -> iterator {
  return iterator(this, nullptr);
}

template <class T, class Compare>
auto avl_tree<T, Compare>::end() const noexcept -> const_iterator {
  return const_iterator(this, nullptr);
}

template <class T, class Compare>
auto avl_tree<T, Compare>::front() noexcept -> reference {
  avl_node *node = mValue.first();

  if (node == nullptr)
    return nullptr;

  while (node->left() != nullptr)
    node = node->left();

  return *static_cast<pointer>(node);
}

template <class T, class Compare>
auto avl_tree<T, Compare>::front() const noexcept -> const_reference {
  avl_node *node = mValue.first();

  if (node == nullptr)
    return nullptr;

  while (node->left() != nullptr)
    node = node->left();

  return *static_cast<pointer>(node);
}

template <class T, class Compare>
auto avl_tree<T, Compare>::back() noexcept -> reference {
  avl_node *node = mValue.first();

  if (node == nullptr)
    return nullptr;

  while (node->right() != nullptr)
    node = node->right();

  return *static_cast<pointer>(node);
}

template <class T, class Compare>
auto avl_tree<T, Compare>::back() const noexcept -> const_reference {
  avl_node *node = mValue.first();

  if (node == nullptr)
    return nullptr;

  while (node->right() != nullptr)
    node = node->right();

  return *static_cast<pointer>(node);
}

template <class T, class Compare>
bool avl_tree<T, Compare>::insert_unique(pointer obj) noexcept {
  auto node    = static_cast<avl_node *>(obj);
  auto current = static_cast<avl_node *>(root());
  if (current == nullptr) {
    mValue.first() = node;
    node->mParent = node->mLeft = node->mRight = nullptr;
    node->mHeight                              = 1;
    mSize += 1;
    return true;
  }

  for (;;) {
    if (value_comp()(*static_cast<pointer>(node),
                     *static_cast<pointer>(current))) {
      if (current->left() != nullptr) {
        current = current->left();
      } else {
        current->mLeft = node;
        node->mParent  = current;
        node->fix_insert(*this);
        mSize += 1;
        return true;
      }
    } else if (value_comp()(*static_cast<pointer>(current),
                            *static_cast<pointer>(node))) {
      if (current->right() != nullptr) {
        current = current->right();
      } else {
        current->mRight = node;
        node->mParent   = current;
        node->fix_insert(*this);
        mSize += 1;
        return true;
      }
    } else {
      // Already exists
      return false;
    }
  }
}

template <class T, class Compare>
auto avl_tree<T, Compare>::insert_or_replace(pointer obj) noexcept -> pointer {
  auto node    = static_cast<avl_node *>(obj);
  auto current = static_cast<avl_node *>(root());
  if (current == nullptr) {
    mValue.first() = node;
    node->mParent = node->mLeft = node->mRight = nullptr;
    node->mHeight                              = 1;
    return nullptr;
  }

  for (;;) {
    if (value_comp()(*static_cast<pointer>(current),
                     *static_cast<pointer>(node))) {
      if (current->left() != nullptr) {
        current = current->left();
      } else {
        current->mLeft = node;
        node->mParent  = current;
        node->fix_insert(*this);
        mSize += 1;
        return nullptr;
      }
    } else if (value_comp()(*static_cast<pointer>(node),
                            *static_cast<pointer>(current))) {
      if (current->right() != nullptr) {
        current = current->right();
      } else {
        current->mRight = node;
        node->mParent   = current;
        node->fix_insert(*this);
        mSize += 1;
        return nullptr;
      }
    } else {
      // Replace
      current->replace(node, *this);
      return static_cast<pointer>(current);
    }
  }
}

template <class T, class Compare>
void avl_tree<T, Compare>::insert_multi(pointer obj) noexcept {
  auto node    = static_cast<avl_node *>(obj);
  auto current = static_cast<avl_node *>(root());
  if (current == nullptr) {
    mValue.first() = node;
    node->mParent = node->mLeft = node->mRight = nullptr;
    node->mHeight                              = 1;
    mSize += 1;
    return;
  }

  for (;;) {
    if (value_comp()(*static_cast<pointer>(node),
                     *static_cast<pointer>(current))) {
      if (current->left() != nullptr) {
        current = current->left();
      } else {
        current->mLeft = node;
        node->mParent  = current;
        node->fix_insert(*this);
        mSize += 1;
        return;
      }
    } else if (value_comp()(*static_cast<pointer>(current),
                            *static_cast<pointer>(node))) {
      if (current->right() != nullptr) {
        current = current->right();
      } else {
        current->mRight = node;
        node->mParent   = current;
        node->fix_insert(*this);
        mSize += 1;
        return;
      }
    } else {
      if (current->left() == nullptr) {
        current->mLeft = node;
        node->mParent  = current;
        node->fix_insert(*this);
        mSize += 1;
        return;
      } else if (current->right() == nullptr) {
        current->mRight = node;
        node->mParent   = current;
        node->fix_insert(*this);
        mSize += 1;
        return;
      } else {
        if (current->left()->height() < current->right()->height())
          current = current->left();
        else
          current = current->right();
      }
    }
  }
}

template <class T, class Compare>
void avl_tree<T, Compare>::erase(pointer obj) noexcept {
  auto      node = static_cast<avl_node *>(obj);
  avl_node *child, *parent;

  if (node->left() != nullptr && node->right() != nullptr) {
    avl_node *old = node;
    avl_node *left;
    node = node->right();

    while ((left = node->left()) != nullptr)
      node = left;

    child  = node->right();
    parent = node->parent();

    if (child)
      child->mParent = parent;

    node->replace_as_child(child, parent, *this);

    if (node->parent() == old)
      parent = node;

    node->mLeft   = old->left();
    node->mRight  = old->right();
    node->mParent = old->parent();
    node->mHeight = old->height();

    old->replace_as_child(node, old->parent(), *this);
    assert(old->left() != nullptr);
    old->left()->mParent = node;

    if (old->right()) {
      old->right()->mParent = node;
    }
  } else {
    if (node->left() == nullptr)
      child = node->right();
    else
      child = node->left();

    parent = node->parent();
    node->replace_as_child(child, parent, *this);

    if (child)
      child->mParent = parent;
  }

  if (parent != nullptr)
    parent->rebalance(*this);

  mSize -= 1;
}

template <class T, class Compare>
template <class Func>
void avl_tree<T, Compare>::clear(Func &&handler) {
  if (mValue.first() != nullptr) {
    clear_impl(mValue.first(), handler);
    mValue.first() = nullptr;
    mSize          = 0;
  }
}

template <class T, class Compare>
template <class Func>
void avl_tree<T, Compare>::clear_impl(avl_node *node, Func &handler) {
  avl_node *left  = node->left();
  avl_node *right = node->right();

  handler(static_cast<pointer>(node));
  if (left != nullptr)
    clear_impl(left, handler);
  if (right != nullptr)
    clear_impl(right, handler);
}

template <class T, class Compare>
auto avl_tree<T, Compare>::find(const_reference value) noexcept -> pointer {
  auto node = static_cast<avl_node *>(root());
  while (node != nullptr) {
    if (value_comp()(value, *static_cast<pointer>(node))) {
      node = node->left();
    } else if (value_comp()(*static_cast<pointer>(node), value)) {
      node = node->right();
    } else {
      return static_cast<pointer>(node);
    }
  }
  return nullptr;
}

template <class T, class Compare>
auto avl_tree<T, Compare>::find(const_reference value) const noexcept
    -> const_pointer {
  auto node = static_cast<const avl_node *>(root());
  while (node != nullptr) {
    if (value_comp()(value, *static_cast<const_pointer>(node))) {
      node = node->left();
    } else if (value_comp()(*static_cast<const_pointer>(node), value)) {
      node = node->right();
    } else {
      return static_cast<const_pointer>(node);
    }
  }
  return nullptr;
}

template <class T, class Compare>
template <class Value, class Fn>
auto avl_tree<T, Compare>::find(Fn &&fn, Value &&value) noexcept -> pointer {
  auto node = static_cast<avl_node *>(root());
  while (node != nullptr) {
    int cmp = fn(value, *static_cast<pointer>(node));
    if (cmp < 0) {
      node = node->left();
    } else if (cmp > 0) {
      node = node->right();
    } else {
      return static_cast<pointer>(node);
    }
  }
  return nullptr;
}

template <class T, class Compare>
template <class Value, class Fn>
auto avl_tree<T, Compare>::find(Fn &&fn, Value &&value) const noexcept
    -> const_pointer {
  auto node = static_cast<const avl_node *>(root());

  while (node != nullptr) {
    int cmp = fn(value, *static_cast<const_pointer>(node));
    if (cmp < 0) {
      node = node->left();
    } else if (cmp > 0) {
      node = node->right();
    } else {
      return static_cast<const_pointer>(node);
    }
  }
  return nullptr;
}

} // namespace tinystl

#endif // TINYSTL_AVL_TREE_H
