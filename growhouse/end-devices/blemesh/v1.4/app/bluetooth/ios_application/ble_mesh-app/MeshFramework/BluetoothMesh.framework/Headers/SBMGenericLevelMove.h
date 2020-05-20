//
//  SBMGenericLevelMove.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 26/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import "SBMControlValueSetSigModel.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Generic LevelMove functionality object. It represents the step to calculate move speed of the element's level state.
 */
@interface SBMGenericLevelMove : NSObject<SBMControlValueSetSigModel>

/**
 Level step to calculate move speed of the level state.
 */
@property (nonatomic, readonly) SInt16 level;

/**
 Initialize new Generic LevelMove functionality object.

 @param level Delta value that functionality should be initialized with.
 @return An initialized Generic LevelMove functionality object, or nil if an object could not be created.
 */
- (instancetype)initWithLevel:(SInt16)level;

/**
 Not available. Use initWithLevel instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithLevel instead.
 */
- (instancetype)init NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
