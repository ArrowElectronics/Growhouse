/* Copyright (c) 2014-2015, The Linux Foundation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*    * Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*    * Redistributions in binary form must reproduce the above
*      copyright notice, this list of conditions and the following
*      disclaimer in the documentation and/or other materials provided
*      with the distribution.
*    * Neither the name of The Linux Foundation nor the names of its
*      contributors may be used to endorse or promote products derived
*      from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
* ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
* IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _PANEL_HX8379C_FWVGA_VIDEO_H_
#define _PANEL_HX8379C_FWVGA_VIDEO_H_
/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include "panel.h"

/*---------------------------------------------------------------------------*/
/* Panel configuration                                                       */
/*---------------------------------------------------------------------------*/
static struct panel_config hx8379c_fwvga_video_panel_data = {
	"qcom,mdss_dsi_hx8379c_fwvga_video", "dsi:0:", "qcom,mdss-dsi-panel",
	10, 0, "DISPLAY_1", 0, 0, 60, 0, 0, 0, 1, 7000, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel resolution                                                          */
/*---------------------------------------------------------------------------*/
static struct panel_resolution hx8379c_fwvga_video_panel_res = {
	480, 854, 60, 60, 60, 0, 6, 6, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel color information                                                   */
/*---------------------------------------------------------------------------*/
static struct color_info hx8379c_fwvga_video_color = {
	24, 0, 0xff, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel on/off command information                                          */
/*---------------------------------------------------------------------------*/
static char hx8379c_fwvga_video_on_cmd0[] = {
	0x04, 0x00, 0x39, 0xC0,
	0xB9, 0xFF, 0x83, 0x79,
};

static char hx8379c_fwvga_video_on_cmd1[] = {
	0x11, 0x00, 0x39, 0xC0,
	0xB1, 0x44, 0x1C, 0x1C,
	0x31, 0x31, 0x50, 0xD0,
	0xEE, 0x54, 0x80, 0x38,
	0x38, 0xF8, 0x32, 0x22,
	0x22, 0xFF, 0xFF, 0xFF,
};

static char hx8379c_fwvga_video_on_cmd2[] = {
	0x0A, 0x00, 0x39, 0xC0,
	0xB2, 0x80, 0xFE, 0x0B,
	0x04, 0x00, 0x50, 0x11,
	0x42, 0x15, 0xFF, 0xFF,
};

static char hx8379c_fwvga_video_on_cmd3[] = {
	0x0B, 0x00, 0x39, 0xC0,
	0xB4, 0x69, 0x6A, 0x69,
	0x6A, 0x69, 0x6A, 0x22,
	0x70, 0x23, 0x70, 0xFF,
};

static char hx8379c_fwvga_video_on_cmd4[] = {
	0x0B, 0x00, 0x39, 0xC0,
	0xBA, 0x41, 0x83, 0xA8,
	0x4D, 0xB2, 0x24, 0x00,
	0x00, 0x50, 0x90, 0xFF,
};

static char hx8379c_fwvga_video_on_cmd5[] = {
	0x05, 0x00, 0x39, 0xC0,
	0xC7, 0x00, 0x00, 0x00,
	0xC0, 0xFF, 0xFF, 0xFF,
};

static char hx8379c_fwvga_video_on_cmd6[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xCC, 0x02, 0xFF, 0xFF,
};

static char hx8379c_fwvga_video_on_cmd7[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xD2, 0x77, 0xFF, 0xFF,
};

static char hx8379c_fwvga_video_on_cmd8[] = {
	0x1E, 0x00, 0x39, 0xC0,
	0xD3, 0x00, 0x07, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x32, 0x10, 0x03, 0x00,
	0x03, 0x03, 0x60, 0x03,
	0x60, 0x00, 0x08, 0x00,
	0x08, 0x45, 0x44, 0x08,
	0x08, 0x37, 0x08, 0x08,
	0x37, 0x09, 0xFF, 0xFF,
};

static char hx8379c_fwvga_video_on_cmd9[] = {
	0x23, 0x00, 0x39, 0xC0,
	0xD5, 0x18, 0x18, 0x19,
	0x19, 0x18, 0x18, 0x20,
	0x21, 0x24, 0x25, 0x18,
	0x18, 0x18, 0x18, 0x00,
	0x01, 0x04, 0x05, 0x02,
	0x03, 0x06, 0x07, 0x18,
	0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18,
	0x18, 0x00, 0x00, 0xFF,
};

static char hx8379c_fwvga_video_on_cmd10[] = {
	0x21, 0x00, 0x39, 0xC0,
	0xD6, 0x18, 0x18, 0x18,
	0x18, 0x19, 0x19, 0x25,
	0x24, 0x21, 0x20, 0x18,
	0x18, 0x18, 0x18, 0x05,
	0x04, 0x01, 0x00, 0x03,
	0x02, 0x07, 0x06, 0x18,
	0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18,
	0x18, 0xFF, 0xFF, 0xFF,
};

static char hx8379c_fwvga_video_on_cmd11[] = {
	0x2B, 0x00, 0x39, 0xC0,
	0xE0, 0x00, 0x04, 0x0B,
	0x2F, 0x39, 0x3F, 0x21,
	0x46, 0x07, 0x0A, 0x0C,
	0x17, 0x0F, 0x13, 0x16,
	0x14, 0x15, 0x07, 0x11,
	0x13, 0x30, 0x00, 0x04,
	0x0B, 0x2F, 0x3A, 0x3F,
	0x21, 0x46, 0x07, 0x0A,
	0x0C, 0x17, 0x0F, 0x14,
	0x16, 0x14, 0x15, 0x07,
	0x11, 0x13, 0x16, 0xFF,
};

static char hx8379c_fwvga_video_on_cmd12[] = {
	0x03, 0x00, 0x39, 0xC0,
	0xB6, 0x4E, 0x4E, 0xFF,
};

static char hx8379c_fwvga_video_on_cmd13[] = {
	0x11, 0x00, 0x05, 0x80
};

static char hx8379c_fwvga_video_on_cmd14[] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd hx8379c_fwvga_video_on_command[] = {
	{0x8, hx8379c_fwvga_video_on_cmd0, 0x00},
	{0x18, hx8379c_fwvga_video_on_cmd1, 0x00},
	{0x10, hx8379c_fwvga_video_on_cmd2, 0x00},
	{0x10, hx8379c_fwvga_video_on_cmd3, 0x00},
	{0x10, hx8379c_fwvga_video_on_cmd4, 0x00},
	{0xc, hx8379c_fwvga_video_on_cmd5, 0x00},
	{0x8, hx8379c_fwvga_video_on_cmd6, 0x00},
	{0x8, hx8379c_fwvga_video_on_cmd7, 0x00},
	{0x24, hx8379c_fwvga_video_on_cmd8, 0x00},
	{0x28, hx8379c_fwvga_video_on_cmd9, 0x00},
	{0x28, hx8379c_fwvga_video_on_cmd10, 0x00},
	{0x30, hx8379c_fwvga_video_on_cmd11, 0x00},
	{0x8, hx8379c_fwvga_video_on_cmd12, 0x00},
	{0x4, hx8379c_fwvga_video_on_cmd13, 0x78},
	{0x4, hx8379c_fwvga_video_on_cmd14, 0x14}
};

#define HX8379C_FWVGA_VIDEO_ON_COMMAND 15


static char hx8379c_fwvga_videooff_cmd0[] = {
	0x28, 0x00, 0x05, 0x80
};

static char hx8379c_fwvga_videooff_cmd1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd hx8379c_fwvga_video_off_command[] = {
	{0x4, hx8379c_fwvga_videooff_cmd0, 0x32},
	{0x4, hx8379c_fwvga_videooff_cmd1, 0x78}
};

#define HX8379C_FWVGA_VIDEO_OFF_COMMAND 2


static struct command_state hx8379c_fwvga_video_state = {
	0, 0
};

/*---------------------------------------------------------------------------*/
/* Command mode panel information                                            */
/*---------------------------------------------------------------------------*/
static struct commandpanel_info hx8379c_fwvga_video_command_panel = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Video mode panel information                                              */
/*---------------------------------------------------------------------------*/
static struct videopanel_info hx8379c_fwvga_video_video_panel = {
	1, 0, 0, 0, 1, 1, 2, 0, 0x9
};

/*---------------------------------------------------------------------------*/
/* Lane configuration                                                        */
/*---------------------------------------------------------------------------*/
static struct lane_configuration hx8379c_fwvga_video_lane_config = {
	2, 0, 1, 1, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel timing                                                              */
/*---------------------------------------------------------------------------*/
static const uint32_t hx8379c_fwvga_video_timings[] = {
	0x7B, 0x21, 0x1A, 0x00, 0x31, 0x2D, 0x1E, 0x23, 0x2B, 0x03, 0x04, 0x00
};

static struct panel_timing hx8379c_fwvga_video_timing_info = {
	0, 4, 0x20, 0x2c
};

/*---------------------------------------------------------------------------*/
/* Panel reset sequence                                                      */
/*---------------------------------------------------------------------------*/
static struct panel_reset_sequence hx8379c_fwvga_video_reset_seq = {
	{1, 0, 1, }, {20, 2, 20, }, 2
};

/*---------------------------------------------------------------------------*/
/* Backlight setting                                                         */
/*---------------------------------------------------------------------------*/
static struct backlight hx8379c_fwvga_video_backlight = {
	1, 1, 4095, 100, 1, "PMIC_8941"
};

#define HX8379C_FWVGA_VIDEO_SIGNATURE 0xFFFF

#endif /*_PANEL_HX8379A_FWVGA_SKUA_VIDEO_H_*/
