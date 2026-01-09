/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "NativeFantom.h"

#include <hermes/hermes.h>
#include <jsi/JSIDynamic.h>
#include <react/bridging/Bridging.h>
#include <react/debug/flags.h>
#include <react/renderer/components/modal/ModalHostViewShadowNode.h>
#include <react/renderer/components/scrollview/ScrollViewShadowNode.h>
#include <react/renderer/components/testcounter/TestCounterShadowNode.h>
#include <react/renderer/core/ConcreteState.h>
#include <react/renderer/scheduler/SchedulerDelegateImpl.h>
#include <react/renderer/uimanager/UIManagerBinding.h>
#include <fstream>
#include <iostream>

#include "TesterAppDelegate.h"

#include <jsi/instrumentation.h>
#include "render/RenderFormatOptions.h"
#include "render/RenderOutput.h"

namespace facebook::react {

NativeFantom::NativeFantom(
    TesterAppDelegate& appDelegate,
    std::shared_ptr<CallInvoker> jsInvoker)
    : NativeFantomCxxSpec<NativeFantom>(std::move(jsInvoker)),
      appDelegate_(appDelegate) {}

SurfaceId NativeFantom::startSurface(
    jsi::Runtime& runtime,
    double viewportWidth,
    double viewportHeight,
    double devicePixelRatio,
    double viewportOffsetX,
    double viewportOffsetY) {
  SurfaceId surfaceId = nextSurfaceId_;
  nextSurfaceId_ += 10;
  appDelegate_.startSurface(
      runtime,
      static_cast<float>(viewportWidth),
      static_cast<float>(viewportHeight),
      surfaceId,
      static_cast<float>(devicePixelRatio),
      static_cast<float>(viewportOffsetX),
      static_cast<float>(viewportOffsetY));
  return surfaceId;
}

void NativeFantom::stopSurface(jsi::Runtime& /*runtime*/, SurfaceId surfaceId) {
  appDelegate_.stopSurface(surfaceId);
}

void NativeFantom::produceFramesForDuration(
    jsi::Runtime& /*runtime*/,
    double milliseconds) {
  appDelegate_.produceFramesForDuration(milliseconds);
}

void NativeFantom::flushMessageQueue(jsi::Runtime& /*runtime*/) {
  appDelegate_.flushMessageQueue();
}

void NativeFantom::flushEventQueue(jsi::Runtime& /*runtime*/) {
  appDelegate_.onRender();
}

void NativeFantom::validateEmptyMessageQueue(jsi::Runtime& /*runtime*/) {
  if (appDelegate_.hasPendingTasksInMessageQueue()) {
    throw std::runtime_error("MessageQueue is not empty");
  }
}

std::vector<std::string> NativeFantom::takeMountingManagerLogs(
    jsi::Runtime& /*runtime*/,
    SurfaceId surfaceId) {
  return appDelegate_.mountingManager_->takeMountingLogs(surfaceId);
}

std::string NativeFantom::getRenderedOutput(
    jsi::Runtime& /*runtime*/,
    SurfaceId surfaceId,
    NativeFantomGetRenderedOutputRenderFormatOptions options) {
  RenderFormatOptions formatOptions{
      options.includeRoot, options.includeLayoutMetrics};

  auto viewTree = appDelegate_.mountingManager_->getViewTree(surfaceId);
  return appDelegate_.mountingManager_->renderer()->render(
      viewTree, formatOptions);
}

void NativeFantom::reportTestSuiteResultsJSON(
    jsi::Runtime& /*runtime*/,
    const std::string& testSuiteResultsJSON) {
  std::cout << testSuiteResultsJSON << std::endl;
}

jsi::Object NativeFantom::getDirectManipulationProps(
    jsi::Runtime& runtime,
    const std::shared_ptr<const ShadowNode>& shadowNode) {
  auto props = appDelegate_.mountingManager_->getViewDirectManipulationProps(
      shadowNode->getTag());
  return facebook::jsi::valueFromDynamic(runtime, props).asObject(runtime);
}

jsi::Object NativeFantom::getFabricUpdateProps(
    jsi::Runtime& runtime,
    const std::shared_ptr<const ShadowNode>& shadowNode) {
  auto props = appDelegate_.mountingManager_->getViewFabricUpdateProps(
      shadowNode->getTag());
  return facebook::jsi::valueFromDynamic(runtime, props).asObject(runtime);
}

void NativeFantom::enqueueNativeEvent(
    jsi::Runtime& /*runtime*/,
    std::shared_ptr<const ShadowNode> shadowNode,
    const std::string& type,
    const std::optional<folly::dynamic>& payload,
    std::optional<RawEvent::Category> category,
    std::optional<bool> isUnique) {
  if (isUnique.value_or(false)) {
    shadowNode->getEventEmitter()->dispatchUniqueEvent(
        std::move(type), payload.value_or(folly::dynamic::object()));
  } else {
    shadowNode->getEventEmitter()->dispatchEvent(
        std::move(type),
        payload.value_or(folly::dynamic::object()),
        category.value_or(RawEvent::Category::Unspecified));
  }
}

void NativeFantom::enqueueScrollEvent(
    jsi::Runtime& /*runtime*/,
    std::shared_ptr<const ShadowNode> shadowNode,
    ScrollOptions options) {
  const auto* scrollViewShadowNode =
      dynamic_cast<const ScrollViewShadowNode*>(&*shadowNode);

  if (scrollViewShadowNode == nullptr) {
    throw std::runtime_error(
        "enqueueScrollEvent() can only be called on <ScrollView />");
  }

  auto point = Point{
      .x = options.x,
      .y = options.y,
  };

  auto scrollEvent = ScrollEvent();

  scrollEvent.contentOffset = point;
  scrollEvent.contentSize =
      scrollViewShadowNode->getStateData().getContentSize();
  scrollEvent.containerSize =
      scrollViewShadowNode->getLayoutMetrics().frame.size;
  scrollEvent.contentInset =
      scrollViewShadowNode->getConcreteProps().contentInset;
  scrollEvent.zoomScale = options.zoomScale.value_or(scrollEvent.zoomScale);

  scrollViewShadowNode->getConcreteEventEmitter().onScroll(scrollEvent);

  auto state =
      std::static_pointer_cast<const ScrollViewShadowNode::ConcreteState>(
          scrollViewShadowNode->getState());
  state->updateState(
      [point](const ScrollViewShadowNode::ConcreteState::Data& oldData)
          -> ScrollViewShadowNode::ConcreteState::SharedData {
        auto newData = oldData;
        newData.contentOffset = point;
        return std::make_shared<
            const ScrollViewShadowNode::ConcreteState::Data>(newData);
      });
}

void NativeFantom::enqueueModalSizeUpdate(
    jsi::Runtime& /*runtime*/,
    std::shared_ptr<const ShadowNode> shadowNode,
    double width,
    double height) {
  const auto* modalHostViewShadowNode =
      dynamic_cast<const ModalHostViewShadowNode*>(&*shadowNode);

  if (modalHostViewShadowNode == nullptr) {
    throw std::runtime_error(
        "enqueueModalSizeUpdate() can only be called on <Modal />");
  }

  auto state =
      std::static_pointer_cast<const ModalHostViewShadowNode::ConcreteState>(
          modalHostViewShadowNode->getState());

  state->updateState(ModalHostViewState(
      {.width = static_cast<Float>(width),
       .height = static_cast<Float>(height)}));
}

jsi::Function NativeFantom::createShadowNodeReferenceCounter(
    jsi::Runtime& runtime,
    std::shared_ptr<const ShadowNode> shadowNode) {
  auto weakShadowNode = std::weak_ptr<const ShadowNode>(shadowNode);

  return jsi::Function::createFromHostFunction(
      runtime,
      jsi::PropNameID::forAscii(runtime, "getReferenceCount"),
      0,
      [weakShadowNode](
          jsi::Runtime&, const jsi::Value&, const jsi::Value*, size_t)
          -> jsi::Value { return {(int)weakShadowNode.use_count()}; });
}

jsi::Function NativeFantom::createShadowNodeRevisionGetter(
    jsi::Runtime& runtime,
    std::shared_ptr<const ShadowNode> shadowNode) {
#if RN_DEBUG_STRING_CONVERTIBLE
  auto weakShadowNode = std::weak_ptr<const ShadowNode>(shadowNode);

  return jsi::Function::createFromHostFunction(
      runtime,
      jsi::PropNameID::forAscii(runtime, "getRevision"),
      0,
      [weakShadowNode](
          jsi::Runtime& runtime, const jsi::Value&, const jsi::Value*, size_t)
          -> jsi::Value {
        if (auto strongShadowNode = weakShadowNode.lock()) {
          const auto& uiManager =
              UIManagerBinding::getBinding(runtime)->getUIManager();

          const auto& currentRevision =
              *uiManager.getNewestCloneOfShadowNode(*strongShadowNode);
          return currentRevision.revision_;
        } else {
          return jsi::Value::null();
        }
      });
#else
  // TODO(T225400348): Remove this when revision_ is available in optimised
  // builds.
  throw std::runtime_error(
      "createShadowNodeRevisionGetter() is only available in debug builds");
#endif
}

void NativeFantom::saveJSMemoryHeapSnapshot(
    jsi::Runtime& runtime,
    const std::string& filePath) {
  runtime.instrumentation().collectGarbage("heapsnapshot");
  runtime.instrumentation().createSnapshotToFile(filePath);
}

#ifdef REACT_NATIVE_DEBUG

void NativeFantom::forceHighResTimeStamp(
    jsi::Runtime& /*runtime*/,
    std::optional<HighResTimeStamp> now) {
  if (now) {
    HighResTimeStamp::setTimeStampProviderForTesting(
        [now] { return now->toChronoSteadyClockTimePoint(); });
  } else {
    HighResTimeStamp::setTimeStampProviderForTesting(nullptr);
  }
}

#else

void NativeFantom::forceHighResTimeStamp(
    jsi::Runtime& runtime,
    std::optional<HighResTimeStamp> /*now*/) {
  throw jsi::JSError(
      runtime, "Mocking timers is not supported in optimized builds");
}

#endif

const int JS_SAMPLING_PROFILER_HZ = 10000;

void NativeFantom::startJSSamplingProfiler(jsi::Runtime& /*runtime*/) {
  auto* hermesRootAPI =
      jsi::castInterface<hermes::IHermesRootAPI>(hermes::makeHermesRootAPI());
  hermesRootAPI->enableSamplingProfiler(JS_SAMPLING_PROFILER_HZ);
}

void NativeFantom::stopJSSamplingProfilerAndSaveToFile(
    jsi::Runtime& runtime,
    const std::string& filePath) {
  auto* hermesRootAPI =
      jsi::castInterface<hermes::IHermesRootAPI>(hermes::makeHermesRootAPI());
  hermesRootAPI->disableSamplingProfiler();
  std::ofstream fileStream(filePath);
  auto* hermesRuntime = dynamic_cast<hermes::HermesRuntime*>(&runtime);
  hermesRuntime->sampledTraceToStreamInDevToolsFormat(fileStream);
}

void NativeFantom::setImageResponse(
    jsi::Runtime& /*rt*/,
    const std::string& uri,
    const NativeFantomSetImageResponseImageResponse& imageResponse) {
  appDelegate_.mountingManager_->imageLoader_->setImageResponse(
      uri,
      {
          .width = imageResponse.width,
          .height = imageResponse.height,
          .cacheStatus = imageResponse.cacheStatus,
          .errorMessage = imageResponse.errorMessage,
      });
}

void NativeFantom::clearImage(jsi::Runtime& /*rt*/, const std::string& uri) {
  appDelegate_.mountingManager_->imageLoader_->clearImage(uri);
}

void NativeFantom::clearAllImages(jsi::Runtime& /*rt*/) {
  appDelegate_.mountingManager_->imageLoader_->clearAllImages();
}

// Threading support for race condition testing

jsi::Object NativeFantom::createWorkerThread(
    jsi::Runtime& runtime,
    const std::string& threadName) {
  std::lock_guard<std::mutex> lock(workerThreadsMutex_);

  // Generate unique thread ID
  std::string threadId = "worker_" + std::to_string(nextThreadId_++);

  // Create TaskDispatchThread with the given name
  auto thread = std::make_shared<TaskDispatchThread>(threadName);
  workerThreads_[threadId] = thread;

  // Return object with threadId
  auto result = jsi::Object(runtime);
  result.setProperty(runtime, "threadId", jsi::String::createFromUtf8(runtime, threadId));
  return result;
}

void NativeFantom::scheduleOnThread(
    jsi::Runtime& runtime,
    const std::string& threadId,
    jsi::Function callback) {
  std::shared_ptr<TaskDispatchThread> thread;
  {
    std::lock_guard<std::mutex> lock(workerThreadsMutex_);
    auto it = workerThreads_.find(threadId);
    if (it == workerThreads_.end()) {
      throw jsi::JSError(runtime, "Worker thread not found: " + threadId);
    }
    thread = it->second;
  }

  // Capture the callback as a shared pointer to keep it alive
  auto sharedCallback = std::make_shared<jsi::Function>(std::move(callback));
  auto& runtimeRef = runtime;

  // Schedule the task on the worker thread
  thread->runAsync([sharedCallback, &runtimeRef]() {
    // Call the JavaScript function on the worker thread
    sharedCallback->call(runtimeRef);
  });
}

void NativeFantom::threadBarrier(
    jsi::Runtime& runtime,
    const std::vector<std::string>& threadIds) {
  for (const auto& threadId : threadIds) {
    std::shared_ptr<TaskDispatchThread> thread;
    {
      std::lock_guard<std::mutex> lock(workerThreadsMutex_);
      auto it = workerThreads_.find(threadId);
      if (it == workerThreads_.end()) {
        throw jsi::JSError(runtime, "Worker thread not found: " + threadId);
      }
      thread = it->second;
    }

    // Use runSync with empty task to wait for thread's queue to drain
    thread->runSync([]() {});
  }
}

void NativeFantom::destroyWorkerThread(
    jsi::Runtime& runtime,
    const std::string& threadId) {
  std::shared_ptr<TaskDispatchThread> thread;
  {
    std::lock_guard<std::mutex> lock(workerThreadsMutex_);
    auto it = workerThreads_.find(threadId);
    if (it == workerThreads_.end()) {
      throw jsi::JSError(runtime, "Worker thread not found: " + threadId);
    }
    thread = it->second;
    workerThreads_.erase(it);
  }

  // Quit the thread (this will join it)
  thread->quit();
}

// SchedulerDelegate configuration for race condition testing

void NativeFantom::setSchedulerDelegate(std::shared_ptr<SchedulerDelegateImpl> delegate) {
  schedulerDelegate_ = delegate;
}

void NativeFantom::enableAndroidStyleTransactionAccumulation(
    jsi::Runtime& runtime,
    bool enabled) {
  // Try to get from stored weak_ptr first
  auto delegate = schedulerDelegate_.lock();

  // If not set, get from TesterAppDelegate
  if (!delegate) {
    auto* schedulerDelegate = appDelegate_.getSchedulerDelegate();
    if (schedulerDelegate) {
      auto* delegateImpl = dynamic_cast<SchedulerDelegateImpl*>(schedulerDelegate);
      if (delegateImpl) {
        // We can't store raw pointers in weak_ptr, but we can call directly
        delegateImpl->setAndroidStyleTransactionAccumulation(enabled);
        return;
      }
    }
  }

  if (!delegate) {
    throw jsi::JSError(runtime, "SchedulerDelegate not available");
  }
  delegate->setAndroidStyleTransactionAccumulation(enabled);
}

void NativeFantom::setTransactionPauseHook(
    jsi::Runtime& runtime,
    jsi::Function hook) {
  // Try to get from stored weak_ptr first
  auto delegate = schedulerDelegate_.lock();

  // If not set, get from TesterAppDelegate
  SchedulerDelegateImpl* delegateImpl = nullptr;
  if (!delegate) {
    auto* schedulerDelegate = appDelegate_.getSchedulerDelegate();
    if (schedulerDelegate) {
      delegateImpl = dynamic_cast<SchedulerDelegateImpl*>(schedulerDelegate);
    }
  } else {
    delegateImpl = delegate.get();
  }

  if (!delegateImpl) {
    throw jsi::JSError(runtime, "SchedulerDelegate not available");
  }

  // Store the JS function to keep it alive
  transactionPauseHookJSFunction_ = std::make_shared<jsi::Function>(std::move(hook));
  auto& runtimeRef = runtime;

  // Set the C++ hook that calls the JS function
  delegateImpl->setTransactionPauseHook([this, &runtimeRef]() {
    if (transactionPauseHookJSFunction_) {
      transactionPauseHookJSFunction_->call(runtimeRef);
    }
  });
}

void NativeFantom::clearTransactionPauseHook(jsi::Runtime& runtime) {
  // Try to get from stored weak_ptr first
  auto delegate = schedulerDelegate_.lock();

  // If not set, get from TesterAppDelegate
  SchedulerDelegateImpl* delegateImpl = nullptr;
  if (!delegate) {
    auto* schedulerDelegate = appDelegate_.getSchedulerDelegate();
    if (schedulerDelegate) {
      delegateImpl = dynamic_cast<SchedulerDelegateImpl*>(schedulerDelegate);
    }
  } else {
    delegateImpl = delegate.get();
  }

  if (!delegateImpl) {
    throw jsi::JSError(runtime, "SchedulerDelegate not available");
  }

  delegateImpl->clearTransactionPauseHook();
  transactionPauseHookJSFunction_ = nullptr;
}

// TestCounter state update for testing

void NativeFantom::updateTestCounterState(
    jsi::Runtime& runtime,
    std::shared_ptr<const ShadowNode> shadowNode) {
  auto testCounterShadowNode =
      std::dynamic_pointer_cast<const TestCounterShadowNode>(shadowNode);
  if (!testCounterShadowNode) {
    throw jsi::JSError(runtime, "Node is not a TestCounter component");
  }

  auto state = testCounterShadowNode->getState();
  if (!state) {
    throw jsi::JSError(runtime, "TestCounter has no state");
  }

  // Cast to ConcreteState to access getData() and updateState()
  auto concreteState = std::static_pointer_cast<const ConcreteState<TestCounterState>>(state);

  // Get current counter value and increment it
  int currentCounter = concreteState->getData().counter;
  int newCounter = currentCounter + 1;

  // Trigger state update via the state object itself
  concreteState->updateState(
      [newCounter](const TestCounterState& /*oldData*/) -> std::shared_ptr<const TestCounterState> {
        return std::make_shared<const TestCounterState>(newCounter);
      });
}

} // namespace facebook::react
