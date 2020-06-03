// *******************************************************************
// * simple-metering-test.h
// *
// *
// * Copyright 2007 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

void emAfTestMeterTick(uint8_t endpoint);
void emAfTestMeterInit(uint8_t endpoint);

// The Test meter's profile interval period timeframe enum value
// is 3 which according to the SE spec is 15 minutes
#ifdef EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_TEST_METER_ENABLE

#define PROFILE_INTERVAL_PERIOD_TIMEFRAME 3
#define PROFILE_INTERVAL_PERIOD_IN_MINUTES 15
#define PROFILE_INTERVAL_PERIOD_IN_SECONDS (PROFILE_INTERVAL_PERIOD_IN_MINUTES * 60)
#define PROFILE_INTERVAL_PERIOD_IN_MILLISECONDS ((PROFILE_INTERVAL_PERIOD_IN_MINUTES * 60) * 1000)
#define MAX_PROFILE_INDEX (EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_TEST_METER_PROFILES - 1)
#define TOTAL_PROFILE_TIME_SPAN_IN_SECONDS (EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_TEST_METER_PROFILES * (PROFILE_INTERVAL_PERIOD_IN_MINUTES * 60))

void afTestMeterPrint(void);
void afTestMeterSetConsumptionRate(uint16_t rate, uint8_t endpoint);
void afTestMeterSetConsumptionVariance(uint16_t variance);
void afTestMeterAdjust(uint8_t endpoint);

// 0 off, 1 if electric, 2 if gas
void afTestMeterMode(uint8_t endpoint, uint8_t electric);

void afTestMeterSetError(uint8_t endpoint, uint8_t error);
// Sets the random error occurence:
//   data = 0: disable
//   otherwise:
void afTestMeterRandomError(uint8_t changeIn256);

void afTestMeterEnableProfiles(uint8_t enable);

bool emAfTestMeterGetProfiles(uint8_t intervalChannel,
                              uint32_t endTime,
                              uint8_t numberOfPeriods);
#else

bool emAfTestMeterGetProfiles(uint8_t intervalChannel,
                              uint32_t endTime,
                              uint8_t numberOfPeriods);

#endif
