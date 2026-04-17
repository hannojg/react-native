/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

import type {CodegenTypes, HostComponent, ViewProps} from 'react-native';

import {codegenNativeComponent} from 'react-native';

type NativeProps = Readonly<{
  ...ViewProps,
  allocationSizeMb?: CodegenTypes.Int32,
  revision?: CodegenTypes.Int32,
}>;

export type MemoryHeavyNativeViewType = HostComponent<NativeProps>;

export default codegenNativeComponent<NativeProps>(
  'RNTMemoryHeavyNativeView',
) as MemoryHeavyNativeViewType;
