/*
 * template_type_deduction.cpp
 *
 * Created on: Oct 14, 2022 22:03
 * Description:
 *
 * Copyright (c) 2022 Pin Loon Lee (pllee4)
 */

#include <iostream>
#include <type_traits>

template <typename T>
void f1(T&) {
  std::cout << "T is ";
  if (std::is_const_v<T>) {
    std::cout << "const ";
  }
  std::cout << typeid(T).name() << std::endl;
}

template <typename T>
void f2(const T&) {
  std::cout << "T is ";
  if (std::is_const_v<T>) {
    std::cout << "const ";
  }
  std::cout << typeid(T).name() << std::endl;
}

void f(int&) { std::cout << "calling f(int &)" << std::endl; }

void f(const int&) { std::cout << "calling f(const int &)" << std::endl; }

void f(int&&) { std::cout << "calling f(int &&)" << std::endl; }

template <typename T>
void f3(T&& param) {
  f(std::forward<T>(param));
}

template <typename T>
void f4(T) {
  std::cout << "T is ";
  if (std::is_const_v<T>) {
    std::cout << "const ";
  }
  std::cout << typeid(T).name() << std::endl;
}

int main() {
  int x = 27;
  const int cx = x;
  const int& rx = x;
  std::cout << "Calling from f1(T&) " << std::endl;
  f1(x);
  f1(cx);
  f1(rx);

  std::cout << "Calling from f2(const T&) " << std::endl;
  f2(x);
  f2(cx);
  f2(rx);

  std::cout << "Calling from f3(T&& param) " << std::endl;
  f3(x);
  f3(cx);
  f3(rx);
  f3(27);

  std::cout << "Calling from f4(T) " << std::endl;
  f4(x);
  f4(cx);
  f4(rx);
  f4(27);
}