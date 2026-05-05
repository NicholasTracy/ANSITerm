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
 * License: LGPL-3.0 — see LICENSE.txt in the library root.
 */

#include "ANSITerm.h"

namespace {

bool readDecimal(Stream& s, uint16_t& out) {
    out = 0;
    bool any = false;
    while (s.available()) {
        int p = s.peek();
        if (p < '0' || p > '9') {
            break;
        }
        s.read();
        any = true;
        out = static_cast<uint16_t>(out * 10u + static_cast<unsigned>(p - '0'));
        if (out > 500) {
            break;
        }
    }
    return any;
}

bool waitByte(Stream& s, int& out, unsigned long deadlineMs) {
    while (millis() < deadlineMs) {
        if (s.available()) {
            out = s.read();
            return true;
        }
        yield();
    }
    return false;
}

bool expectCharBy(Stream& s, char ch, unsigned long deadlineMs) {
    int c = 0;
    if (!waitByte(s, c, deadlineMs)) {
        return false;
    }
    return c == ch;
}

bool isInternalHorizontalRow(uint8_t row, uint8_t startRow, uint8_t rows, uint8_t rowHeight) {
    for (uint8_t i = 1; i < rows; i++) {
        uint8_t hr = static_cast<uint8_t>(startRow + i * rowHeight);
        if (row == hr) {
            return true;
        }
    }
    return false;
}

bool isInternalVerticalCol(uint8_t col, uint8_t startCol, uint8_t cols, uint8_t colWidth) {
    for (uint8_t j = 1; j < cols; j++) {
        uint8_t vc = static_cast<uint8_t>(startCol + j * colWidth);
        if (col == vc) {
            return true;
        }
    }
    return false;
}

} // namespace

// Constructor: Initializes the ANSITerm object with a specified Stream (e.g., Serial)
ANSITerm::ANSITerm(Stream &stream) : _stream(stream) {}

// Initialization method with various options
void ANSITerm::begin(bool clear, bool resetCursor, bool enableMouse, bool showCursor,
                     const char* defaultTextColor, const char* defaultBackgroundColor) {
    if (clear) {
        clearScreen();
    }
    if (resetCursor) {
        setCursorPosition(1, 1);
    }
    if (enableMouse) {
        enableMouseReporting();
    }
    if (showCursor) {
        this->showCursor();
    } else {
        hideCursor();
    }

    // Set default text color
    setTextColor(defaultTextColor);

    // Set default background color
    setBackgroundColor(defaultBackgroundColor);
}

// Clears the terminal screen
void ANSITerm::clearScreen() {
    sendAnsiCommand(ANSI_CLEAR_SCREEN);
}

// Sets the cursor position to the specified row and column
void ANSITerm::setCursorPosition(uint8_t row, uint8_t col) {
    char command[24];
    snprintf(command, sizeof(command), ANSI_SET_CURSOR_POS, row, col);
    sendAnsiCommand(command);
}

// Resets text formatting (color, bold, etc.) to the default state
void ANSITerm::resetFormatting() {
    sendAnsiCommand(ANSI_RESET_FORMATTING);
}

// Sets the text color using an ANSI color number
void ANSITerm::setTextColor(uint8_t color) {
    char command[24];
    snprintf(command, sizeof(command), ANSI_SET_TEXT_COLOR, color);
    sendAnsiCommand(command);
}

// Sets the text color using a color name or hex value
void ANSITerm::setTextColor(const char* color) {
    if (!color) {
        return;
    }
    if (isdigit(color[0])) {
        setTextColor(atoi(color)); // ANSI number
    } else if (color[0] == '#') {
        setTextColor(hexToAnsi(color)); // Hex color
    } else {
        setTextColor(colorNameToAnsi(color)); // Color name
    }
}

// Sets the background color using an ANSI color number
void ANSITerm::setBackgroundColor(uint8_t color) {
    char command[24];
    snprintf(command, sizeof(command), ANSI_SET_BACKGROUND_COLOR, color);
    sendAnsiCommand(command);
}

// Sets the background color using a color name or hex value
void ANSITerm::setBackgroundColor(const char* color) {
    if (!color) {
        return;
    }
    if (isdigit(color[0])) {
        setBackgroundColor(atoi(color)); // ANSI number
    } else if (color[0] == '#') {
        setBackgroundColor(hexToAnsi(color)); // Hex color
    } else {
        setBackgroundColor(colorNameToAnsi(color)); // Color name
    }
}

