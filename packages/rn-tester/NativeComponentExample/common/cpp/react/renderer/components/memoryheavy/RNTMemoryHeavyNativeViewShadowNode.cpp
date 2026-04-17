/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "RNTMemoryHeavyNativeViewShadowNode.h"

#include <algorithm>
#include <cstddef>

namespace facebook::react {

namespace {

constexpr std::size_t kBytesPerMegabyte = 1024 * 1024;
constexpr std::size_t kPageSize = 4096;

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
}

RNTMemoryHeavyNativeViewReproShadowNode::RNTMemoryHeavyNativeViewReproShadowNode(
    const ShadowNode& sourceShadowNode,
    const ShadowNodeFragment& fragment)
    : BaseShadowNode(sourceShadowNode, fragment)
{
  allocateHeapBuffer();
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
