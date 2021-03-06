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

#ifndef TIN_PUCK_MOTORS_H
#define TIN_PUCK_MOTORS_H

void tin_set_speed_left(int speed);
void tin_set_speed_right(int speed);

#ifndef TIN_MOTORS_MAX_V
#define TIN_MOTORS_MAX_V 650
#endif

#ifndef TIN_MOTORS_THRESHOLD_V
#define TIN_MOTORS_THRESHOLD_V 601
#endif

/* Units: cm/s */
void tin_set_speed(double left, double right);

void tin_set_motors_powersave(unsigned int enabled);

void tin_init_motors(void);

#endif
