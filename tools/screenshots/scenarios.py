"""Per-example ANSI fixtures: static hero frames + interactive motion timelines."""

from __future__ import annotations

import random
from typing import Callable

from ansi_lib import AnsiBuilder, click_marker

FrameFn = Callable[[random.Random], list[str]]
StaticFn = Callable[[random.Random], str]


def _begin(b: AnsiBuilder, fg: str = "cyan", bg: str = "black") -> AnsiBuilder:
    return b.clear().hide_cursor().reset().bg(bg).fg(fg)


# --- Basic -----------------------------------------------------------------


def basic_static(_rng: random.Random) -> str:
    b = AnsiBuilder()
    _begin(b)
    b.fg("yellow").text_at(2, 2, "Welcome to ANSITerm!")
    b.fg("white").box(4, 5, 8, 50)
    b.fg("green").text_at(6, 8, "Box drawing with ANSI characters.")
    b.fg("cyan").button(10, 20, 12, 50, "Click Me!")
    b.fg("white").text_at(14, 2, "Activate: mouse click, Enter, or Space")
    return b.build()


def basic_motion(rng: random.Random) -> list[str]:
    frames = [basic_static(rng)]
    # Mouse moves toward button
    for col in (24, 30, 35):
        b = AnsiBuilder()
        b.raw(basic_static(rng))
        click_marker(b, 11, col)
        b.fg("white").text_at(16, 2, f"Mouse → ({11},{col})")
        frames.append(b.build())
    # Click
    b = AnsiBuilder()
    b.raw(basic_static(rng))
    b.fg("black").bg("white").text_at(11, 35, "X").reset()
    b.fg("yellow").text_at(16, 2, "SGR mouse press on button")
    frames.append(b.build())
    # Result
    b = AnsiBuilder()
    _begin(b, "white", "black")
    b.fg("green").text_at(6, 10, "Button clicked!")
    b.fg("white").text_at(8, 10, "(Enter / Space / mouse)")
    frames.append(b.build())
    return frames


# --- Advanced --------------------------------------------------------------

SYMBOLS = [
    ("\u2665", "Row 1, Col 1"),
    ("\u2666", "Row 1, Col 2"),
    ("\u2663", "Row 1, Col 3"),
    ("\u2660", "Row 1, Col 4"),
    ("\u2713", "Row 2, Col 1"),
    ("\u2717", "Row 2, Col 2"),
    ("\u263a", "Row 2, Col 3"),
    ("\u2605", "Row 2, Col 4"),
    ("\u2191", "Row 3, Col 1"),
    ("\u266b", "Row 3, Col 2"),
    ("\u221e", "Row 3, Col 3"),
    ("\u2193", "Row 3, Col 4"),
]


def _advanced_shell(b: AnsiBuilder, load_fill: int = 20) -> AnsiBuilder:
    _begin(b)
    b.fg("yellow").box(1, 10, 3, 70, double=True)
    b.text_at(2, 27, "Advanced Terminal Interface")
    b.fg("white").box(6, 20, 8, 60)
    bar = "\u2588" * load_fill + " " * (20 - load_fill)
    b.text_at(7, 25, f"Loading: [{bar}]")
    return b


def _advanced_full(b: AnsiBuilder) -> AnsiBuilder:
    _advanced_shell(b, 20)
    b.fg("cyan").button(10, 20, 12, 50, "\u2192 Start Again \u2190", double=True)
    b.fg("green").table(14, 10, 20, 70, 3, 4)
    cells = [
        (15, 12),
        (15, 27),
        (15, 42),
        (15, 57),
        (17, 12),
        (17, 27),
        (17, 42),
        (17, 57),
        (19, 12),
        (19, 27),
        (19, 42),
        (19, 57),
    ]
    for (r, c), (sym, label) in zip(cells, SYMBOLS):
        b.text_at(r, c, f"{sym} {label}")
    b.fg("white").text_at(22, 10, "Restart: click the button, Enter, or Space")
    return b


def advanced_static(_rng: random.Random) -> str:
    return _advanced_full(AnsiBuilder()).build()


def advanced_motion(rng: random.Random) -> list[str]:
    frames: list[str] = []
    for fill in range(0, 21, 2):
        frames.append(_advanced_shell(AnsiBuilder(), fill).build())
    frames.append(advanced_static(rng))
    # Keyboard activate
    b = AnsiBuilder()
    b.raw(advanced_static(rng))
    b.fg("yellow").text_at(23, 10, "Key: Enter → restart")
    frames.append(b.build())
    # Brief clear flash then again
    frames.append(_begin(AnsiBuilder()).fg("white").text_at(2, 2, "Restarting…").build())
    frames.append(advanced_static(rng))
    return frames


# --- Buttons ---------------------------------------------------------------


