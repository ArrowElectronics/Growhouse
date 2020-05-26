//
//  SBMBluetoothMeshConfigurationLimits.h
//  BluetoothMesh
//
//  Created by Arkadiusz Marszałek on 15/03/2019.
//  Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 Object specifying configuration limits.
 */
@interface SBMBluetoothMeshConfigurationLimits : NSObject

/**
 Maximum number of networks. Maximum supported value is 16.
 Default value is 4.
 */
@property (nonatomic, readwrite) UInt8 networks;

/**
 Maximum number of groups.
 Default value is 10.
 */
@property (nonatomic, readwrite) UInt8 groups;

/**
 Maximum number of provisioned nodes.
 Default value is 255.
 */
@property (nonatomic, readwrite) UInt8 nodes;

/**
 Maximum number of networks that provisioned node can store.
 Default value is 4.
 */
@property (nonatomic, readwrite) UInt8 nodeNetworks;

/**
 Maximum number of groups that provisioned node can store.
 Default value is 4.
 */
@property (nonatomic, readwrite) UInt8 nodeGroups;

/**
 Size of replay protection list. It specifies maximum number of nodes that can be communicated with.
 Default value is 32.
 */
@property (nonatomic, readwrite) UInt8 rplSize;

/**
 Maximum number of concurrent segmented messages being received.
 Default value is 4.
 */
@property (nonatomic, readwrite) UInt8 segmentedMessagesReceived;

/**
 Maximum number of concurrent segmented messages being sent.
 Default value is 4.
 */
@property (nonatomic, readwrite) UInt8 segmentedMessagesSent;

/**
 Maximum number of parallel provisioning sessions.
 Default value is 2.
 */
@property (nonatomic, readwrite) UInt8 provisionSessions;

@end

NS_ASSUME_NONNULL_END
