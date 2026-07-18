# CI: GitHub Actions (Arduino CLI + CodeQL)

This document matches the workflows under `.github/workflows/` as of the current `master` branch.

## Workflow files

| Workflow | File | Purpose |
|----------|------|---------|
| Arduino CI | [`.github/workflows/arduino-ci.yml`](.github/workflows/arduino-ci.yml) | Lint + compile examples and targeted network sketches |
| CodeQL | [`.github/workflows/codeql.yml`](.github/workflows/codeql.yml) | C/C++ CodeQL database built via Arduino CLI compile steps |

## Triggers

**Arduino CI**

- Push and pull request to `master` or `main`
- Tags matching `v*` (same branches convention as push)

**CodeQL**

- Push and pull request to `master` or `main`
- Weekly schedule: Sundays `12 6 * * 0` (UTC)

## Arduino CI jobs

### Lint (`arduino-lint-action`)

- Uses [arduino/arduino-lint-action@v2](https://github.com/arduino/arduino-lint-action).
- Settings: `compliance: strict`, `library-manager: submit`.

### Compile AVR examples (matrix)

- Boards: `arduino:avr:uno`, `arduino:avr:leonardo`, `arduino:avr:mega`.
- Installs core `arduino:avr` and library **SD** (for `ANSFileExample`).
- Compiles every sketch under `examples/**/*.ino` **except**:
  - `examples/NetworkTerminalExample/`
  - `examples/ESPWiFiControlExample/`
- Uses `arduino-cli compile --fqbn … --library . <sketch.ino>` so the checkout root is treated as the library under test.
- **UNO/Leonardo exception:** `examples/ANSFileExample/ANSFileExample.ino` is **skipped** on UNO and Leonardo because SD + file-list buffers exceed their SRAM; it still builds on Mega.

### Compile ESP examples

- Boards: `esp32:esp32:esp32`, `esp8266:esp8266:generic`.
- Compiles `ESPWiFiControlExample` and `NetworkTerminalExample` for each.

### Compile `NetworkTerminalExample` (additional stacks)

1. **UNO WiFi Rev2 + WiFiNINA** — `arduino:megaavr:uno2018`, `-DANSITERM_NET_USE_WIFI_NINA`, `WiFiNINA` library.
2. **Mega + Ethernet** — `arduino:avr:mega`, `-DANSITERM_NET_USE_ETHERNET`, `Ethernet` library.

## CodeQL job

- Initializes CodeQL for `cpp` with `build-mode: manual` ([github/codeql-action](https://github.com/github/codeql-action)).
- Installs **arduino:avr**, **arduino:megaavr**, **esp32:esp32**, **esp8266:esp8266**, and libraries **SD**, **Ethernet**, **WiFiNINA**.
- Compiles AVR examples (excluding network + ESP Wi-Fi control) on Mega, ESP sketches on ESP32/ESP8266, plus the WiFiNINA/Ethernet network variants.
- Runs `github/codeql-action/analyze@v3` after builds.

## Maintainer checklist

- [ ] Confirm `library.properties` maintainer/contact fields match what you want for releases and registry.
- [ ] CI green on `master` / `main` for both workflows after pushing ESP-targeted example changes.

## Related notes

- Historic Travis configuration is not part of the current CI path; automation is GitHub Actions only.
- Prefer ESP32/ESP8266 jobs for new network UI examples; keep AVR for classic Serial demos.
