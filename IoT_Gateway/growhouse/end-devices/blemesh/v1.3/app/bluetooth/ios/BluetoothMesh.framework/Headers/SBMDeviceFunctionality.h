//
//  SBMDeviceFunctionality.h
//  ble_mesh-lib
//
//  Created by Lukasz Rzepka on 23.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>

#import "SBMDeviceCompositionData.h"

/**
 Device functionality. This is mainly used for the setting the device's control option in the UI.
 */
@interface SBMDeviceFunctionality : NSObject

/**
 The model identifier is used to represent an element that can control and monitor the configuration of a node.
 */
@property (nonatomic, readonly) SBMModelIdentifier functionalityId;

/**
 This value is a index of Element containing this functionality.
 */
@property (nonatomic, readonly) NSInteger functionalityElementId;
@end
