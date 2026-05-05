/*
 * ANSITerm — ANSFileExample
 * https://github.com/NicholasTracy/ANSITerm
 *
 * What you learn: Listing .ans files on SD, opening one line-by-line with writeTextAt,
 * and overlaying drawButton + detectClick() to return to the file list.
 *
 * Hardware: SD shield/module on SPI (default chip select 4 — change chipSelect for your wiring).
 * Needs enough RAM for SD + String lines; skipped on small AVR boards in CI.
 *
 * License: LGPL-3.0 — see LICENSE.txt in the library root.
 */

#include <ANSITerm.h>
#include <SD.h>
#include <SPI.h>

ANSITerm terminal(Serial);

const int chipSelect = 4;

void displayAnsiFile(const char* filename);
void displayFileTree();

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ;
    }

    terminal.begin(true, true, true, true, "white", "black");

    if (!SD.begin(chipSelect)) {
        terminal.writeTextAt(2, 2, "SD card init failed — check wiring and CS pin.");
        while (true) {
            delay(1000);
        }
    }

    displayFileTree();
}

void loop() {
}

void displayFileTree() {
tree_top:
    terminal.clearScreen();
    terminal.setTextColor("cyan");
    terminal.writeTextAt(2, 2, "SD card — .ans files (click a name):");

    File root = SD.open("/");
    int row = 4;

    while (true) {
        if (terminal.pollHostTerminalReconnect()) {
            root.close();
            goto tree_top;
        }

        File entry = root.openNextFile();
        if (!entry) {
            break;
        }

        if (!entry.isDirectory()) {
            String fileName = entry.name();
            if (fileName.endsWith(".ans")) {
                terminal.setTextColor("yellow");
                terminal.writeTextAt(row, 4, fileName.c_str());

                if (terminal.pollHostTerminalReconnect()) {
                    entry.close();
                    root.close();
                    goto tree_top;
                }
                if (terminal.detectClick(row, 4, row, 4 + fileName.length())) {
                    entry.close();
                    root.close();
                    displayAnsiFile(fileName.c_str());
                    goto tree_top;
                }
                row++;
            }
        }
        entry.close();
    }

    root.close();

    while (true) {
        if (terminal.pollHostTerminalReconnect()) {
            goto tree_top;
        }
        delay(50);
    }
}

void displayAnsiFile(const char* filename) {
    terminal.clearScreen();

    File file = SD.open(filename);
    if (!file) {
        terminal.setTextColor("red");
        terminal.writeTextAt(2, 2, "Could not open file.");
        return;
    }

    terminal.setTextColor("white");
    int row = 2;
    while (file.available()) {
        String line = file.readStringUntil('\n');
        terminal.writeTextAt(row++, 2, line.c_str());
        if (row >= 20) {
            break;
        }
    }

    file.close();

    terminal.setTextColor("green");
    terminal.drawButton(22, 10, 24, 50, "Exit");

    while (true) {
        if (terminal.pollHostTerminalReconnect()) {
            return;
        }
        if (terminal.detectClick(22, 10, 24, 50)) {
            return;
        }
    }
}
