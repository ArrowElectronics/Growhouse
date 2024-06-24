// *******************************************************************
// * green-power-server.c
// *
// *
// * Copyright 2014 by Silicon Laboratories. All rights reserved.           *80*
// *******************************************************************

#include "enums.h"
#include "app/framework/include/af.h"
#include "app/framework/util/common.h"
#include "app/framework/plugin/green-power-server/green-power-server.h"
#include "app/framework/plugin/green-power-common/green-power-common.h"
#include "stack/include/gp-types.h"

// declare forward.
#define GP_ADDR_SRC_ID_WILDCARD (0xFFFFFFFF)
#define DEFAULT_SRC_ID          { .id.sourceId = GP_ADDR_SRC_ID_WILDCARD,          \
                                  .applicationId = EMBER_GP_APPLICATION_SOURCE_ID, \
                                  .endpoint = 0xFF }

// Following GPDF to Zcl Command only applies for following ApplicationID
// EMBER_GP_APPLICATION_SOURCE_ID    (0b000)
// EMBER_GP_APPLICATION_IEEE_ADDRESS (0b010)
//
// NOTE: all mapped ZCL commands have frame control of
// ZCL_FRAME_CONTROL_CLIENT_TO_SERVER.

static EmberAfGreenPowerServerGpdCommandTranslation customizedTranslationTable[EMBER_AF_PLUGIN_GREEN_POWER_SERVER_CUSTOMIZED_GPD_TRANSLATION_TABLE_SIZE] =  { 0 };

