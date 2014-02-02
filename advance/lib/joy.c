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

#include "portable.h"

#include "joy.h"

struct joy_entry {
	const char* name;
	unsigned code;
};

static struct joy_entry JOY[] = {
/* joystick 1 */
{ "1joy_1", JOYB_1_1},
{ "1joy_2", JOYB_1_2 },
{ "1joy_3", JOYB_1_3 },
{ "1joy_4", JOYB_1_4 },
{ "1joy_5", JOYB_1_5 },
{ "1joy_6", JOYB_1_6 },
{ "1joy_7", JOYB_1_7 },
{ "1joy_8", JOYB_1_8 },
{ "1joy_9", JOYB_1_9 },
{ "1joy_10", JOYB_1_10 },
{ "1joy_11", JOYB_1_11 },
{ "1joy_12", JOYB_1_12 },
{ "1joy_13", JOYB_1_13 },
{ "1joy_14", JOYB_1_14 },
{ "1joy_15", JOYB_1_15 },
{ "1joy_16", JOYB_1_16 },
{ "1joy_up", JOYB_1_UP },
{ "1joy_down", JOYB_1_DOWN },
{ "1joy_left", JOYB_1_LEFT },
{ "1joy_right", JOYB_1_RIGHT },
/* joystick 2 */
{ "2joy_1", JOYB_2_1},
{ "2joy_2", JOYB_2_2 },
{ "2joy_3", JOYB_2_3 },
{ "2joy_4", JOYB_2_4 },
{ "2joy_5", JOYB_2_5 },
{ "2joy_6", JOYB_2_6 },
{ "2joy_7", JOYB_2_7 },
{ "2joy_8", JOYB_2_8 },
{ "2joy_9", JOYB_2_9 },
{ "2joy_10", JOYB_2_10 },
{ "2joy_11", JOYB_2_11 },
{ "2joy_12", JOYB_2_12 },
{ "2joy_13", JOYB_2_13 },
{ "2joy_14", JOYB_2_14 },
{ "2joy_15", JOYB_2_15 },
{ "2joy_16", JOYB_2_16 },
{ "2joy_up", JOYB_2_UP },
{ "2joy_down", JOYB_2_DOWN },
{ "2joy_left", JOYB_2_LEFT },
{ "2joy_right", JOYB_2_RIGHT },
/* joystick 3 */
{ "3joy_1", JOYB_3_1},
{ "3joy_2", JOYB_3_2 },
{ "3joy_3", JOYB_3_3 },
{ "3joy_4", JOYB_3_4 },
{ "3joy_5", JOYB_3_5 },
{ "3joy_6", JOYB_3_6 },
{ "3joy_7", JOYB_3_7 },
{ "3joy_8", JOYB_3_8 },
{ "3joy_9", JOYB_3_9 },
{ "3joy_10", JOYB_3_10 },
{ "3joy_11", JOYB_3_11 },
{ "3joy_12", JOYB_3_12 },
{ "3joy_13", JOYB_3_13 },
{ "3joy_14", JOYB_3_14 },
{ "3joy_15", JOYB_3_15 },
{ "3joy_16", JOYB_3_16 },
{ "3joy_up", JOYB_3_UP },
{ "3joy_down", JOYB_3_DOWN },
{ "3joy_left", JOYB_3_LEFT },
{ "3joy_right", JOYB_3_RIGHT },
/* joystick 4 */
{ "4joy_1", JOYB_4_1},
{ "4joy_2", JOYB_4_2 },
{ "4joy_3", JOYB_4_3 },
{ "4joy_4", JOYB_4_4 },
{ "4joy_5", JOYB_4_5 },
{ "4joy_6", JOYB_4_6 },
{ "4joy_7", JOYB_4_7 },
{ "4joy_8", JOYB_4_8 },
{ "4joy_9", JOYB_4_9 },
{ "4joy_10", JOYB_4_10 },
{ "4joy_11", JOYB_4_11 },
{ "4joy_12", JOYB_4_12 },
{ "4joy_13", JOYB_4_13 },
{ "4joy_14", JOYB_4_14 },
{ "4joy_15", JOYB_4_15 },
{ "4joy_16", JOYB_4_16 },
{ "4joy_up", JOYB_4_UP },
{ "4joy_down", JOYB_4_DOWN },
{ "4joy_left", JOYB_4_LEFT },
{ "4joy_right", JOYB_4_RIGHT },
{ 0, 0 }
};

unsigned joy_code(const char* name)
{
	struct joy_entry* i;

	for(i=JOY;i->name;++i)
		if (strcmp(name, i->name)==0)
			return i->code;

	return JOYB_MAX;
}
