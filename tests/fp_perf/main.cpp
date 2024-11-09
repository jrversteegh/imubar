/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cmath>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(fp_perf);

static constexpr int array_size = 1000;
static constexpr int repeat = 200;
int i1[array_size];
int i2[array_size];
int64_t li1[array_size];
int64_t li2[array_size];
double d1[array_size];
double d2[array_size];
float f1[array_size];
float f2[array_size];

void initialize_arrays() {
  for (int i = 0; i < array_size; ++i) {
    i1[i] = i;
    i2[i] = array_size - i;
    li1[i] = i;
    li2[i] = array_size - i;
    d1[i] = double(i) / array_size;
    d2[i] = 1.0 - d1[i];
    f1[i] = float(i) / array_size;
    f2[i] = 1.0f - f1[i];
  }
}

template<typename T>
T run_addmul(T* a1, T* a2) {
  T result = 0.0;
  for (int i = 0; i < array_size; ++i) {
    for (int j = 0; j < array_size; ++j) {
      result += a1[i] * a2[j];
    }
  }
  return result;
}

template<typename T>
T run_root(T* a1, T* a2) {
  T result = 0.0;
  for (int i = 0; i < repeat; ++i) {
    for (int j = 0; j < array_size; ++j) {
      result += std::sqrt(a1[j]);
    }
  }
  return result;
}

template<typename T>
T run_trig(T* a1, T* a2) {
  T result = 0.0;
  for (int i = 0; i < repeat; ++i) {
    for (int j = 0; j < array_size; ++j) {
      result += std::sin(a1[j]);
    }
  }
  return result;
}

template <typename T>
using F = T(*)(T*, T*);

template <typename T>
constexpr char const* flop_unit() {
  return "MFlops";
}

template <>
constexpr char const* flop_unit<int>() {
  return "MIops (i32)";
}

template <>
constexpr char const* flop_unit<int64_t>() {
  return "MIops (i64)";
}

template <>
constexpr char const* flop_unit<float>() {
  return "MFlops (fp32)";
}

template <>
constexpr char const* flop_unit<double>() {
  return "MFlops (fp64)";
}

template <typename T>
void log_run(const char* opname, F<T> f, T* a1, T* a2, double ops) {
  auto start = k_uptime_get();
  LOG_INF("Running %s...", opname);
  auto result = f(a1, a2);
  auto time = k_uptime_get() - start;
  auto mflops = ops * array_size / 1000 / time;
  LOG_INF("Result: %e in %lldms => %.2f %s", static_cast<double>(result), time, mflops, flop_unit<T>());
}


int main(void) {
  LOG_INF("Starting FP Perf...");
  initialize_arrays();

  log_run("I32 ADDMUL", &run_addmul<int>, i1, i2, 2 * array_size);
  log_run("I64 ADDMUL", &run_addmul<int64_t>, li1, li2, 2 * array_size);
  log_run("FP32 ADDMUL", &run_addmul<float>, f1, f2, 2 * array_size);
  log_run("FP64 ADDMUL", &run_addmul<double>, d1, d2, 2 * array_size);
  log_run("FP32 ROOT", &run_root<float>, f1, f2, 2 * repeat);
  log_run("FP64 ROOT", &run_root<double>, d1, d2, 2 * repeat);
  log_run("FP32 TRIG", &run_trig<float>, f1, f2, 2 * repeat);
  log_run("FP64 TRIG", &run_trig<double>, d1, d2, 2 * repeat);

  LOG_INF("FP Perf done.");
  return 0;
}
