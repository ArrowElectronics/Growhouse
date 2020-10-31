//
//  SBMConfiguration.h
//  ble_mesh-lib
//
//  Created by Lukasz Rzepka on 04.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMApplicationKey.h"
#import "SBMElement.h"
#import "SBMDeviceFunctionality.h"

/**
 Configuration of the Node.
 */

@interface SBMMeshNodeConfiguration : NSObject

/**
 Active node functionality.
 */
@property (nonatomic, strong, readonly, nullable) SBMDeviceFunctionality* activeFunctionality;

/**
 After add Node to Group in this array will be available Application Key of this group. It is possible to add one Node to many Groups.
 */
@property (nonatomic, strong, readonly, nonnull) NSArray<SBMApplicationKey*>* applicationKeys;

/**
 There will be available configured Models of the Node. Configured means that Model is binded with Application Key and it means that Model is added to Group. It is possible to add Model to Group by adding Node to Group.
 
 @param elementIndex Integer value. Index of Element from which Models will be taken.
 @return Array of the Models.
 */
- (nullable NSArray<SBMModel*>*)configuredModelsForElementIndex:(NSInteger)elementIndex;
@end
