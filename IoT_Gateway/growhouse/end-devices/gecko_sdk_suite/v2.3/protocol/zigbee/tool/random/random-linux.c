//==============================================================================
// File: random-linux.c
//
// Description:
//   This code handles Linux specifics related to generating a cryptographic
//   quality random number.
//
// Copyright 2016 Silicon Laboratories, Inc.                                *80*
//==============================================================================

#include PLATFORM_HEADER

#include <stdio.h>  // fread
#include <errno.h>  // errno
#include "random-number-generation.h"

//==============================================================================
// Globals
//==============================================================================

// /dev/random is supposed to provide random numbers suitable for security
// operations.  It is assumed that most Linux distributions save a random
// number on shutdown so that it can be used as the seed for the random number
// generator on bootup.  However we can't guarantee that it is the best source.
// Our strategy is print out the source for the randomness and hope security
// conscious users will verify this on their own systems.

static const char randomDevice[] = "/dev/random";

//==============================================================================
// Funtions
//==============================================================================

int platformRandomDataFunction(unsigned char* buffer, unsigned long size)
{
  int status = 0;

  fprintf(stderr, "Using %s for random number generation\n", randomDevice);
  FILE* randDevice = fopen(randomDevice, "r");
  if (randDevice == NULL) {
    fprintf(stderr, "Could not open random device: %s\n", strerror(errno));
    return 1;
  }
  fprintf(stderr, "Gathering sufficient entropy... (may take up to a minute)...\n");

  if (size != fread(buffer,
                    1,       // size of each element
                    size,    // number of elements
                    randDevice)) {
    fprintf(stderr, "Failed to read random data from device: %s\n", strerror(errno));
    status = 1;
  }
  fclose(randDevice);
  return status;
}
