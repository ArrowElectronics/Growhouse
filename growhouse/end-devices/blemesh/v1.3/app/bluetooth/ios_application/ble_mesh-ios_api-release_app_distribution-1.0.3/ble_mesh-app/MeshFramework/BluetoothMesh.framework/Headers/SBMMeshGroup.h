//
//  SBMMeshGroup.h
//  ble_mesh-lib
//
//  Created by Michal Lenart on 24/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>

/**
 A SBMMeshGroup is the object that contains an Application Key and a publish and subscribe addresses that will be
 used to configure the models inside a SBMMeshNode. It is abstract part of Bluetooth Mesh not included in official Bluetooth Mesh Specification.
 */

@interface SBMMeshGroup : NSObject

/**
 Group name.
 */
@property (nonatomic, readonly, nonnull) NSString* name;

/**
 Group belongs to SBMMeshNetwork with this index.
 */
@property (nonatomic, readonly) UInt16 networkIndex;

/**
 Index of Application Key.
 */
@property (nonatomic, readonly) UInt16 applicationKeyIndex;

/**
 Property with Application Key data. This information is only for read and from the application perspective should be not used for anything more. Used in framework logic.
 */
@property (nonatomic, readonly, nonnull) NSData* applicationKey;

/**
 Address to publish message to Group. This information is only for read and from the application perspective should be not used for anything more. Used in framework logic.
 */
@property (nonatomic, readonly) UInt16 publicationAddress;

/**
 Address to subscribe to Group. This information is only for read and from the application perspective should be not used for anything more. Used in framework logic.
 */
@property (nonatomic, readonly) UInt16 subscriptionAddress;

@end
