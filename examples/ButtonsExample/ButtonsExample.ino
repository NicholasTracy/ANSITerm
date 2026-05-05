/*
 * ANSITerm — ButtonsExample
 * https://github.com/NicholasTracy/ANSITerm
 *
 * What you learn: Single-line vs double-line buttons (drawButton / drawDoubleButton),
 * polling detectClick(), and toggling the built-in LED from a “soft” button.
 *
 * Hardware: Board with LED_BUILTIN. USB serial + UTF-8 terminal; mouse reporting for clicks.
 *
 * License: LGPL-3.0 — see LICENSE.txt in the library root.
 */

#include <ANSITerm.h>

ANSITerm terminal(Serial);
bool ledState = false;

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ;
    }

    terminal.begin(true, true, true, true, "white", "black");

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, ledState);

    terminal.setTextColor("cyan");
    terminal.writeTextAt(2, 5, "Button interaction example");

    terminal.setTextColor("yellow");
    terminal.drawButton(5, 10, 7, 40, "Toggle LED");

    terminal.setTextColor("green");
    terminal.drawDoubleButton(9, 10, 11, 40, "Exit");

    terminal.setTextColor("white");
    terminal.writeTextAt(13, 5, "Click a button (terminal must forward mouse events).");

    while (true) {
        if (terminal.detectClick(5, 10, 7, 40)) {
            ledState = !ledState;
            digitalWrite(LED_BUILTIN, ledState);
            terminal.writeTextAt(15, 5, ledState ? "LED is ON " : "LED is OFF");
        }

        if (terminal.detectClick(9, 10, 11, 40)) {
            terminal.clearScreen();
            terminal.writeTextAt(6, 10, "Goodbye.");
            break;
        }
    }
}

void loop() {
}
