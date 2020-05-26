//
//  SBMLightningLightnessRange.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 23/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import "SBMControlValueGetSigModel.h"
#import "SBMControlValueSetSigModel.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Lightning LightnessRange functionality object. It represents the minimum and maximum lightness of and element.
 */
@interface SBMLightningLightnessRange : NSObject<SBMControlValueGetSigModel, SBMControlValueSetSigModel>

/**
 Value of Lightning LightnessRange functionality minimum. 0 is prohibited.
 */
@property (nonatomic, readonly) UInt16 min;

/**
 Value of Lightning LightnessRange functionality maximum. 0 is prohibited.
 */
@property (nonatomic, readonly) UInt16 max;

/**
 Initialize new Lightning LightnessRange functionality object.

 @param min Lightness minimum value that functionality should be initialized with.
 @param max Lightness maximum value that functionality should be initialized with.
 @return An initialized Lightning LightnessRange functionality object, or nil if an object could not be created.
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
