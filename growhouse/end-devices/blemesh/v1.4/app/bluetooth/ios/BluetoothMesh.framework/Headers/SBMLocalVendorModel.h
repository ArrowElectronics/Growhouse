//
//  SBMLocalVendorModelClient.h
//  BluetoothMesh
//
//  Created by Łukasz Rzepka on 12.12.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMApplicationKey.h"

@class SBMLocalVendorSettings;

NS_ASSUME_NONNULL_BEGIN

/**
 Vendor model to support by Mesh.
 */
@interface SBMLocalVendorModel : NSObject

/**
 Bluetooth assigned Company Identifier.
 */
@property (nonatomic, readonly) UInt16 vendorCompanyIdentifier;

/**
 Vendor assigned Model Identifier.
 */
@property (nonatomic, readonly) UInt16 vendorAssignedModelIdentifier;

/**
 SBMLocalVendorModel initialization.
 
 @param vendorCompanyIdentifier Vendor company identifier.
 @param vendorAssignedModelIdentifier Vendor assigned model identifier.
 */
- (instancetype)initWithVendorCompanyIdentifier:(UInt16)vendorCompanyIdentifier
                  vendorAssignedModelIdentifier:(UInt16)vendorAssignedModelIdentifier;

/**
 Not available. Use initWithVendorCompanyIdentifier instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithVendorCompanyIdentifier instead.
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 Object that represents registration settings.
 
 To configure local vendor model registration settings use SBMLocalVendorRegistrator.
 
 @return SBMLocalVendorSettings object if local vendor model is registered, or nil if vendor model is NOT registered.
 */
- (nullable SBMLocalVendorSettings *)registrationSettings;

@end

NS_ASSUME_NONNULL_END
