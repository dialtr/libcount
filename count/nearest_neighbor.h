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

#ifndef COUNT_NEAREST_NEIGHBOR_H_
#define COUNT_NEAREST_NEIGHBOR_H_

#include <stdlib.h>

namespace libcount {

// Scan an array of doubles, populating the 'neighbor_indices' array with
// at most N array indices of values closest in value to the probe value.
// Returns the actual number of neighbors found, which may be smaller than
// N if 'array_length' is smaller than N.
size_t NearestNeighbors(const double* array, size_t array_length,
                        const double probe_value, size_t N,
                        size_t* neighbor_indices);

}  // namespace libcount

#endif  // COUNT_NEAREST_NEIGHBOR
