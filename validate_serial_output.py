import serial
import sys
import time

def read_serial(port):
    ser = serial.Serial(port, 9600, timeout=10)
    time.sleep(2)  # Wait for the board to reset
    lines = []
    while True:
        line = ser.readline().decode('utf-8').strip()
        if line:
            print(line)  # Print for debugging purposes
            lines.append(line)
        # We expect a certain sequence of messages from the sketch
        if "Button Clicked!" in line:
            break  # Break the loop when the expected end message is received
    ser.close()
    return lines

def validate_output(lines):
    expected_output = [
        "Welcome to ANSITerm!",
        "This is a box drawn with ANSI characters.",
        "Button Clicked!"
    ]
    
    for expected in expected_output:
        if not any(expected in line for line in lines):
            print(f"Expected output '{expected}' not found in serial output.")
            sys.exit(1)  # Exit with error if any expected output is missing
    
    print("All expected output found. Test Passed.")
    sys.exit(0)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 validate_serial_output.py <serial_port>")
        sys.exit(1)

    port = sys.argv[1]
    output_lines = read_serial(port)
    validate_output(output_lines)
