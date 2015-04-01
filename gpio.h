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

#ifndef GPIO_H_
#define GPIO_H_

/*
 * GPIO library abstraction layer.  Porting usnes to another architechture
 * should (hopefully) be simply a matter of editing this file, and linking 
 * against the correct library.
 */

#include <bcm2835.h>

#define GPIO_HIGH HIGH
#define GPIO_LOW  LOW

#define microdelay(us) bcm2835_delayMicroseconds(us)
#define gpio_init      bcm2835_init
#define gpio_fini      bcm2835_close
#define gpio_out(pin)  bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP)
#define gpio_in(pin)   bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT)
#define gpio_pud(pin)  bcm2835_gpio_set_pud(pin, BCM2835_GPIO_PUD_DOWN)
#define gpio_pup(pin)  bcm2835_gpio_set_pud(pin, BCM2835_GPIO_PUD_UP)
#define gpio_write     bcm2835_gpio_write
#define gpio_read      bcm2835_gpio_lev

#endif
