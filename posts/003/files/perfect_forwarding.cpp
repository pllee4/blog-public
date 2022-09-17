/*
 * perfect_forwarding.cpp
 *
 * Created on: Aug 13, 2022 18:19
 * Description:
 *
 * Copyright (c) 2022 Pin Loon Lee (pllee4)
 */

#include <iostream>
#include <string>

class Object {
 public:
  Object() = default;

  void SetName(const std::string &name) { name_ = std::move(name); }
  std::string GetName() const { return name_; }

 private:
  std::string name_;
};

void UseObject(Object &) {
  std::cout << "calling UseObject(Object &)" << std::endl;
}

void UseObject(const Object &) {
  std::cout << "calling UseObject(const Object &)" << std::endl;
}

void UseObject(Object &&) {
  std::cout << "calling UseObject(Object &&)" << std::endl;
}

template <typename T>
void NotForwardToUseObject(T x) {
  UseObject(x);
}

template <typename T>
void HalfForwardToUseObject(T &&x) {  // universal reference
  UseObject(x);
}

template <typename T>
void ForwardToUseObject(T &&x) {
  UseObject(static_cast<T &&>(x));   // cast x to its original type and 1value- or r-value-ness
}

template <typename T>
void PerfectForwardToUseObject(T &&x) {
  UseObject(std::forward<T>(x));
}

int main() {
  Object object;
  const Object const_object;

  std::cout << "Using UseObject directly" << std::endl;
  UseObject(object);
  UseObject(const_object);
  UseObject(std::move(object));

  std::cout << "\nUsing NotForwardToUseObject" << std::endl;
  NotForwardToUseObject(object);
  NotForwardToUseObject(const_object);
  NotForwardToUseObject(std::move(object));

  std::cout << "\nUsing HalfForwardToUseObject" << std::endl;
  HalfForwardToUseObject(object);
  HalfForwardToUseObject(const_object);
  HalfForwardToUseObject(std::move(object));

  std::cout << "\nUsing ForwardToUseObject" << std::endl;
  ForwardToUseObject(object);
  ForwardToUseObject(const_object);
  ForwardToUseObject(std::move(object));

  std::cout << "\nUsing PerfectForwardToUseObject" << std::endl;
  PerfectForwardToUseObject(object);
  PerfectForwardToUseObject(const_object);
  PerfectForwardToUseObject(std::move(object));
}