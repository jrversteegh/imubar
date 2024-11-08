/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cmath>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(fp_perf);

static constexpr int array_size = 0x1000;
static constexpr int repeat = 0x100;
double a1[array_size];
double a2[array_size];

void initialize_arrays() {
  for (int i = 0; i < array_size; ++i) {
    a1[i] = double(i) / array_size;
    a2[i] = 1.0 - a1[i];
  }
}

double run_addmul() {
  double result = 0.0;
  for (int i = 0; i < array_size; ++i) {
    for (int j = 0; j < array_size; ++j) {
      result += a1[i] * a2[j];
    }
  }
  return result;
}

double run_root() {
  double result = 0.0;
  for (int i = 0; i < repeat; ++i) {
    for (int j = 0; j < array_size; ++j) {
      result += std::sqrt(a1[j]);
    }
  }
  return result;
}

double run_trig() {
  double result = 0.0;
  for (int i = 0; i < repeat; ++i) {
    for (int j = 0; j < array_size; ++j) {
      result += std::sin(a1[j]) + std::cos(a2[j]);
    }
  }
  return result;
}


int main(void) {
  LOG_INF("Starting FP Perf...");
  initialize_arrays();

  auto start = k_uptime_get();
  LOG_INF("Running ADDMUL...");
  auto result = run_addmul();
  auto time = k_uptime_get() - start;
  LOG_INF("Result: %e in %lldms", result, time);

  start = k_uptime_get();
  LOG_INF("Running ROOT...");
  result = run_root();
  time = k_uptime_get() - start;
  LOG_INF("Result: %e in %lldms", result, time);

  start = k_uptime_get();
  LOG_INF("Running TRIG...");
  result = run_trig();
  time = k_uptime_get() - start;
  LOG_INF("Result: %e in %lldms", result, time);

  LOG_INF("FP Perf done.");
  return 0;
}
