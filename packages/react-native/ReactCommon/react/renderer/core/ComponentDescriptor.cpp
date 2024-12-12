/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ComponentDescriptor.h"
#include <react/utils/ContextContainer.h>

namespace facebook::react {

template <typename TRawPropsParser>
ComponentDescriptor<TRawPropsParser>::ComponentDescriptor(
    const ComponentDescriptorParameters& parameters,
    TRawPropsParser&& rawPropsParser)
    : eventDispatcher_(parameters.eventDispatcher),
      contextContainer_(parameters.contextContainer),
      flavor_(parameters.flavor),
      rawPropsParser_(std::move(rawPropsParser)) {}

template <typename TRawPropsParser>
const std::shared_ptr<const ContextContainer>&
ComponentDescriptor<TRawPropsParser>::getContextContainer() const {
  return contextContainer_;
}

} // namespace facebook::react
