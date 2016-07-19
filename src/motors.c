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

static volatile unsigned int tin_motors_powersave = 1;

static int tin_left_speed = 0;
static int tin_right_speed = 0;

static TinTask tin_motor_left_task;
static TinTask tin_motor_right_task;

static int abs(int value) {
    if (value < 0) {
        return value * -1;
    }
    return value;
}

void tin_set_motors_powersave(unsigned int enabled) {
    tin_motors_powersave = 1;
}

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

/* Don't know whether M_PI is properly pulled in */
#ifndef M_PI
/* Stolen from math.h ... yeah. */
#define M_PI 3.14159265358979323846 /* pi */
#endif

/* From http://www.e-puck.org/index.php?option=com_content&view=article&id=7&Itemid=9
 *
 *     On the e-puck design we decided to use miniature steppers motors with
 *     gear reduction. The motor has 20 steps per revolution and the gear
 *     has a reduction of 50:1. The final axis is sufficiently strong to
 *     support directly the wheel.
 *
 *     The wheels diameter is about 41 mm.  [Irrelevant things].
 *     The maximum speed of the wheels is about 1000 steps / s,
 *     which corresponds to one wheel revolution per second.
 *
 * So let's do math.  With a diameter of 41mm, the circumference is 4.1*pi cm.
 * A single step means movement by 4.1*pi/1000 cm.
 * The tin_set_speed_{left,right} want the stepping frequency in Hz, or in other words:
 *     4.1*pi/1000 cm/s
 * So we just divide by this freaky constant.
 */
static const double STEPHZ_PER_CMS = 1 / (4.1 * M_PI / 1000);

void tin_set_speed(double left, double right) {
    tin_set_speed_left((int)(left * STEPHZ_PER_CMS));
    tin_set_speed_right((int)(right * STEPHZ_PER_CMS));
}


#if TIN_MOTORS_MAX_V <= TIN_MOTORS_THRESHOLD_V
#error TIN_MOTORS_MAX_V must be higher than TIN_MOTORS_THRESHOLD_V
#endif

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
    static unsigned int on = 0;
    if(on && abs(tin_left_speed) < TIN_MOTORS_THRESHOLD_V) {
        MOTOR1_PHA_PORT = OFF;
        MOTOR1_PHB_PORT = OFF;
        MOTOR1_PHC_PORT = OFF;
        MOTOR1_PHD_PORT = OFF;
        on = 0;
        tin_task_set_period(&tin_motor_left_task, (unsigned int) 10000 / abs(tin_left_speed) - 10000 / TIN_MOTORS_MAX_V);
        return;
    }
    if (tin_left_speed > 0) {
        tin_left_motor_phase--;
        if (tin_left_motor_phase < 0) tin_left_motor_phase = 3;
    } else {
        tin_left_motor_phase++;
        if (tin_left_motor_phase > 3) tin_left_motor_phase = 0;
    }
    tin_update_left_phase();
    if(abs(tin_left_speed) < TIN_MOTORS_THRESHOLD_V && tin_motors_powersave) {
        on = 1;
        tin_task_set_period(&tin_motor_left_task, 10000 / TIN_MOTORS_MAX_V);
    }
}

void tin_run_motor_right(void) {
    static unsigned int on = 0;
    if(on && abs(tin_right_speed) < TIN_MOTORS_THRESHOLD_V) {
        MOTOR1_PHA_PORT = OFF;
        MOTOR1_PHB_PORT = OFF;
        MOTOR1_PHC_PORT = OFF;
        MOTOR1_PHD_PORT = OFF;
        on = 0;
        tin_task_set_period(&tin_motor_right_task, (unsigned int) 10000 / abs(tin_right_speed) - 10000 / TIN_MOTORS_MAX_V);
        return;
    }
    if (tin_right_speed < 0) {
        tin_right_motor_phase--;
        if (tin_right_motor_phase < 0) tin_right_motor_phase = 3;
    } else {
        tin_right_motor_phase++;
        if (tin_right_motor_phase > 3) tin_right_motor_phase = 0;
    }
    tin_update_right_phase();
    if(abs(tin_right_speed) < TIN_MOTORS_THRESHOLD_V && tin_motors_powersave) {
        on = 1;
        tin_task_set_period(&tin_motor_right_task, 10000 / TIN_MOTORS_MAX_V);
    }
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
