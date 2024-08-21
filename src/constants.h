/* ANSITerm Library
 * https://github.com/NicholasTracy/ANSITerm
 * 2024 Nicholas Tracy <https://github.com/NicholasTracy>
 * ---------------
 * constants.h
 * ---------------
 * Original concept based on the BasicTerm library.
 * https://github.com/nottwo/BasicTerm
 * 2011 Trannie Carter <borys@nottwo.org>
 * ---------------
 * Licensed for use under the terms of the General Public License v3
 * ---------------
 */
 
#ifndef CONSTANTS_H
#define CONSTANTS_H

// ANSI escape codes for screen and cursor control
#define ANSI_CLEAR_SCREEN          "\033[2J"
#define ANSI_SET_CURSOR_POS        "\033[%d;%dH"
#define ANSI_CURSOR_UP             "\033[%dA"
#define ANSI_CURSOR_DOWN           "\033[%dB"
#define ANSI_CURSOR_FORWARD        "\033[%dC"
#define ANSI_CURSOR_BACKWARD       "\033[%dD"
#define ANSI_CURSOR_HIDE           "\033[?25l"
#define ANSI_CURSOR_SHOW           "\033[?25h"
#define ANSI_CURSOR_BLINK          "\033[?12h"
#define ANSI_CURSOR_NO_BLINK       "\033[?12l"

// ANSI escape codes for color management
#define ANSI_SET_TEXT_COLOR        "\033[38;5;%dm"
#define ANSI_SET_BACKGROUND_COLOR  "\033[48;5;%dm"
#define ANSI_RESET_FORMATTING      "\033[0m"

// ANSI box-drawing characters
#define ANSI_BOX_DOUBLE_HORIZONTAL  "\u2550"  // ═
#define ANSI_BOX_DOUBLE_VERTICAL    "\u2551"  // ║
#define ANSI_BOX_DOUBLE_TOP_LEFT    "\u2554"  // ╔
#define ANSI_BOX_DOUBLE_TOP_RIGHT   "\u2557"  // ╗
#define ANSI_BOX_DOUBLE_BOTTOM_LEFT "\u255A"  // ╚
#define ANSI_BOX_DOUBLE_BOTTOM_RIGHT "\u255D" // ╝
#define ANSI_BOX_DOUBLE_T_INTERSECT "\u2566"  // ╦
#define ANSI_BOX_DOUBLE_L_INTERSECT "\u2560"  // ╠
#define ANSI_BOX_DOUBLE_R_INTERSECT "\u2563"  // ╣
#define ANSI_BOX_DOUBLE_B_INTERSECT "\u2569"  // ╩
#define ANSI_BOX_DOUBLE_CROSS       "\u256C"  // ╬

#define ANSI_BOX_HORIZONTAL   "\u2500"  // ─
#define ANSI_BOX_VERTICAL     "\u2502"  // │
#define ANSI_BOX_TOP_LEFT     "\u250C"  // ┌
#define ANSI_BOX_TOP_RIGHT    "\u2510"  // ┐
#define ANSI_BOX_BOTTOM_LEFT  "\u2514"  // └
#define ANSI_BOX_BOTTOM_RIGHT "\u2518"  // ┘
#define ANSI_BOX_T_INTERSECT  "\u252C"  // ┬
#define ANSI_BOX_L_INTERSECT  "\u251C"  // ├
#define ANSI_BOX_R_INTERSECT  "\u2524"  // ┤
#define ANSI_BOX_B_INTERSECT  "\u2534"  // ┴
#define ANSI_BOX_CROSS        "\u253C"  // ┼


// Block Elements
#define ANSI_BLOCK_LIGHT_SHADE   "\u2591"  // ░ Light Shade
#define ANSI_BLOCK_MEDIUM_SHADE  "\u2592"  // ▒ Medium Shade
#define ANSI_BLOCK_DARK_SHADE    "\u2593"  // ▓ Dark Shade
#define ANSI_BLOCK_FULL          "\u2588"  // █ Full Block
#define ANSI_BLOCK_LOWER_HALF    "\u2584"  // ▄ Lower Half Block
#define ANSI_BLOCK_UPPER_HALF    "\u2580"  // ▀ Upper Half Block
#define ANSI_BLOCK_LEFT_HALF     "\u258C"  // ▌ Left Half Block
#define ANSI_BLOCK_RIGHT_HALF    "\u2590"  // ▐ Right Half Block

//Arrows and Pointers
#define ANSI_ARROW_UP            "\u2191"  // ↑
#define ANSI_ARROW_DOWN          "\u2193"  // ↓
#define ANSI_ARROW_LEFT          "\u2190"  // ←
#define ANSI_ARROW_RIGHT         "\u2192"  // →
#define ANSI_ARROW_UP_DOWN       "\u2195"  // ↕
#define ANSI_ARROW_LEFT_RIGHT    "\u2194"  // ↔
#define ANSI_POINTER_LEFT        "\u25C4"  // ◄
#define ANSI_POINTER_RIGHT       "\u25BA"  // ►

//Math Symbols
#define ANSI_PLUS_MINUS          "\u00B1"  // ±
#define ANSI_MULTIPLICATION      "\u00D7"  // ×
#define ANSI_DIVISION            "\u00F7"  // ÷
#define ANSI_INFINITY            "\u221E"  // ∞
#define ANSI_SUMMATION           "\u2211"  // ∑
#define ANSI_SQUARE_ROOT         "\u221A"  // √
#define ANSI_ANGLE               "\u2220"  // ∠

//Shapes
#define ANSI_TRIANGLE_UP         "\u25B2"  // ▲
#define ANSI_TRIANGLE_DOWN       "\u25BC"  // ▼
#define ANSI_TRIANGLE_LEFT       "\u25C0"  // ◀
#define ANSI_TRIANGLE_RIGHT      "\u25B6"  // ▶
#define ANSI_CIRCLE              "\u25CF"  // ●
#define ANSI_HOLLOW_CIRCLE       "\u25CB"  // ○
#define ANSI_SQUARE              "\u25A0"  // ■
#define ANSI_HOLLOW_SQUARE       "\u25A1"  // □
#define ANSI_DIAMOND             "\u25C6"  // ◆
#define ANSI_HOLLOW_DIAMOND      "\u25C7"  // ◇
#define ANSI_HEART               "\u2665"  // ♥
#define ANSI_SPADE               "\u2660"  // ♠
#define ANSI_CLUB                "\u2663"  // ♣
#define ANSI_SMILEY              "\u263A"  // ☺
#define ANSI_SUN                 "\u263C"  // ☼
#define ANSI_MOON                "\u263E"  // ☾

//Miscellaneous Symbols
#define ANSI_CHECKMARK           "\u2713"  // ✓
#define ANSI_CROSSMARK           "\u2717"  // ✗
#define ANSI_BULLET              "\u2022"  // •
#define ANSI_STAR                "\u2605"  // ★
#define ANSI_HOLLOW_STAR         "\u2606"  // ☆
#define ANSI_MALE                "\u2642"  // ♂
#define ANSI_FEMALE              "\u2640"  // ♀
#define ANSI_MUSIC_NOTE          "\u266A"  // ♪
#define ANSI_PEACE               "\u262E"  // ☮
#define ANSI_YIN_YANG            "\u262F"  // ☯
#define ANSI_SNOWFLAKE           "\u2744"  // ❄

#endif // CONSTANTS_H


