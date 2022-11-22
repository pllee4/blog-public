/*
 * memory_leak.cpp
 *
 * Created on: Nov 13, 2022 21:02
 * Description:
 *
 * Copyright (c) 2022 Pin Loon Lee (pllee4)
 */

#include <unistd.h>

#include <iostream>
#include <map>
#include <memory>
#include <vector>

struct BigMemory {
  std::vector<std::vector<int>> vec{50, std::vector<int>(50, 0)};
};

class BadClass {
 public:
  BadClass() : big_memory_(new BigMemory) {}

  ~BadClass() {
    if (big_memory_) delete big_memory_;
  }

 private:
  BigMemory* big_memory_;
};

class HiddenBadClass {
 public:
  HiddenBadClass() { AddNewId(0); }

  ~HiddenBadClass() {
    for (const auto [key, ptr] : map_) {
      if (ptr) delete ptr;
    }
  }

  void AddNewId(int id) { map_.insert({id, new BadClass}); }

 private:
  std::map<int, BadClass*> map_;
};

class StillHiddenBadClass {
 public:
  StillHiddenBadClass() { AddNewId(0); }

  void AddNewId(int id) { map_.insert({id, std::make_shared<BadClass>()}); }

 private:
  std::map<int, std::shared_ptr<BadClass>> map_;
};

class BaseClass {
 public:
  virtual ~BaseClass() = default;
  virtual BigMemory GetMemory() const = 0;
};

class DerivedClass : public BaseClass {
 public:
  BigMemory GetMemory() const override { return derived_memory_; }

 private:
  BigMemory derived_memory_;
};

int main() {
  while (true) {
    BadClass bad_class;
    usleep(1);
  }
  // while (true) {
  //   HiddenBadClass hidden_bad_class;
  //   usleep(1);
  // }
  // while (true) {
  //   StillHiddenBadClass still_hidden_bad_class;
  //   usleep(1);
  // }
  // while (true) {
  //   {
  //     auto derived_class_ptr = std::make_unique<DerivedClass>();
  //     std::unique_ptr<BaseClass> base_class_ptr(std::move(derived_class_ptr));
  //   }
  //   usleep(1);
  // }
}