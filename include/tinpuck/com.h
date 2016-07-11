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

#ifndef TIN_PUCK_COM_H
#define TIN_PUCK_COM_H

#ifndef TIN_PACKAGE_MAX_LENGTH
#define TIN_PACKAGE_MAX_LENGTH 128
#endif

#ifndef TIN_PACKAGE_TIMEOUT
#define TIN_PACKAGE_TIMEOUT 250
#endif

struct TinPackage;

typedef struct TinPackage TinPackage;

typedef void (*TinPackageCallback)(TinPackage* package);

struct TinPackage {
    char source;
    char target;
    char command;

    unsigned int length;

    char *data;

    TinPackageCallback callback;

    volatile unsigned int completed;

    struct TinPackage* next;
};

void tin_init_com(void);

void tin_com_set_address(unsigned int address);
void tin_com_register(char command, TinPackageCallback callback);
void tin_com_send(TinPackage* package);

void tin_com_print(const char* message);

#endif
