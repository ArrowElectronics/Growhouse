//
//  SBMBluetoothMeshConfiguration.h
//  BluetoothMesh
//
//  Created by Łukasz Rzepka on 03.12.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMLogger.h"
#import "SBMLocalVendorModel.h"
#import "SBMBluetoothMeshConfigurationLimits.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Object for Mesh configuration.
 It is a place to specify which vendor models should be supported by framework.
 It is needed to provide SBMLogger object with specified delegate to receive logs from framework.
*/
@interface SBMBluetoothMeshConfiguration : NSObject

/**
 Mesh logger.
 */
@property (nonatomic, readonly) SBMLogger* logger;

/**
 Supported vendor models.
 */
@property (nonatomic, readonly) NSArray<SBMLocalVendorModel *> *localVendorModels;

/**
 Object specifying configuration limits.
 */
@property (nonatomic, readonly) SBMBluetoothMeshConfigurationLimits* limits;

/**
 SBMBluetoothMeshConfiguration initialization. Use this initializer to set custom configuration limits.
 @warning Changing configuration limits between each application launch may cause the database to be corrupted.
 @param localVendorModels NSArray contain vendor models that should be supported by Mesh.
 @param limits SBMBluetoothMeshConfigurationLimits object that specifies configuration limits.
 @param logger SBMLogger object to receive Mesh logs.
 */
- (instancetype)initWithLocalVendorModels:(NSArray<SBMLocalVendorModel *> *)localVendorModels
                                   limits:(SBMBluetoothMeshConfigurationLimits*)limits
                                andLogger:(SBMLogger *)logger;

/**
 SBMBluetoothMeshConfiguration initialization. When this initializer is used, configuration limits are set to default. See defaultLimits.
 @param localVendorModels NSArray contain vendor models that should be supported by Mesh.
 @param logger SBMLogger object to receive Mesh logs.
 */
- (instancetype)initWithLocalVendorModels:(NSArray<SBMLocalVendorModel *> *)localVendorModels
                                andLogger:(SBMLogger *)logger;

/**
 Not available. Use initWithLocalVendorModels instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithLocalVendorModels instead.
 */
- (instancetype)init NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
