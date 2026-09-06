#include <array>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <type_traits>
#include <utility>

enum class MotorMode : int {
  IDLE = 0,
  POSITION = 1,
  VELOCITY = 2,
  TORQUE = 3,
};

enum class Driver1 : uint8_t {
  IDLE = 0,
  POSITION = 10,
  VELOCITY = 11,
  TORQUE = 12,
};

enum class Driver2 : uint8_t {
  IDLE = 0x00,
  POSITION_ABS = 0x10,     // ─┐
  POSITION_REL = 0x11,     // ─┤ all map to MotorMode::POSITION
  POSITION_MULTI = 0x12,   // ─┘
  VELOCITY_OPEN = 0x20,    // ─┐ both map to MotorMode::VELOCITY
  VELOCITY_CLOSED = 0x21,  // ─┘
  TORQUE_DIRECT = 0x30,    // 1:1 with MotorMode::TORQUE
};

template <auto From, typename To>
struct Remap;

template <auto From, typename To>
inline constexpr auto remap_17a = Remap<From, To>::value;

template <auto From, typename To>
inline constexpr auto remap_17a_v =
    static_cast<std::underlying_type_t<To>>(Remap<From, To>::value);

#define REMAP(from, to)               \
  template <>                         \
  struct Remap<from, decltype(to)> {  \
    static constexpr auto value = to; \
  };

#define REMAP_BIDIR(a, b) REMAP(a, b) REMAP(b, a)

// Driver 1
REMAP_BIDIR(MotorMode::IDLE, Driver1::IDLE)
REMAP_BIDIR(MotorMode::POSITION, Driver1::POSITION)
REMAP_BIDIR(MotorMode::VELOCITY, Driver1::VELOCITY)
REMAP_BIDIR(MotorMode::TORQUE, Driver1::TORQUE)

// Driver 2
REMAP_BIDIR(MotorMode::IDLE, Driver2::IDLE)
REMAP_BIDIR(MotorMode::TORQUE, Driver2::TORQUE_DIRECT)

REMAP_BIDIR(MotorMode::POSITION, Driver2::POSITION_ABS)
REMAP(Driver2::POSITION_REL, MotorMode::POSITION)
REMAP(Driver2::POSITION_MULTI, MotorMode::POSITION)

REMAP_BIDIR(MotorMode::VELOCITY, Driver2::VELOCITY_OPEN)
REMAP(Driver2::VELOCITY_CLOSED, MotorMode::VELOCITY)

template <typename Enum>
constexpr Enum from_value(std::underlying_type_t<Enum> v) {
  return static_cast<Enum>(v);
}

template <class Enum>
inline constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept {
  return static_cast<std::underlying_type_t<Enum>>(e);
}

// better C++17
template <typename A, typename B>
struct RemapTable;

// Detects whether RemapTable<A, B>::entries exists
template <typename A, typename B, typename = void>
struct has_remap_table : std::false_type {};

template <typename A, typename B>
struct has_remap_table<A, B,
    std::void_t<decltype(RemapTable<A, B>::entries)>> : std::true_type {};

// Compile-time: constexpr instead of consteval, trait instead of requires
template <auto From, typename To>
constexpr To remap_fn_17() {
  using F = decltype(From);
  if constexpr (has_remap_table<F, To>::value) {
    for (auto [a, b] : RemapTable<F, To>::entries)
      if (a == From) return b;
    throw "unmapped value (forward)";
  } else if constexpr (has_remap_table<To, F>::value) {
    for (auto [a, b] : RemapTable<To, F>::entries)
      if (b == From) return a;
    throw "unmapped value (reverse)";
  } else {
    throw "no remap table for this enum pair";
  }
}

// Runtime: same change — trait instead of requires
template <typename From, typename To>
To remap_rt_17(From value) {
  if constexpr (has_remap_table<From, To>::value) {
    for (auto [a, b] : RemapTable<From, To>::entries)
      if (a == value) return b;
  } else if constexpr (has_remap_table<To, From>::value) {
    for (auto [a, b] : RemapTable<To, From>::entries)
      if (b == value) return a;
  }
  throw std::runtime_error("unmapped enum value"); // or warning with some default value
}

// C++20 starts
template <auto From, typename To>
consteval To remap_fn() {
  using F = decltype(From);
  if constexpr (requires { RemapTable<F, To>::entries; }) {
    for (auto [a, b] : RemapTable<F, To>::entries)
      if (a == From) return b;
    throw "unmapped value (forward)";
  } else if constexpr (requires { RemapTable<To, F>::entries; }) {
    for (auto [a, b] : RemapTable<To, F>::entries)
      if (b == From) return a;
    throw "unmapped value (reverse)";
  } else {
    throw "no remap table for this enum pair";
  }
}

