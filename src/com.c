/**
 * Tin-Puck Bluetooth Communication Subsystem
 *
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

static unsigned char address = 0;

void tin_init_com(void) {
    // reset to known state
    U1MODE = 0;
    U1STA = 0;

    // direct interrupt on character reception
    U1STAbits.URXISEL = 0;

    // direct interrupt on character transmission
    U1STAbits.UTXISEL = 0;

    // set baudrate to 115200
    U1BRG = (unsigned int) ((FCY / ((float) 115200)) / 16.0 - 1);

    // clear TX interrupt flag
    IFS0bits.U1TXIF = 0;
    // enable TX interrupt
    //IEC0bits.U1TXIE = ON;

    // enable UART
    U1MODEbits.UARTEN = ON;

    // enable TX
    U1STAbits.UTXEN = ON;
}

void tin_com_print(const char* message) {
    while (*message) {
        U1TXREG = (unsigned int) *message++;
        while (!U1STAbits.TRMT);
    }
}

/*

void tin_com_send(TinComFuture* future) {

}*/

/*
ISR(_U2RXInterrupt) {
    IFS1bits.U2RXIF = 0;

    static bool state = 0;
    state ^= 1;
    e_set_led(0, 1);
}

ISR(_U2TXInterrupt) {
    IFS1bits.U2TXIF = 0;
}
*/