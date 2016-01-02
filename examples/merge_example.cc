#include <inttypes.h>
#include <openssl/sha.h>
#include <stdio.h>
#include "count/hll.h"

using libcount::HLL;

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
  const int kPrecision = 14;

  // Create two HLL objects to track set cardinality.
  HLL* hll_1 = HLL::Create(kPrecision);
  HLL* hll_2 = HLL::Create(kPrecision);

  // Count 'kIterations' elements with 'kTrueCardinality' cardinality. In our
  // simulation, each object will get unique items, so that when we perform
  // the merge operation, we can expect the estimated cardinality to be about
  // double the value of 'kTrueCardinality'.
  const uint64_t kIterations = 10000000;
  const int kTrueCardinality = 1000;
  for (int i = 0; i < kIterations; ++i) {
    hll_1->Update(hash(i % kTrueCardinality));
    hll_2->Update(hash((i % kTrueCardinality) + kTrueCardinality));
  }

  // Merge contents of hll_2 into hll_1.
  hll_1->Merge(hll_2);

  // Obtain the cardinality estimate.
  const uint64_t estimate = hll_1->Estimate();

  // Display results.
  printf("actual cardinality:    %d\n", kTrueCardinality * 2);
  printf("estimated cardinality: %" PRIuFAST64 "\n", estimate);

  // Delete object.
  delete hll_2;
  delete hll_1;

  return 0;
}
