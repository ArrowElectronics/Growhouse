//
//  SBMBluetoothMeshMacros.h
//  BluetoothMesh
//
//  Created by Łukasz Rzepka on 20/03/2019.
//  Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#define BLUETOOTH_MESH_API_DEPRECATED(APIVersion) __attribute__((deprecated("Deprecated in API v." #APIVersion".")))
#define BLUETOOTH_MESH_API_DEPRECATED_USE_INSTEAD(APIVersion, msg) __attribute__((deprecated("Deprecated in API v." #APIVersion ". Please use " msg " instead.")))
