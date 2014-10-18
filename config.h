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

#ifndef CONFIG_H_
#define CONFIG_H_

#include <linux/input.h>

#define A_EVENT      KEY_A
#define B_EVENT      KEY_B
#define X_EVENT      KEY_X
#define Y_EVENT      KEY_Y
#define L_EVENT      KEY_L
#define R_EVENT      KEY_R
#define UP_EVENT     KEY_UP
#define DOWN_EVENT   KEY_DOWN
#define LEFT_EVENT   KEY_LEFT
#define RIGHT_EVENT  KEY_RIGHT
#define START_EVENT  KEY_ENTER
#define SELECT_EVENT KEY_ESC

#define DEFAULT_CLOCK_PIN 21
#define DEFAULT_LATCH_PIN 20
#define DEFAULT_DATA_PIN  16

#define POLL_MS 60
#define PLUG_MS 1000

#endif
