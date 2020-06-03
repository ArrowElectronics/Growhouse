/**
 * Custom Application Tokens
 */
#define CREATOR_REPORT_TABLE  (0x8725)
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved
#define NVM3KEY_REPORT_TABLE (NVM3KEY_DOMAIN_ZIGBEE | 0x4000)

#ifdef DEFINETYPES
// Include or define any typedef for tokens here
#endif //DEFINETYPES
#ifdef DEFINETOKENS
// Define the actual token storage information here

// Following is for backward compatibility.
// The default reporting will generate a table that is mandatory
// but user may still allocate some table for adding more reporting over
// the air or by cli as part of reporting plugin.
#if defined EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS_TABLE_SIZE
#define REPORT_TABLE_SIZE (EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS_TABLE_SIZE + EMBER_AF_PLUGIN_REPORTING_TABLE_SIZE)
#else
#define REPORT_TABLE_SIZE (EMBER_AF_PLUGIN_REPORTING_TABLE_SIZE)
#endif

DEFINE_INDEXED_TOKEN(REPORT_TABLE,
                     EmberAfPluginReportingEntry,
                     REPORT_TABLE_SIZE,
                     { EMBER_ZCL_REPORTING_DIRECTION_REPORTED,
                       EMBER_AF_PLUGIN_REPORTING_UNUSED_ENDPOINT_ID })

#endif //DEFINETOKENS
