/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <react/renderer/components/testcounter/TestCounterState.h>
#include <react/renderer/components/view/ConcreteViewShadowNode.h>
#include <react/renderer/core/PropsParserContext.h>

namespace facebook::react {

extern const char TestCounterComponentName[];

/*
 * `ShadowNode` for <TestCounter> component.
 */
class TestCounterShadowNode final : public ConcreteViewShadowNode<
                                         TestCounterComponentName,
                                         ViewProps,
                                         ViewEventEmitter,
                                         TestCounterState> {
 public:
  using ConcreteViewShadowNode::ConcreteViewShadowNode;

  static ShadowNodeTraits BaseTraits() {
    auto traits = ConcreteViewShadowNode::BaseTraits();
    traits.set(ShadowNodeTraits::Trait::LeafYogaNode);
    return traits;
  }
};

} // namespace facebook::react
