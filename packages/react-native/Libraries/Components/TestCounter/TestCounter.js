/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

import type {HostComponent} from '../../Renderer/shims/ReactNativeTypes';
import type {ViewProps} from '../View/ViewPropTypes';

import * as NativeComponentRegistry from '../../NativeComponent/NativeComponentRegistry';

type NativeProps = $ReadOnly<{|
  ...ViewProps,
|}>;

const TestCounterNativeComponent: HostComponent<NativeProps> =
  NativeComponentRegistry.get<NativeProps>('TestCounter', () => ({
    uiViewClassName: 'TestCounter',
    bubblingEventTypes: {},
    directEventTypes: {},
    validAttributes: {},
  }));

export default TestCounterNativeComponent;
