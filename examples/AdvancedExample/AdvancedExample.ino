/*
 * ANSITerm Library - Arduino ANSI Terminal Control Library
 * https://github.com/NicholasTracy/ANSITerm
 * 
 * Author: Nicholas Tracy (2024)
 * https://github.com/NicholasTracy
 *
 * File: AdvancedExample.ino
 * 
 * Description:
 * This example demonstrates the advanced capabilities of the ANSITerm library over a USB serial connection, 
 * including drawing boxes, tables, buttons, and animating a loading bar.
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

    // Initialize the terminal with color names
    terminal.begin(true, true, true, true, "cyan", "black"); // cyan text on black background

    // === Advanced Example Operations ===

    // Display a title with double-line box
    terminal.setTextColor("yellow");
    terminal.drawDoubleBox(1, 10, 3, 70);
    terminal.writeTextAt(2, 35, "Advanced Terminal Interface");

    // Draw a loading bar container with light lines
    terminal.setTextColor("white");
    terminal.drawBox(6, 20, 8, 60);
    terminal.writeTextAt(7, 22, "Loading: [                    ]");

    // Animate the loading bar filling up and then going back down
    animateLoadingBar(7, 32, 20); // Start at row 7, column 32, with a length of 20 characters

    // Draw an interactive button with double lines
    terminal.setTextColor("cyan");
    terminal.drawDoubleButton(10, 20, 12, 50, ANSI_ARROW_RIGHT " Start Again " ANSI_ARROW_LEFT);

    // Draw a table below the button
    terminal.setTextColor("green");
    terminal.drawTable(14, 10, 20, 70, 3, 4); // 3 rows and 4 columns table

    // Fill the table with text and symbols
    terminal.writeTextAt(15, 12, ANSI_HEART " Row 1, Col 1");
    terminal.writeTextAt(15, 30, ANSI_DIAMOND " Row 1, Col 2");
    terminal.writeTextAt(15, 50, ANSI_CLUB " Row 1, Col 3");
    terminal.writeTextAt(15, 68, ANSI_SPADE " Row 1, Col 4");

    terminal.writeTextAt(17, 12, ANSI_CHECKMARK " Row 2, Col 1");
    terminal.writeTextAt(17, 30, ANSI_CROSSMARK " Row 2, Col 2");
    terminal.writeTextAt(17, 50, ANSI_SMILEY " Row 2, Col 3");
    terminal.writeTextAt(17, 68, ANSI_STAR " Row 2, Col 4");

    // Event loop to handle button click
    while (true) {
        // Check if the "Start Again" button was clicked
        if (terminal.detectClick(10, 20, 12, 50)) {
            terminal.clearScreen(); // Clear screen for a new start
            setup(); // Restart the setup to demonstrate the interface again
        }
    }
}

void loop() {
    // Main loop can be left empty for this example
}

// Function to animate a loading bar filling up and then emptying
void animateLoadingBar(uint8_t row, uint8_t startCol, uint8_t length) {
    // Filling up the loading bar
    for (int i = 0; i <= length; i++) {
        terminal.writeTextAt(row, startCol + i, ANSI_BLOCK_FULL);
        delay(100); // Adjust the delay for animation speed
    }

    delay(500); // Pause briefly before reversing

    // Emptying the loading bar
    for (int i = length; i >= 0; i--) {
        terminal.deleteAtPosition(row, startCol + i);
        delay(100); // Adjust the delay for animation speed
    }
}
