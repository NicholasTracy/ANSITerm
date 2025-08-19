/*
 * ANSITerm Library - Arduino ANSI Terminal Control Library
 * https://github.com/NicholasTracy/ANSITerm
 * 
 * Author: Nicholas Tracy (2024)
 * https://github.com/NicholasTracy
 *
 * File: ANSIFileExample.ino
 * 
 * Description:
 * This example demonstrates how to load and display `.ans` files from an SD card using the ANSITerm library. 
 * It features a rudimentary file tree to navigate through the files on the SD card, allowing the user 
 * to click on a file to load and display it. After displaying the file, an exit button is overlaid 
 * to return to the file tree.
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
#include <SD.h>
#include <SPI.h>

// Initialize ANSITerm with the Serial stream
ANSITerm terminal(Serial);

// Define the chip select pin for the SD card module
const int chipSelect = 4;

// Function declarations
void displayFileTree();
void displayAnsiFile(const char* filename);

void setup() {
    // Begin serial communication at 9600 baud
    Serial.begin(9600);

    // Wait for a successful serial connection
    while (!Serial) {
        ; // Wait for serial port to connect. Needed for native USB port only boards
    }

    // Initialize the terminal
    terminal.begin(true, true, true, true, "white", "black"); // white text on black background

    // Initialize the SD card
    if (!SD.begin(chipSelect)) {
        terminal.writeTextAt(2, 2, "SD card initialization failed!");
        while (true); // Halt the program if SD card initialization fails
    }

    // Display the file tree
    displayFileTree();
}

void loop() {
    // Main loop can be left empty for this example
}

// Function to display the file tree of `.ans` files on the SD card
void displayFileTree() {
    terminal.clearScreen();
    terminal.setTextColor("cyan");
    terminal.writeTextAt(2, 2, "SD Card File Tree:");

    File root = SD.open("/");
    int row = 4;

    while (true) {
        File entry = root.openNextFile();
        if (!entry) {
            break; // No more files
        }

        if (!entry.isDirectory()) {
            String fileName = entry.name();
            if (fileName.endsWith(".ans")) {
                terminal.setTextColor("yellow");
                terminal.writeTextAt(row, 4, fileName.c_str());

                // Check for user click on the file
                if (terminal.detectClick(row, 4, row, 4 + fileName.length())) {
                    displayAnsiFile(fileName.c_str());
                    return;
                }
                row++;
            }
        }
        entry.close();
    }

    root.close();
}

// Function to display a selected `.ans` file
void displayAnsiFile(const char* filename) {
    terminal.clearScreen();

    File file = SD.open(filename);
    if (!file) {
        terminal.setTextColor("red");
        terminal.writeTextAt(2, 2, "Error opening file!");
        return;
    }

    terminal.setTextColor("white");
    int row = 2;
    while (file.available()) {
        String line = file.readStringUntil('\n');
        terminal.writeTextAt(row++, 2, line.c_str());
        if (row >= 20) { // Adjust based on your terminal's row capacity
            break;
        }
    }

    file.close();

    // Draw an overlay exit button
    terminal.setTextColor("green");
    terminal.drawButton(22, 10, 24, 50, "Exit");

    // Wait for the user to click the exit button
    while (true) {
        if (terminal.detectClick(22, 10, 24, 50)) {
            displayFileTree(); // Return to the file tree
            break;
        }
    }
}