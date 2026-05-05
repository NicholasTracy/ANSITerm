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
- Settings: `compliance: permissive`, `library-manager: false`  
  (avoids blocking the repo on strict Library Manager submission rules while nested or legacy layout exists.)

### Compile AVR examples (matrix)

- Boards: `arduino:avr:uno`, `arduino:avr:leonardo`, `arduino:avr:mega`.
- Installs core `arduino:avr` and library **SD** (for `ANSFileExample`).
- Compiles every sketch under `examples/**/*.ino` **except** `examples/SSHExample/` (handled in dedicated jobs).
- Uses `arduino-cli compile --fqbn … --library . <sketch.ino>` so the checkout root is treated as the library under test.
- **UNO exception:** `examples/ANSFileExample/ANSFileExample.ino` is **skipped** on UNO because dynamic RAM usage exceeds typical UNO SRAM; it still builds on Leonardo and Mega.

### Compile `SSHExample` (three separate jobs)

`examples/SSHExample/SSHExample.ino` is built with explicit stacks:

1. **UNO WiFi Rev2 + WiFiNINA** — `arduino:megaavr:uno2018`, `-DANSITERM_SSH_USE_WIFI_NINA`, `WiFiNINA` library.
2. **Mega + Ethernet** — `arduino:avr:mega`, `-DANSITERM_SSH_USE_ETHERNET`, `Ethernet` library.
3. **ESP32** — `esp32:esp32:esp32`, ESP32 core from [Espressif’s package index](https://espressif.github.io/arduino-esp32/package_esp32_index.json).

## CodeQL job

- Initializes CodeQL for `cpp` with `build-mode: manual` ([github/codeql-action](https://github.com/github/codeql-action)).
- Installs **arduino:avr**, **arduino:megaavr**, libraries **SD**, **Ethernet**, **WiFiNINA**, and **esp32:esp32** (same additional URL as Arduino CI).
- **Example sketches** (excluding `SSHExample`): compiled once with `--fqbn arduino:avr:mega --library .` (Mega chosen so RAM-heavy examples fit).
- **SSHExample:** same three compiles as Arduino CI (megaavr + WiFiNINA, Mega + Ethernet, ESP32).
- Runs `github/codeql-action/analyze@v3` after builds.

## Maintainer checklist

- [ ] Confirm `library.properties` maintainer/contact fields match what you want for releases and registry (if submitting to Library Manager later).
- [ ] Optional: tighten lint back to strict / Library Manager mode after removing `.development` if it appears (duplicate `ANSITerm/` subtree has been removed from the repo).
- [ ] CI green on `master` / `main` for both workflows.

## Related notes

- Historic Travis configuration is not part of the current CI path; automation is GitHub Actions only.
- To extend coverage, add FQBNs to the AVR matrix or add jobs mirroring the SSH pattern (extra cores + `--library .`).
