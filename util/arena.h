// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_UTIL_ARENA_H_
#define STORAGE_LEVELDB_UTIL_ARENA_H_

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace leveldb {

class Arena {
 public:
  // 默认构造函数, 没有做任何特殊的操作
  // 显示初始化成员变量为默认值
  Arena();

  // 资源管理类, 不能进行拷贝和赋值
  Arena(const Arena&) = delete;
  Arena& operator=(const Arena&) = delete;

  ~Arena();

  // Return a pointer to a newly allocated memory block of "bytes" bytes.
  // 返回指向新分配的"bytes"大小内存空间的指针地址, 即分配指定大小的内存空间
  char* Allocate(size_t bytes);

  // Allocate memory with the normal alignment guarantees provided by malloc.
  // 使用和malloc一样的内存对齐方式分配内存
  char* AllocateAligned(size_t bytes);

  // Returns an estimate of the total memory usage of data allocated
  // by the arena.
  // 返回当前已经分配的总内存
  size_t MemoryUsage() const {
    return memory_usage_.load(std::memory_order_relaxed);
  }

 private:
  char* AllocateFallback(size_t bytes);
  /**
   * 指定大小分配新的内存块Block存放到vector数组中
   * 1. new[] block_bytes大小的内存空间
   * 2. new的char*地址push_back到vector中
   * 3. 更新memory_usage_成员变量
   */
  char* AllocateNewBlock(size_t block_bytes);

  // Allocation state
  char* alloc_ptr_;
  size_t alloc_bytes_remaining_;

  // Array of new[] allocated memory blocks
  // vector存储使用new[]分配的内存空间, 即vector成员是内存块首地址
  std::vector<char*> blocks_;

  // Total memory usage of the arena.
  // 总共已经使用了的内存空间, 原子变量
  // TODO(costan): This member is accessed via atomics, but the others are
  //               accessed without any locking. Is this OK?
  // TODO: 只有memory_usage_使用的原子访问, 其他变量没有加锁, 其作用是什么？
  // 只是保证MemoryUsage函数的线程安全性？
  std::atomic<size_t> memory_usage_;
};

inline char* Arena::Allocate(size_t bytes) {
  // The semantics of what to return are a bit messy if we allow
  // 0-byte allocations, so we disallow them here (we don't need
  // them for our internal use).
  assert(bytes > 0); // 不提供分配0个内存的操作
  if (bytes <= alloc_bytes_remaining_) {    // 当前内存块够用, 不用分配新的内存块
    char* result = alloc_ptr_;
    alloc_ptr_ += bytes;
    alloc_bytes_remaining_ -= bytes;
    return result;
  }
  return AllocateFallback(bytes);   // 内存块空闲空间不够, 需要新分配
}

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_UTIL_ARENA_H_
