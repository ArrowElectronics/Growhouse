//
//  SBMMeshManager.h
//  ble_mesh-lib
//
//  Created by Lukasz Rzepka on 10.04.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMNetwork.h"
#import "SBMBluetoothMeshConfiguration.h"

NS_ASSUME_NONNULL_BEGIN

/**
 It is the main object of the Bluetooth Mesh iOS API. 
 */
@interface SBMBluetoothMesh : NSObject

/**
 SBMBluetoothMesh is a singleton object. Before first usage call initialize method with configuration.
 */
+ (SBMBluetoothMesh *)sharedInstance;

/**
 Not available. Use sharedInstance instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use sharedInstance instead.
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 Initialize of BluetoothMesh stack.
 It is needed to provide configuration of BluetoothMesh before useage of framework.
 
 @param configuration SBMBluetoothMeshConfiguration.
 */
- (void)initialize:(SBMBluetoothMeshConfiguration *)configuration;

/**
 Get initialized mesh configuration.
 
 @return SBMBluetoothMeshConfiguration mesh configuration.
 */
- (nullable SBMBluetoothMeshConfiguration *)configuration;

/**
 Create a Network object. It is a main container for Mesh objects.
 
 @param name Name of Network.
 */
- (SBMNetwork *)createNetwork:(NSString *)name;

/**
 Method used to get existing networks.

 @return Array of existing networks.
 */
- (NSArray<SBMNetwork *> *)networks;

/**
 Remove network from local storage. It is needed to remove all subnets before remove network.

 @param network Network to remove.
 @param error Error describing reason of failure.
 */
- (void)removeNetwork:(SBMNetwork *)network error:(NSError **)error __attribute__((swift_error(nonnull_error)));

/**
 Clear local Mesh structure.
 */
- (void)clearDatabase;

@end

NS_ASSUME_NONNULL_END
