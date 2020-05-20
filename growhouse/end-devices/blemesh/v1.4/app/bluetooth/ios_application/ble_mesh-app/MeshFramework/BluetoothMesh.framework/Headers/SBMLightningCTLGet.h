//
//  SBMLightningCTLGet.h
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
@interface SBMLightningCTLGet : NSObject<SBMControlValueGetSigModel>

/**
 Value of Lightning CTLGet functionality lightness.
 */
@property (nonatomic, readonly) UInt16 lightness;

/**
 Value of Lightning CTLGet functionality temperature.
 */
@property (nonatomic, readonly) UInt16 temperature;

/**
 Not available.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available.
 */
- (instancetype)init NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
