//
//  SBMGattServiceTypeEnum.h
//  BluetoothMesh
//
//  Created by Lukasz Rzepka on 18.06.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

/**
 Service type to connect.
 */

typedef NS_ENUM(NSUInteger, SBMGattServiceType) {
    SBMGattServiceTypeProvisioning, /**< Mesh provisioning service */
    SBMGattServiceTypeProxy /**< Mesh proxy service */
};
