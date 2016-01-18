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

#include <stdlib.h>
#include <stdio.h>
#include "count/nearest_neighbor.h"

using libcount::NearestNeighbors;

int NearestNeighborTest() {
  // Test with a sample neighborhood
  const double neighborhood[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};

  // How many elements in the neighborhood?
  const size_t neighborhood_size =
      sizeof(neighborhood) / sizeof(neighborhood[0]);

  // The value for which we shall seek neighbors.
  const double probe_value = 5.0;

  // The number of neighbors we'll seek.
  const size_t neighbor_count = 5;

  // Array to hold found_neighbors
  size_t neighbors[neighbor_count] = {0};

  // Expected neighbors
  const size_t expected_neighbors[neighbor_count] = {4, 3, 5, 2, 6};

  // Run the test
  const size_t actual_neighbor_count = NearestNeighbors(
      neighborhood, neighborhood_size, probe_value, neighbor_count, neighbors);

  // Did we find the right number of neighbors?
  if (actual_neighbor_count != neighbor_count) {
    fprintf(stderr, "actual_neighbor_count = %lu, expected %lu\n",
            actual_neighbor_count, neighbor_count);
    return EXIT_FAILURE;
  }

  // Validate the found neighbors against the expected set
  for (size_t i = 0; i < neighbor_count; ++i) {
    if (neighbors[i] != expected_neighbors[i]) {
      fprintf(stderr, "neighbors[%lu] was expected to be %lu, not %lu\n", i,
              expected_neighbors[i], neighbors[i]);
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) { return NearestNeighborTest(); }
