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

static TinTask tin_time_task;

volatile long tin_time = 0;

void tin_update_time(void) {
    tin_time++;
}

void tin_init_time(void) {
    tin_task_register(&tin_time_task, tin_update_time, 10);
}

unsigned long e_get_time() {
    SYNCHRONIZED({
        return (unsigned long) tin_time;
    })
}
