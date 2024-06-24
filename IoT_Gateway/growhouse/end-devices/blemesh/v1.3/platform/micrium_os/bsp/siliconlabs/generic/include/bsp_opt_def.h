/*
*********************************************************************************************************
*                                             uC/OS-Common
*                                  Common Features for Micrium Stacks
*
*                          (c) Copyright 2015-2016; Micrium, Inc.; Weston, FL
*                    All rights reserved. Protected by international copyright laws.
*                                        http://www.micrium.com
*
*********************************************************************************************************
* Licensing Terms:
*
*   uC/OS is professional-grade software that is provided by Micrium in source form for evaluation only.
*   The evaluation period for the software is 45 days.  If you would like to use the software beyond this
*   period, you must contact Micrium for a license.
*   Micrium offers four license types:
*
*        Educational: This FREE license entitles its bearer to use the licensed source code in academic
*                     projects ONLY.
*
*        Maker:       This FREE license entitles its bearer to use the licensed source code in personal
*                     (non-commercial) projects ONLY.
*
*        Startup:     This FREE license entitles its bearer to use the licensed source code in
*                     commercial projects only if used by pre-revenue startups or startups meeting
*                     certain criteria. Contact sales@micrium.com to learn if you qualify.
*
*        Commercial:  This license type is required for any commercial entity. Contact sales@micrium.com
*                     to discuss licensing options.
*
*   Micrium reserves the right to refuse or revoke FREE software licenses for any reason.  Unlicensed use
*   of the software beyond the 45-day evaluation period is NOT allowed, and the software may NOT be
*   redistributed without written authorization from Micrium.  If you would like additional information
*   on licensing, you can contact info@micrium.com.
*
*   Please help us continue to provide the Embedded community with the finest software available.
*
*   Your honesty is greatly appreciated.
*********************************************************************************************************
* Documentation:
*   You can find user manuals, API references, release notes and more at: https://doc.micrium.com
*********************************************************************************************************
* Technical Support:
*   Support is available for commercially licensed users of Micrium's software.  For additional
*   information on support, you can contact info@micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                         BSP OPTIONS DEFINES
*
* File : bsp_opt_def.h
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _BSP_OPT_DEF_H_
#define  _BSP_OPT_DEF_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           High Frequency Clock Selection
*********************************************************************************************************
*/

#define  BSP_HF_CLK_DEFAULT                     0u
#define  BSP_HF_CLK_HFRCO                       1u
#define  BSP_HF_CLK_HFXO                        2u
#define  BSP_HF_CLK_DPLL_48MHZ                  3u

/*
*********************************************************************************************************
*                                           Low Frequency Clock Selection
*********************************************************************************************************
*/

#define  BSP_LF_CLK_DEFAULT                     0u
#define  BSP_LF_CLK_LFRCO                       1u
#define  BSP_LF_CLK_LFXO                        2u
#define  BSP_LF_CLK_ULFRCO                      3u

/*
*********************************************************************************************************
*                                           EMU DCDC Mode Selection
*********************************************************************************************************
*/

#define  BSP_DCDC_BYPASS                        0u
#define  BSP_DCDC_DVDD                          1u
#define  BSP_DCDC_OFF                           2u

/*
*********************************************************************************************************
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of bsp opt def module include.                  */
