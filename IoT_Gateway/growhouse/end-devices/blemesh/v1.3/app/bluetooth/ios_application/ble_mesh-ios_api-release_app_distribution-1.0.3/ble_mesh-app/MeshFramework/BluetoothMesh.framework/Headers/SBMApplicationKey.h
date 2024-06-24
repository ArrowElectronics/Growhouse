//
//  SBMApplicationKey.h
//  ble_mesh-lib
//
//  Created by Lukasz Rzepka on 04.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>

/**
 It is a container for application key index. Application key is used to bind model from the device with specific group. Each SBMMeshGroup has own application key index.
*/

@interface SBMApplicationKey : NSObject

/**
 Key is an application key index for application key from the mesh.
 */
@property (nonatomic, readonly) NSInteger key;
@end
