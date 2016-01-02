#include <inttypes.h>
#include <openssl/sha.h>
#include <stdio.h>
#include "count/c.h"

uint64_t hash(int i) {
  // Structure that is 160 bits wide used to extract 64 bits from a SHA-1.
  struct hashval {
    uint64_t high64;
    char     low96;
  } hash;

  // Calculate the SHA-1 hash of the integer.
  SHA_CTX ctx;
  SHA1_Init(&ctx);
  SHA1_Update(&ctx, (unsigned char*)&i, sizeof(i));
  SHA1_Final((unsigned char*)&hash, &ctx);

  // Return 64 bits of the hash.
  return hash.high64;
}

int main(int argc, char* argv[]) {
  const int kPrecision = 9;
  int error = 0;

  // Create an HLL object to track set cardinality.
  hll_t* hll = HLL_create(kPrecision, &error);

  // Count 'kIterations' elements with 'kTrueCardinality' cardinality.
  const uint64_t kIterations = 1000000;
  const int kTrueCardinality = 100;
  for (int i = 0; i < kIterations; ++i) {
    HLL_update(hll, hash(i % kTrueCardinality));
  }

  // Obtain the cardinality estimate.
  const uint64_t estimate = HLL_estimate(hll);

  // Display results.
  printf("actual cardinality:    %d\n", kTrueCardinality);
  printf("estimated cardinality: %" PRIuFAST64 "\n", estimate);

  // Free object.
  HLL_free(hll);

  return 0;
}
