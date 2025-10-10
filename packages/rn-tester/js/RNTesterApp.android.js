/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

import RNTesterApp from './RNTesterAppShared';
import RNTesterShareApp from './RNTesterShareApp.android';
import {AppRegistry} from 'react-native';

AppRegistry.registerComponent('RNTesterApp', () => RNTesterApp);
AppRegistry.registerComponent('RNTesterShareApp', () => RNTesterShareApp);

module.exports = RNTesterApp;
