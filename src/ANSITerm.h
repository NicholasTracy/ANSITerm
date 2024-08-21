/* ANSITerm Library
 * https://github.com/NicholasTracy/ANSITerm
 * 2024 Nicholas Tracy <https://github.com/NicholasTracy>
 * ---------------
 * ANSITerm.h
 * Original concept based on the BasicTerm library.
 * https://github.com/nottwo/BasicTerm
 * 2011 Trannie Carter <borys@nottwo.org>
 * ---------------
 * Licensed for use under the terms of the General Public License v3
 * ---------------
 */

#ifndef ANSITERM_H
#define ANSITERM_H

#include <Arduino.h>
#include "constants.h"
#include "web_colors.h"

class ANSITerm {
public:
    ANSITerm(Stream &stream);

    void begin();
    void clearScreen();
    void setCursorPosition(uint8_t row, uint8_t col);
    void setTextColor(uint8_t color);
    void setBackgroundColor(uint8_t color);
    void resetFormatting();

    // Methods for color management
    void setTextColorHex(const char* hex);
    void setBackgroundColorHex(const char* hex);
    void setTextColorRGB(uint8_t r, uint8_t g, uint8_t b);
    void setBackgroundColorRGB(uint8_t r, uint8_t g, uint8_t b);
    void setTextColorName(const char* name);
    void setBackgroundColorName(const char* name);

    // Methods for graphics
    void drawBox(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol);
    void drawTable(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol, uint8_t rows, uint8_t cols);
    void writeTextAt(uint8_t row, uint8_t col, const char* text);
    void drawDoubleBox(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol);


    // Methods for button creation and mouse interaction
    void drawButton(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol, const char* text);
    void drawDoubleButton(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol, const char* text);
    bool detectClick(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol);

    // Method to delete content at a specific position
    void deleteAtPosition(uint8_t row, uint8_t col);

    // Method to print .ans files
    void printAnsFile(const char* filename);

private:
    Stream &_stream;

    uint8_t rgbToAnsi(uint8_t r, uint8_t g, uint8_t b);
    uint8_t hexToAnsi(const char* hex);
    void sendAnsiCommand(const char* command);
    void enableMouseReporting();
    void disableMouseReporting();
    void parseMouseReport(uint8_t& row, uint8_t& col);
};

#endif // ANSITERM_H




