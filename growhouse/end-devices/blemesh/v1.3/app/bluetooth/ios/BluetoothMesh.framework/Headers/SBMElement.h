//
//  SBMElement.h
//  ble_mesh-lib
//
//  Created by Lukasz Rzepka on 04.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMModel.h"

/**
 Each node contains one or more Elements. Element is a container for a Models contained by Node.
 */
@interface SBMElement : NSObject

/**
 Index of Element.
 */
@property (nonatomic, readonly) NSInteger index;

/**
 Location of Element.
 */
@property (nonatomic, readonly) NSInteger location;

/**
 Unicast Address.
 */
@property (nonatomic, readonly) NSInteger unicastAddress;

/**
 Array of Models contained by Element.
 */
@property (nonatomic, readonly, nonnull) NSArray<SBMModel*>* models;
@end
