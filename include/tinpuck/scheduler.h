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

#ifndef TIN_PUCK_SCHEDULER_H
#define TIN_PUCK_SCHEDULER_H

typedef struct TinTask {
    unsigned int period;
    unsigned int counter;
    unsigned int active;
    void (*callback)(void);
    struct TinTask* next;
} TinTask;

void tin_init_scheduler(void);

void tin_task_register(TinTask* task, void (*callback)(void), unsigned int period);
void tin_task_activate(TinTask* task);
void tin_task_deactivate(TinTask* task);
void tin_task_set_period(TinTask* task, unsigned int period);

#endif
