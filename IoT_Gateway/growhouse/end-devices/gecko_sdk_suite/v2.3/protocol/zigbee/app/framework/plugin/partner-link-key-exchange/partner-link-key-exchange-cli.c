// *******************************************************************
// * partner-link-key-exchange-cli.c
// *
// *
// * Copyright 2013 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

#include "app/framework/include/af.h"
#include "app/util/serial/command-interpreter2.h"
#include "app/framework/plugin/partner-link-key-exchange/partner-link-key-exchange.h"

#if defined(EZSP_HOST)
extern EzspStatus emberAfSetEzspPolicy(EzspPolicyId policyId,
                                       EzspDecisionId decisionId,
                                       PGM_P policyName,
                                       PGM_P decisionName);
#endif

extern void emAfPrintStatus(PGM_P task,
                            EmberStatus status);

#if !defined(EMBER_AF_GENERATE_CLI)
void cbkeAllowPartner(void);
void cbkePartnerCommand(void);

EmberCommandEntry emberAfPluginPartnerLinkKeyExchangeCommands[] = {
  emberCommandEntryAction("partner", cbkePartnerCommand, "vu", ""),
  emberCommandEntryAction("allow-partner", cbkeAllowPartner, "u", ""),
  emberCommandEntryTerminator(),
};
#endif // EMBER_AF_GENERATE_CLI

void cbkeAllowPartner(void)
{
  emAfAllowPartnerLinkKey = (bool)emberUnsignedCommandArgument(0);

#if defined(EZSP_HOST)
  emberAfSetEzspPolicy(EZSP_APP_KEY_REQUEST_POLICY,
                       (emAfAllowPartnerLinkKey
                        ? EZSP_ALLOW_APP_KEY_REQUESTS
                        : EZSP_DENY_APP_KEY_REQUESTS),
                       "App Link Key Request Policy",
                       (emAfAllowPartnerLinkKey
                        ? "Allow"
                        : "Deny"));

#endif
}

void cbkePartnerCommand(void)
{
  EmberNodeId target = (EmberNodeId)emberUnsignedCommandArgument(0);
  uint8_t endpoint     = (uint8_t)emberUnsignedCommandArgument(1);
  EmberStatus status = emberAfInitiatePartnerLinkKeyExchange(target,
                                                             endpoint,
                                                             NULL); // callback
  emberAfCoreDebugExec(emAfPrintStatus("partner link key request", status));
}
