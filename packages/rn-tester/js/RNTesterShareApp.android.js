/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow
 * @format
 */

import RNTesterComponentTitle from './components/RNTesterComponentTitle';
import {BackHandler, Button, View} from 'react-native';

export default function RNTesterShareApp() {
  return <View style={{
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
  }}><RNTesterComponentTitle>RNTesterShareApp</RNTesterComponentTitle>
  <Button title="Close share app" onPress={() => {
    BackHandler.exitApp();
  }} /></View>
}
