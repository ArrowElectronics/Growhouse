//
//  SBMVendorModel.h
//  BluetoothMesh
//
//  Created by Arkadiusz Marszałek on 29/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import "SBMModel.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Vendor model.
 */
@interface SBMVendorModel : NSObject<SBMModel>

/**
 Bluetooth assigned Company Identifier.
 */
- (UInt16)vendorCompanyIdentifier;

/**
 Vendor assigned Model Identifier.
 */
- (UInt16)vendorAssignedModelIdentifier;

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
