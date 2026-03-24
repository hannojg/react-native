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

import RNTesterText from '../../components/RNTesterText';
import * as React from 'react';
import {StyleSheet, View, Button} from 'react-native';

function Playground() {
  const [shouldSetStyle, setShouldSetStyle] = React.useState(true);

  return (
    <View style={styles.container}>
      <Button title="Toggle Style" onPress={() => setShouldSetStyle(!shouldSetStyle)} />
      <View
        style={[
          shouldSetStyle
            ? {
                borderStyle: 'dashed',
                borderWidth: 2,
              }
            : undefined,
          {
            height: 40,
            width: 40,
            backgroundColor: 'blue',
          },
        ]}
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
