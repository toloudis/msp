"""Tests for the gfxcompare command line (compare and run)."""
import sys
from pathlib import Path

import numpy as np
from PIL import Image

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import gfxcompare  # noqa: E402


def make_dirs(tmp_path, differ=False):
    a, b = tmp_path / "a", tmp_path / "b"
    a.mkdir(); b.mkdir()
    img = np.full((8, 8, 3), 100, dtype=np.uint8)
    for name in ("f_0001.PNG", "f_0002.PNG"):
        Image.fromarray(img).save(a / name)
        other = img.copy()
        if differ and name == "f_0002.PNG":
            other[0, 0, 0] = 160
        Image.fromarray(other).save(b / name)
    return a, b


def test_cli_compare_identical_exits_0(tmp_path):
    a, b = make_dirs(tmp_path)
    out = tmp_path / "out"
    assert gfxcompare.main(["compare", str(a), str(b), "--out", str(out)]) == 0
    assert (out / "report.md").exists()


def test_cli_compare_difference_exits_1_and_lists_failure_first(tmp_path):
    a, b = make_dirs(tmp_path, differ=True)
    out = tmp_path / "out"
    assert gfxcompare.main(["compare", str(a), str(b), "--out", str(out)]) == 1
    report = (out / "report.md").read_text()
    assert report.index("f_0002.PNG") < report.index("f_0001.PNG")
    assert (out / "diff" / "f_0002.png").exists()
    assert "2.00/255" in report            # the tolerance used is stated


def test_cli_compare_tolerance_flags(tmp_path):
    a, b = make_dirs(tmp_path, differ=True)
    out = tmp_path / "out"
    assert gfxcompare.main(["compare", str(a), str(b), "--out", str(out),
                            "--max-err", "80", "--pixel-err", "80"]) == 0


def test_cli_compare_empty_folders_fail(tmp_path):
    a, b = tmp_path / "a", tmp_path / "b"
    a.mkdir(); b.mkdir()
    assert gfxcompare.main(["compare", str(a), str(b), "--out", str(tmp_path / "out")]) == 1


def test_cli_compare_includes_timing_when_present(tmp_path):
    a, b = make_dirs(tmp_path)
    header = "backend,frame,sim_time,path,depth,cpu_ms,gpu_ms,draws\n"
    (a / "timing.csv").write_text(header + "dx11,0,0.0000,Target,0,2.0000,4.0000,10\n")
    (b / "timing.csv").write_text(header + "vulkan,0,0.0000,Target,0,1.0000,2.0000,10\n")
    out = tmp_path / "out"
    assert gfxcompare.main(["compare", str(a), str(b), "--out", str(out)]) == 0
    report = (out / "report.md").read_text()
    assert "| Target |" in report and "0.50" in report


def test_cli_run_requires_scene(monkeypatch, tmp_path, capsys):
    monkeypatch.delenv("MSP_GFX_TEST_SCENE", raising=False)
    code = gfxcompare.main(["run", "--exe", "rn.exe", "--out", str(tmp_path)])
    assert code == 2
    assert "scene" in capsys.readouterr().err.lower()
