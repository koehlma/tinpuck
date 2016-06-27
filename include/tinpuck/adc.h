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

#ifndef TIN_PUCK_ADC_H
#define TIN_PUCK_ADC_H

void tin_init_adc(void);

void tin_get_ir_ambient(unsigned int* ir_ambient);
void tin_get_ir_reflection(unsigned int* ir_reflection);

void tin_get_proximity_raw(unsigned int* proximity);

void tin_calibrate_proximity();
void tin_get_proximity(unsigned int* proximity);

#endif
