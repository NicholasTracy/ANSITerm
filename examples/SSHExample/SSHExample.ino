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

#if defined(ARDUINO_ARCH_ESP32)
  #include <WiFi.h>
  #define ANSITERM_SSH_TRANSPORT_NAME "WiFi (ESP32)"
  using NetworkServer = WiFiServer;
  using NetworkClient = WiFiClient;
#elif defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
  #define ANSITERM_SSH_TRANSPORT_NAME "WiFi (ESP8266)"
  using NetworkServer = WiFiServer;
  using NetworkClient = WiFiClient;
#elif defined(ANSITERM_SSH_USE_ETHERNET)
  #include <SPI.h>
  #include <Ethernet.h>
  #define ANSITERM_SSH_TRANSPORT_NAME "Ethernet"
  using NetworkServer = EthernetServer;
  using NetworkClient = EthernetClient;
#elif defined(ANSITERM_SSH_USE_WIFI_NINA)
  #include <SPI.h>
  #include <WiFiNINA.h>
  #define ANSITERM_SSH_TRANSPORT_NAME "WiFiNINA"
  using NetworkServer = WiFiServer;
  using NetworkClient = WiFiClient;
#elif defined(ARDUINO_ARCH_AVR) && __has_include(<Ethernet.h>)
  #include <SPI.h>
  #include <Ethernet.h>
  #define ANSITERM_SSH_TRANSPORT_NAME "Ethernet"
  using NetworkServer = EthernetServer;
  using NetworkClient = EthernetClient;
#elif __has_include(<WiFiNINA.h>)
  #include <SPI.h>
  #include <WiFiNINA.h>
  #define ANSITERM_SSH_TRANSPORT_NAME "WiFiNINA"
  using NetworkServer = WiFiServer;
  using NetworkClient = WiFiClient;
#elif __has_include(<Ethernet.h>)
  #include <SPI.h>
  #include <Ethernet.h>
  #define ANSITERM_SSH_TRANSPORT_NAME "Ethernet"
  using NetworkServer = EthernetServer;
  using NetworkClient = EthernetClient;
#else
  #error "SSHExample requires ESP WiFi, WiFiNINA, or Ethernet support."
#endif

// Your network credentials for WiFi-based transports
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Default MAC and fallback static network configuration for Ethernet transports
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress fallbackIp(192, 168, 1, 177);
IPAddress fallbackDns(192, 168, 1, 1);
IPAddress fallbackGateway(192, 168, 1, 1);
IPAddress fallbackSubnet(255, 255, 255, 0);

NetworkServer server(23);
NetworkClient client;
ANSITerm terminal(client);

bool connectNetwork();
IPAddress getLocalIP();
IPAddress getSubnetMask();
IPAddress getGatewayIP();
IPAddress getDnsIP();

void setup() {
    // Start serial communication (for debugging purposes)
    Serial.begin(9600);
    delay(100);

    if (!connectNetwork()) {
        Serial.println("Network initialization failed.");
        return;
    }

    // Start the server
    server.begin();

    // Print IP address to the serial monitor
    Serial.print("IP Address: ");
    Serial.println(getLocalIP());
}

void loop() {
    // Check if a client has connected
    client = server.available();
    if (client) {
        // Initialize the terminal with color names
        terminal.begin(true, true, true, true, "yellow", "black"); // yellow text on black background

        // Get IP address and other network information
        IPAddress ip = getLocalIP();
        IPAddress subnet = getSubnetMask();
        IPAddress gateway = getGatewayIP();
        IPAddress dns = getDnsIP();

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

bool connectNetwork() {
    Serial.print("Initializing ");
    Serial.println(ANSITERM_SSH_TRANSPORT_NAME);

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266) || defined(ANSITERM_SSH_USE_WIFI_NINA) || (!defined(ARDUINO_ARCH_AVR) && __has_include(<WiFiNINA.h>))
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Connected!");
    return true;
#else
    if (Ethernet.begin(mac) == 0) {
        Serial.println("DHCP failed, using fallback static IP.");
        Ethernet.begin(mac, fallbackIp, fallbackDns, fallbackGateway, fallbackSubnet);
    }

    delay(1000);
    return Ethernet.hardwareStatus() != EthernetNoHardware;
#endif
}

IPAddress getLocalIP() {
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266) || defined(ANSITERM_SSH_USE_WIFI_NINA) || (!defined(ARDUINO_ARCH_AVR) && __has_include(<WiFiNINA.h>))
    return WiFi.localIP();
#else
    return Ethernet.localIP();
#endif
}

IPAddress getSubnetMask() {
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266) || defined(ANSITERM_SSH_USE_WIFI_NINA) || (!defined(ARDUINO_ARCH_AVR) && __has_include(<WiFiNINA.h>))
    return WiFi.subnetMask();
#else
    return Ethernet.subnetMask();
#endif
}

IPAddress getGatewayIP() {
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266) || defined(ANSITERM_SSH_USE_WIFI_NINA) || (!defined(ARDUINO_ARCH_AVR) && __has_include(<WiFiNINA.h>))
    return WiFi.gatewayIP();
#else
    return Ethernet.gatewayIP();
#endif
}

IPAddress getDnsIP() {
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266) || defined(ANSITERM_SSH_USE_WIFI_NINA) || (!defined(ARDUINO_ARCH_AVR) && __has_include(<WiFiNINA.h>))
    return WiFi.dnsIP();
#else
    return Ethernet.dnsServerIP();
#endif
}
