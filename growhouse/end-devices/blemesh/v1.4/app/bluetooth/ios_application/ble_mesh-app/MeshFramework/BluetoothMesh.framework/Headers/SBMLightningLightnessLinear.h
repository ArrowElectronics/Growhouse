//
//  SBMLightningLightnessLinear.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 26/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import "SBMControlValueGetSigModel.h"
#import "SBMControlValueSetSigModel.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Lightning LightnessLinear functionality object. It represents the lightness of a light on a linear scale.
 */
@interface SBMLightningLightnessLinear : NSObject<SBMControlValueGetSigModel, SBMControlValueSetSigModel>

/**
 Value of Lightning LightnessLinear functionality.
 */
@property (nonatomic, readonly) UInt16 lightness;

/**
 Initialize new Lightning LightnessLinear functionality object.

 @param lightness Lightness value that functionality should be initialized with.
 @return An initialized Lightning LightnessLinear functionality object, or nil if an object could not be created.
 */
- (instancetype)initWithLightness:(UInt16)lightness;

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
