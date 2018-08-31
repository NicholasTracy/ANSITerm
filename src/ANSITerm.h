/* ANSITerm Library
 * ---------------
 * ANSITerm.h
 * This file provide definitions for the complete list of ANSI escape sequences
 * as well as the necasarry deffinitions for the library.
 * ---------------
 * Provides full ANSI escape sequence control of terminals over serial.
 * Primarilly for use with virtual terms e.g. PuTTy and xTerm 
 * ---------------
 * Original code developed and distributed as BasicTerm.
 * https://github.com/nottwo/BasicTerm
 * Copyright 2011 Trannie Carter <borys@nottwo.org>
 * ---------------
 * Modified/rewritten and redistributed as ANSITerm.
 * Copyright 2018 Nicholas Tracy <https://github.com/NicholasTracy>
 * ---------------
 * Licensed for use under the terms of the GNU Lesser General Public License v3
 * ---------------
 */

#ifndef ANSITERM_H
#define ANSITERM_H

#include <Arduino.h>

#define ANSITERM_MAJOR_VERSION          0
#define ANSITERM_MINOR_VERSION          1
#define ANSITERM_BUGFIX_VERSION			0
	
//All ASCII codes are in octals
//This reduces the code footprint and speeds up serial

//Escape Sequence Start
	#define AT_ESC			033 	//ESC
	#define AT_LB			133 	//[
	
//Other ASCII
	#define AT_NULL			000		//Null
	#define AT_SC			073 	//; Used to chain some values in certain commands.
	#define AT_C			072		//: Used to chain some values, used only for terminals that use it.
	#define AT_DEL			177		//Delete
	
//Set Text Attributes
	#define AT_NORMAL		0		//0
	#define AT_BOLD 		1		//1
	#define AT_FAINT		2		//2
	#define AT_ITAL			3		//3 Not Widely Supported
	#define AT_UNDERSCORE	4		//4
	#define AT_BLINK		5		//5
	#define AT_RBLINK		6		//6 Not Widely Supported
	#define AT_REVERSE		7		//7
	#define AT_CONCEALED	8		//8 Not Widely Supported
	#define AT_STRIKE		9		//9
	
	#define AT_BLACK		0		//x0..
	#define AT_RED			1
	#define AT_GREEN 		2
	#define AT_YELLOW 		3
	#define AT_BLUE			4
	#define AT_MAGENTA		5
	#define AT_CYAN			6
	#define AT_WHITE		7		//..x7
	
	#define AT_FG			3       //3x 	foreground	 normal 	4-bit
	#deinfe AT_BG			4		//4x 	background	 normal 	4-bit
	#define AT_FGB			9		//9x 	foreground	 bright 	4-bit
	#define AT_BGB			10		//10x 	background	 bright 	4-bit
	#define AT_FG8			38		//38 	foreground	 8-bit 		256 color.
	#define AT_BG8			48		//48 	backgrond	 8-bit 		256 color.
	#define AT_8BIT			5		//5 Used to select from 256 color pallete mode
	#define AT_24BIT		2		//2 Used to set RGB Value "true color"

	#define AT_OFF			2		//2x Toggle off specific attributes

	#define AT_SGR			155		//m Command suffix.

//Cursor Position
	#define AT_CSET			102		//f
	//#define AT_CSET		72 		//H
	#define AT_CUP					//A
	#define AT_CDN					//B
	#define AT_CFWD					//C
	#define AT_CBWD					//D
	#define AT_SCP					//s
	#define AT_RCP					//u
	#define AT_DSR
	
//Screen Commands
	#define AT_CLS					//2J
	#define AT_

class ANSITerm {

	public:
        ANSITerm(Stream *);
		boolean begin(uint8_t
		
    private: 
        Stream *serial;
};

#endif
