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

#include <catch/catch.hpp>
#include "count/counter.h"

using ::libcount::Counter;

TEST_CASE("Precision Ranges All Supported", "[counter]")
{
	for (int i = 4; i <= 18; ++i) {
	  Counter counter(i);
	  REQUIRE(counter.Precision() == i);
	}
}

TEST_CASE("Precision Below Acceptable Range", "[counter]")
{
	const int kBelowRangePrecisionValue = 1;
	Counter counter(kBelowRangePrecisionValue);
	REQUIRE(counter.Precision() == 4);
}

TEST_CASE("Precision Above Acceptable Range", "[counter]")
{
	const int kBelowRangePrecisionValue = 20;
	Counter counter(kBelowRangePrecisionValue);
	REQUIRE(counter.Precision() == 18);
}

