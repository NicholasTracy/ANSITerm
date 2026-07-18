#!/usr/bin/env python3
"""Optional hardware smoke test for BasicExample (not wired into CI).

Requires: pyserial (`pip install pyserial`) and a board running BasicExample
at 9600 baud. Mouse click text is only observed if the terminal/host injects
an SGR click or you click in a connected ANSI terminal that shares the port
(unusual). Prefer checking the welcome/box strings for unattended use.
"""

import serial
import sys
import time

EXPECTED_SNIPPETS = [
    "Welcome to ANSITerm!",
    "Box drawing with ANSI characters.",
]

OPTIONAL_CLICK = "Button clicked!"
DEFAULT_DEADLINE_S = 30


def read_serial(port, deadline_s=DEFAULT_DEADLINE_S):
    ser = serial.Serial(port, 9600, timeout=1)
    time.sleep(2)  # Wait for the board to reset
    lines = []
    deadline = time.monotonic() + deadline_s
    while time.monotonic() < deadline:
        raw = ser.readline()
        if not raw:
            continue
        line = raw.decode("utf-8", errors="replace").strip()
        if not line:
            continue
        print(line)
        lines.append(line)
        if OPTIONAL_CLICK in line:
            break
        if all(any(snippet in l for l in lines) for snippet in EXPECTED_SNIPPETS):
            # Core UI rendered; click message is optional without mouse injection.
            break
    ser.close()
    return lines


def validate_output(lines):
    for expected in EXPECTED_SNIPPETS:
        if not any(expected in line for line in lines):
            print(f"Expected output '{expected}' not found in serial output.")
            sys.exit(1)

    if any(OPTIONAL_CLICK in line for line in lines):
        print(f"Optional click message found ({OPTIONAL_CLICK}).")
    else:
        print(
            f"Note: '{OPTIONAL_CLICK}' not seen (expected without a mouse click)."
        )

    print("All required output found. Test Passed.")
    sys.exit(0)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 validate_serial_output.py <serial_port>")
        sys.exit(1)

    port = sys.argv[1]
    output_lines = read_serial(port)
    validate_output(output_lines)
