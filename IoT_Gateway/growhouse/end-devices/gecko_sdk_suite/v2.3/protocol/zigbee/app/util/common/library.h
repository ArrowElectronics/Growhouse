// File: library.h
//
// Description: Code to display or retrieve the presence or absence of
//   Ember stack libraries on the device.
//
// Copyright 2009 by Ember Corporation. All rights reserved.                *80*

#ifndef SILABS_APP_UTIL_LIBRARY_H
#define SILABS_APP_UTIL_LIBRARY_H

void printAllLibraryStatus(void);
bool isLibraryPresent(uint8_t libraryId);

#define LIBRARY_COMMANDS                                     \
  emberCommandEntryAction("libs", printAllLibraryStatus, "", \
                          "Prints the status of all Ember stack libraries"),

#endif // SILABS_APP_UTIL_LIBRARY_H
