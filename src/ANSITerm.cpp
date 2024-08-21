/* ANSITerm Library
 * https://github.com/NicholasTracy/ANSITerm
 * 2024 Nicholas Tracy <https://github.com/NicholasTracy>
 * ---------------
 * ANSITerm.cpp
 * Original concept based on the BasicTerm library.
 * https://github.com/nottwo/BasicTerm
 * 2011 Trannie Carter <borys@nottwo.org>
 * ---------------
 * Licensed for use under the terms of the General Public License v3
 * ---------------
 */

#include "ANSITerm.h"
#include <SD.h> // For handling .ans file reading, assuming SD card is used

ANSITerm::ANSITerm(Stream &stream) : _stream(stream) {}

void ANSITerm::begin() {
    // Initialization code, if necessary
    enableMouseReporting();
}

void ANSITerm::clearScreen() {
    sendAnsiCommand(ANSI_CLEAR_SCREEN);
}

void ANSITerm::deleteAtPosition(uint8_t row, uint8_t col) {
    setCursorPosition(row, col);
    _stream.print(' '); // Print a space to "delete" the content at the specified position
}

void ANSITerm::setCursorPosition(uint8_t row, uint8_t col) {
    char command[16];
    sprintf(command, ANSI_SET_CURSOR_POS, row, col);
    sendAnsiCommand(command);
}

void ANSITerm::setTextColor(uint8_t color) {
    char command[8];
    sprintf(command, ANSI_SET_TEXT_COLOR, color);
    sendAnsiCommand(command);
}

void ANSITerm::setBackgroundColor(uint8_t color) {
    char command[8];
    sprintf(command, ANSI_SET_BACKGROUND_COLOR, color);
    sendAnsiCommand(command);
}

void ANSITerm::resetFormatting() {
    sendAnsiCommand(ANSI_RESET_FORMATTING);
}

void ANSITerm::setTextColorHex(const char* hex) {
    setTextColor(hexToAnsi(hex));
}

void ANSITerm::setBackgroundColorHex(const char* hex) {
    setBackgroundColor(hexToAnsi(hex));
}

void ANSITerm::setTextColorRGB(uint8_t r, uint8_t g, uint8_t b) {
    setTextColor(rgbToAnsi(r, g, b));
}

void ANSITerm::setBackgroundColorRGB(uint8_t r, uint8_t g, uint8_t b) {
    setBackgroundColor(rgbToAnsi(r, g, b));
}

void ANSITerm::setTextColorName(const char* name) {
    Color color = getWebColor(name);
    setTextColorRGB(color.r, color.g, color.b);
}

void ANSITerm::setBackgroundColorName(const char* name) {
    Color color = getWebColor(name);
    setBackgroundColorRGB(color.r, color.g, color.b);
}

void ANSITerm::drawBox(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol) {
    setCursorPosition(startRow, startCol);
    _stream.print(ANSI_BOX_TOP_LEFT);
    for (uint8_t col = startCol + 1; col < endCol; col++) {
        _stream.print(ANSI_BOX_HORIZONTAL);
    }
    _stream.print(ANSI_BOX_TOP_RIGHT);

    for (uint8_t row = startRow + 1; row < endRow; row++) {
        setCursorPosition(row, startCol);
        _stream.print(ANSI_BOX_VERTICAL);
        setCursorPosition(row, endCol);
        _stream.print(ANSI_BOX_VERTICAL);
    }

    setCursorPosition(endRow, startCol);
    _stream.print(ANSI_BOX_BOTTOM_LEFT);
    for (uint8_t col = startCol + 1; col < endCol; col++) {
        _stream.print(ANSI_BOX_HORIZONTAL);
    }
    _stream.print(ANSI_BOX_BOTTOM_RIGHT);
}

void ANSITerm::drawDoubleBox(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol) {
    setCursorPosition(startRow, startCol);
    _stream.print(ANSI_BOX_DOUBLE_TOP_LEFT);
    for (uint8_t col = startCol + 1; col < endCol; col++) {
        _stream.print(ANSI_BOX_DOUBLE_HORIZONTAL);
    }
    _stream.print(ANSI_BOX_DOUBLE_TOP_RIGHT);

    for (uint8_t row = startRow + 1; row < endRow; row++) {
        setCursorPosition(row, startCol);
        _stream.print(ANSI_BOX_DOUBLE_VERTICAL);
        setCursorPosition(row, endCol);
        _stream.print(ANSI_BOX_DOUBLE_VERTICAL);
    }

    setCursorPosition(endRow, startCol);
    _stream.print(ANSI_BOX_DOUBLE_BOTTOM_LEFT);
    for (uint8_t col = startCol + 1; col < endCol; col++) {
        _stream.print(ANSI_BOX_DOUBLE_HORIZONTAL);
    }
    _stream.print(ANSI_BOX_DOUBLE_BOTTOM_RIGHT);
}

