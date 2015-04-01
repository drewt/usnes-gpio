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

#ifndef SNES_H_
#define SNES_H_

#include <stdint.h>

/*
 * Flags for snes_read_n.
 */
enum {
	SNES_SHARED_CLOCK = 1, /* the controllers share a CLOCK pin */
	SNES_SHARED_LATCH = 2, /* the controllers share a LATCH pin */
};

typedef uint32_t snes_controller_t;
typedef uint16_t snes_state_t;

/*
 * XXX: ordered according to the order in which button states are read from
 *      the controller.
 */
enum snes_button {
	SNES_R,
	SNES_L,
	SNES_X,
	SNES_A,
	SNES_RIGHT,
	SNES_LEFT,
	SNES_DOWN,
	SNES_UP,
	SNES_START,
	SNES_SELECT,
	SNES_Y,
	SNES_B
};
#define SNES_NR_BUTTONS (SNES_B+1)

#define SNES_BTN_MASK(btn) (0x10 << (btn))

/*
 * returns non-zero if 'btn' (a SNES_* constant) is pressed in state 'state'
 */
#define snes_button_pressed(state, btn) !((state) & SNES_BTN_MASK(btn))

/* snes_state_t accessors */
#define snes_r(state)      snes_button_pressed(state, SNES_R)
#define snes_l(state)      snes_button_pressed(state, SNES_L)
#define snes_x(state)      snes_button_pressed(state, SNES_X)
#define snes_a(state)      snes_button_pressed(state, SNES_A)
#define snes_right(state)  snes_button_pressed(state, SNES_RIGHT)
#define snes_left(state)   snes_button_pressed(state, SNES_LEFT)
#define snes_down(state)   snes_button_pressed(state, SNES_DOWN)
#define snes_up(state)     snes_button_pressed(state, SNES_UP)
#define snes_start(state)  snes_button_pressed(state, SNES_START)
#define snes_select(state) snes_button_pressed(state, SNES_SELECT)
#define snes_y(state)      snes_button_pressed(state, SNES_Y)
#define snes_b(state)      snes_button_pressed(state, SNES_B)

/* snes_controller_t accessors */
#define snes_clock(ctl)  ((ctl) & 0xFF)
#define snes_latch(ctl) (((ctl) & 0xFF00) >> 8)
#define snes_data(ctl)  (((ctl) & 0xFF0000) >> 16)

/* the "empty" state, where no button is pressed */
#define snes_state_empty() (0xFFFF)

/*
 * returns non-zero if the state of 'btn' (a SNES_* constant) changed between
 * states 'old' and 'new'
 */
#define snes_state_changed(old, new, btn) \
	(((old) & SNES_BTN_MASK(btn)) != ((new) & SNES_BTN_MASK(btn)))

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
 * Read the states of multiple SNES controllers.
 */
int snes_read_multi(int n, snes_controller_t *ctl, snes_state_t *state,
		int flags);

/*
 * Check if a controller is connected.
 */
int snes_connected(snes_controller_t controller);

#endif
