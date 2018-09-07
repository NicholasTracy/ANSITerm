/* ANSITerm Library
 * https://github.com/NicholasTracy/ANSITerm
 * 2018 Nicholas Tracy <https://github.com/NicholasTracy>
 * ---------------
 * ANSITerm.cpp
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

#include "ANSITerm.h"

uint8_t arrayPointer
           
//Store each command as an array of type byte. Each can be executed using a simple loop
	byte sequence[]={};

	while(arrayPointer<sizeof(sequence))
	{
		Stream.write(sequence[arrayPointer]);
		arrayPointer++;
	}
	arrayPointer = 0;