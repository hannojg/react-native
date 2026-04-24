/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <react/renderer/components/AppSpecs/EventEmitters.h>
#include <react/renderer/components/AppSpecs/Props.h>
#include <react/renderer/components/AppSpecs/States.h>
#include <react/renderer/components/view/ConcreteViewShadowNode.h>

#include <cstdint>
#include <vector>

namespace facebook::react {

extern const char RNTMemoryHeavyNativeViewComponentName[];

class RNTMemoryHeavyNativeViewReproShadowNode final
    : public ConcreteViewShadowNode<
          RNTMemoryHeavyNativeViewComponentName,
          RNTMemoryHeavyNativeViewProps,
          RNTMemoryHeavyNativeViewEventEmitter,
          RNTMemoryHeavyNativeViewState> {
 public:
  using BaseShadowNode = ConcreteViewShadowNode<
      RNTMemoryHeavyNativeViewComponentName,
      RNTMemoryHeavyNativeViewProps,
      RNTMemoryHeavyNativeViewEventEmitter,
      RNTMemoryHeavyNativeViewState>;
  using ConcreteProps = typename BaseShadowNode::ConcreteProps;
  using ConcreteEventEmitter = typename BaseShadowNode::ConcreteEventEmitter;
  using ConcreteState = typename BaseShadowNode::ConcreteState;

  RNTMemoryHeavyNativeViewReproShadowNode(
      const ShadowNodeFragment& fragment,
      const ShadowNodeFamily::Shared& family,
      ShadowNodeTraits traits);

  RNTMemoryHeavyNativeViewReproShadowNode(
      const ShadowNode& sourceShadowNode,
      const ShadowNodeFragment& fragment);

  ~RNTMemoryHeavyNativeViewReproShadowNode() override;

  static ShadowNodeTraits BaseTraits()
  {
    auto traits = BaseShadowNode::BaseTraits();
    traits.set(ShadowNodeTraits::Trait::LeafYogaNode);
    return traits;
  }

  size_t getExternalMemoryPressureSize() const override;

 private:
  void allocateHeapBuffer();

  std::vector<std::uint8_t> heapBuffer_{};
};

} // namespace facebook::react
