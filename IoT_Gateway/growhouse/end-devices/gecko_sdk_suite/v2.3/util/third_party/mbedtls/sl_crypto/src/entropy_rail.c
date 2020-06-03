/*
 *  \brief Collect entropy from the RAIL API on Silicon Labs radios.
 *
 *  Copyright (C) 2018, Silicon Labs, http://www.silabs.com
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_ENTROPY_RAIL_C)

#include "em_device.h"

#if defined(_EFR_DEVICE)

#include "entropy_rail.h"
#include "rail.h"
#include <string.h>

void mbedtls_entropy_rail_init( mbedtls_entropy_rail_context *ctx )
{
    memset(ctx, 0, sizeof(mbedtls_entropy_rail_context));
}

void mbedtls_entropy_rail_free( mbedtls_entropy_rail_context *ctx )
{
    memset(ctx, 0, sizeof(mbedtls_entropy_rail_context));
}

int mbedtls_entropy_rail_poll( mbedtls_entropy_rail_context *ctx,
			      unsigned char *output, size_t len, size_t *olen )
{
    (void) ctx;

    if ( len > UINT16_MAX )
    {
        return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    }

    *olen = (size_t) RAIL_GetRadioEntropy(RAIL_EFR32_HANDLE,
					  (uint8_t*) output,
					  (uint16_t) len);
    return 0;
}

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT) && \
  defined(MBEDTLS_ENTROPY_HARDWARE_ALT_RAIL)

int mbedtls_hardware_poll( void *data,
                           unsigned char *output,
                           size_t len,
                           size_t *olen )
{
    return mbedtls_entropy_rail_poll((mbedtls_entropy_rail_context*)data,
				     output, len, olen);
}

#endif /* #if defined(MBEDTLS_ENTROPY_HARDWARE_ALT) && ... */

#endif /* #if defined(_EFR_DEVICE) */

#endif /* #if defined(MBEDTLS_ENTROPY_RAIL_C) */
