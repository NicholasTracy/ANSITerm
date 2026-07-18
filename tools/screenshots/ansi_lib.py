"""Minimal ANSI helpers mirroring ANSITerm output conventions."""

from __future__ import annotations

# Box drawing (UTF-8), same glyphs as src/constants.h
H = "\u2500"
V = "\u2502"
TL = "\u250C"
TR = "\u2510"
BL = "\u2514"
BR = "\u2518"
DH = "\u2550"
DV = "\u2551"
DTL = "\u2554"
DTR = "\u2557"
DBL = "\u255A"
DBR = "\u255D"
CROSS = "\u253C"
T_TOP = "\u252C"
T_BOT = "\u2534"
T_LEFT = "\u251C"
T_RIGHT = "\u2524"

# Common CSS-ish names → xterm-256 (close to ANSITerm cube quantization)
COLORS = {
    "black": 0,
    "white": 15,
    "red": 196,
    "green": 46,
    "yellow": 226,
    "blue": 21,
    "magenta": 201,
    "cyan": 51,
    "orange": 208,
    "gray": 244,
}


class AnsiBuilder:
    def __init__(self) -> None:
        self._chunks: list[str] = []

    def raw(self, s: str) -> "AnsiBuilder":
        self._chunks.append(s)
        return self

    def clear(self) -> "AnsiBuilder":
        return self.raw("\033[2J")

    def hide_cursor(self) -> "AnsiBuilder":
        return self.raw("\033[?25l")

    def show_cursor(self) -> "AnsiBuilder":
        return self.raw("\033[?25h")

    def reset(self) -> "AnsiBuilder":
        return self.raw("\033[0m")

    def cup(self, row: int, col: int) -> "AnsiBuilder":
        return self.raw(f"\033[{row};{col}H")

    def fg(self, name_or_index: str | int) -> "AnsiBuilder":
        idx = COLORS[name_or_index] if isinstance(name_or_index, str) else name_or_index
        return self.raw(f"\033[38;5;{idx}m")

    def bg(self, name_or_index: str | int) -> "AnsiBuilder":
        idx = COLORS[name_or_index] if isinstance(name_or_index, str) else name_or_index
        return self.raw(f"\033[48;5;{idx}m")

    def text_at(self, row: int, col: int, text: str) -> "AnsiBuilder":
        return self.cup(row, col).raw(text)

    def box(
        self,
        r0: int,
        c0: int,
        r1: int,
        c1: int,
        *,
        double: bool = False,
    ) -> "AnsiBuilder":
        h, v, tl, tr, bl, br = (DH, DV, DTL, DTR, DBL, DBR) if double else (H, V, TL, TR, BL, BR)
        self.cup(r0, c0).raw(tl + h * (c1 - c0 - 1) + tr)
        for r in range(r0 + 1, r1):
            self.cup(r, c0).raw(v)
            self.cup(r, c1).raw(v)
        self.cup(r1, c0).raw(bl + h * (c1 - c0 - 1) + br)
        return self

    def button(
        self,
        r0: int,
        c0: int,
        r1: int,
        c1: int,
        label: str,
        *,
        double: bool = False,
        selected: bool = False,
    ) -> "AnsiBuilder":
        if selected:
            self.fg("black").bg("yellow")
        self.box(r0, c0, r1, c1, double=double or selected)
        width = c1 - c0 - 1
        text = label if len(label) <= width else label[:width]
        pad = max(0, (width - len(text)) // 2)
        row = r0 + (r1 - r0) // 2
        self.text_at(row, c0 + 1 + pad, text)
        self.reset()
        if selected:
            self.bg("black").fg("white")
        return self

    def table(self, r0: int, c0: int, r1: int, c1: int, rows: int, cols: int) -> "AnsiBuilder":
        self.box(r0, c0, r1, c1)
        row_span = r1 - r0
        col_span = c1 - c0
        rh = max(1, row_span // rows)
        cw = max(1, col_span // cols)
        for j in range(1, cols):
            vc = c0 + j * cw
            self.cup(r0, vc).raw(T_TOP)
            self.cup(r1, vc).raw(T_BOT)
            for r in range(r0 + 1, r1):
                if r % rh == 0 and r != r0:
                    continue
                self.cup(r, vc).raw(V)
        for i in range(1, rows):
            hr = r0 + i * rh
            self.cup(hr, c0).raw(T_LEFT)
            self.cup(hr, c1).raw(T_RIGHT)
            for c in range(c0 + 1, c1):
                if (c - c0) % cw == 0 and c != c0:
                    self.cup(hr, c).raw(CROSS)
                else:
                    self.cup(hr, c).raw(H)
        return self

    def build(self) -> str:
        return "".join(self._chunks)


def sgr_mouse_press(row: int, col: int, button: int = 0) -> str:
    """SGR mouse press sequence (what a supporting terminal would send *to* the MCU).
    For demos we only need a visual click cue on-screen; keep for documentation fixtures.
    """
    return f"\033[<{button};{col};{row}M"


def click_marker(b: AnsiBuilder, row: int, col: int) -> AnsiBuilder:
    """Draw a temporary click highlight at a cell."""
    return b.fg("black").bg("white").text_at(row, col, " ").reset()
