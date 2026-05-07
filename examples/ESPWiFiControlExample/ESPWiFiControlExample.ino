/*
 * ANSITerm — ESPWiFiControlExample
 * https://github.com/NicholasTracy/ANSITerm
 *
 * What you learn: A dual-terminal Wi-Fi control dashboard for ESP8266/ESP32.
 * The same ANSI UI is rendered on USB Serial and on a network terminal client.
 *
 * Network terminal: raw TCP on port 23 (telnet-style). This is not encrypted SSH.
 * Mouse clicks are supported when the client sends SGR mouse reports; arrow keys +
 * Enter always work as a fallback.
 *
 * License: LGPL-3.0 — see LICENSE.txt in the library root.
 */

#include <ANSITerm.h>

#if defined(ARDUINO_ARCH_ESP32)
  #include <WiFi.h>
#elif defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
#else
  #error "ESPWiFiControlExample supports ESP32 and ESP8266 only."
#endif
#include <EEPROM.h>

const char* STA_SSID = "your_wifi_ssid";
const char* STA_PASS = "your_wifi_password";
const char* AP_SSID = "ANSITerm-ESP";
const char* AP_PASS = "ansiterm123";
const uint16_t TCP_TERMINAL_PORT = 23;

const uint32_t UI_REFRESH_MS = 300;
const uint32_t STA_CONNECT_TIMEOUT_MS = 12000;
const uint8_t MAX_SCAN_RESULTS = 8;
const uint8_t MAX_SSID_LEN = 32;
const uint8_t MAX_PASS_LEN = 64;
const uint16_t SETTINGS_MAGIC = 0xA75E;
const uint8_t SETTINGS_VERSION = 1;
const size_t EEPROM_SIZE = 512;

WiFiServer tcpServer(TCP_TERMINAL_PORT);
WiFiClient tcpClient;

ANSITerm serialTerm(Serial);
ANSITerm* networkTerm = nullptr;

enum MenuItem : uint8_t {
    ItemMode = 0,
    ItemScan = 1,
    ItemPrevNetwork = 2,
    ItemNextNetwork = 3,
    ItemEditPasskey = 4,
    ItemConnectSta = 5,
    ItemDisconnectSta = 6,
    ItemToggleAp = 7,
    ItemRefresh = 8,
    ItemCount = 9
};

enum WiFiModeChoice : uint8_t { ModeSTA = 0, ModeAP = 1, ModeAPSTA = 2 };

struct UiSession {
    bool initialized = false;
    uint8_t selected = 0;
    uint8_t lastDrawnSelected = ItemCount;
    bool editingPasskey = false;
};

UiSession serialUi;
UiSession networkUi;

WiFiModeChoice selectedMode = ModeAPSTA;
bool apEnabled = false;
bool staConnecting = false;
unsigned long staConnectStartedAt = 0;
unsigned long lastUiPaintMs = 0;
bool forceRedraw = true;

char selectedSsid[MAX_SSID_LEN + 1] = {0};
char selectedPass[MAX_PASS_LEN + 1] = {0};
char scanSsid[MAX_SCAN_RESULTS][MAX_SSID_LEN + 1];
int32_t scanRssi[MAX_SCAN_RESULTS];
uint8_t scanCount = 0;
int8_t selectedScanIndex = -1;
char lastMessage[48] = "Ready";

struct PersistedSettings {
    uint16_t magic;
    uint8_t version;
    uint8_t mode;
    uint8_t apEnabled;
    char ssid[MAX_SSID_LEN + 1];
    char pass[MAX_PASS_LEN + 1];
};

void beginTerminal(ANSITerm& term, bool enableMouse);
void ensureNetworkClient();
void initWiFi();
void setWiFiMode(WiFiModeChoice mode);
void startApIfNeeded();
void stopApIfNeeded();
void restartTcpTerminalServer();
void startStaConnection();
void disconnectSta();
void scanForNetworks();
void loadSettings();
void saveSettings();
void tickConnectionState();
void handleInputs();
void handleOneTerminalInput(ANSITerm& term, Stream& stream, UiSession& ui);
void handlePasskeyTyping(Stream& stream, UiSession& ui);
void performMenuAction(MenuItem action);
void renderAll();
void renderDashboardStatic(ANSITerm& term, bool networkView);
void renderDashboardDynamic(ANSITerm& term, UiSession& ui);
void drawButton(ANSITerm& term, uint8_t row1, uint8_t col1, uint8_t row2, uint8_t col2,
                const char* label, bool selected);
