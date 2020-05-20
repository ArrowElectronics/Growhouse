//
//  SBMGenericTransitionTime.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 23/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import "SBMControlValueGetSigModel.h"
#import "SBMControlValueSetSigModel.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Generic TransitionTime functionality object. It represents how long an element shall take to transition from a present state to a new state.
 */
@interface SBMGenericTransitionTime : NSObject<SBMControlValueGetSigModel, SBMControlValueSetSigModel>

/**
 Value of Generic TransitionTime functionality. It consists of two fields, where first 6 bits represent the number of transition steps, and the last 2 bits represent the step resolution.
 */
@property (nonatomic, readonly) UInt8 transitionTime;

/**
 Initialize new Generic TransitionTime functionality object.

 @param transitionTime TransitionTime value that functionality should be initialized with.
 @return An initialized Generic TransitionTime functionality object, or nil if an object could not be created.
 */
- (instancetype)initWithTransitionTime:(UInt8)transitionTime;

/**
 Not available. Use initWithTransitionTime instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithTransitionTime instead.
 */
- (instancetype)init NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
