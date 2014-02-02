/*
 *    __                              __                     
 *   /\ \                            /\ \                    
 *   \_\ \     __       __    ____   \_\ \     __       __   
 *   /'_` \  /'__`\   /'__`\ /',__\  /'_` \  /'__`\   /'__`\ 
 *  /\ \L\ \/\ \L\.\_/\  __//\__, `\/\ \L\ \/\ \L\.\_/\  __/ 
 *  \ \___,_\ \__/.\_\ \____\/\____/\ \___,_\ \__/.\_\ \____\
 *   \/__,_ /\/__/\/_/\/____/\/___/  \/__,_ /\/__/\/_/\/____/
 *                                                          
 *
 * This file is part of the AdvMenuPLUS project.
 *
 * Copyright (C) 2014 daesdae.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/** \file
 * Joy.
 */

/** \addtogroup Joystick */
/*@{*/

#ifndef __JOY_H
#define __JOY_H

#include "extra.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \name Buttons
 * Button definitions.
 */
/*@{*/

/* joystick 1 */
#define JOYB_1_1 1
#define JOYB_1_2 2
#define JOYB_1_3 3
#define JOYB_1_4 4
#define JOYB_1_5 5
#define JOYB_1_6 6
#define JOYB_1_7 7
#define JOYB_1_8 8
#define JOYB_1_9 9
#define JOYB_1_10 10
#define JOYB_1_11 11
#define JOYB_1_12 12
#define JOYB_1_13 13
#define JOYB_1_14 14
#define JOYB_1_15 15
#define JOYB_1_16 16
#define JOYB_1_UP 17
#define JOYB_1_DOWN 18
#define JOYB_1_LEFT 19
#define JOYB_1_RIGHT 20

/* joystick 2 */
#define JOYB_2_1 21
#define JOYB_2_2 22
#define JOYB_2_3 23
#define JOYB_2_4 24
#define JOYB_2_5 25
#define JOYB_2_6 26
#define JOYB_2_7 27
#define JOYB_2_8 28
#define JOYB_2_9 29
#define JOYB_2_10 30
#define JOYB_2_11 31
#define JOYB_2_12 32
#define JOYB_2_13 33
#define JOYB_2_14 34
#define JOYB_2_15 35
#define JOYB_2_16 36
#define JOYB_2_UP 37
#define JOYB_2_DOWN 38
#define JOYB_2_LEFT 39
#define JOYB_2_RIGHT 40

/* joystick 3 */
#define JOYB_3_1 41
#define JOYB_3_2 42
#define JOYB_3_3 43
#define JOYB_3_4 44
#define JOYB_3_5 45
#define JOYB_3_6 46
#define JOYB_3_7 47
#define JOYB_3_8 48
#define JOYB_3_9 49
#define JOYB_3_10 50
#define JOYB_3_11 51
#define JOYB_3_12 52
#define JOYB_3_13 53
#define JOYB_3_14 54
#define JOYB_3_15 55
#define JOYB_3_16 56
#define JOYB_3_UP 57
#define JOYB_3_DOWN 58
#define JOYB_3_LEFT 59
#define JOYB_3_RIGHT 60

/* joystick 4 */
#define JOYB_4_1 61
#define JOYB_4_2 62
#define JOYB_4_3 63
#define JOYB_4_4 64
#define JOYB_4_5 65
#define JOYB_4_6 66
#define JOYB_4_7 67
#define JOYB_4_8 68
#define JOYB_4_9 69
#define JOYB_4_10 70
#define JOYB_4_11 71
#define JOYB_4_12 72
#define JOYB_4_13 73
#define JOYB_4_14 74
#define JOYB_4_15 75
#define JOYB_4_16 76
#define JOYB_4_UP 77
#define JOYB_4_DOWN 78
#define JOYB_4_LEFT 79
#define JOYB_4_RIGHT 80

#define JOYB_MAX 256
/*@}*/

unsigned joy_code(const char* name);
	
#ifdef __cplusplus
};
#endif

#endif

/*@}*/
