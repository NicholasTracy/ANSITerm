/*
 * ANSITerm — AdvancedExample
 * https://github.com/NicholasTracy/ANSITerm
 *
 * What you learn: double-line frames, animated progress using writeTextAt/deleteAtPosition,
 * drawTable(), and symbols from constants.h — plus restart via mouse click or Enter/Space.
 *
 * Hardware: USB serial. Terminal must support UTF-8; a ~70-column pane fits the table.
 * Closing/reopening the USB serial session redraws the demo on native-USB boards when supported.
 *
 * License: LGPL-3.0 — see LICENSE.txt in the library root.
 */

#include <ANSITerm.h>

ANSITerm terminal(Serial);

const uint8_t kRestartRow0 = 10;
const uint8_t kRestartRow1 = 12;
const uint8_t kRestartCol0 = 20;
const uint8_t kRestartCol1 = 50;

void animateLoadingBar(uint8_t row, uint8_t startCol, uint8_t length);
void runAdvancedDemo();
bool hitRestart(uint8_t row, uint8_t col);

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ;
    }

    runAdvancedDemo();
}

void loop() {
}

void runAdvancedDemo() {
    for (;;) {
        terminal.begin(true, true, true, true, "cyan", "black");

        terminal.setTextColor("yellow");
        terminal.drawDoubleBox(1, 10, 3, 70);
        terminal.writeTextAt(2, 27, "Advanced Terminal Interface");

        terminal.setTextColor("white");
        terminal.drawBox(6, 20, 8, 60);
        terminal.writeTextAt(7, 25, "Loading: [                    ]");
        animateLoadingBar(7, 35, 20);

        terminal.setTextColor("cyan");
        terminal.drawDoubleButton(kRestartRow0, kRestartCol0, kRestartRow1, kRestartCol1,
                                  ANSI_ARROW_RIGHT " Start Again " ANSI_ARROW_LEFT);

        // Table: outer corners at (14,10)-(20,70); 3×4 cells. Row spacing is even, so labels sit on
        // rows 15, 17, and 19 (one text row per cell between horizontal rules).
        terminal.setTextColor("green");
        terminal.drawTable(14, 10, 20, 70, 3, 4);

        terminal.writeTextAt(15, 12, ANSI_HEART " Row 1, Col 1");
        terminal.writeTextAt(15, 27, ANSI_DIAMOND " Row 1, Col 2");
        terminal.writeTextAt(15, 42, ANSI_CLUB " Row 1, Col 3");
        terminal.writeTextAt(15, 57, ANSI_SPADE " Row 1, Col 4");

        terminal.writeTextAt(17, 12, ANSI_CHECKMARK " Row 2, Col 1");
        terminal.writeTextAt(17, 27, ANSI_CROSSMARK " Row 2, Col 2");
        terminal.writeTextAt(17, 42, ANSI_SMILEY " Row 2, Col 3");
        terminal.writeTextAt(17, 57, ANSI_STAR " Row 2, Col 4");

        terminal.writeTextAt(19, 12, ANSI_ARROW_UP " Row 3, Col 1");
        terminal.writeTextAt(19, 27, ANSI_MUSIC_NOTE " Row 3, Col 2");
        terminal.writeTextAt(19, 42, ANSI_INFINITY " Row 3, Col 3");
        terminal.writeTextAt(19, 57, ANSI_ARROW_DOWN " Row 3, Col 4");

        terminal.setTextColor("white");
        terminal.writeTextAt(22, 10, "Restart: click the button, Enter, or Space");

        bool restart = false;
        while (!restart) {
            if (terminal.reconnected()) {
                restart = true;
                break;
            }

            if (Serial.available() && Serial.peek() == ' ') {
                Serial.read();
                terminal.clearScreen();
                restart = true;
                break;
            }

            ANSITermInput ev;
            while (terminal.pollInput(ev)) {
                if (ev.kind == ANSITermInput::Enter
                    || (ev.kind == ANSITermInput::MousePress
                        && hitRestart(ev.mouseRow, ev.mouseCol))) {
                    terminal.clearScreen();
                    restart = true;
                    break;
                }
            }
        }
    }
}

bool hitRestart(uint8_t row, uint8_t col) {
    return row >= kRestartRow0 && row <= kRestartRow1 && col >= kRestartCol0 && col <= kRestartCol1;
}

void animateLoadingBar(uint8_t row, uint8_t startCol, uint8_t length) {
    for (uint8_t i = 0; i < length; i++) {
        terminal.writeTextAt(row, startCol + i, ANSI_BLOCK_FULL);
        delay(100);
    }
    delay(500);
    for (int16_t i = static_cast<int16_t>(length) - 1; i >= 0; i--) {
        terminal.deleteAtPosition(row, startCol + static_cast<uint8_t>(i));
        delay(100);
    }
}
