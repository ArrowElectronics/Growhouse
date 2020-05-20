//
//  SBMSigModel.h
//  BluetoothMesh
//
//  Created by Arkadiusz Marszałek on 23/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import "SBMModel.h"
#import "SBMModelIdentifier.h"

NS_ASSUME_NONNULL_BEGIN

/**
 SIG Model.
 */
@interface SBMSigModel : NSObject<SBMModel>

/**
 A boolean value that determines whether SIG model supports publishing.
 */
@property (nonatomic, readonly) BOOL supportPublish;

/**
 A boolean value that determines whether SIG model supports subscribing.
 */
@property (nonatomic, readonly) BOOL supportSubscribe;

/**
 Identifier of the SIG model.
 */
@property (nonatomic, readonly) SBMModelIdentifier modelIdentifier;

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
