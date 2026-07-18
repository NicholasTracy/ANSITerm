#!/usr/bin/env python3
"""Update Wiki pages with screenshot embeds from the roster (tag-pinned raw URLs)."""

from __future__ import annotations

import argparse
import os
import subprocess
import tempfile
from pathlib import Path

import yaml

ROOT = Path(__file__).resolve().parents[2]
ROSTER = ROOT / "docs" / "screenshots" / "roster.yml"
WIKI_REPO = "https://github.com/NicholasTracy/ANSITerm.wiki.git"


def run(cmd: list[str], cwd: Path | None = None) -> None:
    subprocess.check_call(cmd, cwd=cwd)


def img_md(raw_ref: str, filename: str, alt: str) -> str:
    url = (
        f"https://raw.githubusercontent.com/NicholasTracy/ANSITerm/"
        f"{raw_ref}/docs/screenshots/{filename}"
    )
    return f"![{alt}]({url})"


def section_for_example(raw_ref: str, entry: dict) -> str:
    lines = [
        "",
        f'<a id="{entry["wiki_anchor"]}"></a>',
        f"## Screenshot: {entry['example']}",
        "",
        entry.get("caption", ""),
        "",
        img_md(raw_ref, entry["static"], f"{entry['example']} static"),
        "",
    ]
    if entry.get("motion"):
        lines += [
            "**Interaction preview** (emulated keys / mouse / Wi‑Fi)",
            "",
            img_md(raw_ref, entry["motion"], f"{entry['example']} motion"),
            "",
        ]
    return "\n".join(lines)


def upsert_section(page: Path, marker: str, body: str) -> None:
    start = f"<!-- SCREENSHOTS:{marker}:START -->"
    end = f"<!-- SCREENSHOTS:{marker}:END -->"
    block = f"{start}\n{body.strip()}\n{end}\n"
    if page.exists():
        text = page.read_text(encoding="utf-8")
        if start in text and end in text:
            pre, rest = text.split(start, 1)
            _, post = rest.split(end, 1)
            text = pre + block + post.lstrip("\n")
        else:
            text = text.rstrip() + "\n\n" + block
    else:
        text = f"# {marker}\n\n{block}"
    page.write_text(text, encoding="utf-8")


def build_roster_page(label: str, raw_ref: str, roster: dict) -> str:
    lines = [
        "# Screenshot Roster",
        "",
        f"Labeled for release `{label}`; image files served from `{raw_ref}` "
        f"(see `docs/screenshots/roster.yml` in the main repo).",
        "",
        "## README heroes",
        "",
        "| Anchor | File |",
        "|--------|------|",
    ]
    for h in roster.get("heroes", []):
        lines.append(f"| `#{h['readme_anchor']}` | `{h['file']}` |")
        lines.append("")
        lines.append(img_md(raw_ref, h["file"], h["caption"]))
        lines.append("")
    lines += [
        "## All examples",
        "",
        "| ID | Static | Motion | Wiki anchor |",
        "|----|--------|--------|-------------|",
    ]
    for e in roster["examples"]:
        motion = e.get("motion", "—")
        lines.append(
            f"| `{e['id']}` | `{e['static']}` | `{motion}` | `#{e['wiki_anchor']}` |"
        )
    lines.append("")
    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--tag", required=True, help="Release label shown on Wiki pages")
    parser.add_argument(
        "--raw-ref",
        default="master",
        help="Git ref for raw.githubusercontent.com image URLs (default: master)",
    )
    parser.add_argument("--wiki-dir", type=Path, default=None)
    args = parser.parse_args()

    roster = yaml.safe_load(ROSTER.read_text(encoding="utf-8"))
    token = os.environ.get("GH_TOKEN") or os.environ.get("GITHUB_TOKEN")
    wiki_url = WIKI_REPO
    if token:
        wiki_url = f"https://x-access-token:{token}@github.com/NicholasTracy/ANSITerm.wiki.git"

    if args.wiki_dir:
        wiki_dir = args.wiki_dir
    else:
        tmp = tempfile.mkdtemp(prefix="ansiterm-wiki-")
        wiki_dir = Path(tmp)
        run(["git", "clone", "--depth", "1", wiki_url, str(wiki_dir)])

    for entry in roster["examples"]:
        page = wiki_dir / f"{entry['wiki_page']}.md"
        upsert_section(page, entry["id"], section_for_example(args.raw_ref, entry))

    using = wiki_dir / "Using-the-Examples.md"
    thumb_lines = ["", "| Example | Preview |", "|---------|---------|"]
    for entry in roster["examples"]:
        url = (
            f"https://raw.githubusercontent.com/NicholasTracy/ANSITerm/"
            f"{args.raw_ref}/docs/screenshots/{entry['static']}"
        )
        thumb_lines.append(
            f"| [{entry['example']}]({entry['wiki_page']}#{entry['wiki_anchor']}) | "
            f"![{entry['id']}]({url}) |"
        )
    thumb_lines.append("")
    upsert_section(using, "gallery", "\n".join(thumb_lines))

    roster_page = wiki_dir / "Screenshot-Roster.md"
    roster_page.write_text(
        build_roster_page(args.tag, args.raw_ref, roster), encoding="utf-8"
    )

    # Sidebar link
    sidebar = wiki_dir / "_Sidebar.md"
    if sidebar.exists():
        sb = sidebar.read_text(encoding="utf-8")
        if "Screenshot-Roster" not in sb:
            sb = sb.rstrip() + "\n* [Screenshot Roster](Screenshot-Roster)\n"
            sidebar.write_text(sb, encoding="utf-8")

    run(["git", "config", "user.name", "github-actions[bot]"], cwd=wiki_dir)
    run(
        ["git", "config", "user.email", "41898282+github-actions[bot]@users.noreply.github.com"],
        cwd=wiki_dir,
    )
    run(["git", "add", "-A"], cwd=wiki_dir)
    # commit if needed
    status = subprocess.check_output(["git", "status", "--porcelain"], cwd=wiki_dir, text=True)
    if status.strip():
        run(
            ["git", "commit", "-m", f"Embed screenshots for {args.tag}."],
            cwd=wiki_dir,
        )
        run(["git", "push", "origin", "HEAD"], cwd=wiki_dir)
        print(f"Wiki updated for {args.tag}")
    else:
        print("Wiki already up to date")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
