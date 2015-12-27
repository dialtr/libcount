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
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "hll.h"
#include "hll_debug.h"

void FillBufferWithRandomLetters(char* buffer, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    buffer[i] = 'A' + rand() % 26;
  }
  buffer[count] = 0;
}

int main(int argc, char* argv[]) {
  // Run unit tests
  int result = HLL_test(stderr);
  if (result != 0) {
    fprintf(stderr, "** TESTS FAILED **\n");
    return EXIT_FAILURE;
  }

  //
  // Perform a slightly more "realistic" test.
  //

  // We'll create a set of kIterations strings, each of length kStringLen,
  // where each string is comprised of uppercase letters from the alpahbet.
  // Thus, for kStringLen = 4, there should be approximately 1/2 million
  // unique values in the population of kIterations elements.
  const int kIterations = 10000000;
  const int kStringLen = 4;
  
  // Initialize a HyperLogLog context structure.
  HLL_CTX* ctx = HLL_init(4);
  if (!ctx) {
    fprintf(stderr, "** HLL_init() failed, returned errno %d\n", errno);
    return EXIT_FAILURE;
  }

  // Run a series of values of predictable cardinality through the system.
  char buffer[kStringLen + 1];
  for (int i = 0; i < kIterations; ++i) {
    FillBufferWithRandomLetters(buffer, kStringLen);
  }

  // Free the context structure.
  HLL_free(ctx);

  return EXIT_SUCCESS;
}
