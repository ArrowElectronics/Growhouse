//
//  SBMLocalVendorSettings.h
//  BluetoothMesh
//
//  Created by Lukasz Rzepka on 20/03/2019.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "SBMLocalVendorModel.h"
#import "SBMVendorModel.h"
#import "SBMBluetoothMeshMacros.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Object that represents vendor registration settings.
 */
@interface SBMLocalVendorSettings : NSObject

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
typedef void (^SBMLocalVendorSettingsMessageHandler)(SBMLocalVendorModel *model,
                                                     NSInteger applicationKeyIndex,
                                                     NSInteger sourceAddress,
                                                     NSInteger destinationAddress,
                                                     NSData * _Nullable virtualAddress,
                                                     NSData *message,
                                                     UInt8 messageFlags);

/**
 Initialize object with opCodes and messageHandler.
 */
- (instancetype)initWithOpCodes:(NSData *)opCodes messageHandler:(SBMLocalVendorSettingsMessageHandler)messageHandler;

/**
 Not available. Use initWithOpCodes:messageHandler instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithOpCodes:messageHandler instead.
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 Data that contains the opcodes that the model handles.
 */
@property (nonatomic, readonly) NSData *opCodes;

/**
 Callback for receiving incoming messages.
 */
@property (nonatomic, readonly) SBMLocalVendorSettingsMessageHandler messageHandler;

@end

NS_ASSUME_NONNULL_END
