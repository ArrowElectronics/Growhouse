//
//  SBMLightningCTLTemperatureRange.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 23/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import "SBMControlValueGetSigModel.h"
#import "SBMControlValueSetSigModel.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Lightning CTLTemperatureRange functionality object. It represents the minimum and maximum color temperatures of tunable white light an element is capable of emitting.
 */
@interface SBMLightningCTLTemperatureRange : NSObject<SBMControlValueGetSigModel, SBMControlValueSetSigModel>

/**
 Value of Lightning CTLTemperatureRange functionality minimum. Values from 0x320 to 0x4E20 represent the color temperature of white light in Kelvin. The value of 0xFFFF means that the color temperature of white light is unknown. All other values are prohibited.
 */
@property (nonatomic, readonly) UInt16 min;

/**
 Value of Lightning CTLTemperatureRange functionality maximum. Values from 0x320 to 0x4E20 represent the color temperature of white light in Kelvin. The value of 0xFFFF means that the color temperature of white light is unknown. All other values are prohibited.
 */
@property (nonatomic, readonly) UInt16 max;

/**
 Initialize new Lightning CTLTemperatureRange functionality object.

 @param min CTLTemperature minimum value that functionality should be initialized with.
 @param max CTLTemperature maximum value that functionality should be initialized with.
 @return An initialized Lightning CTLTemperatureRange functionality object, or nil if an object could not be created.
 */
- (instancetype)initWithMin:(UInt16)min max:(UInt16)max;

/**
 Not available. Use initWithMin instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithMin instead.
 */
- (instancetype)init NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
