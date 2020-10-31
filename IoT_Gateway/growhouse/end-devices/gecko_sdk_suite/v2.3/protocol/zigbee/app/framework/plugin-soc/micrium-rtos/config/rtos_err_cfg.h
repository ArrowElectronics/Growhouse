/*
 *********************************************************************************************************
 *
 *                                        RTOS_ERR CONFIGURATION
 *
 * Filename      : rtos_err_cfg.h
 *********************************************************************************************************
 */

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                               MODULE
 *********************************************************************************************************
 *********************************************************************************************************
 */

#ifndef SILABS_RTOS_ERR_CFG_H
#define SILABS_RTOS_ERR_CFG_H

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                             INCLUDE FILES
 *********************************************************************************************************
 *********************************************************************************************************
 */

#include  <common/include/lib_def.h>

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                          INCLUDE FILES NOTE
 *
 * Note(s) : (1) No files including rtos_err.h must be included by this file. This could lead to circular
 *               inclusion problems.
 *********************************************************************************************************
 *********************************************************************************************************
 */

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                               DEFINES
 *
 * Note(s) : (1) RTOS_ERR_CFG_EXT_EN allows to configure whether the RTOS_ERR type contains only an error
 *               code (DEF_DISABLED) or contains more debug information (DEF_ENABLED). If set to
 *               DEF_ENABLED, a string containing the file name and line number where the error has been
 *               set and also the function name, if compiling in C99, will be included in the RTOS_ERR
 *               type. Setting this configuration to DEF_ENABLED may have an impact on performance and
 *               resource usage, it is recommended to set it to DEF_DISABLED once development is
 *               complete.
 *
 *           (2) RTOS_ERR_CFG_STR_EN allows to have strings associated with each error code, in order to
 *               print them. If set to DEF_DISABLED, the error code enum value will be outputted instead.
 *               For example, if set to DEF_ENABLED, it would be possible to print RTOS_ERR_NONE or
 *               RTOS_ERR_INVALID_ARG as a string instead of printing the numerical value associated,
 *               which would be 0 for RTOS_ERR_NONE and higher than 0 for RTOS_ERR_INVALID_ARG.
 *********************************************************************************************************
 *********************************************************************************************************
 */

#define  RTOS_ERR_CFG_EXT_EN                                DEF_DISABLED

#define  RTOS_ERR_CFG_STR_EN                                DEF_DISABLED

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                             MODULE END
 *********************************************************************************************************
 *********************************************************************************************************
 */

#endif /* End of rtos_err_cfg.h module include.                */
