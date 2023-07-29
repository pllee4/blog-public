/*
 * capture_in_lambda.cpp
 *
 * Created on: Jul 22, 2023 12:09
 * Description:
 *
 * Copyright (c) 2023 Pin Loon Lee (pllee4)
 */

#include <functional>
#include <iostream>
#include <memory>

class TestLambdaCapture {
 public:
  using LambdaFunction = std::function<int()>;

  explicit TestLambdaCapture(const int value) : value_(value) {}

  LambdaFunction GetDefaultByReferenceCapture() const {
    const auto func = [&]() { return value_; };
    return func;
  }

  // LambdaFunction GetDefaultByValueCapture() const {
  //   const auto func = [=]() { return value_; };
  //   return func;
  // }

  LambdaFunction GetDefaultByValueCapture() const {
    auto object_ptr = this;
    const auto func = [object_ptr]() { return object_ptr->value_; };
    return func;
  }

  LambdaFunction GetGeneralizedLambdaCapture() const {
    const auto func = [value = value_]() { return value; };
    return func;
  }

  LambdaFunction GetCopyOfThisLambdaCapture() const {
    const auto func = [*this]() { return value_; };
    return func;
  }

  void SetValue(int value) { value_ = value; }

 private:
  int value_;
};

class LambdaClosure {
 public:
  auto operator()() const { std::cout << "A simple lambda" << std::endl; }
};

class LambdaClosureWithCapture {
 public:
  LambdaClosureWithCapture(const int &value) : value_{value} {}
  auto operator()() const { std::cout << "Value: " << value_ << std::endl; }

 private:
  int value_;
};

int number = 100;
int main() {
  // auto simpleLambda{[] { std::cout << "A simple lambda" << std::endl; }};
  LambdaClosure simpleLambda;
  simpleLambda();

  int value = 100;
  // auto lambdaWithCapture { [value]() {
  //   std::cout << "Value: " << value << std::endl;
  // }};
  // lambdaWithCapture();

  // LambdaClosureWithCapture lambdaWithCapture(100);

  // auto lambdaWithCapture{[value]() mutable {
  //   value *= 2;
  //   std::cout << "Value: " << value << std::endl;
  // }};
  // lambdaWithCapture();

  auto lambdaWithCapture{[&]() {
    value *= 2;
    std::cout << "Value: " << value << std::endl;
  }};
  lambdaWithCapture();
  std::cout << "Original value: " << value << std::endl;

  TestLambdaCapture::LambdaFunction func1, func2, func3, func4, func5;
  // {
    TestLambdaCapture test(2);
    func1 = test.GetDefaultByReferenceCapture();
    func2 = test.GetDefaultByValueCapture();
    func3 = test.GetGeneralizedLambdaCapture();
    func4 = test.GetCopyOfThisLambdaCapture();

    auto test5 = std::make_shared<TestLambdaCapture>(2);
    func5 = test5->GetDefaultByValueCapture();
  // }

  const auto printValue = [func1, func2, func3, func4, func5]() {
    std::cout << "func1 " << func1() << std::endl;
    std::cout << "func2 " << func2() << std::endl;
    std::cout << "func3 " << func3() << std::endl;
    std::cout << "func4 " << func4() << std::endl;
    std::cout << "func5 " << func5() << std::endl;
  };

  printValue();

  auto captureGlobal{[=] { number = 200; }};
  captureGlobal();
  std::cout << "Global number: " << number << std::endl;
  return 0;
}