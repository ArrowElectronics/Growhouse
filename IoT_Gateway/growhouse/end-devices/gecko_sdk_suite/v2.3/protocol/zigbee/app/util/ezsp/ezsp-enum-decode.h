// File: ezsp-enum-decode.h
//
// Description:
//
// Copyright 2007 by Ember Corporation. All rights reserved.                *80*

#ifndef SILABS_EZSP_ENUM_DECODE_H
#define SILABS_EZSP_ENUM_DECODE_H

const char *decodeEzspConfigId(uint8_t value);
const char *decodeEzspPolicyId(uint8_t value);
const char *decodeEzspDecisionId(uint8_t value);
const char *decodeEzspMfgTokenId(uint8_t value);
const char *decodeEzspStatus(uint8_t value);
const char *decodeFrameId(uint8_t value);

#endif // __EZSP_H__
