#include <stdio.h>

extern double __toplevel(void);

int main() {
  printf("result: %f\n", __toplevel());
  return 0;
}