/*****************************************************************************
*
* File Name : wm_spi_codec_demo.c
*
* Description: ntp demo function
*
* Copyright (c) 2014 Winner Micro Electronic Design Co., Ltd.
* All rights reserved.
*
* Author : dave
*
* Date : 2014-10-28
*****************************************************************************/
#include "wm_include.h"
#include "wm_demo.h"
#include "wm_dlna_dmr.h"
#include <string.h>

#if DEMO_DLNA_DMR

#define  SPI_CLK  8   //CTS
#define  SPI_DATO  9   //RTS
#define  SPI_CCS    11
#define  DAT_REQ    3
#define  LED        4
#define  RST        5
#define  RUN_MODE       6


#define SPI_CK(x)  tls_gpio_write(SPI_CLK,x)
#define SPI_DO(x)  tls_gpio_write(SPI_DATO,x)
#define SPI_CS(x)  tls_gpio_write(SPI_CCS,x)

#define LED_ON(x)  tls_gpio_write(LED,x)

#define AUDIO_RST(x)  tls_gpio_write(RST,x)

#define    MUSIC_BUF_MAX_INDX     40
#define    HTTP_CLIENT_BUFFER_SIZE   512
#define    MUSI_BUF_SIZE  (HTTP_CLIENT_BUFFER_SIZE*MUSIC_BUF_MAX_INDX)

static tls_os_queue_t * sd_down_mbox = NULL;
#define UPNP_SD_STK_SIZE  500 //spi write down
OS_STK         sd_down_task_stk[UPNP_SD_STK_SIZE];

u32 SendCnt;
u32 writeCnt;
u32 DownSize;
char  MusicData[MUSI_BUF_SIZE];

