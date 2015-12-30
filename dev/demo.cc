// Copyright 2015 The libhll Authors.
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

#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <set>
#include <string>
#include "count/hll.h"
#include "count/hllc.h"
#include "count/hllc_debug.h"

using std::cerr;
using std::endl;
using libcount::HLL;

void FillBufferWithRandomLetters(char* buffer, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    buffer[i] = 'A' + rand() % 26;
  }
  buffer[count] = 0;
}

uint64_t HashString(const char* buffer) {
  // The hashval structure is large enough to hold 160 bits, the size
  // needed to hold a SHA-1 hash. We compute the SHA-1 using the
  // standard OpenSSL functions, using the address of a hashval to
  // hold the result. We then grab the first 64 bits of the hash by
  // addressing the h64 member.
  struct hashval {
    uint64_t h64;
    unsigned char l96[12];
  };

  const size_t len = strlen(buffer);
  SHA_CTX ctx;
  SHA1_Init(&ctx);
  SHA1_Update(&ctx, buffer, len);
  hashval hv = {0};
  SHA1_Final((unsigned char*)&hv, &ctx);
  return hv.h64;
}

int main(int argc, char* argv[]) {
  // Run unit tests
  int result = HLL_test();
  if (result != 0) {
    cerr << "** TESTS FAILED **\n" << endl;
    return EXIT_FAILURE;
  }

  //
  // Perform a slightly more "realistic" test.
  //

  // Randomly seed the PRNG
  srand(time(NULL));

  // We'll create a set of kIterations strings, each of length kStringLen,
  // where each string is comprised of uppercase letters from the alpahbet.
  // Thus, for kStringLen = 4, there should be approximately 1/2 million
  // unique values in the population of kIterations elements.
  const int kIterations = 100000000;
  const int kStringLen = 5;

  // Initialize a HyperLogLog context structure.
  const int kPrecision = 8;
  HLL_CTX* ctx = HLL_init(kPrecision);
  if (!ctx) {
    cerr << "** HLL_init() failed, returned errno " << errno << endl;
    return EXIT_FAILURE;
  }

  // Initialize a C++ HyperLogLog instance
  HLL* hll = HLL::Create(kPrecision);

  // Run a series of values of predictable cardinality through the system.
  char buffer[kStringLen + 1];
  for (int i = 0; i < kIterations; ++i) {
    // Create a random element
    FillBufferWithRandomLetters(buffer, kStringLen);

    // Calculate a 64 bit hash of the element.
    const uint64_t hash = HashString(buffer);

    // Update the HLL context (and C++ instance) with the element
    HLL_update(ctx, hash);
    hll->Update(hash);
  }

  // Calculate the upper bound of the cardinality possible for the test.
  // The upper bound is just the number of possible strings that we could
  // have generated.
  const uint64_t upper_bound = static_cast<uint64_t>(
    (pow(26.0f, static_cast<double>(kStringLen))));

  // Get an estimate of the cardinality using the library
  uint64_t estimate = 0.0f;
  HLL_cardinality(ctx, &estimate);

  uint64_t estimate_cpp = hll->GetCardinalityEstimate();

  cerr << endl
       << "cardinality upper bound:  " << upper_bound << endl
       << "cardinality estimate:     " << estimate << endl
       << "C++ cardinality estimate: " << estimate_cpp << endl;

  // Free the context structure.
  HLL_free(ctx);

  // Delete C++ instance
  delete hll;

  return EXIT_SUCCESS;
}
