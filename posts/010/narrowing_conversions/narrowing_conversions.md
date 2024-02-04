# Narrowing Conversions in C++
Can you spot any errors in the code below?

## Motivation
The story began when I saw a code change similar to the one below
```
  const auto id_1 = 1u;
  unsigned char id_2 = 2u; // original code was "const auto id_2"
  const auto ids = std::set<uint8_t>{id_1, id_2};
  // some code that would change value of id_2
```

As a C++ programmer, my first instinct was "Come on, I know you want to make some changes on id_2, why don't use auto explicitly instead?"

However, upon making this change:
```
auto id_2 = 2u;
```
I received a warning:

```
warning: narrowing conversion of ‘id_2’ from ‘unsigned int’ to ‘unsigned char’ [-Wnarrowing]
```

Hmm... it seems like the programmer who made the code change must have seen the warning before and thus changed the type to `unsigned char`.

This raised a crucial question: Why does narrowing conversion occur?

## Unpacking Narrowing Conversions

Narrowing conversions are potentially unsafe numeric conversions where the destination type may not accommodate all values of the source type. It's essential to avoid narrowing conversions whenever possible. Let's categorize these conversions into two main types: from floating point to integral types and vice versa.

After searching, reading and testing, I have gathered some information about the narrowing conversion which would explain the scenario by the end of this post.

Here's the summary I have made for the "rules" of conversion, and we will go through the conditions with relevant examples.

Before diving into examples, let's understand the examples and rank of integral types and floating point types:

Rank of integral types
```
  bool < char < short < int < long < long long
```

Rank of floating point types
```
   float < double < long double
```

## Conversion from Floating Point Type
Starting with conversion from floating point to integral types, any such conversion is considered narrowing. For instance,

```
void FloatingToIntegral() {
  double d;
//  float d;
//  long double d;

  int i{d};
  uint8_t u8{d};
  long l{d};
  short s{d};
  char c{d};
  unsigned int ui{d};
}
```

However, converting from floating point to floating point types is permissible when the destination is a higher-ranked floating point type:

```
void FloatingToFloating() {
  float f{1.2345f};  // must explicit f for -Wconversion
  double d{f};
}
```

Note that suffix `f` is added when initialize `f` to indicate it is a type of `float` instead the normal default of `double`, this is needed to when the flag of `-Wconversion` is set, otherwise it would warn about change of value due to float conversion as shown below
```
warning: conversion from ‘double’ to ‘float’ changes value from ‘1.2344999999999999e+0’ to ‘1.23450005e+0f’ [-Wfloat-conversion]
```

Narrowing conversion occurs when converting to a lower-ranked floating point type:
```
  double d{1.2345678};
  float f{d};  // narrowing conversions
```

Nevertheless, there is one caveat when the flag `-Wconversion` is not set.
The conversion is allowed if the value is converted from `constexpr` (assume only `constexpr` for now, we would discuss more about this later) and it is in range of the destination type (even if the destination type has lower precision to store the value). Thus, there is no warning for
```
  constexpr double d{1.2345678};  // won't cause warning if there is no -Wconversion
  float f{d};
```

Do note that the original type has to be in range of destination type.

For instance, if the value of `double` is out of range of `float`, it would still be considered as narrowing conversion.
```
  constexpr double d{
      std::numeric_limits<double>::max()};  // narrowing conversions for
                                            // destination f
  float f{d};
```

## Conversion from Integral Type
Moving on to conversion from integral types, the rules become more complicated.
If an integral type is converted to floating point type, usually it would be treated as narrowing conversion.
The exception would be similar to the previously discussed case when `-Wconversion` flag is not set during compilation, and the converted value could be stored in destination type.

```
void IntegralToFloating() {
  constexpr short int i{std::numeric_limits<short int>::max()};
  float f{i};
}
```
Narrowing conversion warning would still be triggered when the code is changed to 
```
  constexpr int i{std::numeric_limits<int>::max()};  // narrowing conversions
                                                     // for destination f
  float f{i};
```
since `float` type is not able to store the maximum value of `int`.

