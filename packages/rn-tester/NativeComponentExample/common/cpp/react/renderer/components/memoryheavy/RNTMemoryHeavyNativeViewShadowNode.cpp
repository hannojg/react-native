/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "RNTMemoryHeavyNativeViewShadowNode.h"

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdio>

#include <logger/react_native_log.h>

namespace facebook::react {

namespace {

constexpr std::size_t kBytesPerMegabyte = 1024 * 1024;
constexpr std::size_t kPageSize = 4096;
std::atomic<int> gActiveShadowNodeInstances{0};

void logShadowNodeLifecycleEvent(
    const char* eventName,
    const RNTMemoryHeavyNativeViewReproShadowNode& shadowNode,
    int activeInstances)
{
  char buffer[256];
  std::snprintf(
      buffer,
      sizeof(buffer),
      "RNTMemoryHeavyNativeViewReproShadowNode %s this=%p family=%p tag=%d heap=%zu active=%d",
      eventName,
      &shadowNode,
      shadowNode.getFamilyShared().get(),
      shadowNode.getTag(),
      shadowNode.getExternalMemoryPressureSize(),
      activeInstances);
  react_native_log_info(buffer);
}

std::size_t getRequestedAllocationSize(int allocationSizeMb)
{
  auto clampedAllocationSizeMb = std::max(allocationSizeMb, 1);
  return static_cast<std::size_t>(clampedAllocationSizeMb) * kBytesPerMegabyte;
}

} // namespace

RNTMemoryHeavyNativeViewReproShadowNode::RNTMemoryHeavyNativeViewReproShadowNode(
    const ShadowNodeFragment& fragment,
    const ShadowNodeFamily::Shared& family,
    ShadowNodeTraits traits)
    : BaseShadowNode(fragment, family, traits)
{
  allocateHeapBuffer();
  logShadowNodeLifecycleEvent(
      "constructed",
      *this,
      gActiveShadowNodeInstances.fetch_add(1, std::memory_order_relaxed) + 1);
}

RNTMemoryHeavyNativeViewReproShadowNode::RNTMemoryHeavyNativeViewReproShadowNode(
    const ShadowNode& sourceShadowNode,
    const ShadowNodeFragment& fragment)
    : BaseShadowNode(sourceShadowNode, fragment)
{
  allocateHeapBuffer();
  logShadowNodeLifecycleEvent(
      "cloned",
      *this,
      gActiveShadowNodeInstances.fetch_add(1, std::memory_order_relaxed) + 1);
}

RNTMemoryHeavyNativeViewReproShadowNode::~RNTMemoryHeavyNativeViewReproShadowNode()
{
  logShadowNodeLifecycleEvent(
      "destructing",
      *this,
      gActiveShadowNodeInstances.fetch_sub(1, std::memory_order_relaxed) - 1);
}

size_t RNTMemoryHeavyNativeViewReproShadowNode::getExternalMemoryPressureSize()
    const {
  return sizeof(*this) + heapBuffer_.capacity() * sizeof(std::uint8_t);
}

void RNTMemoryHeavyNativeViewReproShadowNode::allocateHeapBuffer()
{
  const auto requestedSize =
      getRequestedAllocationSize(getConcreteProps().allocationSizeMb);

  heapBuffer_.resize(requestedSize);

  // Touch each page so the OS commits real heap memory for the repro.
  for (std::size_t offset = 0; offset < heapBuffer_.size(); offset += kPageSize) {
    heapBuffer_[offset] = static_cast<std::uint8_t>((offset / kPageSize) % 251);
  }

  if (!heapBuffer_.empty()) {
    heapBuffer_.back() = 0x7F;
  }
}

} // namespace facebook::react
