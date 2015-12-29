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

#include "count/hll.h"
#include <stdlib.h>

namespace libcount {

HLL::HLL(int precision) {
  // TODO(tdial): Implement
}

HLL::~HLL() {
  // TODO(tdial): Implement
}

HLL* HLL::Create(int precision, int* error) {
  // TODO(tdial): Implement
  return NULL;
}

void HLL::Update(uint64_t hash) {
  // TODO(tdial): Implement
}

uint64_t HLL::GetCardinalityEstimate() const {
  // TODO(tdial): Implement
  return 0;
}

}  // namespace libcount
