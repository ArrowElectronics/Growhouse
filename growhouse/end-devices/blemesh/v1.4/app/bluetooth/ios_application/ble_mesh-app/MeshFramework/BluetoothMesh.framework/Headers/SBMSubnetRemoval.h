//
//  SBMSubnetRemoval.h
//  BluetoothMesh
//
//  Created by Łukasz Rzepka on 05.12.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMSubnetRemovalErrorResult.h"
@class SBMSubnet;

/**
 Action invoked on successfull subnet removal.

 @param subnet Subnet that has been removed.
 */
typedef void(^SBMSubnetRemovalSuccess)(SBMSubnet *subnet);

/**
 Action invoked on failed subnet removal.

 @param subnet Subnet that was being removed.
 @param result Result of network removal operation.
 @param error Error describing reason of failure.
 */
typedef void(^SBMSubnetRemovalError)(SBMSubnet *subnet, SBMSubnetRemovalErrorResult *result, NSError *error);

/**
 Actoion invoked on failed subnet force removal.

 @param subnet Subnet that was being force removed.
 @param error Error describing reason of failure.
 */
typedef void(^SBMSubnetForceRemovalError)(SBMSubnet* subnet, NSError* error);
