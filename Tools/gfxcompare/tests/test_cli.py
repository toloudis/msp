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


def test_cli_compare_frame_window(tmp_path):
    # f_0001 only in A: outside the window it is ignored, inside it fails
    a, b = make_dirs(tmp_path)
    (b / "f_0001.PNG").unlink()
    out = tmp_path / "out"
    assert gfxcompare.main(["compare", str(a), str(b), "--out", str(out), "--start-frame", "2"]) == 0
    assert "frames 2 to the end" in (out / "report.md").read_text()
    assert gfxcompare.main(["compare", str(a), str(b), "--out", str(out)]) == 1


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


FAKE_RENDERNODE = """
import pathlib, sys
import numpy as np
from PIL import Image
args = sys.argv[1:]
out = pathlib.Path(args[args.index("-capOutputPath") + 1])
first = int(args[args.index("-capStartFrame") + 1]) if "-capStartFrame" in args else 1
last = int(args[args.index("-capEndFrame") + 1]) if "-capEndFrame" in args else 3
for n in range(first, last + 1):
    Image.fromarray(np.full((8, 8, 3), 100, dtype=np.uint8)).save(out / f"f_{n:04d}.PNG")
for a in args:
    if a.startswith("say="):
        sys.stderr.write(a[4:].replace("_", " ") + "\\n")
"""


def run_fake(tmp_path, a_says, b_says, extra=()):
    script = tmp_path / "fake_rendernode.py"
    script.write_text(FAKE_RENDERNODE)
    scene = tmp_path / "scene.mab"
    scene.write_text("")
    def args(says):
        return " ".join([str(script)] + ["say=" + s.replace(" ", "_") for s in says])
    out = tmp_path / "out"
    code = gfxcompare.main(["run", "--exe", sys.executable, "--scene", str(scene), "--out", str(out),
                            "--a-args", args(a_says), "--b-args", args(b_says)] + list(extra))
    return code, (out / "report.md").read_text()


def test_cli_run_passes_and_reports_backends(tmp_path):
    code, report = run_fake(tmp_path, ["Graphics backend: dx11 on GPU X"],
                            ["Graphics backend: vulkan on GPU X", "Vulkan validation: 0 errors, 0 warnings"])
    assert code == 0
    assert "dx11 on GPU X" in report and "vulkan on GPU X" in report


def test_cli_run_frame_window_renders_only_those_frames(tmp_path):
    code, report = run_fake(tmp_path, [], [], extra=["--start-frame", "2", "--end-frame", "2"])
    assert code == 0
    assert sorted(p.name for p in (tmp_path / "out" / "a").glob("*.PNG")) == ["f_0002.PNG"]
    assert "frames 2 to 2" in report


def test_cli_run_fails_on_validation_messages(tmp_path):
    code, report = run_fake(tmp_path, ["Graphics backend: dx11 on GPU X"],
                            ["Graphics backend: vulkan on GPU X", "Vulkan validation: 0 errors, 2 warnings"])
    assert code == 1
    assert "FAIL" in report and "2 warnings" in report


def test_cli_run_fails_on_different_adapters(tmp_path):
    code, report = run_fake(tmp_path, ["Graphics backend: dx11 on GPU X"], ["Graphics backend: vulkan on GPU Y"])
    assert code == 1
    assert "FAIL" in report and "adapter" in report.lower()


def test_cli_run_reports_vulkan_validation_off(tmp_path):
    code, report = run_fake(tmp_path, ["Graphics backend: dx11 on GPU X"],
                            ["Graphics backend: vulkan on GPU X", "Vulkan validation: off"])
    assert code == 0
    assert "B Vulkan validation: off" in report
