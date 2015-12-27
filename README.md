# libhll - A HyperLogLog implementation
#######################################

This library provides an implementation of the HyperLogLog algorithm for
establishing an estimate of the cardinality of large sets. A full explanation
of the method can be found in "HyperLogLog in Practice: Algorithmic Engineering
of a State of the Art Cardinality Estimation Algorithm" by Heule, Nunkesser,
and Hall.

NOTE: The implementation presented here does not employ all the innovations
described by the authors of HyperLogLog++. Notably, while it does employ the
constants derived by the authors, it does not attempt to save memory by using
a sparse representation, nor does it revert to LinearCounting for low
cardinalities.

It is my hope to enhance the library so that it eventually supports all
features described in the paper.

For maximum flexibility, the first cut of the implementation exposes a C-style
interface, following in the spirit of the OpenSSL cryptographic hash functions.
A C++ wrapper class will eventually be provided.

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
