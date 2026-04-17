/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import "RNTMemoryHeavyNativeViewComponentView.h"

#import "../common/cpp/react/renderer/components/memoryheavy/RNTMemoryHeavyNativeViewComponentDescriptor.h"

#import <react/renderer/components/AppSpecs/Props.h>
#import <react/renderer/components/AppSpecs/RCTComponentViewHelpers.h>

using namespace facebook::react;

@interface RNTMemoryHeavyNativeViewComponentView () <RCTRNTMemoryHeavyNativeViewViewProtocol>
@end

static UIColor *RNTMemoryHeavyNativeViewColorForRevision(NSInteger revision)
{
  NSInteger wrappedRevision = (revision * 47) % 360;
  CGFloat hue = (CGFloat)wrappedRevision / 360.0;
  return [UIColor colorWithHue:hue saturation:0.45 brightness:0.9 alpha:1.0];
}

@implementation RNTMemoryHeavyNativeViewComponentView {
  UIView *_view;
}

+ (ComponentDescriptorProvider)componentDescriptorProvider
{
  return concreteComponentDescriptorProvider<
      RNTMemoryHeavyNativeViewReproComponentDescriptor>();
}

+ (void)load
{
  [super load];
}

- (instancetype)initWithFrame:(CGRect)frame
{
  if (self = [super initWithFrame:frame]) {
    static const auto defaultProps =
        std::make_shared<const RNTMemoryHeavyNativeViewProps>();
    _props = defaultProps;

    _view = [[UIView alloc] init];
    _view.backgroundColor = RNTMemoryHeavyNativeViewColorForRevision(0);
    self.contentView = _view;
  }

  return self;
}

- (void)updateProps:(const Props::Shared &)props oldProps:(const Props::Shared &)oldProps
{
  const auto &oldViewProps =
      *std::static_pointer_cast<const RNTMemoryHeavyNativeViewProps>(_props);
  const auto &newViewProps =
      *std::static_pointer_cast<const RNTMemoryHeavyNativeViewProps>(props);

  if (oldViewProps.revision != newViewProps.revision) {
    _view.backgroundColor =
        RNTMemoryHeavyNativeViewColorForRevision(newViewProps.revision);
  }

  _view.accessibilityIdentifier = [NSString
      stringWithFormat:@"memory-heavy-native-view-%d-%d",
                       newViewProps.revision,
                       newViewProps.allocationSizeMb];

  [super updateProps:props oldProps:oldProps];
}

@end

Class<RCTComponentViewProtocol> RNTMemoryHeavyNativeViewCls(void)
{
  return RNTMemoryHeavyNativeViewComponentView.class;
}
