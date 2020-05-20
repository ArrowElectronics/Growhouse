//
//  SBMControlValueSetVendorModel.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 04/12/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMVendorModel.h"
#import "SBMLocalVendorModel.h"
#import "SBMBluetoothMeshMacros.h"

/**
 Possible vendor model message flags.
 */
typedef NS_ENUM(UInt8, SBMControlValueSetVendorModelFlag) {
    /** Reply. */
    SBMControlValueSetVendorModelFlagReply          = 1 << 0,
    /** Multicast reply. */
    SBMControlValueSetVendorModelFlagMulticastReply = 1 << 1,
    /** Non-relayed message. */
    SBMControlValueSetVendorModelFlagTtl0           = 1 << 2,
    /** Transport layer segmentation is forced also for short messages. */
    SBMControlValueSetVendorModelFlagSegtrans       = 1 << 4,
};

NS_ASSUME_NONNULL_BEGIN

/**
 Base functionality protocol. Any functionality that can be used to set Vendor model values should conform to this protocol.
 */
@protocol SBMControlValueSetVendorModel <NSObject>

/**
 Local vendor model.
 */
@property (nonatomic, readonly, nullable) SBMLocalVendorModel *localVendorModelClient BLUETOOTH_MESH_API_DEPRECATED_USE_INSTEAD(2.1.0, "Use vendorModel instead.");

/**
 Vendor model to control.
 */
@property (nonatomic, readonly) SBMVendorModel *vendorModel;

/**
 The application message, including opcode.
 */
@property (nonatomic, readonly) NSData *data;

/**
 Specifies vendor model message flags.
 */
@property (nonatomic, readonly) SBMControlValueSetVendorModelFlag flags;

@end

NS_ASSUME_NONNULL_END