static const EmberAfGreenPowerServerGpdCommandTranslation defaultTranslationTable[] =
{
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_IDENTIFY, HA_PROFILE_ID, ZCL_IDENTIFY_CLUSTER_ID, ZCL_IDENTIFY_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x02, 0x00, 0x3C } },

  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_RECALL_SCENE0, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, ZCL_RECALL_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x00 } }, // GroupId/SceneId = 0
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_RECALL_SCENE1, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, ZCL_RECALL_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x01 } }, // GroupId/SceneId = 1
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_RECALL_SCENE2, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, ZCL_RECALL_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x02 } }, // GroupId/SceneId = 1
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_RECALL_SCENE3, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, ZCL_RECALL_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x03 } }, // GroupId/SceneId = 1
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_RECALL_SCENE4, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, ZCL_RECALL_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x04 } }, // GroupId/SceneId = 1
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_RECALL_SCENE5, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, ZCL_RECALL_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x05 } }, // GroupId/SceneId = 1
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_RECALL_SCENE6, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, ZCL_RECALL_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x06 } }, // GroupId/SceneId = 1
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_RECALL_SCENE7, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, ZCL_RECALL_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x07 } }, // GroupId/SceneId = 1
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_STORE_SCENE0, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, ZCL_STORE_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0x00, 0x00, 0x00 } }, // GroupId/SceneId = 0
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_STORE_SCENE1, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, ZCL_STORE_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0x00, 0x01, 0x01 } }, // GroupId/SceneId = 0
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_STORE_SCENE2, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, ZCL_STORE_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0x00, 0x02, 0x02 } }, // GroupId/SceneId = 0
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_STORE_SCENE3, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, ZCL_STORE_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0x00, 0x03, 0x03 } }, // GroupId/SceneId = 0
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_STORE_SCENE4, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, ZCL_STORE_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0x00, 0x04, 0x04 } }, // GroupId/SceneId = 0
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_STORE_SCENE5, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, ZCL_STORE_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0x00, 0x05, 0x05 } }, // GroupId/SceneId = 0
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_STORE_SCENE6, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, ZCL_STORE_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0x00, 0x06, 0x06 } }, // GroupId/SceneId = 0
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_STORE_SCENE7, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, ZCL_STORE_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0x00, 0x07, 0x07 } }, // GroupId/SceneId = 0
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_OFF, HA_PROFILE_ID, ZCL_ON_OFF_CLUSTER_ID, ZCL_OFF_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },                  // GroupId/SceneId = 0
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_ON, HA_PROFILE_ID, ZCL_ON_OFF_CLUSTER_ID, ZCL_ON_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },                    // GroupId/SceneId = 0
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_TOGGLE, HA_PROFILE_ID, ZCL_ON_OFF_CLUSTER_ID, ZCL_TOGGLE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },            // GroupId/SceneId = 0
  /* {EMBER_ZCL_GP_GPDF_RELEASE } -- NO corresponding ZCL comman. */

  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_MOVE_UP, HA_PROFILE_ID, ZCL_LEVEL_CONTROL_CLUSTER_ID, ZCL_MOVE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_MOVE_MODE_UP } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_MOVE_DOWN, HA_PROFILE_ID, ZCL_LEVEL_CONTROL_CLUSTER_ID, ZCL_MOVE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_MOVE_MODE_DOWN } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_STEP_UP, HA_PROFILE_ID, ZCL_LEVEL_CONTROL_CLUSTER_ID, ZCL_STEP_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_STEP_MODE_UP } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_STEP_DOWN, HA_PROFILE_ID, ZCL_LEVEL_CONTROL_CLUSTER_ID, ZCL_STEP_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_STEP_MODE_DOWN } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_LEVEL_CONTROL_STOP, HA_PROFILE_ID, ZCL_LEVEL_CONTROL_CLUSTER_ID, ZCL_STOP_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_MOVE_UP_WITH_ON_OFF, HA_PROFILE_ID, ZCL_LEVEL_CONTROL_CLUSTER_ID, ZCL_MOVE_WITH_ON_OFF_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_MOVE_MODE_UP } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_MOVE_DOWN_WITH_ON_OFF, HA_PROFILE_ID, ZCL_LEVEL_CONTROL_CLUSTER_ID, ZCL_MOVE_WITH_ON_OFF_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_MOVE_MODE_DOWN } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_STEP_UP_WITH_ON_OFF, HA_PROFILE_ID, ZCL_LEVEL_CONTROL_CLUSTER_ID, ZCL_STEP_WITH_ON_OFF_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_STEP_MODE_UP } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_STEP_DOWN_WITH_ON_OFF, HA_PROFILE_ID, ZCL_LEVEL_CONTROL_CLUSTER_ID, ZCL_STEP_WITH_ON_OFF_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_STEP_MODE_DOWN } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_MOVE_HUE_STOP, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_MOVE_HUE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x02, EMBER_ZCL_HUE_MOVE_MODE_STOP, 0xFF } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_MOVE_HUE_UP, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_MOVE_HUE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_HUE_MOVE_MODE_UP } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_MOVE_HUE_DOWN, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_MOVE_HUE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_HUE_MOVE_MODE_DOWN } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_STEP_HUE_UP, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_STEP_HUE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_HUE_STEP_MODE_UP } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_STEP_HUE_DOWN, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_STEP_HUE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_HUE_STEP_MODE_DOWN } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_MOVE_SATURATION_STOP, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_MOVE_SATURATION_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x01, EMBER_ZCL_SATURATION_MOVE_MODE_STOP } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_MOVE_SATURATION_UP, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_MOVE_SATURATION_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_SATURATION_MOVE_MODE_UP } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_MOVE_SATURATION_DOWN, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_MOVE_SATURATION_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_SATURATION_MOVE_MODE_DOWN } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_STEP_SATURATION_UP, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_STEP_SATURATION_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_SATURATION_STEP_MODE_UP } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_STEP_SATURATION_DOWN, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_STEP_SATURATION_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_SATURATION_STEP_MODE_DOWN } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_MOVE_COLOR, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_STEP_SATURATION_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0 } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_STEP_COLOR, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, ZCL_STEP_COLOR_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0 } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_LOCK_DOOR, HA_PROFILE_ID, ZCL_DOOR_LOCK_CLUSTER_ID, ZCL_LOCK_DOOR_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_UNLOCK_DOOR, HA_PROFILE_ID, ZCL_DOOR_LOCK_CLUSTER_ID, ZCL_UNLOCK_DOOR_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },

  /*{ EMBER_ZCL_GP_GPDF_PRESS1_OF1,}       -- NO corresponding ZCL comman. */
  /*{ EMBER_ZCL_GP_GPDF_RELEASE1_OF1, }    -- NO corresponding ZCL comman. */
  /*{ EMBER_ZCL_GP_GPDF_PRESS1_OF2,}       -- NO corresponding ZCL comman. */
  /*{ EMBER_ZCL_GP_GPDF_RELEASE1_OF2,}     -- NO corresponding ZCL comman. */
  /*{ EMBER_ZCL_GP_GPDF_PRESS2_OF2,}       -- NO corresponding ZCL comman. */
  /*{ EMBER_ZCL_GP_GPDF_RELEASE2_OF2,}     -- NO corresponding ZCL comman. */
  /*{ EMBER_ZCL_GP_GPDF_SHORT_PRESS1_OF1,} -- NO corresponding ZCL comman. */
  /*{ EMBER_ZCL_GP_GPDF_SHORT_PRESS1_OF2,} -- NO corresponding ZCL comman. */
  /*{ EMBER_ZCL_GP_GPDF_SHORT_PRESS2_OF2,} -- NO corresponding ZCL comman. */

  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_UNLOCK_DOOR, HA_PROFILE_ID, ZCL_DOOR_LOCK_CLUSTER_ID, ZCL_UNLOCK_DOOR_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_ATTRIBUTE_REPORTING, HA_PROFILE_ID, 0xFFFF, ZCL_REPORT_ATTRIBUTES_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0 } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_MFR_SP_ATTR_RPTG, HA_PROFILE_ID, 0xFFFF, ZCL_REPORT_ATTRIBUTES_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0 } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_MULTI_CLUSTER_RPTG, HA_PROFILE_ID, 0xFFFF, ZCL_REPORT_ATTRIBUTES_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0 } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_MFR_SP_MULTI_CLUSTER_RPTG, HA_PROFILE_ID, 0xFFFF, ZCL_REPORT_ATTRIBUTES_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0 } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_REQUEST_ATTRIBUTE, HA_PROFILE_ID, 0xFFFF, ZCL_READ_ATTRIBUTES_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0 } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_READ_ATTR_RESPONSE, HA_PROFILE_ID, 0xFFFF, ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0 } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_ZCL_TUNNELING_WITH_PAYLOAD, HA_PROFILE_ID, 0xFFFF, ZCL_REPORT_ATTRIBUTES_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0 } },
  { true, EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION, DEFAULT_SRC_ID, 0xFF, EMBER_ZCL_GP_GPDF_ANY_GPD_SENSOR_CMD, HA_PROFILE_ID, 0xFFFF, ZCL_REPORT_ATTRIBUTES_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0 } },

  /*{EMBER_ZCL_GP_GPDF_MFR_DEF_GPD_CMD0}     -- NO corresponding ZCL comman.*/
  /*{EMBER_ZCL_GP_GPDF_MFR_DEF_GPD_CMD1}     -- NO corresponding ZCL comman.*/
  /*{EMBER_ZCL_GP_GPDF_MFR_DEF_GPD_CMD2}     -- NO corresponding ZCL comman.*/
  /*{EMBER_ZCL_GP_GPDF_MFR_DEF_GPD_CMD3}     -- NO corresponding ZCL comman.*/
  /*{EMBER_ZCL_GP_GPDF_MFR_DEF_GPD_CMD4}     -- NO corresponding ZCL comman.*/
  /*{EMBER_ZCL_GP_GPDF_MFR_DEF_GPD_CMD5}     -- NO corresponding ZCL comman.*/
  /*{EMBER_ZCL_GP_GPDF_MFR_DEF_GPD_CMD6}     -- NO corresponding ZCL comman.*/
  /*{EMBER_ZCL_GP_GPDF_MFR_DEF_GPD_CMD7}     -- NO corresponding ZCL comman.*/
  /*{EMBER_ZCL_GP_GPDF_MFR_DEF_GPD_CMD8}     -- NO corresponding ZCL comman.*/
  /*{EMBER_ZCL_GP_GPDF_MFR_DEF_GPD_CMD9}     -- NO corresponding ZCL comman.*/
  /*{EMBER_ZCL_GP_GPDF_MFR_DEF_GPD_CMD_A}    -- NO corresponding ZCL comman.*/
  /*{EMBER_ZCL_GP_GPDF_MFR_DEF_GPD_CMD_B}    -- NO corresponding ZCL comman.*/
  /*{EMBER_ZCL_GP_GPDF_MFR_DEF_GPD_CMD_C}    -- NO corresponding ZCL comman.*/
  /*{EMBER_ZCL_GP_GPDF_MFR_DEF_GPD_CMD_D}    -- NO corresponding ZCL comman.*/
  /*{EMBER_ZCL_GP_GPDF_MFR_DEF_GPD_CMD_E}    -- NO corresponding ZCL comman.*/
  /*{EMBER_ZCL_GP_GPDF_MFR_DEF_GPD_CMD_F}    -- NO corresponding ZCL comman.*/
  /*{EMBER_ZCL_GP_GPDF_DECOMMISSIONING}      -- NO corresponding ZCL comman.*/
  /*{EMBER_ZCL_GP_GPDF_SUCCESS}              -- NO corresponding ZCL comman.*/
  /*EMBER_ZCL_GP_GPDF_COMMISSIONING          -- NO corresponding ZCL comman.*/
  /*EMBER_ZCL_GP_GPDF_DECOMMISSIONING        -- NO corresponding ZCL comman.*/
  /*EMBER_ZCL_GP_GPDF_SUCCESS                -- NO corresponding ZCL comman.*/
  /*EMBER_ZCL_GP_GPDF_CHANNEL_REQUEST        -- NO corresponding ZCL comman.*/
  /*EMBER_ZCL_GP_GPDF_COMMISSIONING_REPLY    -- NO corresponding ZCL comman.*/
  /*EMBER_ZCL_GP_GPDF_WRITE_ATTRIBUTES       -- NO corresponding ZCL comman.*/
  /*EMBER_ZCL_GP_GPDF_READ_ATTRIBUTES        -- NO corresponding ZCL comman.*/
  /*EMBER_ZCL_GP_GPDF_CHANNEL_CONFIGURATION  -- NO corresponding ZCL comman.*/
  /*EMBER_ZCL_GP_GPDF_ZCL_TUNNELING          -- NO corresponding ZCL comman.*/
};