void writeField(ANSITerm& term, uint8_t row, uint8_t col, uint8_t width, const char* text);
void drawActionButtonById(ANSITerm& term, uint8_t id, bool selected);
void applySelectedNetworkFromScan();
void maskPasskey(const char* pass, char* out, size_t outSize);
void passkeyPreview(const char* pass, bool editing, char* out, size_t outSize);
void setMessage(const char* text);
const char* modeLabel(WiFiModeChoice mode);
const char* statusLabel(int status);
void ipToText(IPAddress ip, char* out, size_t outSize);
void formatSignalBars(long rssi, char* out, size_t outSize);
uint8_t buttonAt(uint8_t row, uint8_t col);

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        ;
    }

    beginTerminal(serialTerm, true);
    EEPROM.begin(EEPROM_SIZE);
    loadSettings();
    initWiFi();
    restartTcpTerminalServer();

    forceRedraw = true;
    lastUiPaintMs = 0;
}

void loop() {
    ensureNetworkClient();
    tickConnectionState();
    handleInputs();

    const unsigned long now = millis();
    if (forceRedraw || now - lastUiPaintMs >= UI_REFRESH_MS) {
        renderAll();
        lastUiPaintMs = now;
        forceRedraw = false;
    }
}

void beginTerminal(ANSITerm& term, bool enableMouse) {
    term.begin(true, true, enableMouse, false, "white", "black");
}

void ensureNetworkClient() {
    if (tcpClient && !tcpClient.connected()) {
        tcpClient.stop();
        if (networkTerm) {
            delete networkTerm;
            networkTerm = nullptr;
        }
        networkUi.initialized = false;
        networkUi.lastDrawnSelected = ItemCount;
        forceRedraw = true;
    }

    if (tcpClient && tcpClient.connected()) {
        return;
    }

    WiFiClient incoming = tcpServer.available();
    if (!incoming) {
        return;
    }

    tcpClient = incoming;
    if (networkTerm) {
        delete networkTerm;
        networkTerm = nullptr;
    }
    networkTerm = new ANSITerm(tcpClient);
    beginTerminal(*networkTerm, true);
    networkUi.selected = serialUi.selected;
    networkUi.initialized = false;
    networkUi.lastDrawnSelected = ItemCount;
    forceRedraw = true;
}

void initWiFi() {
    setWiFiMode(selectedMode);
    startApIfNeeded();
    if ((selectedMode == ModeSTA || selectedMode == ModeAPSTA)
        && selectedSsid[0] != '\0'
        && strcmp(selectedSsid, "your_wifi_ssid") != 0) {
        WiFi.begin(selectedSsid, selectedPass);
        staConnecting = true;
        staConnectStartedAt = millis();
        setMessage("Auto-connecting STA");
    } else if (strcmp(selectedSsid, "your_wifi_ssid") == 0) {
        setMessage("Select SSID then Connect STA");
    }
}

void setWiFiMode(WiFiModeChoice mode) {
    selectedMode = mode;
    switch (mode) {
        case ModeSTA:
            WiFi.mode(WIFI_STA);
            apEnabled = false;
            break;
        case ModeAP:
            WiFi.mode(WIFI_AP);
            apEnabled = true;
            break;
        case ModeAPSTA:
            WiFi.mode(WIFI_AP_STA);
            apEnabled = true;
            break;
        default:
            WiFi.mode(WIFI_AP_STA);
            selectedMode = ModeAPSTA;
            apEnabled = true;
            break;
    }
    saveSettings();
    restartTcpTerminalServer();
}

void startApIfNeeded() {
    if (!apEnabled) {
        return;
    }
    WiFi.softAP(AP_SSID, AP_PASS);
    saveSettings();
    restartTcpTerminalServer();
}

