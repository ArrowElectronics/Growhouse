//
//  SBMVendorRegistrationSettings.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 08/12/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "SBMVendorModel.h"
#import "SBMBluetoothMeshMacros.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Object that represents vendor registration settings.
 */
BLUETOOTH_MESH_API_DEPRECATED_USE_INSTEAD(2.1.0, "SBMLocalVendorSettings")
@interface SBMVendorRegistrationSettings : NSObject

/**
 Callback for handling incoming vendor messages.

 @param model Model receiving the message.
 @param applicationKeyIndex Index of the application key used in encryption.
 @param sourceAddress Source address.
 @param destinationAddress Destination address.
 @param virtualAddress Full label UUID, or nil if the destination address is not a virtual address.
 @param message Application message, including opcode.
 @param messageFlags Indicates a non-relayed message if 0x04 is set.
 */
typedef void (^SBMVendorRegistrationSettingsMessageHandler)(SBMVendorModel *model,
                                                            NSInteger applicationKeyIndex,
                                                            NSInteger sourceAddress,
                                                            NSInteger destinationAddress,
                                                            NSData *virtualAddress,
                                                            NSData *message,
                                                            UInt8 messageFlags) BLUETOOTH_MESH_API_DEPRECATED_USE_INSTEAD(2.1.0, "SBMLocalVendorSettings");

/**
 Callback for handling periodic publishing.

 @param model Model publishing a message.
 @param messageFlags Return parameter for writing message flags that are used in sending the message out.
 @return Data containing the message to publish.
 */
typedef NSData* (^SBMVendorRegistrationSettingsPublishHandler)(SBMVendorModel *model, UInt8 *messageFlags) BLUETOOTH_MESH_API_DEPRECATED(2.1.0);

/**
 Data that contains the opcodes that the model handles.
 */
@property (nonatomic) NSData *opCodes  BLUETOOTH_MESH_API_DEPRECATED_USE_INSTEAD(2.1.0, "SBMLocalVendorSettings");;

/**
 Callback for receiving incoming messages.
 */
@property (nonatomic) SBMVendorRegistrationSettingsMessageHandler messageHandler  BLUETOOTH_MESH_API_DEPRECATED_USE_INSTEAD(2.1.0, "SBMLocalVendorSettings");

/**
 Callback for publishing messages. May be nil if the model does not support publishing.
 */
@property (nonatomic) SBMVendorRegistrationSettingsPublishHandler publishHandler  BLUETOOTH_MESH_API_DEPRECATED(2.1.0);

@end

NS_ASSUME_NONNULL_END