static EmberAfGreenPowerServerCommissioningState commissioningState;

static const EmberAfGreenPowerServerGpdCommandTranslation * findMatchingDeviceInTranslationTable(EmberGpAddress * gpAddr,
                                                                                                 uint8_t gpdCommandId,
                                                                                                 const EmberAfGreenPowerServerGpdCommandTranslation * table,
                                                                                                 uint16_t tableSize)
{
  uint16_t index;
  if (gpAddr == NULL
      || table == NULL
      || tableSize == 0) {
    return NULL;
  }

  for (index = 0; index < tableSize; index++) {
    if (!table[index].validEntry) {
      continue;
    }

    if (table[index].gpdCommand == gpdCommandId
        && (((table[index].gpAddr.applicationId == EMBER_GP_APPLICATION_SOURCE_ID
              && table[index].gpAddr.id.sourceId == GP_ADDR_SRC_ID_WILDCARD))
            || emberAfGreenPowerCommonGpAddrCompare(&table[index].gpAddr, gpAddr) == 0)) {
      return &table[index];
    }
  }

  return NULL;
}

const EmberAfGreenPowerServerGpdCommandTranslation * emberAfGreenPowerServerFindGpdfMapping(EmberGpAddress * gpAddr,
                                                                                            uint8_t gpdCommandId)
{
  const EmberAfGreenPowerServerGpdCommandTranslation  * entry = NULL;

  entry = findMatchingDeviceInTranslationTable(gpAddr,
                                               gpdCommandId,
                                               &customizedTranslationTable[0],
                                               sizeof(customizedTranslationTable) / sizeof(customizedTranslationTable[0]));

  if (entry == NULL) {
    entry = findMatchingDeviceInTranslationTable(gpAddr,
                                                 gpdCommandId,
                                                 &defaultTranslationTable[0],
                                                 sizeof(defaultTranslationTable) / sizeof(defaultTranslationTable[0]));
  }
  return entry;
}

EmberStatus emberAfGreenPowerClusterForwardGpdfCommand(EmberGpAddress *addr,
                                                       uint8_t gpdCommandId,
                                                       uint8_t const * gpdCommandPayload)
{
  EmberStatus retVal = EMBER_ERR_FATAL;
  const EmberAfGreenPowerServerGpdCommandTranslation * mapping =
    emberAfGreenPowerServerFindGpdfMapping(addr, gpdCommandId);

  if (mapping == NULL) {
    emberAfGreenPowerClusterPrintln("ERR: Unable to find translation entry for gpdCommand(%d)",
                                    gpdCommandId);
    goto kickout;
  }

  //A.4.2.7 Scene commands
  if (mapping->zigbeeCommandId == ZCL_RECALL_SCENE_COMMAND_ID) {
    uint16_t groupId = *((uint16_t *) &mapping->zclPayloadDefault[1]);
    uint8_t sceneId = (uint8_t) mapping->zclPayloadDefault[3];
    if (groupId == EMBER_AF_GREEN_POWER_SERVER_GPDF_SCENE_DERIVED_GROUP_ID) {
      // derive groupId from the GPD ID
      groupId = emGpdAlias(addr);
    }
    emberAfFillCommandScenesClusterRecallScene(groupId, sceneId);
    goto configEndpoint;
  } else if (mapping->zigbeeCommandId == ZCL_STORE_SCENE_COMMAND_ID) {
    char groupName[] = "BEEF";
    uint16_t groupId = *((uint16_t *) &mapping->zclPayloadDefault[1]);
    uint8_t sceneId = (uint8_t) mapping->zclPayloadDefault[3];
    if (groupId == EMBER_AF_GREEN_POWER_SERVER_GPDF_SCENE_DERIVED_GROUP_ID) {
      // derive groupId from the GPD ID
      groupId = emGpdAlias(addr);
    }

    //TODO: groupName? Spec didn't clarify.
#ifdef ZCL_USING_GROUPS_CLUSTER_SERVER
    emberAfFillCommandGroupsClusterAddGroup(groupId, groupName);
    retVal = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, emberGetNodeId());
    if (retVal != EMBER_SUCCESS) {
      emberAfGreenPowerClusterPrintln("Failed to AddGroup(0x%2X) to endpoint(%d)",
                                      groupId,
                                      EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT);
    }
#endif
    emberAfFillCommandScenesClusterStoreScene(groupId, sceneId);
    goto configEndpoint;
  }

  // General command filling.
  emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND           \
                             | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER), \
                            mapping->zigbeeCluster,                 \
                            mapping->zigbeeCommandId,               \
                            "");

  if (mapping->payloadSrc & EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED) {
    emberAfAppendToExternalBuffer(&mapping->zclPayloadDefault[1],
                                  mapping->zclPayloadDefault[0]);
  }

  if (mapping->payloadSrc & EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD) {
    emberAfAppendToExternalBuffer(&gpdCommandPayload[1],
                                  gpdCommandPayload[0]);
  }

  configEndpoint:    if (mapping->endpoint == 0xFF) {
    emberAfSetCommandEndpoints(EMBER_AF_PLUGIN_GREEN_POWER_SERVER_HIDDEN_PROXY_ZCL_MESSAGE_SRC_ENDPOINT,
                               EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT);
  } else {
    emberAfSetCommandEndpoints(EMBER_AF_PLUGIN_GREEN_POWER_SERVER_HIDDEN_PROXY_ZCL_MESSAGE_SRC_ENDPOINT,
                               mapping->endpoint);
  }

  retVal = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, emberAfGetNodeId());

  kickout:
  if (retVal != EMBER_SUCCESS) {
    emberAfGreenPowerClusterPrintln("Unable to forward Green Power message to ZCL endpoint(%d) with error (0x%X)",
                                    EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT,
                                    retVal);
  }
  return retVal;
}

