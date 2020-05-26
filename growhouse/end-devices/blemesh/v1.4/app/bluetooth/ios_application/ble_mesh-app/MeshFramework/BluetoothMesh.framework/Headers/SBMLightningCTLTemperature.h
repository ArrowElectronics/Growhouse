//
//  SBMLightningCTLTemperature.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 23/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import "SBMControlValueGetSigModel.h"
#import "SBMControlValueSetSigModel.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Lightning CTLTemperature functionality object. It represents the color temperature of tunable white light emitted by an element.
 */
@interface SBMLightningCTLTemperature : NSObject<SBMControlValueGetSigModel, SBMControlValueSetSigModel>

/**
 Value of Lightning CTLTemperature functionality temperature.
 */
@property (nonatomic, readonly) UInt16 temperature;

/**
 Value of Lightning CTLTemperature functionality deltaUV. It determines the distance from the Black Body curve.
 */
@property (nonatomic, readonly) SInt16 deltaUv;

/**
 Initialize new Lightning CTLTemperature functionality object.

 @param temperature Temperature value that functionality should be initialized with.
 @param deltaUv DeltaUV value that functionality should be initialized with.
 @return An initialized Lightning CTLTemperature functionality object, or nil if an object could not be created.
 */
- (instancetype)initWithTemperature:(UInt16)temperature deltaUv:(SInt16)deltaUv;

/**
 Not available. Use initWithTemperature instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithTemperature instead.
 */
- (instancetype)init NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
