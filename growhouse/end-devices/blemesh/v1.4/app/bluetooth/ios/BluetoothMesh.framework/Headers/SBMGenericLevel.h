//
//  SBMGenericLevel.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 23/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import "SBMControlValueGetSigModel.h"
#import "SBMControlValueSetSigModel.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Generic Level functionality object. It represents the state of an element. The meaning of the state is determined by the model.
 */
@interface SBMGenericLevel : NSObject<SBMControlValueGetSigModel, SBMControlValueSetSigModel>

/**
 Value of Generic Level functionality.
 */
@property (nonatomic, readonly) SInt16 level;

/**
 Initialize new Generic Level functionality object.

 @param level Level value that functionality should be initialized with.
 @return An initialized Generic Level functionality object, or nil if an object could not be created.
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
