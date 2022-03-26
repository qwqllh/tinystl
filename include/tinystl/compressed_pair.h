#ifndef TINYSTL_COMPRESSED_PAIR_H
#define TINYSTL_COMPRESSED_PAIR_H

#include <type_traits>
#include <utility>

namespace tinystl {

namespace compressed_pair_detail {

template <class T,
          int  Place,
          bool IsEmptyBase =
              std::is_empty<T>::value && !std::is_final<T>::value>
class compressed_pair_element {
public:
  using reference       = T &;
  using const_reference = const T &;

  template <class... Args>
  constexpr compressed_pair_element(Args &&...args) noexcept(
      std::is_nothrow_constructible<T, decltype(args)...>::value)
      : value(std::forward<Args>(args)...) {}

  reference       get() noexcept { return value; }
  const_reference get() const noexcept { return value; }

private:
  T value;
};

template <class T, int Place>
class compressed_pair_element<T, Place, true> : private T {
public:
  using reference       = T &;
  using const_reference = const T &;

  template <class... Args>
  constexpr compressed_pair_element(Args &&...args) noexcept(
      std::is_nothrow_constructible<T, decltype(args)...>::value)
      : T(std::forward<Args>(args)...) {}

  reference       get() noexcept { return *this; }
  const_reference get() const noexcept { return *this; }
};

} // namespace compressed_pair_detail

template <class T1,
          class T2,
          bool = std::is_default_constructible<T1>::value
              &&std::is_default_constructible<T2>::value>
class compressed_pair
    : private compressed_pair_detail::compressed_pair_element<T1, 0>,
      private compressed_pair_detail::compressed_pair_element<T2, 1> {
  using Base1 = compressed_pair_detail::compressed_pair_element<T1, 0>;
  using Base2 = compressed_pair_detail::compressed_pair_element<T2, 1>;

public:
  template <class U1, class U2>
  constexpr compressed_pair(U1 &&value1, U2 &&value2) noexcept(
      std::is_nothrow_constructible<U1, decltype(value1)>::value
          &&std::is_nothrow_constructible<U2, decltype(value2)>::value)
      : Base1(std::forward<U1>(value1)), Base2(std::forward<U2>(value2)) {}

  auto first() noexcept -> typename Base1::reference {
    return static_cast<Base1 *>(this)->get();
  }

  auto first() const noexcept -> typename Base1::const_reference {
    return static_cast<const Base1 *>(this)->get();
  }

  auto second() noexcept -> typename Base2::reference {
    return static_cast<Base2 *>(this)->get();
  }

  auto second() const noexcept -> typename Base2::const_reference {
    return static_cast<const Base2 *>(this)->get();
  }

  void swap(compressed_pair &other) {
    using std::swap;
    swap(first(), other.first());
    swap(second(), other.second());
  }
};

template <class T1, class T2>
class compressed_pair<T1, T2, true>
    : private compressed_pair_detail::compressed_pair_element<T1, 0>,
      private compressed_pair_detail::compressed_pair_element<T2, 1> {
  using Base1 = compressed_pair_detail::compressed_pair_element<T1, 0>;
  using Base2 = compressed_pair_detail::compressed_pair_element<T2, 1>;

public:
  constexpr compressed_pair() noexcept(
      std::is_nothrow_default_constructible<Base1>::value
          &&std::is_nothrow_default_constructible<Base2>::value)
      : Base1(), Base2() {}

  template <class U1, class U2>
  constexpr compressed_pair(U1 &&value1, U2 &&value2) noexcept(
      std::is_nothrow_constructible<U1, decltype(value1)>::value
          &&std::is_nothrow_constructible<U2, decltype(value2)>::value)
      : Base1(std::forward<U1>(value1)), Base2(std::forward<U2>(value2)) {}

  auto first() noexcept -> typename Base1::reference {
    return static_cast<Base1 *>(this)->get();
  }

  auto first() const noexcept -> typename Base1::const_reference {
    return static_cast<const Base1 *>(this)->get();
  }

  auto second() noexcept -> typename Base2::reference {
    return static_cast<Base2 *>(this)->get();
  }

  auto second() const noexcept -> typename Base2::const_reference {
    return static_cast<const Base2 *>(this)->get();
  }

  void swap(compressed_pair &other) {
    using std::swap;
    swap(first(), other.first());
    swap(second(), other.second());
  }
};

template <class T1, class T2>
inline void swap(compressed_pair<T1, T2> &l, compressed_pair<T1, T2> &r) {
  l.swap(r);
}

} // namespace tinystl

#endif // TINYSTL_COMPRESSED_PAIR_H
