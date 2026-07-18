# Example screenshot tooling

Generates PNG/GIF previews of ANSITerm example UIs for `docs/screenshots/`.

## Why host-rendered?

GitHub Actions cannot drive a physical board + terminal. Scenarios in `scenarios.py` emit the same ANSI sequences the sketches draw, including **emulated interactions** (arrow keys, Enter, mouse click cues, Wi‑Fi scan/connect with random SSIDs) for motion GIFs.

## Commands

```bash
pip install -r tools/screenshots/requirements.txt
python tools/screenshots/generate.py          # write docs/screenshots/*
python tools/screenshots/generate.py --check  # local byte compare (may differ from Linux)
python tools/screenshots/sync_wiki.py --tag v0.2.1 --raw-ref master
```

Committed PNG/GIF bytes are produced on **ubuntu-latest** (FreeType differs across OS). Prefer CI goldens over Windows/macOS output when updating docs.

## Roster

[`docs/screenshots/roster.yml`](../../docs/screenshots/roster.yml) is the source of truth for IDs, filenames, README anchors, and Wiki page links.

## CI

[`.github/workflows/example-screenshots.yml`](../../.github/workflows/example-screenshots.yml):

- **PR:** regenerate on Linux; fail if `docs/screenshots` drifts
- **Push / dispatch:** on drift, open a squash-merged PR with Linux goldens
- **Tag `v*`:** regenerate → PR → auto-merge → Wiki sync
