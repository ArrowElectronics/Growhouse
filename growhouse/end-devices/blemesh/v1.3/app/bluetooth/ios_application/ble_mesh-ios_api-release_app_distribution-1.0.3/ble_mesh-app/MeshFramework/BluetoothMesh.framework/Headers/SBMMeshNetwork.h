//
//  SBMMeshNetwork.h
//  ble_mesh-lib
//
//  Created by Michal Lenart on 23/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMMeshGroup.h"
#import "SBMMeshNode.h"

/**
 A SBMMeshNetwork object contains informations about the Bluetooth Mesh Network.
 */

@interface SBMMeshNetwork : NSObject

/**
 Network name.
 */
@property (nonatomic, readonly, nonnull) NSString* name;

/**
 Network key index.
 */
@property (nonatomic, readonly) UInt16 index;

/**
 Network key data
 */
@property (nonatomic, readonly, nonnull) NSData* key;

/**
 Only framework can create object.
 */
- (nonnull instancetype)init NS_UNAVAILABLE;

@end