#if 0
char MUSIC_DATA[2673] = {
	0xFF, 0xE2, 0x2B, 0x74, 0xD5, 0xFD, 0x00, 0x02, 0x58, 0x2A, 0xFF, 0x20, 0xA1, 0x00, 0x00, 0x83,
	0x30, 0x05, 0xF9, 0x75, 0x40, 0x00, 0x00, 0x01, 0x49, 0x20, 0x00, 0x7F, 0xF9, 0xE8, 0x42, 0x40,
	0x04, 0x30, 0x78, 0x7F, 0x03, 0xF0, 0x00, 0x76, 0x0E, 0xCC, 0x10, 0xFF, 0xE2, 0x26, 0x5D, 0x40,
	0x03, 0xE5, 0xDE, 0x8F, 0xCA, 0x41, 0xFF, 0x81, 0x1C, 0x30, 0xC3, 0x08, 0x43, 0x22, 0x92, 0x00,
	0x08, 0x42, 0x54, 0xD5, 0xBB, 0x50, 0x19, 0xF1, 0x04, 0xDC, 0x42, 0x7B, 0x21, 0x5A, 0x65, 0xF5,
	0x00, 0xAB, 0x06, 0xC0, 0x03, 0xF2, 0x0C, 0x51, 0xDA, 0x89, 0x7D, 0x31, 0x99, 0x1B, 0x44, 0x54,
	0xAD, 0xDF, 0x3D, 0xE8, 0xD2, 0x85, 0x4F, 0x71, 0x18, 0x0B, 0x1A, 0xCD, 0xF6, 0xB3, 0xF2, 0x77,
	0x53, 0x5D, 0xA4, 0xDA, 0x33, 0x7F, 0xBB, 0x73, 0x19, 0x92, 0x66, 0x82, 0xE5, 0x6A, 0x9E, 0x7E,
	0xFB, 0xB3, 0x98, 0xD7, 0x46, 0x3D, 0xF5, 0x9B, 0xD5, 0x08, 0x27, 0x33, 0x3B, 0xFF, 0xF6, 0xA6,
	0xE6, 0xFF, 0xE2, 0x39, 0x74, 0xC4, 0x20, 0x5A, 0x10, 0x72, 0x2F, 0x68, 0xCF, 0xC8, 0xA8, 0x81,
	0x9A, 0x83, 0xBA, 0xDE, 0x05, 0x8A, 0x29, 0x02, 0x7E, 0xED, 0x38, 0xD2, 0x01, 0xD5, 0xBB, 0x4C,
	0xA9, 0xE8, 0xEF, 0x77, 0x46, 0x67, 0x99, 0xF4, 0xFF, 0x6D, 0xFA, 0xBB, 0x4C, 0x3D, 0xDD, 0x8C,
	0x1A, 0xBF, 0xA1, 0x01, 0x00, 0x88, 0x1A, 0x3F, 0xAB, 0x66, 0xFF, 0xFF, 0xBF, 0xFF, 0x51, 0x10,
	0x41, 0x54, 0x0B, 0x06, 0x00, 0xC9, 0x88, 0xC2, 0x6A, 0x30, 0x9A, 0x8E, 0x27, 0xC3, 0x79, 0xA1,
	0x8C, 0xAF, 0x92, 0x0D, 0x06, 0xE0, 0x52, 0x76, 0x6F, 0x61, 0x10, 0x2C, 0x02, 0xA1, 0xE4, 0xC5,
	0x7F, 0x1A, 0x21, 0x53, 0x4F, 0xB7, 0xC8, 0xD3, 0x08, 0x9C, 0xC1, 0xC0, 0x27, 0x6B, 0x34, 0x84,
	0xA9, 0xDE, 0x87, 0x32, 0xB3, 0xB1, 0x8F, 0x74, 0x66, 0xFB, 0x76, 0xD0, 0x9A, 0x32, 0x3D, 0x02,
	0x87, 0x27, 0xE7, 0x46, 0xDE, 0x46, 0x74, 0x63, 0xBF, 0xB6, 0xBA, 0x4C, 0xBF, 0xFF, 0xBF, 0xB6,
	0xFF, 0xEC, 0xA2, 0xB4, 0x47, 0x5F, 0xFF, 0x7D, 0xAE, 0xFF, 0xEE, 0x92, 0x7F, 0xD2, 0x45, 0x79,
	0x05, 0x5F, 0xC8, 0xEA, 0x3C, 0x93, 0xEF, 0xBB, 0xFF, 0xF0, 0xB9, 0x8E, 0xA4, 0x1A, 0xF1, 0x45,
	0x1B, 0x16, 0x09, 0xB8, 0xAC, 0x30, 0xC0, 0xB7, 0x08, 0xD2, 0x63, 0xB4, 0x7B, 0x09, 0x93, 0x2D,
	0xD1, 0xB3, 0x74, 0xF9, 0xB3, 0xFD, 0x26, 0x6F, 0xD6, 0x87, 0xAF, 0x37, 0x35, 0x5A, 0x28, 0x6F,
	0xE9, 0xFF, 0xBA, 0xB7, 0xFF, 0xFF, 0x6F, 0x4F, 0xFF, 0xFF, 0xE2, 0x2B, 0x64, 0x55, 0x44, 0x2D,
	0x08, 0xD5, 0x9D, 0x7B, 0xC3, 0xE6, 0x88, 0x00, 0x0A, 0x00, 0x02, 0xF3, 0x5F, 0xC0, 0x00, 0x00,
	0xFB, 0x7E, 0x87, 0x6F, 0xB5, 0x7F, 0xFF, 0x98, 0xDF, 0xFF, 0xDE, 0xCA, 0xBB, 0xD6, 0x9A, 0x7B,
	0x7F, 0xFF, 0x8F, 0xFF, 0x67, 0x3D, 0xFF, 0xCB, 0x1D, 0x40, 0x32, 0x60, 0x0C, 0x46, 0x05, 0x96,
	0x50, 0x4A, 0x71, 0xA8, 0x62, 0x39, 0x7E, 0x3F, 0xAD, 0x9D, 0xEF, 0xFF, 0xC7, 0xB3, 0xFF, 0xFF,
	0xFF, 0xFF, 0xD8, 0x25, 0xF7, 0x7F, 0xFF, 0xF2, 0x02, 0x88, 0xFF, 0x7A, 0x42, 0xF2, 0x5F, 0xFD,
	0x6C, 0x4F, 0x04, 0xC2, 0x22, 0x89, 0xB9, 0x28, 0x9D, 0x54, 0x0C, 0x33, 0xE2, 0x17, 0x02, 0xB0,
	0x30, 0x16, 0xD4, 0x56, 0xA6, 0xFE, 0x2F, 0xAD, 0xFC, 0xEF, 0xC3, 0x6E, 0x57, 0x2A, 0x97, 0x54,
	0x3F, 0xD5, 0x95, 0x77, 0xF6, 0x74, 0xFA, 0x3F, 0x2C, 0x15, 0xEC, 0xF2, 0x18, 0x68, 0xAA, 0xEC,
	0xFF, 0x70, 0x0D, 0x1F, 0xF4, 0xA2, 0x1A, 0xFF, 0xE2, 0x90, 0xFF, 0xE2, 0x39, 0x64, 0x7A, 0xCF,
	0x38, 0x07, 0x14, 0x45, 0x7B, 0xC7, 0x1C, 0x42, 0x12, 0x0B, 0xE8, 0x02, 0xFB, 0x8E, 0x00, 0x00,
	0x00, 0x20, 0x06, 0x25, 0x5E, 0x5A, 0x69, 0x47, 0x55, 0x9E, 0x89, 0x6E, 0x55, 0xEF, 0xAC, 0xF7,
	0x3A, 0x73, 0xCF, 0x91, 0xF5, 0xA1, 0xF8, 0x6D, 0xFC, 0xB6, 0x5D, 0xFF, 0xD7, 0x96, 0x14, 0xF9,
	0x7F, 0xAD, 0x5F, 0xE5, 0x33, 0xD1, 0x8E, 0x6F, 0xFC, 0x1A, 0x4F, 0xF9, 0xDF, 0xFF, 0xE2, 0x51,
	0x86, 0x7D, 0x54, 0xB2, 0x65, 0xB2, 0x4B, 0x68, 0xE6, 0x0C, 0x1E, 0x6B, 0xC2, 0x04, 0x13, 0x83,
	0x39, 0x6C, 0x55, 0xF5, 0x3D, 0xCA, 0xCE, 0x76, 0x31, 0xFC, 0x42, 0xF4, 0xF6, 0x2D, 0xB2, 0xE1,
	0x24, 0xFB, 0xE4, 0xBF, 0xDD, 0xC5, 0xBD, 0x5F, 0xA0, 0x40, 0xFF, 0xE4, 0xB5, 0x17, 0xF6, 0xFD,
	0x69, 0xFF, 0xA1, 0x96, 0x59, 0xFF, 0x41, 0x87, 0x28, 0x90, 0x60, 0x26, 0xBF, 0x74, 0xDB, 0x0C,
	0x5D, 0xE8, 0x0A, 0x95, 0xAF, 0x6C, 0xD2, 0x6C, 0x4F, 0x47, 0x50, 0xE3, 0x1D, 0x00, 0x3F, 0x51,
	0x2D, 0x60, 0xEB, 0x3D, 0x38, 0x43, 0xFA, 0x1D, 0x92, 0xFB, 0x3F, 0x48, 0x6B, 0xFA, 0xDD, 0x28,
	0x2F, 0xFF, 0xE1, 0x93, 0x4E, 0xFA, 0xE6, 0x99, 0xAB, 0xFF, 0x49, 0xB5, 0x8B, 0xFC, 0x68, 0x59,
	0x6D, 0x26, 0xED, 0x1F, 0x39, 0xCB, 0x79, 0x36, 0x72, 0xB6, 0xE6, 0x26, 0xED, 0x08, 0xEA, 0x68,
	0xCD, 0x53, 0xED, 0xAA, 0xBA, 0x0B, 0xF4, 0xD0, 0x78, 0x0A, 0x18, 0xF5, 0xE8, 0x35, 0xFE, 0x56,
	0x71, 0xDF, 0xFF, 0xE2, 0x2B, 0x64, 0xC2, 0xC9, 0x90, 0x06, 0xD8, 0x21, 0x7F, 0xC7, 0x08, 0xE2,
	0x00, 0x0D, 0x18, 0x06, 0xFF, 0xB6, 0x08, 0x00, 0x00, 0xEA, 0xA7, 0x88, 0x01, 0x09, 0xDB, 0xCC,
	0x04, 0x9E, 0xFF, 0x26, 0x6F, 0x7F, 0xB7, 0xB5, 0x1A, 0x80, 0x94, 0xEB, 0x50, 0xF9, 0x2C, 0xBF,
	0x94, 0x7C, 0xC3, 0x11, 0xFF, 0x96, 0x62, 0x1F, 0xFE, 0x17, 0x2A, 0xE4, 0xBF, 0x51, 0xCF, 0xAC,
	0x24, 0xF5, 0x2A, 0xE5, 0x39, 0x28, 0xE8, 0x8C, 0x56, 0x67, 0x33, 0x89, 0x3F, 0xBE, 0x0D, 0xAB,
	0xFD, 0x0E, 0x09, 0xF5, 0xE8, 0x17, 0xF4, 0x47, 0x38, 0xDF, 0xFB, 0xBA, 0x11, 0xBD, 0x3A, 0xF5,
	0x90, 0xCE, 0xAD, 0xFF, 0xEA, 0x72, 0xFF, 0xFE, 0xBF, 0xD9, 0x7F, 0xA2, 0x32, 0xFF, 0xA5, 0x50,
	0x97, 0x4F, 0xF7, 0xFD, 0xD5, 0x3B, 0xB8, 0x36, 0xAB, 0xFF, 0x3E, 0x03, 0xFF, 0x2E, 0xCD, 0x7D,
	0x34, 0xAA, 0xA1, 0xEA, 0x1E, 0x59, 0x13, 0x65, 0x26, 0x6D, 0x26, 0xEE, 0x1A, 0xE6, 0x21, 0x7C,
	0x44, 0xEA, 0x29, 0xFF, 0xE2, 0x39, 0x64, 0xB9, 0x8E, 0x9E, 0x09, 0xA4, 0x8B, 0x75, 0xC3, 0x3C,
	0xE5, 0x26, 0x11, 0x12, 0xCE, 0xE7, 0x36, 0x28, 0x84, 0x90, 0x84, 0x54, 0xDC, 0xA2, 0xF1, 0x9C,
	0xAC, 0xDF, 0xA7, 0xD1, 0x3A, 0x1C, 0x4C, 0x08, 0x45, 0xBF, 0x8D, 0xDF, 0xF3, 0x58, 0x85, 0xBF,
	0xD5, 0x4C, 0xC3, 0x5B, 0xE4, 0x7F, 0xE6, 0x79, 0x3F, 0xFD, 0x54, 0x43, 0xFF, 0xFF, 0xFE, 0xE9,
	0xF8, 0xD6, 0xFC, 0x50, 0xBB, 0xB5, 0xBB, 0x59, 0x76, 0xE7, 0x9D, 0xFF, 0xD8, 0x71, 0x9F, 0xD0,
	0x90, 0xDA, 0x3D, 0x5F, 0xA8, 0xA0, 0x7C, 0x58, 0x01, 0x66, 0x25, 0x6D, 0x40, 0x01, 0xFD, 0xD2,
	0x58, 0x3E, 0xEF, 0xC9, 0xA9, 0xBF, 0x70, 0x16, 0xFE, 0x3F, 0xE8, 0xB0, 0x6C, 0x9E, 0x34, 0x40,
	0x5F, 0xB1, 0x97, 0x16, 0x71, 0x8F, 0xCA, 0x7F, 0xFB, 0xB9, 0x71, 0x9F, 0x9A, 0x31, 0xC9, 0x77,
	0xF2, 0xAC, 0x4F, 0xDF, 0xE8, 0xFB, 0xFC, 0x44, 0x1B, 0xA8, 0x8B, 0xE5, 0x56, 0x49, 0xB7, 0x86,
	0xA4, 0x88, 0x81, 0xC9, 0xA1, 0x50, 0x10, 0xC8, 0x10, 0x9E, 0x2E, 0x89, 0x71, 0x4B, 0x98, 0x68,
	0x5B, 0xE7, 0x9E, 0x10, 0x91, 0xCE, 0xB5, 0x44, 0x87, 0xFC, 0xDB, 0x1E, 0xDF, 0xE0, 0x62, 0x2A,
	0x03, 0xFA, 0x1A, 0xFF, 0x67, 0x2B, 0x7F, 0xDF, 0xC3, 0xBB, 0xF4, 0x23, 0xD1, 0x95, 0xFD, 0xEA,
	0x1E, 0xDA, 0xCD, 0xF9, 0x8B, 0x53, 0x77, 0xFE, 0xA5, 0xFE, 0xCD, 0xED, 0x77, 0xD9, 0x8D, 0x20,
	0x7C, 0xAA, 0xC2, 0x9E, 0x8F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE2, 0x2B, 0x74, 0x8C,
	0xD9, 0xCD, 0x09, 0x39, 0x13, 0x77, 0xC3, 0x35, 0x45, 0x38, 0x0C, 0xB8, 0xDA, 0xE8, 0xE4, 0x10,
	0x84, 0x48, 0xFF, 0x10, 0x90, 0x62, 0x72, 0xDE, 0x3F, 0x5C, 0x48, 0x67, 0xF8, 0x62, 0x55, 0xBF,
	0x41, 0xFF, 0xA8, 0x83, 0xD6, 0xA3, 0xC2, 0xFF, 0x94, 0xE4, 0x5D, 0x9A, 0x12, 0x97, 0xFC, 0xBF,
	0xFF, 0x2E, 0x7D, 0xF9, 0x5F, 0xC6, 0xB5, 0xBC, 0xDB, 0xFD, 0xCF, 0x09, 0x53, 0xFF, 0x55, 0x5E,
	0x8B, 0xB5, 0xAE, 0xDF, 0x63, 0x25, 0x09, 0x8A, 0x82, 0xA8, 0x14, 0x34, 0xFC, 0xD5, 0x60, 0x56,
	0x49, 0x27, 0x07, 0xFF, 0x35, 0x1A, 0xDB, 0x85, 0xA9, 0x43, 0x3D, 0xD2, 0xCD, 0xEE, 0xF4, 0xCA,
	0xA3, 0x48, 0x0A, 0xD1, 0x8B, 0xF2, 0xB0, 0xBB, 0xE4, 0x6E, 0x5F, 0xD4, 0x21, 0x22, 0xBF, 0xD6,
	0x5A, 0x02, 0xEB, 0x77, 0x61, 0x0F, 0x27, 0x41, 0x90, 0xC1, 0x7F, 0x80, 0xF5, 0x8D, 0xF4, 0xFE,
	0x64, 0x5F, 0xFD, 0x6F, 0xFF, 0xFE, 0x40, 0xF3, 0x7F, 0xE8, 0xFF, 0xF4, 0xFF, 0xE2, 0x39, 0x64,
	0xAE, 0x3E, 0xCA, 0x0A, 0x20, 0xCD, 0x71, 0xAF, 0x34, 0xE2, 0x36, 0x0E, 0xA8, 0xB2, 0xE7, 0x66,
	0x09, 0x44, 0x28, 0xBC, 0x48, 0x40, 0x81, 0x04, 0x93, 0x24, 0x76, 0xD1, 0xEF, 0xAE, 0x0D, 0xC2,
	0xB9, 0x21, 0x17, 0x8D, 0xAB, 0x7D, 0x58, 0x2F, 0x4E, 0x8F, 0xF9, 0x14, 0xF3, 0x7F, 0xA5, 0x77,
	0xFE, 0xBF, 0xA2, 0x72, 0x7F, 0xBF, 0xB7, 0x4B, 0xFF, 0xDA, 0xBE, 0x8E, 0x5D, 0xBE, 0x95, 0xFF,
	0xDA, 0x44, 0x4F, 0xFF, 0xFA, 0xB3, 0xD1, 0x68, 0xDF, 0x4F, 0xFF, 0xBF, 0xD8, 0x1D, 0xFF, 0xA9,
	0xEE, 0x7F, 0xFE, 0xE4, 0x86, 0xA3, 0xE3, 0xC1, 0xA0, 0x4A, 0x49, 0x44, 0x86, 0xC9, 0x28, 0x14,
	0x47, 0x67, 0x30, 0xC1, 0xD9, 0x60, 0x9C, 0xB2, 0x44, 0xA7, 0xD6, 0xD2, 0x08, 0xBF, 0xD4, 0x5F,
	0xFF, 0x99, 0x3B, 0x7D, 0x7C, 0x67, 0xEA, 0x29, 0x06, 0x8B, 0x4E, 0x4F, 0xF9, 0x5D, 0x0F, 0x6F,
	0xA2, 0x35, 0xBE, 0x9F, 0xE7, 0x29, 0xAF, 0x9C, 0x38, 0x24, 0x84, 0xCA, 0x47, 0xB7, 0xEB, 0x7F,
	0xFD, 0xBF, 0xE9, 0xFF, 0xEA, 0x09, 0xFF, 0xFF, 0xD3, 0xEA, 0xEE, 0xF7, 0x43, 0x7F, 0xFF, 0xF7,
	0x0E, 0x4F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x0C, 0x55, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE2, 0x2B, 0x64, 0x42, 0x2D, 0xFF, 0x08, 0x34, 0x8B, 0x71, 0xAF,
	0x3C, 0xA2, 0x26, 0x10, 0x2B, 0x3A, 0xE7, 0x66, 0x10, 0x84, 0x94, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFE, 0x01, 0x05, 0x14, 0x75, 0xD0, 0x00, 0xEA, 0x43, 0x75, 0x00, 0x82, 0x36, 0x8D, 0x43, 0x7F,
	0xEB, 0x37, 0xFC, 0x97, 0xCE, 0x1E, 0x04, 0x4E, 0x42, 0x5F, 0xFF, 0x2F, 0xF3, 0xFE, 0x8A, 0x9F,
	0x56, 0x4D, 0xB7, 0xD4, 0x57, 0xE5, 0x9F, 0xDA, 0x61, 0xFD, 0x5F, 0x95, 0x25, 0xD9, 0xC5, 0x5F,
	0x53, 0xEA, 0x3D, 0xEF, 0xF1, 0x08, 0x24, 0x24, 0x9B, 0x49, 0xC6, 0xC3, 0x67, 0x12, 0x35, 0xB4,
	0x09, 0x8B, 0x00, 0x96, 0x9C, 0x36, 0x6E, 0x1E, 0xF4, 0x7F, 0x9D, 0x21, 0x15, 0x3F, 0xEA, 0x6B,
	0x59, 0x29, 0x98, 0x29, 0x27, 0x98, 0xDB, 0xFD, 0x47, 0x92, 0xAF, 0xFA, 0x96, 0xCF, 0xEB, 0xAD,
	0x37, 0xFF, 0x59, 0xBC, 0x90, 0x4E, 0x2E, 0xD1, 0xBF, 0xF4, 0x5F, 0xDD, 0x5B, 0xFF, 0x4F, 0xFD,
	0x91, 0x55, 0xFF, 0xFF, 0x98, 0xFF, 0xE2, 0x39, 0x74, 0x7C, 0x58, 0xF6, 0x10, 0x3E, 0x07, 0x71,
	0xAF, 0x2C, 0xE2, 0x5A, 0x8D, 0x81, 0x9E, 0xE4, 0xE4, 0x11, 0xC4, 0x6C, 0x7D, 0x36, 0xFA, 0x5F,
	0xFF, 0xFF, 0xC7, 0x9B, 0xFE, 0x43, 0xFF, 0xF9, 0x50, 0x4C, 0x66, 0x4C, 0xD8, 0xFF, 0xC1, 0x98,
	0x6F, 0x56, 0xD0, 0x77, 0xAB, 0xFD, 0x5D, 0xFF, 0xC8, 0xB7, 0xA9, 0x40, 0xD3, 0xA1, 0x27, 0xEA,
	0x01, 0x17, 0x77, 0x3F, 0xDC, 0x51, 0xFF, 0x5A, 0x7F, 0xAF, 0xB9, 0xDC, 0x92, 0x9B, 0xEB, 0x0A,
	0xFF, 0x50, 0x90, 0x4F, 0xB5, 0x27, 0x1F, 0xEE, 0xD1, 0x22, 0xDF, 0xFB, 0xFF, 0xFF, 0x91, 0x80,
	0x16, 0x59, 0x24, 0xDC, 0x15, 0x9A, 0x8D, 0x5A, 0x84, 0x98, 0xAC, 0xF9, 0x56, 0xB1, 0x52, 0x25,
	0xB1, 0xA7, 0xBA, 0xCB, 0x48, 0x6C, 0xEE, 0xAA, 0x99, 0x46, 0x3B, 0xAD, 0x04, 0x93, 0x02, 0x29,
	0x03, 0xAB, 0x7F, 0xEB, 0x5F, 0xFC, 0x73, 0x7F, 0x41, 0x23, 0xFF, 0xEF, 0x88, 0x81, 0x07, 0xB5,
	0xFE, 0x83, 0xFE, 0x8D, 0x5F, 0xCD, 0xA5, 0x3F, 0xA4, 0xF3, 0xBD, 0x8C, 0xFF, 0x2A, 0xF7, 0x75,
	0xB1, 0xEC, 0xB8, 0x5F, 0xFF, 0xD4, 0x18, 0x3A, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0xFF, 0xE2, 0x2B,
	0x64, 0x75, 0x15, 0xFF, 0x09, 0x51, 0x9D, 0x6F, 0xAF, 0x1D, 0xA7, 0x4A, 0x0D, 0xA0, 0xDE, 0xDF,
	0x3E, 0x28, 0x8E, 0x48, 0x55, 0xCD, 0x59, 0xAD, 0x1B, 0xBA, 0xB7, 0x50, 0x16, 0x8F, 0xDB, 0x43,
	0x6A, 0x4F, 0xA8, 0xE6, 0x26, 0x9E, 0x7F, 0xD4, 0xAE, 0x8E, 0xFF, 0xE6, 0x54, 0xFF, 0xFF, 0xD8,
	0xD6, 0xFF, 0xF8, 0xC6, 0x3F, 0x58, 0x7B, 0x69, 0x02, 0x5D, 0x7F, 0x1F, 0x25, 0xFC, 0x0A, 0x9C,
	0xAD, 0x6C, 0x3E, 0xCF, 0xF4, 0xBD, 0xF8, 0xA6, 0xD8, 0xF6, 0x7D, 0xDF, 0xAC, 0x3E, 0x28, 0x41,
	0x66, 0xD2, 0x51, 0xB1, 0xFB, 0x37, 0xEB, 0x01, 0xD0, 0x8C, 0x2B, 0x57, 0x5D, 0x2B, 0x08, 0x20,
	0xB6, 0xBC, 0xBD, 0xA6, 0xB7, 0xFE, 0x2E, 0xAD, 0x89, 0xBD, 0xE3, 0x7E, 0xD5, 0xD6, 0x69, 0x49,
	0xDE, 0xB1, 0x18, 0x23, 0x86, 0x03, 0x6B, 0xA6, 0xB7, 0xCD, 0x5E, 0xE6, 0xA2, 0xB3, 0x4D, 0x37,
	0xFA, 0x9B, 0xE9, 0x7E, 0x75, 0x14, 0xF2, 0x21, 0x10, 0xB8, 0x25, 0x6F, 0xFA, 0x8F, 0xFF, 0xE2,
	0x39, 0x64, 0x3B, 0x91, 0xF8, 0x0F, 0xA0, 0xD1, 0x6B, 0xC3, 0x29, 0xA5, 0x36, 0x0F, 0x31, 0x9A,
	0xDB, 0x86, 0x28, 0x84, 0x6C, 0x3B, 0x0E, 0xFE, 0xA1, 0xE4, 0xBF, 0x48, 0x69, 0xD5, 0xC1, 0x68,
	0x8B, 0xF7, 0xE2, 0x50, 0x17, 0xFF, 0xFF, 0xF5, 0x84, 0xB0, 0x25, 0x38, 0xAD, 0xD6, 0x0E, 0x8F,
	0xB4, 0x30, 0x10, 0xAC, 0xB5, 0xB2, 0x29, 0x79, 0x7F, 0x45, 0x2F, 0xE5, 0x6F, 0xA9, 0x43, 0x0A,
	0xBA, 0x2F, 0xF9, 0xAC, 0xFF, 0xA9, 0x5B, 0xD7, 0x9B, 0xFF, 0xD6, 0xC1, 0x9D, 0xDF, 0x2B, 0xEB,
	0x4D, 0x43, 0xC3, 0xBF, 0xBC, 0xB1, 0xD3, 0xD6, 0x07, 0x38, 0x77, 0x91, 0x13, 0x4A, 0x91, 0xE3,
	0xDD, 0xC3, 0x4B, 0xFF, 0x3D, 0xFF, 0xFA, 0x8F, 0x11, 0x05, 0xDB, 0xBE, 0xBB, 0x61, 0xA8, 0x81,
	0xD3, 0xFF, 0x4E, 0x99, 0xAB, 0x58, 0xF1, 0xA9, 0x28, 0x81, 0x22, 0x0A, 0x86, 0xF1, 0xF0, 0xB8,
	0xEB, 0xB6, 0xF5, 0xF3, 0x73, 0x6B, 0x62, 0xBD, 0x91, 0xA5, 0x09, 0x2D, 0xC4, 0xF4, 0x5D, 0x47,
	0x61, 0x07, 0x2C, 0x05, 0x38, 0xF1, 0x0C, 0x20, 0x4C, 0x88, 0xB0, 0xCA, 0x24, 0x65, 0xC1, 0x1E,
	0xD1, 0x3E, 0x1B, 0x57, 0x47, 0x45, 0x52, 0xF2, 0x75, 0x8B, 0xD6, 0x3F, 0x4E, 0xFD, 0xA7, 0xF3,
	0xCB, 0x53, 0x26, 0x4E, 0xC3, 0xF4, 0xBF, 0x7D, 0x6B, 0x9B, 0x4B, 0xF5, 0x99, 0x6A, 0x06, 0xDC,
	0x7D, 0x11, 0xF1, 0x24, 0x72, 0x1A, 0x48, 0x25, 0x43, 0x35, 0x11, 0x99, 0x15, 0x16, 0x12, 0x1A,
	0xFF, 0xFC, 0x54, 0x5B, 0x8A, 0x7F, 0xFF, 0xE2, 0x2B, 0x64, 0x0C, 0x40, 0xFF, 0x0A, 0x48, 0xCF,
	0x65, 0x9B, 0x21, 0xE7, 0x36, 0x0F, 0xF1, 0x9E, 0xCF, 0x66, 0x10, 0x84, 0x6C, 0x58, 0xA3, 0x7F,
	0xF2, 0xA2, 0xA2, 0xDF, 0xFF, 0xFF, 0xF1, 0x66, 0x6B, 0x14, 0x25, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFE, 0x3D, 0x25, 0x25, 0x0C, 0x50, 0x00, 0xFF, 0xF6, 0x31, 0x43, 0x05, 0x04, 0x14, 0x26,
	0x28, 0xEC, 0x6D, 0x11, 0x51, 0x76, 0x72, 0xBB, 0x6E, 0x7F, 0xB3, 0x4D, 0x70, 0xB4, 0x52, 0x22,
	0x0C, 0x24, 0x0C, 0x11, 0x35, 0x25, 0x21, 0x34, 0x0E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE2, 0x39, 0x74, 0x51, 0xCD, 0xE7, 0x10, 0x3C,
	0xD1, 0x2B, 0xDB, 0x2D, 0xEC, 0x36, 0x09, 0xC8, 0xF2, 0x3C, 0xE4, 0x11, 0x4C, 0x49, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0xE2, 0x2B, 0x54, 0xDD, 0x33, 0xFF, 0x0E, 0xE0, 0x01, 0x2E, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA8,
	0x02, 0x5C, 0xAC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

};

