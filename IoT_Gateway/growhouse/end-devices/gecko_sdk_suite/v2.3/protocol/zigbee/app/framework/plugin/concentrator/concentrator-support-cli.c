// *****************************************************************************
// * concentrator-support-cli.c
// *
// * CLI interface to manage the concentrator's periodic MTORR broadcasts.
// *
// * Copyright 2012 by Ember Corporation. All rights reserved.              *80*
// *****************************************************************************

#include "app/framework/include/af.h"
#include "app/framework/util/af-main.h"
#include "app/util/serial/command-interpreter2.h"
#include "app/framework/plugin/concentrator/concentrator-support.h"
#include "source-route-common.h"

// *****************************************************************************
// Forward Declarations

void emberAfPluginConcentratorStatus(void);
void emberAfPluginConcentratorStartDiscovery(void);
void emberAfPluginConcentratorAggregationCommand(void);
void emberAfPluginConcentratorPrintHostSourceRouteTable(void);
void emberAfPluginConcentratorPrintSourceRouteTable(void);
void emberAfGetSourceRoute(void);
// *****************************************************************************
// Globals

#if !defined(EMBER_AF_GENERATE_CLI)
EmberCommandEntry emberAfPluginConcentratorCommands[] = {
  emberCommandEntryAction("status", emberAfPluginConcentratorStatus, "",
                          "Prints current status and configured parameters of the concentrator"),
  emberCommandEntryAction("start", emberAfPluginConcentratorStartDiscovery, "",
                          "Starts the periodic broadcast of MTORRs"),
  emberCommandEntryAction("stop", emberAfPluginConcentratorStopDiscovery, "",
                          "Stops the periodic broadcast of MTORRs"),
  emberCommandEntryAction("agg", emberAfPluginConcentratorAggregationCommand, "",
                          "Sends an MTORR broadcast now."),
  emberCommandEntryAction("print-table",
                          emberAfPluginConcentratorPrintSourceRouteTable,
                          "",
                          "Prints the source route table."),
  emberCommandEntryAction("print-host-table",
                          emberAfPluginConcentratorPrintHostSourceRouteTable,
                          "",
                          "Prints the host-side source route table."),
  emberCommandEntryTerminator(),
};
#endif // EMBER_AF_GENERATE_CLI

// *****************************************************************************
// Functions

void emberAfPluginConcentratorPrintHostSourceRouteTable(void)
{
  uint8_t i;
  // emberAfAppPrintln("Source Route Table\n");
  // emberAfAppPrintln(  "index dest    closer  older");
  // for (i = 0; i < sourceRouteTableSize; i++) {
  //   emberAfAppPrintln("%d:    0x%2X  %d      %d",
  //                     i,
  //                     sourceRouteTable[i].destination,
  //                     sourceRouteTable[i].closerIndex,
  //                     sourceRouteTable[i].olderIndex);
  // }
  emberAfAppPrintln("Source Routes");

  for (i = 0; i < sourceRouteGetCount(); i++) {
    uint8_t closerIndex = sourceRouteTable[i].closerIndex;
    emberAfAppPrint("%d: 0x%2X -> ",
                    i,
                    sourceRouteTable[i].destination);

    while (closerIndex != NULL_INDEX) {
      emberAfAppPrint("0x%2X -> ", sourceRouteTable[closerIndex].destination);
      closerIndex = sourceRouteTable[closerIndex].closerIndex;
    }
    emberAfAppPrintln("0x%2X (Me)", emberAfGetNodeId());
  }
  emberAfAppPrintln("%d of %d total entries.",
                    sourceRouteGetCount(),
                    sourceRouteTableSize);
}

