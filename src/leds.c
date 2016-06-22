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

#include "tinpuck.h"

#define LED0_PORT _LATA6
#define LED1_PORT _LATA7
#define LED2_PORT _LATA9
#define LED3_PORT _LATA12
#define LED4_PORT _LATA10
#define LED5_PORT _LATA13
#define LED6_PORT _LATA14
#define LED7_PORT _LATA15

#define LED0_DIR _TRISA6
#define LED1_DIR _TRISA7
#define LED2_DIR _TRISA9
#define LED3_DIR _TRISA12
#define LED4_DIR _TRISA10
#define LED5_DIR _TRISA13
#define LED6_DIR _TRISA14
#define LED7_DIR _TRISA15

#define LED_FRONT_PORT _LATC1
#define LED_FRONT_DIR  _TRISC1

#define LED_BODY_PORT _LATC2
#define LED_BODY_DIR  _TRISC2

void tin_init_leds(void) {
    LED0_DIR = OUTPUT;
    LED1_DIR = OUTPUT;
    LED2_DIR = OUTPUT;
    LED3_DIR = OUTPUT;
    LED4_DIR = OUTPUT;
    LED5_DIR = OUTPUT;
    LED6_DIR = OUTPUT;
    LED7_DIR = OUTPUT;

    LED_FRONT_DIR = OUTPUT;

    LED_BODY_DIR = OUTPUT;
}

void tin_set_led(unsigned int number, unsigned int value) {
    switch (number) {
        case LED0:
            LED0_PORT = value;
            break;
        case LED1:
            LED1_PORT = value;
            break;
        case LED2:
            LED2_PORT = value;
            break;
        case LED3:
            LED3_PORT = value;
            break;
        case LED4:
            LED4_PORT = value;
            break;
        case LED5:
            LED5_PORT = value;
            break;
        case LED6:
            LED6_PORT = value;
            break;
        case LED7:
            LED7_PORT = value;
            break;
        case LED_FRONT:
            LED_FRONT_PORT = value;
            break;
        case LED_BODY:
            LED_BODY_PORT = value;
            break;
        default:
            break;
    }
}
