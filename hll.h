#ifndef HLL_H_
#define HLL_H_

#include <inttypes.h>

extern "C" {

// Opaque structure used to track a cardinality estimate.
struct HLL_CTX;

// Initialize a context structure for estimating the cardinality of a set.
// The 'precision' argument is an integer in the range [4..16] inclusive
// that governs the number of buckets to use in the stochastic averaging
// algorithm that is used to improve estimates.
HLL_CTX* HLL_init(uint64_t precision);

// Update the counter with an element from the set.
int HLL_update(HLL_CTX*, uint64_t element_hash);

// Obtain an estimate of the cardinality of the set.
int HLL_cardinality(const HLL_CTX* ctx, uint64_t* cardinality);

// Free a context previously allocated with HLL_init().
void HLL_free(HLL_CTX* ctx);

}  // extern "C"

#endif
