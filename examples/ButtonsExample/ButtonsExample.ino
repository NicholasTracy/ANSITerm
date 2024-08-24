/*
 * ANSITerm Library - Arduino ANSI Terminal Control Library
 * https://github.com/NicholasTracy/ANSITerm
 * 
 * Author: Nicholas Tracy (2024)
 * https://github.com/NicholasTracy
 *
 * File: ButtonsExample.ino
 * 
 * Description:
 * This example demonstrates how to create and interact with buttons using the ANSITerm library. 
 * It showcases the use of both single-line and double-line box buttons and detects user interaction 
 * through the serial connection. Additionally, the example includes interaction with the hardware by 
 * toggling the onboard LED when a button is clicked.
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

// State to keep track of LED status
bool ledState = false;

void setup() {
    // Begin serial communication at 9600 baud
    Serial.begin(9600);

    // Wait for a successful serial connection
    while (!Serial) {
        ; // Wait for serial port to connect. Needed for native USB port only boards
    }

    // Initialize the terminal with color names
    terminal.begin(true, true, true, true, "white", "black"); // white text on black background

    // Initialize the onboard LED pin as an output
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, ledState); // Set initial LED state

    // === Buttons Example Operations ===

    // Display a title
    terminal.setTextColor("cyan");
    terminal.writeTextAt(2, 5, "Button Interaction Example");

    // Draw a single-line box button
    terminal.setTextColor("yellow");
    terminal.drawButton(5, 10, 7, 40, "Toggle LED");

    // Draw a double-line box button
    terminal.setTextColor("green");
    terminal.drawDoubleButton(9, 10, 11, 40, "Exit");

    // Instructions for interaction
    terminal.setTextColor("white");
    terminal.writeTextAt(13, 5, "Click a button to see the response.");

    // Event loop to handle button clicks
    while (true) {
        // Check if the "Toggle LED" button was clicked
        if (terminal.detectClick(5, 10, 7, 40)) {
            ledState = !ledState; // Toggle the LED state
            digitalWrite(LED_BUILTIN, ledState); // Update the LED state
            terminal.writeTextAt(15, 5, ledState ? "LED is ON" : "LED is OFF");
        }

        // Check if the "Exit" button was clicked
        if (terminal.detectClick(9, 10, 11, 40)) {
            terminal.clearScreen();
            terminal.writeTextAt(6, 10, "Exiting...");
            break; // Exit loop after handling click
        }
    }
}

void loop() {
    // Main loop can be left empty for this example
}