#endif
static void spi_write_byte(u8 out)
{
	unsigned char i = 0;
	for (i = 0; i < 8; i++)
	{
		SPI_CK(0);
		SPI_CK(0);
		SPI_CK(0);
		SPI_CK(0);
		SPI_CK(0);
		SPI_CK(0);     //延时用

		if ((out & 0x80) ) SPI_DO(1);
		else			  SPI_DO(0);

		SPI_CK(0);
	//	SPI_CK(0);
	//	SPI_CK(0);
//		SPI_CK(0);
		//SPI_CK(0);     //延时用
		out <<= 1;
		SPI_CK(1);
		SPI_CK(1);//做延时用
		SPI_CK(1);
		SPI_CK(1);
		SPI_CK(1);
		SPI_CK(1);
		SPI_CK(1);

	}


}

static void spi_send_audio(char * buf,int len )
{
	int  i;
	for(i = 0;i<len;i++)
	{
		spi_write_byte(*buf++);
	}
}


static void download_finish_callback(char * buf, int datalen)
{
	int pos = writeCnt%MUSI_BUF_SIZE;
	memcpy(&MusicData[pos], buf, datalen);
	writeCnt += datalen;
}
static void spitocodec(void)
{
	int index;
	int pos;

	if(writeCnt>SendCnt)
	{
		index=SendCnt/HTTP_CLIENT_BUFFER_SIZE;
		index%=MUSIC_BUF_MAX_INDX;
		pos=index*HTTP_CLIENT_BUFFER_SIZE;
		//printf("\ns:%x %x %x ",MusicData[pos],MusicData[pos+1],MusicData[pos+2]);
		SPI_CS(0);
		//tls_os_time_delay(1);
		delay_us(100);
		//printf("writeCnt=%d, SendCnt=%d, pos=%d\n",writeCnt,SendCnt,pos);
		spi_send_audio(MusicData+pos,HTTP_CLIENT_BUFFER_SIZE);
		SPI_CS(1);
		SendCnt+=HTTP_CLIENT_BUFFER_SIZE;
		DownSize=HTTP_CLIENT_BUFFER_SIZE;
		//这个地方应该调用http中的api，启动下载
		tls_dmr_download_data(DownSize, download_finish_callback);
	}
	else
		printf("no cache data!\n");
}

