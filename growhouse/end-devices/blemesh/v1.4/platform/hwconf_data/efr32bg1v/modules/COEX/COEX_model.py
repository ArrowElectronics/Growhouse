from . import halconfig_types as types
from . import halconfig_dependency as dep

name = "COEX"
displayname = "Coexistence"
description = "Coexistence between multiple radios. Radio hold-off, request, grant."
compatibility = dep.Dependency(mcu_type=dep.McuType.RADIO)  # = all
category = " Radio"
studio_module = {
    "basename" : "SDK.HAL.COEX",
    "modules" : [types.StudioFrameworkModule("BASE", [types.Framework.ZNET, types.Framework.THREAD, types.Framework.CONNECT]),
                 types.StudioFrameworkModule("BLE", types.Framework.BLE)],
    }
enable = {
    "define": "HAL_COEX_ENABLE",
    "description": "Enable radio coexistence",
}
options = {
    "BSP_COEX_REQ": {
        "type": types.Pin(),
        "description": "REQUEST signal",
        "subcategory": "REQUEST",
        "longdescription": "Pin used for REQUEST signal",
    },
    # depends on BSP_COEX_REQ
    "BSP_COEX_REQ_ASSERT_LEVEL": {
        "type": "enum",
        "description": "REQUEST assert signal level",
        "values": [types.EnumValue('1', 'High'),
                   types.EnumValue('0', 'Low')],
        "subcategory": "REQUEST",
        "longdescription": "Polarity of REQUEST signal",
    },
    # depends on BSP_COEX_REQ
    "HAL_COEX_REQ_SHARED": {
        "type": "boolean",
        "description": "Enable REQUEST shared mode",
        "defaultValue": "False",
        "subcategory": "REQUEST",
        "longdescription": "Configure the REQUEST signal for shared mode",
    },
    # depends on HAL_COEX_REQ_SHARED
    "HAL_COEX_REQ_BACKOFF":{
        "type": "uint8_t",
        "description": "Max REQUEST backoff mask [0-255]",
        "min": "0",
        "max": "255",
        "defaultValue": "15",
        "subcategory": "REQUEST",
        "longdescription": "Maximum backoff time in microseconds after REQUEST was deasserted",
    },
    # depends on BSP_COEX_REQ
    "HAL_COEX_REQ_WINDOW":{
        "type": "uint16_t",
        "description": "Assert time between REQUEST and RX/TX start (us) [BLE only]",
        "min": "0",
        "max": "5000",
        "defaultValue": "500",
        "subcategory": "REQUEST",
        "longdescription": "Specify the number of microseconds between asserting REQUEST and starting RX/TX (BLE only)",
    },
    # depends on BSP_COEX_REQ
    "HAL_COEX_RETRYRX_ENABLE": {
        "type": "boolean",
        "description": "Enable REQUEST receive retry",
        "defaultValue": "False",
        "subcategory": "REQUEST",
        "longdescription": "Enable the receive retry",
    },
    # depends on HAL_COEX_RETRYRX_ENABLE
    "HAL_COEX_RETRYRX_TIMEOUT": {
        "type": "uint8_t",
        "description": "REQUEST receive retry timeout(ms)",
        "min": "0",
        "max": "255",
        "defaultValue": "16",
        "subcategory": "REQUEST",
        "longdescription": "Receive retry REQ timeout in milliseconds",
    },
    # depends on BSP_COEX_PRI and HAL_COEX_RETRYRX_ENABLE
    "HAL_COEX_RETRYRX_HIPRI": {
        "type": "boolean",
        "description": "REQUEST receive retry assert PRIORITY",
        "defaultValue": "True",
        "subcategory": "REQUEST",
        "longdescription": "Enable the receive retry high priority",
    },
    "BSP_COEX_GNT": {
        "type": types.Pin(),
        "description": "GRANT signal",
        "subcategory": "GRANT",
        "longdescription": "Pin used for grant (GNT) signal",
    },
    "BSP_COEX_GNT_ASSERT_LEVEL": {
        "type": "enum",
        "description": "GRANT assert signal level",
        "values": [types.EnumValue('1', 'High'),
                   types.EnumValue('0', 'Low')],
        "subcategory": "GRANT",
        "longdescription": "Polarity of grant (GNT) signal",
    },
    "HAL_COEX_TX_ABORT": {
        "type": "boolean",
        "description": "Abort transmission mid-packet if GRANT is lost",
        "defaultValue": "False",
        "subcategory": "GRANT",
        "longdescription": "If grant signal is deasserted, local device aborts transmission",
    },
    "HAL_COEX_ACKHOLDOFF": {
        "type": "boolean",
        "description": "Disable ACKing when GRANT deasserted, RHO asserted, or REQUEST not secured (shared REQUEST only)",
        "defaultValue": "True",
        "subcategory": "GRANT",
        "longdescription": "Disable ACKing when GNT deasserted, RHO asserted, or REQ not secured (shared REQ only)",
    },
    "BSP_COEX_PRI": {
        "type": types.Pin(),
        "description": "PRIORITY signal",
        "subcategory": "PRIORITY",
        "longdescription": "Pin used for PRIORITY signal",
    },
    "BSP_COEX_PRI_ASSERT_LEVEL": {
        "type": "enum",
        "description": "PRIORITY assert signal level",
        "values": [types.EnumValue('1', 'High'),
                   types.EnumValue('0', 'Low')],
        "subcategory": "PRIORITY",
        "longdescription": "Polarity of PRIORITY signal",
    },
    # depends on BSP_COEX_PRI
    "HAL_COEX_PRI_SHARED": {
        "type": "boolean",
        "description": "Enable PRIORITY shared mode",
        "defaultValue": "False",
        "subcategory": "PRIORITY",
    },
    # depends on BSP_COEX_PRI
    "HAL_COEX_TX_HIPRI": {
        "type": "boolean",
        "description": "Assert PRIORITY when transmitting packet",
        "defaultValue": "True",
        "subcategory": "PRIORITY",
        "longdescription": "Assert a high priority when the local device is transmitting a packet",
    },
    # depends on BSP_COEX_PRI
    "HAL_COEX_RX_HIPRI": {
        "type": "boolean",
        "description": "Assert PRIORITY when receiving packet",
        "defaultValue": "True",
        "subcategory": "PRIORITY",
        "longdescription": "Assert a high priority when the local device is receiving a packet",
    },
    "HAL_COEX_PRIORITY_ESCALATION_ENABLE": {
        "type": "boolean",
        "description": "Include TX PRIORITY Escalation",
        "defaultValue": "True",
        "subcategory": "PRIORITY",
        "longdescription": "Compile-time include TX PRIORITY escalation feature",
    },
    "HAL_COEX_CCA_THRESHOLD":{
        "type": "uint8_t",
        "description": "CCA/GRANT TX PRIORITY Escalation Threshold",
        "min": "0",
        "max": "7",
        "defaultValue": "4",
        "subcategory": "PRIORITY",
        "longdescription": "Sets the threshold for escalating TX PRIORITY to high priority due to MAC failures from CCA/GRANT denials (five consecutive CCA/GRANT denials is one MAC failure)",
    },
    "HAL_COEX_MAC_FAIL_THRESHOLD":{
        "type": "uint8_t",
        "description": "MAC Fail TX PRIORITY Escalation threshold",
        "min": "0",
        "max": "3",
        "defaultValue": "0",
        "subcategory": "PRIORITY",
        "longdescription": "Sets the threshold for escalating TX PRIORITY to high priority due to MAC failures from CCA/GRANT denials (five consecutive CCA/GRANT denials is one MAC failure) or no RX_ACK received (four consecutive RX_ACK failures is one MAC failure)",
    },
    "BSP_COEX_PWM_REQ": {
        "type": types.Pin(),
        "description": "PWM REQUEST signal (shared REQUEST only)",
        "subcategory": "PWM",
        "longdescription": "Pin used for PWM REQUEST signal when shared REQUEST enabled",
    },
    "BSP_COEX_PWM_REQ_ASSERT_LEVEL": {
        "type": "enum",
        "description": "PWM REQUEST signal level (shared REQUEST only)",
        "values": [types.EnumValue('1', 'High'),
                   types.EnumValue('0', 'Low')],
        "subcategory": "PWM",
        "longdescription": "Polarity of PWM REQUEST signal when shared REQUEST enabled",
    },
    "HAL_COEX_PWM_DEFAULT_ENABLED": {
        "type": "boolean",
        "description": "Enable PWM REQUEST at startup",
        "defaultValue": "False",
        "subcategory": "PWM",
        "longdescription": "Enable PWM REQUEST at startup (also run-time controllable)",
    },
    "HAL_COEX_PWM_REQ_PERIOD": {
        "type": "uint8_t",
        "description": "PWM REQUEST Period (0.5ms)",
        "min": "5",
        "max": "109",
        "defaultValue": "78",
        "subcategory": "PWM",
        "longdescription": "PWM REQUEST Period (5ms to 109ms in 0.5ms steps)",
    },
    "HAL_COEX_PWM_REQ_DUTYCYCLE": {
        "type": "uint8_t",
        "description": "PWM REQUEST Duty-Cycle (%)",
        "min": "1",
        "max": "95",
        "defaultValue": "20",
        "subcategory": "PWM",
        "longdescription": "PWM REQUEST Duty-Cycle (1% to 95% in 1% steps)",
    },
    "HAL_COEX_PWM_PRIORITY": {
        "type": "enum",
        "description": "Assert PRIORITY when PWM REQUEST asserted",
        "values": [types.EnumValue('0', 'Low'),
                   types.EnumValue('1', 'High')],
        "subcategory": "PWM",
        "longdescription": "Assert a high priority when local device is asserting PWM",
    },
    "BSP_COEX_RHO": {
        "type": types.Pin(),
        "description": "RHO signal",
        "subcategory": "Radio Hold Off",
        "longdescription": "Pin used for RHO signal",
    },
    "BSP_COEX_RHO_ASSERT_LEVEL": {
        "type": "enum",
        "description": "RHO assert signal level",
        "values": [types.EnumValue('1', 'High'),
                   types.EnumValue('0', 'Low')],
        "subcategory": "Radio Hold Off",
        "longdescription": "Polarity of the RHO signal level",
    },
    "HAL_COEX_DP_ENABLED": {
        "type": "boolean",
        "description": "Enable Directional Priority",
        "defaultValue": "False",
        "subcategory": "Directional Priority",
        "longdescription": "Enable Directional Priority",
    },
    "HAL_COEX_DP_TIMER": {
        "type": types.Peripheral(filter=["TIMER", "WTIMER"],
                                 inherit_options=True,
                                 define_value_prefix="HAL_TIMER_",
                                 define_name_postfix="_DP"),
        "description": "Directional Priority Timer",
        "subcategory": "Directional Priority",
        "longdescription": "Directional Priority Timer module",
    },
    "HAL_COEX_DP_PULSE_WIDTH_US": {
        "type": "uint8_t",
        "description": "Microseconds to hold directional priority RX priority pulse",
        "min": 0,
        "max": 255,
        "defaultValue": 20,
        "subcategory": "Directional Priority",
        "longdescription": "Microseconds to hold directional priority RX priority pulse",
    },
    "BSP_COEX_DP": {
        "type": types.PRSChannelLocation("BSP_COEX_DP", custom_name="COEX_DP"),
        "description": "Directional Priority PRS channel",
        "subcategory": "Directional Priority",
        "longdescription": "Directional Priority PRS channel",
    },
    "BSP_COEX_DP_REQUEST_INV_CHANNEL": {
        "type": types.PRSChannelLocation("BSP_COEX_DP_REQUEST_INV", custom_name="COEX_DP_REQUEST_INV", gpio=False),
        "description": "Inverted REQUEST PRS channel",
        "subcategory" : "Directional Priority",
        "longdescription": "Inverted REQUEST PRS channel",
    },
    "BSP_COEX_DP_RACLNAEN_INV_CHANNEL": {
        "type": types.PRSChannelLocation("BSP_COEX_DP_RACLNAEN_INV", custom_name="COEX_DP_RACLNAEN_INV", gpio=False),
        "description": "Inverted RACLNAEN PRS channel",
        "subcategory" : "Directional Priority",
        "longdescription": "Inverted RACLNAEN PRS channel",
    },
    "HAL_COEX_PHY_ENABLED": {
        "type": "boolean",
        "description": "Enable radio configuration optimized for radio coexistence",
        "defaultValue": "False",
        "dependency": dep.Dependency(sdid=[84,89,95]),
        "subcategory": "Passive Configuration",
        "longdescription": "Enable radio configuration optimized for radio coexistence",
    },
}
