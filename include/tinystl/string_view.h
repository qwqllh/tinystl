/// string_view
/// 参考https://en.cppreference.com/w/cpp/string/basic_string_view
///
/// 实现C++17 std::string_view的所有构造函数和成员函数。所有实现行为与标准一致。
///
/// 没有实现std::hash的支持，因此不能在unordered容器中使用。
/// 对于literals，因为非下划线开头的literals为标准保留，故实现为operator""_sv。

#ifndef TINYSTL_STRING_VIEW_H
#define TINYSTL_STRING_VIEW_H

#include <cassert>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <string>

namespace tinystl {

template <class CharT, class Traits = std::char_traits<CharT>>
class basic_string_view {
public:
  using traits_type            = Traits;
  using value_type             = CharT;
  using pointer                = value_type *;
  using const_pointer          = const value_type *;
  using reference              = value_type &;
  using const_reference        = const value_type &;
  using const_iterator         = const_pointer;
  using iterator               = const_iterator;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using reverse_iterator       = const_reverse_iterator;
  using size_type              = std::size_t;
  using difference_type        = std::ptrdiff_t;

  static constexpr const size_type npos = size_type(-1);

  // Constructors and assignment
  constexpr basic_string_view() noexcept                          = default;
  constexpr basic_string_view(const basic_string_view &) noexcept = default;
  constexpr basic_string_view(const CharT *s, size_type count) noexcept;
  constexpr basic_string_view(const CharT *s);
  constexpr basic_string_view(std::nullptr_t) = delete;

  constexpr basic_string_view &operator=(const basic_string_view &) noexcept = default;

  // Iterators
  constexpr const_iterator         begin() const noexcept;
  constexpr const_iterator         cbegin() const noexcept;
  constexpr const_iterator         end() const noexcept;
  constexpr const_iterator         cend() const noexcept;
  constexpr const_reverse_iterator rbegin() const noexcept;
  constexpr const_reverse_iterator rend() const noexcept;
  constexpr const_reverse_iterator crbegin() const noexcept;
  constexpr const_reverse_iterator crend() const noexcept;

  // Element access
  constexpr const_reference operator[](size_type pos) const noexcept;

  constexpr const_reference at(size_type pos) const;
  constexpr const_reference front() const;
  constexpr const_reference back() const;
  constexpr const_pointer   data() const noexcept;

  // Capacity
  constexpr size_type size() const noexcept;
  constexpr size_type length() const noexcept;
  constexpr size_type max_size() const noexcept;
  constexpr bool      empty() const noexcept;

  // Modifiers
  constexpr void remove_prefix(size_type n);
  constexpr void remove_suffix(size_type n);

  constexpr void swap(basic_string_view &v) noexcept;

  // Operations
  constexpr size_type         copy(CharT *dest, size_type count, size_type pos = 0) const;
  constexpr basic_string_view substr(size_type pos = 0, size_type count = npos) const;

  constexpr int compare(basic_string_view v) const noexcept;
  constexpr int compare(size_type pos1, size_type count1, basic_string_view v) const;
  constexpr int compare(size_type         pos1,
                        size_type         count1,
                        basic_string_view v,
                        size_type         pos2,
                        size_type         count2) const;
  constexpr int compare(const CharT *s) const;
  constexpr int compare(size_type pos1, size_type count1, const CharT *s) const;
  constexpr int compare(size_type pos1, size_type count1, const CharT *s, size_type count2) const;

  constexpr bool starts_with(basic_string_view sv) const noexcept;
  constexpr bool starts_with(CharT c) const noexcept;
  constexpr bool starts_with(const CharT *s) const;

  constexpr bool ends_with(basic_string_view sv) const noexcept;
  constexpr bool ends_with(CharT c) const noexcept;
  constexpr bool ends_with(const CharT *s) const;

  constexpr bool contains(basic_string_view sv) const noexcept;
  constexpr bool contains(CharT c) const noexcept;
  constexpr bool contains(const CharT *s) const;

  constexpr size_type find(basic_string_view v, size_type pos = 0) const noexcept;
  constexpr size_type find(CharT ch, size_type pos = 0) const noexcept;
  constexpr size_type find(const CharT *s, size_type pos, size_type count) const;
  constexpr size_type find(const CharT *s, size_type pos = 0) const;

