"""Tests for timing: reading g3dProfiler CSVs and comparing two runs."""
import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from timing import compare_timing, load_csv, summarize  # noqa: E402

HEADER = "backend,frame,sim_time,path,depth,cpu_ms,gpu_ms,draws\n"


def write_csv(path, rows):
    path.write_text(HEADER + "".join(r + "\n" for r in rows))
    return path


def test_load_csv_reads_rows(tmp_path):
    rows = load_csv(write_csv(tmp_path / "t.csv", ["dx11,0,1.0000,Target,0,2.5000,1.2500,7"]))
    assert rows == [{"backend": "dx11", "frame": "0", "sim_time": "1.0000", "path": "Target",
                     "depth": "0", "cpu_ms": "2.5000", "gpu_ms": "1.2500", "draws": "7"}]


def test_summarize_medians(tmp_path):
    rows = load_csv(write_csv(tmp_path / "t.csv", [
        "dx11,0,0.0000,Target,0,1.0000,1.0000,3",
        "dx11,1,0.0000,Target,0,2.0000,4.0000,5",
        "dx11,2,0.0000,Target,0,9.0000,2.0000,4",
    ]))
    s = summarize(rows)
    assert s["Target"]["cpu_median"] == pytest.approx(2.0)
    assert s["Target"]["gpu_median"] == pytest.approx(2.0)
    assert s["Target"]["draws_median"] == pytest.approx(4.0)
    assert s["Target"]["count"] == 3


def test_summarize_empty_gpu_is_none(tmp_path):
    rows = load_csv(write_csv(tmp_path / "t.csv", ["dx11,0,0.0000,Target,0,1.0000,,0"]))
    assert summarize(rows)["Target"]["gpu_median"] is None


def test_compare_timing_ratio():
    a = {"Target": {"cpu_median": 1.0, "gpu_median": 2.0, "draws_median": 10, "count": 3}}
    b = {"Target": {"cpu_median": 2.0, "gpu_median": 3.0, "draws_median": 10, "count": 3},
         "Target/New": {"cpu_median": 1.0, "gpu_median": 1.0, "draws_median": 1, "count": 3}}
    rows = {r["path"]: r for r in compare_timing(a, b)}
    assert rows["Target"]["gpu_ratio"] == pytest.approx(1.5)
    assert rows["Target"]["cpu_ratio"] == pytest.approx(2.0)
    assert rows["Target/New"]["gpu_ratio"] is None
    assert rows["Target/New"]["a_gpu"] is None


def test_compare_timing_zero_baseline_has_no_ratio():
    a = {"P": {"cpu_median": 0.0, "gpu_median": 0.0, "draws_median": 0, "count": 1}}
    b = {"P": {"cpu_median": 1.0, "gpu_median": 1.0, "draws_median": 0, "count": 1}}
    row = compare_timing(a, b)[0]
    assert row["cpu_ratio"] is None and row["gpu_ratio"] is None
