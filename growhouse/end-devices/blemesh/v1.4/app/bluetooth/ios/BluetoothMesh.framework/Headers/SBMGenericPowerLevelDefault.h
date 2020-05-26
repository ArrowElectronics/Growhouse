//
//  SBMGenericPowerLevelDefault.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 26/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import "SBMControlValueGetSigModel.h"
#import "SBMControlValueSetSigModel.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Generic PowerLevelDefault functionality object. It represents the power level of an element when the device is powered up and when the Generic OnPowerUp state bound to the Generic PowerLevel state is set to 0x01 (Default).
 */
@interface SBMGenericPowerLevelDefault : NSObject<SBMControlValueGetSigModel, SBMControlValueSetSigModel>

/**
 Value of Generic PowerLevelLast functionality. 0 means that PowerLevelLast value should be used as the default value.
 */
@property (nonatomic, readonly) UInt16 powerLevel;

/**
 Initialize new Generic PowerLevelDefault functionality object.

 @param powerLevel PowerLevel value that functionality should be initialized with.
 @return An initialized Generic PowerLevelDefault functionality object, or nil if an object could not be created.
 */
- (instancetype)initWithPowerLevel:(UInt16)powerLevel;

/**
 Not available. Use initWithPowerLevel instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithPowerLevel instead.
 */
- (instancetype)init NS_UNAVAILABLE;


@end

NS_ASSUME_NONNULL_END
