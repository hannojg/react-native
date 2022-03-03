/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow strict-local
 */

import type {RNTesterModuleExample} from '../../types/RNTesterTypes';
import {StyleSheet, Text, View, FlatList} from 'react-native';
import BaseFlatListExample from './BaseFlatListExample';
import { genItemData, getItemLayout, ItemComponent } from '../../components/ListExampleShared';

import * as React from 'react';

const offsetToItemIndex = 43;
const DATA = genItemData(100);

export function FlatList_contentOffset(): React.Node {
  const [data, setData] = React.useState(DATA);

  return (
    <>
      <FlatList
        testID="flat_list"
        data={data}
        keyExtractor={(item, index) => item + index}
        getItemLayout={(data, index) => getItemLayout(data, index, false)}
        contentOffset={{
          x: 0,
          // + 6: the item should roughly be centered
          y: getItemLayout(data, offsetToItemIndex + 6, false).offset,
        }}
        renderItem={({ item }) =>
        <ItemComponent
          item={item}
          horizontal={false}
        />
        }
      />
    </>
  );
}

const styles = StyleSheet.create({
  titleContainer: {
    position: 'absolute',
    top: 45,
    left: 0,
    right: 0,
    justifyContent: 'flex-end',
    alignItems: 'center',
    backgroundColor: 'gray',
    zIndex: 1,
  },
  titleText: {
    fontSize: 24,
    lineHeight: 44,
    fontWeight: 'bold',
  },
});

export default ({
  title: 'Content Offset',
  name: 'contentOffset',
  description:
    'Reproducing contentOffset bugs on RN',
  render: () => <FlatList_contentOffset />,
}: RNTesterModuleExample);
