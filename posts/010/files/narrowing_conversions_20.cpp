/*
 * narrowing_conversions_20.cpp
 *
 * Created on: Dec 08, 2023 21:05
 * Description:
 *
 * Copyright (c) 2023 Pin Loon Lee (pllee4)
 */

#include <limits>

template <typename T>
consteval T GetConsteval() {
  return std::numeric_limits<T>::max();
}

void IntegralToFloating() {
  // will cause warning
  // float f{GetConsteval<int>()};

  float f{GetConsteval<short>()};
  // double d{GetConsteval<int>()};
}

int main() { IntegralToFloating(); }