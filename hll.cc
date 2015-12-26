#include "hll.h"
#include "hll_debug.h"

extern "C" {

struct HLL_CTX {
  int x;
};

HLL_CTX* HLL_init(uint64_t precision) {
  return NULL;
}

int HLL_update(HLL_CTX*, uint64_t element_hash) {
  // TODO: Implement
  return -1;
}

int HLL_cardinality(const HLL_CTX* ctx, uint64_t* cardinality) {
  // TODO: Implement
  return -1;
}

void HLL_free(HLL_CTX* ctx) {
  // TODO: Implement
}

void HLL_debug_print(FILE* file, HLL_CTX* ctx) {
  // TODO: Implement
}

}  // extern "C"