void stopApIfNeeded() {
    WiFi.softAPdisconnect(true);
    saveSettings();
    restartTcpTerminalServer();
}

void restartTcpTerminalServer() {
    if (tcpClient && tcpClient.connected()) {
        tcpClient.stop();
    }
    if (networkTerm) {
        delete networkTerm;
        networkTerm = nullptr;
    }
    networkUi.initialized = false;
    networkUi.lastDrawnSelected = ItemCount;

    tcpServer.close();
    tcpServer.begin();
    setMessage("TCP terminal ready on :23");
    forceRedraw = true;
}

void startStaConnection() {
    if (selectedSsid[0] == '\0') {
        setMessage("No SSID selected");
        return;
    }
    if (strcmp(selectedSsid, "your_wifi_ssid") == 0) {
        setMessage("Set real SSID before connect");
        return;
    }
    WiFi.begin(selectedSsid, selectedPass);
    staConnecting = true;
    staConnectStartedAt = millis();
    setMessage("Connecting...");
    saveSettings();
    restartTcpTerminalServer();
}

void disconnectSta() {
    WiFi.disconnect();
    staConnecting = false;
    setMessage("Disconnected STA");
    restartTcpTerminalServer();
}

void scanForNetworks() {
    setMessage("Scanning...");
    const int found = WiFi.scanNetworks();
    if (found <= 0) {
        scanCount = 0;
        selectedScanIndex = -1;
        setMessage("No networks found");
        return;
    }

    scanCount = static_cast<uint8_t>(found > MAX_SCAN_RESULTS ? MAX_SCAN_RESULTS : found);
    for (uint8_t i = 0; i < scanCount; i++) {
        String ssid = WiFi.SSID(i);
        snprintf(scanSsid[i], sizeof(scanSsid[i]), "%s", ssid.c_str());
        scanRssi[i] = WiFi.RSSI(i);
    }
    selectedScanIndex = 0;
    applySelectedNetworkFromScan();
    setMessage("Scan complete");
    saveSettings();
}

void tickConnectionState() {
    if (!staConnecting) {
        return;
    }

    const int status = WiFi.status();
    if (status == WL_CONNECTED || status == WL_CONNECT_FAILED || status == WL_NO_SSID_AVAIL) {
        staConnecting = false;
        if (status == WL_CONNECTED) {
            setMessage("Connected");
            restartTcpTerminalServer();
        } else if (status == WL_CONNECT_FAILED) {
            setMessage("Connect failed");
        } else {
            setMessage("SSID not found");
        }
        forceRedraw = true;
        return;
    }

    const unsigned long now = millis();
    if (now - staConnectStartedAt > STA_CONNECT_TIMEOUT_MS) {
        WiFi.disconnect();
        staConnecting = false;
        setMessage("Connect timeout");
        forceRedraw = true;
    }
}

void handleInputs() {
    handleOneTerminalInput(serialTerm, Serial, serialUi);
    if (networkTerm && tcpClient && tcpClient.connected()) {
        handleOneTerminalInput(*networkTerm, tcpClient, networkUi);
    }
}