bool emberAfGreenPowerClusterGpNotificationCallback(uint16_t options,
                                                    uint32_t gpdSrcId,
                                                    uint8_t* gpdIeee,
                                                    uint8_t  gpdEndpoint,
                                                    uint32_t gpdSecurityFrameCounter,
                                                    uint8_t  gpdCommandId,
                                                    uint8_t* gpdCommandPayload,
                                                    uint16_t gppShortAddress,
                                                    uint8_t  gppDistance)
{
  emberAfGreenPowerClusterPrintln("received GPD command %d", gpdCommandId);
  emberAfGreenPowerClusterPrint("received GPD command payload: ");
  emberAfGreenPowerClusterPrintBuffer(&gpdCommandPayload[1],
                                      gpdCommandPayload[0],
                                      true);
  emberAfGreenPowerClusterPrintln("");

  EmberGpAddress gpAddr;
  gpAddr.applicationId = options & EMBER_AF_GP_NOTIFICATION_OPTION_APPLICATION_ID;
  gpAddr.id.sourceId = gpdSrcId;
  if (gpdIeee != NULL) {
    MEMCOPY(&gpAddr.id.gpdIeeeAddress, gpdIeee, EUI64_SIZE);
  }
  gpAddr.endpoint = gpdEndpoint;

  // translation / fwd msg
  emberAfGreenPowerClusterForwardGpdfCommand(&gpAddr,
                                             gpdCommandId,
                                             gpdCommandPayload);

  return true;
}

bool emDecryptKey(uint8_t appId,
                  uint32_t gpdSrcId,
                  uint8_t *gpdIeee,
                  const EmberKeyData* linkKey,
                  uint8_t mic[4],
                  EmberKeyData* key);

/** @brief Green Power Cluster Gp Commissioning Notification
 *
 *
 *
 * @param options   Ver.: since gp-1.0-09-5499-24
 * @param gpdSrcId   Ver.: since gp-1.0-09-5499-24
 * @param gpdIeee   Ver.: since gp-1.0-09-5499-24
 * @param gpdSecurityFrameCounter   Ver.: since gp-1.0-09-5499-24
 * @param gpdCommandId   Ver.: since gp-1.0-09-5499-24
 * @param gpdCommandPayload   Ver.: since gp-1.0-09-5499-24
 * @param gppShortAddress   Ver.: since gp-1.0-09-5499-24
 * @param gppLink   Ver.: since gp-1.0-09-5499-24
 * @param mic   Ver.: since gp-1.0-09-5499-24
 */
