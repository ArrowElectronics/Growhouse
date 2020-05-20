//
//  SBMLightningCTLDefault.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 26/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import "SBMControlValueGetSigModel.h"
#import "SBMControlValueSetSigModel.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Lightning CTLDefault functionality object. It represents the color temperature of an element when the device is powered up and when the Generic OnPowerUp state bound to the Lightning CTLTemperature state is set to 0x01 (Default).
 */
@interface SBMLightningCTLDefault : NSObject<SBMControlValueGetSigModel, SBMControlValueSetSigModel>

/**
 Value of Lightning CTLDefault functionality lightness.
 */
@property (nonatomic, readonly) UInt16 lightness;

/**
 Value of Lightning CTLDefault functionality temperature.
 */
@property (nonatomic, readonly) UInt16 temperature;

/**
 Value of Lightning CTLDefault functionality deltaUV.
 */
@property (nonatomic, readonly) SInt16 deltaUv;

/**
 Initialize new Lightning CTLDefault functionality object.

 @param lightness Lightness value that functionality should be initialized with.
 @param temperature Temperature value that functionality should be initialized with.
 @param deltaUv DeltaUV value that functionality should be initialized with.
 @return An initialized Lightning CTLDefault functionality object, or nil if an object could not be created.
 */
- (instancetype)initWithLightness:(UInt16)lightness temperature:(UInt16)temperature deltaUv:(SInt16)deltaUv;

/**
 Not available. Use initWithLightness instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithLightness instead.
 */
- (instancetype)init NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