static void codec_isr_callback(void *context)
{
	u16 ret;

	ret = tls_get_gpio_int_flag(DAT_REQ);

	if(ret)
	{
		tls_clr_gpio_int_flag(DAT_REQ);
		tls_os_queue_send(sd_down_mbox, (void *)0, 0);
	}
}


static void codec_isr_init(void)
{
	u16 gpio_pin;

	gpio_pin = DAT_REQ;

	tls_gpio_cfg(gpio_pin, TLS_GPIO_DIR_INPUT, TLS_GPIO_ATTR_PULLHIGH);
	tls_gpio_isr_register(gpio_pin, codec_isr_callback, NULL);
	tls_gpio_int_enable(gpio_pin, TLS_GPIO_INT_TRIG_FALLING_EDGE);
}
static void httpstopdownloadmusic()
{
	tls_gpio_int_disable(DAT_REQ);
}

static void first_download_finish_callback(char * buf, int datalen)
{
	int pos = writeCnt%MUSI_BUF_SIZE;
	memcpy(&MusicData[pos], buf, datalen);
	writeCnt += datalen;

	if(writeCnt<MUSI_BUF_SIZE && datalen > 0)
	{
		tls_dmr_download_data(MUSI_BUF_SIZE, first_download_finish_callback);
		return;
	}

	codec_isr_init();
	AUDIO_RST(0);
	delay_us(200);
	AUDIO_RST(1);
}

