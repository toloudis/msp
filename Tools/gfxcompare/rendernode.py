"""Run MachStudio's RenderNode build to render a scene's frames.

The capture settings go after -render: -render loads the scene, and loading
replaces the capture settings with the scene's own.

RenderNode prints "Graphics backend: <name> on <adapter>" at startup and,
for Vulkan, "Vulkan validation: N errors, M warnings" at exit, both on
stderr; run returns stderr so callers can check them.
"""
from __future__ import annotations

import re
import subprocess
from pathlib import Path

from imgcompare import IMAGE_SUFFIXES


def build_command(exe: Path, scene: Path, out_dir: Path, profile_csv: Path,
                  extra_args: list[str], fmt: str = "PNG",
                  width: int | None = None, height: int | None = None,
                  start_frame: int | None = None, end_frame: int | None = None) -> list[str]:
    cmd = [str(exe)] + list(extra_args)
    cmd += ["-gfxProfile", str(profile_csv)]
    cmd += ["-render", str(scene)]
    cmd += ["-capFormat", fmt, "-capOutputPath", str(out_dir)]
    if width is not None:
        cmd += ["-capWidth", str(width)]
    if height is not None:
        cmd += ["-capHeight", str(height)]
    if start_frame is not None:
        cmd += ["-capStartFrame", str(start_frame)]
    if end_frame is not None:
        cmd += ["-capEndFrame", str(end_frame)]
    return cmd


def run(cmd: list[str], timeout_s: float, out_dir: Path) -> str:
    """Runs cmd and returns its stderr; raises RuntimeError if it fails or writes no images to out_dir."""
    proc = subprocess.run(cmd, capture_output=True, text=True, errors="replace", timeout=timeout_s)
    if proc.returncode != 0:
        tail = (proc.stderr or proc.stdout or "")[-2000:]
        raise RuntimeError(f"{Path(cmd[0]).name} failed with exit code {proc.returncode}: {tail}")
    images = [p for p in Path(out_dir).iterdir() if p.suffix.lower() in IMAGE_SUFFIXES]
    if not images:
        raise RuntimeError(f"{Path(cmd[0]).name} wrote no images to {out_dir}")
    return proc.stderr or ""


_VALIDATION = re.compile(r"Vulkan validation: (?P<errors>\d+) errors, (?P<warnings>\d+) warnings")
_BACKEND = re.compile(r"^Graphics backend: (?P<name>\S+) on (?P<adapter>.+?)\s*$", re.MULTILINE)


def validation_counts(stderr: str) -> tuple[int, int] | None:
    """(errors, warnings) from the Vulkan validation line, or None if there is none (DX11)."""
    m = _VALIDATION.search(stderr)
    return (int(m["errors"]), int(m["warnings"])) if m else None


def backend_adapter(stderr: str) -> tuple[str, str] | None:
    """(backend, adapter) from the "Graphics backend" line, or None if there is none."""
    m = _BACKEND.search(stderr)
    return (m["name"], m["adapter"]) if m else None


_VENDOR = re.compile(r"^(NVIDIA|AMD|ATI|Intel\(R\)|Intel)\s+", re.IGNORECASE)


def same_adapter(a: str, b: str) -> bool:
    """Whether two adapter names are the same GPU: DXGI names start with the
    vendor ("NVIDIA Quadro RTX 5000"), Vulkan's may not ("Quadro RTX 5000")."""
    def bare(name: str) -> str:
        return _VENDOR.sub("", name.strip()).casefold()
    return bare(a) == bare(b)
