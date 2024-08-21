#include <ANSITerm.h>

ANSITerm terminal(Serial);

void setup() {
    Serial.begin(9600);
    terminal.begin();
    terminal.clearScreen();

    // Set colors using hex code
    terminal.setTextColorHex("#FF4500"); // OrangeRed
    terminal.setBackgroundColorHex("#1E90FF"); // DodgerBlue
    terminal.setCursorPosition(5, 5);
    Serial.println("Text with hex colors");

    terminal.resetFormatting();
    
    // Set colors using RGB values
    terminal.setTextColorRGB(75, 0, 130); // Indigo
    terminal.setBackgroundColorRGB(240, 128, 128); // LightCoral
    terminal.setCursorPosition(7, 5);
    Serial.println("Text with RGB colors");

    terminal.resetFormatting();

    // Set colors using web color names
    terminal.setTextColorByName("green");
    terminal.setBackgroundColorByName("yellow");
    terminal.setCursorPosition(9, 5);
    Serial.println("Text with web color names");
    
    terminal.resetFormatting();
}

void loop() {
    // Looping code if needed
}

