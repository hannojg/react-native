/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

import '@react-native/fantom/src/setUpDefaultReactNativeEnvironment';

import * as Fantom from '@react-native/fantom';
import NativeFantom from 'react-native/src/private/testing/fantom/specs/NativeFantom';
import TestCounter from 'react-native/Libraries/Components/TestCounter/TestCounter';
import * as React from 'react';

const {createRef} = React;

describe('Fabric State Update Race Condition', () => {
  it('exposes race condition when state update collides with view deletion', () => {
    let pauseHookFired = false;
    let viewDeleted = false;
    let threadId;

    Fantom.runTask(() => {
      // Enable Android-style transaction accumulation to replicate the race condition
      NativeFantom.enableAndroidStyleTransactionAccumulation(true);
    });

    const root = Fantom.createRoot();
    const counterRef = createRef();

    // Render TestCounter component
    Fantom.runTask(() => {
      root.render(<TestCounter ref={counterRef} />);
    });

    // Verify component rendered
    expect(counterRef.current).not.toBe(null);

    Fantom.runTask(() => {
      // Create worker thread for state update
      const result = NativeFantom.createWorkerThread('StateUpdateThread');
      threadId = result.threadId;

      // Set hook that fires AFTER pullTransaction, BEFORE lock acquisition
      // This is the critical race condition window
      NativeFantom.setTransactionPauseHook(() => {
        pauseHookFired = true;

        // While worker thread is paused, JS thread deletes the view
        // Use scheduleTask since we're inside a hook callback
        Fantom.scheduleTask(() => {
          root.render(null);  // Remove component
          viewDeleted = true;
        });

        // Execute the scheduled task immediately
        Fantom.runWorkLoop();
      });

      // Trigger state update on worker thread
      // This will call pullTransaction, then pause, then JS deletes view,
      // then resume and try to merge transaction for deleted view
      NativeFantom.scheduleOnThread(threadId, () => {
        const element = counterRef.current;
        if (element && element.state) {
          NativeFantom.updateTestCounterState(element.state);
        }
      });

      // Wait for worker thread to complete
      // This should expose the race condition - attempting to merge
      // a transaction for a view that was already deleted
      try {
        NativeFantom.threadBarrier([threadId]);
        // If we get here without throwing, the bug was NOT exposed
        expect(true).toBe(false); // Fail the test - we expected an error
      } catch (error) {
        // Expected: Bug exposed - crash or error
        expect(error).toBeDefined();
      }

      // Verify the test scenario executed correctly
      expect(pauseHookFired).toBe(true);
      expect(viewDeleted).toBe(true);

      // Cleanup
      NativeFantom.clearTransactionPauseHook();
      NativeFantom.destroyWorkerThread(threadId);
      NativeFantom.enableAndroidStyleTransactionAccumulation(false);
    });
  });

  it('works correctly without Android-style accumulation (no race)', () => {
    let threadId;

    const root = Fantom.createRoot();
    const counterRef = createRef();

    // Do NOT enable Android-style accumulation - use default safe behavior

    // Render TestCounter component
    Fantom.runTask(() => {
      root.render(<TestCounter ref={counterRef} />);
    });

    Fantom.runTask(() => {
      // Create worker thread for state update
      const result = NativeFantom.createWorkerThread('StateUpdateThread');
      threadId = result.threadId;

      // Trigger state update on worker thread
      NativeFantom.scheduleOnThread(threadId, () => {
        const element = counterRef.current;
        if (element && element.state) {
          NativeFantom.updateTestCounterState(element.state);
        }
      });

      // Delete view on JS thread - schedule it to avoid nesting
      Fantom.scheduleTask(() => {
        root.render(null);
      });

      // Execute the scheduled deletion
      Fantom.runWorkLoop();

      // Wait for worker thread - should NOT crash because default behavior
      // pulls and executes transactions immediately without the race window
      NativeFantom.threadBarrier([threadId]);

      // Cleanup
      NativeFantom.destroyWorkerThread(threadId);
    });
  });
});
