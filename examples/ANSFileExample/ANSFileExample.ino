/*
 * ANSITerm — ANSFileExample
 * https://github.com/NicholasTracy/ANSITerm
 *
 * What you learn: Listing .ans files on SD, opening one line-by-line with writeTextAt,
 * and overlaying drawButton + detectClick() / pollInput() to return to the file list.
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
const uint8_t MAX_LISTED_FILES = 12;
const uint8_t MAX_NAME_LEN = 31;

char listedFiles[MAX_LISTED_FILES][MAX_NAME_LEN + 1];
uint8_t listedRows[MAX_LISTED_FILES];
uint8_t listedCount = 0;

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

    listedCount = 0;
    File root = SD.open("/");
    uint8_t row = 4;

    while (listedCount < MAX_LISTED_FILES) {
        File entry = root.openNextFile();
        if (!entry) {
            break;
        }

        if (!entry.isDirectory()) {
            String fileName = entry.name();
            if (fileName.endsWith(".ans")) {
                strncpy(listedFiles[listedCount], fileName.c_str(), MAX_NAME_LEN);
                listedFiles[listedCount][MAX_NAME_LEN] = '\0';
                listedRows[listedCount] = row;

                terminal.setTextColor("yellow");
                terminal.writeTextAt(row, 4, listedFiles[listedCount]);

                listedCount++;
                row++;
            }
        }
        entry.close();
    }

    root.close();

    if (listedCount == 0) {
        terminal.setTextColor("red");
        terminal.writeTextAt(4, 4, "No .ans files found.");
    }

    while (true) {
        if (terminal.reconnected()) {
            goto tree_top;
        }

        ANSITermInput ev;
        while (terminal.pollInput(ev)) {
            if (ev.kind != ANSITermInput::MousePress) {
                continue;
            }
            for (uint8_t i = 0; i < listedCount; i++) {
                const uint8_t nameLen = static_cast<uint8_t>(strlen(listedFiles[i]));
                const uint8_t r = listedRows[i];
                const uint8_t c0 = 4;
                const uint8_t c1 = static_cast<uint8_t>(4 + nameLen);
                if (ev.mouseRow == r && ev.mouseCol >= c0 && ev.mouseCol <= c1) {
                    displayAnsiFile(listedFiles[i]);
                    goto tree_top;
                }
            }
        }
        delay(20);
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
        if (terminal.reconnected()) {
            return;
        }
        if (terminal.detectClick(22, 10, 24, 50)) {
            return;
        }
        delay(20);
    }
}
