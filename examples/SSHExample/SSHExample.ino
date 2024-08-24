/*
 * ANSITerm Library - Arduino ANSI Terminal Control Library
 * https://github.com/NicholasTracy/ANSITerm
 * 
 * Author: Nicholas Tracy (2024)
 * https://github.com/NicholasTracy
 *
 * File: SSHExample.ino
 * 
 * Description:
 * This example demonstrates using the ANSITerm library over a network connection. 
 * It sets up a basic terminal interface that displays the network status, including 
 * the IP address and other relevant information. This example assumes the use of 
 * a WiFi-capable board such as the ESP8266 or ESP32, and a TCP server for SSH-like interaction.
 * 
 * License:
 * This library is licensed under the GNU General Public License v3 (GPLv3).
 * You are free to use, modify, and distribute this library, provided that you comply with 
 * the terms of the GPLv3. There are no warranties, expressed or implied, provided with 
 * this software.
 * 
 * For more details, see the full license at: https://www.gnu.org/licenses/gpl-3.0.en.html
 * 
 */

#include <ANSITerm.h>
#include <WiFi.h>         // Include WiFi library for network status (use <ESP8266WiFi.h> for ESP8266)
#include <WiFiClient.h>

// Your network credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Server settings
WiFiServer server(23);    // Create a TCP server on port 23 (Telnet port)
WiFiClient client;        // Create a client object

// Initialize ANSITerm with a WiFiClient stream
ANSITerm terminal(client);

void setup() {
    // Start serial communication (for debugging purposes)
    Serial.begin(9600);
    delay(100);

    // Connect to WiFi
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected!");

    // Start the server
    server.begin();

    // Print IP address to the serial monitor
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    // Check if a client has connected
    client = server.available();
    if (client) {
        // Initialize the terminal with color names
        terminal.begin(true, true, true, true, "yellow", "black"); // yellow text on black background

        // Get IP address and other network information
        IPAddress ip = WiFi.localIP();
        IPAddress subnet = WiFi.subnetMask();
        IPAddress gateway = WiFi.gatewayIP();
        IPAddress dns = WiFi.dnsIP();

        // Display network information
        terminal.setTextColor("yellow"); // Set text color to yellow
        terminal.writeTextAt(2, 2, "Network Information:");

        terminal.setTextColor("white"); // Set text color to white
        terminal.writeTextAt(4, 2, "IP Address: ");
        terminal.writeTextAt(4, 20, ip.toString().c_str());

        terminal.writeTextAt(5, 2, "Subnet Mask: ");
        terminal.writeTextAt(5, 20, subnet.toString().c_str());

        terminal.writeTextAt(6, 2, "Gateway: ");
        terminal.writeTextAt(6, 20, gateway.toString().c_str());

        terminal.writeTextAt(7, 2, "DNS: ");
        terminal.writeTextAt(7, 20, dns.toString().c_str());

        // Display a prompt
        terminal.setTextColor("green"); // Set text color to green
        terminal.writeTextAt(9, 2, "SSH Terminal Ready...");

        while (client.connected()) {
            // Handle client connection or implement additional functionalities here
        }

        // Close the connection when the client disconnects
        client.stop();
    }
}
