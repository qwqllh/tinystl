#ifndef TINYSTL_SET_H
#define TINYSTL_SET_H

#include <tinystl/avl_tree.h>
#include <tinystl/compressed_pair.h>

#include <memory>
#include <set>
#include <type_traits>
#include <utility>

namespace tinystl {

template <class Key>
struct set_node : public avl_node {
  Key mKey;

  template <class... Args>
  set_node(Args &&...args) noexcept(std::is_nothrow_constructible<Key, decltype(args)...>::value)
      : mKey(std::forward<Args>(args)...) {}
};

template <class Key,
          class Compare   = std::less<Key>,
          class Allocator = std::allocator<set_node<Key>>>
class set {
private:
  using node_type = set_node<Key>;

  struct internal_compare_inherit_flag {};
  struct internal_compare_member_flag {};

  /// Inherit Compare so that compressed_pair could optimize memory for empty
  /// compare class.
  template <bool = std::is_final<Compare>::value>
  struct internal_compare : public Compare {
    using type_flag = internal_compare_inherit_flag;

    template <class... Args>
    internal_compare(Args &&...args) noexcept(
        std::is_nothrow_constructible<Compare, decltype(args)...>::value)
        : Compare(std::forward<Args>(args)...) {}

    const Compare &decay() const noexcept { return *static_cast<const Compare *>(this); }

    bool operator()(const node_type &a, const node_type &b) {
      return static_cast<const Compare *>(this)->operator()(a.mKey, b.mKey);
    }
  };

  /// Specialize for final Compare class.
  template <>
  struct internal_compare<true> {
    using type_flag = internal_compare_member_flag;

    Compare mCompare;

    template <class... Args>
    internal_compare(Args &&...args) noexcept(
        std::is_nothrow_constructible<Compare, decltype(args)...>::value)
        : mCompare(std::forward<Args>(args)...) {}

    const Compare &decay() const noexcept { return mCompare; }

    bool operator()(const node_type &a, const node_type &b) { return mCompare(a.mKey, b.mKey); }
  };

  using internal_tree_type = avl_tree<node_type, internal_compare<>>;

  class set_iterator {
  public:
    using key_type          = Key;
    using value_type        = Key;
    using size_type         = std::size_t;
    using difference_type   = std::ptrdiff_t;
    using reference         = value_type &;
    using const_reference   = const value_type &;
    using pointer           = value_type *;
    using const_pointer     = const value_type *;
    using iterator_category = std::bidirectional_iterator_tag;

    constexpr set_iterator() noexcept                     = default;
    constexpr set_iterator(const set_iterator &) noexcept = default;

    set_iterator &operator++() noexcept {
      ++it;
      return (*this);
    }

    set_iterator operator++(int) noexcept {
      set_iterator ret = (*this);
      ++(*this);
      return ret;
    }

    set_iterator &operator--() noexcept {
      --it;
      return (*this);
    }

    set_iterator operator--(int) noexcept {
      set_iterator ret = (*this);
      --(*this);
      return ret;
    }

    reference operator*() noexcept {
      node_type &ret = *it;
      return ret.mKey;
    }

    reference operator->() noexcept {
      node_type &ret = *it;
      return ret.mKey;
    }

    constexpr bool operator==(const set_iterator rhs) const noexcept { return it == rhs.it; }

    constexpr bool operator!=(const set_iterator rhs) const noexcept { return it != rhs.it; }

    template <class, class, class>
    friend class set;

  private:
    constexpr set_iterator(typename internal_tree_type::iterator i) noexcept : it(i) {}

    constexpr set_iterator(internal_tree_type *tree, avl_node *node) noexcept : it(tree, node) {}

    typename internal_tree_type::iterator it;
  };

  class set_const_iterator {
  public:
    using key_type          = Key;
    using value_type        = Key;
    using size_type         = std::size_t;
    using difference_type   = std::ptrdiff_t;
    using reference         = value_type &;
    using const_reference   = const value_type &;
    using pointer           = value_type *;
    using const_pointer     = const value_type *;
    using iterator_category = std::bidirectional_iterator_tag;

    constexpr set_const_iterator() noexcept                           = default;
    constexpr set_const_iterator(const set_const_iterator &) noexcept = default;

