/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

import type {RNTesterModuleExample} from '../../types/RNTesterTypes';

import * as React from 'react';
import {Button, StyleSheet, Text, View} from 'react-native';

function Playground() {
  const [state, setState] = React.useState(true);

  return (
    <View style={styles.container}>
      <Text
        style={{
          fontStyle: state ? 'italic' : undefined,
        }}>
        Edit "RNTesterPlayground.js" to change this file
      </Text>
      <Button
        title="Press me 2x to trigger crash"
        onPress={() => {
          setState(p => !p);
        }}
      />
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    padding: 10,
  },
});

export default ({
  title: 'Playground',
  name: 'playground',
  description: 'Test out new features and ideas.',
  render: (): React.Node => <Playground />,
}: RNTesterModuleExample);
