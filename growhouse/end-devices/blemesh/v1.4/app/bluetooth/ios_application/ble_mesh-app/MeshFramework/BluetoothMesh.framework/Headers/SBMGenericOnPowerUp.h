//
//  SBMGenericOnPowerUp.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 23/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import "SBMControlValueGetSigModel.h"
#import "SBMControlValueSetSigModel.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Possible states of Generic OnPowerUp functionality.
 */
typedef NS_ENUM(NSInteger, SBMGenericOnPowerUpState) {
    /** Value that represents off state of an element. Element is in an off state after being powered up. */
    SBMGenericOnPowerUpStateOff,
    /** Value that represents on state of an element. Element is in an on state and uses default state values after being powered up. */
    SBMGenericOnPowerUpStateOn,
    /** Value that represents restore state of an element. If a transition was in progress when powered down, the element restores the target state when powered up. Otherwise the element restores the state it was in when powered down. */
    SBMGenericOnPowerUpStateRestore,
};

/**
 Generic OnPowerUp functionality object. It represents the behavior of an element when powered up.
 */
@interface SBMGenericOnPowerUp : NSObject<SBMControlValueGetSigModel, SBMControlValueSetSigModel>

/**
 State of Generic OnPowerUp functionality.
 */
@property (nonatomic, readonly) SBMGenericOnPowerUpState onPowerUpState;

/**
 Initialize new Generic OnPowerUp functionality object.

 @param onPowerUpState OnPowerUp state that functionality should be initialized with.
 @return An initialized Generic OnPowerUp functionality object, or nil if an object could not be created.
 */
- (instancetype)initWithOnPowerUpState:(SBMGenericOnPowerUpState)onPowerUpState;

/**
 Not available. Use initWithOnPowerUpState instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithOnPowerUpState instead.
 */
- (instancetype)init NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