def _buttons_ui(selection: int, led_on: bool | None = None) -> str:
    b = AnsiBuilder()
    _begin(b, "white", "black")
    b.fg("cyan").text_at(2, 5, "Button interaction example")
    b.button(5, 10, 7, 40, "Toggle LED", selected=(selection == 0))
    b.button(9, 10, 11, 40, "Exit", double=True, selected=(selection == 1))
    b.fg("white").text_at(13, 5, "Keyboard: Arrow keys = move highlight   Enter = activate")
    b.text_at(14, 5, "Mouse: click works only if the terminal sends SGR mouse reports.")
    b.text_at(15, 5, "(PuTTY over USB Serial usually does not — use arrows + Enter.)")
    if led_on is not None:
        b.fg("green" if led_on else "red").text_at(16, 5, "LED is ON " if led_on else "LED is OFF")
    return b.build()


def buttons_static(_rng: random.Random) -> str:
    return _buttons_ui(0, False)


def buttons_motion(_rng: random.Random) -> list[str]:
    frames = [
        _buttons_ui(0, False),
        _buttons_ui(0, False) + AnsiBuilder().fg("yellow").text_at(18, 5, "Key: ArrowDown").build(),
        _buttons_ui(1, False),
        _buttons_ui(1, False) + AnsiBuilder().fg("yellow").text_at(18, 5, "Key: ArrowUp").build(),
        _buttons_ui(0, False),
        _buttons_ui(0, False) + AnsiBuilder().fg("yellow").text_at(18, 5, "Key: Enter").build(),
        _buttons_ui(0, True),
        _buttons_ui(0, True) + AnsiBuilder().fg("yellow").text_at(18, 5, "Mouse click Exit").build(),
    ]
    b = AnsiBuilder()
    _begin(b)
    b.fg("white").text_at(6, 10, "Goodbye.")
    frames.append(b.build())
    return frames


# --- Graphic ---------------------------------------------------------------

WALL = "\u2592"
PLAYER = "\u263a"
W, H = 40, 20


def _walls(b: AnsiBuilder) -> AnsiBuilder:
    b.fg("blue")
    for x in range(W):
        b.text_at(1, x + 1, WALL)
        b.text_at(H, x + 1, WALL)
    for y in range(H):
        b.text_at(y + 1, 1, WALL)
        b.text_at(y + 1, W, WALL)
    for y in range(5, H - 4):
        b.text_at(y + 1, 21, WALL)
    return b


def _graphic_frame(px: int, py: int, ex: int, ey: int, status: str = "") -> str:
    b = AnsiBuilder()
    _begin(b, "white", "black")
    _walls(b)
    b.fg("green").text_at(py + 1, px + 1, PLAYER)
    b.fg("red").text_at(ey + 1, ex + 1, "&")
    b.fg("white").text_at(H + 2, 2, "WASD move  Space fire  R restart")
    if status:
        b.fg("yellow").text_at(H + 3, 2, status)
    return b.build()


def graphic_static(_rng: random.Random) -> str:
    return _graphic_frame(10, 10, 30, 5)


