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

#define SELECTOR0_PORT _RG6
#define SELECTOR1_PORT _RG7
#define SELECTOR2_PORT _RG8
#define SELECTOR3_PORT _RG9

#define SELECTOR0_DIR _TRISG6
#define SELECTOR1_DIR _TRISG7
#define SELECTOR2_DIR _TRISG8
#define SELECTOR3_DIR _TRISG9

void tin_init_selector(void) {
    SELECTOR0_DIR = INPUT;
    SELECTOR1_DIR = INPUT;
    SELECTOR2_DIR = INPUT;
    SELECTOR3_DIR = INPUT;
}

unsigned int tin_get_selector(void) {
    return (SELECTOR3_PORT << 3) | (SELECTOR2_PORT << 2) | (SELECTOR1_PORT << 1) | SELECTOR0_PORT;
}
