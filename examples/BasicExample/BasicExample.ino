/*
 * ANSITerm Library - Arduino ANSI Terminal Control Library
 * https://github.com/NicholasTracy/ANSITerm
 * 
 * Author: Nicholas Tracy (2024)
 * https://github.com/NicholasTracy
 *
 * File: BasicExample.ino
 * 
 * Description:
 * This example demonstrates the basic capabilities of the ANSITerm library, 
 * including setting text and background colors, drawing boxes, and displaying text.
 * It also shows how to initialize the terminal with different color formats 
 * such as color names, hex values, and ANSI color codes.
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

#include <ANSITerm.h>

// Initialize ANSITerm with the Serial stream
ANSITerm terminal(Serial);

void setup() {
    // Begin serial communication at 9600 baud
    Serial.begin(9600);

    // Wait for a successful serial connection
    while (!Serial) {
        ; // Wait for serial port to connect. Needed for native USB port only boards
    }

    // === Terminal Initialization Examples ===
    
    // Example 1: Use default settings (clear screen, reset cursor, enable mouse, show cursor, white text, black background)
    // terminal.begin();
    
    // Example 2: Custom initialization with ANSI color numbers (green text, white background)
    // terminal.begin(true, true, true, true, "32", "47");
    
    // Example 3: Custom initialization with hex colors (custom text and background colors)
    // terminal.begin(true, true, true, true, "#FF5733", "#C70039");
    
    // Example 4: Custom initialization with color names (cyan text, black background)
    terminal.begin(true, true, true, true, "cyan", "black");

    // === Basic Example Operations ===

    // Display a welcome message
    terminal.setTextColor("yellow");
    terminal.writeTextAt(2, 2, "Welcome to ANSITerm!");

    // Draw a simple box with single-line characters
    terminal.setTextColor("white");
    terminal.drawBox(4, 5, 8, 40);

    // Write some text inside the box
    terminal.setTextColor("green");
    terminal.writeTextAt(6, 10, "This is a box drawn with ANSI characters.");

    // Draw a button with centered text
    terminal.setTextColor("cyan");
    terminal.drawButton(10, 20, 12, 50, "Click Me!");

    // Event loop to handle button click
    while (true) {
        // Check if the "Click Me!" button was clicked
        if (terminal.detectClick(10, 20, 12, 50)) {
            terminal.clearScreen(); // Clear the screen
            terminal.writeTextAt(6, 10, "Button Clicked!"); // Display a message
            break; // Exit the loop
        }
    }
}

void loop() {
    // Main loop can be left empty for this basic example
}


