#ifndef HLL_DEBUG_H_
#define HLL_DEBUG_H_

#include <stdio.h>
#include "hll.h"

extern "C" {

// Print debug information about a context.
void HLL_debug_print(FILE* file, HLL_CTX* ctx);

// Run unit tests. Returns 0 on success, -1 otherwise. Logs errors to file.
int HLL_test(FILE* file);

}  // extern "C"

#endif  // HLL_DEBUG_H_
