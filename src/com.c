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

#define TX_STATE_SOURCE 0
#define TX_STATE_TARGET 1
#define TX_STATE_COMMAND 2
#define TX_STATE_LENGTH 3
#define TX_STATE_DATA 4


static unsigned char address = 0;

static struct TinPackage* tx_queue = NULL;
static unsigned int tx_state = TX_STATE_SOURCE;
static unsigned int tx_position = 0;

static TinPackage rx_package;
static char rx_data[TIN_PACKAGE_MAX_LENGTH];



unsigned char com_buffer[64];

static unsigned int position = 0;


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

    // clear RX interrupt flag
    IFS0bits.U1RXIF = 0;
    // enable RX interrupt
    IEC0bits.U1RXIE = ON;

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

void tin_com_send(TinPackage* package) {
    SYNCHRONIZED({
        if (!tx_queue) {
            tx_queue = package;
            IFS0bits.U1TXIF = 1;
            IEC0bits.U1TXIE = ON;
        } else {
            struct TinPackage* current = tx_queue;
            while (current->next) {
                current = current->next;
            }
            current->next = package;
        }
    })
}

ISR(_U1RXInterrupt) {
    IFS0bits.U1RXIF = 0;

    // TODO: implement packet handling

    while (U1STAbits.URXDA) {
        com_buffer[position] = U1RXREG;
        position++;
        position %= 64;
    }
}

ISR(_U1TXInterrupt) {
    IFS0bits.U1TXIF = 0;

    switch (tx_state) {
        case TX_STATE_SOURCE:
            U1TXREG = (unsigned int) tx_queue->source;
            tx_state = TX_STATE_TARGET;
            break;
        case TX_STATE_TARGET:
            U1TXREG = (unsigned int) tx_queue->target;
            tx_state = TX_STATE_COMMAND;
            break;
        case TX_STATE_COMMAND:
            U1TXREG = (unsigned int) tx_queue->command;
            tx_state = TX_STATE_LENGTH;
            break;
        case TX_STATE_LENGTH:
            U1TXREG = tx_queue->length;
            tx_state = TX_STATE_DATA;
            break;
        case TX_STATE_DATA:
            if (tx_position < tx_queue->length) {
                U1TXREG = (unsigned int) tx_queue->data[tx_position];
                tx_position++;
            } else {
                if (tx_queue->callback) {
                    tx_queue->callback(tx_queue);
                }
                tx_position = 0;
                tx_state = TX_STATE_SOURCE;
                if (tx_queue->next) {
                    tx_queue = tx_queue->next;
                } else {
                    tx_queue = NULL;
                    IEC0bits.U1TXIE = OFF;
                }
            }
            break;
        default:
            break;
    }
}
