// Copyright 2015-2023 The libcount Authors.
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

#ifndef INCLUDE_COUNT_COUNTER_H_
#define INCLUDE_COUNT_COUNTER_H_

#include <cstdint>

namespace libcount {

class Counter {
 public:
	// Construct an instance of a HyperLogLog++ cardinality estimator. Valid
	// values for precision are [4..18] inclusive, and governs the precision
	// of the count estimate. Note: In the event that the caller provides an
	// invalid precision value, the implementation will force the precision
	// into the allowed range. The actual precision in effect can be learned
	// via a call to the Precision() accessor function.
	Counter(int requested_precision);

	// Destructor
	~Counter();

	// Return the precision of the counter.
	int Precision() const;

	// Copy and assignment intentionally not implemented.
	Counter(const Counter&) = delete;
	Counter& operator=(const Counter&) = delete;
  Counter(Counter&&) = delete;
	Counter& operator=(Counter&&) = delete;
	
	const int kMinPrecision = 4;
	const int kMaxPrecision = 18;

 private:
	int precision_;
};

}  // namespace libcount

#endif  // INCLUDE_COUNT_COUNTER_H_