  constexpr size_type rfind(basic_string_view v, size_type pos = npos) const noexcept;
  constexpr size_type rfind(CharT c, size_type pos = npos) const noexcept;
  constexpr size_type rfind(const CharT *s, size_type pos, size_type count) const;
  constexpr size_type rfind(const CharT *s, size_type pos = npos) const;

  constexpr size_type find_first_of(basic_string_view v, size_type pos = 0) const noexcept;
  constexpr size_type find_first_of(CharT c, size_type pos = 0) const noexcept;
  constexpr size_type find_first_of(const CharT *s, size_type pos, size_type count) const;
  constexpr size_type find_first_of(const CharT *s, size_type pos = 0) const;

  constexpr size_type find_last_of(basic_string_view v, size_type pos = npos) const noexcept;
  constexpr size_type find_last_of(CharT c, size_type pos = npos) const noexcept;
  constexpr size_type find_last_of(const CharT *s, size_type pos, size_type count) const;
  constexpr size_type find_last_of(const CharT *s, size_type pos = npos) const;

  constexpr size_type find_first_not_of(basic_string_view v, size_type pos = 0) const noexcept;
  constexpr size_type find_first_not_of(CharT c, size_type pos = 0) const noexcept;
  constexpr size_type find_first_not_of(const CharT *s, size_type pos, size_type count) const;
  constexpr size_type find_first_not_of(const CharT *s, size_type pos = 0) const;