template <auto From, typename To>
inline constexpr auto remap = remap_fn<From, To>();

template <auto From, typename To>
inline constexpr auto remap_17 = remap_fn_17<From, To>();

// template <auto From, typename To>
// inline constexpr auto remap_v = std::to_underlying(remap<From, To>); // C++23
template <auto From, typename To>
inline constexpr auto remap_v = to_underlying(remap<From, To>);

template <auto From, typename To>
inline constexpr auto remap_v_17 = to_underlying(remap_17<From, To>);

template <typename From, typename To>
To remap_rt(From value) {
  if constexpr (requires { RemapTable<From, To>::entries; }) {
    for (auto [a, b] : RemapTable<From, To>::entries)
      if (a == value) return b;
  } else if constexpr (requires { RemapTable<To, From>::entries; }) {
    for (auto [a, b] : RemapTable<To, From>::entries)
      if (b == value) return a;
  }
  throw std::runtime_error(
      "unmapped enum value");  // or warning with some default value
}

template <>
struct RemapTable<MotorMode, Driver1> {
  struct E {
    MotorMode a;
    Driver1 b;
  };
  static constexpr E entries[] = {
      {MotorMode::IDLE, Driver1::IDLE},
      {MotorMode::POSITION, Driver1::POSITION},
      {MotorMode::VELOCITY, Driver1::VELOCITY},
      {MotorMode::TORQUE, Driver1::TORQUE},
  };
};

template <>
struct RemapTable<MotorMode, Driver2> {
  struct E {
    MotorMode a;
    Driver2 b;
  };
  static constexpr E entries[] = {
      {MotorMode::IDLE, Driver2::IDLE},
      {MotorMode::TORQUE, Driver2::TORQUE_DIRECT},
      // first match wins forward, any match works reverse
      {MotorMode::POSITION, Driver2::POSITION_ABS},  // default
      {MotorMode::POSITION, Driver2::POSITION_REL},
      {MotorMode::POSITION, Driver2::POSITION_MULTI},
      {MotorMode::VELOCITY, Driver2::VELOCITY_OPEN},  // default
      {MotorMode::VELOCITY, Driver2::VELOCITY_CLOSED},
  };
};

// C++20 ends

