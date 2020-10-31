//
//  SBMBluetoothAdapter.h
//  ble_mesh-lib
//
//  Created by Michal Lenart on 19/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMGattServiceTypeEnum.h"

///-------------------------------
/// @name Provisioning
///-------------------------------

/**
 The Mesh Provisioning Service.
 */
static NSString * _Nonnull meshProvisioningServiceUUID = @"1827";

/**
 The Mesh Provisioning Data In characteristic is used to write data to device during provisioning process,
 */
static NSString * _Nonnull characteristicMeshProvisioningDataInUUID = @"2ADB";

/**
 The Mesh Provisioning Data Out characteristic is used to read data from device during provisioning process.
 */
static NSString * _Nonnull characteristicMeshProvisioningDataOutUUID = @"2ADC";


///-------------------------------
/// @name Proxy
///-------------------------------

/**
 The Mesh Proxy Service.
 */
static NSString * _Nonnull meshProxyServiceUUID = @"1828";

/**
 The Mesh Proxy Data In characteristic is used to write data to proxy device.
 */
static NSString * _Nonnull characteristicMeshProxyDataInUUID = @"2ADD";

/**
 The Mesh Proxy Data Out characteristic is used to read data from proxy device.
 */
static NSString * _Nonnull characteristicMeshProxyDataOutUUID = @"2ADE";

@class SBMBluetoothAdapter;

/**
 SBMBluetoothAdapterDelegate it is a delegate used by Mesh library to send messages to Bluetooth devices. It is necessary to prepare implantation for BluetoothAdapter delegate.
 */

@protocol SBMBluetoothAdapterDelegate

/**
 Request send by Mesh library to prepare Bluetooth connection with the specified Mesh service. GATT connection must be already opened. After this call the platform must do a GATT service discovery, locate the data out characteristic for the requested service, and turn notifications on for it. After this the connection is considered open.
 
 @param bluetoothAdapter SBMBluetoothAdapter object.
 @param gattHandle Integer value. It is a value containing a GATT connection handler for a Bluetooth device.
 @param gattServiceType Enum value. Contain information with which service connection should be established.
 */
- (void)bluetoothAdapter:(nonnull SBMBluetoothAdapter *)bluetoothAdapter gattRequestToGattHandle:(NSInteger)gattHandle :(SBMGattServiceType)gattServiceType;

/**
 Request send by Mesh Library to close a Bluetooth connection.
 
 @param bluetoothAdapter SBMBluetoothAdapter object.
 @param gattHandle Integer value. It is a value containing a GATT connection handler for a Bluetooth device.
 */
- (void)bluetoothAdapter:(nonnull SBMBluetoothAdapter *)bluetoothAdapter disconnectionRequestToGattHandle:(NSInteger)gattHandle;

/**
 Request send by Mesh Library to send data over a GATT connection. Send the data to the remote device.
 
 @param bluetoothAdapter SBMBluetoothAdapter object.
 @param gattHandle Integer value. It is a value containing a GATT connection handler for a Bluetooth device.
 @param data Data object. Data needed to send to the Bluetooth device.
 @param serviceUUID String object. Send data to characteristic from this Bluetooth service for Bluetooth device. It is string for meshProvisioningServiceUUID or meshProxyServiceUUID.
 */
- (void)bluetoothAdapter:(nonnull SBMBluetoothAdapter *)bluetoothAdapter gattWriteToGattHandle:(NSInteger)gattHandle data:(nonnull NSData *)data serviceUUID:(nonnull NSString *)serviceUUID characteristic:(nonnull NSString *)characteristicUUID;
@end

/**
 SBMBluetoothAdapter is a GATT event handler. BluetoothAdapter object is responsible for exchange messages between Bluetooth peripherals and Mesh library. Is is the gate of communication from the Bluetooth Mesh iOS Framework to the user application, all the operations that depends on the Bluetooth Mesh network will return it’s value to delegate that must be implemented in the application.
 */

@interface SBMBluetoothAdapter : NSObject

/**
 SBMBluetoothAdapterDelegate it is a delegate used by Mesh library to send messages to Bluetooth devices. It is needed to set this property.
 */
@property (nonatomic, nullable, weak) id<SBMBluetoothAdapterDelegate> delegate;

/**
 Inform Bluetooth Mesh about successfully prepared Bluetooth connection with the specified Mesh service containing GATT service discovery, locate the data out characteristic for the requested service, and turn notifications on for it.
 
 @param gattHandle Integer value. It is a value containing a GATT connection handler for a Bluetooth device.
 */
- (void)connectGatt:(NSInteger)gattHandle;

/**
 Inform Bluetooth Mesh about close Bluetooth connection.
 */
- (void)disconnectGatt:(NSInteger)gattHandle;

/**
 Call this method with received data from subscribed Bluetooth characteristic. To receive data it is needed to activate notification for required characteristic.
 
 @param gattHandle Integer value. It is a value containing a GATT connection handler for a Bluetooth device.
 @param data Data object. Received data from the Bluetooth device.
 */
- (void)writeDataToGattHandle:(NSInteger)gattHandle byte:(nonnull NSData *)data;

/**
 Call this method after MTU change. The maximum amount of data, in bytes, that can be sent to a characteristic in a single write type. MTU can be taken from maximumWriteValueLength from the connected CBPeripheral for CBCharacteristicWriteWithoutResponse.
 
 @param MTU Integer value. Value for maximum write length.
 */
- (void)updateMTU:(NSInteger)MTU;

@end
