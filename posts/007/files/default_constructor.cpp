/*
 * default_constructor.cpp
 *
 * Created on: Jan 31, 2023 22:25
 * Description:
 *
 * Copyright (c) 2023 Pin Loon Lee (pllee4)
 */

#include <iostream>
#include <vector>

class ClassWithGeneratedDefaultConstructor {
 public:
  int m_value1;
  double m_value2;
  std::string m_value3;
};

class ClassWithDefaultConstructor {
 public:
  // ClassWithDefaultConstructor() : m_value1(0), m_value2(0.0), m_value3("") {}
  ClassWithDefaultConstructor() = default;

  int m_value1{0};
  double m_value2{2.0};
  std::string m_value3{"Empty String"};
};

class DeletedDefaultConstructor {
  DeletedDefaultConstructor() = delete;
};

class PassengerInfo {
 public:
  // PassengerInfo() = default;
  PassengerInfo() = delete;
  PassengerInfo(int age) : m_age(age) {}
  PassengerInfo(std::string first_name, std::string last_name, int age)
      : m_first_name(first_name), m_last_name(last_name), m_age(age) {}

  std::string m_first_name{"First Name"};
  std::string m_last_name{"Last Name"};
  int m_age{20};
};

int main() {
  // ClassWithGeneratedDefaultConstructor example1;
  // std::cout << example1.m_value1 << std::endl;

  // std::vector<PassengerInfo> passenger_info_vec(1000);
  // PassengerInfo passenger_info_arr[1000];

  PassengerInfo passenger_info1(10);
  std::cout << passenger_info1.m_first_name << std::endl;
  std::cout << passenger_info1.m_last_name << std::endl;
  std::cout << passenger_info1.m_age << std::endl;

  PassengerInfo passenger_info2("FirstName", "LastName", 30);
  std::cout << passenger_info2.m_first_name << std::endl;
  std::cout << passenger_info2.m_last_name << std::endl;
  std::cout << passenger_info2.m_age << std::endl;
}