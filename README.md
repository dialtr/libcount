# libhll - A HyperLogLog implementation
#######################################

The goal of this library is to provide a complete C/C++ implementation of the
HyperLogLog++ cardinality estimation algorithm, as described in the paper:
"HyperLogLog in Practice: Algorithmic Engineering of a State of the Art
Cardinality Estimation Algorithm" by Heule, Nunkesser, and Hall.

In its current (alpha) state, the library does not implement several of the
features described in the paper. Most notably, the current implementation does
not fall back to LinearCounting for small cardinalities. It also does not
currently implement sparse storage for the internal registers, nor any other
type of compression scheme.

The implementation does, however, employ several improvements to HyperLogLog
suggested by the previously mentioned authors. Primarily, the library uses
64-bit hashes in order to handle large sets. Additionally, the empirical
bias correction method (and associated constants) described in the paper
are used in this library.

For the sake of simplicity and portability, the library exposes a C-style
interface, modeled after the OpenSSH hash functions. The sample code below
demonstrates how the main APIs are used in a standard workflow.

## Sample Workflow

     #include <stdio.h>
     #include "hll.h"

     int main(int argc, char* argv[]) {
       // Initialize an HLL context structure with a precision of 8.
       const int kPrecision = 8;
       HLL_CTX* ctx = HLL_init(kPrecision);

       // Call HLL_update() once for each element in the set. In a real
       // program, a 64-bit hash of the element would be computed and this
       // hash would be passed to HLL_update().
       const int kNumItems = 1000;
       for (int i = 0; i < kNumItems; ++i) {
         HLL_update(ctx, i);
       }

       // Obtain an estimate of the cardinality of the set.
       uint64_t estimate = 0;
       HLL_cardinality(ctx, &estimate);
       printf("Estimate of cardinality: %llu\n", estimate);
       
       // Free resources associated with the HLL context.
       HLL_free(ctx);
     }

## Current Status

This library is currently in ALPHA.

## Future Planned Development

1. Fallback to LinearCounting for small cardinalities
2. Support for merging HLL_CTX structures to enable parallelization
3. Optimizations (Use builtins where available, etc,.) 
4. Experimental support for NVIDIA Cuda

