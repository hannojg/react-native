/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.facebook.react.uiapp.component

import android.graphics.Color
import android.view.View
import com.facebook.react.uimanager.ThemedReactContext

internal class MemoryHeavyNativeView(context: ThemedReactContext) : View(context) {
  private var allocationSizeMb: Int = 250
  private var revision: Int = Int.MIN_VALUE

  init {
    setRevision(0)
  }

  fun setAllocationSizeMb(nextAllocationSizeMb: Int) {
    allocationSizeMb = nextAllocationSizeMb
    contentDescription = "Memory-heavy native view ${allocationSizeMb}MB"
  }

  fun setRevision(nextRevision: Int) {
    if (revision == nextRevision) {
      return
    }

    revision = nextRevision
    val hue = ((revision * 47) % 360).toFloat()
    val color = Color.HSVToColor(floatArrayOf(hue, 0.45f, 0.9f))
    setBackgroundColor(color)
    contentDescription = "Memory-heavy native view revision ${revision} ${allocationSizeMb}MB"
  }
}
