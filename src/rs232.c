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

void tin_init_rs232(unsigned long baudrate) {
    // reset to known state
    U2MODE = 0;
    U2STA = 0;

    // direct interrupt on character reception
    U2STAbits.URXISEL = 0;

    // direct interrupt on character transmission
    U2STAbits.UTXISEL = 0;

    // set baudrate
    U2BRG = (unsigned int) ((FCY / ((float) baudrate)) / 16.0 - 1);

    // clear TX interrupt flag
    IFS1bits.U2TXIF = 0;
    // enable TX interrupt
    //IEC1bits.U2TXIE = ON;

    // enable UART
    U2MODEbits.UARTEN = ON;

    // enable TX
    U2STAbits.UTXEN = ON;
}

/*
ISR(_U2TXInterrupt) {
    IFS1bits.U2TXIF = 0;
    static unsigned int state = 0;
    state ^= 1;
    e_set_led(5, state);
}*/