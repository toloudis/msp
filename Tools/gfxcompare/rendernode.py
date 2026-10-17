"""Run MachStudio's RenderNode build to render a scene's frames.

The capture settings go after -render: -render loads the scene, and loading
replaces the capture settings with the scene's own.
"""
from __future__ import annotations

import subprocess
from pathlib import Path

from imgcompare import IMAGE_SUFFIXES


def build_command(exe: Path, scene: Path, out_dir: Path, profile_csv: Path,
                  extra_args: list[str], fmt: str = "PNG",
                  width: int | None = None, height: int | None = None) -> list[str]:
    cmd = [str(exe)] + list(extra_args)
    cmd += ["-gfxProfile", str(profile_csv)]
    cmd += ["-render", str(scene)]
    cmd += ["-capFormat", fmt, "-capOutputPath", str(out_dir)]
    if width is not None:
        cmd += ["-capWidth", str(width)]
    if height is not None:
        cmd += ["-capHeight", str(height)]
    return cmd


def run(cmd: list[str], timeout_s: float, out_dir: Path) -> None:
    """Runs cmd; raises RuntimeError if it fails or writes no images to out_dir."""
    proc = subprocess.run(cmd, capture_output=True, text=True, errors="replace", timeout=timeout_s)
    if proc.returncode != 0:
        tail = (proc.stderr or proc.stdout or "")[-2000:]
        raise RuntimeError(f"{Path(cmd[0]).name} failed with exit code {proc.returncode}: {tail}")
    images = [p for p in Path(out_dir).iterdir() if p.suffix.lower() in IMAGE_SUFFIXES]
    if not images:
        raise RuntimeError(f"{Path(cmd[0]).name} wrote no images to {out_dir}")
