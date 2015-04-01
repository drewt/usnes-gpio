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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include "uinput.h"
#include "config.h"

int uinput_init(void)
{
	int fd;
	struct uinput_user_dev uidev;

	/* set up the uinput device */
	if ((fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK)) < 0)
		return -1;

	if (ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
		return -1;
	if (ioctl(fd, UI_SET_EVBIT, EV_SYN) < 0)
		return -1;

	for (unsigned c = 0; c < NR_CONTROLLERS; c++) {
		for (unsigned b = 0; b < SNES_NR_BUTTONS; b++) {
			if (ioctl(fd, UI_SET_KEYBIT, controller[c].keymap[b]) < 0)
				return -1;
		}
	}

	memset(&uidev, 0, sizeof(uidev));
	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "usnes");
	uidev.id.bustype = BUS_USB;
	uidev.id.vendor = 0x1234;
	uidev.id.product = 0xfedc;
	uidev.id.version = 1;

	if (write(fd, &uidev, sizeof(uidev)) < 0)
		return -1;

	if (ioctl(fd, UI_DEV_CREATE) < 0)
		return -1;

	return fd;
}

int uinput_fini(int fd)
{
	return ioctl(fd, UI_DEV_DESTROY);
}

/* Send a key event to the uinput device */
int uinput_key_event(int fd, unsigned int btn, int value)
{
	struct input_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.type = EV_KEY;
	ev.code = btn;
	ev.value = value;
	return write(fd, &ev, sizeof(ev));
}

/* Send a synchronization event to the uinput device */
int uinput_syn_event(int fd)
{
	struct input_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.type = EV_SYN;
	ev.code = SYN_REPORT;
	return write(fd, &ev, sizeof(ev));
}
