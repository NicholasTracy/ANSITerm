/*
 * ANSITerm Library - Arduino ANSI Terminal Control Library
 * https://github.com/NicholasTracy/ANSITerm
 * 
 * Author: Nicholas Tracy (2024)
 * https://github.com/NicholasTracy
 *
 * File: ANSITerm.h
 * 
 * Description:
 * ANSITerm is an Arduino library designed to provide advanced ANSI escape sequence control 
 * for terminals via the Stream class. This library allows users to create interactive 
 * and graphical terminal interfaces, supporting a wide range of text formatting, color 
 * management, drawing capabilities, and mouse interaction.
 * 
 * The original concept for this library is based on the BasicTerm library by Trannie Carter.
 * https://github.com/nottwo/BasicTerm (2011)
 * 
 * License:
 * This library is licensed under the GNU General Public License v3 (GPLv3).
 * You are free to use, modify, and distribute this library, provided that you comply with 
 * the terms of the GPLv3. There are no warranties, expressed or implied, provided with 
 * this software.
 * 
 * For more details, see the full license at: https://www.gnu.org/licenses/gpl-3.0.en.html
 * 
 */
 
#ifndef CONSTANTS_H
#define CONSTANTS_H

// ANSI escape codes for screen and cursor control
#define ANSI_CLEAR_SCREEN          "\033[2J"        // Clears the terminal screen
#define ANSI_SET_CURSOR_POS        "\033[%d;%dH"    // Sets cursor position to row %d and column %d
#define ANSI_CURSOR_UP             "\033[%dA"       // Moves cursor up by %d rows
#define ANSI_CURSOR_DOWN           "\033[%dB"       // Moves cursor down by %d rows
#define ANSI_CURSOR_FORWARD        "\033[%dC"       // Moves cursor forward (right) by %d columns
#define ANSI_CURSOR_BACKWARD       "\033[%dD"       // Moves cursor backward (left) by %d columns
#define ANSI_CURSOR_HIDE           "\033[?25l"      // Hides the cursor
#define ANSI_CURSOR_SHOW           "\033[?25h"      // Shows the cursor
#define ANSI_CURSOR_BLINK          "\033[?12h"      // Enables cursor blinking
#define ANSI_CURSOR_NO_BLINK       "\033[?12l"      // Disables cursor blinking

// ANSI escape codes for color management
#define ANSI_SET_TEXT_COLOR        "\033[38;5;%dm"  // Sets text color to ANSI color %d
#define ANSI_SET_BACKGROUND_COLOR  "\033[48;5;%dm"  // Sets background color to ANSI color %d
#define ANSI_RESET_FORMATTING      "\033[0m"        // Resets all text formatting to default

// ANSI box-drawing characters
#define ANSI_BOX_HORIZONTAL        "\u2500"  // ─ Single-line horizontal
#define ANSI_BOX_VERTICAL          "\u2502"  // │ Single-line vertical
#define ANSI_BOX_TOP_LEFT          "\u250C"  // ┌ Single-line top-left corner
#define ANSI_BOX_TOP_RIGHT         "\u2510"  // ┐ Single-line top-right corner
#define ANSI_BOX_BOTTOM_LEFT       "\u2514"  // └ Single-line bottom-left corner
#define ANSI_BOX_BOTTOM_RIGHT      "\u2518"  // ┘ Single-line bottom-right corner
#define ANSI_BOX_T_INTERSECT       "\u252C"  // ┬ Single-line T intersection (top)
#define ANSI_BOX_L_INTERSECT       "\u251C"  // ├ Single-line T intersection (left)
#define ANSI_BOX_R_INTERSECT       "\u2524"  // ┤ Single-line T intersection (right)
#define ANSI_BOX_B_INTERSECT       "\u2534"  // ┴ Single-line T intersection (bottom)
#define ANSI_BOX_CROSS             "\u253C"  // ┼ Single-line cross intersection

// ANSI double-line box-drawing characters
#define ANSI_BOX_DOUBLE_HORIZONTAL  "\u2550"  // ═ Double-line horizontal
#define ANSI_BOX_DOUBLE_VERTICAL    "\u2551"  // ║ Double-line vertical
#define ANSI_BOX_DOUBLE_TOP_LEFT    "\u2554"  // ╔ Double-line top-left corner
#define ANSI_BOX_DOUBLE_TOP_RIGHT   "\u2557"  // ╗ Double-line top-right corner
#define ANSI_BOX_DOUBLE_BOTTOM_LEFT "\u255A"  // ╚ Double-line bottom-left corner
#define ANSI_BOX_DOUBLE_BOTTOM_RIGHT "\u255D" // ╝ Double-line bottom-right corner
#define ANSI_BOX_DOUBLE_T_INTERSECT "\u2566"  // ╦ Double-line T intersection (top)
#define ANSI_BOX_DOUBLE_L_INTERSECT "\u2560"  // ╠ Double-line T intersection (left)
#define ANSI_BOX_DOUBLE_R_INTERSECT "\u2563"  // ╣ Double-line T intersection (right)
#define ANSI_BOX_DOUBLE_B_INTERSECT "\u2569"  // ╩ Double-line T intersection (bottom)
#define ANSI_BOX_DOUBLE_CROSS       "\u256C"  // ╬ Double-line cross intersection

// ANSI block elements
#define ANSI_BLOCK_FULL            "\u2588"  // █ Full Block
#define ANSI_BLOCK_HALF            "\u2592"  // ▒ Medium Shade Block
#define ANSI_BLOCK_LIGHT           "\u2591"  // ░ Light Shade Block

// ANSI arrow symbols
#define ANSI_ARROW_UP              "\u2191"  // ↑ Up Arrow
#define ANSI_ARROW_DOWN            "\u2193"  // ↓ Down Arrow
#define ANSI_ARROW_LEFT            "\u2190"  // ← Left Arrow
#define ANSI_ARROW_RIGHT           "\u2192"  // → Right Arrow

// ANSI miscellaneous symbols
#define ANSI_CHECKMARK             "\u2713"  // ✓ Checkmark
#define ANSI_CROSSMARK             "\u2717"  // ✗ Crossmark
#define ANSI_SMILEY                "\u263A"  // ☺ Smiley face
#define ANSI_HEART                 "\u2665"  // ♥ Heart
#define ANSI_DIAMOND               "\u2666"  // ♦ Diamond
#define ANSI_CLUB                  "\u2663"  // ♣ Club
#define ANSI_SPADE                 "\u2660"  // ♠ Spade

#endif // CONSTANTS_H