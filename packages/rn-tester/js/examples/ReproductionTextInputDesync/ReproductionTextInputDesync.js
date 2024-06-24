/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow strict-local
 */

'use strict';

import type {
  RNTesterModule,
  RNTesterModuleExample,
} from '../../types/RNTesterTypes';

module.exports = ({
    displayName: (undefined: ?string),
    title: 'Reproduction TextInput Desync',
    documentationURL: 'https://reactnative.dev/docs/textinput',
    category: 'Basic',
    description: 'Reproduction.',
    examples,
  }: RNTesterModule);