// Draws a box using single-line box-drawing characters
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

// Draws a box using double-line box-drawing characters
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

// Draws a single-line grid inside (startRow,startCol)-(endRow,endCol).
// rows/cols are cell counts; interior space is split evenly (integer division).
void ANSITerm::drawTable(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol, uint8_t rows, uint8_t cols) {
    drawBox(startRow, startCol, endRow, endCol);

    if (rows < 1 || cols < 1 || endRow <= startRow || endCol <= startCol) {
        return;
    }

    uint8_t rowSpan = static_cast<uint8_t>(endRow - startRow);
    uint8_t colSpan = static_cast<uint8_t>(endCol - startCol);
    uint8_t rowHeight = rowSpan / rows;
    uint8_t colWidth = colSpan / cols;

    if (rowHeight < 1 || colWidth < 1) {
        return;
    }

    // Top/bottom borders: replace horizontal segments with tees where vertical dividers meet the frame
    for (uint8_t j = 1; j < cols; j++) {
        uint8_t vc = static_cast<uint8_t>(startCol + j * colWidth);
        setCursorPosition(startRow, vc);
        _stream.print(ANSI_BOX_T_INTERSECT);
        setCursorPosition(endRow, vc);
        _stream.print(ANSI_BOX_B_INTERSECT);
    }

    // Left/right borders: tees where horizontal dividers meet the frame
    for (uint8_t i = 1; i < rows; i++) {
        uint8_t hr = static_cast<uint8_t>(startRow + i * rowHeight);
        setCursorPosition(hr, startCol);
        _stream.print(ANSI_BOX_L_INTERSECT);
        setCursorPosition(hr, endCol);
        _stream.print(ANSI_BOX_R_INTERSECT);
    }

    // Full horizontal rules on each interior grid row (crossings filled where verticals meet)
    for (uint8_t i = 1; i < rows; i++) {
        uint8_t hr = static_cast<uint8_t>(startRow + i * rowHeight);
        for (uint8_t col = static_cast<uint8_t>(startCol + 1); col < endCol; col++) {
            setCursorPosition(hr, col);
            if (isInternalVerticalCol(col, startCol, cols, colWidth)) {
                _stream.print(ANSI_BOX_CROSS);
            } else {
                _stream.print(ANSI_BOX_HORIZONTAL);
            }
        }
    }

    // Vertical segments on interior columns (skip rows already drawn as part of horizontal rules)
    for (uint8_t j = 1; j < cols; j++) {
        uint8_t vc = static_cast<uint8_t>(startCol + j * colWidth);
        for (uint8_t row = static_cast<uint8_t>(startRow + 1); row < endRow; row++) {
            if (isInternalHorizontalRow(row, startRow, rows, rowHeight)) {
                continue;
            }
            setCursorPosition(row, vc);
            _stream.print(ANSI_BOX_VERTICAL);
        }
    }
}

// Writes text at the specified row and column
void ANSITerm::writeTextAt(uint8_t row, uint8_t col, const char* text) {
    if (!text) {
        return;
    }
    setCursorPosition(row, col);
    _stream.print(text);
}

// Deletes (clears) content at a specific row and column, leaving it blank
void ANSITerm::deleteAtPosition(uint8_t row, uint8_t col) {
    setCursorPosition(row, col);
    _stream.print(' '); // Print a space to clear the position
}

// Draws a button with centered text using single-line box-drawing characters
void ANSITerm::drawButton(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol, const char* text) {
    if (!text) {
        return;
    }
    drawBox(startRow, startCol, endRow, endCol);

    uint8_t textLength = strlen(text);
    uint8_t boxWidth = endCol - startCol - 1;
    uint8_t textRow = startRow + (endRow - startRow) / 2;
    uint8_t textCol = startCol + (boxWidth - textLength) / 2 + 1;

    writeTextAt(textRow, textCol, text);
}

// Draws a button with centered text using double-line box-drawing characters
void ANSITerm::drawDoubleButton(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol, const char* text) {
    if (!text) {
        return;
    }
    drawDoubleBox(startRow, startCol, endRow, endCol);

    uint8_t textLength = strlen(text);
    uint8_t boxWidth = endCol - startCol - 1;
    uint8_t textRow = startRow + (endRow - startRow) / 2;
    uint8_t textCol = startCol + (boxWidth - textLength) / 2 + 1;

    writeTextAt(textRow, textCol, text);
}