void handleOneTerminalInput(ANSITerm& term, Stream& stream, UiSession& ui) {
    if (ui.editingPasskey) {
        handlePasskeyTyping(stream, ui);
        return;
    }

    // PuTTY/telnet may not emit SGR mouse or CSI arrows consistently.
    // Accept plain keys as a universal fallback.
    while (stream.available() && stream.peek() != '\033') {
        const int raw = stream.read();
        if (raw < 0) {
            break;
        }
        const char c = static_cast<char>(raw);
        if (c == 'w' || c == 'W' || c == 'k' || c == 'K') {
            if (ui.selected == 0) {
                ui.selected = ItemCount - 1;
            } else {
                ui.selected--;
            }
            forceRedraw = true;
            continue;
        }
        if (c == 's' || c == 'S' || c == 'j' || c == 'J') {
            ui.selected = static_cast<uint8_t>((ui.selected + 1) % ItemCount);
            forceRedraw = true;
            continue;
        }
        if (c == '\r' || c == '\n' || c == ' ') {
            performMenuAction(static_cast<MenuItem>(ui.selected));
            forceRedraw = true;
            continue;
        }
    }

    ANSITermInput ev;
    while (term.pollInput(ev)) {
        if (ev.kind == ANSITermInput::ArrowUp) {
            if (ui.selected == 0) {
                ui.selected = ItemCount - 1;
            } else {
                ui.selected--;
            }
            forceRedraw = true;
        } else if (ev.kind == ANSITermInput::ArrowDown) {
            ui.selected = static_cast<uint8_t>((ui.selected + 1) % ItemCount);
            forceRedraw = true;
        } else if (ev.kind == ANSITermInput::Enter) {
            performMenuAction(static_cast<MenuItem>(ui.selected));
            forceRedraw = true;
        } else if (ev.kind == ANSITermInput::MousePress) {
            const uint8_t hit = buttonAt(ev.mouseRow, ev.mouseCol);
            if (hit < ItemCount) {
                ui.selected = hit;
                performMenuAction(static_cast<MenuItem>(hit));
                forceRedraw = true;
            }
        }
    }
}

void handlePasskeyTyping(Stream& stream, UiSession& ui) {
    while (stream.available()) {
        const int raw = stream.read();
        if (raw < 0) {
            return;
        }
        const char c = static_cast<char>(raw);
        if (c == '\r' || c == '\n') {
            ui.editingPasskey = false;
            serialUi.editingPasskey = false;
            networkUi.editingPasskey = false;
            saveSettings();
            setMessage("Passkey updated");
            forceRedraw = true;
            return;
        }
        if (c == 8 || c == 127) {
            const size_t len = strlen(selectedPass);
            if (len > 0) {
                selectedPass[len - 1] = '\0';
            }
            forceRedraw = true;
            continue;
        }
        if (c >= 32 && c <= 126) {
            const size_t len = strlen(selectedPass);
            if (len < MAX_PASS_LEN) {
                selectedPass[len] = c;
                selectedPass[len + 1] = '\0';
                forceRedraw = true;
            }
        }
    }
}

void performMenuAction(MenuItem action) {
    if (action == ItemMode) {
        selectedMode = static_cast<WiFiModeChoice>((selectedMode + 1) % 3);
        const bool needAp = (selectedMode != ModeSTA);
        setWiFiMode(selectedMode);
        if (needAp) {
            startApIfNeeded();
        } else {
            stopApIfNeeded();
        }
        return;
    }

    if (action == ItemScan) {
        scanForNetworks();
        return;
    }

    if (action == ItemPrevNetwork) {
        if (scanCount > 0) {
            if (selectedScanIndex <= 0) {
                selectedScanIndex = scanCount - 1;
            } else {
                selectedScanIndex--;
            }
            applySelectedNetworkFromScan();
        } else {
            setMessage("Scan first");
        }
        return;
    }

    if (action == ItemNextNetwork) {
        if (scanCount > 0) {
            selectedScanIndex = (selectedScanIndex + 1) % scanCount;
            applySelectedNetworkFromScan();
        } else {
            setMessage("Scan first");
        }
        return;
    }

    if (action == ItemEditPasskey) {
        serialUi.editingPasskey = true;
        if (networkTerm && tcpClient && tcpClient.connected()) {
            networkUi.editingPasskey = true;
        }
        setMessage("Typing passkey (Enter to save)");
        return;
    }

    if (action == ItemConnectSta) {
        startStaConnection();
        return;
    }

    if (action == ItemDisconnectSta) {
        disconnectSta();
        return;
    }

    if (action == ItemToggleAp) {
        apEnabled = !apEnabled;
        if (apEnabled) {
            if (selectedMode == ModeSTA) {
                setWiFiMode(ModeAPSTA);
            }
            startApIfNeeded();
        } else {
            if (selectedMode == ModeAP) {
                setWiFiMode(ModeSTA);
            }
            stopApIfNeeded();
        }
        saveSettings();
        return;
    }

    if (action == ItemRefresh) {
        forceRedraw = true;
        setMessage("Refreshed");
    }
}