bool emberAfGreenPowerClusterGpCommissioningNotificationCallback(uint16_t commNotificationOptions, //XXX look at this
                                                                 uint32_t gpdSrcId,
                                                                 uint8_t* gpdIeee,
                                                                 uint8_t endpoint,
                                                                 uint32_t gpdSecurityFrameCounter,
                                                                 uint8_t gpdCommandId,
                                                                 uint8_t* gpdCommandPayload,
                                                                 uint16_t gppShortAddress,
                                                                 uint8_t gppLink,
                                                                 uint32_t commissioningNotificationMic)
{
  static uint8_t lastKey[16];
  emberAfCorePrint("Gp CN Callback: gppShortID %2x", gppShortAddress);
  if (gpdCommandId == EMBER_ZCL_GP_GPDF_COMMISSIONING) {
    emberAfCorePrint("comm gdpf\n");
    uint8_t index = 1;
    uint8_t appInfo;
    uint8_t numberOfGpdCommands;
    uint8_t numClientClusters, numServerClusters;
    uint32_t outgoingFrameCounter;
    uint32_t keyMic;
    EmberGpApplicationInfo appInfoStruct;
    appInfoStruct.deviceId = gpdCommandPayload[index++];
    uint8_t gpdfOptions = gpdCommandPayload[index++];
    uint8_t gpdfExtendedOptions;
    uint8_t securityLevel;
    uint8_t securityKeyType;
    EmberKeyData key;
    EmberKeyData linkKey = { {
                               0x5A,
                               0x69,
                               0x67,
                               0x42,
                               0x65,
                               0x65,
                               0x41,
                               0x6C,
                               0x6C,
                               0x69,
                               0x61,
                               0x6E,
                               0x63,
                               0x65,
                               0x30,
                               0x39
                             } };
    uint8_t commReplyOptions = 0; //We don't need this if we don't send a reply, but it's based on things we find parsing the gpdf, so fill it in as we go

    // drop frame if not in commissioning mode
    if (!commissioningState.inCommissioningMode) {
      goto kickout;
    }
    //TODO deal with auto-commissioning frames

    if (gpdfOptions
        & EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_EXTENDED_OPTIONS_FIELD) {
      gpdfExtendedOptions = (gpdCommandPayload[index++]);
    } else {
      gpdfExtendedOptions = 0;
      // TODO: defaults
    }

    securityLevel = gpdfExtendedOptions & EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_SECURITY_LEVEL_CAPABILITIES; //XXX change reserved to security level
    securityKeyType = (gpdfExtendedOptions & EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_KEY_TYPE) >> EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_KEY_TYPE_OFFSET;

    if (gpdfExtendedOptions
        & EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_GPD_KEY_PRESENT) {
      MEMMOVE(key.contents, gpdCommandPayload + index,
              EMBER_ENCRYPTION_KEY_SIZE);
      index += EMBER_ENCRYPTION_KEY_SIZE;
    }

    if (gpdfExtendedOptions
        & EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_GPD_KEY_ENCRYPTION) {
      uint8_t mic[4];

      keyMic = emberFetchLowHighInt32u(gpdCommandPayload + index);
      index += 4;
      emDecryptKey(commNotificationOptions & 0x03, gpdSrcId, gpdIeee, &linkKey, mic, &key);
      MEMCOPY(lastKey, key.contents, 16);
      emberAfGreenPowerClusterPrintln("\nrx MIC %4x", keyMic);
      emberAfGreenPowerClusterPrintln("\ncalced MIC");
      int i;
      for (i = 0; i < 4; i++) {
        emberAfGreenPowerClusterPrintln("%x", mic[i]);
      }
      ;
      for (i = 0; i < EMBER_ENCRYPTION_KEY_SIZE; i++) {
        emberAfCorePrint("%x", key.contents[i]);
      }
    }

    if (gpdfExtendedOptions
        & EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_GPD_OUTGOING_COUNTER_PRESENT) {
      outgoingFrameCounter = emberFetchLowHighInt32u(gpdCommandPayload + index);
      index += 4;
    }

    if (gpdfOptions
        & EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_APPLICATION_INFORMATION_PRESENT) {
      appInfo = gpdCommandPayload[index++];
    } else {
      appInfo = 0;
    }

    if (appInfo & EMBER_AF_GP_APPLICATION_INFORMATION_MANUFACTURE_ID_PRESENT) {
      appInfoStruct.manufacturerId = emberFetchLowHighInt16u(gpdCommandPayload + index);
      index += 2;
    }
    if (appInfo & EMBER_AF_GP_APPLICATION_INFORMATION_MODEL_ID_PRESENT) {
      appInfoStruct.modelId = emberFetchLowHighInt16u(gpdCommandPayload + index);
      index += 2;
    }
    if (appInfo & EMBER_AF_GP_APPLICATION_INFORMATION_GPD_COMMANDS_PRESENT) {
      uint8_t i;
      numberOfGpdCommands = gpdCommandPayload[index++];
      for (i = 0; i < numberOfGpdCommands; i++) {
        appInfoStruct.gpdCommands[i] = gpdCommandPayload[index];
        //TODO do something with GPD commands
        index++;
      }
    }
    if (appInfo & EMBER_AF_GP_APPLICATION_INFORMATION_CLUSTER_LIST_PRESENT) {
      uint8_t i;
      uint8_t numClusters = gpdCommandPayload[index++];
      numClientClusters = numClusters >> 4;
      numServerClusters = numClusters & 0x0F;
      for (i = 0; i < numServerClusters; i++) {
        //TODO do something with this
        appInfoStruct.serverClusters[i] = emberFetchLowHighInt16u(gpdCommandPayload + index);
        index += 2;
      }
      for (i = 0; i < numClientClusters; i++) {
        //TODO do something with this
        appInfoStruct.clientClusters[i] = emberFetchLowHighInt16u(gpdCommandPayload + index);
        index += 2;
      }
    }

    if (index > gpdCommandPayload[0] + 1) {
      //we ran off the end of the payload
      goto kickout;
    }
    //we've parsed everything out of the gpdf, now decide what to do
    if (gpdfOptions & EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_RX_ON_CAP) {
      //this is an RX capable gpd, so we send back a commissioning reply GPDF
      uint8_t commReplyPayload[30];
      EmberApsFrame *apsFrame;
      EmberStatus retval;

      //commReplyOptions |= 0x10; //security level 2
      commReplyOptions |= securityLevel << EMBER_AF_GP_GPD_COMMISSIONING_REPLY_OPTIONS_SECURITY_LEVEL_OFFSET;

      if (gpdfOptions & EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_PAN_ID_REQUEST) {
        commReplyOptions |= EMBER_AF_GP_GPD_COMMISSIONING_REPLY_OPTIONS_PAN_ID_PRESENT;
      }
      if (gpdfOptions & EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_GP_SECURITY_KEY_REQUEST) {
        //TODO: add the rest of the cases
        //  commReplyOptions |= EMBER_AF_GP_GPD_COMMISSIONING_REPLY_OPTIONS_GPD_SECURITY_KEY_PRESENT;
        //XXX figure out why the SVE logs don't include the key when it's requested

        switch ((gpdfExtendedOptions & EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_KEY_TYPE) >> EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_KEY_TYPE_OFFSET) {
          case EMBER_ZCL_GP_SECURITY_KEY_TYPE_NONE:
            goto kickout;
          case EMBER_ZCL_GP_SECURITY_KEY_TYPE_INDIVIDIGUAL_GPD_KEY:
            commReplyOptions |= (EMBER_ZCL_GP_SECURITY_KEY_TYPE_INDIVIDIGUAL_GPD_KEY << EMBER_AF_GP_GPD_COMMISSIONING_REPLY_OPTIONS_KEY_TYPE_OFFSET);
            break;

          case EMBER_ZCL_GP_SECURITY_KEY_TYPE_ZIGBEE_NETWORK_KEY:
          case EMBER_ZCL_GP_SECURITY_KEY_TYPE_GPD_GROUP_KEY:
          case EMBER_ZCL_GP_SECURITY_KEY_TYPE_NETWORK_DERIVED_GROUP_KEY:
          case EMBER_ZCL_GP_SECURITY_KEY_TYPE_DERIVED_INDIVIDUAL_GPD_KEY:
            commReplyOptions |= (EMBER_ZCL_GP_SECURITY_KEY_TYPE_INDIVIDIGUAL_GPD_KEY << EMBER_AF_GP_GPD_COMMISSIONING_REPLY_OPTIONS_KEY_TYPE_OFFSET);
            /*
               //fill in fake key for now
               for(i=0;i<16;i++)
               key.contents[i]  = 0xc0 + i;
               MEMCOPY(lastKey,key.contents,16);

               commReplyOptions |= EMBER_AF_GP_GPD_COMMISSIONING_REPLY_OPTIONS_GPD_SECURITY_KEY_PRESENT;
               commReplyOptions |= ((gpdfExtendedOptions & EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_KEY_TYPE) >>EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_KEY_TYPE_OFFSET) <<EMBER_AF_GP_GPD_COMMISSIONING_REPLY_OPTIONS_KEY_TYPE_OFFSET;
             */
            break;
        }
      }
      if (securityLevel) {
        commReplyOptions |= EMBER_AF_GP_GPD_COMMISSIONING_REPLY_OPTIONS_GPDKEY_ENCRYPTION;
      }
      commReplyPayload[1] = commReplyOptions;
      index = 2;
      if (commReplyOptions & EMBER_AF_GP_GPD_COMMISSIONING_REPLY_OPTIONS_PAN_ID_PRESENT) {
        emberStoreLowHighInt16u(commReplyPayload + index, emberAfGetPanId());
        index += 2;
      }

      if (commReplyOptions & EMBER_AF_GP_GPD_COMMISSIONING_REPLY_OPTIONS_GPD_SECURITY_KEY_PRESENT) {
        if (commReplyOptions & EMBER_AF_GP_GPD_COMMISSIONING_REPLY_OPTIONS_GPDKEY_ENCRYPTION) {
          uint8_t mic[4];
          emDecryptKey(commNotificationOptions & 0x03, gpdSrcId, gpdIeee, &linkKey, mic, &key);
          MEMCOPY(commReplyPayload + index, key.contents, 16);
          index += 16;
          MEMCOPY(commReplyPayload + index, mic, 4);
          index += 4;
        } else {
          //send clear key
          MEMCOPY(commReplyPayload + index, key.contents, 16); //XXXJAR
          index += 16;
        }
      }

      emberAfFillCommandGreenPowerClusterGpResponseSmart(commNotificationOptions & 0x03, //app ID, FIXME
                                                         gppShortAddress,
                                                         emberAfGetRadioChannel(),
                                                         gpdSrcId,
                                                         gpdIeee,
                                                         endpoint,
                                                         EMBER_ZCL_GP_GPDF_COMMISSIONING_REPLY,
                                                         index - 1,
                                                         commReplyPayload

                                                         );

      apsFrame = emberAfGetCommandApsFrame();
      apsFrame->sourceEndpoint = GP_ENDPOINT;
      apsFrame->destinationEndpoint = GP_ENDPOINT;
      retval = emberAfSendCommandBroadcast(
        EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS);

      emberAfGreenPowerClusterPrintln("response send returned %d", retval);
    } else {
      //send the gp pairing, and we're done-ish
      EmberEUI64 ourEUI;
      EmberApsFrame *apsFrame;
      uint8_t retval;
      emberAfGetEui64(ourEUI);
      uint32_t pairingOptions;

      /*
         emberAfFillCommandGreenPowerClusterGpPairingSmart(uint32_t options,
         uint32_t gpdSrcId,
         uint8_t* gpdIeee,
         uint8_t endpoint,
         uint8_t* sinkIeeeAddress,
         uint16_t sinkNwkAddress,
         uint16_t sinkGroupId,
         uint8_t deviceId,
         uint32_t gpdSecurityFrameCounter,
         uint8_t* gpdKey,
         uint16_t assignedAlias,
         uint8_t forwardingRadius);
       */
      pairingOptions =
        EMBER_AF_GP_PAIRING_OPTION_ADD_SINK
        | (commNotificationOptions & 0x02) //FIXME: clean up
        | (EMBER_GP_SINK_TYPE_LW_UNICAST << 5)
        | (securityLevel << EMBER_AF_GP_PAIRING_OPTION_SECURITY_LEVEL_OFFSET)
        | EMBER_AF_GP_PAIRING_OPTION_COMMUNICATION_MODE //TODO: labels for individual modes
        | (securityKeyType << EMBER_AF_GP_PAIRING_OPTION_SECURITY_KEY_TYPE_OFFSET)
        | EMBER_AF_GP_PAIRING_OPTION_GPD_SECURITY_FRAME_COUNTER_PRESENT
        | EMBER_AF_GP_PAIRING_OPTION_GPD_SECURITY_KEY_PRESENT;

      emberAfFillCommandGreenPowerClusterGpPairingSmart(pairingOptions,
                                                        gpdSrcId,
                                                        gpdIeee,
                                                        endpoint,
                                                        ourEUI,
                                                        emberGetNodeId(),
                                                        0xFFFF, //groupID
                                                        0, //deviceId,
                                                        0, //securityFrameCounter
                                                        key.contents,
                                                        0xFFFF, //assigned alias
                                                        0xFF //Forwarding Radius
                                                        );

      apsFrame = emberAfGetCommandApsFrame();
      apsFrame->sourceEndpoint = GP_ENDPOINT;
      apsFrame->destinationEndpoint = GP_ENDPOINT;
      retval = emberAfSendCommandBroadcast(
        EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS);
      emberAfGreenPowerClusterPrintln("pairing send returned %d", retval);
    }

    kickout: return true;
  } else if (gpdCommandId == EMBER_ZCL_GP_GPDF_CHANNEL_REQUEST) {
    //Basic sink just runs with the first one, doesn't elect the best
    uint8_t nextChannel = gpdCommandPayload[1] & 0x0F;
    uint8_t UNUSED secondNextChannel = (gpdCommandPayload[1] & 0xF0) >> 4;  //TODO: what should this be used for?
    uint8_t channelConfigPayload[1];

    EmberApsFrame *apsFrame;
    uint8_t retval;
    //TODO check/clean up this grossness
    channelConfigPayload[0] = (emberAfGetRadioChannel() - 11) & 0x0F;
    channelConfigPayload[0] |= 0x10; // BASIC

    emberAfGreenPowerClusterPrintln("sending GP Response back with payload %1x and tx channel %1x", channelConfigPayload[0], nextChannel);

    emberAfFillCommandGreenPowerClusterGpResponseSmart(commNotificationOptions, //FIXME, should write our own options
                                                       gppShortAddress,
                                                       nextChannel,
                                                       0x00000000, //srcId
                                                       NULL, //IEEE
                                                       0, //endpoint
                                                       EMBER_ZCL_GP_GPDF_CHANNEL_CONFIGURATION,
                                                       1,
                                                       channelConfigPayload

                                                       );

    apsFrame = emberAfGetCommandApsFrame();
    apsFrame->sourceEndpoint = GP_ENDPOINT;
    apsFrame->destinationEndpoint = GP_ENDPOINT;
    retval = emberAfSendCommandBroadcast(
      EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS);

    return true;
  } else if (gpdCommandId == EMBER_ZCL_GP_GPDF_SUCCESS) {
    //XXXCOPIED and modded
    //send the gp pairing, and we're done-ish
    EmberEUI64 ourEUI;
    EmberApsFrame *apsFrame;
    uint8_t retval;
    emberAfGetEui64(ourEUI);
    uint32_t pairingOptions;
    EmberKeyData key;       //modded
    //uint8_t i;
    // for(i=0;i<16;i++)
    //  key.contents[i] = 0xC0 + i;
    MEMCOPY(key.contents, lastKey, 16);
    //endmodded

    /*
       emberAfFillCommandGreenPowerClusterGpPairingSmart(uint32_t options,
       uint32_t gpdSrcId,
       uint8_t* gpdIeee,
       uint8_t endpoint,
       uint8_t* sinkIeeeAddress,
       uint16_t sinkNwkAddress,
       uint16_t sinkGroupId,
       uint8_t deviceId,
       uint32_t gpdSecurityFrameCounter,
       uint8_t* gpdKey,
       uint16_t assignedAlias,
       uint8_t forwardingRadius);
     */
    pairingOptions =
      EMBER_AF_GP_PAIRING_OPTION_ADD_SINK
      | (EMBER_GP_SINK_TYPE_LW_UNICAST << 5)
      | (EMBER_GP_SECURITY_LEVEL_FC_MIC << 9)
      | (commNotificationOptions & 0x02)     //FIXME: clean up
      | EMBER_AF_GP_PAIRING_OPTION_COMMUNICATION_MODE     //TODO: labels for individual modes
      | (EMBER_GP_SECURITY_KEY_GPD_OOB << 11)
      | EMBER_AF_GP_PAIRING_OPTION_GPD_SECURITY_FRAME_COUNTER_PRESENT
      | EMBER_AF_GP_PAIRING_OPTION_GPD_SECURITY_KEY_PRESENT;

    emberAfFillCommandGreenPowerClusterGpPairingSmart(pairingOptions,
                                                      gpdSrcId,
                                                      gpdIeee,
                                                      endpoint,
                                                      ourEUI,
                                                      emberGetNodeId(),
                                                      0xFFFF,       //groupID
                                                      0,       //deviceId,
                                                      0,       //securityFrameCounter
                                                      key.contents,
                                                      0xFFFF,       //assigned alias
                                                      0xFF       //Forwarding Radius
                                                      );

    apsFrame = emberAfGetCommandApsFrame();
    apsFrame->sourceEndpoint = GP_ENDPOINT;
    apsFrame->destinationEndpoint = GP_ENDPOINT;
    retval = emberAfSendCommandBroadcast(
      EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS);
    emberAfGreenPowerClusterPrintln("pairing send returned %d", retval);
    //ENDXXXCOPIED

    return true;
  } else if (gpdCommandId == EMBER_ZCL_GP_GPDF_DECOMMISSIONING) {
    EmberApsFrame *apsFrame;
    uint8_t retval;
    uint32_t pairingOptions =
      EMBER_AF_GP_PAIRING_OPTION_REMOVE_GPD
      | (commNotificationOptions & 0x02)         //FIXME: clean up
      | (EMBER_GP_SINK_TYPE_LW_UNICAST << 5)
      | (EMBER_GP_SECURITY_LEVEL_FC_MIC << 9)
      | EMBER_AF_GP_PAIRING_OPTION_COMMUNICATION_MODE         //TODO: labels for individual modes
      | (EMBER_GP_SECURITY_KEY_GPD_OOB << 11)
      //| EMBER_AF_GP_PAIRING_OPTION_GPD_SECURITY_FRAME_COUNTER_PRESENT
      //| EMBER_AF_GP_PAIRING_OPTION_GPD_SECURITY_KEY_PRESENT;
    ;

    emberAfFillCommandGreenPowerClusterGpPairingSmart(pairingOptions,
                                                      gpdSrcId,
                                                      gpdIeee,
                                                      endpoint,
                                                      NULL,
                                                      emberGetNodeId(),
                                                      0xFFFF,           //groupID
                                                      0,           //deviceId,
                                                      0,           //securityFrameCounter
                                                      NULL,
                                                      0xFFFF,           //assigned alias
                                                      0xFF           //Forwarding Radius
                                                      );

    apsFrame = emberAfGetCommandApsFrame();
    apsFrame->sourceEndpoint = GP_ENDPOINT;
    apsFrame->destinationEndpoint = GP_ENDPOINT;
    retval = emberAfSendCommandBroadcast(
      EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS);
    emberAfGreenPowerClusterPrintln("pairing send returned %d", retval);
    return true;
  }

  return false;
}

