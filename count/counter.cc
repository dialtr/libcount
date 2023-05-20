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

#include "count/counter.h"

namespace libcount {

Counter::Counter(int precision) : precision_(0) {
	if (precision < kMinPrecision) {
		precision_ = kMinPrecision;
	} else if (precision > kMaxPrecision) {
		precision_ = kMaxPrecision;
	} else {
		precision_ = precision;
	}
}

Counter::~Counter() {
	// TODO(tdial): Implement
}

int Counter::Precision() const {
	return precision_;
}

}  // namespace libcount

