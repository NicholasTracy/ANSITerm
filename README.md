# ANSITerm Library

ANSITerm is an Arduino library that sends ANSI/VT-style escape sequences to a terminal via any `Stream` (e.g., `Serial`), letting you clear the screen, position the cursor, colorize text, draw boxes/tables, and (where supported) handle mouse clicks.

[![Arduino CI](https://github.com/NicholasTracy/ANSITerm/actions/workflows/arduino-ci.yml/badge.svg)](https://github.com/NicholasTracy/ANSITerm/actions/workflows/arduino-ci.yml)
[![License: LGPL v3](https://img.shields.io/badge/License-LGPL%20v3-blue.svg)](LICENSE.txt)

> Note: This library is under active development. Please report issues with a minimal sketch and your terminal + board details.

**User guide (non-technical):** [ANSITerm Wiki](https://github.com/NicholasTracy/ANSITerm/wiki) — installation, choosing a terminal, example walkthroughs, and troubleshooting.

---

## Features (high level)

- Screen & cursor control: `clearScreen()`, `setCursorPosition()`, `resetFormatting()`
- Colors: 8-bit palette index, `#RRGGBB` / `#RGB` hex, CSS names, and RGB triplets via `setTextColor()` / `setBackgroundColor()`
- Drawing: single/double line boxes, tables, text at coordinates, delete at position
- UI helpers: single/double “buttons”, click detection (requires terminal mouse reporting)
- Input: `pollInput()` for Enter, arrows, and SGR mouse press/release
- Mouse: enable/disable reporting, parse terminal mouse packets

See the `examples/` folder for usage patterns, or the [Examples section of the Wiki](https://github.com/NicholasTracy/ANSITerm/wiki/Using-the-Examples).

---

## Installation

### Via Library Manager (after first release)
Search **ANSITerm** in *Arduino IDE → Tools → Manage Libraries…* and click **Install**.

### Manual
1. Download this repository (Code → Download ZIP).
2. Extract to your Arduino libraries folder (e.g., `~/Arduino/libraries/ANSITerm`).
3. Restart the IDE.

---

## Supported / CI-tested boards

| Target | Examples compiled in CI |
|--------|-------------------------|
| `arduino:avr:uno` / `leonardo` / `mega` | Basic, Advanced, Buttons, Graphic; ANSFile on Mega only (skipped on Uno/Leonardo for SRAM) |
| `esp32:esp32:esp32` | `ESPWiFiControlExample`, `NetworkTerminalExample` |
| `esp8266:esp8266:generic` | `ESPWiFiControlExample`, `NetworkTerminalExample` |
| `arduino:megaavr:uno2018` + WiFiNINA | `NetworkTerminalExample` |
| `arduino:avr:mega` + Ethernet | `NetworkTerminalExample` |

`architectures=*` — the library itself builds anywhere a `Stream` is available. Network examples need the matching Wi‑Fi/Ethernet stack for your board.

---

## Minimal example

```cpp
#include <ANSITerm.h>

ANSITerm terminal(Serial);

void setup() {
  Serial.begin(9600);
  while (!Serial) { ; }
  terminal.begin();
  terminal.clearScreen();
  terminal.setTextColor("yellow");
  terminal.writeTextAt(2, 10, "Welcome to ANSITerm!");
}
void loop() {}
```

---

## Contributing

- Prefer the [Wiki](https://github.com/NicholasTracy/ANSITerm/wiki) for how-to questions.
- File bugs, features, docs gaps, or questions via [Issues](https://github.com/NicholasTracy/ANSITerm/issues/new/choose) (templates provided).
- Pull requests welcome. Please keep patches focused and ensure CI is green.

---

## Releasing

1. Update `library.properties` `version=` (e.g., `0.1.0`).
2. Tag and push: `git tag -a v0.1.0 -m "ANSITerm 0.1.0" && git push --follow-tags`.
3. Create a GitHub Release from the tag.
4. Submit your repository URL to the Arduino **[Library Manager registry](https://github.com/arduino/library-registry)** (add to `repositories.txt`). Subsequent semver tags are indexed automatically once accepted.

---

## License

LGPL-3.0 — see [LICENSE.txt](LICENSE.txt).
