/* ANSITerm Library
 * https://github.com/NicholasTracy/ANSITerm
 * 2018 Nicholas Tracy <https://github.com/NicholasTracy>
 * ---------------
 * constants.h
 * ---------------
 * defines hex values for human readable sequence names.
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
 
 #ifndef _constants_h_
#define _constants_h_

//All ASCII codes are in hex bytes.
//This reduces the code footprint and speeds up the stream by using a byte array and write.

//Escape Sequence Start
	#define AT_ESC			 					//27		ESC
	
//Other ASCII
	#define AT_NULL							//00		Null
	#define AT_SC			 					//59		';' Used to chain some values in certain commands.
	#define AT_C									//58		':' Used to chain some values, alternative used by some terminals
	#define AT_DEL								//127	Delete
	#define AT_?									//63		Literal '?'
	#define AT_BELL							//07		Bell
	
//Set Text Attributes
	#define AT_NORMAL						//48		0
	#define AT_BOLD 							//49		1
	#define AT_FAINT							//50		2
	#define AT_ITAL								//51		3 Not Widely Supported
	#define AT_UNDERSCORE			//52		4
	#define AT_BLINK							//53		5
	#define AT_RBLINK						//54		6 Not Widely Supported
	#define AT_REVERSE					//55		7
	#define AT_CONCEALED				//56		8 Not Widely Supported
	#define AT_STRIKE						//57		9
	
	#define AT_BLACK							//48		0
	#define AT_RED								//49		1
	#define AT_GREEN 						//50		2
	#define AT_YELLOW 					//51		3
	#define AT_BLUE							//52		4
	#define AT_MAGENTA					//53		5
	#define AT_CYAN							//54		6
	#define AT_WHITE							//55		7
	
	#define AT_FG4								//51,xx			3x	foreground	normal	4-bit
	#define AT_BG4								//52,xx			4x	background	normal	4-bit
	#define AT_FGB								//57,xx			9x	foreground	bright 	4-bit
	#define AT_BGB								//49,48,xx 	10x	background	bright 	4-bit
	#define AT_FG8								//51,56			38 	foreground	 8-bit 		256 color.
	#define AT_BG8								//52,56			48 	backgrond	 8-bit 		256 color.
	#define AT_8BIT								//53				5		Used to select from 256 color pallete mode
	#define AT_24BIT							//50				2		Used to set RGB Value "true color"

	#define AT_OFF								//50,xx			2x	Toggle off specific attributes
	#define AT_SGR								//109			m 	Command suffix.

//Cursor Position
	#define AT_CSET							//xx,AT_SC,xx,102		f	Cursor Position
  //#define AT_CSET			 				//xx,AT_SC,xx,72		H	same as f
	#define AT_CUP								//xx,65							A
	#define AT_CDN								//xx,66							B
	#define AT_CFWD							//xx,67							C
	#define AT_CBWD							//xx,68							D
	#define AT_SCP								//115							s
	#define AT_RCP								//117							u
	#define AT_DSR								//									n
	
//Screen Commands
	#define AT_CLS								//50,74		2J


#endif
