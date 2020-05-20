//
//  SBMNetwork.h
//  ble_mesh-lib
//
//  Created by Michal Lenart on 23/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMSubnet.h"

NS_ASSUME_NONNULL_BEGIN

/**
 A SBMNetwork object contains informations about the Bluetooth Mesh Network.
 */
@interface SBMNetwork : NSObject

/**
 Network's uuid.
 */
@property (nonatomic, readonly) NSData* uuid;

/**
 Network' name.
 */
@property (nonatomic, readwrite) NSString* name;

/**
 Network's subnets.
 */
@property (nonatomic, readonly) NSArray<SBMSubnet *> *subnets;

/**
 Checks if new Subnet object can be created.
 */
- (BOOL)canCreateSubnet;

/**
 Creates network's subnet with given name.
 If can return nil in case of max number of subnets is reached. See canCreateSubnet.

 @param name Name of subnet to create.
 @return Created subnet.
 */
- (SBMSubnet *)createSubnetWithName:(NSString*)name;

/**
 Returns network's primary subnet.

 @return Network's primary subnet.
 */
- (SBMSubnet *)getPrimarySubnet;

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
