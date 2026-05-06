/*
 * ANSITerm — BasicExample
 * https://github.com/NicholasTracy/ANSITerm
 *
 * What you learn: Serial setup, terminal.begin(), named colors, drawBox(), drawButton(),
 * writeTextAt(), and detectClick() for a simple on-screen “button”.
 *
 * Hardware: Any Arduino-class board with a USB serial port.
 * Terminal: Use a real terminal app (VS Code, PuTTY, screen, etc.) at 9600 baud so UTF-8
 * box drawing renders; enable mouse reporting in the terminal if clicks should register.
 *
 * License: LGPL-3.0 — see LICENSE.txt in the library root.
 */

#include <ANSITerm.h>

ANSITerm terminal(Serial);

void drawMainScreen() {
    terminal.begin(true, true, true, true, "cyan", "black");

    terminal.setTextColor("yellow");
    terminal.writeTextAt(2, 2, "Welcome to ANSITerm!");

    terminal.setTextColor("white");
    terminal.drawBox(4, 5, 8, 40);

    terminal.setTextColor("green");
    terminal.writeTextAt(6, 10, "This is a box drawn with ANSI characters.");

    terminal.setTextColor("cyan");
    terminal.drawButton(10, 20, 12, 50, "Click Me!");
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
        if (terminal.detectClick(10, 20, 12, 50)) {
            terminal.clearScreen();
            terminal.writeTextAt(6, 10, "Button clicked!");
            break;
        }
    }
}

void loop() {
}
