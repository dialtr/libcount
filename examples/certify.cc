// Copyright 2015 The libcount Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License. See the AUTHORS file for names of
// contributors.

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <openssl/sha.h>
#include <stdlib.h>
#include <iostream>
#include "count/hll.h"
#include "count/hll_limits.h"

using libcount::HLL;
using libcount::HLL_MIN_PRECISION;
using libcount::HLL_MAX_PRECISION;
using std::cout;
using std::endl;

uint64_t hash(int i) {
  // Structure that is 160 bits wide used to extract 64 bits from a SHA-1.
  struct hashval {
    uint64_t high64;
    char low96[12];
  } hash;

  // Calculate the SHA-1 hash of the integer.
  SHA_CTX ctx;
  SHA1_Init(&ctx);
  SHA1_Update(&ctx, (unsigned char*)&i, sizeof(i));
  SHA1_Final((unsigned char*)&hash, &ctx);

  // Return 64 bits of the hash.
  return hash.high64;
}

struct TestResults {
  int precision;
  uint64_t size;
  uint64_t cardinality;
  uint64_t estimate;
  double percent_error;
};

int certify(int precision, uint64_t size, uint64_t cardinality,
            TestResults* results) {
  assert(results != NULL);
  // The precision must be 4..18 inclusive.
  if ((precision < HLL_MIN_PRECISION) || (precision > HLL_MAX_PRECISION)) {
    return EINVAL;
  }

  // The cardinality must be 1..size inclusive.
  if ((cardinality == 0) || (cardinality > size)) {
    return EINVAL;
  }

  // Initialize the results structure.
  results->precision = precision;
  results->size = size;
  results->cardinality = cardinality;
  results->estimate = 0;
  results->percent_error = 0.0;

  HLL* hll = HLL::Create(precision);
  if (!hll) {
    return EINVAL;
  }

  // Push 'size' elements through the counter. We are guaranteed exactly
  // 'cardinality' unique hash values.
  for (uint64_t i = 0; i < size; ++i) {
    hll->Update(hash(i % cardinality));
  }

  // Calculate the estimate
  results->estimate = hll->Estimate();

  // Calculate percentage difference.
  const double actual = static_cast<double>(results->cardinality);
  const double estimated = static_cast<double>(results->estimate);
  const double percent = ((estimated - actual) / actual) * 100.0;

  // Report percentage error.
  results->percent_error = percent;

  // Cleanup.
  delete hll;

  return 0;
}

int main(int argc, char* argv[]) {
  for (int p = HLL_MIN_PRECISION; p <= HLL_MAX_PRECISION; ++p) {
    (void)p;
  }
  return 0;
}
