/***************************************************************************//**
 * @file
 * @brief BSP Configuration - Configuration Template File
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _BSP_CFG_H_
#define  _BSP_CFG_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                              INCLUDES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <bsp/siliconlabs/generic/include/bsp_opt_def.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                        CLOCK CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

#define  BSP_HF_CLK_SEL                                     BSP_HF_CLK_HFRCO

#define  BSP_LF_CLK_SEL                                     BSP_LF_CLK_LFRCO

/********************************************************************************************************
 ********************************************************************************************************
 *                                            MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of bsp_cfg.h module include.