/** @brief Green Power Cluster Gp Sink Commissioning Mode
 *
 *
 *
 * @param options   Ver.: always
 * @param gpmAddrForSecurity   Ver.: always
 * @param gpmAddrForPairing   Ver.: always
 * @param sinkEndpoint   Ver.: always
 */
bool emberAfGreenPowerClusterGpSinkCommissioningModeCallback(uint8_t options,
                                                             uint16_t gpmAddrForSecurity,
                                                             uint16_t gpmAddrForPairing,
                                                             uint8_t sinkEndpoint)
{
  uint8_t proxyOptions;
  EmberApsFrame *apsFrame;
  if (options & EMBER_AF_GP_SINK_COMMISSIONING_MODE_OPTIONS_ACTION) {
    commissioningState.inCommissioningMode = true;

    //enter commissioning mode
    if (options
        & (EMBER_AF_GP_SINK_COMMISSIONING_MODE_OPTIONS_INVOLVE_GPM_IN_SECURITY
           | EMBER_AF_GP_SINK_COMMISSIONING_MODE_OPTIONS_INVOLVE_GPM_IN_PAIRING)) {
      //these SHALL be 0 for now
      //TODO also check involve-TC
      goto kickout;
    }
    if (options & EMBER_AF_GP_SINK_COMMISSIONING_MODE_OPTIONS_INVOLVE_PROXIES) {
      uint8_t retval;
      //involve proxies
      commissioningState.proxiesInvolved = true;

      //TODO: set exit based on gpsCommissioningExitMode attribute
      proxyOptions =
        EMBER_AF_GP_PROXY_COMMISSIONING_MODE_OPTION_ACTION
        | EMBER_AF_GP_PROXY_COMMISSIONING_MODE_EXIT_MODE_ON_GP_PROXY_COMMISSIONING_MODE_EXIT;
      //| EMBER_AF_GP_PROXY_COMMISSIONING_MODE_OPTION_UNICAST_COMMUNICATION;
      emberAfFillCommandGreenPowerClusterGpProxyCommissioningModeSmart(proxyOptions,
                                                                       300, //commissioningWindow,
                                                                       0 //channel
                                                                       );
      apsFrame = emberAfGetCommandApsFrame();
      apsFrame->sourceEndpoint = GP_ENDPOINT;
      apsFrame->destinationEndpoint = GP_ENDPOINT;
      retval = emberAfSendCommandBroadcast(
        EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS);
      emberAfGreenPowerClusterPrintln("send returned %d", retval);
    } else {
      commissioningState.proxiesInvolved = false;
    }
  } else if (commissioningState.inCommissioningMode) {
    //exit comissioning mode
    commissioningState.inCommissioningMode = false;
    if (commissioningState.proxiesInvolved) {
      uint8_t retval;
      //involve proxies
      proxyOptions = 0;
      emberAfFillCommandGreenPowerClusterGpProxyCommissioningModeSmart(proxyOptions,
                                                                       300, //commissioningWindow,
                                                                       0 //channel
                                                                       );
      apsFrame = emberAfGetCommandApsFrame();
      apsFrame->sourceEndpoint = GP_ENDPOINT;
      apsFrame->destinationEndpoint = GP_ENDPOINT;
      retval = emberAfSendCommandBroadcast(
        EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS);
      emberAfGreenPowerClusterPrintln("send returned %d", retval);
    }
  }

  kickout: return true;
}

