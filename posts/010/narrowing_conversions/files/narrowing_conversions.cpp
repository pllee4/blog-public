/* 
 * narrowing_conversions.cpp
 * 
 * Created on: Dec 09, 2023 23:03
 * Description: 
 * 
 * Copyright (c) 2023 Pin Loon Lee (pllee4)
 */ 

#include <limits>
#include <set>

// all are considered as narrowing conversions
// void FloatingToIntegral() {
//   double d;
//   float d;
//   long double d;

//   int i{d};
//   uint8_t u8{d};
//   long l{d};
//   short s{d};
//   char c{d};
//   unsigned int ui{d};
// }

void FloatingToFloating() {
  float f{1.2345f};  // must explicit f for -Wconversion
  double d{f};

  // double d{1.2345678};
  // float f{d};  // narrowing conversions

  // constexpr double d{
  //     std::numeric_limits<double>::max()};  // narrowing conversions for
  //                                           // destination f

  // constexpr double d{
  //     1.2345678};  // won't cause warning if there is no -Wconversion
  // float f{d};
}

void IntegralToFloating() {
  constexpr short int i{std::numeric_limits<short int>::max()};

  // constexpr int i{std::numeric_limits<int>::max()};  // narrowing conversions
  //                                                    // for destination f

  float f{i};
  // double d{i};
}

void IntegralToIntegral() {
  unsigned short int value{25};
  long int i{value};

  // constexpr short int value{25};

  // constexpr short int value{-25};  // narrowing conversions for destination that
  //                                  //  could not store negative values
  // short int value{25};  // narrowing conversions for destination that is unsigned
  // unsigned int i{value};
}

void Example() {
  const auto id_1 = 1u;
  // const auto id_2 = 2u;
  unsigned char id_2 = 2u;  // original code is "const auto id_2"
  // auto id_2 = 2u; // original code is "const auto id_2"
  const auto ids = std::set<uint8_t>{id_1, id_2};
  // some code that would change value of id_2
}

void ThanksForReadingTillTheEnd() {
  const auto id_1 = 1;
  auto id_2 = 2;
  const auto ids = std::set<int>{id_1, id_2};
  // some code that would change value of id_2
}

int main() {}