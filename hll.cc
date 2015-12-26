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

#include "hll.h"
#include <assert.h>
#include <errno.h>
#include <string.h>
#include "hll_debug.h"

namespace {

const int HLL_MIN_PRECISION = 4;
const int HLL_MAX_PRECISION = 16;

}  // anonymous namespace

extern "C" {

struct HLL_CTX {
  int      precision;
  uint64_t updates;
  uint64_t register_count;
  uint64_t registers[1];
};

HLL_CTX* HLL_init(int precision) {
  // The caller must specify a precision value in the appropriate range.
  if ((precision < HLL_MIN_PRECISION) ||
      (precision > HLL_MAX_PRECISION)) {
    errno = EINVAL;
    return NULL;
  }

  // Determine how much memory to allocate. This varies based on the precision
  // since we must have (2 ^ precision) registers available for the stocastic
  // averaging algorithm. We therefore allocate space for the structure itself
  // as well as 2 ^ precision registers, subtracting one register because the
  // layout of HLL_CTX already provides space for one.
  uint64_t registers = 1 << precision;
  const size_t size = sizeof(HLL_CTX) + ((registers - 1) * sizeof(uint64_t));

  // Allocate the memory. If malloc() returns NULL, that's a pretty serious
  // problem. In this unlikely event, we'll just return NULL and assume that
  // malloc() has set errno appropriately.
  HLL_CTX* context = reinterpret_cast<HLL_CTX*>(malloc(size));
  if (context == NULL) {
    return NULL;
  }

  // Initialize the context structure's members.
  memset(context, 0, size);
  context->precision = precision;
  context->register_count = registers;

  return context;
}

int HLL_update(HLL_CTX*, uint64_t element_hash) {
  // TODO: Implement
  return -1;
}

int HLL_cardinality(const HLL_CTX* ctx, uint64_t* cardinality) {
  // TODO: Implement
  return -1;
}

void HLL_free(HLL_CTX* ctx) {
  assert(ctx != NULL);
  free(ctx);
}

void HLL_debug_print(FILE* file, HLL_CTX* ctx) {
  // TODO: Implement
}

int HLL_test(FILE* file) {
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
  const int kPrecision = 8;
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
    fprintf(file, "HLL_init(): HLL_CTX updates value is %llu, expected %llu\n",
      ctx->updates, kExpectedUpdates);
    return -1;
  }

  // Register count should be 2 ^ precision
  const uint64_t kExpectedRegisterCount = (1 << ctx->precision);
  if (ctx->register_count != kExpectedRegisterCount) {
    fprintf(file,
      "HLL_init(): HLL_CTX register_count is %llu, expected %llu\n",
      ctx->register_count, kExpectedRegisterCount);
    return -1;
  }

  // All registers should be initialized to zero.
  for (uint64_t r = 0; r < ctx->register_count; ++r) {
    if (ctx->registers[r] != 0) {
      fprintf(file, "HLL_init(): HLL_CTX register[%llu] is %llu, expected 0\n",
        r, ctx->registers[r]);
      return -1;
    }
  }

  // Free the context structure.
  HLL_free(ctx);

  return 0;
}

}  // extern "C"

