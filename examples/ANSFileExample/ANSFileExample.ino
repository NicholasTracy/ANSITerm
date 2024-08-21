#include <ANSITerm.h>
#include <SD.h>
#include <SPI.h>

const int chipSelect = 4; // Change this to the correct chip select pin for your SD card

ANSITerm terminal(Serial);

void setup() {
    Serial.begin(9600);
    terminal.begin();
    terminal.clearScreen();

    if (!SD.begin(chipSelect)) {
        Serial.println("SD card initialization failed!");
        return;
    }

    terminal.printAnsFile("example.ans"); // Replace with your .ans file name
}

void loop() {
    // Additional loop code if needed
}
