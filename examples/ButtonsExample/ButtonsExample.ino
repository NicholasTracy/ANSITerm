/*
 * ANSITerm — ButtonsExample
 * https://github.com/NicholasTracy/ANSITerm
 *
 * What you learn: Single-line vs double-line buttons (drawButton / drawDoubleButton),
 * polling detectClick(), and toggling the built-in LED from a “soft” button.
 *
 * Hardware: Board with LED_BUILTIN. USB serial + UTF-8 terminal.
 *
 * Mouse vs PuTTY: ANSI mouse sends escape sequences on the *same* serial link.
 * PuTTY in Serial/COM mode does not generate those sequences from clicks — only a few
 * terminals do over raw serial. Use the keyboard shortcuts below for testing, or try a
 * terminal that supports xterm mouse reporting over your connection (often SSH, not COM).
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
    terminal.writeTextAt(13, 5, "Mouse clicks need SGR terminal reporting (PuTTY Serial does not).");
    terminal.writeTextAt(14, 5, "Keys: T = toggle LED   X = exit");

    while (true) {
        // Plain keys (not starting an ESC escape): PuTTY-friendly fallback.
        while (Serial.available()) {
            int raw = Serial.peek();
            if (raw < 0) {
                break;
            }
            if (static_cast<uint8_t>(raw) == '\033') {
                break;
            }
            char ch = static_cast<char>(Serial.read());
            if (ch == '\r' || ch == '\n') {
                continue;
            }
            if (ch == 't' || ch == 'T') {
                ledState = !ledState;
                digitalWrite(LED_BUILTIN, ledState);
                terminal.writeTextAt(15, 5, ledState ? "LED is ON " : "LED is OFF");
            } else if (ch == 'x' || ch == 'X') {
                terminal.clearScreen();
                terminal.writeTextAt(6, 10, "Goodbye.");
                return;
            }
        }

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
