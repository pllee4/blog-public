/*
 * pass_key.cpp
 *
 * Created on: Apr 26, 2025 21:37
 * Description:
 * References:
 * https://accu.org/journals/overload/31/176/mertz/#:~:text=as%20it%20pleases.-,The%20passkey%20idiom,create%20a%20little%20key%20token.
   https://www.modernescpp.com/index.php/argument-dependent-lookup-and-hidden-friends/
   https://www.youtube.com/watch?v=zvWCgiVvpPU
 * Copyright (c) 2025 Pin Loon Lee (pllee4)
 */

#include <cstdint>
#include <iostream>

// hidden friend Argument Dependant Lookup (ADL)
class Coordinate {
 public:
  Coordinate(int x, int y) : x_(x), y_(y){};

  friend std::ostream& operator<<(std::ostream& os,
                                  const Coordinate& coordinate) {
    return os << coordinate.x_ << " " << coordinate.y_;
  }

 private:
  int x_;
  int y_;
};

// Generalized Passkey Idiom
// int OnlyFooCanAccess(Passkey<Foo>);
class ConnectionFactory;

class PasskeyBase {  // deal with aggregate initialization madness
 protected:
  constexpr PasskeyBase() = default;
};

template <typename T>
class Passkey : PasskeyBase {
  friend T;
  constexpr Passkey() = default;
  constexpr Passkey(const Passkey&) = delete;  // copy assignment operator
  constexpr Passkey& operator=(const Passkey&) =
      delete;  // copy assignment constructor

  constexpr Passkey(Passkey&&) noexcept = delete;  // move assignment operator
  constexpr Passkey& operator=(Passkey&&) noexcept = delete;
};

// template <typename T>
// class Passkey {
//   friend T;
//   constexpr Passkey() = default;
// //   int dummy; // indeterminate
// };

class ConstructorKey {
  friend class ConnectionFactory;
  constexpr ConstructorKey(){};
  constexpr ConstructorKey(const ConstructorKey& other) =
      delete;  // constructor
};

class Connection {
 public:
  Connection(const uint16_t port, ConstructorKey) : port_(port) {}

 private:
  void InternalSetterFunction(){
      // internal function
  };

  uint16_t port_;
  bool internal_flag;
};

class ConnectionFactory {
 public:
  Connection createConnection(const uint16_t port) {
    return Connection(port, {});
  }
};

int main() {
  // Connection connection{123, {}};  // ConstructorKey::ConstructorKey()  is
  // private
  ConnectionFactory factory;
  Connection conn = factory.createConnection(1883);  // OK
  // ConstructorKey* hack = nullptr;
  // Connection hack_connection(80, *hack);

  std::cout << Coordinate(2, 1) << std::endl;
}