  constexpr size_type find_last_not_of(basic_string_view v, size_type pos = npos) const noexcept;
  constexpr size_type find_last_not_of(CharT c, size_type pos = npos) const noexcept;
  constexpr size_type find_last_not_of(const CharT *s, size_type pos, size_type count) const;
  constexpr size_type find_last_not_of(const CharT *s, size_type pos = npos) const;

private:
  const_pointer mData = nullptr;
  size_type     mSize = 0;
};

template <class CharT, class Traits>
constexpr basic_string_view<CharT, Traits>::basic_string_view(const CharT *s,
                                                              size_type    count) noexcept
    : mData(s), mSize(count) {}

template <class CharT, class Traits>
constexpr basic_string_view<CharT, Traits>::basic_string_view(const CharT *s)
    : mData(s), mSize(traits_type::length(s)) {}

template <class CharT, class Traits>
constexpr void basic_string_view<CharT, Traits>::remove_prefix(size_type n) {
  assert(n <= mSize && "string_view::remove_prefix - prefix size greater than string_view size.");
  mData += n;
  mSize -= n;
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::begin() const noexcept -> const_iterator {
  return mData;
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::cbegin() const noexcept -> const_iterator {
  return begin();
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::end() const noexcept -> const_iterator {
  return (mData + mSize);
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::cend() const noexcept -> const_iterator {
  return end();
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::rbegin() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(end());
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::rend() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(begin());
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::crbegin() const noexcept
    -> const_reverse_iterator {
  return rbegin();
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::crend() const noexcept -> const_reverse_iterator {
  return rend();
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::operator[](size_type pos) const noexcept
    -> const_reference {
  return mData[pos];
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::at(size_type pos) const -> const_reference {
  assert(pos < mSize && "string_view::at - index out of range.");
  return mData[pos];
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::front() const -> const_reference {
  assert(!empty() && "string_view::front - string_view is empty.");
  return mData[0];
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::back() const -> const_reference {
  assert(!empty() && "string_view::back - string_view is empty.");
  return mData[mSize - 1];
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::data() const noexcept -> const_pointer {
  return mData;
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::size() const noexcept -> size_type {
  return mSize;
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::length() const noexcept -> size_type {
  return size();
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::max_size() const noexcept -> size_type {
  return std::numeric_limits<size_type>::max();
}

template <class CharT, class Traits>
constexpr bool basic_string_view<CharT, Traits>::empty() const noexcept {
  return size() == 0;
}

template <class CharT, class Traits>
constexpr void basic_string_view<CharT, Traits>::remove_suffix(size_type n) {
  assert(n <= mSize && "string_view::remove_suffix - suffix size greater than string_view size.");
  mSize -= n;
}

template <class CharT, class Traits>
constexpr void basic_string_view<CharT, Traits>::swap(basic_string_view &v) noexcept {
  std::swap(mData, v.mData);
  std::swap(mSize, v.mSize);
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::copy(CharT    *dest,
                                                      size_type count,
                                                      size_type pos) const -> size_type {
  assert(pos <= size() && "string_view::copy - pos out of range.");
  count = std::min(count, size() - pos);
  traits_type::copy(dest, data() + pos, count);
  return count;
}

template <class CharT, class Traits>
constexpr basic_string_view<CharT, Traits>
basic_string_view<CharT, Traits>::substr(size_type pos, size_type count) const {
  assert(pos <= size() && "string_view::copy - pos out of range.");
  return basic_string_view(data() + pos, std::min(size() - pos, count));
}

template <class CharT, class Traits>
constexpr int basic_string_view<CharT, Traits>::compare(basic_string_view v) const noexcept {
  size_type count = std::min(size(), v.size());

  int ret = traits_type::compare(data(), v.data(), count);
  if (ret != 0)
    return ret;

  return (size() > v.size() ? 1 : (size() < v.size() ? -1 : 0));
}

template <class CharT, class Traits>
constexpr int basic_string_view<CharT, Traits>::compare(size_type         pos1,
                                                        size_type         count1,
                                                        basic_string_view v) const {
  return substr(pos1, count1).compare(v);
}

template <class CharT, class Traits>
constexpr int basic_string_view<CharT, Traits>::compare(
    size_type pos1, size_type count1, basic_string_view v, size_type pos2, size_type count2) const {
  return substr(pos1, count1).compare(v.substr(pos2, count2));
}

template <class CharT, class Traits>
constexpr int basic_string_view<CharT, Traits>::compare(const CharT *s) const {
  return compare(basic_string_view(s));
}

template <class CharT, class Traits>
constexpr int
basic_string_view<CharT, Traits>::compare(size_type pos1, size_type count1, const CharT *s) const {
  return substr(pos1, count1).compare(s);
}

template <class CharT, class Traits>
constexpr int basic_string_view<CharT, Traits>::compare(size_type    pos1,
                                                        size_type    count1,
                                                        const CharT *s,
                                                        size_type    count2) const {
  return substr(pos1, count1).compare(basic_string_view(s, count2));
}

template <class CharT, class Traits>
constexpr bool basic_string_view<CharT, Traits>::starts_with(basic_string_view sv) const noexcept {
  return (size() >= sv.size() && traits_type::compare(data(), sv.data(), sv.size()) == 0);
}

template <class CharT, class Traits>
constexpr bool basic_string_view<CharT, Traits>::starts_with(CharT c) const noexcept {
  return (!empty() && mData[0] == c);
}

template <class CharT, class Traits>
constexpr bool basic_string_view<CharT, Traits>::starts_with(const CharT *s) const {
  return starts_with(basic_string_view(s));
}

template <class CharT, class Traits>
constexpr bool basic_string_view<CharT, Traits>::ends_with(basic_string_view sv) const noexcept {
  return (size() >= sv.size() &&
          traits_type::compare(data() + size() - sv.size(), sv.data(), sv.size()) == 0);
}

template <class CharT, class Traits>
constexpr bool basic_string_view<CharT, Traits>::ends_with(CharT c) const noexcept {
  return (!empty() && back() == c);
}

template <class CharT, class Traits>
constexpr bool basic_string_view<CharT, Traits>::ends_with(const CharT *s) const {
  return ends_with(basic_string_view(s));
}

template <class CharT, class Traits>
constexpr bool basic_string_view<CharT, Traits>::contains(basic_string_view sv) const noexcept {
  return (find(sv, 0) != npos);
}

template <class CharT, class Traits>
constexpr bool basic_string_view<CharT, Traits>::contains(CharT c) const noexcept {
  return (find(c, 0) != npos);
}

template <class CharT, class Traits>
constexpr bool basic_string_view<CharT, Traits>::contains(const CharT *s) const {
  return contains(basic_string_view(s));
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::find(basic_string_view v,
                                                      size_type pos) const noexcept -> size_type {
  if (pos + v.size() > size())
    return npos;

  const_pointer pEnd = data() + size() - v.size() + 1;
  for (const_pointer p = data() + pos; p < pEnd; ++p) {
    if (traits_type::compare(p, v.data(), v.size()) == 0)
      return static_cast<size_type>(p - data());
  }
  return npos;
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::find(CharT ch, size_type pos) const noexcept
    -> size_type {
  for (const_pointer p = data() + pos; p < data() + size(); ++p) {
    if (traits_type::eq(*p, ch))
      return static_cast<size_type>(p - data());
  }
  return npos;
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::find(const CharT *s,
                                                      size_type    pos,
                                                      size_type    count) const -> size_type {
  return find(basic_string_view(s, count), pos);
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::find(const CharT *s, size_type pos) const
    -> size_type {
  return find(basic_string_view(s), pos);
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::rfind(basic_string_view v,
                                                       size_type pos) const noexcept -> size_type {
  if (v.size() > size())
    return npos;

  // The following for loop may loop forever if data() is nullptr.
  if (empty()) {
    return v.empty() ? 0 : npos;
  }

  pos = std::min(pos, size() - v.size());
  for (const_pointer p = data() + pos; p >= data(); --p) {
    if (traits_type::compare(p, v.data(), v.size()) == 0)
      return static_cast<size_type>(p - data());
  }
  return npos;
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::rfind(CharT c, size_type pos) const noexcept
    -> size_type {
  if (empty())
    return false;

  for (const_pointer p = data() + size() - 1; p >= data(); --p) {
    if (traits_type::eq(*p, c))
      return (p - data());
  }
  return npos;
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::rfind(const CharT *s,
                                                       size_type    pos,
                                                       size_type    count) const -> size_type {
  return rfind(basic_string_view(s, count), pos);
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::rfind(const CharT *s, size_type pos) const
    -> size_type {
  return rfind(basic_string_view(s), pos);
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::find_first_of(basic_string_view v,
                                                               size_type         pos) const noexcept
    -> size_type {
  for (const_pointer p = data(); p < data() + size(); ++p) {
    if (v.contains(*p))
      return static_cast<size_type>(p - data());
  }
  return npos;
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::find_first_of(CharT     c,
                                                               size_type pos) const noexcept
    -> size_type {
  for (const_pointer p = data(); p < data() + size(); ++p) {
    if (traits_type::eq(*p, c))
      return static_cast<size_type>(p - data());
  }
  return npos;
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::find_first_of(const CharT *s,
                                                               size_type    pos,
                                                               size_type count) const -> size_type {
  return find_first_of(basic_string_view(s, count), pos);
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::find_first_of(const CharT *s, size_type pos) const
    -> size_type {
  return find_first_of(basic_string_view(s), pos);
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::find_last_of(basic_string_view v,
                                                              size_type         pos) const noexcept
    -> size_type {
  if (empty())
    return npos;

  // Safe to loop. data() is not nullptr.
  for (const_pointer p = data() + size() - 1; p >= data(); --p) {
    if (v.find(*p) != npos)
      return static_cast<size_type>(p - data());
  }
  return npos;
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::find_last_of(CharT c, size_type pos) const noexcept
    -> size_type {
  if (empty())
    return npos;

  for (const_pointer p = data() + size() - 1; p >= data(); --p) {
    if (traits_type::eq(*p, c))
      return static_cast<size_type>(p - data());
  }
  return npos;
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::find_last_of(const CharT *s,
                                                              size_type    pos,
                                                              size_type count) const -> size_type {
  return find_last_of(basic_string_view(s, count), pos);
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::find_last_of(const CharT *s, size_type pos) const
    -> size_type {
  return find_last_of(basic_string_view(s), pos);
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::find_first_not_of(basic_string_view v,
                                                                   size_type pos) const noexcept
    -> size_type {
  for (const_pointer p = data(); p < data() + size(); ++p) {
    if (!v.contains(*p))
      return static_cast<size_type>(p - data());
  }
  return npos;
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::find_first_not_of(CharT     c,
                                                                   size_type pos) const noexcept
    -> size_type {
  for (const_pointer p = data(); p < data() + size(); ++p) {
    if (!traits_type::eq(*p, c))
      return static_cast<size_type>(p - data());
  }
  return npos;
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::find_first_not_of(const CharT *s,
                                                                   size_type    pos,
                                                                   size_type    count) const
    -> size_type {
  return find_first_not_of(basic_string_view(s, count), pos);
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::find_first_not_of(const CharT *s,
                                                                   size_type    pos) const
    -> size_type {
  return find_first_not_of(basic_string_view(s), pos);
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::find_last_not_of(basic_string_view v,
                                                                  size_type pos) const noexcept
    -> size_type {
  if (empty())
    return npos;

  // Safe to loop. data() is not nullptr.
  for (const_pointer p = data() + size() - 1; p >= data(); --p) {
    if (!v.contains(*p))
      return static_cast<size_type>(p - data());
  }
  return npos;
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::find_last_not_of(CharT     c,
                                                                  size_type pos) const noexcept
    -> size_type {
  if (empty())
    return npos;

  for (const_pointer p = data() + size() - 1; p >= data(); --p) {
    if (!traits_type::eq(*p, c))
      return static_cast<size_type>(p - data());
  }
  return npos;
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::find_last_not_of(const CharT *s,
                                                                  size_type    pos,
                                                                  size_type    count) const
    -> size_type {
  return find_last_not_of(basic_string_view(s, count), pos);
}

template <class CharT, class Traits>
constexpr auto basic_string_view<CharT, Traits>::find_last_not_of(const CharT *s,
                                                                  size_type    pos) const
    -> size_type {
  return find_last_not_of(basic_string_view(s), pos);
}

template <class CharT, class Traits>
constexpr bool operator==(basic_string_view<CharT, Traits> lhs,
                          basic_string_view<CharT, Traits> rhs) noexcept {
  if (std::addressof(lhs) == std::addressof(rhs))
    return true;
  return lhs.compare(rhs) == 0;
}

template <class CharT, class Traits>
constexpr bool operator!=(basic_string_view<CharT, Traits> lhs,
                          basic_string_view<CharT, Traits> rhs) noexcept {
  if (std::addressof(lhs) == std::addressof(rhs))
    return false;
  return lhs.compare(rhs) != 0;
}

template <class CharT, class Traits>
constexpr bool operator<(basic_string_view<CharT, Traits> lhs,
                         basic_string_view<CharT, Traits> rhs) noexcept {
  return lhs.compare(rhs) < 0;
}

template <class CharT, class Traits>
constexpr bool operator<=(basic_string_view<CharT, Traits> lhs,
                          basic_string_view<CharT, Traits> rhs) noexcept {
  return lhs.compare(rhs) <= 0;
}

template <class CharT, class Traits>
constexpr bool operator>(basic_string_view<CharT, Traits> lhs,
                         basic_string_view<CharT, Traits> rhs) noexcept {
  return lhs.compare(rhs) > 0;
}

template <class CharT, class Traits>
constexpr bool operator>=(basic_string_view<CharT, Traits> lhs,
                          basic_string_view<CharT, Traits> rhs) noexcept {
  return lhs.compare(rhs) >= 0;
}

template <class CharT, class Traits>
std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os,
                                              basic_string_view<CharT, Traits>   v) {
  if (os.width() <= v.size()) {
    os.rdbuf()->sputn(v.data(), v.size());
    return os;
  }

  std::unique_ptr<CharT[]> buffer(new CharT[os.width()]);

  if ((os.flags() & std::ios_base::adjustfield) == std::ios_base::left) {
    v.copy(buffer.get(), v.size());
    for (auto i = v.size(); i < os.width(); ++i)
      buffer[i] = os.fill();
  } else {
    for (auto i = 0; i < os.width() - v.size(); ++i)
      buffer[i] = os.fill();
    v.copy(buffer.get() + os.width() - v.size(), v.size());
  }

  os.write(buffer.get(), os.width());
  os.width(0);
  return os;
}

using string_view    = basic_string_view<char>;
using wstring_view   = basic_string_view<wchar_t>;
using u16string_view = basic_string_view<char16_t>;
using u32string_view = basic_string_view<char32_t>;

namespace string_view_literals {

inline constexpr string_view operator""_sv(const char *str, std::size_t len) noexcept {
  return {str, len};
}

inline constexpr wstring_view operator""_sv(const wchar_t *str, std::size_t len) noexcept {
  return {str, len};
}

inline constexpr u16string_view operator""_sv(const char16_t *str, std::size_t len) noexcept {
  return {str, len};
}

inline constexpr u32string_view operator""_sv(const char32_t *str, std::size_t len) noexcept {
  return {str, len};
}

} // namespace string_view_literals

namespace literals {

using namespace string_view_literals;

} // namespace literals

} // namespace tinystl

#endif // TINYSTL_STRING_VIEW_H
