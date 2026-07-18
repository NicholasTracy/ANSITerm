"""Render ANSI byte streams to PNG / GIF via pyte + Pillow."""

from __future__ import annotations

from pathlib import Path

import pyte
from PIL import Image, ImageDraw, ImageFont

# xterm-256 approximate palette (standard + cube + grayscale)
def _xterm_palette() -> list[tuple[int, int, int]]:
    pal: list[tuple[int, int, int]] = [
        (0, 0, 0),
        (205, 0, 0),
        (0, 205, 0),
        (205, 205, 0),
        (0, 0, 238),
        (205, 0, 205),
        (0, 205, 205),
        (229, 229, 229),
        (127, 127, 127),
        (255, 0, 0),
        (0, 255, 0),
        (255, 255, 0),
        (92, 92, 255),
        (255, 0, 255),
        (0, 255, 255),
        (255, 255, 255),
    ]
    for r in range(6):
        for g in range(6):
            for b in range(6):
                pal.append(
                    (
                        55 + r * 40 if r else 0,
                        55 + g * 40 if g else 0,
                        55 + b * 40 if b else 0,
                    )
                )
    for i in range(24):
        v = 8 + i * 10
        pal.append((v, v, v))
    return pal


PALETTE = _xterm_palette()


def _parse_color(value, default: tuple[int, int, int]) -> tuple[int, int, int]:
    if value is None or value == "default":
        return default
    if isinstance(value, int) and 0 <= value < len(PALETTE):
        return PALETTE[value]
    if isinstance(value, str):
        # pyte 0.8+ often stores 256-color / truecolor as RRGGBB hex
        hexrgb = value.removeprefix("#")
        if len(hexrgb) == 6 and all(c in "0123456789abcdefABCDEF" for c in hexrgb):
            return (int(hexrgb[0:2], 16), int(hexrgb[2:4], 16), int(hexrgb[4:6], 16))
        if value.isdigit():
            idx = int(value)
            if 0 <= idx < len(PALETTE):
                return PALETTE[idx]
        named = {
            "black": (0, 0, 0),
            "red": (205, 0, 0),
            "green": (0, 205, 0),
            "brown": (205, 205, 0),
            "yellow": (255, 255, 0),
            "blue": (0, 0, 238),
            "magenta": (205, 0, 205),
            "cyan": (0, 205, 205),
            "white": (229, 229, 229),
        }
        if value in named:
            return named[value]
    return default


_BUNDLED_FONT = Path(__file__).resolve().parent / "fonts" / "DejaVuSansMono.ttf"


def _font(size: int) -> ImageFont.ImageFont:
    # Prefer the vendored TTF so PNG/GIF bytes match across Windows/macOS/Linux CI.
    candidates = [
        str(_BUNDLED_FONT),
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf",
        "C:/Windows/Fonts/consola.ttf",
        "C:/Windows/Fonts/cascadiamono.ttf",
        "/System/Library/Fonts/Menlo.ttc",
    ]
    for path in candidates:
        if Path(path).exists():
            try:
                return ImageFont.truetype(path, size=size)
            except OSError:
                continue
    return ImageFont.load_default()


def apply_ansi(screen: pyte.Screen, stream: pyte.Stream, ansi: str) -> None:
    stream.feed(ansi)


def make_screen(cols: int, rows: int) -> tuple[pyte.Screen, pyte.Stream]:
    screen = pyte.Screen(cols, rows)
    stream = pyte.Stream(screen)
    return screen, stream


