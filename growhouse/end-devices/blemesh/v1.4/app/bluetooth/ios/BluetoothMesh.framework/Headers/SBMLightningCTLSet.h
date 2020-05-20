//
//  SBMLightningCTL.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 23/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import "SBMControlValueGetSigModel.h"
#import "SBMControlValueSetSigModel.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Lightning CTLGet functionality object. It represents the color temperature of tunable white light emitted by an element.
 */
@interface SBMLightningCTLSet : NSObject<SBMControlValueSetSigModel>

/**
 Value of Lightning CTLSet functionality lightness.
 */
@property (nonatomic, readonly) UInt16 lightness;

/**
 Value of Lightning CTLSet functionality temperature.
 */
@property (nonatomic, readonly) UInt16 temperature;

/**
 Value of Lightning CTLSet functionality deltaUV.
 */
@property (nonatomic, readonly) SInt16 deltaUv;

/**
 Initialize new Lightning CTLSet functionality object.

 @param lightness Lightness value that functionality should be initialized with.
 @param temperature Temperature value that functionality should be initialized with.
 @param deltaUv DeltaUV value that functionality should be initialized with.
 @return An initialized Lightning CTLSet functionality object, or nil if an object could not be created.
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
