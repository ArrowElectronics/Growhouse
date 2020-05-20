//
// test-harness-z3-nwk.h
//
// August 3, 2015
// Refactored November 23, 2015
//
// ZigBee 3.0 nwk test harness functionality
//

// -----------------------------------------------------------------------------
// Constants

#define NWK_LEAVE_COMMAND           (0x04)
#define NWK_REJOIN_REQUEST_COMMAND  (0x06)
#define NWK_REJOIN_RESPONSE_COMMAND (0x07)

// -----------------------------------------------------------------------------
// Globals

extern bool emAfPluginTestHarnessZ3IgnoreLeaveCommands;