void renderAll() {
    if (!serialUi.initialized) {
        renderDashboardStatic(serialTerm, false);
        serialUi.initialized = true;
        serialUi.lastDrawnSelected = ItemCount;
    }
    renderDashboardDynamic(serialTerm, serialUi);

    if (networkTerm && tcpClient && tcpClient.connected()) {
        if (!networkUi.initialized) {
            renderDashboardStatic(*networkTerm, true);
            networkUi.initialized = true;
            networkUi.lastDrawnSelected = ItemCount;
        }
        renderDashboardDynamic(*networkTerm, networkUi);
    }
}

void renderDashboardStatic(ANSITerm& term, bool networkView) {
    term.clearScreen();
    term.setTextColor("cyan");
    term.drawDoubleBox(1, 2, 4, 78);
    term.writeTextAt(2, 5, "ANSITerm ESP Wi-Fi Control Center");
    term.setTextColor("yellow");
    term.writeTextAt(3, 5, networkView ? "View: TCP terminal :23 (telnet-style)"
                                       : "View: USB serial terminal");

    term.setTextColor("white");
    term.drawBox(6, 2, 18, 48);
    term.setTextColor("green");
    term.writeTextAt(7, 4, "Connection Status");
    term.setTextColor("white");
    term.writeTextAt(9, 4, "Mode:");
    term.writeTextAt(10, 4, "STA:");
    term.writeTextAt(11, 4, "Signal:");
    term.writeTextAt(12, 4, "STA IP:");
    term.writeTextAt(13, 4, "AP  IP:");
    term.writeTextAt(14, 4, "Gateway:");
    term.writeTextAt(15, 4, "AP:");
    term.writeTextAt(16, 4, "Status:");
    term.writeTextAt(17, 4, "SSID:");
    term.writeTextAt(18, 4, "Pass:");

    term.setTextColor("magenta");
    term.drawBox(6, 50, 23, 78);
    term.writeTextAt(7, 53, "Actions");

    term.setTextColor("cyan");
    term.drawBox(19, 2, 23, 48);
    term.writeTextAt(20, 4, "Controls:");
    term.writeTextAt(21, 4, "Arrows or W/S or J/K + Enter");
    term.writeTextAt(22, 4, "Mouse click when supported by terminal");
}

void renderDashboardDynamic(ANSITerm& term, UiSession& ui) {
    char line[48];
    char bars[16];
    char ip[20];
    char masked[40];

    term.setTextColor("white");
    writeField(term, 9, 14, 32, modeLabel(selectedMode));
    writeField(term, 10, 14, 32, statusLabel(WiFi.status()));

    formatSignalBars(WiFi.RSSI(), bars, sizeof(bars));
    if (WiFi.status() == WL_CONNECTED) {
        snprintf(line, sizeof(line), "%s %ld dBm", bars, WiFi.RSSI());
    } else {
        snprintf(line, sizeof(line), "%s n/a", bars);
    }
    writeField(term, 11, 14, 32, line);

    ipToText(WiFi.localIP(), ip, sizeof(ip));
    writeField(term, 12, 14, 32, ip);
    ipToText(WiFi.softAPIP(), ip, sizeof(ip));
    writeField(term, 13, 14, 32, ip);
    ipToText(WiFi.gatewayIP(), ip, sizeof(ip));
    writeField(term, 14, 14, 32, ip);
    writeField(term, 15, 14, 32, apEnabled ? "Enabled" : "Disabled");
    writeField(term, 16, 14, 32, staConnecting ? "Connecting..." : lastMessage);
    writeField(term, 17, 14, 32, selectedSsid[0] ? selectedSsid : "(none)");
    passkeyPreview(selectedPass, ui.editingPasskey, masked, sizeof(masked));
    if (ui.editingPasskey) {
        writeField(term, 18, 14, 32, masked);
    } else {
        writeField(term, 18, 14, 32, masked);
    }

    if (ui.lastDrawnSelected >= ItemCount) {
        for (uint8_t i = 0; i < ItemCount; i++) {
            drawActionButtonById(term, i, i == ui.selected);
        }
        ui.lastDrawnSelected = ui.selected;
    } else if (ui.lastDrawnSelected != ui.selected) {
        if (ui.lastDrawnSelected < ItemCount) {
            drawActionButtonById(term, ui.lastDrawnSelected, false);
        }
        drawActionButtonById(term, ui.selected, true);
        ui.lastDrawnSelected = ui.selected;
    }

    if (selectedScanIndex >= 0 && selectedScanIndex < static_cast<int8_t>(scanCount)) {
        snprintf(line, sizeof(line), "Net %d/%d %lddBm", selectedScanIndex + 1, scanCount,
                 static_cast<long>(scanRssi[selectedScanIndex]));
        writeField(term, 19, 52, 25, line);
    } else {
        writeField(term, 19, 52, 25, "Net: (none)");
    }
    writeField(term, 20, 52, 25, selectedSsid[0] ? selectedSsid : "(scan/select)");
}

