/***************************************************************************//**
 * @file displayconfigapp.h
 * @brief Display application specific configuration file.
 * @version 3.20.5
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef _DISPLAY_CONFIG_APP_H_
#define _DISPLAY_CONFIG_APP_H_

/* Include pixel matrix allocation support. */
#define PIXEL_MATRIX_ALLOC_SUPPORT

/* Enable allocation of pixel matrices from the static pixel matrix pool.
 *    NOTE:
 *    The allocator does not support free'ing pixel matrices. It allocates
 *    continuosly from the static pool without keeping track of the sizes of
 *    old allocations. I.e. this is a one-shot allocator, and the  user should
 *    allocate buffers once at the beginning of the program.
 */
#define USE_STATIC_PIXEL_MATRIX_POOL

/* Specify the size of the static pixel matrix pool. For the weatherstation demo
 *    we need one pixel matrix (framebuffer) covering the whole display.
 */
#define PIXEL_MATRIX_POOL_SIZE   (DISPLAY0_HEIGHT * DISPLAY0_WIDTH / 8)

#endif /* _DISPLAY_CONFIG_APP_H_ */
