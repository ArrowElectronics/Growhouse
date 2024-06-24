// *******************************************************************
// * key-establishment-storage.h
// *
// * API for the storage of public temporary partner data.
// * - Partner Certificate
// * - Partner Ephemeral Public Key
// * - A single SMAC
// *
// * Copyright 2008 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

// If isCertificate is false, data is a public key.
bool storePublicPartnerData(bool isCertificate,
                            uint8_t* data);
bool storePublicPartnerData163k1(bool isCertificate,
                                 uint8_t* data);
bool storePublicPartnerData283k1(bool isCertificate,
                                 uint8_t* data);
bool retrieveAndClearPublicPartnerData163k1(EmberCertificateData* partnerCertificate,
                                            EmberPublicKeyData* partnerEphemeralPublicKey);

bool retrieveAndClearPublicPartnerData(EmberCertificate283k1Data* partnerCertificate,
                                       EmberPublicKey283k1Data* partnerEphemeralPublicKey);
bool retrieveAndClearPublicPartnerData283k1(EmberCertificate283k1Data* partnerCertificate,
                                            EmberPublicKey283k1Data* partnerEphemeralPublicKey);
bool storeSmac(EmberSmacData* smac);
bool getSmacPointer(EmberSmacData** smacPtr);

void clearAllTemporaryPublicData(void);