def graphic_motion(rng: random.Random) -> list[str]:
    px, py, ex, ey = 10, 10, 30, 5
    frames = [_graphic_frame(px, py, ex, ey, "Start")]
    path = [("d", 1, 0), ("d", 1, 0), ("w", 0, -1), ("d", 1, 0), ("s", 0, 1), ("a", -1, 0)]
    for key, dx, dy in path:
        px += dx
        py += dy
        # enemy drifts left
        if ex > 22:
            ex -= 1
        frames.append(_graphic_frame(px, py, ex, ey, f"Key: {key.upper()}"))
    # fire / win flash
    frames.append(_graphic_frame(px, py, ex, ey, "Space → fire"))
    b = AnsiBuilder()
    _begin(b)
    _walls(b)
    b.fg("red").text_at(ey + 1, ex + 1, "X")
    b.fg("green").text_at(py + 1, px + 1, PLAYER)
    b.fg("red").text_at(H // 2 + 1, W // 2 - 4, "YOU WIN!")
    b.fg("white").text_at(H // 2 + 2, W // 2 - 9, "Press R to restart")
    frames.append(b.build())
    return frames


# --- ANS files -------------------------------------------------------------


def _ans_list(names: list[str], selection: int) -> str:
    b = AnsiBuilder()
    _begin(b)
    b.fg("cyan").text_at(2, 2, "SD card — .ans files:")
    b.fg("white").text_at(3, 2, "Arrows/Enter select; mouse click also works")
    for i, name in enumerate(names):
        row = 5 + i
        if i == selection:
            b.bg("blue").fg("white").text_at(row, 4, f"> {name}").reset().bg("black")
        else:
            b.fg("yellow").text_at(row, 4, f"  {name}")
    return b.build()


def ansfile_static(rng: random.Random) -> str:
    names = [f"demo{i:02d}.ans" for i in range(1, 5)]
    names[1] = f"art_{rng.randint(10, 99)}.ans"
    return _ans_list(names, 0)


def ansfile_motion(rng: random.Random) -> list[str]:
    names = [
        "welcome.ans",
        f"banner_{rng.randint(100, 999)}.ans",
        "logo.ans",
        f"scene_{rng.choice(['a', 'b', 'c'])}.ans",
    ]
    frames = [_ans_list(names, 0)]
    frames.append(_ans_list(names, 0) + AnsiBuilder().fg("yellow").text_at(12, 2, "Key: ArrowDown").build())
    frames.append(_ans_list(names, 1))
    frames.append(_ans_list(names, 1) + AnsiBuilder().fg("yellow").text_at(12, 2, "Key: Enter").build())
    b = AnsiBuilder()
    _begin(b)
    b.fg("white").text_at(2, 2, f"Showing: {names[1]}")
    b.fg("cyan").text_at(4, 2, "╔══════════════════════════╗")
    b.text_at(5, 2, "║  Sample .ans preview     ║")
    b.text_at(6, 2, "║  (truncated for screenshot)║")
    b.text_at(7, 2, "╚══════════════════════════╝")
    b.fg("green").button(22, 10, 24, 50, "Exit")
    b.fg("white").text_at(25, 10, "Exit: click, Enter, or Space")
    frames.append(b.build())
    frames.append(
        b.build()
        + AnsiBuilder().fg("yellow").text_at(26, 10, "Mouse click Exit").build()
    )
    frames.append(_ans_list(names, 1))
    return frames


# --- Network terminal ------------------------------------------------------


def network_static(rng: random.Random) -> str:
    ip = f"192.168.{rng.randint(0, 3)}.{rng.randint(2, 250)}"
    b = AnsiBuilder()
    _begin(b, "yellow", "black")
    b.fg("yellow").text_at(2, 2, "Network Information:")
    b.fg("white")
    b.text_at(4, 2, "IP Address: ")
    b.text_at(4, 20, ip)
    b.text_at(5, 2, "Subnet Mask: ")
    b.text_at(5, 20, "255.255.255.0")
    b.text_at(6, 2, "Gateway: ")
    b.text_at(6, 20, ".".join(ip.split(".")[:3] + ["1"]))
    b.text_at(7, 2, "DNS: ")
    b.text_at(7, 20, "8.8.8.8")
    b.fg("green").text_at(9, 2, "TCP terminal ready (telnet-style, not SSH).")
    b.text_at(10, 2, "Disconnect the client to end this session.")
    return b.build()


def network_motion(rng: random.Random) -> list[str]:
    ssid = f"HomeNet-{rng.randint(100, 999)}"
    frames: list[str] = []
    for n in range(0, 8):
        b = AnsiBuilder()
        _begin(b)
        b.fg("cyan").text_at(2, 2, "Wi-Fi connecting…")
        b.fg("white").text_at(4, 2, f"SSID: {ssid}")
        b.text_at(5, 2, "." * (n + 1))
        frames.append(b.build())
    frames.append(network_static(rng))
    b = AnsiBuilder()
    b.raw(network_static(rng))
    b.fg("yellow").text_at(12, 2, "Client connected on :23")
    frames.append(b.build())
    return frames


# --- ESP Wi-Fi control -----------------------------------------------------

MENU = [
    "Mode: AP+STA",
    "Scan Wi-Fi",
    "Prev Network",
    "Next Network",
    "Edit Passkey",
    "Connect STA",
    "Disconnect STA",
    "Toggle AP",
    "Refresh",
]


def _esp_dashboard(
    rng: random.Random,
    *,
    selection: int = 0,
    status: str = "Ready",
    ssid: str = "",
    scanning: bool = False,
    networks: list[tuple[str, int]] | None = None,
    connected: bool = False,
) -> str:
    if not ssid:
        ssid = f"CafeWiFi-{rng.randint(10, 99)}"
    nets = networks or []
    b = AnsiBuilder()
    _begin(b, "white", "black")
    b.fg("cyan").box(1, 2, 4, 78, double=True)
    b.text_at(2, 5, "ANSITerm ESP Wi-Fi Control Center")
    b.fg("yellow").text_at(3, 5, "View: USB serial terminal")
    b.fg("white").box(6, 2, 19, 48)
    b.fg("green").text_at(7, 4, "Connection Status")
    b.fg("white")
    b.text_at(9, 4, "Mode:    AP+STA")
    b.text_at(10, 4, f"STA:     {'Connected' if connected else 'Disconnected'}")
    bars = "[#####]" if connected else "[-----]"
    b.text_at(11, 4, f"Signal:  {bars} {'-58 dBm' if connected else 'n/a'}")
    b.text_at(12, 4, f"STA IP:  {'192.168.1.' + str(rng.randint(20, 80)) if connected else '0.0.0.0'}")
    b.text_at(13, 4, "AP  IP:  192.168.4.1")
    b.text_at(14, 4, "Gateway: 192.168.1.1")
    b.text_at(15, 4, "AP:      Enabled")
    b.text_at(16, 4, f"Status:  {status}")
    b.text_at(17, 4, f"SSID:    {ssid}")
    b.text_at(18, 4, "Pass:    ********")
    b.fg("magenta").box(6, 50, 24, 78)
    b.text_at(7, 53, "Actions")
    for i, label in enumerate(MENU):
        row = 9 + i
        mark = ">" if i == selection else " "
        if i == selection:
            b.fg("black").bg("yellow").text_at(row, 52, f"{mark} {label:<22}").reset().bg("black")
        else:
            b.fg("white").text_at(row, 52, f"{mark} {label:<22}")
    if nets:
        sel = nets[0]
        b.fg("white").text_at(20, 52, f"Net 1/{len(nets)} {sel[1]}dBm")
        b.text_at(21, 52, sel[0][:22])
    else:
        b.fg("white").text_at(20, 52, "Net: (none)")
        b.text_at(21, 52, ssid[:22] if ssid else "(scan/select)")
    b.fg("cyan").box(20, 2, 24, 48)
    b.text_at(21, 4, "Controls:")
    b.text_at(22, 4, "Arrows or W/S or J/K + Enter")
    b.text_at(23, 4, "Mouse click when supported by terminal")
    if scanning:
        b.fg("yellow").text_at(25, 4, "Scanning Wi-Fi…")
    return b.build()


def esp_static(rng: random.Random) -> str:
    return _esp_dashboard(rng, selection=0, status="Select SSID then Connect STA")


def esp_motion(rng: random.Random) -> list[str]:
    prefixes = ["Coffee", "Airport", "Maker", "IoT", "Guest", "Lab", "Mesh"]
    networks = [
        (f"{rng.choice(prefixes)}-{rng.randint(100, 999)}", -rng.randint(40, 85))
        for _ in range(5)
    ]
    networks.sort(key=lambda t: t[1], reverse=True)
    pick = networks[0][0]
    frames = [
        _esp_dashboard(rng, selection=0, status="Ready"),
        _esp_dashboard(rng, selection=1, status="Ready")
        + AnsiBuilder().fg("yellow").text_at(25, 4, "Key: ArrowDown → Scan").build(),
        _esp_dashboard(rng, selection=1, status="Scanning...", scanning=True),
        _esp_dashboard(
            rng,
            selection=1,
            status="Scan complete",
            ssid=pick,
            networks=networks,
        ),
        _esp_dashboard(
            rng,
            selection=3,
            status="Selected scanned SSID",
            ssid=pick,
            networks=networks,
        )
        + AnsiBuilder().fg("yellow").text_at(25, 4, "Key: Next Network").build(),
        _esp_dashboard(
            rng,
            selection=5,
            status="Connecting...",
            ssid=pick,
            networks=networks,
        )
        + AnsiBuilder().fg("yellow").text_at(25, 4, "Key: Enter → Connect STA").build(),
        _esp_dashboard(
            rng,
            selection=5,
            status="Connected",
            ssid=pick,
            networks=networks,
            connected=True,
        ),
        _esp_dashboard(
            rng,
            selection=5,
            status="Connected",
            ssid=pick,
            networks=networks,
            connected=True,
        )
        + AnsiBuilder().fg("yellow").text_at(25, 4, "Mouse click Refresh").build(),
        _esp_dashboard(
            rng,
            selection=8,
            status="Refreshed",
            ssid=pick,
            networks=networks,
            connected=True,
        ),
    ]
    return frames


SCENARIOS: dict[str, dict] = {
    "basic": {
        "static": basic_static,
        "motion": basic_motion,
        "cols": 80,
        "rows": 24,
    },
    "advanced": {
        "static": advanced_static,
        "motion": advanced_motion,
        "cols": 80,
        "rows": 28,
    },
    "buttons": {
        "static": buttons_static,
        "motion": buttons_motion,
        "cols": 80,
        "rows": 24,
    },
    "graphic": {
        "static": graphic_static,
        "motion": graphic_motion,
        "cols": 48,
        "rows": 26,
    },
    "ansfile": {
        "static": ansfile_static,
        "motion": ansfile_motion,
        "cols": 80,
        "rows": 28,
    },
    "network": {
        "static": network_static,
        "motion": network_motion,
        "cols": 80,
        "rows": 24,
    },
    "esp": {
        "static": esp_static,
        "motion": esp_motion,
        "cols": 80,
        "rows": 28,
    },
}