// Detects if a mouse click occurs within the specified button or box boundaries
bool ANSITerm::detectClick(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol) {
    uint8_t row, col;
    parseMouseReport(row, col);
    return (row >= startRow && row <= endRow && col >= startCol && col <= endCol);
}

// Enables mouse tracking in the terminal
void ANSITerm::enableMouseReporting() {
    sendAnsiCommand("\033[?1000h"); // Enable basic mouse tracking
    sendAnsiCommand("\033[?1015h"); // Enable extended coordinates
    sendAnsiCommand("\033[?1006h"); // Enable SGR (1006) extended mode
}

// Disables mouse tracking
void ANSITerm::disableMouseReporting() {
    sendAnsiCommand("\033[?1000l"); // Disable basic mouse tracking
    sendAnsiCommand("\033[?1015l"); // Disable extended coordinates
    sendAnsiCommand("\033[?1006l"); // Disable SGR (1006) extended mode
}

// Parses the mouse report to determine the row and column where a click occurred
void ANSITerm::parseMouseReport(uint8_t& row, uint8_t& col) {
    row = col = 0;
    // SGR extended mouse (1006): CSI < Pb ; Px ; Py M  or  m — Px/Py are column/row (1-based cells).
    // Waits briefly for each byte so USB-serial packets split across frames still parse.
    // Do not consume non-ESC bytes — leaves typed characters for the sketch.
    // Terminals that do not encode clicks as this sequence (e.g. PuTTY raw Serial) never send input here.
    const unsigned long deadline = millis() + 75;
    if (!_stream.available()) {
        return;
    }
    if (_stream.peek() != '\033') {
        return;
    }
    int c = 0;
    if (!waitByte(_stream, c, deadline) || c != '\033') {
        return;
    }
    if (!expectCharBy(_stream, '[', deadline)) {
        return;
    }
    if (!expectCharBy(_stream, '<', deadline)) {
        return;
    }
    uint16_t btn = 0;
    uint16_t px = 0;
    uint16_t py = 0;
    if (!readDecimal(_stream, btn)) {
        return;
    }
    if (!expectCharBy(_stream, ';', deadline)) {
        return;
    }
    if (!readDecimal(_stream, px)) {
        return;
    }
    if (!expectCharBy(_stream, ';', deadline)) {
        return;
    }
    if (!readDecimal(_stream, py)) {
        return;
    }
    if (!waitByte(_stream, c, deadline)) {
        return;
    }
    if (c != 'M' && c != 'm') {
        return;
    }
    (void)btn;
    col = (px > 255) ? 255 : static_cast<uint8_t>(px);
    row = (py > 255) ? 255 : static_cast<uint8_t>(py);
}

// Makes the cursor visible
void ANSITerm::showCursor() {
    sendAnsiCommand(ANSI_CURSOR_SHOW);
}

// Hides the cursor
void ANSITerm::hideCursor() {
    sendAnsiCommand(ANSI_CURSOR_HIDE);
}

// Converts a hex color code to an ANSI color code
uint8_t ANSITerm::hexToAnsi(const char* hex) {
    if (!hex) {
        return 0;
    }
    if (hex[0] == '#') {
        hex++;
    }
    long int rgb = strtol(hex, NULL, 16);
    uint8_t r = (rgb >> 16) & 0xFF;
    uint8_t g = (rgb >> 8) & 0xFF;
    uint8_t b = rgb & 0xFF;
    return rgbToAnsi(r, g, b);
}

// Converts a color name to an ANSI color code
uint8_t ANSITerm::colorNameToAnsi(const char* name) {
    if (!name) {
        return 0;
    }
    Color color = getWebColor(name);
    return rgbToAnsi(color.r, color.g, color.b);
}

// Converts RGB values to an ANSI color code
uint8_t ANSITerm::rgbToAnsi(uint8_t r, uint8_t g, uint8_t b) {
    uint8_t red = (r / 51) * 36;
    uint8_t green = (g / 51) * 6;
    uint8_t blue = b / 51;
    return 16 + red + green + blue;
}

// Sends an ANSI command to the terminal
void ANSITerm::sendAnsiCommand(const char* command) {
    _stream.print(command);
}