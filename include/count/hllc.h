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

#ifndef INCLUDE_COUNT_HLLC_H_
#define INCLUDE_COUNT_HLLC_H_

#include <inttypes.h>

extern "C" {

// Opaque structure used to track a cardinality estimate.
struct HLL_CTX;

// Initialize a context structure for estimating the cardinality of a set.
// The 'precision' argument is an integer in the range [4..16] inclusive
// that governs the number of buckets to use in the stochastic averaging
// algorithm that is used to improve estimates.
HLL_CTX* HLL_init(int precision);

// Update the counter with an element from the set.
int HLL_update(HLL_CTX* ctx, uint64_t element_hash);

// Obtain an estimate of the cardinality of the set.
int HLL_cardinality(const HLL_CTX* ctx, uint64_t* cardinality);

// Free a context previously allocated with HLL_init().
void HLL_free(HLL_CTX* ctx);

}  // extern "C"

#endif  // INCLUDE_COUNT_HLLC_H_
