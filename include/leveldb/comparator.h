// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_INCLUDE_COMPARATOR_H_
#define STORAGE_LEVELDB_INCLUDE_COMPARATOR_H_

#include <string>

#include "leveldb/export.h"

namespace leveldb {

class Slice;

// A Comparator object provides a total order across slices that are
// used as keys in an sstable or a database.  A Comparator implementation
// must be thread-safe since leveldb may invoke its methods concurrently
// from multiple threads.
/**
 * 提供不同slice的排序方式, 这些slice用于sstable/database中的key
 * Comparator的实现类必须是线程安全的, 因为leveldb会从多个线程中调用方法
 */
class LEVELDB_EXPORT Comparator {
 public:
  virtual ~Comparator();

  // Three-way comparison.  Returns value:
  //   < 0 iff "a" < "b",
  //   == 0 iff "a" == "b",
  //   > 0 iff "a" > "b"
  // 比较接口(纯虚函数), 对两个Slice对象进行比较
  virtual int Compare(const Slice& a, const Slice& b) const = 0;

  // The name of the comparator.  Used to check for comparator
  // mismatches (i.e., a DB created with one comparator is
  // accessed using a different comparator.
  //
  // The client of this package should switch to a new name whenever
  // the comparator implementation changes in a way that will cause
  // the relative ordering of any two keys to change.
  //
  // Names starting with "leveldb." are reserved and should not be used
  // by any clients of this package.
  /**
   * comparator的名字(纯虚函数), 用于检测打开数据库的comparator是否匹配
   * Name ==> comparator一一对应, 唯一标识. 只要comparator的实现会造成任何两个key的相对
   * 顺序发生改变, 都应该更改Name为一个新名字
   * 以"leveldb."开头的名字为保留名字, 不应该被任何该类的实现类使用
   */
  virtual const char* Name() const = 0;

  // Advanced functions: these are used to reduce the space requirements
  // for internal data structures like index blocks.

  // If *start < limit, changes *start to a short string in [start,limit).
  // Simple comparator implementations may return with *start unchanged,
  // i.e., an implementation of this method that does nothing is correct.
  /**
   * 高级功能: 查找最小分隔符(纯虚函数), 用于减少内部数据结构(索引块)所占的空间大小
   * 如果*start字符串比limit小, 则将*start字符串改为[*start, limit)的短字符串
   * 简单实现是不做任何操作直接返回*start(即此方法不执行任何操作是正确的)
   */
  virtual void FindShortestSeparator(std::string* start,
                                     const Slice& limit) const = 0;

  // Changes *key to a short string >= *key.
  // Simple comparator implementations may return with *key unchanged,
  // i.e., an implementation of this method that does nothing is correct.
  /**
   * 将*key更改为 >= *key的短字符(纯虚函数)
   * 简单实现是不做任何操作直接返回*start(即此方法不执行任何操作是正确的)
   */
  virtual void FindShortSuccessor(std::string* key) const = 0;
};

// Return a builtin comparator that uses lexicographic byte-wise
// ordering.  The result remains the property of this module and
// must not be deleted.
/**
 * 返回使用字典序的内置比较器(单例), 线程安全, 进程内共享
 * 外部使用者不用释放返回的comparator
 */
LEVELDB_EXPORT const Comparator* BytewiseComparator();

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_INCLUDE_COMPARATOR_H_
