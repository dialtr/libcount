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

#ifndef COUNT_EMPIRICAL_DATA_H_
#define COUNT_EMPIRICAL_DATA_H_

extern const double THRESHOLD_DATA[19];
extern const double RAW_ESTIMATE_DATA[15][201];
extern const double BIAS_DATA[15][201];

// Return the empirical alpha value used for scaling harmonic means.
extern double EmpiricalAlpha(int precision);

// Return the cardinality threshold for the given precision value.
// Valid values for precision are [4..18] inclusive.
extern double EmpiricalThreshold(int precision);

// Return the empirical bias value for the raw estimate and precision.
extern double EmpiricalBias(double raw_estimate, int precision);

#endif  // COUNT_EMPIRICAL_DATA_H_