    set_const_iterator &operator++() noexcept {
      ++it;
      return (*this);
    }

    set_const_iterator operator++(int) noexcept {
      set_iterator ret = (*this);
      ++(*this);
      return ret;
    }

    set_const_iterator &operator--() noexcept {
      --it;
      return (*this);
    }

    set_const_iterator operator--(int) noexcept {
      set_iterator ret = (*this);
      --(*this);
      return ret;
    }

    const_reference operator*() noexcept {
      const node_type &ret = *it;
      return ret.mKey;
    }

    reference operator->() noexcept {
      const node_type &ret = *it;
      return ret.mKey;
    }

    constexpr bool operator==(const set_const_iterator rhs) const noexcept { return it == rhs.it; }

    constexpr bool operator!=(const set_const_iterator rhs) const noexcept { return it != rhs.it; }

    template <class, class, class>
    friend class set;

  private:
    constexpr set_const_iterator(typename internal_tree_type::const_iterator i) noexcept : it(i) {}

    constexpr set_const_iterator(internal_tree_type *tree, avl_node *node) noexcept
        : it(tree, node) {}

    typename internal_tree_type::const_iterator it;
  };

  Compare internal_key_comp(internal_compare_inherit_flag) const
      noexcept(std::is_nothrow_copy_constructible<Compare>::value) {
    return static_cast<Compare>(mValue.first().key_comp());
  }

  Compare internal_key_comp(internal_compare_member_flag) const
      noexcept(std::is_nothrow_copy_constructible<Compare>::value) {
    return mValue.first().key_comp().mCompare;
  }

public:
  using key_type         = Key;
  using value_type       = Key;
  using size_type        = std::size_t;
  using difference_type  = std::ptrdiff_t;
  using key_compare      = Compare;
  using value_compare    = Compare;
  using allocator_type   = Allocator;
  using reference        = value_type &;
  using const_reference  = const value_type &;
  using pointer          = typename std::allocator_traits<Allocator>::pointer;
  using const_pointer    = typename std::allocator_traits<Allocator>::const_pointer;
  using iterator         = set_iterator;
  using const_iterator   = set_const_iterator;
  using allocator_traits = std::allocator_traits<Allocator>;

  set() noexcept(
      std::is_nothrow_default_constructible<compressed_pair<internal_tree_type, Allocator>>::value)
      : mValue() {}

  explicit set(const Allocator &alloc) noexcept(
      std::is_nothrow_default_constructible<internal_tree_type>::value
          &&std::is_nothrow_copy_constructible<Allocator>::value)
      : mValue({}, alloc) {}

  // TODO: implement copy constructor.
  set(const set &other) = delete;

  // TODO: implement move constructor.
  set(set &&other) = delete;

  ~set();

  iterator begin() noexcept { return iterator(mValue.first().begin()); }
  iterator end() noexcept { return iterator(mValue.first().end()); }

  const_iterator begin() const noexcept { return const_iterator(mValue.first().begin()); }

  const_iterator end() const noexcept { return const_iterator(mValue.first().end()); }

  const_iterator cbegin() const noexcept { return begin(); }
  const_iterator cend() const noexcept { return end(); }

  allocator_type get_allocator() const noexcept { return mValue.second(); }

  bool      empty() const noexcept { return mValue.first().empty(); }
  size_type size() const noexcept { return mValue.first().size(); }

  void clear() noexcept;

  std::pair<iterator, bool> insert(const value_type &value);
  std::pair<iterator, bool> insert(value_type &&value);

  void      erase(iterator pos);
  size_type erase(const key_type &key);

  size_type count(const key_type &key) const noexcept;

  iterator       find(const key_type &key) noexcept;
  const_iterator find(const key_type &key) const noexcept;

  bool contains(const key_type &key) const noexcept;

  template <class... Args>
  std::pair<iterator, bool> emplace(Args &&...args);

