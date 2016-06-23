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

#define MOTOR1_PHA_PORT _LATD0
#define MOTOR1_PHB_PORT _LATD1
#define MOTOR1_PHC_PORT _LATD2
#define MOTOR1_PHD_PORT _LATD3

#define MOTOR2_PHA_PORT _LATD4
#define MOTOR2_PHB_PORT _LATD5
#define MOTOR2_PHC_PORT _LATD6
#define MOTOR2_PHD_PORT _LATD7

#define MOTOR1_PHA_DIR _TRISD0
#define MOTOR1_PHB_DIR _TRISD1
#define MOTOR1_PHC_DIR _TRISD2
#define MOTOR1_PHD_DIR _TRISD3

#define MOTOR2_PHA_DIR _TRISD4
#define MOTOR2_PHB_DIR _TRISD5
#define MOTOR2_PHC_DIR _TRISD6
#define MOTOR2_PHD_DIR _TRISD7

static volatile int tin_left_motor_phase = 0;
static volatile int tin_right_motor_phase = 0;

static int tin_left_speed = 0;
static int tin_right_speed = 0;

static TinTask tin_motor_left_task;
static TinTask tin_motor_right_task;

void tin_set_speed_left(int speed) {
    MOTOR1_PHA_PORT = OFF;
    MOTOR1_PHB_PORT = OFF;
    MOTOR1_PHC_PORT = OFF;
    MOTOR1_PHD_PORT = OFF;

    if (speed < -1000) {
        speed = -1000;
    } else if (speed > 1000) {
        speed = 1000;
    }

    tin_left_speed = speed;

    if (speed < 0) {
        speed *= -1;
    }

    if (speed == 0) {
        tin_task_deactivate(&tin_motor_left_task);
    } else {
        tin_task_set_period(&tin_motor_left_task, (unsigned int) (10000 / speed));
        tin_task_activate(&tin_motor_left_task);
    }
}

void tin_set_speed_right(int speed) {
    MOTOR2_PHA_PORT = OFF;
    MOTOR2_PHB_PORT = OFF;
    MOTOR2_PHC_PORT = OFF;
    MOTOR2_PHD_PORT = OFF;

    if (speed < -1000) {
        speed = -1000;
    } else if (speed > 1000) {
        speed = 1000;
    }

    tin_right_speed = speed;

    if (speed < 0) {
        speed *= -1;
    }

    if (speed == 0) {
        tin_task_deactivate(&tin_motor_right_task);
    } else {
        tin_task_set_period(&tin_motor_right_task, (unsigned int) (10000 / speed));
        tin_task_activate(&tin_motor_right_task);
    }
}

void tin_set_speed(int left, int right) {
    tin_set_speed_left(left);
    tin_set_speed_right(right);
}

void tin_update_left_phase(void) {
    switch (tin_left_motor_phase) {
        case 0:
            MOTOR1_PHA_PORT = OFF;
            MOTOR1_PHB_PORT = ON;
            MOTOR1_PHC_PORT = OFF;
            MOTOR1_PHD_PORT = ON;
            break;
        case 1:
            MOTOR1_PHA_PORT = OFF;
            MOTOR1_PHB_PORT = ON;
            MOTOR1_PHC_PORT = ON;
            MOTOR1_PHD_PORT = OFF;
            break;
        case 2:
            MOTOR1_PHA_PORT = ON;
            MOTOR1_PHB_PORT = OFF;
            MOTOR1_PHC_PORT = ON;
            MOTOR1_PHD_PORT = OFF;
            break;
        case 3:
            MOTOR1_PHA_PORT = ON;
            MOTOR1_PHB_PORT = OFF;
            MOTOR1_PHC_PORT = OFF;
            MOTOR1_PHD_PORT = ON;
            break;
    }
}

void tin_update_right_phase(void) {
    switch (tin_right_motor_phase) {
        case 0:
            MOTOR2_PHA_PORT = OFF;
            MOTOR2_PHB_PORT = ON;
            MOTOR2_PHC_PORT = OFF;
            MOTOR2_PHD_PORT = ON;
            break;
        case 1:
            MOTOR2_PHA_PORT = OFF;
            MOTOR2_PHB_PORT = ON;
            MOTOR2_PHC_PORT = ON;
            MOTOR2_PHD_PORT = OFF;
            break;
        case 2:
            MOTOR2_PHA_PORT = ON;
            MOTOR2_PHB_PORT = OFF;
            MOTOR2_PHC_PORT = ON;
            MOTOR2_PHD_PORT = OFF;
            break;
        case 3:
            MOTOR2_PHA_PORT = ON;
            MOTOR2_PHB_PORT = OFF;
            MOTOR2_PHC_PORT = OFF;
            MOTOR2_PHD_PORT = ON;
            break;
    }
}

void tin_run_motor_left(void) {
    if (tin_left_speed > 0) {
        tin_left_motor_phase--;
        if (tin_left_motor_phase < 0) tin_left_motor_phase = 3;
    } else {
        tin_left_motor_phase++;
        if (tin_left_motor_phase > 3) tin_left_motor_phase = 0;
    }
    tin_update_left_phase();
}

void tin_run_motor_right(void) {
    if (tin_right_speed < 0) {
        tin_right_motor_phase--;
        if (tin_right_motor_phase < 0) tin_right_motor_phase = 3;
    } else {
        tin_right_motor_phase++;
        if (tin_right_motor_phase > 3) tin_right_motor_phase = 0;
    }
    tin_update_right_phase();
}

void tin_init_motors(void) {
    MOTOR1_PHA_DIR = OUTPUT;
    MOTOR1_PHB_DIR = OUTPUT;
    MOTOR1_PHC_DIR = OUTPUT;
    MOTOR1_PHD_DIR = OUTPUT;

    MOTOR2_PHA_DIR = OUTPUT;
    MOTOR2_PHB_DIR = OUTPUT;
    MOTOR2_PHC_DIR = OUTPUT;
    MOTOR2_PHD_DIR = OUTPUT;

    tin_task_register(&tin_motor_left_task, tin_run_motor_left, 0);
    tin_task_register(&tin_motor_right_task, tin_run_motor_right, 0);
}