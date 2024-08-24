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
 * License:
 * This library is licensed under the GNU General Public License v3 (GPLv3).
 * You are free to use, modify, and distribute this library, provided that you comply with 
 * the terms of the GPLv3. There are no warranties, expressed or implied, provided with 
 * this software.
 * 
 * For more details, see the full license at: https://www.gnu.org/licenses/gpl-3.0.en.html
 * 
 */

#ifndef ANSITERM_H
#define ANSITERM_H

#include <Arduino.h>
#include "constants.h"
#include "web_colors.h"

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
    void begin(bool clear = true, bool resetCursor = true, bool enableMouse = true, bool showCursor = true,
               const char* defaultTextColor = "white", const char* defaultBackgroundColor = "black");

    // Clears the terminal screen
    void clearScreen();
    
    // Sets the cursor position to the specified row and column
    void setCursorPosition(uint8_t row, uint8_t col);
    
    // Resets text formatting (color, bold, etc.) to the default state
    void resetFormatting();

    // Sets the text color using an ANSI color number
    void setTextColor(uint8_t color);             
    
    // Sets the text color using a color name or hex value
    void setTextColor(const char* color);         
    
    // Sets the background color using an ANSI color number
    void setBackgroundColor(uint8_t color);       
    
    // Sets the background color using a color name or hex value
    void setBackgroundColor(const char* color);   

    // Draws a box using single-line box-drawing characters
    void drawBox(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol);
    
    // Draws a box using double-line box-drawing characters
    void drawDoubleBox(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol);
    
    // Draws a table with the specified number of rows and columns using single-line box-drawing characters
    void drawTable(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol, uint8_t rows, uint8_t cols);
    
    // Writes text at the specified row and column
    void writeTextAt(uint8_t row, uint8_t col, const char* text);
    
    // Deletes (clears) content at a specific row and column, leaving it blank
    void deleteAtPosition(uint8_t row, uint8_t col);

    // Draws a button with centered text using single-line box-drawing characters
    void drawButton(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol, const char* text);
    
    // Draws a button with centered text using double-line box-drawing characters
    void drawDoubleButton(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol, const char* text);
    
    // Detects if a mouse click occurs within the specified button or box boundaries
    bool detectClick(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol);

    // Enables mouse tracking in the terminal
    void enableMouseReporting();
    
    // Disables mouse tracking
    void disableMouseReporting();
    
    // Parses the mouse report to determine the row and column where a click occurred
    void parseMouseReport(uint8_t& row, uint8_t& col);

    // Makes the cursor visible
    void showCursor();
    
    // Hides the cursor
    void hideCursor();

private:
    // Stream object for communication
    Stream &_stream;

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





