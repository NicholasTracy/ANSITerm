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

#include <string.h>
#include <stdlib.h>

//Default settings for ANSITerm constructor (default Serial stream. VT100 terminal level, 4 Bit color mode)
ANSITerm::ANSITerm()
{

}
ANSITerm::begin(Stream &s=Serial, uint8_t t=VT100, uint8_t c=4_BIT) 
: stream(s), termLevel(t), colorMode(c)
{


}
ANSITerm::end()
{


}
ANSITerm::setTerminalLevel()
{


}
ANSITerm::setColorMode()
{


}
ANSITerm::getTerminalLevel()
{


}
ANSITerm::getColorMode()
{


}
ANSITerm::setRBufferLen()
{


}
ANSITerm::setTBufferLen()
{

}
ANSITerm::getRBufferLen()
{

}
ANSITerm::setTBufferLen()
{

}
ANSITerm::write(char)
{
	
}
ANSITerm::beep(void)
{


}
ANSITerm::read(void)
{
	int8_t rxData = stream->read();
	if(rxData != -1){
		parse(rxData);
	}

}
ANSITerm::clear()
{


}
ANSITerm::parse()
{


}