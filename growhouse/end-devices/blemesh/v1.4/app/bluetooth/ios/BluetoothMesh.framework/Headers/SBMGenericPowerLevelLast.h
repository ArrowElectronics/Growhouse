//
//  SBMGenericPowerLevelLast.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 26/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import "SBMControlValueGetSigModel.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Generic PowerLevelLast functionality object. It represents the last known non-zero value of the power level state of an element.
 */
@interface SBMGenericPowerLevelLast : NSObject<SBMControlValueGetSigModel>

/**
 Value of Generic PowerLevelLast functionality. 0 is prohibited.
 */
@property (nonatomic, readonly) UInt16 powerLevel;

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
