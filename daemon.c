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

#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <stdio.h>
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

static unsigned char clock_pin = DEFAULT_CLOCK_PIN;
static unsigned char latch_pin = DEFAULT_LATCH_PIN;
static unsigned char data_pin  = DEFAULT_DATA_PIN;
static unsigned char foreground = 0;

/* Die, printing an error message */
static void derror(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

static void sigkill_handler(int signo)
{
	gpio_write(clock_pin, LOW);
	gpio_fini();
	uinput_fini(uinput_fd);
	exit(EXIT_SUCCESS);
}

/* Check for and handle press/release events for a given button */
static inline void check_button(snes_state_t old, snes_state_t new,
		unsigned int btn, unsigned long key)
{
	if (snes_state_changed(old, new, btn)) {
		if (uinput_key_event(uinput_fd, key, snes_button_pressed(new, btn)) < 0)
			perror("key_event");
		if (uinput_syn_event(uinput_fd) < 0)
			perror("syn_event");
	}
}

static void init_all(void)
{
	struct sigaction kill_action;

	if (!gpio_init())
		derror("gpio_init");

	if ((uinput_fd = uinput_init()) < 0)
		derror("uinput_init");

	/* clean exit on SIGINT/SIGTERM */
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
	uputs("\t-c,--clock <pin>    Use the GPIO pin <pin> as the clock pin");
	uputs("\t-d,--data <pin>     Use the GPIO pin <pin> as the data pin");
	uputs("\t-f,--foreground     Run the driver as a foreground process");
	uputs("\t-h,--help           Display this message and exit");
	uputs("\t-l,--latch <pin>    Use the GPIO pin <pin> as the latch pin");
	exit(status);
}

static int parse_int(const char *str, int min, int max)
{
	char *endptr = NULL;
	long chan = strtol(str, &endptr, 10);
	if (chan < min || chan > max || (endptr && *endptr != '\0'))
		usage(EXIT_FAILURE);
	return chan;
}

static void parse_opts(int argc, char *argv[])
{
	for (;;) {
		static struct option long_options[] = {
			{ "clock",      required_argument, 0, 'c' },
			{ "latch",      required_argument, 0, 'l' },
			{ "data",       required_argument, 0, 'd' },
			{ "foreground", no_argument,       0, 'f' },
			{ "help",       no_argument,       0, 'h' },
			{ 0, 0, 0, 0 }
		};
		int options_index = 0;
		int c = getopt_long(argc, argv, "c:l:d:fh", long_options, &options_index);

		if (c < 0)
			break;
		switch (c) {
		case 'c':
			clock_pin = parse_int(optarg, 0, 40);
			break;
		case 'l':
			latch_pin = parse_int(optarg, 0, 40);
			break;
		case 'd':
			data_pin = parse_int(optarg, 0, 40);
			break;
		case 'f':
			foreground = 1;
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

int main(int argc, char *argv[])
{
	snes_controller_t cntl;
	snes_state_t old, new;

	parse_opts(argc, argv);

	printf("Starting usnes... ");
	fflush(stdout);
	init_all();
	printf("OK\n");
	if (!foreground)
		daemonize();

	/* poll controller state every POLL_MS milliseconds */
	cntl = snes_open(clock_pin, latch_pin, data_pin);
	old = snes_state_empty();
	for (;;) {
		while (!snes_connected(cntl))
			usleep(PLUG_MS*1000);
		new = snes_read(cntl);
		check_button(old, new, SNES_A,      A_EVENT);
		check_button(old, new, SNES_B,      B_EVENT);
		check_button(old, new, SNES_X,      X_EVENT);
		check_button(old, new, SNES_Y,      Y_EVENT);
		check_button(old, new, SNES_L,      L_EVENT);
		check_button(old, new, SNES_R,      R_EVENT);
		check_button(old, new, SNES_UP,     UP_EVENT);
		check_button(old, new, SNES_DOWN,   DOWN_EVENT);
		check_button(old, new, SNES_LEFT,   LEFT_EVENT);
		check_button(old, new, SNES_RIGHT,  RIGHT_EVENT);
		check_button(old, new, SNES_START,  START_EVENT);
		check_button(old, new, SNES_SELECT, SELECT_EVENT);
		usleep(POLL_MS*1000);
		old = new;
	}
}
