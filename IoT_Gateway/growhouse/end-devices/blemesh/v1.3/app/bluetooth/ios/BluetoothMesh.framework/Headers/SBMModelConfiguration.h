//
//  SBMModelConfiguration.h
//  ble_mesh-lib
//
//  Created by Lukasz Rzepka on 18.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>

/**
 Model configuration.
 */

@interface SBMModelConfiguration : NSObject

/**
 Model supports publishing.
 */
@property (nonatomic, readonly) BOOL supportPublish;

/**
 Model supports subscribing.
 */
@property (nonatomic, readonly) BOOL supportSubscribe;
@end
