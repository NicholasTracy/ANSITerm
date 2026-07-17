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

#ifndef ANSITERM_H
#define ANSITERM_H

#include <Arduino.h>
#include "constants.h"
#include "web_colors.h"

// One logical input event from the terminal (keyboard arrows, Enter, or SGR mouse).
struct ANSITermInput {
    enum Kind : uint8_t {
        None,
        Enter,
        ArrowUp,
        ArrowDown,
        ArrowLeft,
        ArrowRight,
        MousePress,
        MouseRelease
    };
    Kind kind = None;
    uint8_t mouseRow = 0;
    uint8_t mouseCol = 0;
};

class ANSITerm {
public:
    // Constructor: Initializes the ANSITerm object with a specified Stream (e.g., Serial)
    ANSITerm(Stream &stream);

    // Initialization method with various options
    // Parameters:
    // - clear: Clears the screen if true (default: true)
    // - resetCursor: Resets the cursor to the top-left corner if true (default: true)
    // - enableMouse: Enables mouse reporting if true (default: true)
    // - showCursor: Shows the cursor if true; hides it if false (default: true)
    // - defaultTextColor: Sets the default text color (can be ANSI number, hex, or name) (default: "white")
    // - defaultBackgroundColor: Sets the default background color (can be ANSI number, hex, or name) (default: "black")
    // Note: begin(..., enableMouse=false) does not send disable sequences if mouse was already enabled.
    void begin(bool clear = true, bool resetCursor = true, bool enableMouse = true, bool showCursor = true,
               const char* defaultTextColor = "white", const char* defaultBackgroundColor = "black");

    // Clears the terminal screen
    void clearScreen();
    
    // Sets the cursor position (1-based row/column, matching common ANSI terminals)
    void setCursorPosition(uint8_t row, uint8_t col);
    
    // Resets terminal SGR formatting (color, bold, etc.) — not the defaults passed to begin()
    void resetFormatting();

    // Sets the text color using an ANSI 256-color palette index (0–255)
    void setTextColor(uint8_t color);

    // Sets the text color from RGB (quantized to the xterm 6×6×6 cube, indices 16–231)
    void setTextColor(uint8_t r, uint8_t g, uint8_t b);
    
    // Sets the text color using a decimal ANSI index string, #RRGGBB / #RGB hex, or lowercase CSS name
    void setTextColor(const char* color);         
    
    // Sets the background color using an ANSI 256-color palette index (0–255)
    void setBackgroundColor(uint8_t color);

    // Sets the background color from RGB (quantized to the xterm 6×6×6 cube, indices 16–231)
    void setBackgroundColor(uint8_t r, uint8_t g, uint8_t b);
    
    // Sets the background color using a decimal ANSI index string, #RRGGBB / #RGB hex, or lowercase CSS name
    void setBackgroundColor(const char* color);   

    // Draws a box using single-line box-drawing characters.
    // Requires endRow > startRow and endCol > startCol (1-based inclusive corners); otherwise no-op.
    void drawBox(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol);
    
    // Draws a box using double-line box-drawing characters (same geometry rules as drawBox)
    void drawDoubleBox(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol);
    
    // Grid of single-line box-drawing cells; rows/cols = number of cells (>=1). Bounding box uses the same
    // row/column convention as drawBox. Interior spacing uses integer division of (end-start).
    void drawTable(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol, uint8_t rows, uint8_t cols);
    
    // Writes text at the specified row and column
    void writeTextAt(uint8_t row, uint8_t col, const char* text);
    
    // Deletes (clears) content at a specific row and column, leaving it blank
    void deleteAtPosition(uint8_t row, uint8_t col);

    // Draws a button with centered text using single-line box-drawing characters.
    // Oversized labels are left-aligned in the interior (byte length; UTF-8 wide glyphs may mis-center).
    void drawButton(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol, const char* text);
    
    // Draws a button with centered text using double-line box-drawing characters
    void drawDoubleButton(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol, const char* text);
    
    // True when an SGR mouse press (not release) falls inside the inclusive rectangle.
    // Consumes one mouse report from the stream when present.
    bool detectClick(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol);

    // Enables mouse tracking in the terminal (SGR/1006; parser supports SGR only)
    void enableMouseReporting();
    
    // Disables mouse tracking
    void disableMouseReporting();
    
    // Parses one SGR mouse press into row/col (1-based). Release events are consumed but leave 0,0.
    // On timeout/malformed input, row and col are 0. Prefer pollInput() for new code.
    void parseMouseReport(uint8_t& row, uint8_t& col);

    // Reads the next Enter, CSI arrow key, or SGR mouse press/release.
    // Non-blocking when no bytes are waiting; incomplete ESC sequences wait up to ~75 ms.
    bool pollInput(ANSITermInput& out);

    // Best-effort: true once when the USB CDC host re-enumerates after disconnect (native-USB Arduino cores with USBCON).
    // UART-only boards usually reset on serial-open (DTR); Network streams are always false. Call from loop while idle.
    bool reconnected();

    // Makes the cursor visible
    void showCursor();
    
    // Hides the cursor
    void hideCursor();

private:
    void syncHostSessionState();
    static bool validBox(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol);
    void drawButtonLabel(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol, const char* text);

    // Stream object for communication
    Stream &_stream;

    // Tracks USB CDC host configuration for reconnected() (USBCON builds only).
    bool _hostUsbConfigured = false;

    // Converts RGB values to an ANSI color code
    uint8_t rgbToAnsi(uint8_t r, uint8_t g, uint8_t b);
    
    // Converts a hex color code to an ANSI color code
    uint8_t hexToAnsi(const char* hex);
    
    // Converts a color name to an ANSI color code
    uint8_t colorNameToAnsi(const char* name);

    // Sends an ANSI command to the terminal
    void sendAnsiCommand(const char* command);
};

#endif // ANSITERM_H
