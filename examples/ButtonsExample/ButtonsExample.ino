/*
 * ANSITerm — ButtonsExample
 * https://github.com/NicholasTracy/ANSITerm
 *
 * What you learn: drawButton / drawDoubleButton, pollInput() for arrows + Enter + mouse,
 * highlighting the focused control, and toggling LED_BUILTIN.
 *
 * Mouse: ANSI “SGR mouse” (mode 1006) sends click coordinates as escape sequences on the
 * same serial link. Many USB-serial terminals (including PuTTY in COM mode) do not — use
 * keyboard navigation there. Terminals that support xterm-style mouse over your link will
 * work with direct clicks.
 *
 * License: LGPL-3.0 — see LICENSE.txt in the library root.
 */

#include <ANSITerm.h>

ANSITerm terminal(Serial);

bool ledState = false;

// Geometry matches drawButton / drawDoubleButton below (1-based row/col).
const uint8_t kToggleRow0 = 5;
const uint8_t kToggleRow1 = 7;
const uint8_t kToggleCol0 = 10;
const uint8_t kToggleCol1 = 40;

const uint8_t kExitRow0 = 9;
const uint8_t kExitRow1 = 11;
const uint8_t kExitCol0 = 10;
const uint8_t kExitCol1 = 40;

uint8_t selection = 0;

void drawButtons(uint8_t sel) {
    if (sel == 0) {
        terminal.setBackgroundColor("blue");
        terminal.setTextColor("white");
    } else {
        terminal.setBackgroundColor("black");
        terminal.setTextColor("yellow");
    }
    terminal.drawButton(kToggleRow0, kToggleCol0, kToggleRow1, kToggleCol1, "Toggle LED");
    terminal.resetFormatting();
    terminal.setBackgroundColor("black");

    if (sel == 1) {
        terminal.setBackgroundColor("blue");
        terminal.setTextColor("white");
    } else {
        terminal.setBackgroundColor("black");
        terminal.setTextColor("green");
    }
    terminal.drawDoubleButton(kExitRow0, kExitCol0, kExitRow1, kExitCol1, "Exit");
    terminal.resetFormatting();
    terminal.setBackgroundColor("black");
}

void paintFullUi() {
    terminal.begin(true, true, true, true, "white", "black");

    terminal.setTextColor("cyan");
    terminal.writeTextAt(2, 5, "Button interaction example");

    selection = 0;
    drawButtons(selection);

    terminal.setTextColor("white");
    terminal.writeTextAt(13, 5, "Keyboard: Arrow keys = move highlight   Enter = activate");
    terminal.writeTextAt(14, 5, "Mouse: click works only if the terminal sends SGR mouse reports.");
    terminal.writeTextAt(15, 5, "(PuTTY over USB Serial usually does not — use arrows + Enter.)");
}

bool hitToggle(uint8_t row, uint8_t col) {
    return row >= kToggleRow0 && row <= kToggleRow1 && col >= kToggleCol0 && col <= kToggleCol1;
}

bool hitExit(uint8_t row, uint8_t col) {
    return row >= kExitRow0 && row <= kExitRow1 && col >= kExitCol0 && col <= kExitCol1;
}

void activateToggle() {
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
    terminal.writeTextAt(16, 5, ledState ? "LED is ON " : "LED is OFF");
}

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ;
    }

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, ledState);

    paintFullUi();

    while (true) {
        if (terminal.pollHostTerminalReconnect()) {
            digitalWrite(LED_BUILTIN, ledState);
            paintFullUi();
            continue;
        }

        ANSITermInput ev;
        while (terminal.pollInput(ev)) {
            switch (ev.kind) {
                case ANSITermInput::ArrowUp:
                    selection = 0;
                    drawButtons(selection);
                    break;
                case ANSITermInput::ArrowDown:
                    selection = 1;
                    drawButtons(selection);
                    break;
                case ANSITermInput::ArrowLeft:
                    selection = (selection == 0) ? 1 : 0;
                    drawButtons(selection);
                    break;
                case ANSITermInput::ArrowRight:
                    selection = (selection == 0) ? 1 : 0;
                    drawButtons(selection);
                    break;
                case ANSITermInput::Enter:
                    if (selection == 0) {
                        activateToggle();
                    } else {
                        terminal.clearScreen();
                        terminal.writeTextAt(6, 10, "Goodbye.");
                        return;
                    }
                    break;
                case ANSITermInput::MousePress:
                    if (hitToggle(ev.mouseRow, ev.mouseCol)) {
                        selection = 0;
                        drawButtons(selection);
                        activateToggle();
                    } else if (hitExit(ev.mouseRow, ev.mouseCol)) {
                        selection = 1;
                        drawButtons(selection);
                        terminal.clearScreen();
                        terminal.writeTextAt(6, 10, "Goodbye.");
                        return;
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

void loop() {
}
