#include <inttypes.h>
#include <stdio.h>
#include "hll.h"
#include "hll_debug.h"

int main(int argc, char* argv[]) {
  int result = HLL_test(stderr);
  if (result != 0) {
    fprintf(stderr, "** TESTS FAILED **\n");
  }
  return 0;
}
