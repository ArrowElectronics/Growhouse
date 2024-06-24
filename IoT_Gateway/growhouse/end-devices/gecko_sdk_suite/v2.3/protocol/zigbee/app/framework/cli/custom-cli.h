#ifndef CUSTOM_CLI_H
 #define CUSTOM_CLI_H
 #ifdef EMBER_AF_ENABLE_CUSTOM_COMMANDS
extern EmberCommandEntry emberAfCustomCommands[];
   #ifndef CUSTOM_SUBMENU_NAME
     #define CUSTOM_SUBMENU_NAME "custom"
   #endif
   #ifndef CUSTOM_SUBMENU_DESCRIPTION
     #define CUSTOM_SUBMENU_DESCRIPTION "Custom commands defined by the developer"
   #endif
   #define CUSTOM_COMMANDS   emberCommandEntrySubMenu(CUSTOM_SUBMENU_NAME, emberAfCustomCommands, CUSTOM_SUBMENU_DESCRIPTION),
 #else
   #ifndef CUSTOM_COMMANDS
     #define CUSTOM_COMMANDS
   #endif
 #endif
#endif
