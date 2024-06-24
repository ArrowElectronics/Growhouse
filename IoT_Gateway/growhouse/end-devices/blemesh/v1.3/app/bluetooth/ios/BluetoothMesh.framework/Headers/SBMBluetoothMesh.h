//
//  SBMMeshManager.h
//  ble_mesh-lib
//
//  Created by Lukasz Rzepka on 10.04.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMBluetoothAdapter.h"
#import "SBMMeshNetwork.h"

/**
 It is the main object of the Bluetooth Mesh iOS API. Exchange of messages between user application and Bluetooth Mesh Framework happens in here (e.g. Provisioning, Configuration, Controlling, Deletion, etc).
 */
@interface SBMBluetoothMesh : NSObject

@property (nonatomic, strong, nonnull) SBMBluetoothAdapter* bluetoothAdapter;

///-------------------------------
/// @name Initialize/Deinitialize
///-------------------------------

/**
 Initializes the Bluetooth Mesh Stack in the Bluetooth Mesh Library.
 */
- (void)initialise;

/**
 Deinitilizes the Bluetooth Mesh Stack in the Bluetooth Mesh Library.
 */
- (void)deinitialise;

///-------------------------------
/// @name Proxy/Provisioning
///-------------------------------

/**
 Attempt a connection to a proxy device. Bluetooth connection already needs to be open and subscription for needed characteristic must to be set.
 
 @param gattHandle Integer containing the handle of the Bluetooth connection.
 @param completion A block object to be executed when the task finishes. This block has no return value and takes two arguments: the gattHandle and the success describing if the connection has been established.
*/
- (void)connectToProxyWithGattHandle:(NSInteger)gattHandle completion:(nonnull void (^)(NSInteger gattHandle, BOOL success))completion;

/**
 Provision a device. Attempts to provision a specified device. Bluetooth connection already needs to be open.
 
 @param network Object containing the informations about mesh network. Device will be added to this network after successful provision.
 @param deviceName String containing a name of device.
 @param deviceUUID UUID of the device to be provisioned.
 @param gattHandle Integer containing the handle of the Bluetooth connection.
 @param completion A block object to be executed when task finishes. This block has no return value and takes two arguments: the node and the error. Node is a object containing informations about provisioned device.
 
 *  *  *  *  *
 
 * UUID of the device:
 
 
 ````
    a = object for "kCBAdvDataServiceData" key from advertisement data.
    b = data for CBUUID: 1827 key from the 'a'
    UUID = data from (0,16) range from the 'b'
 ````
 
 */
- (void)provisionDevice:(nonnull SBMMeshNetwork *)network deviceName:(nonnull NSString *)deviceName deviceUUID:(nonnull NSData *)deviceUUID gattHandle:(NSInteger)gattHandle completion:(nonnull void (^)(SBMMeshNode * _Nullable node, NSError * _Nullable error))completion;

///-------------------------------
/// @name Device composition data (DCD)
///-------------------------------

/**
 Get a node's device composition data (DCD). Send a request to a node to retrieve device composition data. Method works only if proxy device is provisioned and connected.
 
 @param node Object to which the request will be sent.
 @param completion A block object to be executed when task finishes. This block has no return value and takes two augments: the DCD and the error. DCD is a object containing informations about received device composition data.
 */
- (void)deviceCompositionDataForNode:(nonnull SBMMeshNode *)node completion:(nonnull void (^)(SBMDeviceCompositionData * _Nullable DCD, NSError * _Nullable error))completion;

///-------------------------------
/// @name Network control
///-------------------------------

/**
 Create a mesh network. From the Bluetooth Mesh specification it is known as create network key.
 
 @param name String containing a name of network.
 @param errorPtr Object contains informations about error.
 */
- (nullable SBMMeshNetwork *)createNetwork:(nonnull NSString *)name error:(NSError * _Nullable * _Nullable)errorPtr;

/**
 Return a created networks in the mesh.
 
 @return Array with networks.
 */
- (nullable NSArray<SBMMeshNetwork *> *)getNetworks;

/**
 Return a nodes from the network.
 
 @param network SBMMeshNetwork object. Nodes from this network will be returned.
 @return Array with nodes.
 */
- (nullable NSArray<SBMMeshNode *> *)getNodesWithNetwork:(nonnull SBMMeshNetwork *)network;

/**
 Remove a specific network from the mesh. Before this action it is required to factory reset all nodes from the network. Method works only if proxy device is provisioned and connected.
 
 @param network SBMeshNetwork object. Network to be removed.
 */
