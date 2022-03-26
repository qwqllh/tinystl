///
/// 以https://github.com/skywind3000/avlmini为基准进行测试
///
/// 测试环境：
/// Macbook Pro 2019 Intel(R) Core(TM) i7-9750H @2.60GHz
/// 编译器： LLVM clang 13.0.1
/// 编译选项：-std=c++14 -DNDEBUG -O2
///
/// 测试节点数量为10,000,000，使用随机数生成。
/// +----------+---------+---------+---------+
/// |          | insert  |   find  |  clear  |
/// +----------+---------+---------+---------+
/// | avl_tree | 13.708s | 10.533s |  0.784s |
/// | avlmini  | 15.694s | 13.092s |  1.408s |
/// +----------+---------+---------+---------+
///
/// 可见，依赖模版保留类型信息，C++能够比C语言略快一点。
/// 这里猜测主要是C语言实现中需要使用函数指针保存比较函数，对编译器优化不友好。相比之下，
/// C++直接传入类型对编译器内联优化要更友好，尤其是std::less这种非常简单的空类型。
/// 毕竟比较在插入、搜索中使用使用都非常频繁。
///
/// 还有一件很有趣的事情，就是avl_tree的clear算法明显快于avlmini。
/// 我在实现avl_tree的clear算法时没有采用avlmini的迭代算法，而是使用尾递归的算法。
/// 一方面是因为递归写法比较好写；另一方面，我选择相信编译器能够对尾递归进行很好的优化。
/// 因为原本递归算法要进行频繁的压栈弹栈操作，很费时间，所以我并没有对clear的时间抱很大的期望；
/// 而且编译器能够对递归算法进行的优化实际上非常有限，但结果远超我的预期。
///

#include "avlmini.h"
#include "tinystl/avl_tree.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <thread>

struct IntElement : public tinystl::avl_node {
  int64_t mValue = 0;

  constexpr IntElement(int64_t value = 0) noexcept
      : avl_node(), mValue(value) {}

  constexpr bool operator<(const IntElement &rhs) const noexcept {
    return mValue < rhs.mValue;
  }

  constexpr operator int64_t() const noexcept { return mValue; }
};

struct IntElement2 {
  struct avl_node node;
  int64_t mValue = 0;

  IntElement2(int64_t value = 0) noexcept : mValue(value) {
    avl_node_init(&node);
  }

  constexpr operator int64_t() const noexcept { return mValue; }
};

constexpr const int maxn = 10000000;

IntElement elements[maxn];
IntElement2 elements2[maxn];

void run_avl_tree() {
  auto start = std::chrono::high_resolution_clock::now();

  tinystl::avl_tree<IntElement> tree;
  for (auto &element : elements) {
    tree.insert_unique(&element);
  }

  auto period = std::chrono::high_resolution_clock::now() - start;

  std::cout
      << "avl_tree insert " << maxn << " nodes: "
      << std::chrono::duration_cast<std::chrono::milliseconds>(period).count()
      << '\n';

  // search
  std::this_thread::sleep_for(std::chrono::seconds(1));

  start = std::chrono::high_resolution_clock::now();
  for (const auto &e : elements) {
    auto p = tree.find(e);
    if (p == nullptr) {
      fprintf(stderr, "%lld should be found but not.\n", int64_t(e));
      std::abort();
    }
  }
  period = std::chrono::high_resolution_clock::now() - start;

  std::cout
      << "avl_tree find " << maxn << " nodes: "
      << std::chrono::duration_cast<std::chrono::milliseconds>(period).count()
      << '\n';

  // clear all
  std::this_thread::sleep_for(std::chrono::seconds(1));

  start = std::chrono::high_resolution_clock::now();
  tree.clear([](IntElement *p) { memset(p, 0, sizeof(IntElement)); });
  period = std::chrono::high_resolution_clock::now() - start;

  std::cout
      << "avl_tree clear " << maxn << " nodes: "
      << std::chrono::duration_cast<std::chrono::milliseconds>(period).count()
      << '\n';
}

void run_avlmini() {
  auto start = std::chrono::high_resolution_clock::now();

  struct avl_tree tree2;
  avl_tree_init(
      &tree2,
      +[](const void *a, const void *b) {
        auto l = static_cast<const IntElement2 *>(a);
        auto r = static_cast<const IntElement2 *>(b);
        if (l->mValue < r->mValue)
          return -1;
        else if (l->mValue == r->mValue)
          return 0;
        else
          return 1;
      },
      sizeof(IntElement2), AVL_OFFSET(IntElement2, node));

  for (auto &element : elements2) {
    avl_tree_add(&tree2, &element);
  }

  auto period = std::chrono::high_resolution_clock::now() - start;
  std::cout
      << "avlmini insert " << maxn << " nodes: "
      << std::chrono::duration_cast<std::chrono::milliseconds>(period).count()
      << '\n';

  // search
  std::this_thread::sleep_for(std::chrono::seconds(1));

  start = std::chrono::high_resolution_clock::now();
  for (const auto &e : elements2) {
    auto p = static_cast<IntElement2 *>(avl_tree_find(&tree2, &e));
    if (p == nullptr) {
      fprintf(stderr, "%lld should be found but not.\n", e.mValue);
      std::abort();
    }
  }
  period = std::chrono::high_resolution_clock::now() - start;

  std::cout
      << "avlmini find " << maxn << " nodes: "
      << std::chrono::duration_cast<std::chrono::milliseconds>(period).count()
      << '\n';

  // clear all
  std::this_thread::sleep_for(std::chrono::seconds(1));

  start = std::chrono::high_resolution_clock::now();
  avl_tree_clear(
      &tree2, +[](void *p) { memset(p, 0, sizeof(IntElement2)); });
  period = std::chrono::high_resolution_clock::now() - start;

  std::cout
      << "avlmini clear " << maxn << " nodes: "
      << std::chrono::duration_cast<std::chrono::milliseconds>(period).count()
      << '\n';
}

int main() {
  srand(time(nullptr));
  for (size_t i = 0; i < maxn; ++i) {
    elements[i] = rand();
    elements2[i] = elements[i].mValue;
  }

  run_avlmini();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  run_avl_tree();

  return 0;
}
