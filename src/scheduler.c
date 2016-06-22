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

static TinTask* tin_task_list_head = NULL;

void tin_init_scheduler(void) {
    // reset timer control register
    T2CON = 0;
    // disable prescaler
    T2CONbits.TCKPS = 0;
    // clear timer value
    TMR2 = 0;
    // interrupt every 100us
    PR2 = (unsigned int)(100 * MICROSEC);
    // clear interrupt flag
    IFS0bits.T2IF = 0;
    // enable interrupt
    IEC0bits.T2IE = 1;
    // enable timer
    T2CONbits.TON = 1;
}

void tin_task_register(TinTask* task, void (*callback)(void), unsigned int period) {
    task->active = 0;
    task->counter = 0;
    task->period = period;
    task->callback = callback;
    task->next = tin_task_list_head;
    tin_task_list_head = task;
}

void tin_task_activate(TinTask* task) {
    task->counter = 0;
    task->active = 1;
}

void tin_task_deactivate(TinTask* task) {
    task->active = 0;
}

void tin_task_set_period(TinTask* task, unsigned int period) {
    task->counter = 0;
    task->period = period;
}

void __attribute__((interrupt, auto_psv)) _T2Interrupt(void) {
    IFS0bits.T2IF = 0;

    TinTask* task = tin_task_list_head;

    while (task) {
        if (task->active) {
            task->counter++;
            if (task->counter >= task->period) {
                task->counter = 0;
                task->callback();
            }
        }
        task = task->next;
    }
}