void drawButton(ANSITerm& term, uint8_t row1, uint8_t col1, uint8_t row2, uint8_t col2,
                const char* label, bool selected) {
    if (selected) {
        term.setTextColor("black");
        term.setBackgroundColor("yellow");
        term.drawDoubleButton(row1, col1, row2, col2, label);
        term.setBackgroundColor("black");
        term.setTextColor("white");
    } else {
        term.setTextColor("white");
        term.drawButton(row1, col1, row2, col2, label);
    }
}

void writeField(ANSITerm& term, uint8_t row, uint8_t col, uint8_t width, const char* text) {
    if (!text) {
        text = "";
    }
    char buf[64];
    snprintf(buf, sizeof(buf), "%-*.*s", width, width, text);
    term.writeTextAt(row, col, buf);
}

void drawActionButtonById(ANSITerm& term, uint8_t id, bool selected) {
    char label[28];
    if (id == ItemMode) {
        snprintf(label, sizeof(label), "Mode: %s", modeLabel(selectedMode));
    } else if (id == ItemScan) {
        snprintf(label, sizeof(label), "Scan Wi-Fi");
    } else if (id == ItemPrevNetwork) {
        snprintf(label, sizeof(label), "Prev Network");
    } else if (id == ItemNextNetwork) {
        snprintf(label, sizeof(label), "Next Network");
    } else if (id == ItemEditPasskey) {
        snprintf(label, sizeof(label), "Edit Passkey");
    } else if (id == ItemConnectSta) {
        snprintf(label, sizeof(label), "Connect STA");
    } else if (id == ItemDisconnectSta) {
        snprintf(label, sizeof(label), "Disconnect STA");
    } else if (id == ItemToggleAp) {
        snprintf(label, sizeof(label), "Toggle AP");
    } else {
        snprintf(label, sizeof(label), "Refresh");
    }

    const uint8_t row = static_cast<uint8_t>(9 + id);
    if (selected) {
        term.setTextColor("black");
        term.setBackgroundColor("yellow");
    } else {
        term.setTextColor("white");
        term.setBackgroundColor("black");
    }

    char line[28];
    snprintf(line, sizeof(line), "%c %-22s", selected ? '>' : ' ', label);
    writeField(term, row, 52, 25, line);
    term.setBackgroundColor("black");
    term.setTextColor("white");
}

const char* modeLabel(WiFiModeChoice mode) {
    if (mode == ModeSTA) {
        return "STA";
    }
    if (mode == ModeAP) {
        return "AP";
    }
    return "AP+STA";
}

const char* statusLabel(int status) {
    switch (status) {
        case WL_CONNECTED:
            return "Connected";
        case WL_NO_SSID_AVAIL:
            return "SSID not found";
        case WL_CONNECT_FAILED:
            return "Connect failed";
        case WL_CONNECTION_LOST:
            return "Connection lost";
        case WL_DISCONNECTED:
            return "Disconnected";
        case WL_IDLE_STATUS:
            return "Idle";
        default:
            return "Unknown";
    }
}

