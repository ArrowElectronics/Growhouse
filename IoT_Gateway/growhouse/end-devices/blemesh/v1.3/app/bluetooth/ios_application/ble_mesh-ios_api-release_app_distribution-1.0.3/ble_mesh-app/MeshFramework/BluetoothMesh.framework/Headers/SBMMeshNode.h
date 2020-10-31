//
//  SBMMeshNode.h
//  ble_mesh-lib
//
//  Created by Michal Lenart on 23/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMMeshNodeConfiguration.h"
#import "SBMDeviceCompositionData.h"

/**
 A SBMMeshNode is the object that contains all the information about a Bluetooth Mesh device in a Network. It is created right after provision a device. After add node to group(Configure available models from DCD), it will be possible to get all configured Elements and Models from the configuration object. DCD can be also downloaded manualu without adding to the specific group.
 */

@interface SBMMeshNode : NSObject

/**
 Node name.
 */
@property (nonatomic, readonly, nonnull) NSString* name;

/**
 Mesh device UUID taken from the Bluetooth device advertisement.
 */
@property (nonatomic, readonly, nonnull) NSData* meshDeviceUUID;

/**
 Address index of the Node in Bluetooth Mesh
 */
@property (nonatomic, readonly) UInt16 meshAddress;

/**
 After add Node to a specific Network, there will be available index of this Network in the Bluetooth Mesh.
 */
@property (nonatomic, readonly) UInt16 networkIndex;

/**
 Node configuration.
 */
@property (nonatomic, readonly, nullable) SBMMeshNodeConfiguration* configuration;

/**
 Only framework can create object.
 */
- (nonnull instancetype)init NS_UNAVAILABLE;

@end