- (void)removeNetwork:(nonnull SBMMeshNetwork *)network;

/**
 Method that sets the name of the network.
 
 @param network SBMMeshNetwork object. Name will be changed for this object.
 @param name String with the network's name.
 */
- (void)changeNetwork:(nonnull SBMMeshNetwork *)network name:(nonnull NSString *)name;

///-------------------------------
/// @name Group control
///-------------------------------

/**
 Return a nodes from the group.
 
 @param group SBMMeshGroup object. Nodes from this group will be returned.
 @return Array with nodes.
 */
- (nonnull NSArray<SBMMeshNode *> *)getNodesWithGroup:(nonnull SBMMeshGroup *)group;

/**
 Method that sets the name of the group.
 
 @param group SBMMeshGroup object. Name will be changed for this object.
 @param name String with the group's name.
 */
- (void)changeGroup:(nonnull SBMMeshGroup *)group name:(nonnull NSString *)name;

/**
 Create group in the network. In Bluetooth Mesh specification it is known as creating application key.
 
 @param name String object. Name for group.
 @param network SBMMeshNetwork object. Create group in this network.
 @param errorPtr NSError.
 */
- (nullable SBMMeshGroup *)createGroupWithName:(nonnull NSString *)name inNetwork:(nonnull SBMMeshNetwork *)network error:(NSError * _Nullable * _Nullable)errorPtr;

/**
 Remove a group from the mesh. Before this action it is needed to remove all nodes from this group. Method works only if proxy device is provisioned and connected.
 
 @param group SBMMeshGroup object. Group to be removed.
 */
- (void)removeGroup:(nonnull SBMMeshGroup *)group;

/**
 Returns groups from network
 
 @param network SBMMeshNetwork object. Groups from this network will be returned.
 @return Array with groups.
 */
- (nullable NSArray<SBMMeshGroup *> *)getGroupsWithNetwork:(nonnull SBMMeshNetwork *)network;

/**
 Method used to configure a Device's model to a Group. It works only if proxy device is provisioned and connected.
 
 @param node SBMMeshNode object containing the device which contains the model to be configured.
 @param group SBMMeshGroup object containing the group. Device will be added to this group.
 @param completion A block object to be executed when task finishes. This block has no return value and takes one argument: the success property, describes if the device was successfully added to group.
 */
- (void)add:(nonnull SBMMeshNode *)node toGroup:(nonnull SBMMeshGroup *)group completion:(nonnull void (^)(BOOL success))completion;

/**
 Method used to remove a Device's model from a Group. Method works only if proxy device is provisioned and connected.
 
 @param node SBMMeshNode object containing the device which contains the model to be removed.
 @param group SBMMeshGroup object containing the group. Device will be removed from this group.
 @param completion A block object to be executed when task finishes. This block has no return value and takes one argument: the success property, describes if the device was successfully removed from the group.
 */
- (void)remove:(nonnull SBMMeshNode *)node fromGroup:(nonnull SBMMeshGroup *)group completion:(nonnull void (^)(BOOL success))completion;

///-------------------------------
/// @name Node control
///-------------------------------

/**
 Method that sets the name of the device.
 
 @param node SBMMeshNode object. Name will be changed for this object.
 @param name String with the device's name.
 */
- (void)change:(nonnull SBMMeshNode *)node name:(nonnull NSString *)name;

/**
 Method that sets the device's functionality, mainly used for the setting the device's control option in the UI.

 @param node SBMMeshNode object. Functionality will be set to this object.
 @param functionality SBMDeviceFunctionality object. This functionality will be set to the device.
 */
- (void)change:(nonnull SBMMeshNode *)node functionality:(nonnull SBMDeviceFunctionality *)functionality;

/**
 Method that send request for check device's proxy feature status. Method works only if proxy device is provisioned and connected.
 
 @param node SBMeshNode object. Request will be send to this object.
 @param completion A block object to be executed when task finishes. This Block has no return value and takes two arguments: the proxy parameter and the success parameter. Proxy parameter describes actual state of proxy feature. Success parameter describes if the status was get successfully.
 */
- (void)checkProxyForNode:(nonnull SBMMeshNode *)node completion:(nonnull void (^)(BOOL proxy, BOOL success))completion;

