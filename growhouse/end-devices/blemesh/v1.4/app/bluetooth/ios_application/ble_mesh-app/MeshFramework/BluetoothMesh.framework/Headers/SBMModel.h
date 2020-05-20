//
//  SBMModel.h
//  ble_mesh-lib
//
//  Created by Lukasz Rzepka on 04.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>
@class SBMElement;
@class SBMGroup;

NS_ASSUME_NONNULL_BEGIN

/**
 Model is an object created after receive the device composition data (DCD) for a Node.
 */
@protocol SBMModel <NSObject>

/**
 Identifier of the model.
 */
@property (nonatomic, readonly) NSInteger identifier;

/**
 Name of the model.
 */
@property (nonatomic, readonly) NSString* name;

/**
 Element that model is contained in.
 */
@property (nonatomic, readonly, weak) SBMElement* meshElement;

/**
 A boolean value that determines whether model is SIG.
 */
@property (nonatomic, readonly) BOOL isSIGModel;

/**
 Array of groups that are bound to this model.
 */
@property (nonatomic, readonly) NSArray<SBMGroup *> *bindedGroups;

@end

NS_ASSUME_NONNULL_END
