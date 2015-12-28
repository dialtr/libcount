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
#include "count/bitwise_ops.h"
#include "count/hll_debug.h"

namespace {

const int HLL_MIN_PRECISION = 4;
const int HLL_MAX_PRECISION = 16;

}  // anonymous namespace

extern "C" {

struct HLL_CTX {
  int precision;
  double alpha;
  uint64_t updates;
  uint64_t zero_count_mask;
  uint64_t register_index_shift;
  uint64_t register_count;
  uint64_t registers[1];
};

double HLL_calculate_alpha(int precision) {
  assert(precision >= HLL_MIN_PRECISION);
  assert(precision <= HLL_MAX_PRECISION);

  double m = precision;

  // Constants taken from Heule, Nunkesser, Hall 2013
  switch (precision) {
    case 4:
      return 0.673f;
    case 5:
      return 0.697f;
    case 6:
      return 0.709f;
    default:
      return (0.7213f / (1.0f + (1.079f / m)));
  }
}

HLL_CTX* HLL_init(int precision) {
  // The caller must specify a precision value in the appropriate range.
  if ((precision < HLL_MIN_PRECISION) || (precision > HLL_MAX_PRECISION)) {
    errno = EINVAL;
    return NULL;
  }

  // Determine how much memory to allocate. This varies based on the precision
  // since we must have (2 ^ precision) registers available for the stochastic
  // averaging algorithm. We therefore allocate space for the structure itself
  // as well as 2 ^ precision registers, subtracting one register because the
  // layout of HLL_CTX already provides space for one.
  const uint64_t registers = ((uint64_t)1) << precision;
  const size_t size = sizeof(HLL_CTX) + ((registers - 1) * sizeof(uint64_t));

  // Allocate the memory. If malloc() returns NULL, that's a pretty serious
  // problem. In this unlikely event, we'll just return NULL and assume that
  // malloc() has set errno appropriately.
  HLL_CTX* context = reinterpret_cast<HLL_CTX*>(malloc(size));
  if (context == NULL) {
    return NULL;
  }

  // Initialize the context structure
  memset(context, 0, size);

  // Store the precision in the context.
  context->precision = precision;

  // Calculate and store the alpha value used in raw cardinality estimation.
  context->alpha = HLL_calculate_alpha(precision);

  // Compute a mask to mask off the leading 'precision' bits in order to
  // facilitate the leading zero count. NOTE: Since AND'ing the element hash
  // with this mask will zero out 'precision' bits, to calculate the true
  // number of leading zeroes after those bits, we'll need to subtract
  // 'precision' from that count.
  // TODO(tdial): Test this.
  context->zero_count_mask =
      ~(((((uint64_t)1) << precision) - 1) << (64 - precision));

  // Compute the number of places to shift a hash right to get register index.
  // TODO(tdial): Test this.
  context->register_index_shift = 64 - precision;

  // Store the number of registers implied by 'precision'.
  context->register_count = registers;

  return context;
}

int HLL_update(HLL_CTX* ctx, uint64_t element_hash) {
  assert(ctx != NULL);
  if (ctx == NULL) {
    errno = EINVAL;
    return -1;
  }

  // Per the explanation in the HyperLogLog++ paper, the incoming stream of
  // element hashes is divided into logical substreams by hashing the first
  // 'precision' bits of the hash. For each logical substream, we will store
  // the maximum number of leading zeroes found in the following bits of the
  // hash, plus one.
  const uint64_t substream_index = element_hash >> ctx->register_index_shift;

  // Using the bits that remain after discarding the first 'precision' bits
  // for the substream index, find the number of leading zeroes, plus one.
  // This is done by first masking off the leading 'precision' bits to zero,
  // and performing the zero count. Next, we subtract 'precision' from the
  // count since by definition, they were zeroed as part of the mask
  // operation. Finally, add one, since the algorithm requires that we store
  // the number of leading zeroes, plus one, in the register.
  const uint64_t zeroes =
      nlz64(element_hash & ctx->zero_count_mask) - ctx->precision + 1;

  /*
  // TODO(tdial): GCC has a builtin for leading zero count for uint64_t
  const uint64_t zeroes =
    __builtin_clzll(element_hash & ctx->zero_count_mask) - ctx->precision + 1;
  */

  // Did we find a new max zero count in the associated register?
  if (zeroes > ctx->registers[substream_index]) {
    ctx->registers[substream_index] = zeroes;
  }

  // Track the number of calls made to the HLL_update() function.
  ++ctx->updates;

  return 0;
}

double HLL_raw_estimate(const HLL_CTX* ctx) {
  assert(ctx != NULL);

  // TODO(tdial): This code was written first to be correct. Check to see
  // if eliminating the temporary variables in the loop makes a difference
  // when compiled with full optimizations.

  // Let m be the number of registers.
  const double m = ctx->register_count;

  // For each register, let max be the contents of the register.
  // Let term be the reciprocal of 2 ^ max
  // And let sum be the sum of all terms.
  double sum = 0.0;
  for (uint64_t i = 0; i < ctx->register_count; ++i) {
    const double max = ctx->registers[i];
    const double term = pow(2.0, -max);
    sum += term;
  }

  // Calculate the harmonic mean
  const double harmonic_mean = m * (1.0f / sum);

  // Calculate the scale factor
  // TODO(tdial): This could be calculated at initialization time.
  const double scale_factor = HLL_calculate_alpha(ctx->precision) * m;

  // Calculate the raw estimate: the harmonic mean multiplied by the scaler.
  const double raw_estimate = harmonic_mean * scale_factor;

  return raw_estimate;
}

int HLL_cardinality(const HLL_CTX* ctx, uint64_t* cardinality) {
  assert(ctx != NULL);
  assert(cardinality != NULL);
  double raw_estimate = HLL_raw_estimate(ctx);
  assert(raw_estimate >= 0.0);
  if (raw_estimate < 0) {
    // TODO(tdial): Should I come up with my own error codes?
    errno = ERANGE;
    return -1;
  }

  *cardinality = (uint64_t)raw_estimate;

  return 0;
}

void HLL_free(HLL_CTX* ctx) {
  assert(ctx != NULL);
  free(ctx);
}

void HLL_debug_print(FILE* file, HLL_CTX* ctx) {
  assert(file != NULL);
  assert(ctx != NULL);
  fprintf(file, "precision:            %d\n", ctx->precision);
  fprintf(file, "updates:              %lu\n", ctx->updates);
  fprintf(file, "zero_count_mask:      %p\n",
    reinterpret_cast<void*>(ctx->zero_count_mask));
  fprintf(file, "register_index_shift: %lu\n", ctx->register_index_shift);
  fprintf(file, "register_count:       %lu\n", ctx->register_count);
  for (uint64_t i = 0; i < ctx->register_count; ++i) {
    fprintf(file, "register[%06lu]:     %lu\n", i, ctx->registers[i]);
  }
}

int HLL_test(FILE* file) {
  //
  // Tests for bitwise_ops.h/.cc
  //

  // Mask of all 1's should return zero.
  uint64_t kNoLeadingZeroes = 0xffffffffffffffff;
  uint64_t test = nlz64(kNoLeadingZeroes);
  if (test != 0) {
    fprintf(file, "nlz64(): should return 0 when passed bitset of all 1's\n");
    return -1;
  }

  // Test several known values of leading zeroes.
  for (int i = 0; i < 64; ++i) {
    const uint64_t kOne = 1;
    const uint64_t kTestMask = (kOne << i);
    const uint64_t kExpectedLeadingZeroes = (64 - i - 1);
    const uint64_t actual = nlz64(kTestMask);
    if (actual != kExpectedLeadingZeroes) {
      fprintf(file, "nlz64(): expected %lu leading zeroes, counted %lu\n",
              kExpectedLeadingZeroes, actual);
      return -1;
    }
  }

  //
  // Tests for libhll
  //

  HLL_CTX* ctx = NULL;

  // Should fail to create context with precision value that is too low.
  const int kTooLowPrecision = 1;
  ctx = HLL_init(kTooLowPrecision);
  if (ctx != NULL) {
    fprintf(file, "HLL_init(): Should fail with precision value of %d\n",
            kTooLowPrecision);
    return -1;
  }

  // Should fail to create context with precision value that is too high.
  const int kTooHighPrecision = 17;
  ctx = HLL_init(kTooHighPrecision);
  if (ctx != NULL) {
    fprintf(file, "HLL_init(): Should fail with precision value of %d\n",
            kTooHighPrecision);
    return -1;
  }

  // Should succeed with precision value that is in range.
  const int kPrecision = 10;
  ctx = HLL_init(kPrecision);
  if (ctx == NULL) {
    fprintf(file, "HLL_init(): Should succeed with precision value of %d\n",
            kPrecision);
    return -1;
  }

  //
  // Verify that the members of the HLL_CTX structure are correct.
  //

  // Precision value should be set
  if (ctx->precision != kPrecision) {
    fprintf(file, "HLL_init(): HLL_CTX precision value is %d, expected %d\n",
            ctx->precision, kPrecision);
    return -1;
  }

  // Updates should be zero
  const uint64_t kExpectedUpdates = 0;
  if (ctx->updates != kExpectedUpdates) {
    fprintf(file, "HLL_init(): HLL_CTX updates value is %lu, expected %lu\n",
            ctx->updates, kExpectedUpdates);
    return -1;
  }

  // Register count should be 2 ^ precision
  const uint64_t kExpectedRegisterCount = (1 << ctx->precision);
  if (ctx->register_count != kExpectedRegisterCount) {
    fprintf(file, "HLL_init(): HLL_CTX register_count is %lu, expected %lu\n",
            ctx->register_count, kExpectedRegisterCount);
    return -1;
  }

  // All registers should be initialized to zero.
  for (uint64_t r = 0; r < ctx->register_count; ++r) {
    if (ctx->registers[r] != 0) {
      fprintf(file, "HLL_init(): HLL_CTX register[%lu] is %lu, expected 0\n", r,
              ctx->registers[r]);
      return -1;
    }
  }

  // Test the update function.
  int status = HLL_update(ctx, 1);
  if (status != 0) {
    fprintf(file, "HLL_update(): Call returned status %d, expected 0\n",
            status);
    return -1;
  }

  // We should have recorded a single call to update()
  if (ctx->updates != 1) {
    fprintf(file, "HLL_update(): ctx->updates should be 1\n");
    return -1;
  }

  // We expect register 0 to contain a particular number of leading zeroes.
  // The expected value should be the number of leading zeroes, plus one.
  // Thus, for a precision of kPrecision, we expect the actual leading
  // zero count to be (64 - kPrecision - 1) because the first kPrecision bits
  // are used for the substream index. We subtract one due to the existence
  // of a single 1 bit in the lowest position, and we add one back because
  // the algorithm always stores the number of leading zeroes plus one in
  // a given register.
  const uint64_t kExpectedLeadingZeroes = (64 - kPrecision - 1) + 1;
  const uint64_t kActualLeadingZeroes = ctx->registers[0];
  if (kExpectedLeadingZeroes != kActualLeadingZeroes) {
    fprintf(file,
            "HLL_update(): expected register 0 to hold value of %lu"
            " but found %lu\n",
            kExpectedLeadingZeroes, kActualLeadingZeroes);
    return -1;
  }

  // Free the context structure.
  HLL_free(ctx);

  return 0;
}

}  // extern "C"
