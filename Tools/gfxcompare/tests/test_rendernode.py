"""Tests for rendernode: the RenderNode command line and running it."""
import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from rendernode import build_command, run  # noqa: E402


def test_build_command_order():
    cmd = build_command(Path("rn.exe"), Path("C:/s/scene.mab"), Path("C:/out"), Path("C:/out/timing.csv"),
                        ["-gfx", "vulkan"], fmt="PNG", width=640, height=360)
    assert cmd[0] == "rn.exe"
    # extra args first, then profiling, then -render, and the capture settings
    # after -render (loading the scene would overwrite them otherwise)
    assert cmd[1:3] == ["-gfx", "vulkan"]
    assert cmd[3:5] == ["-gfxProfile", str(Path("C:/out/timing.csv"))]
    assert cmd[5:7] == ["-render", str(Path("C:/s/scene.mab"))]
    rest = cmd[7:]
    assert rest[rest.index("-capFormat") + 1] == "PNG"
    assert rest[rest.index("-capOutputPath") + 1] == str(Path("C:/out"))
    assert rest[rest.index("-capWidth") + 1] == "640"
    assert rest[rest.index("-capHeight") + 1] == "360"


def test_build_command_without_size_has_no_size_flags():
    cmd = build_command(Path("rn.exe"), Path("s.mab"), Path("o"), Path("o/t.csv"), [])
    assert "-capWidth" not in cmd and "-capHeight" not in cmd


def test_run_reports_failure_exit_code(tmp_path):
    cmd = [sys.executable, "-c", "import sys; sys.stderr.write('boom'); sys.exit(3)"]
    with pytest.raises(RuntimeError, match=r"exit code 3.*boom"):
        run(cmd, 60, tmp_path)


def test_run_requires_images(tmp_path):
    with pytest.raises(RuntimeError, match="no images"):
        run([sys.executable, "-c", "pass"], 60, tmp_path)


def test_run_succeeds_when_images_written(tmp_path):
    script = "import pathlib,sys; pathlib.Path(sys.argv[1], 'f_0001.PNG').write_bytes(b'x')"
    run([sys.executable, "-c", script, str(tmp_path)], 60, tmp_path)
