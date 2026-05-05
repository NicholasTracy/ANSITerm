/*
 * ANSITerm — SSHExample (network stream)
 * https://github.com/NicholasTracy/ANSITerm
 *
 * What you learn: Constructing ANSITerm on a TCP client instead of Serial, and printing
 * the same ANSI UI once a client connects (telnet-style).
 *
 * Hardware: ESP8266/ESP32 Wi-Fi, WiFiNINA, or Ethernet — see CI defines ANSITERM_SSH_USE_*.
 * Client: `telnet <ip> 23` or similar raw TCP terminal (this sketch is not full SSH crypto).
 * Each new TCP session runs the UI once; pollHostTerminalReconnect() applies to Serial only here.
 *
 * License: LGPL-3.0 — see LICENSE.txt in the library root.
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

// Wi-Fi only — ignored when building for Ethernet.
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Ethernet: hardware MAC and optional static fallback if DHCP fails
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress fallbackIp(192, 168, 1, 177);
IPAddress fallbackDns(192, 168, 1, 1);
IPAddress fallbackGateway(192, 168, 1, 1);
IPAddress fallbackSubnet(255, 255, 255, 0);

NetworkServer server(23); // Raw TCP port (telnet convention — not encrypted SSH)
NetworkClient client;
ANSITerm terminal(client);

bool connectNetwork();
IPAddress getLocalIP();
IPAddress getSubnetMask();
IPAddress getGatewayIP();
IPAddress getDnsIP();
void ipToText(IPAddress ip, char* out, size_t outSize);

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
        char ipText[16];
        char subnetText[16];
        char gatewayText[16];
        char dnsText[16];
        ipToText(ip, ipText, sizeof(ipText));
        ipToText(subnet, subnetText, sizeof(subnetText));
        ipToText(gateway, gatewayText, sizeof(gatewayText));
        ipToText(dns, dnsText, sizeof(dnsText));

        terminal.writeTextAt(4, 2, "IP Address: ");
        terminal.writeTextAt(4, 20, ipText);

        terminal.writeTextAt(5, 2, "Subnet Mask: ");
        terminal.writeTextAt(5, 20, subnetText);

        terminal.writeTextAt(6, 2, "Gateway: ");
        terminal.writeTextAt(6, 20, gatewayText);

        terminal.writeTextAt(7, 2, "DNS: ");
        terminal.writeTextAt(7, 20, dnsText);

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

void ipToText(IPAddress ip, char* out, size_t outSize) {
    snprintf(out, outSize, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
}
