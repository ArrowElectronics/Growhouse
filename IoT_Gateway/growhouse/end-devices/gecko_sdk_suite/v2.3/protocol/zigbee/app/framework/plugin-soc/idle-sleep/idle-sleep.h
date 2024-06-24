// Copyright 2013 Silicon Laboratories, Inc.

extern bool emAfStayAwakeWhenNotJoined;
extern bool emAfForceEndDeviceToStayAwake;

void emberAfForceEndDeviceToStayAwake(bool stayAwake);
bool emAfOkToIdleOrSleep(void);
