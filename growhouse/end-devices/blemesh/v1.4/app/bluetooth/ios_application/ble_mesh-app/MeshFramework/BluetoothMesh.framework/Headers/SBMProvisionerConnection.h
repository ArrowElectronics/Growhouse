//
//  SBMProvisionerConnection.h
//  BluetoothMesh
//
//  Created by Arkadiusz Marszałek on 11/12/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMConnectableDevice.h"
#import "SBMSubnet.h"
#import "SBMProvisionerOOB.h"
#import "SBMProvisionerParameters.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Object that is used to provision device.
 */
@interface SBMProvisionerConnection : NSObject

/**
 Action invoked on provisioning completion.

 @param provisioner Provisioner connection.
 @param node Provisioned node, or nil if provisioning failed.
 @param error Error describing reason of failure, or nil if provisioning completed successfully.
 */
typedef void (^SBMProvisionerProvisioningCallback)(SBMProvisionerConnection *provisioner, SBMNode * _Nullable node, NSError * _Nullable error);

/**
 Connectable device.
 */
@property (nonatomic, strong, readonly) id<SBMConnectableDevice> device;

/**
 Subnet to which the provisioned device should be added.
 */
@property (nonatomic, strong, readonly) SBMSubnet *subnet;

/**
 Out of band provisioner object.
 */
@property (nonatomic, strong, nullable) id<SBMProvisionerOOB> provisionerOOB;

/**
 Initialize new provisioner connection object.

 @param device Device for which provisioner connection object should be created.
 @param subnet Subnet in which given device is.
 @return An initialized provisioner connection object, or nil if an object could not be created.
 */
- (instancetype)initForDevice:(id<SBMConnectableDevice>)device subnet:(SBMSubnet *)subnet;

/**
 Not available. Use initForDevice instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initForDevice instead.
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 Method used to provision device as subnet node.

 @param callback Action invoked on completed provisioning.
 */
- (void)provisionAsNode:(SBMProvisionerProvisioningCallback)callback;

/**
 Method used to provision device with parameters as subnet node.

 @param parameters Parameters of the device to be provisioned.
 @param callback Action invoked on completed provisioning.
 */
- (void)provisionAsNodeWithParameters:(SBMProvisionerParameters* _Nullable)parameters callback:(SBMProvisionerProvisioningCallback)callback;

/**
 Method used to provision device as subnet node with setting it as proxy and node identity.

 @param callback Action invoked on completed provisioning.
 */
- (void)provisionAsProxy:(SBMProvisionerProvisioningCallback)callback;

/**
 Method used to provision device with parameters as subnet node with setting it as proxy and node identity.

 @param parameters Parameters of the device to be provisioned.
 @param callback Action invoked on completed provisioning.
 */
- (void)provisionAsProxyWithParameters:(SBMProvisionerParameters* _Nullable)parameters callback:(SBMProvisionerProvisioningCallback)callback;

/**
 Method used to provision device as subnet node with setting it as proxy without node identity.

 @param callback Action invoked on completed provisioning.
 */
- (void)provisionAsProxyWithoutNodeIdentity:(SBMProvisionerProvisioningCallback)callback;

/**
 Method used to provision device with parameters as subnet node with setting it as proxy without node identity.

 @param parameters Parameters of the device to be provisioned.
 @param callback Action invoked on completed provisioning.
 */
- (void)provisionAsProxyWithoutNodeIdentityWithParameters:(SBMProvisionerParameters* _Nullable)parameters callback:(SBMProvisionerProvisioningCallback)callback;

/**
 The Mesh Provisioning Service.
 */
+ (NSString*)meshProvisioningServiceUUID;

/**
 The Mesh Provisioning Data In characteristic is used to write data to device during provisioning process,
 */
+ (NSString*)characteristicMeshProvisioningDataInUUID;

/**
 The Mesh Provisioning Data Out characteristic is used to read data from device during provisioning process.
 */
+ (NSString*)characteristicMeshProvisioningDataOutUUID;

@end

NS_ASSUME_NONNULL_END
