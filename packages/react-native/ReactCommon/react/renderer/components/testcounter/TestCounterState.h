/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <memory>

namespace facebook::react {

/*
 * State for <TestCounter> component.
 * Simple test component with a counter for testing state updates.
 */
class TestCounterState final {
 public:
  using Shared = std::shared_ptr<const TestCounterState>;

  TestCounterState() : counter(0) {}
  explicit TestCounterState(int counter_) : counter(counter_) {}

  const int counter;
};

} // namespace facebook::react
