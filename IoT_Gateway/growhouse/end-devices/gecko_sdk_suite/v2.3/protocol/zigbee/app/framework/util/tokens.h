// *******************************************************************
// * tokens.h
// *
// * Framework header file, responsible for including generated tokens
// *
// * Copyright 2014 Silicon Laboratories, Inc.                              *80*
// *******************************************************************

#include "app/framework/include/af-types.h"

#ifdef GENERATED_TOKEN_HEADER

// If we have generated header, use it.
#include GENERATED_TOKEN_HEADER

#else

// We don't have generated header. Default is to have no tokens.

#define GENERATED_TOKEN_LOADER(endpoint) {}
#define GENERATED_TOKEN_SAVER {}

#endif // GENERATED_TOKEN_HEADER
