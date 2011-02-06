/**
 * Oxygen Operating System
 * Copyright (C) 2011 Lukas Heidemann
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include <api/types.h>

//----------------------------------------------------------------------------//
// Printing
//----------------------------------------------------------------------------//

/**
 * Prints the given string to the screen.
 *
 * @param str The string to print.
 */
void console_print(const char *str);

/**
 * Prints the given number to the screen in hexadecimal format.
 *
 * @param num The number to print.
 */
void console_print_hex(uintptr_t num);

/**
 * Prints the given number to the screen in decimal format.
 *
 * @param num The number to print.
 */
void console_print_dec(intptr_t num);

/**
 * Puts a single character to the screen.
 *
 * @param c The character to put.
 */
void console_put(char c);

/**
 * Clears the console leaving a blank screen and the cursor at the upper right
 * corner.
 */
void console_clear();

//----------------------------------------------------------------------------//
// Color
//----------------------------------------------------------------------------//

#define COLOR_BLACK         0x00
#define COLOR_BLUE          0x01
#define COLOR_GREEN         0x02
#define COLOR_CYAN          0x03
#define COLOR_RED           0x04
#define COLOR_MAGENTA       0x05
#define COLOR_BROWN         0x06
#define COLOR_LIGHT_GRAY    0x07
#define COLOR_DARK_GRAY     0x08
#define COLOR_LIGHT_BLUE    0x09
#define COLOR_LIGHT_GREEN   0x0A
#define COLOR_LIGHT_CYAN    0x0B
#define COLOR_LIGHT_RED     0x0C
#define COLOR_LIGHT_MAGENTA 0x0D
#define COLOR_LIGHT_BRWOM   0x0E
#define COLOR_WHITE         0x0F

/**
 * Sets the console's foreground color.
 *
 * @param color The new foreground color.
 */
void console_set_foreground(uint8_t color);

/**
 * Sets the console's background color.
 *
 * @param color The new background color.
 */
void console_set_background(uint8_t color);

/**
 * Returns the console's foreground color.
 *
 * @return The foreground color.
 */
uint8_t console_get_foreground();

/**
 * Returns the console's background color.
 *
 * @return The background color.
 */
uint8_t console_get_background();

//----------------------------------------------------------------------------//
// Cursor
//----------------------------------------------------------------------------//

/**
 * Returns the console's cursor position on the X axis.
 *
 * @return Cursor's X position.
 */
size_t console_get_x();

/**
 * Returns the console's cursor position on the Y axis.
 *
 * @return Cursor's Y position.
 */
size_t console_get_y();

/**
 * Sets the console's cursor position on the X axis.
 *
 * @param x The new cursor's x position.
 */
void console_set_x(size_t x);

/**
 * Sets the console's cursor position on the Y axis.
 *
 * @param y The new cursor's y position.
 */
void console_set_y(size_t y);

//----------------------------------------------------------------------------//
// Size
//----------------------------------------------------------------------------//

/**
 * Returns the console's width.
 *
 * @return Console's width.
 */
size_t console_get_width();

/**
 * Returns the console's height.
 *
 * @return Console's height.
 */
size_t console_get_height();
