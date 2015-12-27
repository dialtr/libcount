#include <inttypes.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
  printf("sizeof(uint64_t) = %lu\n", sizeof(uint64_t));
  
  const uint64_t kOne = 1;
  const uint64_t value = kOne << 63;

  const uint64_t value2 = (1 << 63);
  return 0;
}

