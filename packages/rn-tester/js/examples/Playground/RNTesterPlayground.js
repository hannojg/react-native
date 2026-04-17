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

import MemoryHeavyNativeView from '../../../NativeComponentExample/js/MemoryHeavyNativeViewNativeComponent';
import RNTesterBlock from '../../components/RNTesterBlock';
import RNTesterButton from '../../components/RNTesterButton';
import RNTesterText from '../../components/RNTesterText';
import * as React from 'react';
import {StyleSheet, View} from 'react-native';

const ALLOCATION_SIZE_MB = 250;

function Playground() {
  const [revision, setRevision] = React.useState(0);
  const [gcStatus, setGcStatus] = React.useState('Not run yet.');

  const callGC = () => {
    const gc = (global: any).gc;
    if (typeof gc === 'function') {
      gc();
      setGcStatus(`Called global.gc() after revision ${revision}.`);
    } else {
      setGcStatus('global.gc() is not available in this runtime.');
    }
  };

  return (
    <View style={styles.container}>
      <RNTesterBlock
        title="Memory-heavy Fabric shadow node"
        description={`Revision 0 renders nothing. Each mounted revision after that allocates about ${ALLOCATION_SIZE_MB} MB inside a custom Fabric shadow node. Remount the component to create a new shadow node revision and use the GC button to call Hermes GC from JS.`}>
        <RNTesterText>Current revision: {revision}</RNTesterText>
        <RNTesterText>Last GC result: {gcStatus}</RNTesterText>
        <View style={styles.buttonRow}>
          <RNTesterButton onPress={() => setRevision(current => current + 1)}>
            Render next revision
          </RNTesterButton>
          <RNTesterButton onPress={callGC}>Call Hermes GC</RNTesterButton>
        </View>
        {revision > 0 ? (
          <MemoryHeavyNativeView
            allocationSizeMb={ALLOCATION_SIZE_MB}
            key={`memory-heavy-native-view-${revision}`}
            revision={revision}
            style={styles.reproView}
          />
        ) : (
          <RNTesterText style={styles.emptyState}>
            Revision 0 keeps the custom Fabric component unmounted.
          </RNTesterText>
        )}
      </RNTesterBlock>
    </View>
  );
}

const styles = StyleSheet.create({
  buttonRow: {
    flexDirection: 'row',
    flexWrap: 'wrap',
    marginHorizontal: -5,
    marginTop: 12,
  },
  container: {
    padding: 10,
  },
  emptyState: {
    marginTop: 16,
  },
  reproView: {
    borderRadius: 12,
    height: 180,
    marginTop: 16,
    width: '100%',
  },
});

export default {
  title: 'Playground',
  name: 'playground',
  description: 'Test out new features and ideas.',
  render: (): React.Node => <Playground />,
} as RNTesterModuleExample;
