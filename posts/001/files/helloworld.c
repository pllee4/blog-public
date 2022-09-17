// 64bit
void print() {
  register char* arg2 asm("rsi") = "Hello world!\n";
  asm("mov $1,%%rax \n\t"
      "mov $1,%%rdi \n\t"
      "mov $13,%%rdx \n\t"
      "syscall \n\t" ::
          : "rdi", "rdx");
}

// write(STDOUT_FILENO,
//   "Hello world!\n",
//   sizeof("Hello world!\n")
// );

void exit() {
  asm("mov $0,%rdi \n\t"
      "mov $60,%rax \n\t"
      "syscall \n\t");
}

// exit(EXIT_SUCCESS);

// 32bit
// void print() {
//   asm("movl $13,%%edx \n\t"
//       "movl $0,%%ecx \n\t"
//       "movl $0,%%ebx \n\t"
//       "movl $4,%%eax \n\t"
//       "int $0x80 \n\t" ::"r"(str)
//       : "edx", "ecx", "ebx");
// }

// void exit(){
//   asm("movl $42,%ebx \n\t"
//       "movl $1,%eax \n\t"
//       "int $0x80 \n\t");
// }

void nomain() {
  print();
  exit();
}

// int main(void) {
// register int    syscall_no  asm("rax") = 1;
// register int    arg1        asm("rdi") = 1;
// register char*  arg2        asm("rsi") = "Hello world!\n";
// register int    arg3        asm("rdx") = 13;
// asm("syscall");
// return 0;
// }