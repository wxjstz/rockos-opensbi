/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ifndef __SERIAL_UART8250_H__
#define __SERIAL_UART8250_H__

#include <sbi/sbi_types.h>

struct uart8250_device {
	volatile char * base;
	u32 in_freq;
	u32 baudrate;
	u32 reg_shift;
	u32 reg_width;
};

void uart8250_putc(struct uart8250_device *dev, char ch);
int uart8250_getc(struct uart8250_device *dev);
int uart8250_init(struct uart8250_device * dev, unsigned long base, u32 in_freq,
		  u32 baudrate, u32 reg_shift, u32 reg_width, u32 reg_offset);

int uart8250_console_init(unsigned long base, u32 in_freq, u32 baudrate,
		  u32 reg_shift, u32 reg_width, u32 reg_offset);

#endif
