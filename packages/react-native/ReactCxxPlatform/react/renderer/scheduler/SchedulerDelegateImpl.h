/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <react/renderer/mounting/MountingTransaction.h>
#include <react/renderer/scheduler/SchedulerDelegate.h>
#include <react/renderer/uimanager/IMountingManager.h>
#include <functional>
#include <mutex>
#include <vector>

namespace facebook::react {

class IMountingManager;

class SchedulerDelegateImpl : public SchedulerDelegate {
 public:
  SchedulerDelegateImpl(std::shared_ptr<IMountingManager> mountingManager) noexcept;

  ~SchedulerDelegateImpl() noexcept override = default;

  // Cannot be moved or copied due to mutex member
  SchedulerDelegateImpl(SchedulerDelegateImpl &&) noexcept = delete;
  SchedulerDelegateImpl &operator=(SchedulerDelegateImpl &&) noexcept = delete;
  SchedulerDelegateImpl(const SchedulerDelegateImpl &) = delete;
  SchedulerDelegateImpl &operator=(const SchedulerDelegateImpl &) = delete;

  // Enable/disable Android-style transaction accumulation (for testing)
  void setAndroidStyleTransactionAccumulation(bool enabled);

  // Set test hook called after pullTransaction, before acquiring lock
  void setTransactionPauseHook(std::function<void()> hook);
  void clearTransactionPauseHook();

 private:
  void schedulerDidFinishTransaction(const std::shared_ptr<const MountingCoordinator> &mountingCoordinator) override;

  void schedulerShouldRenderTransactions(
      const std::shared_ptr<const MountingCoordinator> &mountingCoordinator) override;

  void schedulerDidRequestPreliminaryViewAllocation(const ShadowNode &shadowNode) override;

  void schedulerDidDispatchCommand(
      const ShadowView &shadowView,
      const std::string &commandName,
      const folly::dynamic &args) override;

  void schedulerDidSetIsJSResponder(const ShadowView &shadowView, bool isJSResponder, bool blockNativeResponder)
      override;

  void schedulerDidSendAccessibilityEvent(const ShadowView &shadowView, const std::string &eventType) override;

  void schedulerShouldSynchronouslyUpdateViewOnUIThread(Tag tag, const folly::dynamic &props) override;

  void schedulerDidUpdateShadowTree(const std::unordered_map<Tag, folly::dynamic> &tagToProps) override;

  std::shared_ptr<IMountingManager> mountingManager_;

  // Android-style pending transaction mechanism (opt-in for testing)
  std::mutex pendingTransactionsMutex_;
  std::vector<MountingTransaction> pendingTransactions_;

  // Test hook - called after pullTransaction, before acquiring lock
  std::function<void()> transactionPauseHook_;

  // Enable Android-style behavior for testing (default: false)
  bool useAndroidStyleTransactionAccumulation_ = false;
};

}; // namespace facebook::react
