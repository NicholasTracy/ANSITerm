/*
 * ANSITerm — AdvancedExample
 * https://github.com/NicholasTracy/ANSITerm
 *
 * What you learn: double-line frames, animated progress using writeTextAt/deleteAtPosition,
 * drawTable(), and symbols from constants.h — plus detectClick() on a framed “button”.
 *
 * Hardware: USB serial. Terminal must support UTF-8 and (for clicks) SGR mouse mode.
 *
 * License: LGPL-3.0 — see LICENSE.txt in the library root.
 */

#include <ANSITerm.h>

ANSITerm terminal(Serial);

void animateLoadingBar(uint8_t row, uint8_t startCol, uint8_t length);
void runAdvancedDemo();

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
        terminal.writeTextAt(2, 35, "Advanced Terminal Interface");

        terminal.setTextColor("white");
        terminal.drawBox(6, 20, 8, 60);
        terminal.writeTextAt(7, 22, "Loading: [                    ]");
        animateLoadingBar(7, 32, 20);

        terminal.setTextColor("cyan");
        terminal.drawDoubleButton(10, 20, 12, 50, ANSI_ARROW_RIGHT " Start Again " ANSI_ARROW_LEFT);

        // Table: outer corners at (14,10)-(20,70); 3×4 cells. Row spacing is even, so labels sit on
        // rows 15, 17, and 19 (one text row per cell between horizontal rules).
        terminal.setTextColor("green");
        terminal.drawTable(14, 10, 20, 70, 3, 4);

        terminal.writeTextAt(15, 12, ANSI_HEART " Row 1, Col 1");
        terminal.writeTextAt(15, 30, ANSI_DIAMOND " Row 1, Col 2");
        terminal.writeTextAt(15, 50, ANSI_CLUB " Row 1, Col 3");
        terminal.writeTextAt(15, 68, ANSI_SPADE " Row 1, Col 4");

        terminal.writeTextAt(17, 12, ANSI_CHECKMARK " Row 2, Col 1");
        terminal.writeTextAt(17, 30, ANSI_CROSSMARK " Row 2, Col 2");
        terminal.writeTextAt(17, 50, ANSI_SMILEY " Row 2, Col 3");
        terminal.writeTextAt(17, 68, ANSI_STAR " Row 2, Col 4");

        terminal.writeTextAt(19, 12, ANSI_ARROW_UP " Row 3, Col 1");
        terminal.writeTextAt(19, 30, ANSI_MUSIC_NOTE " Row 3, Col 2");
        terminal.writeTextAt(19, 50, ANSI_INFINITY " Row 3, Col 3");
        terminal.writeTextAt(19, 68, ANSI_ARROW_DOWN " Row 3, Col 4");

        while (true) {
            if (terminal.detectClick(10, 20, 12, 50)) {
                terminal.clearScreen();
                break;
            }
        }
    }
}

void animateLoadingBar(uint8_t row, uint8_t startCol, uint8_t length) {
    for (uint8_t i = 0; i <= length; i++) {
        terminal.writeTextAt(row, startCol + i, ANSI_BLOCK_FULL);
        delay(100);
    }
    delay(500);
    for (int16_t i = static_cast<int16_t>(length); i >= 0; i--) {
        terminal.deleteAtPosition(row, startCol + static_cast<uint8_t>(i));
        delay(100);
    }
}
