/*
 * ANSITerm — BasicExample
 * https://github.com/NicholasTracy/ANSITerm
 *
 * What you learn: Serial setup, terminal.begin(), named colors, drawBox(), drawButton(),
 * writeTextAt(), and pollInput() for a simple on-screen “button”.
 *
 * Hardware: Any Arduino-class board with a USB serial port.
 * Terminal: Use a real terminal app (VS Code, PuTTY, screen, etc.) at 9600 baud so UTF-8
 * box drawing renders. Mouse clicks need SGR mouse reporting; Enter or Space also activates.
 *
 * License: LGPL-3.0 — see LICENSE.txt in the library root.
 */

#include <ANSITerm.h>

ANSITerm terminal(Serial);

const uint8_t kBtnRow0 = 10;
const uint8_t kBtnRow1 = 12;
const uint8_t kBtnCol0 = 20;
const uint8_t kBtnCol1 = 50;

void drawMainScreen() {
    terminal.begin(true, true, true, true, "cyan", "black");

    terminal.setTextColor("yellow");
    terminal.writeTextAt(2, 2, "Welcome to ANSITerm!");

    terminal.setTextColor("white");
    terminal.drawBox(4, 5, 8, 50);

    terminal.setTextColor("green");
    terminal.writeTextAt(6, 8, "Box drawing with ANSI characters.");

    terminal.setTextColor("cyan");
    terminal.drawButton(kBtnRow0, kBtnCol0, kBtnRow1, kBtnCol1, "Click Me!");

    terminal.setTextColor("white");
    terminal.writeTextAt(14, 2, "Activate: mouse click, Enter, or Space");
}

bool hitButton(uint8_t row, uint8_t col) {
    return row >= kBtnRow0 && row <= kBtnRow1 && col >= kBtnCol0 && col <= kBtnCol1;
}

void showClicked() {
    terminal.clearScreen();
    terminal.writeTextAt(6, 10, "Button clicked!");
}

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ; // Required on native-USB boards (Leonardo, SAMD, etc.)
    }

    drawMainScreen();

    while (true) {
        if (terminal.reconnected()) {
            drawMainScreen();
            continue;
        }

        // pollInput() discards non-ESC bytes; catch Space before that happens.
        if (Serial.available() && Serial.peek() == ' ') {
            Serial.read();
            showClicked();
            return;
        }

        ANSITermInput ev;
        while (terminal.pollInput(ev)) {
            if (ev.kind == ANSITermInput::Enter
                || (ev.kind == ANSITermInput::MousePress && hitButton(ev.mouseRow, ev.mouseCol))) {
                showClicked();
                return;
            }
        }
    }
}

void loop() {
}
