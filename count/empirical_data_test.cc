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

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "count/empirical_data.h"

namespace {

bool IsDoubleEqual(double a, double b, double epsilon) {
  return (fabs(a - b) < epsilon);
}

}  // namespace

int EstimateDataIncreases() {
  // For our first test, we ensure that the raw estimate tables contain
  // increasing values as the index into each table increases. If this
  // condition did not hold, there would be ambiguity in interpolating
  // a value. For example, in this hypothetical table:
  //
  // [ 2.0, 3.0, 2.5 ]
  //
  // The value at index 2 is less than the value at index 1, which would
  // would imply that for a raw estimoate of 2.75, there are two possible
  // interpolation points.

  // The estimate arrays are terminated by zeroes. Since there are
  // varying numbers of elements in the tables associated with each
  // precision, we must detect the end of the array by comparing its
  // value to zero. We do so using a special floating point comparator
  // defined above, since it is not really safe to compare floats
  // for equality otherwise.
  const double EPSILON = 0.0001;

  // We'll track the number of errors that we encounter and write the
  // errors to standard error. We'll fail the test at the end if the
  // error count is greater than zero.
  size_t error_count = 0;

  // There are a total of fifteen arrays (one for each precision value
  // from precision 4..18 inclusive.
  const size_t TABLE_COUNT = 15;

  // The tables may contain at most 201 usable values. However, the
  // test will compare adjacent elements in the array, and so the loop
  // counter should terminate 1 element earlier.
  const size_t MAX_TABLE_INDEX = 200;

  for (size_t table = 0; table < TABLE_COUNT; ++table) {
    const double* estimates = RAW_ESTIMATE_DATA[table];
    for (size_t index = 0; index < MAX_TABLE_INDEX; ++index) {
      const double lhs = estimates[index];
      const double rhs = estimates[index + 1];
      if (IsDoubleEqual(rhs, 0.0, EPSILON)) {
        // We reached the terminating entry in the table.
        break;
      }

      // Did the lhs, rhs values we're examining violate the condition?
      if (lhs >= rhs) {
        fprintf(
            stderr,
            "ERROR: lhs >= rhs: In the raw estimate table for precision %lu: \n"
            "  Value at index %lu was: %lf\n"
            "  Value at index %lu was: %lf\n\n",
            table + 4, index, estimates[index], index + 1,
            estimates[index + 1]);

        ++error_count;
      }
    }
  }

  return (error_count == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

int main(int argc, char* argv[]) {
  // Values in the raw estimate lookup tables should increase in index order.
  int status = EstimateDataIncreases();
  if (status != EXIT_SUCCESS) {
    return status;
  }
}