void ANSITerm::drawDoubleButton(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol, const char* text) {
    drawDoubleBox(startRow, startCol, endRow, endCol);

    uint8_t textLength = strlen(text);
    uint8_t boxWidth = endCol - startCol - 1;
    uint8_t textRow = startRow + (endRow - startRow) / 2;
    uint8_t textCol = startCol + (boxWidth - textLength) / 2 + 1;

    writeTextAt(textRow, textCol, text);
}

void ANSITerm::drawTable(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol, uint8_t rows, uint8_t cols) {
    drawBox(startRow, startCol, endRow, endCol);

    uint8_t rowHeight = (endRow - startRow) / rows;
    uint8_t colWidth = (endCol - startCol) / cols;

    for (uint8_t r = 1; r < rows; r++) {
        setCursorPosition(startRow + r * rowHeight, startCol);
        _stream.print(ANSI_BOX_L_INTERSECT);
        for (uint8_t c = 1; c < cols; c++) {
            setCursorPosition(startRow + r * rowHeight, startCol + c * colWidth);
            _stream.print(ANSI_BOX_CROSS);
        }
        setCursorPosition(startRow + r * rowHeight, endCol);
        _stream.print(ANSI_BOX_R_INTERSECT);
    }

    for (uint8_t c = 1; c < cols; c++) {
        setCursorPosition(startRow, startCol + c * colWidth);
        _stream.print(ANSI_BOX_T_INTERSECT);
        setCursorPosition(endRow, startCol + c * colWidth);
        _stream.print(ANSI_BOX_B_INTERSECT);
    }

    for (uint8_t r = startRow + 1; r < endRow; r++) {
        for (uint8_t c = 1; c < cols; c++) {
            setCursorPosition(r, startCol + c * colWidth);
            _stream.print(ANSI_BOX_VERTICAL);
        }
    }
}

void ANSITerm::writeTextAt(uint8_t row, uint8_t col, const char* text) {
    setCursorPosition(row, col);
    _stream.print(text);
}

void ANSITerm::drawButton(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol, const char* text) {
    drawBox(startRow, startCol, endRow, endCol);

    uint8_t textLength = strlen(text);
    uint8_t boxWidth = endCol - startCol - 1;
    uint8_t textRow = startRow + (endRow - startRow) / 2;
    uint8_t textCol = startCol + (boxWidth - textLength) / 2 + 1;

    writeTextAt(textRow, textCol, text);
}

bool ANSITerm::detectClick(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol) {
    if (_stream.available()) {
        uint8_t row, col;
        parseMouseReport(row, col);
        
        if (row >= startRow && row <= endRow && col >= startCol && col <= endCol) {
            return true;
        }
    }
    return false;
}

void ANSITerm::enableMouseReporting() {
    sendAnsiCommand("\033[?1000h"); // Enable basic mouse tracking
    sendAnsiCommand("\033[?1015h"); // Enable extended coordinates
    sendAnsiCommand("\033[?1006h"); // Enable SGR (1006) extended mode
}

void ANSITerm::disableMouseReporting() {
    sendAnsiCommand("\033[?1000l"); // Disable basic mouse tracking
    sendAnsiCommand("\033[?1015l"); // Disable extended coordinates
    sendAnsiCommand("\033[?1006l"); // Disable SGR (1006) extended mode
}

void ANSITerm::parseMouseReport(uint8_t& row, uint8_t& col) {
    char buf[6];
    if (_stream.readBytes(buf, 6) == 6) {
        if (buf[0] == '\033' && buf[1] == '[' && buf[2] == 'M') {
            col = buf[4] - 32;
            row = buf[5] - 32;
        }
    }
}

void ANSITerm::printAnsFile(const char* filename) {
    File ansFile = SD.open(filename);
    if (!ansFile) {
        _stream.println("Error: File not found");
        return;
    }

    while (ansFile.available()) {
        char c = ansFile.read();
        _stream.print(c);
    }

    ansFile.close();
}

// Helper functions for color conversion
uint8_t ANSITerm::rgbToAnsi(uint8_t r, uint8_t g, uint8_t b) {
    uint8_t red = (r / 51) * 36;
    uint8_t green = (g / 51) * 6;
    uint8_t blue = b / 51;
    return 16 + red + green + blue;
}

uint8_t ANSITerm::hexToAnsi(const char* hex) {
    if (hex[0] == '#') hex++;
    long int rgb = strtol(hex, NULL, 16);
    uint8_t r = (rgb >> 16) & 0xFF;
    uint8_t g = (rgb >> 8) & 0xFF;
    uint8_t b = rgb & 0xFF;
    return rgbToAnsi(r, g, b);
}

void ANSITerm::sendAnsiCommand(const char* command) {
    _stream.print(command);
}