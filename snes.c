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

#include "gpio.h"
#include "snes.h"

static inline void pulse_high(unsigned char pin, unsigned int us)
{
	gpio_write(pin, GPIO_HIGH);
	microdelay(us);
	gpio_write(pin, GPIO_LOW);
}

static inline void pulse_low(unsigned char pin, unsigned int us)
{
	gpio_write(pin, GPIO_LOW);
	microdelay(us);
	gpio_write(pin, GPIO_HIGH);
}

/*
 * 12us per clock cycle, 50% duty.  Button state is sampled on the falling
 * edge of a clock pulse.
 */
static inline int clock_cycle(unsigned char clock, unsigned char data)
{
	int val = gpio_read(data);
	pulse_low(clock, 6);
	microdelay(6);
	return val;
}

snes_controller_t snes_open(unsigned char clock, unsigned char latch,
		unsigned char data)
{
	gpio_out(clock);
	gpio_out(latch);
	gpio_in(data);
	gpio_pup(data);
	gpio_write(clock, GPIO_HIGH);
	gpio_write(latch, GPIO_LOW);
	return clock | (latch << 8) | (data << 16);
}

snes_state_t snes_read(snes_controller_t ctl)
{
	unsigned char clock = snes_clock(ctl);
	unsigned char latch = snes_latch(ctl);
	unsigned char data  = snes_data(ctl);
	snes_state_t state = 0;

	pulse_high(latch, 12);
	microdelay(6);

	for (int i = 0; i < 16; i++) {
		state <<= 1;
		if (clock_cycle(clock, data))
			state |= 1;
	}
	return state;
}

int snes_read_multi(int n, snes_controller_t *ctl, snes_state_t *state,
		int flags)
{
	// send pulse on LATCH
	if (flags & SNES_SHARED_LATCH) {
		pulse_high(snes_latch(ctl[0]), 12);
	} else {
		for (int i = 0; i < n; i++)
			gpio_write(snes_latch(ctl[i]), GPIO_HIGH);
		microdelay(12);
		for (int i = 0; i < n; i++)
			gpio_write(snes_latch(ctl[i]), GPIO_LOW);
	}
	microdelay(6);

	for (int i = 0; i < n; i++)
		state[i] = 0;

	for (int btn = 0; btn < 16; btn++) {
		// read button state
		for (int i = 0; i < n; i++) {
			state[i] <<= 1;
			if (gpio_read(snes_data(ctl[i])))
				state[i] |= 1;
		}

		// send pulse on CLOCK
		if (flags & SNES_SHARED_CLOCK) {
			pulse_low(snes_clock(ctl[0]), 6);
		} else {
			for (int i = 0; i < n; i++)
				gpio_write(snes_clock(ctl[i]), GPIO_LOW);
			microdelay(6);
			for (int i = 0; i < n; i++)
				gpio_write(snes_clock(ctl[i]), GPIO_HIGH);
		}
		microdelay(6);
	}
	return 0;
}

/*
 * Check if a controller is connected.  Since we use a pull-up resistor on
 * the data pin, we know a controller is connected when data reads low.
 */
int snes_connected(snes_controller_t controller)
{
	return !gpio_read(snes_data(controller));
}
