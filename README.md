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

Both C and C++ interfaces are available. The examples below demonstrate use.

## Current Status

This library is currently in ALPHA.

## Building

     git clone https://github.com/dialtr/libcount
     cd libcount
     make
     sudo make install

## Minimal Examples

### C++
```C++
#include <count/hll.h>

uint64_t Hash(int x) {
  // Users of this library should provide a good hash function
  // for hashing objects that are being counted. One suggestion
  // is to use a cryptographic hash function (SHA1, MD5, etc)
  // and return a subset of those bits.
  return x;
}

int main(int argc, char* argv[]) {
  const int kPrecision = 8;

  // Create an HLL object to track set cardinality.
  HLL* hll = HLL::Create(kPrecision);

  // Update object with hash of each element in your set.
  const kNumItems = 10000;
  for (int i = 0; i < kNumItems; ++i) {
    hll->Update(Hash(i));
  }

  // Obtain the cardinality estimate.
  uint64_t estimate = hll->EstimateCardinality();

  // Delete object.
  delete hll;
  
  return 0;
}
```

### C
```C
#include <count/c.h>

uint64_t Hash(int x) {
  // Users of this library should provide a good hash function
  // for hashing objects that are being counted. One suggestion
  // is to use a cryptographic hash function (SHA1, MD5, etc)
  // and return a subset of those bits.
  return x;
}

int main(int argc, char* argv[]) {
  const int kPrecision = 8;
  int error = 0;

  // Create an HLL object to track set cardinality.
  hll_t* hll = HLL_create(kPrecision, &error);

  // Update object with hash of each element in your set.
  const kNumItems = 10000;
  for (int i = 0; i < kNumItems; ++i) {
    HLL_update(hll, Hash(i));
  }

  // Obtain the cardinality estimate.
  uint64_t estimate = HLL_estimate_cardinality(hll);

  // Free object
  HLL_free(hll);

  return 0;
}
```

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
* Sparse "register" storage
