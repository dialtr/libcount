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

#include "count/empirical_data.h"
#include <stdio.h>
#include <stdlib.h>
#include "count/hll_limits.h"

using libcount::HLL_MIN_PRECISION;
using libcount::HLL_MAX_PRECISION;

// Ensure that the raw estimates in the empirical data tables are in ascending
// order. The interpolation algorithm depends on this being true.
int raw_estimates_are_increasing() {
  for (int i = HLL_MIN_PRECISION; i <= HLL_MAX_PRECISION; ++i) {
    const double* const estimates = RAW_ESTIMATE_DATA[i];
    const int NUM_ENTRIES = ValidTableEntries(estimates, 201);
    for (int j = 1; j < NUM_ENTRIES; ++j) {
      const double left = estimates[j - 1];
      const double right = estimates[j];
      if (right < left) {
        fprintf(stderr,
                "In table for precision = %d:\n"
                "estimate at index %d was less than at index %d",
                i, j, j - 1);
        fflush(stderr);
        return EXIT_FAILURE;
      }
    }
  }
  return EXIT_SUCCESS;
}

// Run all empirical data tests and fail immediately if a test fails.
int main(int argc, char* argv[]) {
  int status = 0;
  status = raw_estimates_are_increasing();
  if (status != EXIT_SUCCESS) {
    return status;
  }
  return status;
}
