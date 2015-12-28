# libcount
##########

The goal of libcount is to provide a complete C/C++ implementation of the
HyperLogLog++ cardinality estimation algorithm, as described in the paper:
"HyperLogLog in Practice: Algorithmic Engineering of a State of the Art
Cardinality Estimation Algorithm" by Heule, Nunkesser, and Hall.

In its current (alpha) state, the library does not implement several of the
features described in the paper. Most notably, the current implementation does
not employ LinearCounting for small cardinalities, and thus will produce
biased (high) cardinality estimations for small sets. It also does not
currently implement sparse storage for the internal registers, nor any
other type of compression scheme.

The implementation does, however, employ several improvements to HyperLogLog
suggested by Heule, Nunkesser, and Hall. For example, the library uses
64-bit hashes in order to facilitate large sets. Additionally, the bias
correction (based on empirical research conducted by the paper's authors)
is implemented in libcount.

For the sake of simplicity and portability, the library exposes a C-style
interface, modeled after the OpenSSH hash functions. A C++ interface is
planned.

## Current Status

This library is currently in ALPHA.

## Building

     git clone https://github.com/dialtr/hyperloglog
     cd hyperloglog
     make
     sudo make install

## Sample Program

     #include <stdio.h>
     #include <count/hll.h>

     int main(int argc, char* argv[]) {
       // Initialize an HLL context structure with a precision of 8.
       const int kPrecision = 8;
       HLL_CTX* ctx = HLL_init(kPrecision);

       // Call HLL_update() once for each element in the set that you are
       // counting. In a real program, you'd use a high-quality 64-bit hash
       // function to obtain a hash code for each element.
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

## Contact
    Please see AUTHORS in the root of the repository for contact information.

## Dependencies
    The libcount.a library has no dependencies outside of the standard C/C++
    libraries. Maintaining this property is a design goal.

    The demo program currently requires OpenSSL due to its use of the SHA1
    hash functions. Future unit tests will also likely require this library.

## Future Planned Development

* Implement LinearCounting for handling small cardinalities
* Additional APIs to support parallelization
* Optimizations
* C++ class library
* Sparse "register" storage
