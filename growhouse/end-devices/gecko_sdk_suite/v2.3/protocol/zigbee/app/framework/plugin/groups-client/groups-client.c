// *******************************************************************
// * groups-client.c
// *
// *
// * Copyright 2010 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

#include "../../include/af.h"

bool emberAfGroupsClusterAddGroupResponseCallback(uint8_t status,
                                                  uint16_t groupId)
{
  emberAfGroupsClusterPrintln("RX: AddGroupResponse 0x%x, 0x%2x",
                              status,
                              groupId);
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfGroupsClusterViewGroupResponseCallback(uint8_t status,
                                                   uint16_t groupId,
                                                   uint8_t* groupName)
{
  emberAfGroupsClusterPrint("RX: ViewGroupResponse 0x%x, 0x%2x, \"",
                            status,
                            groupId);
  emberAfGroupsClusterPrintString(groupName);
  emberAfGroupsClusterPrintln("\"");
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfGroupsClusterGetGroupMembershipResponseCallback(uint8_t capacity,
                                                            uint8_t groupCount,
                                                            uint8_t* groupList)
{
  uint8_t i;
  emberAfGroupsClusterPrint("RX: GetGroupMembershipResponse 0x%x, 0x%x,",
                            capacity,
                            groupCount);
  for (i = 0; i < groupCount; i++) {
    emberAfGroupsClusterPrint(" [0x%2x]",
                              emberAfGetInt16u(groupList + (i << 1), 0, 2));
  }
  emberAfGroupsClusterPrintln("");
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfGroupsClusterRemoveGroupResponseCallback(uint8_t status,
                                                     uint16_t groupId)
{
  emberAfGroupsClusterPrintln("RX: RemoveGroupResponse 0x%x, 0x%2x",
                              status,
                              groupId);
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}
