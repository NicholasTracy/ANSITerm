/*
 * ANSITerm — NetworkTerminalExample
 * https://github.com/NicholasTracy/ANSITerm
 *
 * What you learn: Constructing ANSITerm on a TCP client Stream instead of Serial,
 * and printing a simple ANSI UI once a client connects (raw TCP / telnet-style).
 *
 * Hardware: ESP8266/ESP32 Wi-Fi, WiFiNINA, or Ethernet — see CI defines ANSITERM_NET_USE_*.
 * Client: `telnet <ip> 23` or any raw TCP terminal. This is not encrypted SSH.
 *
 * License: LGPL-3.0 — see LICENSE.txt in the library root.
 */

#include <ANSITerm.h>

#if defined(ARDUINO_ARCH_ESP32)
  #include <WiFi.h>
  #define ANSITERM_NET_TRANSPORT_NAME "WiFi (ESP32)"
  using NetworkServer = WiFiServer;
  using NetworkClient = WiFiClient;
#elif defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
  #define ANSITERM_NET_TRANSPORT_NAME "WiFi (ESP8266)"
  using NetworkServer = WiFiServer;
  using NetworkClient = WiFiClient;
#elif defined(ANSITERM_NET_USE_ETHERNET) || defined(ANSITERM_SSH_USE_ETHERNET)
  #include <SPI.h>
  #include <Ethernet.h>
  #define ANSITERM_NET_TRANSPORT_NAME "Ethernet"
  using NetworkServer = EthernetServer;
  using NetworkClient = EthernetClient;
#elif defined(ANSITERM_NET_USE_WIFI_NINA) || defined(ANSITERM_SSH_USE_WIFI_NINA)
  #include <SPI.h>
  #include <WiFiNINA.h>
  #define ANSITERM_NET_TRANSPORT_NAME "WiFiNINA"
  using NetworkServer = WiFiServer;
  using NetworkClient = WiFiClient;
#elif defined(ARDUINO_ARCH_AVR) && __has_include(<Ethernet.h>)
  #include <SPI.h>
  #include <Ethernet.h>
  #define ANSITERM_NET_TRANSPORT_NAME "Ethernet"
  using NetworkServer = EthernetServer;
  using NetworkClient = EthernetClient;
#elif __has_include(<WiFiNINA.h>)
  #include <SPI.h>
  #include <WiFiNINA.h>
  #define ANSITERM_NET_TRANSPORT_NAME "WiFiNINA"
  using NetworkServer = WiFiServer;
  using NetworkClient = WiFiClient;
#elif __has_include(<Ethernet.h>)
  #include <SPI.h>
  #include <Ethernet.h>
  #define ANSITERM_NET_TRANSPORT_NAME "Ethernet"
  using NetworkServer = EthernetServer;
  using NetworkClient = EthernetClient;
#else
  #error "NetworkTerminalExample requires ESP WiFi, WiFiNINA, or Ethernet support."
#endif

// Wi-Fi only — ignored when building for Ethernet.
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
const unsigned long WIFI_CONNECT_TIMEOUT_MS = 20000;

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
void serveClientSession();

void setup() {
    Serial.begin(9600);
    delay(100);

    if (!connectNetwork()) {
        Serial.println("Network initialization failed.");
        return;
    }

    server.begin();

    Serial.print("IP Address: ");
    Serial.println(getLocalIP());
    Serial.println("Waiting for TCP clients on port 23 (telnet-style)...");
}

void loop() {
    client = server.available();
    if (client) {
        serveClientSession();
        client.stop();
    }
    delay(10);
    yield();
}

void serveClientSession() {
    terminal.begin(true, true, true, true, "yellow", "black");

    IPAddress ip = getLocalIP();
    IPAddress subnet = getSubnetMask();
    IPAddress gateway = getGatewayIP();
    IPAddress dns = getDnsIP();

    terminal.setTextColor("yellow");
    terminal.writeTextAt(2, 2, "Network Information:");

    terminal.setTextColor("white");
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

    terminal.setTextColor("green");
    terminal.writeTextAt(9, 2, "TCP terminal ready (telnet-style, not SSH).");
    terminal.writeTextAt(10, 2, "Disconnect the client to end this session.");

    while (client.connected()) {
        // Keep the session alive without a tight busy-loop.
        while (client.available()) {
            (void)client.read();
        }
        delay(20);
        yield();
    }
}

bool connectNetwork() {
    Serial.print("Initializing ");
    Serial.println(ANSITERM_NET_TRANSPORT_NAME);

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266) || defined(ANSITERM_NET_USE_WIFI_NINA) || defined(ANSITERM_SSH_USE_WIFI_NINA) || (!defined(ARDUINO_ARCH_AVR) && !defined(ANSITERM_NET_USE_ETHERNET) && !defined(ANSITERM_SSH_USE_ETHERNET) && __has_include(<WiFiNINA.h>))
    WiFi.begin(ssid, password);

    const unsigned long started = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - started >= WIFI_CONNECT_TIMEOUT_MS) {
            Serial.println();
            Serial.println("Wi-Fi connect timeout — check SSID/password.");
            return false;
        }
        delay(500);
        Serial.print(".");
        yield();
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
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266) || defined(ANSITERM_NET_USE_WIFI_NINA) || defined(ANSITERM_SSH_USE_WIFI_NINA) || (!defined(ARDUINO_ARCH_AVR) && !defined(ANSITERM_NET_USE_ETHERNET) && !defined(ANSITERM_SSH_USE_ETHERNET) && __has_include(<WiFiNINA.h>))
    return WiFi.localIP();
#else
    return Ethernet.localIP();
#endif
}

IPAddress getSubnetMask() {
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266) || defined(ANSITERM_NET_USE_WIFI_NINA) || defined(ANSITERM_SSH_USE_WIFI_NINA) || (!defined(ARDUINO_ARCH_AVR) && !defined(ANSITERM_NET_USE_ETHERNET) && !defined(ANSITERM_SSH_USE_ETHERNET) && __has_include(<WiFiNINA.h>))
    return WiFi.subnetMask();
#else
    return Ethernet.subnetMask();
#endif
}

IPAddress getGatewayIP() {
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266) || defined(ANSITERM_NET_USE_WIFI_NINA) || defined(ANSITERM_SSH_USE_WIFI_NINA) || (!defined(ARDUINO_ARCH_AVR) && !defined(ANSITERM_NET_USE_ETHERNET) && !defined(ANSITERM_SSH_USE_ETHERNET) && __has_include(<WiFiNINA.h>))
    return WiFi.gatewayIP();
#else
    return Ethernet.gatewayIP();
#endif
}

IPAddress getDnsIP() {
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266) || defined(ANSITERM_NET_USE_WIFI_NINA) || defined(ANSITERM_SSH_USE_WIFI_NINA) || (!defined(ARDUINO_ARCH_AVR) && !defined(ANSITERM_NET_USE_ETHERNET) && !defined(ANSITERM_SSH_USE_ETHERNET) && __has_include(<WiFiNINA.h>))
    return WiFi.dnsIP();
#else
    return Ethernet.dnsServerIP();
#endif
}

void ipToText(IPAddress ip, char* out, size_t outSize) {
    snprintf(out, outSize, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
}
