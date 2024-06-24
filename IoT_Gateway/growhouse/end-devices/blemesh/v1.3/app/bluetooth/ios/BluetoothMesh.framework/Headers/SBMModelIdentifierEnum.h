//
//  SBMModelIdentifier.h
//  ble_mesh-lib
//
//  Created by Lukasz Rzepka on 18.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

/**
 Models specified by Mesh Model Bluetooth® Specification
 https://www.bluetooth.com/specifications/mesh-specifications
 */

typedef NS_ENUM(NSInteger, SBMModelIdentifier) {
    SBMModelIdentifierConfigurationServer = 0x0000, /**< Mesh Model Configuration Server */
    SBMModelIdentifierConfigurationClient = 0x0001, /**< Mesh Model Configuration Client */
    SBMModelIdentifierHealthServer = 0x0002, /**< Mesh Model Health Server */
    SBMModelIdentifierHealthClient = 0x0003, /**< Mesh Model Health Client */
    
    ///-------------------------------
    /// @name Generic server/client models
    ///-------------------------------

    SBMModelIdentifierGenericOnOffServer = 0x1000, /**< Mesh Model Generic OnOff Server */
    SBMModelIdentifierGenericOnOffClient = 0x1001, /**< Mesh Model Generic OnOff Client */
    SBMModelIdentifierGenericLevelServer = 0x1002, /**< Mesh Model Generic Level Server */
    SBMModelIdentifierGenericLevelClient = 0x1003, /**< Mesh Model Generic Level Client */
    SBMModelIdentifierGenericDefaultTransitionTimeServer = 0x1004, /**< Mesh Model Generic Default Transition Time Server */
    SBMModelIdentifierGenericDefaultTransitionTimeClient = 0x1005, /**< Mesh Model Generic Default Transition Time Client */
    SBMModelIdentifierGenericPowerOnOffServer = 0x1006, /**< Mesh Model Generic Power OnOff Server */
    SBMModelIdentifierGenericPowerOnOffSetupServer = 0x1007, /**< Mesh Model Generic Power OnOff Setup Server */
    SBMModelIdentifierGenericPowerOnOffClient = 0x1008, /**< Mesh Model Generic Power OnOff Clientt */
    SBMModelIdentifierGenericPowerLevelServer = 0x1009, /**< Mesh Model Generic Power Level Server */
    SBMModelIdentifierGenericPowerLevelSetupServer = 0x100A, /**< Mesh Model Generic Power Level Setup Server */
    SBMModelIdentifierGenericPowerLevelClient = 0x100B, /**< Mesh Model Generic Power Level Client */
    SBMModelIdentifierGenericBatteryServer = 0x100C, /**< Mesh Model Generic Battery Server */
    SBMModelIdentifierGenericBatteryClient = 0x100D, /**< Mesh Model Generic Battery Client */
    SBMModelIdentifierGenericLocationServer = 0x100E, /**< Mesh Model Generic Location Server */
    SBMModelIdentifierGenericLocationSetupServer = 0x100F, /**< Mesh Model Generic Location Setup Server */
    SBMModelIdentifierGenericLocationClient = 0x1010, /**< Mesh Model Generic Location Client */
    SBMModelIdentifierGenericAdminPropertyServer = 0x1011, /**< Mesh Model Generic Admin Property Server*/
    SBMModelIdentifierGenericManufacturerPropertyServer = 0x1012, /**< Mesh Model Generic Manufacturer Property Server */
    SBMModelIdentifierGenericUserPropertyServer = 0x1013, /**< Mesh Model Generic User Property Server */
    SBMModelIdentifierGenericClientPropertyServer = 0x1014, /**< Mesh Model Generic Client Property Server */
    SBMModelIdentifierGenericPropertyClient = 0x1015, /**< Mesh Model Generic Property Client */
    
    ///-------------------------------
    /// @name Sensor server/client models
    ///-------------------------------
    
    SBMModelIdentifierSensorServer = 0x1100, /**< Mesh Model Sensor Server */
    SBMModelIdentifierSensorSetupServer = 0x1101, /**< Mesh Model Sensor Setup Server */
    SBMModelIdentifierSensorClient = 0x1102, /**< Mesh Model Sensor Client */
    
    ///-------------------------------
    /// @name Time server/client models
    ///-------------------------------
    
    SBMModelIdentifierTimeServer = 0x1200, /**< Mesh Model Time Server */
    SBMModelIdentifierTimeSetupServer = 0x1201, /**< Mesh Model Time Setup Server */
    SBMModelIdentifierTimeClient = 0x1202, /**< Mesh Model Time Clien */
    
    ///-------------------------------
    /// @name Scene server/client models
    ///-------------------------------
    
    SBMModelIdentifierSceneServer = 0x1203, /**< Mesh Model Scene Server */
    SBMModelIdentifierSceneSetupServer = 0x1204, /**< Mesh Model Scene Setup Server */
    SBMModelIdentifierSceneClient = 0x1205, /**< Mesh Model Scene Client */
    
    ///-------------------------------
    /// @name Scheduler server/client models
    ///-------------------------------
    
    SBMModelIdentifierSchedulerServer = 0x1206, /**< Mesh Model Scheduler Server */
    SBMModelIdentifierSchedulerSetupServer = 0x1207, /**< Mesh Model Scheduler Setup Server */
    SBMModelIdentifierSchedulerClient = 0x1208, /**< Mesh Model Scheduler Client */
    
    ///-------------------------------
    /// @name Light server/client models
    ///-------------------------------
    
    SBMModelIdentifierLightLightnessServer = 0x1300, /**< Mesh Model Light Lightness Server */
    SBMModelIdentifierLightLightnessSetupServer = 0x1301, /**< Mesh Model Light Lightness Setup Server */
    SBMModelIdentifierLightLightnessClient = 0x1302, /**< Mesh Model Light Lightness Client */
    SBMModelIdentifierLightCTLServer = 0x1303, /**< Mesh Model Light CTL Server */
    SBMModelIdentifierLightCTLSetupServer = 0x1304, /**< Mesh Model Light CTL Setup Server */
    SBMModelIdentifierLightCTLClient = 0x1305, /**< Mesh Model Light CTL Client */
    SBMModelIdentifierLightCTLTemperatureServer = 0x1306, /**< Mesh Model Light CTL Temperature Server */
    SBMModelIdentifierLightHSLServer = 0x1307, /**< Mesh Model Light HSL Server */
    SBMModelIdentifierLightHSLSetupServer = 0x1308, /**< Mesh Model HSL Setup Server */
    SBMModelIdentifierLightHSLClient = 0x1309, /**< Mesh Model Light HSL Client */
    SBMModelIdentifierLightHSLHueServer = 0x130A, /**< Mesh Model HSL Hue Server */
    SBMModelIdentifierLightHSLSaturationServer = 0x130B, /**< Mesh Model HSL Saturation Server */
    SBMModelIdentifierLightxyLServer = 0x130C, /**< Mesh Model Light xyL Server */
    SBMModelIdentifierLightxyLSetupServer = 0x130D, /**< Mesh Model Light xyL Setup Server */
    SBMModelIdentifierLightxyLClient = 0x130E, /**< Mesh Model Light xyL Client */
    SBMModelIdentifierLightLCServer = 0x130F, /**< Mesh Model Light LC Server */
    SBMModelIdentifierLightLCSetupServer = 0x1310, /**< Mesh Model Light LC Setup Server */
    SBMModelIdentifierLightLCClient = 0x1311 /**< Mesh Model Light LC Client */
};
