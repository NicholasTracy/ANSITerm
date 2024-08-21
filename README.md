# <p align="center">ANSITerm Library</p>

### <p align="center">ANSITerm is an Arduino library providing ANSI escape sequence control of a capable terminal using the stream class.</p>

---

### <p align="center">WARNING: This code may not work as desired. This is a work in progress and currenlty under testing. Only use this if you want to test for yourself. If you find a bug, please check the [Issues](https://github.com/NicholasTracy/ANSITerm/issues) section to see if it has already been brought to my attention. If it isn't there, please create a new issue and include as much detail as possible so that I can recreate the bug you encountered.</p>

---

<p align="center">
    Current Travis CI Build Status for the development branch:<br>
    <a href="https://travis-ci.org/NicholasTracy/ANSITerm">
        <img src="https://travis-ci.org/NicholasTracy/ANSITerm.svg?branch=development" alt="Travis CI Build Status">
    </a>
</p>

---

##### This code is loosely based on the work done in the *BasicTerm* Library.
[BasicTerm GitHub](http://github.com/nottwo/BasicTerm)

##### Also, some inspiration was taken from the VT100 library by Rob Tillaart.
[VT100 GitHub](https://github.com/RobTillaart/Arduino/tree/master/libraries/VT100)

##### ANSI Escape Sequences used in this library are found at:
[Ascii-Table.com - ANSI escape sequences](http://ascii-table.com/ansi-escape-sequences.php)

**_and_**

[Wikipedia article on ANSI escape codes](https://en.wikipedia.org/wiki/ANSI_escape_code#Escape_sequences)

---

### Installation

#### Library Manager Method:

This method will be available after the 0.1.0 release is made ready.

Open the library manager in Arduino IDE 1.6 or newer and search for ANSITerm. Click `Install` and wait for the install to complete. Afterward, the library should be ready to use. You can check the Examples menu to see how the library works.

#### Manual Method:

Download the ANSITerm repository and extract the ANSITerm directory into `~/sketchbook/libraries` and restart the Arduino IDE. This should be sufficient to make ANSITerm available in the Sketch | Import Library menu.

If you are having trouble, please refer to this link for an in-depth guide on both methods:
[Arduino Library Installation Guide](https://www.arduino.cc/en/Guide/Libraries#toc3)

---

### Features

The `ANSITerm` library allows you to control ANSI-compatible terminals via Arduino, offering a wide range of capabilities for creating interactive and graphical terminal-based interfaces. Below are the key features and functions provided by the library.

#### **Screen and Cursor Control:**
- `clearScreen()`: Clears the terminal screen.
- `setCursorPosition(uint8_t row, uint8_t col)`: Sets the cursor to a specific row and column.
- `resetFormatting()`: Resets the text formatting (color, bold, etc.) to the default state.

#### **Color Management:**
- `setTextColor(uint8_t color)`: Sets the text color using an 8-bit ANSI color code.
- `setBackgroundColor(uint8_t color)`: Sets the background color using an 8-bit ANSI color code.
- `setTextColorHex(const char* hex)`: Sets the text color using a hex color code.
- `setBackgroundColorHex(const char* hex)`: Sets the background color using a hex color code.
- `setTextColorRGB(uint8_t r, uint8_t g, uint8_t b)`: Sets the text color using RGB values.
- `setBackgroundColorRGB(uint8_t r, uint8_t g, uint8_t b)`: Sets the background color using RGB values.
- `setTextColorByName(const char* name)`: Sets the text color using common web color names.
- `setBackgroundColorByName(const char* name)`: Sets the background color using common web color names.

#### **Drawing and Graphics:**
- `drawBox(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol)`: Draws a box using single-line box-drawing characters.
- `drawDoubleBox(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol)`: Draws a box using double-line box-drawing characters.
- `drawTable(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol, uint8_t rows, uint8_t cols)`: Draws a table with a specified number of rows and columns using single-line box-drawing characters.
- `writeTextAt(uint8_t row, uint8_t col, const char* text)`: Writes text at a specified position.
- `deleteAtPosition(uint8_t row, uint8_t col)`: Deletes (clears) content at a specific row and column, leaving it blank.

#### **Buttons and Interaction:**
- `drawButton(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol, const char* text)`: Draws a button with centered text using single-line box-drawing characters.
- `drawDoubleButton(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol, const char* text)`: Draws a button with centered text using double-line box-drawing characters.
- `detectClick(uint8_t startRow, uint8_t startCol, uint8_t endRow, uint8_t endCol)`: Detects if a mouse click occurs within the specified button or box boundaries.

#### **Mouse Interaction:**
- `enableMouseReporting()`: Enables mouse tracking in the terminal, allowing for detection of mouse events.
- `disableMouseReporting()`: Disables mouse tracking.
- `parseMouseReport(uint8_t& row, uint8_t& col)`: Parses the mouse report to determine the row and column where a click occurred.

---

### ANSI Characters and Symbols

The library includes a comprehensive set of ANSI characters and symbols that can be used to create visually rich terminal interfaces. Below is a summary of the available characters:

#### **Box-Drawing Characters:**
- Single-Line:
  - Horizontal: `\u2500` (─)
  - Vertical: `\u2502` (│)
  - Corners: `\u250C` (┌), `\u2510` (┐), `\u2514` (└), `\u2518` (┘)
  - Intersections: `\u252C` (┬), `\u2534` (┴), `\u251C` (├), `\u2524` (┤), `\u253C` (┼)

- Double-Line:
  - Horizontal: `\u2550` (═)
  - Vertical: `\u2551` (║)
  - Corners: `\u2554` (╔), `\u2557` (╗), `\u255A` (╚), `\u255D` (╝)
  - Intersections: `\u2566` (╦), `\u2569` (╩), `\u2560` (╠), `\u2563` (╣), `\u256C` (╬)

#### **Block Elements:**
- Full Block: `\u2588` (█)
- Lower Half Block: `\u2584` (▄)
- Upper Half Block: `\u2580` (▀)
- Left Half Block: `\u258C` (▌)
- Right Half Block: `\u2590` (▐)
- Light Shade: `\u2591` (░)
- Medium Shade: `\u2592` (▒)
- Dark Shade: `\u2593` (▓)

#### **Arrows and Pointers:**
- Up Arrow: `\u2191` (↑)
- Down Arrow: `\u2193` (↓)
- Left Arrow: `\u2190` (←)
- Right Arrow: `\u2192` (→)
- Left Pointer: `\u25C4` (◄)
- Right Pointer: `\u25BA` (►)

#### **Mathematical Symbols:**
- Plus-Minus: `\u00B1` (±)
- Multiplication: `\u00D7` (×)
- Division: `\u00F7` (÷)
- Infinity: `\u221E` (∞)
- Summation: `\u2211` (∑)
- Square Root: `\u221A` (√)
- Angle: `\u2220` (∠)

#### **Geometric Shapes:**
- Triangles: `\u25B2` (▲), `\u25BC` (▼), `\u25C0` (◀), `\u25B6` (▶)
- Circle: `\u25CF` (●), Hollow Circle: `\u25CB` (○)
- Square: `\u25A0` (■), Hollow Square: `\u25A1` (□)
- Diamond: `\u25C6` (◆), Hollow Diamond: `\u25C7` (◇)
- Heart: `\u2665` (♥)
- Spade: `\u2660` (♠)
- Club: `\u2663` (♣)
- Star: `\u2605` (★), Hollow Star: `\u2606` (☆)

#### **Miscellaneous Symbols:**
- Checkmark: `\u2713` (✓)
- Crossmark: `\u2717` (✗)
- Bullet: `\u2022` (•)
- Male: `\u2642` (♂)
- Female: `\u2640` (♀)
- Music Note: `\u266A` (♪)
- Peace: `\u262E` (☮)
- Yin-Yang: `\u262F` (☯)
- Sun: `\u263C` (☼)
- Moon: `\u263E` (☾)
- Snowflake: `\u2744` (❄)

---

### Example Usage

For detailed examples on how to use the `ANSITerm` library, please refer to the `examples` directory. Below is a basic example of how to set up and use the library:

```cpp
#include <ANSITerm.h>

ANSITerm terminal(Serial);

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ; // Wait for serial port to connect.
    }
    terminal.begin();
    terminal.clearScreen();

    terminal.setTextColorByName("yellow");
    terminal.drawDoubleBox(1, 10, 3, 70);
    terminal.writeTextAt(2, 35, "Welcome to ANSITerm!");

    terminal.setTextColorByName("cyan");
    terminal.drawDoubleButton(10, 20, 12, 50, "Click Me!");
}

void loop() {
    // Main loop
}
```
---

### License:


BasicTerm is licensed under the terms of the GNU Lesser
General Public License v3.

No warranties or guarentees are made, the library is provided as is.

Please refer to the documentation provided for more information.
=======
[License Information]( ANSITerm/LICENSE )


### Author:

Nicholas Tracy [Github](https://github.com/NicholasTracy)


Original Idea and bases for this library credited to:
Trannie Carter & <borys@nottwo.org>

---

### Current release status:

Library is currently still in development. No releases have been made yet. 
