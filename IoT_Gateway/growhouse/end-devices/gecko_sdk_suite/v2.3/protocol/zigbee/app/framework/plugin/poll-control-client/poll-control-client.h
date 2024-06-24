// *******************************************************************
// * poll-control-client.h
// *
// *
// * Copyright 2012 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

// Set fast polling mode
void emAfSetFastPollingMode(bool mode);

// Set fast polling timeout
void emAfSetFastPollingTimeout(uint16_t timeout);

// Set response mode
void emAfSetResponseMode(bool mode);

// Print mode and timeout
void emAfPollControlClientPrint(void);