void emberAfPluginGreenPowerServerClearCustomizedTranslationTable()
{
  MEMSET(customizedTranslationTable,
         0x00,
         sizeof(EmberAfGreenPowerServerGpdCommandTranslation)
         * EMBER_AF_PLUGIN_GREEN_POWER_SERVER_CUSTOMIZED_GPD_TRANSLATION_TABLE_SIZE);
}

void emberAfPluginGreenPowerServerSetCustomizedTranslationTable(uint8_t index,
                                                                EmberAfGreenPowerServerGpdCommandTranslation * trans)
{
  if (index < EMBER_AF_PLUGIN_GREEN_POWER_SERVER_CUSTOMIZED_GPD_TRANSLATION_TABLE_SIZE) {
    MEMCOPY(&customizedTranslationTable[index],
            trans,
            sizeof(EmberAfGreenPowerServerGpdCommandTranslation));
  }
}

void emberAfPluginGreenPowerServerTablePrint(void)
{
  // print customized table.
  emberAfGreenPowerClusterPrintln("Price Server Customized Translation Table:");
  emberAfGreenPowerClusterPrintln("index:\tvalid\topt\tgpEp\tieee\t\t\tsrcId\t\tep\tgpdCommandId\tprofile\tcluster\tcmdId\tpayloadType\tpayload");
  uint8_t index;
  uint8_t len;
  for (index = 0; index < EMBER_AF_PLUGIN_GREEN_POWER_SERVER_CUSTOMIZED_GPD_TRANSLATION_TABLE_SIZE; index++) {
    EmberAfGreenPowerServerGpdCommandTranslation  * entry = &customizedTranslationTable[index];
    emberAfGreenPowerClusterPrint("%d\t\t%d\t0x%X\t0x%X\t",
                                  index,
                                  entry->validEntry,
                                  entry->options,
                                  entry->gpAddr.endpoint);
    if ((entry->options & EMBER_AF_GP_NOTIFICATION_OPTION_APPLICATION_ID)
        == EMBER_GP_APPLICATION_IEEE_ADDRESS) {
      emberAfGreenPowerClusterPrint("0x");
      emberAfPrintBigEndianEui64(entry->gpAddr.id.gpdIeeeAddress);
      emberAfGreenPowerClusterPrint("\t");
    } else {
      emberAfGreenPowerClusterPrint("0x0000000000000000\t");
    }

    if ((entry->options & EMBER_AF_GP_NOTIFICATION_OPTION_APPLICATION_ID)
        == EMBER_GP_APPLICATION_SOURCE_ID) {
      emberAfGreenPowerClusterPrint("0x%4X\t", entry->gpAddr.id.sourceId);
    } else {
      emberAfGreenPowerClusterPrint("0x00000000\t", entry->gpAddr.id.sourceId);
    }

    emberAfGreenPowerClusterPrint("%d\t0x%X\t\t0x%2X\t0x%2X\t0x%X\t",
                                  entry->endpoint,
                                  entry->gpdCommand,
                                  entry->zigbeeProfile,
                                  entry->zigbeeCluster,
                                  entry->zigbeeCommandId);
    emberAfGreenPowerClusterPrint("%X\t\t0x", entry->payloadSrc);

    len = emberAfStringLength(entry->zclPayloadDefault);
    if (len >= EMBER_AF_GREEN_POWER_SERVER_TRANSLATION_TABLE_ENTRY_ZCL_PAYLOAD_LEN) {
      len = EMBER_AF_GREEN_POWER_SERVER_TRANSLATION_TABLE_ENTRY_ZCL_PAYLOAD_LEN - 1;
    }
    emberAfPrintBuffer(EMBER_AF_PRINT_APP, entry->zclPayloadDefault, len + 1, false);
    emberAfGreenPowerClusterPrintln("");
  }
}
