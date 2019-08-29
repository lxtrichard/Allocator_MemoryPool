# **STL allocator + memory pool**

## Overview

STL provides us a default [std::allocator](https://en.cppreference.com/w/cpp/memory/allocator), but we can implement your own to replace it. In this project, we designed a memory pool to speed up the dynamic allocation of a large number of small blocks (e.g., 8 bytes, 16 bytes, ...), and to reduce memory fragmentation. The following function are achieved.

- Implement the memory allocator for STL vector.
- The allocator optimizes the memory allocation speed using memory pool.
- The allocator supports arbitrary memory size allocation request.

## How to test

Basically, you can:

1. Create more than ten thousand vectors with different number of elements.
2. Pick up 1000 random vectors and resize the vectors with random sizes.
3. Release all the vectors.

Feel free to extend the following code skeleton for your own tests:

```c++
#include <iostream>
#include <random>
#include <vector>

// include header of your allocator here
template<class T>
using MyAllocator = std::allocator<T>; // replace the std::allocator with your allocator
using Point2D = std::pair<int, int>;

const int TestSize = 10000;
const int PickSize = 1000;

int main()
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(1, TestSize);

  // vector creation
  using IntVec = std::vector<int, MyAllocator<int>>;
  std::vector<IntVec, MyAllocator<IntVec>> vecints(TestSize);
  for (int i = 0; i < TestSize; i++)
    vecints[i].resize(dis(gen));

  using PointVec = std::vector<Point2D, MyAllocator<Point2D>>;
  std::vector<PointVec, MyAllocator<PointVec>> vecpts(TestSize);
  for (int i = 0; i < TestSize; i++)
    vecpts[i].resize(dis(gen));

  // vector resize
  for (int i = 0; i < PickSize; i++)
  {
    int idx = dis(gen) - 1;
    int size = dis(gen);
    vecints[idx].resize(size);
    vecpts[idx].resize(size);
  }

  // vector element assignment
  {
    int val = 10;
    int idx1 = dis(gen) - 1;
    int idx2 = vecints[idx1].size() / 2;
    vecints[idx1][idx2] = val;
    if (vecints[idx1][idx2] == val)
      std::cout << "correct assignment in vecints: " << idx1 << std::endl;
    else
      std::cout << "incorrect assignment in vecints: " << idx1 << std::endl;
  }
  {
    Point2D val(11, 15);
    int idx1 = dis(gen) - 1;
    int idx2 = vecpts[idx1].size() / 2;
    vecpts[idx1][idx2] = val;
    if (vecpts[idx1][idx2] == val)
      std::cout << "correct assignment in vecpts: " << idx1 << std::endl;
    else
      std::cout << "incorrect assignment in vecpts: " << idx1 << std::endl;
  }

  return 0;
}
```

