#include <iostream>

// __attribute__((warn_unused_result)) 
// int Init() {
//   return 0;
// }

// __attribute__((deprecated)) int Init() {
  
// }

#define __WARN(msg) __WARN_GCC(GCC warning msg)
#define __WARN_GCC(s) _Pragma(#s)

#ifndef __DEPRECATED_MACRO
#define __DEPRECATED_MACRO __WARN("Macro is deprecated")
#endif

#define MAX(a,b) (a > b? a: b) 
// #define MAX(a,b) __DEPRECATED_MACRO std::max(a,b)

__attribute__((__weak__)) int Init() { 
  std::cout << "Init is not supported" << std::endl;
  return 0; 
}

int main(int argc, char** argv) {
  Init();
  std::cout << MAX(9, 10) << std::endl;
  return 0;
}