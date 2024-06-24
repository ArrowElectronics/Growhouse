// Copyright 2013 Silicon Laboratories, Inc.

extern bool emAfEnablePollCompletedCallback;

typedef struct {
  uint32_t pollIntervalMs;
  uint8_t numPollsFailing;
} EmAfPollingState;
extern EmAfPollingState emAfPollingStates[];
void emAfPollCompleteHandler(EmberStatus status, uint8_t limit);

void emberAfPluginEndDeviceSupportMoveNetworkEventHandler(void);
