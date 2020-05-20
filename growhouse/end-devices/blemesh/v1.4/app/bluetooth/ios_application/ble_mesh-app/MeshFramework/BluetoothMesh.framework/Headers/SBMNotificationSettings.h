//
//  SBMNotificationSettings.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 21/11/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMNode.h"
#import "SBMGroup.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Possible types of notification settings.
 */
typedef NS_ENUM(NSUInteger, SBMNotificationSettingsKind) {
    /** Notification settings type is not set. */
    SBMNotificationSettingsKindUnset,
    /** Node notification settings. */
    SBMNotificationSettingsKindNode,
    /** Group notification settings. */
    SBMNotificationSettingsKindGroup,
    /** Virtual address notification settings. */
    SBMNotificationSettingsKindVirtualAddress,
    /** Local address notification settings. */
    SBMNotificationSettingsKindLocalAddress,
};

/**
 Object that represents notification settings.
 */
@interface SBMNotificationSettings : NSObject<NSCopying>

/**
 Value that specifies kind of notification settings.
 */
@property (nonatomic, readonly) SBMNotificationSettingsKind kind;

/**
 A node object used for notification.
 */
@property (nonatomic, strong, readonly) SBMNode* node;

/**
 A group object used for notification.
 */
@property (nonatomic, strong, readonly) SBMGroup* group;

/**
 A virtual address used for notification.
 */
@property (nonatomic, strong, readonly) NSUUID* virtualAddress;

/**
 Initialize notification settings object with specified node.

 @param node Node object used for notification.
 @return Initialized notification settings object, or nil if an object could not be created for some reason that would not result in an exception.
 */
- (instancetype)initWithNode:(SBMNode*)node;

/**
 Initialize notification settings object with specified group.

 @param group Group object used for notification.
 @return Initialized notification settings object, or nil if an object could not be created for some reason that would not result in an exception.
 */
- (instancetype)initWithGroup:(SBMGroup*)group;

/**
 Initialize notification settings object with specified virtual address.

 @param virtualAddress Virtual address used for notification.
 @return Initialized notification settings object, or nil if an object could not be created for some reason that would not result in an exception.
 */
- (instancetype)initWithVirtualAddress:(NSUUID*)virtualAddress;

/**
 Initialize notification settings object with local address.
 
 IMPORTANT: It is only possible to create object with SBMNotificationSettingsKindLocalAddress.
 In other cases initializator will return nil. To create object with other kind use:
 initWithNode - SBMNotificationSettingsKindNode
 initWithGroup - SBMNotificationSettingsKindGroup
 initWithVirtualAddress - SBMNotificationSettingsKindVirtualAddress
 init - SBMNotificationSettingsKindUnset
 
 @param kind SBMNotificationSettingsKind kind of notification settings.
 @return Initialized notification settings object, or nil if an object could not be created for some reason that would not result in an exception.
*/
- (nullable instancetype)initWithKind:(SBMNotificationSettingsKind)kind;


@end

NS_ASSUME_NONNULL_END
