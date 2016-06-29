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

#define MODE_ANALOG 0
#define MODE_DIGITAL 1

#define PULSE_IR0_PORT _LATF7	 // pulse IR 0 and 4
#define PULSE_IR1_PORT _LATF8	// pulse IR 1 and 5
#define PULSE_IR2_PORT _LATG0	// pulse IR 2 and 6
#define PULSE_IR3_PORT _LATG1   // pulse IR 3 and 7

#define PULSE_IR0_DIR _TRISF7
#define PULSE_IR1_DIR _TRISF8
#define PULSE_IR2_DIR _TRISG0
#define PULSE_IR3_DIR _TRISG1

#define BASE_SAMPLE_FREQ 16384.0

#define PROXIMITY_START (int) (BASE_SAMPLE_FREQ / 256.0)
#define PROXIMITY_PULSE (int) (0.0003 * BASE_SAMPLE_FREQ)

#define ADCS_2_CHANNEL	(unsigned int) (2.0 * FCY / (BASE_SAMPLE_FREQ * (14 + 1) * 2) - 1)
#define ADCS_3_CHANNEL	(unsigned int) (2.0 * FCY / (BASE_SAMPLE_FREQ * (14 + 1) * 3) - 1)
#define ADCS_4_CHANNEL	(unsigned int) (2.0 * FCY / (BASE_SAMPLE_FREQ * (14 + 1) * 4) - 1)
#define ADCS_5_CHANNEL	(unsigned int) (2.0 * FCY / (BASE_SAMPLE_FREQ * (14 + 1) * 5) - 1)
#define ADCS_6_CHANNEL	(unsigned int) (2.0 * FCY / (BASE_SAMPLE_FREQ * (14 + 1) * 6) - 1)
#define ADCS_7_CHANNEL	(unsigned int) (2.0 * FCY / (BASE_SAMPLE_FREQ * (14 + 1) * 7) - 1)
#define ADCS_8_CHANNEL	(unsigned int) (2.0 * FCY / (BASE_SAMPLE_FREQ * (14 + 1) * 8) - 1)
#define ADCS_9_CHANNEL	(unsigned int) (2.0 * FCY / (BASE_SAMPLE_FREQ * (14 + 1) * 9) - 1)
#define ADCS_10_CHANNEL	(unsigned int) (2.0 * FCY / (BASE_SAMPLE_FREQ * (14 + 1) * 10) - 1)
#define ADCS_11_CHANNEL	(unsigned int) (2.0 * FCY / (BASE_SAMPLE_FREQ * (14 + 1) * 11) - 1)

static int tin_proximity_ambient[8];
static int tin_proximity_reflected[8];
static int tin_proximity_offset[8] = {0, 0, 0, 0, 0, 0, 0, 0};

static unsigned int tin_proximity_counter = 0;

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
    // set conversion clock
    ADCON3bits.ADCS = ADCS_8_CHANNEL;

    // clear ADC interrupt flag
    IFS0bits.ADIF = OFF;
    // enable ADC interrupt
    IEC0bits.ADIE = ON;

    // enable ADC conversion
    ADCON1bits.ADON = ON;
}

void __attribute__((__interrupt__, auto_psv)) _ADCInterrupt(void) {
    IFS0bits.ADIF = 0;

    static unsigned int counter = 0;
    static unsigned int sensors = 0;

    if (counter == PROXIMITY_START - 1) {
        switch (sensors) {
            case 0:
                tin_proximity_ambient[0] = ADCBUF0;
                tin_proximity_ambient[4] = ADCBUF4;
                PULSE_IR0_PORT = ON;
                break;
            case 1:
                tin_proximity_ambient[1] = ADCBUF1;
                tin_proximity_ambient[5] = ADCBUF5;
                PULSE_IR1_PORT = ON;
                break;
            case 2:
                tin_proximity_ambient[2] = ADCBUF2;
                tin_proximity_ambient[6] = ADCBUF6;
                PULSE_IR2_PORT = ON;
                break;
            case 3:
                tin_proximity_ambient[3] = ADCBUF3;
                tin_proximity_ambient[7] = ADCBUF7;
                PULSE_IR3_PORT = ON;
                break;
            default:
                sensors = 0;
        }

    } else if (counter == PROXIMITY_PULSE - 1) {
        switch (sensors) {
            case 0:
                tin_proximity_reflected[0] = ADCBUF0;
                tin_proximity_reflected[4] = ADCBUF4;
                PULSE_IR0_PORT = OFF;
                break;
            case 1:
                tin_proximity_reflected[1] = ADCBUF1;
                tin_proximity_reflected[5] = ADCBUF5;
                PULSE_IR1_PORT = OFF;
                break;
            case 2:
                tin_proximity_reflected[2] = ADCBUF2;
                tin_proximity_reflected[6] = ADCBUF6;
                PULSE_IR2_PORT = OFF;
                break;
            case 3:
                tin_proximity_reflected[3] = ADCBUF3;
                tin_proximity_reflected[7] = ADCBUF7;
                PULSE_IR3_PORT = OFF;
                break;
            default:
                sensors = 0;
        }
        sensors++;
        if (sensors >= 4) {
            sensors = 0;
            tin_proximity_counter++;
        }
    }
    if (counter++ >= PROXIMITY_START) {
        counter = 0;
    }
}

void tin_calibrate_proximity() {
    unsigned int counter, previous, index;
    unsigned int proximity[8];

    for (index = 0; index < 8; index++) {
        tin_proximity_offset[index] = 0;
    }

    for (counter = 0; counter < 10; counter++) {
        tin_get_proximity_raw(proximity);
        previous = tin_proximity_counter;
        for (index = 0; index < 8; index++) {
            tin_proximity_offset[index] += proximity[index];
        }
        while (previous == tin_proximity_counter);
    }

    for (index = 0; index < 8; index++) {
        tin_proximity_offset[index] = (unsigned int) (tin_proximity_offset[index] / 10.0);
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
        ir_reflection[index] = (unsigned int) tin_proximity_reflected[index];
    }
}

void tin_get_proximity_raw(unsigned int* proximity) {
    int value;
    unsigned int index;
    for (index = 0; index < 8; index++) {
        value = tin_proximity_ambient[index] - tin_proximity_reflected[index];
        proximity[index] = value > 0 ? (unsigned int) value : 0;
    }
}

void tin_get_proximity(unsigned int *proximity) {
    int value;
    unsigned int index;
    tin_get_proximity_raw(proximity);
    for (index = 0; index < 8; index++) {
        value = proximity[index] - tin_proximity_offset[index];
        proximity[index] = value > 0 ? (unsigned int) value : 0;
    }
}