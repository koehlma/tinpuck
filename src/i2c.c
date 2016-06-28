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

void tin_init_i2c(void) {
    // frequency of SCL at 100kHz
    I2CBRG = 150;
    // disable I2C
    I2CCONbits.I2CEN = OFF;
    // enable I2C
    I2CCONbits.I2CEN = ON;
    // clear master interrupt flag
    IFS0bits.MI2CIF = 0;
    // clear slave interrupt flag
    IFS0bits.SI2CIF = 0;
    // priority level
    IPC3bits.MI2CIP = 5;
    // enable master I2C interrupt
	IEC0bits.MI2CIE = ON;
    // disable slave I2C interrupt
    IEC0bits.SI2CIE = ON;
}
