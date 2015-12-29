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

#include "count/hll.h"
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "count/utility.h"

namespace {

using libcount::CountLeadingZeroes;

// Helper to calculate the index into the table of registers from the hash
inline int RegisterIndexOf(uint64_t hash, int precision) {
  return (hash >> (64 - precision));
}

// Helper to count the leading zeros (less the bits used for the reg. index)
inline uint8_t ZeroCountOf(uint64_t hash, int precision) {
  // Mask off the leading bits used for the index.
  const uint64_t mask = ~(((1 << precision) - 1) << (64 - precision));

  // Count zeroes, less the index bits we just masked off.
  return (CountLeadingZeroes(hash & mask) - static_cast<uint8_t>(precision));
} 

}  // namespace

namespace libcount {

const int HLL_MIN_PRECISION = 4;
const int HLL_MAX_PRECISION = 16;

HLL::HLL(int precision)
  : precision_(precision),
    updates_(0),
    register_count_(0),
    registers_(NULL) {
  // The precision is vetted by the Create() function.  Assertions nonetheless.
  assert(precision >= HLL_MIN_PRECISION);
  assert(precision <= HLL_MAX_PRECISION);

  // We employ (2 ^ precision) "registers" to store max leading zeroes.
  register_count_ = (1 << precision);

  // Allocate space for the registers. We can safely economize by using bytes
  // for the counters because we know the value can't ever exceed ~60.
  registers_ = new uint8_t[register_count_];
  memset(registers_, 0, register_count_ * sizeof(registers_[0]));
}

HLL::~HLL() {
  delete[] registers_;
}

HLL* HLL::Create(int precision, int* error) {
  if ((precision < HLL_MIN_PRECISION) || (precision > HLL_MAX_PRECISION)) {
    MaybeAssign(error, EINVAL);
    return NULL;
  }
  return new HLL(precision);
}

void HLL::Update(uint64_t hash) {
  // Which register will potentially receive the zero count of this hash?
  const int index = RegisterIndexOf(hash, precision_);
  
  // Count the zeroes for the hash, and add one, per the algorithm spec.
  const uint8_t count = ZeroCountOf(hash, precision_) + 1;

  // Update the appropriate register if the new count is greater than current.
  if (count > registers_[index]) {
    registers_[index] = count;
  }
}

uint64_t HLL::GetCardinalityEstimate() const {
  // TODO(tdial): Implement
  return 0;
}

}  // namespace libcount