void ipToText(IPAddress ip, char* out, size_t outSize) {
    snprintf(out, outSize, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
}

void formatSignalBars(long rssi, char* out, size_t outSize) {
    if (WiFi.status() != WL_CONNECTED) {
        snprintf(out, outSize, "[-----]");
        return;
    }

    uint8_t level = 0;
    if (rssi >= -55) {
        level = 5;
    } else if (rssi >= -62) {
        level = 4;
    } else if (rssi >= -69) {
        level = 3;
    } else if (rssi >= -76) {
        level = 2;
    } else if (rssi >= -85) {
        level = 1;
    }

    char bars[8] = {'[', '-', '-', '-', '-', '-', ']', '\0'};
    for (uint8_t i = 0; i < level; i++) {
        bars[1 + i] = '#';
    }
    snprintf(out, outSize, "%s", bars);
}

void applySelectedNetworkFromScan() {
    if (selectedScanIndex < 0 || selectedScanIndex >= static_cast<int8_t>(scanCount)) {
        return;
    }
    snprintf(selectedSsid, sizeof(selectedSsid), "%s", scanSsid[selectedScanIndex]);
    selectedPass[0] = '\0';
    saveSettings();
    setMessage("Selected scanned SSID");
}

void maskPasskey(const char* pass, char* out, size_t outSize) {
    const size_t len = strlen(pass);
    if (len == 0) {
        snprintf(out, outSize, "(open)");
        return;
    }
    size_t n = len;
    if (n > outSize - 1) {
        n = outSize - 1;
    }
    for (size_t i = 0; i < n; i++) {
        out[i] = '*';
    }
    out[n] = '\0';
}

void passkeyPreview(const char* pass, bool editing, char* out, size_t outSize) {
    if (!editing) {
        maskPasskey(pass, out, outSize);
        return;
    }

    const size_t len = strlen(pass);
    if (len == 0) {
        snprintf(out, outSize, "(type passkey, Enter to save)");
        return;
    }

    char tmp[80];
    size_t n = len;
    if (n > sizeof(tmp) - 1) {
        n = sizeof(tmp) - 1;
    }
    for (size_t i = 0; i < n; i++) {
        tmp[i] = '*';
    }
    tmp[n] = '\0';
    tmp[n - 1] = pass[len - 1];
    snprintf(out, outSize, "%s  (Enter to save)", tmp);
}

void setMessage(const char* text) {
    if (!text) {
        text = "";
    }
    snprintf(lastMessage, sizeof(lastMessage), "%s", text);
}

void loadSettings() {
    PersistedSettings s;
    EEPROM.get(0, s);

    if (s.magic != SETTINGS_MAGIC || s.version != SETTINGS_VERSION) {
        selectedMode = ModeAPSTA;
        apEnabled = true;
        snprintf(selectedSsid, sizeof(selectedSsid), "%s", STA_SSID);
        snprintf(selectedPass, sizeof(selectedPass), "%s", STA_PASS);
        saveSettings();
        setMessage("Using default settings");
        return;
    }

    if (s.mode <= ModeAPSTA) {
        selectedMode = static_cast<WiFiModeChoice>(s.mode);
    } else {
        selectedMode = ModeAPSTA;
    }
    apEnabled = (s.apEnabled != 0);
    snprintf(selectedSsid, sizeof(selectedSsid), "%s", s.ssid);
    snprintf(selectedPass, sizeof(selectedPass), "%s", s.pass);
    setMessage("Settings restored");
}

void saveSettings() {
    PersistedSettings s;
    s.magic = SETTINGS_MAGIC;
    s.version = SETTINGS_VERSION;
    s.mode = static_cast<uint8_t>(selectedMode);
    s.apEnabled = apEnabled ? 1 : 0;
    snprintf(s.ssid, sizeof(s.ssid), "%s", selectedSsid);
    snprintf(s.pass, sizeof(s.pass), "%s", selectedPass);

    EEPROM.put(0, s);
    EEPROM.commit();
}

uint8_t buttonAt(uint8_t row, uint8_t col) {
    if (col >= 52 && col <= 76 && row >= 9 && row <= 17) {
        return static_cast<uint8_t>(row - 9);
    }
    return ItemCount;
}
