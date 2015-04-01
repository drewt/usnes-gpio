/* Copyright 2014-2015 Drew Thoreson
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <linux/input.h>
#include "snes.h"

/*
 * Controller definitions.  You may add as many controllers to this list as
 * you like and usnes will automagically poll them.
 *
 * It is a good idea to share CLOCK/LATCH pins between controllers, to reduce
 * latency differences and the amount of polling that has to be done.  But if
 * you share CLOCK/LATCH pins, they must be shared between ALL controllers,
 * otherwise usnes will fall back to polling them individually.
 */
static struct controller {
	unsigned long keymap[SNES_NR_BUTTONS];
	unsigned char clock;
	unsigned char latch;
	unsigned char data;
} controller[] =
{{
	.keymap = {
		[SNES_R] = KEY_R,
		[SNES_L] = KEY_L,

		[SNES_A] = KEY_A,
		[SNES_B] = KEY_B,
		[SNES_X] = KEY_X,
		[SNES_Y] = KEY_Y,

		[SNES_UP]    = KEY_UP,
		[SNES_DOWN]  = KEY_DOWN,
		[SNES_LEFT]  = KEY_LEFT,
		[SNES_RIGHT] = KEY_RIGHT,

		[SNES_START]  = KEY_ENTER,
		[SNES_SELECT] = KEY_ESC
	},
	.clock = 21,
	.latch = 20,
	.data  = 16
},
/* e.g. second controller with shared CLOCK and LATCH pins: */
/*{
	.keymap = {
		[SNES_R] = KEY_Q,
		[SNES_L] = KEY_W,

		[SNES_A] = KEY_E,
		[SNES_B] = KEY_T,
		[SNES_X] = KEY_U,
		[SNES_Y] = KEY_I,

		[SNES_UP]    = KEY_O,
		[SNES_DOWN]  = KEY_P,
		[SNES_LEFT]  = KEY_S,
		[SNES_RIGHT] = KEY_D,

		[SNES_START]  = KEY_F,
		[SNES_SELECT] = KEY_G
	},
	.clock = 21,
	.latch = 20,
	.data  = 12
}*/
};
#define NR_CONTROLLERS (sizeof(controller)/sizeof(*controller))

/*
 * The delay, in milliseconds, that usnes will wait after polling.
 */
#define POLL_MS 60

#endif
