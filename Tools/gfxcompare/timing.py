"""Read g3dProfiler CSVs (-gfxProfile) and compare two runs per scope path.

CSV columns: backend,frame,sim_time,path,depth,cpu_ms,gpu_ms,draws
(gpu_ms is empty when the GPU time is unknown).
"""
from __future__ import annotations

import csv
import statistics
from pathlib import Path


def load_csv(path: Path) -> list[dict]:
    with open(path, newline="") as f:
        return list(csv.DictReader(f))


def _median(values: list[float]) -> float | None:
    return statistics.median(values) if values else None


def summarize(rows: list[dict]) -> dict[str, dict]:
    """path -> cpu_median, gpu_median (None if no GPU values), draws_median, count."""
    by_path: dict[str, dict[str, list[float]]] = {}
    for row in rows:
        entry = by_path.setdefault(row["path"], {"cpu": [], "gpu": [], "draws": []})
        entry["cpu"].append(float(row["cpu_ms"]))
        if row["gpu_ms"]:
            entry["gpu"].append(float(row["gpu_ms"]))
        entry["draws"].append(float(row["draws"]))
    return {path: {"cpu_median": _median(v["cpu"]), "gpu_median": _median(v["gpu"]),
                   "draws_median": _median(v["draws"]), "count": len(v["cpu"])}
            for path, v in by_path.items()}


def _ratio(a: float | None, b: float | None) -> float | None:
    if a is None or b is None or a == 0:
        return None
    return b / a


def compare_timing(a: dict, b: dict) -> list[dict]:
    """One row per path in either summary: medians from A and B and the B/A ratios."""
    rows = []
    for path in sorted(set(a) | set(b)):
        sa, sb = a.get(path, {}), b.get(path, {})
        row = {"path": path,
               "a_cpu": sa.get("cpu_median"), "b_cpu": sb.get("cpu_median"),
               "a_gpu": sa.get("gpu_median"), "b_gpu": sb.get("gpu_median"),
               "a_draws": sa.get("draws_median"), "b_draws": sb.get("draws_median")}
        row["cpu_ratio"] = _ratio(row["a_cpu"], row["b_cpu"])
        row["gpu_ratio"] = _ratio(row["a_gpu"], row["b_gpu"])
        rows.append(row)
    return rows
