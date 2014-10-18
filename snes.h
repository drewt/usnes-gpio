/* Copyright 2014 Drew Thoreson
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

#ifndef SNES_H_
#define SNES_H_

#include <stdint.h>

typedef uint32_t snes_controller_t;
typedef uint16_t snes_state_t;

/* these serve double-duty as masks as well as symbolic constants */
#define SNES_R      0x10
#define SNES_L      0x20
#define SNES_X      0x40
#define SNES_A      0x80
#define SNES_RIGHT  0x100
#define SNES_LEFT   0x200
#define SNES_DOWN   0x400
#define SNES_UP     0x800
#define SNES_START  0x1000
#define SNES_SELECT 0x2000
#define SNES_Y      0x4000
#define SNES_B      0x8000

/* snes_state_t accessors */
#define snes_r(state)      !(state & SNES_R)
#define snes_l(state)      !(state & SNES_L)
#define snes_x(state)      !(state & SNES_X)
#define snes_a(state)      !(state & SNES_A)
#define snes_right(state)  !(state & SNES_RIGHT)
#define snes_left(state)   !(state & SNES_LEFT)
#define snes_down(state)   !(state & SNES_DOWN)
#define snes_up(state)     !(state & SNES_UP)
#define snes_start(state)  !(state & SNES_START)
#define snes_select(state) !(state & SNES_SELECT)
#define snes_y(state)      !(state & SNES_Y)
#define snes_b(state)      !(state & SNES_B)

/* snes_controller_t accessors */
#define snes_clock(ctl)  (ctl & 0xFF)
#define snes_latch(ctl) ((ctl & 0xFF00) >> 8)
#define snes_data(ctl)  ((ctl & 0xFF0000) >> 16)

/* the "empty" state, where no button is pressed */
#define snes_state_empty() (0xFFFF)

/*
 * returns non-zero if the state of 'btn' (a SNES_* constant) changed between
 * states 'old' and 'new'
 */
#define snes_state_changed(old, new, btn) \
	((old & btn) != (new & btn))

/*
 * returns non-zero if 'btn' (a SNES_* constant) is pressed in state 'state'
 */
#define snes_button_pressed(state, btn) !(state & btn)

/*
 * Open a SNES controller on the given GPIO pins.
 */
snes_controller_t snes_open(unsigned char clock, unsigned char latch,
		unsigned char data);

/*
 * Read the state of a SNES controller.
 */
snes_state_t snes_read(snes_controller_t controller);

/*
 * Check if a controller is connected.
 */
int snes_connected(snes_controller_t controller);

#endif
