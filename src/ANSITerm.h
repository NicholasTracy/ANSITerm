/* ANSITerm Library
 * https://github.com/NicholasTracy/ANSITerm
 * 2018 Nicholas Tracy <https://github.com/NicholasTracy>
 * ---------------
 * ANSITerm.h
 * ---------------
 * Provides full ANSI escape sequence control of capable terminals over the Arduino stream class.
 * Primarilly for use with virtual terms e.g. PuTTy and xTerm 
 * ---------------
 * Original concept based on the BasicTerm library.
 * https://github.com/nottwo/BasicTerm
 * 2011 Trannie Carter <borys@nottwo.org>
 * ---------------
 * Licensed for use under the terms of the General Public License v3
 * ---------------
 */

#ifndef ANSITERM_H
#define ANSITERM_H

#include "constants.h"
#include "Arduino.h"


#define ANSITERM_MAJOR_VERSION          0
#define ANSITERM_MINOR_VERSION          1
#define ANSITERM_BUGFIX_VERSION			0

class ANSITerm {

	public:
		ANSITerm(void);
		void begin(Stream &s=Serial, *TERMLEVEL t=VT100_MODE,  *COLORMODE c=4_BIT_COLOR, );
		void end(void);
		void setStream(Stream &s);
		Stream getStream(void);
		void setTBufferLength(*long);
		void setRBufferLength(*long);
		long getTBufferLength(void);
		long getRBufferLength(void);
		void print(char[] array);
		void print(int8_t number);
		void print(int16_t number);
		void print(int32_t number);
		void print(float number);
		void setColorMode(color_mode *cMode);
		void setTerminalLevel(term_level *tLevel);
		color_mode getColorMode(void);
		term_level getTerminalLevel(void);
		void beep(void);
		uint16_t read(void);
		char[] read(void);
		void cursorShow(void);
		void cursorHide(void);
		void cursorXY(uint8_t x, uint8_t y);
		void cursorUp(uint8_t n);
		void cursorDown(uint8_t n);
		void cursorLeft(uint8_t n);
		void cursorRight(uint8_t n);
		void cursorSave(void);
		void cursorRestore(void);
		uint8_t getCursor(void);
		void clearScreen(void);
		void drawBox(uint8_t x, uint8_t y, uint8_t x2, uint8_t y2);
		void drawLine(uint8_t x, uint8_t y, uint8_t x2, uint8_t y2);
		void setFColor();
		void setBColor();
			getFColor(void);
			getBColor(void);
		void justify(justify_text j);
    private: 
        Stream stream;
		term_level termLevel;
		color_mode colorMode;
		line_style lineStyle;
		justify_text justifyText;
		shading_block shade;
		long maxTxBufferLen;
		long maxRxBufferLen;
		uint8_t *transmitBuffer[maxTxBufferLen];
		uint8_t *receiveBuffer[maxRxBufferLen];
};

// User constants for public functions.
//Color mode definitions.
enum	color_mode{4_BIT, 8_BIT, TRUECOLOR};
//Terminal Level definitions
enum	term_level{VT100_MODE, XTERM_MODE, ANSI_FULL};
//Function key definitions
#define FN_F1
#define FN_F2
#define FN_F3
#define FN_F4
#define FN_F5
#define FN_F6
#define FN_F7
#define FN_F8
#define FN_F9
#define FN_F10
#define FN_F11
#define FN_F12
//Arrow key definitions
#define A_UP
#define A_DOWN
#define A_LEFT
#define A_RIGHT
//Line style definitions
enum line_style{LIGHT, WIDE, DOTTED, DASHED, DOUBLE};
//Text justifications
enum justify_text{LEFT, RIGHT, CENTER};
//Shading
enum shading_block{NONE, LIGHT, MEDIUM, DARK};

//Special Characters
#define

//


#endif