void emberAfPluginConcentratorPrintSourceRouteTable(void)
{
  uint8_t i;
  for (i = 0; i < emberAfGetSourceRouteTableFilledSize(); i++) {
    uint8_t index = i;
    uint8_t closerIndex;
    EmberNodeId destination;
    EmberStatus status = emberAfGetSourceRouteTableEntry(index,
                                                         &destination,
                                                         &closerIndex);
    if (status != EMBER_SUCCESS) {
      emberAfAppPrintln("0 of 0 total entries.");
      return;
    }

    emberAfAppPrint("%d: 0x%2X -> ",
                    index, destination);

    while (closerIndex != NULL_INDEX) {
      index = closerIndex;
      status = emberAfGetSourceRouteTableEntry(index,
                                               &destination,
                                               &closerIndex);
      if (status == EMBER_SUCCESS) {
        emberAfAppPrint("0x%2X -> ", destination);
      } else {
        emberAfAppPrint("NULL ");
        return;
      }
    }
    emberAfAppPrintln("0x%2X (Me)", emberAfGetNodeId());
  }
  emberAfAppPrintln("%d of %d total entries.",
                    emberAfGetSourceRouteTableFilledSize(),
                    emberAfGetSourceRouteTableTotalSize());
}

void emberAfPluginConcentratorStatus(void)
{
  bool active = (emberAfPluginConcentratorUpdateEventControl.status
                 != EMBER_EVENT_INACTIVE);
  uint32_t nowMS32 = halCommonGetInt32uMillisecondTick();

  emberAfAppPrintln("Active: %p",
                    (active
                     ? "yes"
                     : "no"));
  emberAfAppPrintln("Type:  %p RAM",
                    ((EMBER_AF_PLUGIN_CONCENTRATOR_CONCENTRATOR_TYPE
                      == EMBER_LOW_RAM_CONCENTRATOR)
                     ? "Low"
                     : "High"));
  emberAfAppPrintln("Time before next broadcast (ms):   %l",
                    emberAfPluginConcentratorUpdateEventControl.timeToExecute - nowMS32);
  emberAfAppPrintln("Min Time Between Broadcasts (sec): %d",
                    EMBER_AF_PLUGIN_CONCENTRATOR_MIN_TIME_BETWEEN_BROADCASTS_SECONDS);
  emberAfAppPrintln("Max Time Between Broadcasts (sec): %d",
                    EMBER_AF_PLUGIN_CONCENTRATOR_MAX_TIME_BETWEEN_BROADCASTS_SECONDS);
  emberAfAppPrintln("Max Hops: %d",
                    (EMBER_AF_PLUGIN_CONCENTRATOR_MAX_HOPS == 0
                     ? EMBER_MAX_HOPS
                     : EMBER_AF_PLUGIN_CONCENTRATOR_MAX_HOPS));
  emberAfAppPrintln("Route Error Threshold:      %d",
                    EMBER_AF_PLUGIN_CONCENTRATOR_ROUTE_ERROR_THRESHOLD);
  emberAfAppPrintln("Delivery Failure Threshold: %d",
                    EMBER_AF_PLUGIN_CONCENTRATOR_DELIVERY_FAILURE_THRESHOLD);
}

void emberAfPluginConcentratorStartDiscovery(void)
{
  uint32_t qsLeft = emberAfPluginConcentratorQueueDiscovery();
  emberAfAppPrintln("%d sec until next MTORR broadcast", (qsLeft >> 2));
}

void emberAfPluginConcentratorAggregationCommand(void)
{
  emberAfPluginConcentratorQueueDiscovery();
}

void emberAfPluginConcentratorSetRouterBehaviorCommand(void)
{
  EmberAfPluginConcentratorRouterBehavior behavior
    = (EmberAfPluginConcentratorRouterBehavior)emberUnsignedCommandArgument(0);

  if (behavior > EMBER_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_MAX) {
    emberAfCorePrintln("Illegal concentrator router behavior: 0x%X", behavior);
  } else {
    emberAfPluginConcentratorSetRouterBehavior(behavior);
    emberAfCorePrintln("Set concentrator router behavior to: 0x%X", behavior);
  }
}