/**
 Method that send request for check device's relay feature status. Method works only if proxy device is provisioned and connected.
 
 @param node SBMeshNode object. Request will be send to this object.
 @param completion A block object to be executed when task finishes. This Block has no return value and takes two argument: the relay parameter and the success parameter. Relay parameter describes actual state of relay feature. Success parameter describes if the status was get successfully.
 */
- (void)checkRelayForNode:(nonnull SBMMeshNode *)node completion:(nonnull void (^)(BOOL relay, BOOL success))completion;

/**
 Method that send request for check device's friend feature status. Method works only if proxy device is provisioned and connected.
 
 @param node SBMeshNode object. Request will be send to this object.
 @param completion A block object to be executed when task finishes. This Block has no return value and takes two argument: the friend parameter and the success parameter. Friend parameter describes actual state of friend feature. Success parameter describes if the status was get successfully.
 */
- (void)checkFriendForNode:(nonnull SBMMeshNode *)node completion:(nonnull void (^)(BOOL isFriend, BOOL success))completion;

/**
 Sets proxy feature status. Method works only if proxy device is provisioned and connected.
 
 @param node SBMMeshNode object of the device which will have the feature status changed.
 @param enable Bool that represents the desired status of the feature.
 @param completion A Block object to be executed when task finishes. This block has no return value and takes one argument: the success parameter, describes if the feature was set successfully.
 */
- (void)setProxy:(nonnull SBMMeshNode *)node enable:(BOOL)enable completion:(nonnull void (^)(BOOL success))completion;

/**
 Sets relay feature status. Method works only if proxy device is provisioned and connected.
 
 @param node SBMMeshNode object of the device which will have the feature status changed.
 @param enable Bool that represents the desired status of the feature.
 @param completion A Block object to be executed when task finishes. This block has no return value and takes one argument: the success parameter, describes if the feature was set successfully.
 */
- (void)setRelay:(nonnull SBMMeshNode *)node enable:(BOOL)enable completion:(nonnull void (^)(BOOL success))completion;

/**
 Sets friend feature status. Method works only if proxy device is provisioned and connected.
 
 @param node SBMMeshNode object of the device which will have the feature status changed.
 @param enable Bool that represents the desired status of the feature.
 @param completion A Block object to be executed when task finishes. This block has no return value and takes one argument: the success parameter, describes if the feature was set successfully.
 */
- (void)setFriend:(nonnull SBMMeshNode *)node enable:(BOOL)enable completion:(nonnull void (^)(BOOL success))completion;

/**
 Removes the Device from the network and factory reset the device. Method works only if proxy device is provisioned and connected.
 
 @param node SBMMeshNode object of the device which will be deleted
 @param completion A Block object to be executed when task finishes. This block has no return value and takes one argument: the success parameter, describes if the device was removed successfully.
 */
- (void)factoryReset:(nonnull SBMMeshNode *)node completion:(nonnull void (^)(BOOL success))completion;

/**
 Device crypto key for device UUID.
 
 @param uuid Data object. Get device crypto key related to this UUID.
 @return Data with device crypto key.
 
 *  *  *  *  *
 
 * UUID of the device:
 
 
 ````
 a = object for "kCBAdvDataServiceData" key from advertisement data.
 b = data for CBUUID: 1827 key from the 'a'
 UUID = data from (0,16) range from the 'b'
 ````
 
 */
- (nullable NSData *)deviceKeyForUUID:(nonnull NSData *)uuid;

///-------------------------------
/// @name Node Functionality Control
///-------------------------------

/**
 Get Generic OnOff Server model state for configured Generic OnOff Server model from the node. Method works only if proxy device is provisioned and connected. Node must be added to group before this operation.
 
 @param node SBMMeshNode object. Value for model state from this node will be returned.
 @param group SBMMeshGroup object. Value for model state binded to this group will be returned.
 @param completion A Block object to be executed when task finishes. This block has no return value and takes one argument: the status parameter, describes state of Generic OnOff Server model: 
 ````
 1 for ON 
 0 for OFF
 ````
 
 */
- (void)getOnOff:(nonnull SBMMeshNode *)node fromGroup:(nonnull SBMMeshGroup *)group completion:(nonnull void (^)(UInt16 status))completion;

/**
 Get Generic Level Server model state for configured Generic Level Server model from the node. Method works only if proxy device is provisioned and connected. Node must be added to group before this operation.
 
 @param node SBMMeshNode object. Value for model state from this node will be returned.
 @param group SBMMeshGroup object. Value for model state binded to this group will be returned.
 @param completion A Block object to be executed when task finishes. This block has no return value and takes one argument: the status describing state of Generic Level Server model. Status is value with range from -32.767 to 32.767.
 
 */
