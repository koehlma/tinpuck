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

#define STATE_SOURCE 0
#define STATE_TARGET 1
#define STATE_COMMAND 2
#define STATE_LENGTH 3
#define STATE_DATA 4

static unsigned char address = 0;

static struct TinPackage* tx_queue = NULL;
static unsigned int tx_state = STATE_SOURCE;
static unsigned int tx_position = 0;

TinPackage rx_package;
static char rx_data[TIN_PACKAGE_MAX_LENGTH];
static char rx_state = STATE_SOURCE;
static unsigned int rx_position = 0;
static unsigned long rx_start = 0;
static TinPackageCallback rx_callbacks[255] = {NULL};

static TinTask timeout_task;

void tin_com_reset() {
    SYNCHRONIZED({
        rx_state = STATE_SOURCE;
        rx_position = 0;
    })
}

void tin_com_timeout() {
    if (tin_get_time() - rx_start > TIN_PACKAGE_TIMEOUT) {
        tin_com_reset();
    }
}

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

    // initialize RX package
    rx_package.data = rx_data;

    // reset receiver
    tin_com_reset();

    // enable timeout scheduler
    tin_task_register(&timeout_task, tin_com_timeout, 100);
    tin_task_activate(&timeout_task);
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

void tin_com_register(char command, TinPackageCallback callback) {
    rx_callbacks[command] = callback;
}

ISR(_U1RXInterrupt) {
    IFS0bits.U1RXIF = 0;

    while (U1STAbits.URXDA) {
        switch (rx_state) {
            case STATE_SOURCE:
                rx_package.source = U1RXREG;
                rx_start = tin_get_time();
                rx_state = STATE_TARGET;
                break;
            case STATE_TARGET:
                rx_package.target = U1RXREG;
                rx_state = STATE_COMMAND;
                break;
            case STATE_COMMAND:
                rx_package.command = U1RXREG;
                rx_state = STATE_LENGTH;
                break;
            case STATE_LENGTH:
                rx_package.length = U1RXREG;
                rx_state = STATE_DATA;
                if (rx_package.length < 1) {
                    rx_position = 0;
                    rx_state = STATE_SOURCE;
                    if (rx_callbacks[rx_package.command]) {
                        rx_callbacks[rx_package.command](&rx_package);
                    }
                }
                break;
            case STATE_DATA:
                rx_package.data[rx_position] = U1RXREG;
                rx_position++;
                if (rx_position >= rx_package.length) {
                    rx_position = 0;
                    rx_state = STATE_SOURCE;
                    if (rx_callbacks[rx_package.command]) {
                        rx_callbacks[rx_package.command](&rx_package);
                    }
                }
                break;
            default:
                break;
        }
    }
}

ISR(_U1TXInterrupt) {
    IFS0bits.U1TXIF = 0;

    TinPackage* package;

    switch (tx_state) {
        case STATE_SOURCE:
            U1TXREG = (unsigned int) tx_queue->source;
            tx_state = STATE_TARGET;
            break;
        case STATE_TARGET:
            U1TXREG = (unsigned int) tx_queue->target;
            tx_state = STATE_COMMAND;
            break;
        case STATE_COMMAND:
            U1TXREG = (unsigned int) tx_queue->command;
            tx_state = STATE_LENGTH;
            break;
        case STATE_LENGTH:
            U1TXREG = tx_queue->length;
            if (tx_queue->length) {
                tx_state = STATE_DATA;
            } else {
                package = tx_queue;
                tx_position = 0;
                tx_state = STATE_SOURCE;
                if (tx_queue->next) {
                    tx_queue = tx_queue->next;
                } else {
                    tx_queue = NULL;
                    IEC0bits.U1TXIE = OFF;
                }
                if (package->callback) {
                    package->callback(package);
                }
            }
            break;
        case STATE_DATA:
            if (tx_position < tx_queue->length) {
                U1TXREG = (unsigned int) tx_queue->data[tx_position];
                tx_position++;
            } else {
                package = tx_queue;
                tx_position = 0;
                tx_state = STATE_SOURCE;
                if (tx_queue->next) {
                    tx_queue = tx_queue->next;
                } else {
                    tx_queue = NULL;
                    IEC0bits.U1TXIE = OFF;
                }
                if (package->callback) {
                    package->callback(package);
                }
            }
            break;
        default:
            break;
    }
}
