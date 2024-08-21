#include <ANSITerm.h>

ANSITerm terminal(Serial);

void setup() {
    Serial.begin(9600);
    terminal.begin();
    terminal.clearScreen();

    // Draw a box from row 2, col 5 to row 10, col 30
    terminal.drawBox(2, 5, 10, 30);

    // Write text inside the box
    terminal.writeTextAt(4, 7, "Hello, Box!");

    // Draw a table from row 12, col 5 to row 20, col 40 with 4 rows and 3 columns
    terminal.drawTable(12, 5, 20, 40, 4, 3);

    // Write text inside the table cells
    terminal.writeTextAt(13, 7, "Cell 1");
    terminal.writeTextAt(13, 20, "Cell 2");
    terminal.writeTextAt(13, 33, "Cell 3");
    terminal.writeTextAt(16, 7, "Cell 4");
    terminal.writeTextAt(16, 20, "Cell 5");
    terminal.writeTextAt(16, 33, "Cell 6");
}

void loop() {
    // Looping code if needed
}
