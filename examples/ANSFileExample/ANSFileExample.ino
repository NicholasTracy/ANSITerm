/*
 * ANSITerm — ANSFileExample
 * https://github.com/NicholasTracy/ANSITerm
 *
 * What you learn: Listing .ans files on SD, opening one line-by-line with writeTextAt,
 * and selecting a file with mouse click or arrows + Enter (Exit via click/Enter/Space).
 *
 * Hardware: SD shield/module on SPI (default chip select 4 — change chipSelect for your wiring).
 * Needs enough RAM for SD + String lines; skipped on small AVR boards in CI.
 *
 * License: LGPL-3.0 — see LICENSE.txt in the library root.
 */

#include <ANSITerm.h>
#include <SD.h>
#include <SPI.h>
#include <string.h>

ANSITerm terminal(Serial);

const int chipSelect = 4;
// Keep the file list compact for AVR SRAM. Files longer than MAX_NAME_LEN are skipped
// (not truncated) so SD.open() always uses a complete name.
const uint8_t MAX_LISTED_FILES = 8;
const uint8_t MAX_NAME_LEN = 24;

char listedFiles[MAX_LISTED_FILES][MAX_NAME_LEN + 1];
uint8_t listedRows[MAX_LISTED_FILES];
uint8_t listedCount = 0;
uint8_t selection = 0;

void displayAnsiFile(const char* filename);
void displayFileTree();
void drawFileList();
void openSelectedFile();

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

void drawFileList() {
    terminal.clearScreen();
    terminal.setTextColor("cyan");
    terminal.writeTextAt(2, 2, "SD card — .ans files:");
    terminal.setTextColor("white");
    terminal.writeTextAt(3, 2, "Arrows/Enter select; mouse click also works");

    if (listedCount == 0) {
        terminal.setTextColor("red");
        terminal.writeTextAt(5, 4, "No .ans files found (or names too long).");
        return;
    }

    for (uint8_t i = 0; i < listedCount; i++) {
        if (i == selection) {
            terminal.setBackgroundColor("blue");
            terminal.setTextColor("white");
        } else {
            terminal.setBackgroundColor("black");
            terminal.setTextColor("yellow");
        }
        char line[36];
        snprintf(line, sizeof(line), "%c %s", (i == selection) ? '>' : ' ', listedFiles[i]);
        terminal.writeTextAt(listedRows[i], 4, line);
        terminal.resetFormatting();
        terminal.setBackgroundColor("black");
    }
}

void openSelectedFile() {
    if (listedCount == 0 || selection >= listedCount) {
        return;
    }
    displayAnsiFile(listedFiles[selection]);
}

void displayFileTree() {
tree_top:
    listedCount = 0;
    selection = 0;
    File root = SD.open("/");
    if (!root) {
        terminal.clearScreen();
        terminal.setTextColor("red");
        terminal.writeTextAt(2, 2, "Could not open SD root.");
        while (true) {
            delay(1000);
        }
    }

    uint8_t row = 5;
    while (listedCount < MAX_LISTED_FILES) {
        File entry = root.openNextFile();
        if (!entry) {
            break;
        }

        if (!entry.isDirectory()) {
            String fileName = entry.name();
            if (fileName.endsWith(".ans") && fileName.length() <= MAX_NAME_LEN) {
                strncpy(listedFiles[listedCount], fileName.c_str(), MAX_NAME_LEN);
                listedFiles[listedCount][MAX_NAME_LEN] = '\0';
                listedRows[listedCount] = row;
                listedCount++;
                row++;
            }
        }
        entry.close();
    }
    root.close();

    drawFileList();

    while (true) {
        if (terminal.reconnected()) {
            goto tree_top;
        }

        if (Serial.available() && Serial.peek() == ' ') {
            Serial.read();
            if (listedCount > 0) {
                openSelectedFile();
                goto tree_top;
            }
        }

        ANSITermInput ev;
        while (terminal.pollInput(ev)) {
            if (listedCount == 0) {
                continue;
            }
            if (ev.kind == ANSITermInput::ArrowUp) {
                selection = (selection == 0) ? static_cast<uint8_t>(listedCount - 1)
                                             : static_cast<uint8_t>(selection - 1);
                drawFileList();
            } else if (ev.kind == ANSITermInput::ArrowDown) {
                selection = static_cast<uint8_t>((selection + 1) % listedCount);
                drawFileList();
            } else if (ev.kind == ANSITermInput::Enter) {
                openSelectedFile();
                goto tree_top;
            } else if (ev.kind == ANSITermInput::MousePress) {
                for (uint8_t i = 0; i < listedCount; i++) {
                    const uint8_t nameLen = static_cast<uint8_t>(strlen(listedFiles[i]));
                    const uint8_t r = listedRows[i];
                    const uint8_t c0 = 4;
                    const uint8_t c1 = static_cast<uint8_t>(6 + nameLen);
                    if (ev.mouseRow == r && ev.mouseCol >= c0 && ev.mouseCol <= c1) {
                        selection = i;
                        openSelectedFile();
                        goto tree_top;
                    }
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
        terminal.writeTextAt(4, 2, "Press Enter/Space or click Exit.");
    } else {
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
    }

    terminal.setTextColor("green");
    terminal.drawButton(22, 10, 24, 50, "Exit");
    terminal.setTextColor("white");
    terminal.writeTextAt(25, 10, "Exit: click, Enter, or Space");

    while (true) {
        if (terminal.reconnected()) {
            return;
        }
        if (Serial.available() && Serial.peek() == ' ') {
            Serial.read();
            return;
        }

        ANSITermInput ev;
        while (terminal.pollInput(ev)) {
            if (ev.kind == ANSITermInput::Enter) {
                return;
            }
            if (ev.kind == ANSITermInput::MousePress
                && ev.mouseRow >= 22 && ev.mouseRow <= 24
                && ev.mouseCol >= 10 && ev.mouseCol <= 50) {
                return;
            }
        }
        delay(20);
    }
}
