/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.facebook.react.uiapp.component

import com.facebook.react.module.annotations.ReactModule
import com.facebook.react.uimanager.SimpleViewManager
import com.facebook.react.uimanager.ThemedReactContext
import com.facebook.react.uimanager.ViewManagerDelegate
import com.facebook.react.uimanager.annotations.ReactProp
import com.facebook.react.viewmanagers.RNTMemoryHeavyNativeViewManagerDelegate
import com.facebook.react.viewmanagers.RNTMemoryHeavyNativeViewManagerInterface

/** View manager for MemoryHeavyNativeView components. */
@ReactModule(name = MemoryHeavyNativeViewManager.REACT_CLASS)
internal class MemoryHeavyNativeViewManager :
    SimpleViewManager<MemoryHeavyNativeView>(),
    RNTMemoryHeavyNativeViewManagerInterface<MemoryHeavyNativeView> {

  companion object {
    const val REACT_CLASS = "RNTMemoryHeavyNativeView"
  }

  private val delegate: ViewManagerDelegate<MemoryHeavyNativeView> =
      RNTMemoryHeavyNativeViewManagerDelegate(this)

  override fun getDelegate(): ViewManagerDelegate<MemoryHeavyNativeView> = delegate

  override fun getName(): String = REACT_CLASS

  override fun createViewInstance(reactContext: ThemedReactContext): MemoryHeavyNativeView =
      MemoryHeavyNativeView(reactContext)

  @ReactProp(name = "allocationSizeMb", defaultInt = 250)
  override fun setAllocationSizeMb(view: MemoryHeavyNativeView, allocationSizeMb: Int) {
    view.setAllocationSizeMb(allocationSizeMb)
  }

  @ReactProp(name = "revision", defaultInt = 0)
  override fun setRevision(view: MemoryHeavyNativeView, revision: Int) {
    view.setRevision(revision)
  }
}