- (void)getLevel:(nonnull SBMMeshNode *)node fromGroup:(nonnull SBMMeshGroup *)group completion:(nonnull void (^)(SInt16 status))completion;

/**
 Get Light Lightness Server model state for configured Light Lightness Server model from the node. Method works only if proxy device is provisioned and connected. Node must be added to group before this operation.
 
 @param node SBMMeshNode object. Value for model state from this node will be returned.
 @param group SBMMeshGroup object. Value for model state binded to this group will be returned.
 @param completion A Block object to be executed when task finishes. This block has no return value and takes one argument: the status describing state of Light Lightness Server model. Status is value with range: 0 to 65.535.
 
 */
- (void)getDimmable:(nonnull SBMMeshNode *)node fromGroup:(nonnull SBMMeshGroup *)group completion:(nonnull void (^)(UInt16 status))completion;

/**
 Set Generic OnOff Server model state for configured Generic OnOff Server model from the node. Method works only if proxy device is provisioned and connected. Node must be added to group before this operation.
 
 @param node SBMMeshNode object. Object of the device to be controlled.
 @param group SBMMeshGroup object. Value for model state binded to this group will changed.
 @param status UInt16 value. Integer containing the wished status. 1 if ON, 0 if is OFF.
 @param transitionMS UInt16 value. Integer containing the transition time in ms, or zero for immediate change request.
 @param delayMS UInt16 value. Integer containing delay between packets in ms.
 @param requestReply Bool value. Boolean that defines if the user would like to receive a status callback
 @param isFinal Bool value. Boolean that defines if the packet is the final one.
 */
- (void)setOnOff:(nullable SBMMeshNode *)node inGroup:(nonnull SBMMeshGroup *)group status:(UInt16)status transitionMS:(UInt32)transitionMS delayMS:(UInt16)delayMS requestReply:(BOOL)requestReply isFinal:(BOOL)isFinal;

/**
 Set Generic Level Server model state for configured Generic Level Server model from the node. Method works only if proxy device is provisioned and connected. Node must be added to group before this operation.
 
 @param node SBMMeshNode object. Object of the device to be controlled.
 @param group SBMMeshGroup object. Value for model state binded to this group will changed.
 @param status UInt16 value. Integer containing the wished status. Status is value with range: -32.767 to 32.767.
 @param transitionMS UInt16 value. Integer containing the transition time in ms, or zero for immediate change request.
 @param delayMS UInt16 value. Integer containing delay between packets in ms.
 @param requestReply Bool value. Boolean that defines if the user would like to receive a status callback
 @param isFinal Bool value. Boolean that defines if the packet is the final one.
 */
- (void)setLevel:(nullable SBMMeshNode *)node inGroup:(nonnull SBMMeshGroup*)group status:(SInt16)status transitionMS:(UInt32)transitionMS delayMS:(UInt16)delayMS requestReply:(BOOL)requestReply isFinal:(BOOL)isFinal;

/**
 Set Light Lightness Server model state for configured Light Lightness Server model from the node. Method works only if proxy device is provisioned and connected. Node must be added to group before this operation.
 
 @param node SBMMeshNode object. Object of the device to be controlled.
 @param group SBMMeshGroup object. Value for model state binded to this group will changed.
 @param status UInt16 value. Integer containing the wished status. Status is value with range: 0 to 65.535.
 @param transitionMS UInt16 value. Integer containing the transition time in ms, or zero for immediate change request.
 @param delayMS UInt16 value. Integer containing delay between packets in ms.
 @param requestReply Bool value. Boolean that defines if the user would like to receive a status callback
 @param isFinal Bool value. Boolean that defines if the packet is the final one.
 */
- (void)setDimmable:(nullable SBMMeshNode *)node inGroup:(nonnull SBMMeshGroup *)group status:(UInt16)status transitionMS:(UInt32)transitionMS delayMS:(UInt16)delayMS requestReply:(BOOL)requestReply isFinal:(BOOL)isFinal;


/**
 Get configured functionalities from the Node. Add node to group to configure it. Method works only if proxy device is provisioned and connected. Node must be added to group before this operation.
 
 @param node SBMMeshNode object. Functionalities from this node will be taken.
 @return Array of known SBMDeviceFunctionality objects.
*/
- (nonnull NSArray<SBMDeviceFunctionality *> *)configuredFunctionalitiesForNode:(nonnull SBMMeshNode *)node;

