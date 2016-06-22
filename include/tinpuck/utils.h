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

#ifndef TIN_PUCK_UTILS_H
#define TIN_PUCK_UTILS_H

#ifndef NULL
#define NULL 0
#endif

#define ON 1
#define OFF 0

#define OUTPUT 0
#define INPUT 1

#define FOSC   7.3728e6     // 7.3728MHz crystal in XTL mode
#define PLL    8.0       	// 8x PLL

#define FCY         ((FOSC * PLL) / (4.0))  // instruction cycle frequency
#define MILLISEC    (FCY / 1.0e3)		    // 1ms delay constant
#define MICROSEC    (FCY / 1.0e6)		    // 1us delay constant
#define NANOSEC     (FCY / 1.0e9)		    // 1ns delay constant

#define DISABLE_INTERRUPTS() { __asm__ volatile ("disi #10000"); }
#define ENABLE_INTERRUPTS() { __asm__ volatile ("disi #2"); }

#define ISR(VECTOR) void __attribute__((__interrupt__, auto_psv)) VECTOR(void)

void tin_enable_interrupts(void);
void tin_disable_interrupts(void);

void __tin_cleanup(unsigned char*);

#define SYNCHRONIZED(CODE)                                                      \
    {                                                                           \
        unsigned char __dummy __attribute__ ((__cleanup__(__tin_cleanup))) = 0; \
        DISABLE_INTERRUPTS();                                                   \
        { CODE };                                                               \
    }

#endif
