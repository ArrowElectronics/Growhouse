// *******************************************************************
// * scenes.h
// *
// *
// * Copyright 2007 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

EmberAfStatus emberAfScenesSetSceneCountAttribute(uint8_t endpoint,
                                                  uint8_t newCount);
EmberAfStatus emberAfScenesMakeValid(uint8_t endpoint,
                                     uint8_t sceneId,
                                     uint16_t groupId);

// DEPRECATED.
#define emberAfScenesMakeInvalid emberAfScenesClusterMakeInvalidCallback

void emAfPluginScenesServerPrintInfo(void);

extern uint8_t emberAfPluginScenesServerEntriesInUse;
#if defined(EMBER_AF_PLUGIN_SCENES_USE_TOKENS) && !defined(EZSP_HOST)
// In this case, we use token storage
  #define emberAfPluginScenesServerRetrieveSceneEntry(entry, i) \
  halCommonGetIndexedToken(&entry, TOKEN_SCENES_TABLE, i)
  #define emberAfPluginScenesServerSaveSceneEntry(entry, i) \
  halCommonSetIndexedToken(TOKEN_SCENES_TABLE, i, &entry)
  #define emberAfPluginScenesServerNumSceneEntriesInUse()                               \
  (halCommonGetToken(&emberAfPluginScenesServerEntriesInUse, TOKEN_SCENES_NUM_ENTRIES), \
   emberAfPluginScenesServerEntriesInUse)
  #define emberAfPluginScenesServerSetNumSceneEntriesInUse(x) \
  (emberAfPluginScenesServerEntriesInUse = (x),               \
   halCommonSetToken(TOKEN_SCENES_NUM_ENTRIES, &emberAfPluginScenesServerEntriesInUse))
  #define emberAfPluginScenesServerIncrNumSceneEntriesInUse()                            \
  ((halCommonGetToken(&emberAfPluginScenesServerEntriesInUse, TOKEN_SCENES_NUM_ENTRIES), \
    ++emberAfPluginScenesServerEntriesInUse),                                            \
   halCommonSetToken(TOKEN_SCENES_NUM_ENTRIES, &emberAfPluginScenesServerEntriesInUse))
  #define emberAfPluginScenesServerDecrNumSceneEntriesInUse()                            \
  ((halCommonGetToken(&emberAfPluginScenesServerEntriesInUse, TOKEN_SCENES_NUM_ENTRIES), \
    --emberAfPluginScenesServerEntriesInUse),                                            \
   halCommonSetToken(TOKEN_SCENES_NUM_ENTRIES, &emberAfPluginScenesServerEntriesInUse))
#else
// Use normal RAM storage
extern EmberAfSceneTableEntry emberAfPluginScenesServerSceneTable[];
  #define emberAfPluginScenesServerRetrieveSceneEntry(entry, i) \
  (entry = emberAfPluginScenesServerSceneTable[i])
  #define emberAfPluginScenesServerSaveSceneEntry(entry, i) \
  (emberAfPluginScenesServerSceneTable[i] = entry)
  #define emberAfPluginScenesServerNumSceneEntriesInUse() \
  (emberAfPluginScenesServerEntriesInUse)
  #define emberAfPluginScenesServerSetNumSceneEntriesInUse(x) \
  (emberAfPluginScenesServerEntriesInUse = (x))
  #define emberAfPluginScenesServerIncrNumSceneEntriesInUse() \
  (++emberAfPluginScenesServerEntriesInUse)
  #define emberAfPluginScenesServerDecrNumSceneEntriesInUse() \
  (--emberAfPluginScenesServerEntriesInUse)
#endif // Use tokens

bool emberAfPluginScenesServerParseAddScene(const EmberAfClusterCommand *cmd,
                                            uint16_t groupId,
                                            uint8_t sceneId,
                                            uint16_t transitionTime,
                                            uint8_t *sceneName,
                                            uint8_t *extensionFieldSets);
bool emberAfPluginScenesServerParseViewScene(const EmberAfClusterCommand *cmd,
                                             uint16_t groupId,
                                             uint8_t sceneId);