  key_compare key_comp() const noexcept(std::is_nothrow_copy_constructible<key_compare>::value) {
    return internal_key_comp(typename internal_compare<>::type_flag());
  }

private:
  compressed_pair<internal_tree_type, Allocator> mValue;
};

template <class Key, class Compare, class Allocator>
set<Key, Compare, Allocator>::~set() {
  clear();
}

template <class Key, class Compare, class Allocator>
void set<Key, Compare, Allocator>::clear() noexcept {
  mValue.first().clear([this](node_type *p) {
    assert(p != nullptr);
    p->mKey.~Key();
    this->mValue.second().deallocate(p, 1);
  });
}

template <class Key, class Compare, class Allocator>
auto set<Key, Compare, Allocator>::insert(const value_type &value) -> std::pair<iterator, bool> {
  return emplace(value);
}

template <class Key, class Compare, class Allocator>
auto set<Key, Compare, Allocator>::insert(value_type &&value) -> std::pair<iterator, bool> {
  return emplace(std::move(value));
}

template <class Key, class Compare, class Allocator>
template <class... Args>
auto set<Key, Compare, Allocator>::emplace(Args &&...args) -> std::pair<iterator, bool> {
  node_type *node = allocator_traits::allocate(mValue.second(), 1);
  assert(node != nullptr);
  allocator_traits::construct(mValue.second(), node, std::forward<Args>(args)...);

  bool success = mValue.first().insert_unique(node);
  if (!success) {
    allocator_traits::destroy(mValue.second(), node);
    allocator_traits::deallocate(mValue.second(), node, 1);
    return std::make_pair(end(), false);
  }

  return std::make_pair(iterator(&mValue.first(), node), true);
}

template <class Key, class Compare, class Allocator>
void set<Key, Compare, Allocator>::erase(iterator pos) {
  assert(pos != end());
  pointer ptr = pos.it.get();
  mValue.first().erase(ptr);
  allocator_traits::destroy(mValue.second(), ptr);
  allocator_traits::deallocate(mValue.second(), ptr, 1);
}

template <class Key, class Compare, class Allocator>
auto set<Key, Compare, Allocator>::erase(const key_type &value) -> size_type {
  pointer p = mValue.first().find(
      [this](const key_type &lhs, const node_type &rhs) {
        auto cmp = this->key_comp();
        if (cmp(lhs, rhs.mKey))
          return -1;
        else if (cmp(rhs.mKey, lhs))
          return 1;
        else
          return 0;
      },
      value);

  if (p == nullptr)
    return 0;

  mValue.first().erase(p);
  allocator_traits::destroy(mValue.second(), p);
  allocator_traits::deallocate(mValue.second(), p, 1);
  return 1;
}

template <class Key, class Compare, class Allocator>
auto set<Key, Compare, Allocator>::count(const key_type &key) const noexcept -> size_type {
  const_pointer p = mValue.first().find(
      [this](const key_type &lhs, const node_type &rhs) {
        auto cmp = this->key_comp();
        if (cmp(lhs, rhs.mKey))
          return -1;
        else if (cmp(rhs.mKey, lhs))
          return 1;
        else
          return 0;
      },
      key);

  return (p ? 1 : 0);
}

template <class Key, class Compare, class Allocator>
auto set<Key, Compare, Allocator>::find(const key_type &key) noexcept -> iterator {
  pointer p = mValue.first().find(
      [this](const key_type &lhs, const node_type &rhs) {
        auto cmp = this->key_comp();
        if (cmp(lhs, rhs.mKey))
          return -1;
        else if (cmp(rhs.mKey, lhs))
          return 1;
        else
          return 0;
      },
      key);

  if (p == nullptr)
    return end();

  return iterator(&mValue.first(), p);
}

template <class Key, class Compare, class Allocator>
auto set<Key, Compare, Allocator>::find(const key_type &key) const noexcept -> const_iterator {
  const_pointer p = mValue.first().find(
      [this](const key_type &lhs, const node_type &rhs) {
        auto cmp = this->key_comp();
        if (cmp(lhs, rhs.mKey))
          return -1;
        else if (cmp(rhs.mKey, lhs))
          return 1;
        else
          return 0;
      },
      key);

  if (p == nullptr)
    return end();

  return const_iterator(&mValue.first(), p);
}

template <class Key, class Compare, class Allocator>
bool set<Key, Compare, Allocator>::contains(const key_type &key) const noexcept {
  return count(key);
}

} // namespace tinystl

#endif // TINYSTL_SET_H