int main() {
  // === 1:1 bidirectional: MotorMode <-> Driver1 ===
  printf("=== 1:1 MotorMode <-> Driver1 ===\n");
  printf("IDLE:     MotorMode(%d) -> Driver1(%d) -> MotorMode(%d)\n",
         to_underlying(MotorMode::IDLE), remap_v<MotorMode::IDLE, Driver1>,
         to_underlying(remap<Driver1::IDLE, MotorMode>));
  printf("POSITION: MotorMode(%d) -> Driver1(%d) -> MotorMode(%d)\n",
         to_underlying(MotorMode::POSITION),
         remap_v<MotorMode::POSITION, Driver1>,
         to_underlying(remap<Driver1::POSITION, MotorMode>));
  printf("VELOCITY: MotorMode(%d) -> Driver1(%d) -> MotorMode(%d)\n",
         to_underlying(MotorMode::VELOCITY),
         remap_v<MotorMode::VELOCITY, Driver1>,
         to_underlying(remap<Driver1::VELOCITY, MotorMode>));
  printf("TORQUE:   MotorMode(%d) -> Driver1(%d) -> MotorMode(%d)\n",
         to_underlying(MotorMode::TORQUE), remap_v<MotorMode::TORQUE, Driver1>,
         to_underlying(remap<Driver1::TORQUE, MotorMode>));

  // === 1:1 bidirectional: MotorMode <-> Driver2 (IDLE, TORQUE) ===
  printf("\n=== 1:1 MotorMode <-> Driver2 ===\n");
  printf("IDLE:   MotorMode(%d) -> Driver2(0x%02X) -> MotorMode(%d)\n",
         to_underlying(MotorMode::IDLE), remap_v<MotorMode::IDLE, Driver2>,
         to_underlying(remap<Driver2::IDLE, MotorMode>));
  printf("TORQUE: MotorMode(%d) -> Driver2(0x%02X) -> MotorMode(%d)\n",
         to_underlying(MotorMode::TORQUE), remap_v<MotorMode::TORQUE, Driver2>,
         to_underlying(remap<Driver2::TORQUE_DIRECT, MotorMode>));

  // === 1:N forward default: MotorMode -> Driver2 picks default variant ===
  printf("\n=== 1:N forward (MotorMode -> Driver2 default) ===\n");
  printf("POSITION -> Driver2(0x%02X)  [default: POSITION_ABS]\n",
         remap_v<MotorMode::POSITION, Driver2>);
  printf("VELOCITY -> Driver2(0x%02X)  [default: VELOCITY_OPEN]\n",
         remap_v<MotorMode::VELOCITY, Driver2>);

  // === N:1 reverse: all Driver2 variants collapse to one MotorMode ===
  printf("\n=== N:1 reverse (Driver2 variants -> MotorMode) ===\n");
  printf("POSITION_ABS(0x%02X)   -> MotorMode(%d)\n",
         to_underlying(Driver2::POSITION_ABS),
         to_underlying(remap<Driver2::POSITION_ABS, MotorMode>));
  printf("POSITION_REL(0x%02X)   -> MotorMode(%d)\n",
         to_underlying(Driver2::POSITION_REL),
         to_underlying(remap<Driver2::POSITION_REL, MotorMode>));
  printf("POSITION_MULTI(0x%02X) -> MotorMode(%d)\n",
         to_underlying(Driver2::POSITION_MULTI),
         to_underlying(remap<Driver2::POSITION_MULTI, MotorMode>));
  printf("VELOCITY_OPEN(0x%02X)  -> MotorMode(%d)\n",
         to_underlying(Driver2::VELOCITY_OPEN),
         to_underlying(remap<Driver2::VELOCITY_OPEN, MotorMode>));
  printf("VELOCITY_CLOSED(0x%02X)-> MotorMode(%d)\n",
         to_underlying(Driver2::VELOCITY_CLOSED),
         to_underlying(remap<Driver2::VELOCITY_CLOSED, MotorMode>));

  // === Same MotorMode, different hardware output ===
  printf("\n=== Same command, different hardware ===\n");
  printf("MotorMode::TORQUE -> Driver1(%d), Driver2(0x%02X)\n",
         remap_v<MotorMode::TORQUE, Driver1>,
         remap_v<MotorMode::TORQUE, Driver2>);
  printf("MotorMode::POSITION -> Driver1(%d), Driver2(0x%02X)\n",
         remap_v<MotorMode::POSITION, Driver1>,
         remap_v<MotorMode::POSITION, Driver2>);

  // === from_value: raw byte from hardware -> MotorMode ===
  printf("\n=== from_value: raw hardware byte -> MotorMode ===\n");
  constexpr auto m1 =
      remap<from_value<Driver2>(0x11), MotorMode>;  // POSITION_REL -> POSITION
  constexpr auto m2 = remap<from_value<Driver2>(0x21),
                            MotorMode>;  // VELOCITY_CLOSED -> VELOCITY
  constexpr auto m3 =
      remap<from_value<Driver1>(12), MotorMode>;  // TORQUE -> TORQUE
  printf("Driver2 byte 0x11 -> MotorMode(%d)  [POSITION]\n", to_underlying(m1));
  printf("Driver2 byte 0x21 -> MotorMode(%d)  [VELOCITY]\n", to_underlying(m2));
  printf("Driver1 byte 12   -> MotorMode(%d)  [TORQUE]\n", to_underlying(m3));

  // === Simulated register read/write ===
  printf("\n=== Simulated register read/write ===\n");
  uint8_t hw_reg = 0;

  // Write: software command -> hardware register
  hw_reg = remap_v<MotorMode::VELOCITY, Driver2>;
  printf("Write Driver2: set velocity mode -> reg = 0x%02X\n", hw_reg);

  // Read: hardware register -> software enum
  hw_reg = 0x12;  // hardware reports POSITION_MULTI
  constexpr auto mode = remap<from_value<Driver2>(0x12), MotorMode>;
  printf("Read Driver2:  reg = 0x%02X -> MotorMode(%d) [POSITION]\n", hw_reg,
         to_underlying(mode));

  const auto test = [](int value) {
    // Cross-driver: Driver2 reading -> Driver1 equivalent (via MotorMode)
    {
      constexpr auto d1_equiv =
          remap_17<remap_17<from_value<Driver2>(0x30), MotorMode>, Driver1>;  // 0x30

      printf("Cross: Driver2(0x30) -> MotorMode -> Driver1(%d)\n",
             to_underlying(d1_equiv));
    }

    auto d1_equiv = remap_rt<MotorMode, Driver1>(
        remap_rt<Driver2, MotorMode>(from_value<Driver2>(value)));
    printf("Cross: Driver2(0x30) -> MotorMode -> Driver1(%d)\n",
           to_underlying(d1_equiv));
  };

  test(0x30);  // crash on runtime for invalid value
  //     auto value = Driver2(1000);
  //     // after convert, missing conversion
}