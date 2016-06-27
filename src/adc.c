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

#define SAMPLE_FREQ 3840.0

#define MODE_ANALOG 0
#define MODE_DIGITAL 1

#define PULSE_IR0_PORT _LATF7	    // pulse IR 0 and 4
#define PULSE_IR1_PORT _LATF8	    // pulse IR 1 and 5
#define PULSE_IR2_PORT _LATG0	    // pulse IR 2 and 6
#define PULSE_IR3_PORT _LATG1       // pulse IR 3 and 7

#define PULSE_IR0_DIR _TRISF7
#define PULSE_IR1_DIR _TRISF8
#define PULSE_IR2_DIR _TRISG0
#define PULSE_IR3_DIR _TRISG1

/* first in front right of robot than turning clockwise */
#define IR0 8   // ir proximity sensor 0 on AD channel 8
#define IR1 9   // ir proximity sensor 1 on AD channel 9
#define IR2 10  // ir proximity sensor 2 on AD channel 10
#define IR3 11  // ir proximity sensor 3 on AD channel 11
#define IR4 12  // ir proximity sensor 4 on AD channel 12
#define IR5 13  // ir proximity sensor 5 on AD channel 13
#define IR6 14  // ir proximity sensor 6 on AD channel 14
#define IR7 15  // ir proximity sensor 7 on AD channel 15

#define SCAN_IR04 ((ON << IR0) | (ON << IR4))
#define SCAN_IR15 ((ON << IR1) | (ON << IR5))
#define SCAN_IR26 ((ON << IR2) | (ON << IR6))
#define SCAN_IR37 ((ON << IR3) | (ON << IR7))

static volatile int tin_proximity_ambient[] = {0, 0, 0, 0, 0, 0, 0, 0};
static volatile int tin_proximity_reflection[] = {0, 0, 0, 0, 0, 0, 0, 0};

static volatile int tin_proximity_calibration[] = {0, 0, 0, 0, 0, 0, 0, 0};

void tin_init_adc(void) {
    PULSE_IR0_DIR = OUTPUT;
    PULSE_IR1_DIR = OUTPUT;
    PULSE_IR2_DIR = OUTPUT;
    PULSE_IR3_DIR = OUTPUT;

    ADCON1 = 0;
    ADCON2 = 0;
    ADCON3 = 0;
    ADCHS = 0;

    // front and body LED
    ADPCFGbits.PCFG0 = MODE_DIGITAL;
    ADPCFGbits.PCFG1 = MODE_DIGITAL;

    ADPCFGbits.PCFG2 = MODE_ANALOG;     // Microphone 0
    ADPCFGbits.PCFG3 = MODE_ANALOG;     // Microphone 1
    ADPCFGbits.PCFG4 = MODE_ANALOG;     // Microphone 2
    ADPCFGbits.PCFG5 = MODE_ANALOG;     // Accelerometer X
    ADPCFGbits.PCFG6 = MODE_ANALOG;     // Accelerometer Y
    ADPCFGbits.PCFG7 = MODE_ANALOG;     // Accelerometer Z
    ADPCFGbits.PCFG8 = MODE_ANALOG;     // IR0
    ADPCFGbits.PCFG9 = MODE_ANALOG;     // IR1
    ADPCFGbits.PCFG10 = MODE_ANALOG;    // IR2
    ADPCFGbits.PCFG11 = MODE_ANALOG;    // IR3
    ADPCFGbits.PCFG12 = MODE_ANALOG;    // IR4
    ADPCFGbits.PCFG13 = MODE_ANALOG;    // IR5
    ADPCFGbits.PCFG14 = MODE_ANALOG;    // IR6
    ADPCFGbits.PCFG15 = MODE_ANALOG;    // IR7

    ADCSSLbits.CSSL2 = OFF;     // Microphone 0
    ADCSSLbits.CSSL3 = OFF;     // Microphone 1
    ADCSSLbits.CSSL4 = OFF;     // Microphone 2
    ADCSSLbits.CSSL5 = OFF;     // Accelerometer X
    ADCSSLbits.CSSL6 = OFF;     // Accelerometer Y
    ADCSSLbits.CSSL7 = OFF;     // Accelerometer Z
    ADCSSLbits.CSSL8 = ON;      // IR0
    ADCSSLbits.CSSL9 = ON;      // IR1
    ADCSSLbits.CSSL10 = ON;     // IR2
    ADCSSLbits.CSSL11 = ON;     // IR3
    ADCSSLbits.CSSL12 = ON;     // IR4
    ADCSSLbits.CSSL13 = ON;     // IR5
    ADCSSLbits.CSSL14 = ON;     // IR6
    ADCSSLbits.CSSL15 = ON;     // IR7

    // set output format to unsigned int
    ADCON1bits.FORM = 0;
    // turn automatic sampling on
    ADCON1bits.ASAM = ON;
    // switch to automatic conversion mode
    ADCON1bits.SSRC = 0b111;

    // scan inputs
    ADCON2bits.CSCNA = ON;
    // interrupt on 8th sample
    ADCON2bits.SMPI = 8 - 1;

    // set automatic sample time
    ADCON3bits.SAMC = 1;
    // set conversion clock time
    ADCON3bits.ADCS = (unsigned int) (2.0 * FCY / (SAMPLE_FREQ * (14 + 1) * 8) - 1);

    // clear ADC interrupt flag
    IFS0bits.ADIF = OFF;
    // enable ADC interrupt
    IEC0bits.ADIE = ON;

    // enable ADC conversion
    ADCON1bits.ADON = ON;
}

