"""Compare rendered frames: two images, or two folders of frames matched by file name.

Images load as float32 HxWxC: PNG (8 or 16 bit) normalized to [0, 1], PFM as
stored. Errors are absolute per-channel differences over RGB (alpha only when
both images have it). A frame passes when its largest error is within
Tolerance.max_err and at most Tolerance.max_pct_over percent of its pixels
have an error above Tolerance.pixel_err. A frame's number is the digits at
the end of its file name (RenderNode's <0000> tag); a folder comparison can
be limited to a window of frame numbers.
"""
from __future__ import annotations

import re
from dataclasses import dataclass
from pathlib import Path

import numpy as np
from PIL import Image

IMAGE_SUFFIXES = (".png", ".pfm")


@dataclass
class Tolerance:
    max_err: float = 2 / 255
    pixel_err: float = 1 / 255
    max_pct_over: float = 0.1          # percent of pixels


@dataclass
class FrameResult:
    name: str
    passed: bool
    reason: str                         # '' when passed
    max_err: float
    mean_err: float
    pct_over: float


def _load_pfm(path: Path) -> np.ndarray:
    with open(path, "rb") as f:
        kind = f.readline().strip()
        if kind not in (b"PF", b"Pf"):
            raise ValueError(f"{path}: not a PFM file")
        width, height = (int(v) for v in f.readline().split())
        scale = float(f.readline().strip())
        channels = 3 if kind == b"PF" else 1
        data = np.frombuffer(f.read(), dtype="<f4" if scale < 0 else ">f4")
    data = data[: width * height * channels].reshape(height, width, channels)
    return np.flipud(data).astype(np.float32)        # PFM rows are bottom-to-top


def load_image(path: Path) -> np.ndarray:
    path = Path(path)
    if path.suffix.lower() == ".pfm":
        return _load_pfm(path)
    with Image.open(path) as im:
        if im.mode in ("I;16", "I;16B", "I;16L", "I"):
            arr = np.asarray(im, dtype=np.float32) / 65535.0
        else:
            if im.mode not in ("RGB", "RGBA", "L"):
                im = im.convert("RGBA" if "A" in im.getbands() else "RGB")
            arr = np.asarray(im, dtype=np.float32) / 255.0
    if arr.ndim == 2:
        arr = arr[:, :, None]
    return arr


def _channels(a: np.ndarray, b: np.ndarray) -> tuple[np.ndarray, np.ndarray]:
    n = 4 if a.shape[2] >= 4 and b.shape[2] >= 4 else min(3, a.shape[2], b.shape[2])
    return a[:, :, :n], b[:, :, :n]


def _error(a: np.ndarray, b: np.ndarray) -> np.ndarray:
    """Per-pixel largest channel error, HxW."""
    a, b = _channels(a, b)
    return np.abs(a.astype(np.float64) - b.astype(np.float64)).max(axis=2)


def compare_images(a: np.ndarray, b: np.ndarray, tol: Tolerance, name: str) -> FrameResult:
    if a.shape[:2] != b.shape[:2]:
        return FrameResult(name, False, f"size differs: {a.shape[1]}x{a.shape[0]} vs {b.shape[1]}x{b.shape[0]}",
                           float("nan"), float("nan"), float("nan"))
    ca, cb = _channels(a, b)
    if not (np.isfinite(ca).all() and np.isfinite(cb).all()):
        return FrameResult(name, False, "non-finite values (NaN/Inf) in image",
                           float("nan"), float("nan"), float("nan"))
    err = _error(a, b)
    max_err = float(err.max())
    mean_err = float(err.mean())
    pct_over = float((err > tol.pixel_err + 1e-9).mean() * 100.0)
    reasons = []
    if max_err > tol.max_err + 1e-9:
        reasons.append(f"max_err {max_err * 255:.2f}/255 > {tol.max_err * 255:.2f}/255")
    if pct_over > tol.max_pct_over:
        reasons.append(f"pct_over {pct_over:.3f}% > {tol.max_pct_over}%")
    return FrameResult(name, not reasons, "; ".join(reasons), max_err, mean_err, pct_over)


def diff_image(a: np.ndarray, b: np.ndarray) -> np.ndarray:
    """Heat map of the per-pixel largest channel error; white at 4 * the default max_err (8/255)."""
    err = _error(a, b)
    scale = 4 * Tolerance().max_err
    v = np.clip(err / scale, 0.0, 1.0)
    return np.repeat((v * 255.0 + 0.5).astype(np.uint8)[:, :, None], 3, axis=2)


_FRAME_NUMBER = re.compile(r"(\d+)$")

FrameWindow = tuple[int | None, int | None]     # (first, last), both included; None: no limit


def frame_number(name: str) -> int | None:
    """The digits at the end of the file name's stem, or None."""
    m = _FRAME_NUMBER.search(Path(name).stem)
    return int(m[1]) if m else None


def _in_window(name: str, frames: FrameWindow | None) -> bool:
    n = frame_number(name)
    if frames is None or n is None:
        return True                     # a name without a number is always compared
    first, last = frames
    return (first is None or n >= first) and (last is None or n <= last)


def _frames(folder: Path, frames: FrameWindow | None) -> dict[str, Path]:
    return {p.name: p for p in Path(folder).iterdir()
            if p.is_file() and p.suffix.lower() in IMAGE_SUFFIXES and _in_window(p.name, frames)}


def compare_dirs(dir_a: Path, dir_b: Path, tol: Tolerance, diff_dir: Path | None,
                 frames: FrameWindow | None = None) -> list[FrameResult]:
    """frames: only frames in this window are compared (one missing from either side still fails)."""
    frames_a, frames_b = _frames(dir_a, frames), _frames(dir_b, frames)
    results = []
    for name in sorted(set(frames_a) | set(frames_b)):
        if name not in frames_b:
            results.append(FrameResult(name, False, "missing in B", float("nan"), float("nan"), float("nan")))
            continue
        if name not in frames_a:
            results.append(FrameResult(name, False, "missing in A", float("nan"), float("nan"), float("nan")))
            continue
        a, b = load_image(frames_a[name]), load_image(frames_b[name])
        result = compare_images(a, b, tol, name)
        results.append(result)
        if diff_dir is not None and not result.passed and a.shape[:2] == b.shape[:2]:
            Path(diff_dir).mkdir(parents=True, exist_ok=True)
            Image.fromarray(diff_image(a, b)).save(Path(diff_dir) / (Path(name).stem + ".png"))
    return results
