/*
 * Copyright (C) 2016, Maximilian Köhl <mail@koehlma.de>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TIN_PUCK_LEDS_H
#define TIN_PUCK_LEDS_H

#define LED0        0
#define LED1        1
#define LED2        2
#define LED3        3
#define LED4        4
#define LED5        5
#define LED6        6
#define LED7        7

#define LED_FRONT   8
#define LED_BODY    9

void tin_init_leds(void);
void tin_set_led(unsigned int number, unsigned int value);

#endif
