from . import halconfig_types as types
from . import halconfig_dependency as dep

name = "ANTDIV"
displayname = "Antenna Diversity"
description = "Antenna Diversity"
compatibility = dep.Dependency(mcu_type=dep.McuType.RADIO)  # = all
category = " Radio"
studio_module = {
    "basename" : "SDK.HAL.ANTDIV",
    "modules" : [types.StudioFrameworkModule("BASE", [types.Framework.ZNET, types.Framework.THREAD, types.Framework.CONNECT])],
    }
enable = {
    "define": "HAL_ANTDIV_ENABLE",
    "description": "Enable antenna diversity. SEL signal must be defined, NSEL is optional.",
}
options = {
    "BSP_ANTDIV_SEL": {
        "type": types.Pin(),
        "description": "Antenna select",
        "longdescription": "Pin used to control external antenna switch",
    },
    "BSP_ANTDIV_NSEL": {
        "type": types.Pin(),
        "description": "Complementary antenna select",
        "longdescription": "Pin for inverted external antenna signal",
    },
}
