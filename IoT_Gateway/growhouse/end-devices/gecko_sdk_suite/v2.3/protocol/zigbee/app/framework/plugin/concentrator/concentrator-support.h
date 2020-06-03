extern uint8_t emAfRouteErrorCount;
extern uint8_t emAfDeliveryFailureCount;

extern EmberEventControl emberAfPluginConcentratorUpdateEventControl;

#define LOW_RAM_CONCENTRATOR  EMBER_LOW_RAM_CONCENTRATOR
#define HIGH_RAM_CONCENTRATOR EMBER_HIGH_RAM_CONCENTRATOR
#define ADDRESS_DISCOVERY_DELAY_QS 2
#define emAfConcentratorStartDiscovery emberAfPluginConcentratorQueueDiscovery
void emAfConcentratorStopDiscovery(void);

uint32_t emberAfPluginConcentratorQueueDiscovery(void);
void emberAfPluginConcentratorStopDiscovery(void);

// These values are defined by appbuilder.
#define NONE (0x00)
#define FULL (0x01)
enum {
  EMBER_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_NONE = NONE,
  EMBER_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_FULL = FULL,

  EMBER_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_MAX = EMBER_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_FULL,
};
typedef uint8_t EmberAfPluginConcentratorRouterBehavior;

extern EmberAfPluginConcentratorRouterBehavior emAfPluginConcentratorRouterBehavior;
#define emberAfPluginConcentratorGetRouterBehavior() \
  (emAfPluginConcentratorRouterBehavior)
#define emberAfPluginConcentratorSetRouterBehavior(behavior) \
  do { emAfPluginConcentratorRouterBehavior = behavior; } while (0);

/** @brief Starts periodic many-to-one route discovery.
 * Periodic discovery is started by default on bootup,
 * but this function may be used if discovery has been
 * stopped by a call to ::emberConcentratorStopDiscovery().
 */
void emberConcentratorStartDiscovery(void);

/** @brief Stops periodic many-to-one route discovery. */
void emberConcentratorStopDiscovery(void);
