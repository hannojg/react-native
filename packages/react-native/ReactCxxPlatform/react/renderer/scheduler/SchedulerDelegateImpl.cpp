/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "SchedulerDelegateImpl.h"

namespace facebook::react {

SchedulerDelegateImpl::SchedulerDelegateImpl(
    std::shared_ptr<IMountingManager> mountingManager) noexcept
    : mountingManager_(std::move(mountingManager)) {}

void SchedulerDelegateImpl::schedulerDidFinishTransaction(
    const std::shared_ptr<const MountingCoordinator>& mountingCoordinator) {
  if (!useAndroidStyleTransactionAccumulation_) {
    // Current behavior: no-op, flush from schedulerShouldRenderTransactions
    return;
  }

  // Android-style behavior: pull and accumulate transaction
  auto mountingTransaction = mountingCoordinator->pullTransaction(
      /* willPerformAsynchronously = */ true);
  if (!mountingTransaction.has_value()) {
    return;
  }

  // 🔴 TEST HOOK: Pause here to simulate race condition
  // This allows testing the race between pullTransaction and lock acquisition
  if (transactionPauseHook_) {
    transactionPauseHook_();
  }

  // Critical section - mirrors Android's FabricUIManagerBinding pattern
  std::unique_lock<std::mutex> lock(pendingTransactionsMutex_);

  auto pendingTransaction = std::find_if(
      pendingTransactions_.begin(),
      pendingTransactions_.end(),
      [&](const auto& tx) {
        return tx.getSurfaceId() == mountingTransaction->getSurfaceId();
      });

  if (pendingTransaction != pendingTransactions_.end()) {
    // Merge with existing pending transaction for this surface
    pendingTransaction->mergeWith(std::move(*mountingTransaction));
  } else {
    // New pending transaction for this surface
    pendingTransactions_.push_back(std::move(*mountingTransaction));
  }
}

void SchedulerDelegateImpl::schedulerShouldRenderTransactions(
    const std::shared_ptr<const MountingCoordinator>& mountingCoordinator) {
  if (!useAndroidStyleTransactionAccumulation_) {
    // Current behavior: pull and execute immediately
    auto surfaceId = mountingCoordinator->getSurfaceId();
    if (auto transaction = mountingCoordinator->pullTransaction();
        transaction.has_value()) {
      if (auto& transactionValue = transaction.value();
          !transactionValue.getMutations().empty()) {
        mountingManager_->executeMount(surfaceId, std::move(transactionValue));
      }
    }
    return;
  }

  // Android-style behavior: execute accumulated pending transactions
  std::vector<MountingTransaction> pendingTransactions;
  {
    // Lock only for the swap, not for execution
    // This allows re-entrancy when mounting manager triggers state updates
    std::unique_lock<std::mutex> lock(pendingTransactionsMutex_);
    pendingTransactions_.swap(pendingTransactions);
  }

  // Execute transactions without holding the lock
  for (auto& transaction : pendingTransactions) {
    if (!transaction.getMutations().empty()) {
      mountingManager_->executeMount(
          transaction.getSurfaceId(),
          std::move(transaction));
    }
  }
}

void SchedulerDelegateImpl::schedulerDidRequestPreliminaryViewAllocation(
    const ShadowNode& shadowNode) {}

void SchedulerDelegateImpl::schedulerDidDispatchCommand(
    const ShadowView& shadowView,
    const std::string& commandName,
    const folly::dynamic& args) {
  mountingManager_->dispatchCommand(shadowView, commandName, args);
}

void SchedulerDelegateImpl::schedulerDidSetIsJSResponder(
    const ShadowView& shadowView,
    bool isJSResponder,
    bool blockNativeResponder) {
  mountingManager_->setIsJSResponder(
      shadowView, isJSResponder, blockNativeResponder);
}

void SchedulerDelegateImpl::schedulerDidSendAccessibilityEvent(
    const ShadowView& shadowView,
    const std::string& eventType) {}

void SchedulerDelegateImpl::schedulerShouldSynchronouslyUpdateViewOnUIThread(
    Tag tag,
    const folly::dynamic& props) {
  mountingManager_->synchronouslyUpdateViewOnUIThread(tag, props);
}

void SchedulerDelegateImpl::schedulerDidUpdateShadowTree(
    const std::unordered_map<Tag, folly::dynamic>& tagToProps) {
  mountingManager_->onUpdateShadowTree(tagToProps);
}

// Configuration methods for testing

void SchedulerDelegateImpl::setAndroidStyleTransactionAccumulation(bool enabled) {
  useAndroidStyleTransactionAccumulation_ = enabled;
}

void SchedulerDelegateImpl::setTransactionPauseHook(std::function<void()> hook) {
  transactionPauseHook_ = std::move(hook);
}

void SchedulerDelegateImpl::clearTransactionPauseHook() {
  transactionPauseHook_ = nullptr;
}

} // namespace facebook::react
