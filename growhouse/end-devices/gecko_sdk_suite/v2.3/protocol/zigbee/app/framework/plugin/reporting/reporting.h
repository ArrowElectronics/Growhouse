// *******************************************************************
// * reporting.h
// *
// *
// * Copyright 2011 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

// The default reporting will generate a table that is mandatory
// but user may still allocate some table for adding more reporting over
// the air or by cli as part of reporting plugin.
#ifndef REPORT_TABLE_SIZE
#if defined EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS_TABLE_SIZE
#define REPORT_TABLE_SIZE (EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS_TABLE_SIZE + EMBER_AF_PLUGIN_REPORTING_TABLE_SIZE)
#else
#define REPORT_TABLE_SIZE (EMBER_AF_PLUGIN_REPORTING_TABLE_SIZE)
#endif
#endif

typedef struct {
  uint32_t lastReportTimeMs;
  EmberAfDifferenceType lastReportValue;
  bool reportableChange;
} EmAfPluginReportVolatileData;
extern EmAfPluginReportVolatileData emAfPluginReportVolatileData[];
EmberAfStatus emberAfPluginReportingConfigureReportedAttribute(const EmberAfPluginReportingEntry *newEntry);
void emAfPluginReportingGetEntry(uint8_t index, EmberAfPluginReportingEntry *result);
void emAfPluginReportingSetEntry(uint8_t index, EmberAfPluginReportingEntry *value);
uint8_t emAfPluginReportingAddEntry(EmberAfPluginReportingEntry* newEntry);
EmberStatus emAfPluginReportingRemoveEntry(uint8_t index);
bool emAfPluginReportingDoEntriesMatch(const EmberAfPluginReportingEntry* const entry1,
                                       const EmberAfPluginReportingEntry* const entry2);
uint8_t emAfPluginReportingConditionallyAddReportingEntry(EmberAfPluginReportingEntry* newEntry);
void emberAfPluginReportingLoadReportingConfigDefaults(void);
bool emberAfPluginReportingGetReportingConfigDefaults(EmberAfPluginReportingEntry *defaultConfiguration);
