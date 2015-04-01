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

#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include "gpio.h"
#include "snes.h"
#include "uinput.h"
#include "config.h"

static int uinput_fd = -1;
static bool foreground = false;

/* Die, printing an error message */
static void derror(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

static void sigkill_handler(int signo)
{
	for (unsigned i = 0; i < NR_CONTROLLERS; i++)
		gpio_write(controller[i].clock, GPIO_LOW);
	gpio_fini();
	uinput_fini(uinput_fd);
	exit(EXIT_SUCCESS);
}

static void init_all(void)
{
	struct sigaction kill_action;

	if (!gpio_init())
		derror("gpio_init");

	if ((uinput_fd = uinput_init()) < 0)
		derror("uinput_init");

	// clean exit on SIGINT/SIGTERM
	sigemptyset(&kill_action.sa_mask);
	kill_action.sa_handler = sigkill_handler;
	kill_action.sa_flags = 0;
	if (sigaction(SIGINT, &kill_action, NULL) < 0)
		derror("sigaction");
	if (sigaction(SIGTERM, &kill_action, NULL) < 0)
		derror("sigaction");
}

static void daemonize(void)
{
	pid_t pid;

	if ((pid = fork()) < 0)
		derror("fork");
	if (pid > 0)
		_exit(EXIT_SUCCESS);

	if (setsid() < 0)
		derror("setsid");

	if ((pid = fork()) < 0)
		derror("fork");
	if (pid > 0)
		_exit(EXIT_SUCCESS);

	if (chdir("/") < 0)
		derror("chdir");

	umask(0);

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

static void usage(int status)
{
#define uputs(str) fprintf(stderr, str "\n")
	uputs("Usage: usnes [option]...");
	uputs("Options:");
	uputs("\t-f,--foreground     Run the driver as a foreground process");
	uputs("\t-h,--help           Display this message and exit");
	exit(status);
}

static void parse_opts(int argc, char *argv[])
{
	for (;;) {
		static struct option long_options[] = {
			{ "foreground", no_argument,       0, 'f' },
			{ "help",       no_argument,       0, 'h' },
			{ 0, 0, 0, 0 }
		};
		int options_index = 0;
		int c = getopt_long(argc, argv, "fh", long_options, &options_index);

		if (c < 0)
			break;
		switch (c) {
		case 'f':
			foreground = true;
			break;
		case 'h':
			usage(EXIT_SUCCESS);
		case '?':
			break;
		default:
			usage(EXIT_FAILURE);
		}
	}
}

static snes_controller_t handle[NR_CONTROLLERS];
static snes_state_t prev_state[NR_CONTROLLERS];
static snes_state_t state[NR_CONTROLLERS];

static int init_controllers(void)
{
	bool shared_clock = true;
	bool shared_latch = true;
	for (unsigned i = 0; i < NR_CONTROLLERS; i++) {
		if (controller[i].clock != controller[0].clock)
			shared_clock = false;
		if (controller[i].latch != controller[0].latch)
			shared_latch = false;
		handle[i] = snes_open(
				controller[i].clock,
				controller[i].latch,
				controller[i].data);
		prev_state[i] = state[i] = snes_state_empty();
	}
	return (shared_clock ? SNES_SHARED_CLOCK : 0)
		| (shared_latch ? SNES_SHARED_LATCH : 0);
}

static void do_event(unsigned long key, int value)
{
	if (uinput_key_event(uinput_fd, key, value) < 0)
		perror("key_event");
	if (uinput_syn_event(uinput_fd) < 0)
		perror("syn_event");
}

static void handle_events(void)
{
	for (unsigned b = 0; b < SNES_NR_BUTTONS; b++) {
		for (unsigned c = 0; c < NR_CONTROLLERS; c++) {
			if (snes_state_changed(prev_state[c], state[c], b)) {
				do_event(controller[c].keymap[b],
					snes_button_pressed(state[c], b));
			}
		}
	}
}

int main(int argc, char *argv[])
{
	int flags;

	parse_opts(argc, argv);
	printf("Starting usnes... ");
	fflush(stdout);
	init_all();
	printf("OK\n");
	if (!foreground)
		daemonize();

	flags = init_controllers();
	for (;;) {
		// poll
		snes_read_multi(NR_CONTROLLERS, handle, state, flags);
		handle_events();

		// wait
		usleep(POLL_MS*1000);
		for (unsigned i = 0; i < NR_CONTROLLERS; i++)
			prev_state[i] = state[i];
	}
}
