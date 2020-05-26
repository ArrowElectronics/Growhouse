//
//  SBMGenericLevelDelta.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 23/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import "SBMControlValueSetSigModel.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Generic LevelDelta functionality object. It represents the change of an element's level state.
 */
@interface SBMGenericLevelDelta : NSObject<SBMControlValueSetSigModel>

/**
 Delta change of the level state.
 */
@property (nonatomic, readonly) SInt32 delta;

/**
 Initialize new Generic LevelDelta functionality object.

 @param delta Delta value that functionality should be initialized with.
 @return An initialized Generic LevelDelta functionality object, or nil if an object could not be created.
 */
- (instancetype)initWithDelta:(SInt32)delta;

/**
 Not available. Use initWithDelta instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithDelta instead.
 */
- (instancetype)init NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