void __attribute__((__interrupt__, auto_psv)) _ADCInterrupt (void) {
    IFS0bits.ADIF = OFF;

    static unsigned int state = 0;

    switch (state) {
        case 0:
            tin_proximity_ambient[0] = ADCBUF0;
            tin_proximity_ambient[1] = ADCBUF1;
            tin_proximity_ambient[2] = ADCBUF2;
            tin_proximity_ambient[3] = ADCBUF3;
            tin_proximity_ambient[4] = ADCBUF4;
            tin_proximity_ambient[5] = ADCBUF5;
            tin_proximity_ambient[6] = ADCBUF6;
            tin_proximity_ambient[7] = ADCBUF7;
            PULSE_IR0_PORT = ON;
            PULSE_IR1_PORT = ON;
            PULSE_IR2_PORT = ON;
            PULSE_IR3_PORT = ON;
            state = 1;
            break;
        case 1:
            state = 2;
            break;
        case 2:
            tin_proximity_reflection[0] = ADCBUF0;
            tin_proximity_reflection[1] = ADCBUF1;
            tin_proximity_reflection[2] = ADCBUF2;
            tin_proximity_reflection[3] = ADCBUF3;
            tin_proximity_reflection[4] = ADCBUF4;
            tin_proximity_reflection[5] = ADCBUF5;
            tin_proximity_reflection[6] = ADCBUF6;
            tin_proximity_reflection[7] = ADCBUF7;
            PULSE_IR0_PORT = OFF;
            PULSE_IR1_PORT = OFF;
            PULSE_IR2_PORT = OFF;
            PULSE_IR3_PORT = OFF;
            state = 0;
            break;
        default:
            state = 0;
    }
}


void tin_get_ir_ambient(unsigned int* ir_ambient) {
    unsigned int index;
    for (index = 0; index < 8; index++) {
        ir_ambient[index] = (unsigned int) tin_proximity_ambient[index];
    }
}

void tin_get_ir_reflection(unsigned int* ir_reflection) {
    unsigned int index;
    for (index = 0; index < 8; index++) {
        ir_reflection[index] = (unsigned int) tin_proximity_reflection[index];
    }
}

void tin_get_proximity_raw(unsigned int* proximity) {
    int value;
    unsigned int index;
    for (index = 0; index < 8; index++) {
        value = tin_proximity_reflection[index] - tin_proximity_ambient[index];
        proximity[index] = value > 0 ? (unsigned int) value : 0;
    }
}

void tin_calibrate_proximity() {
    unsigned int index;
    unsigned int proximity[8];
    tin_get_proximity_raw(proximity);
    for (index = 0; index < 8; index++) {
        tin_proximity_calibration[index] = proximity[index];
    }
}

void tin_get_proximity(unsigned int* proximity) {
    int value;
    unsigned int index;
    tin_get_proximity_raw(proximity);
    for (index = 0; index < 8; index++) {
        value = proximity[index] - tin_proximity_calibration[index];
        proximity[index] = value > 0 ? (unsigned int) value : 0;
    }
}
