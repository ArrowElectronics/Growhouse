//
//  SBMModel.h
//  ble_mesh-lib
//
//  Created by Lukasz Rzepka on 04.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMModelIdentifierEnum.h"
#import "SBMModelConfiguration.h"

/**
 Model is an object created after receive the device composition data (DCD) for a Node. There is no much interaction with this object in this version.
 */

@interface SBMModel : NSObject

/**
 Information about supported configuration by Model
 */
@property (nonatomic, readonly, nonnull) SBMModelConfiguration* configuration;

/**
 Define if model is SIG Model or Vendor Model.
 */
@property (nonatomic, readonly) BOOL isSigModel;

/**
 Identifier of the model
 */
@property (nonatomic, readonly) SBMModelIdentifier identifier;

/**
 Model name
 */
@property (nonatomic, readonly, nonnull) NSString* name;
@end
