#include <ANSITerm.h>

ANSITerm terminal(Serial);

void setup() {
    Serial.begin(9600);
    terminal.begin();
    terminal.clearScreen();

    // Draw a button
    terminal.drawButton(5, 10, 8, 30, "Click Me!");

    // Wait for a click
    while (true) {
        if (terminal.detectClick(5, 10, 8, 30)) {
            terminal.writeTextAt(10, 10, "Button clicked!");
            break;
        }
    }
}

void loop() {
    // Additional loop code if needed
}
