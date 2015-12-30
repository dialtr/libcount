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
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "count/utility.h"

namespace {

// The minimum and maximum allowed precision values.
const int HLL_MIN_PRECISION = 4;
const int HLL_MAX_PRECISION = 16;

using libcount::CountLeadingZeroes;

// Helper to return the appropriate alpha value used to scale the raw estimate
double GetAlphaForPrecision(int precision) {
  assert(precision >= HLL_MIN_PRECISION);
  assert(precision <= HLL_MAX_PRECISION);

  // TODO(tdial): These magic constants were derived from empirical research
  // by the authors of the HyperLogLog++ paper. Cite reference.
  switch (precision) {
    case 4:
      return 0.673f;
    case 5:
      return 0.697f;
    case 6:
      return 0.709f;
    default:
      return (0.7213f / (1.0f + (1.079f / static_cast<double>(precision))));
  }
}

// Helper to calculate the index into the table of registers from the hash
inline int RegisterIndexOf(uint64_t hash, int precision) {
  return (hash >> (64 - precision));
}

// Helper to count the leading zeros (less the bits used for the reg. index)
inline uint8_t ZeroCountOf(uint64_t hash, int precision) {
  // Make a mask for isolating the leading bits used for the register index.
  const uint64_t ONE = 1;
  const uint64_t mask = ~(((ONE << precision) - ONE) << (64 - precision));

  // Count zeroes, less the index bits we're masking off.
  return (CountLeadingZeroes(hash & mask) - static_cast<uint8_t>(precision));
}

}  // namespace

namespace libcount {

HLL::HLL(int precision)
    : precision_(precision), updates_(0), register_count_(0), registers_(NULL) {
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

HLL::~HLL() { delete[] registers_; }

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
  assert(index < register_count_);

  // Count the zeroes for the hash, and add one, per the algorithm spec.
  const uint8_t count = ZeroCountOf(hash, precision_) + 1;
  assert(count <= 64);

  // Update the appropriate register if the new count is greater than current.
  if (count > registers_[index]) {
    registers_[index] = count;
  }
}

uint64_t HLL::EstimateCardinality() const {
  // Let 'm' be the number of registers.
  const double m = static_cast<double>(register_count_);

  // For each register, let 'max' be the contents of the register
  // Let 'term' be the reciprocal of 2 ^ max
  // And let 'sum' be the sum of all terms
  double sum = 0.0;
  for (int i = 0; i < register_count_; ++i) {
    const double max = static_cast<double>(registers_[i]);
    const double term = pow(2.0, -max);
    sum += term;
  }

  // Next, calculate the harmonic mean
  const double harmonic_mean = m * (1.0 / sum);
  assert(harmonic_mean >= 0.0);

  // The estimate is bias-corrected using a constant that depends on precision
  const double scale_factor = m * GetAlphaForPrecision(precision_);
  assert(scale_factor >= 0.0);

  // Calculate the bias-corrected estimate
  const double estimate = harmonic_mean * scale_factor;
  assert(estimate >= 0.0);

  return static_cast<uint64_t>(estimate);
}

}  // namespace libcount