def screen_to_image(
    screen: pyte.Screen,
    *,
    cell_w: int = 10,
    cell_h: int = 18,
    margin: int = 12,
    bg_default: tuple[int, int, int] = (12, 12, 18),
    fg_default: tuple[int, int, int] = (220, 220, 220),
) -> Image.Image:
    cols, rows = screen.columns, screen.lines
    img = Image.new(
        "RGB",
        (cols * cell_w + margin * 2, rows * cell_h + margin * 2),
        bg_default,
    )
    draw = ImageDraw.Draw(img)
    font = _font(max(12, cell_h - 4))

    for y in range(rows):
        for x in range(cols):
            cell = screen.buffer[y][x]
            char = cell.data or " "
            fg = _parse_color(cell.fg, fg_default)
            bg = _parse_color(cell.bg, bg_default)
            if cell.reverse:
                fg, bg = bg, fg
            x0 = margin + x * cell_w
            y0 = margin + y * cell_h
            draw.rectangle([x0, y0, x0 + cell_w, y0 + cell_h], fill=bg)
            if char != " ":
                draw.text((x0 + 1, y0 + 1), char, fill=fg, font=font)
    return img


def render_png(ansi: str, cols: int, rows: int, out: Path, **kwargs) -> None:
    screen, stream = make_screen(cols, rows)
    apply_ansi(screen, stream, ansi)
    img = screen_to_image(screen, **kwargs)
    out.parent.mkdir(parents=True, exist_ok=True)
    img.save(out, format="PNG", optimize=True)


TimedFrame = tuple[str, int]  # (ansi, duration_ms)


def pace_frames(
    frames_ansi: list[str] | list[TimedFrame],
    *,
    step_ms: int = 380,
    action_ms: int = 900,
    result_ms: int = 1300,
    end_hold_ms: int = 3200,
) -> list[TimedFrame]:
    """Slow motion GIFs: linger on cues/results and pause before looping."""
    if not frames_ansi:
        return []
    # Already timed
    if isinstance(frames_ansi[0], tuple):
        timed = [(a, int(d)) for a, d in frames_ansi]  # type: ignore[misc]
        ansi_only = [a for a, _ in timed]
    else:
        ansi_only = list(frames_ansi)  # type: ignore[arg-type]
        def _is_cue(text: str) -> bool:
            lower = text.lower()
            # Cue overlays from scenarios (avoid matching ordinary UI arrows).
            return (
                "key:" in lower
                or "mouse →" in lower
                or "mouse click" in lower
                or "sgr mouse" in lower
                or "space →" in lower
            )

        timed = []
        for i, ansi in enumerate(ansi_only):
            is_cue = _is_cue(ansi)
            prev_cue = i > 0 and _is_cue(ansi_only[i - 1])
            next_cue = i + 1 < len(ansi_only) and _is_cue(ansi_only[i + 1])
            if is_cue:
                timed.append((ansi, action_ms))
            elif prev_cue or next_cue:
                # Pause to read the screen before/after an emulated action.
                timed.append((ansi, result_ms))
            else:
                timed.append((ansi, step_ms))

    # Extra readable pause on the final frame before the loop restarts.
    last_ansi, last_ms = timed[-1]
    timed[-1] = (last_ansi, max(last_ms, end_hold_ms))
    return timed


def render_gif(
    frames_ansi: list[str] | list[TimedFrame],
    cols: int,
    rows: int,
    out: Path,
    *,
    duration_ms: int | None = None,
    end_hold_ms: int = 3200,
    **kwargs,
) -> None:
    """Each entry is a *full screen redraw* ANSI string (clear + draw)."""
    timed = pace_frames(frames_ansi, end_hold_ms=end_hold_ms)
    if duration_ms is not None:
        timed = [(ansi, duration_ms) for ansi, _ in timed]
        timed[-1] = (timed[-1][0], max(duration_ms, end_hold_ms))

    images: list[Image.Image] = []
    durations: list[int] = []
    for ansi, ms in timed:
        screen, stream = make_screen(cols, rows)
        apply_ansi(screen, stream, ansi)
        images.append(screen_to_image(screen, **kwargs))
        durations.append(ms)
    out.parent.mkdir(parents=True, exist_ok=True)
    if not images:
        raise ValueError("no frames")
    images[0].save(
        out,
        format="GIF",
        save_all=True,
        append_images=images[1:],
        duration=durations,
        loop=0,
        optimize=True,
    )