The conversion from integral type to integral type has more cases to be considered as integral type could be signed and unsigned. In general,

*"The rank of any unsigned integer type shall equal the rank of the corresponding signed integer type, if any."*

Similar to the conversion from floating point type to floating point type, it would not be considered as narrowing conversion if conversion happens from integral type to another integral type with higher rank, with the condition that it can represent all values of the original type.
It works fine as expected for
```
void IntegralToIntegral() {
  unsigned short int value{25};
  long int i{value};
}
```
Extra caution is required when the destination type is an unsigned.
```
  short int value{25};  // narrowing conversions for destination that is unsigned
  unsigned int i{value};
```

Even though `int` has higher rank than `short int`, since the destination is `unsigned`, it could not represent the negative values from `short int`, thus this would still be considered as narrowing conversion.

I guess you might be wondering, would the `constexpr` help?

Yes, you have the correct guess. Changing to `constexpr` would help, and it does work even if `Wconversion` flag is set.

```
  constexpr short int value{25};
  unsigned int i{value};
```
However, there is one additional rule, the original value must be able to be stored exactly in destination.
To explain further, the code below would still trigger the warning for narrowing conversion when a negative value could not be stored exactly in the destination type.
```
  constexpr short int value{-25};  // narrowing conversions for destination that
                                   //  could not store negative values
  unsigned int i{value};
```

## Insight
We have completed both crucial components for narrowing conversion. Now let's reveal something interesting.
For simplicity, all the examples above used `constexpr` and explicit declaration for the type.
Would `const` work for the case of `constexpr` ? Yes it worked when I tested! In fact, the `const` seems to be the one doing the job when I coded with `consteval` in C++20.

Narrowing conversion is not triggered for
```
template <typename T>
consteval T GetConsteval() {
  return std::numeric_limits<T>::max();
}

void IntegralToFloating() {
  float f{GetConsteval<short>()};
}
```
Now that we have gone through different kinds of examples to understand narrowing conversion, let's revisit the initial example.

```
  const auto id_1 = 1u;
  unsigned char id_2 = 2u; // original code was "const auto id_2"
  const auto ids = std::set<uint8_t>{id_1, id_2};
  // some code that would change value of id_2
```

We can notice that the original code with `const` for `id_2` prevented the warning of narrowing conversion.
When the `auto` was used, specially with suffix `u`, the deduced type was `unsigned int`. (You can check the
type by using `typeid(<type to be checked>).name()` which notably for gcc and clang, returns the mangled name, as specified by the [Itanium C++ ABI](https://itanium-cxx-abi.github.io/cxx-abi/abi.html#mangling-type))
Hence in original code, there was actually conversion from `unsigned int` to `uint8_t` but it might not be obvious at first glance without throwing a warning when `const` was used. Since `uint8_t` was treated as `unsigned char` (both have having same size of 1 byte), the compiler complained about the narrowing conversion from `unsigned int` to `unsigned char` with the code change of removing `const`. This is why `auto` won't work in this scenario and the programmer chose to explicit use `unsigned char`, although I personally feel it would be clearer to use `uint8_t` instead.

Anyway, thanks to the type used which is not default `int`, I could take a detailed look at narrowing conversion.
If the code was written in the way below, no warning would be raised, and hope you know why by now! And...by DEFAULT(hint :shushing_face)!

```
void ThanksForReadingTillTheEnd() {
  const auto id_1 = 1;
  auto id_2 = 2;
  const auto ids = std::set<int>{id_1, id_2};
  // some code that would change value of id_2
}
```

References:
https://www.learncpp.com/cpp-tutorial/narrowing-conversions-list-initialization-and-constexpr-initializers/
https://clang.llvm.org/extra/clang-tidy/checks/cppcoreguidelines/narrowing-conversions.html
https://www.oreilly.com/library/view/c-in-a/0596006977/ch04.html