///-------------------------------
/// @name Blacklistig Feature
///-------------------------------

/**
 Add specific node to blacklist, effectively excluding it from key refresh procedure.
 
 @param node SBMMeshNode object of the device to be blacklisted. Blacklisted devices do not participate in key refresh procedure.
 */
- (void)blacklist:(nonnull SBMMeshNode *)node;

/**
 Start key refresh procedure, effectively refreshing keys for all nodes in given network. All nodes that should not take part in key refresh procedure should be blacklisted.
 
 @param network SBMMeshNetwork object of the network for which the key refresh procedure should be started.
 @param timeout Key refresh phase timeout in milliseconds. The node is considered to have failed the procedure if it does not respond for phase change request in time.
 @param completion Action that is invoked when the key refresh procedure is finished.
 */
- (void)startKeyRefresh:(nonnull SBMMeshNetwork *)network withTimeout:(UInt32)timeout completion:(nonnull void (^)(BOOL success))completion;

/**
 Suspend ongoing key refresh procedure. Key refresh should be suspended when mobile device leaves network to avoid spurious timeouts.
 
 @param network SBMMeshNetwork object of the network for which the key refresh procedure should be suspended.
 */
- (void)suspendKeyRefresh:(nonnull SBMMeshNetwork *)network;

/**
 Resume previously suspended key refresh procedure. Key refresh should be resumed when mobile device reenters network.
 
 @param network SBMMeshNetwork object of the network for which the key refresh procedure should be resumed.
 @param completion Action that is invoked when resumed key refresh procedure is finished.
 */
- (void)resumeKeyRefresh:(nonnull SBMMeshNetwork *)network completion:(nonnull void (^)(BOOL success))completion;

///-------------------------------
/// @name Network/Device Mesh Match
///-------------------------------

/**
 Use this method to check if Bluetooth device is a proxy one.
 
 @param networkIndex Integer value. Network index of the network the user would like to connect.
 @param data Data object. Advertisement data from the Bluetooth device.
 
  *  *  *  *  *
 
 * To get data from Bluetooth advertisement:
 
 
 ````
 serviceData = dictionaryFromAdvertisment[CBUUID(string:"1828"]
 serviceDataLength = serviceData.length + 3
 serviceDataType = 0x16
 meshProxyService = 0x1828
 data = Data()
 data.append(Data(bytes: &serviceDataLength, count: MemoryLayout.size(ofValue: serviceDataLength)))
 data.append(Data(bytes: &serviceDataType, count: MemoryLayout.size(ofValue: serviceDataType)))
 data.append(Data(bytes: &meshProxyService, count: MemoryLayout.size(ofValue: meshProxyService)))
 data.append(Data(referencing: serviceData))
 ````
 */
- (BOOL)networkHashMatches:(NSInteger)networkIndex advertisingData:(nonnull NSData *)data;

/**
 If a device was just provisioned, it has a 60 seconds window of GATT advertising waiting to be configured. To find this device the user can use following method.
 
 @param meshDeviceUUID Data object. DeviceUUID as data.
 @param data Data object. Advertisement data from the Bluetooth device.
 
 *  *  *  *  *
 
 * UUID of the device:
 
 
 ````
 a = object for "kCBAdvDataServiceData" key from advertisement data.
 b = data for CBUUID: 1827 key from the 'a'
 UUID = data from (0,16) range from the 'b'
 ````
 
 
 
 * To get data from Bluetooth advertisement:
 
 
 ````
 serviceData = dictionaryFromAdvertisment[CBUUID(string:"1828"]
 serviceDataLength = serviceData.length + 3
 serviceDataType = 0x16
 meshProxyService = 0x1828
 data = Data()
 data.append(Data(bytes: &serviceDataLength, count: MemoryLayout.size(ofValue: serviceDataLength)))
 data.append(Data(bytes: &serviceDataType, count: MemoryLayout.size(ofValue: serviceDataType)))
 data.append(Data(bytes: &meshProxyService, count: MemoryLayout.size(ofValue: meshProxyService)))
 data.append(Data(referencing: serviceData))
 ````
 */
- (BOOL)deviceIdentityMatches:(nonnull NSData *)meshDeviceUUID advertisingData:(nonnull NSData *)data;

@end