static void httpdownloadmusic()
{
	//printf("httpdownloadmusic enter\n");
	httpstopdownloadmusic();
	writeCnt = 0;
	SendCnt = 0;
	tls_dmr_download_data(MUSI_BUF_SIZE, first_download_finish_callback);
}

static float get_grogress(int totlen)
{
	//printf("SendCnt %d totlen %d\n", SendCnt, totlen);
	return ((float)SendCnt) / totlen;
}

static void
sd_down_thread(void *arg)
{
	void * msg;
	while(TRUE)
	{
		tls_os_queue_receive(sd_down_mbox, &msg, 0, 0);
		spitocodec();
	}
}

static void mute_callback(enum dmr_control_type type, const char* channel, int* value)
{
	printf("mute callback : type = %s , channel = %s, value = %d\n", (type == 0 ? "GET" : "SET"), channel, *value);
}
static void volume_callback(enum dmr_control_type type, const char* channel, int* value)
{
	printf("volume callback : type = %s , channel = %s, value = %d\n", (type == 0 ? "GET" : "SET"), channel, *value);
}
static void volumedb_callback(enum dmr_control_type type, const char* channel, int* value)
{
	printf("volumedb callback : type = %s , channel = %s, value = %d\n", (type == 0 ? "GET" : "SET"), channel, *value);
}
static void loudness_callback(enum dmr_control_type type, const char* channel, int* value)
{
	printf("loudness callback : type = %s , channel = %s, value = %d\n", (type == 0 ? "GET" : "SET"), channel, *value);
}
extern u8 *wpa_supplicant_get_mac(void);
int  CreateMediaRender(char * buf)
{
	tls_os_status_t status;
	u8 uuid[17] = {0};
	u8 *mac = wpa_supplicant_get_mac();
	sprintf((char *)uuid, "%02x%02x%02x%02x%02x%02x-dmr",
	mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	tls_dmr_init((char *)uuid, (char *)uuid);

	tls_gpio_cfg(SPI_CLK, TLS_GPIO_DIR_OUTPUT, TLS_GPIO_ATTR_FLOATING);
	tls_gpio_cfg(SPI_DATO, TLS_GPIO_DIR_OUTPUT, TLS_GPIO_ATTR_FLOATING);
	tls_gpio_cfg(SPI_CCS, TLS_GPIO_DIR_OUTPUT, TLS_GPIO_ATTR_PULLHIGH);
	tls_gpio_cfg(DAT_REQ, TLS_GPIO_DIR_INPUT, TLS_GPIO_ATTR_FLOATING);
	tls_gpio_cfg(LED, TLS_GPIO_DIR_OUTPUT, TLS_GPIO_ATTR_PULLHIGH);
	tls_gpio_cfg(RST, TLS_GPIO_DIR_OUTPUT, TLS_GPIO_ATTR_PULLHIGH);

//	tls_gpio_cfg(RUN_MODE, TLS_GPIO_DIR_INPUT, TLS_GPIO_ATTR_FLOATING);

	SPI_CS(1);
	LED_ON(0);

	status = tls_os_queue_create(&sd_down_mbox,64);
	if (status != TLS_OS_SUCCESS) {
		return -1;
	}

	 tls_os_task_create(NULL, NULL,
                       sd_down_thread,
                       NULL,
                       (void *)sd_down_task_stk,
                       UPNP_SD_STK_SIZE * sizeof(u32),
                       DEMO_DMR_TASK_PRIO,//DEMO_DMR_TASK_PRIO,
                       0);

	tls_dmr_set_play_callback(httpdownloadmusic);
	tls_dmr_set_stop_callback(httpstopdownloadmusic);
	tls_dmr_set_seek_callback(httpstopdownloadmusic);
	tls_dmr_set_pause_callback(httpstopdownloadmusic);
	tls_dmr_set_play_progress_callback(get_grogress);

	tls_dmr_set_mute_callback(mute_callback);
	tls_dmr_set_volume_callback(volume_callback);
	tls_dmr_set_volumedb_callback(volumedb_callback);
	tls_dmr_set_loudness_callback(loudness_callback);
	return 0;
}


#endif

