"""Tests for rendernode: the RenderNode command line and running it."""
import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from rendernode import backend_adapter, build_command, run, same_adapter, validation_counts  # noqa: E402


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


def test_build_command_frame_window_after_render():
    cmd = build_command(Path("rn.exe"), Path("s.mab"), Path("o"), Path("o/t.csv"), [],
                        start_frame=296, end_frame=315)
    render = cmd.index("-render")
    assert cmd.index("-capStartFrame") > render and cmd[cmd.index("-capStartFrame") + 1] == "296"
    assert cmd.index("-capEndFrame") > render and cmd[cmd.index("-capEndFrame") + 1] == "315"


def test_build_command_without_frame_window_has_no_frame_flags():
    cmd = build_command(Path("rn.exe"), Path("s.mab"), Path("o"), Path("o/t.csv"), [])
    assert "-capStartFrame" not in cmd and "-capEndFrame" not in cmd


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


def test_run_returns_stderr(tmp_path):
    script = ("import pathlib,sys; pathlib.Path(sys.argv[1], 'f_0001.PNG').write_bytes(b'x'); "
              "sys.stderr.write('hello')")
    assert "hello" in run([sys.executable, "-c", script, str(tmp_path)], 60, tmp_path)


def test_validation_counts_parsed():
    err = "noise\nVulkan validation: 3 errors, 12 warnings\nmore\n"
    assert validation_counts(err) == (3, 12)


def test_validation_counts_absent_for_dx11():
    assert validation_counts("Graphics backend: dx11 on NVIDIA Quadro RTX 5000\n") is None


def test_backend_adapter_parsed():
    err = "Graphics backend: vulkan on NVIDIA Quadro RTX 5000\r\nother\n"
    assert backend_adapter(err) == ("vulkan", "NVIDIA Quadro RTX 5000")
    assert backend_adapter("nothing here") is None


def test_same_adapter_ignores_vendor_name():
    assert same_adapter("NVIDIA Quadro RTX 5000", "Quadro RTX 5000")
    assert same_adapter("Quadro RTX 5000", "nvidia quadro rtx 5000")
    assert same_adapter("Intel(R) UHD Graphics P630", "Intel(R) UHD Graphics P630")
    assert same_adapter("AMD Radeon RX 7900 XTX", "Radeon RX 7900 XTX")
    assert not same_adapter("NVIDIA Quadro RTX 5000", "Intel(R) UHD Graphics P630")
    assert not same_adapter("NVIDIA Quadro RTX 5000", "NVIDIA Quadro RTX 4000")


def test_build_command_capture_args_go_last():
    # extra capture settings (-capSampling, -capPasses, ...) are scene
    # settings: they go after -render, with the others
    cmd = build_command(Path("rn.exe"), Path("s.mab"), Path("o"), Path("o/t.csv"), ["-gfx", "vulkan"],
                        start_frame=296, end_frame=300, capture_args=["-capSampling", "2", "-capPasses", "BTY,Z"])
    render = cmd.index("-render")
    assert cmd.index("-capSampling") > render and cmd[cmd.index("-capSampling") + 1] == "2"
    assert cmd[-4:] == ["-capSampling", "2", "-capPasses", "BTY,Z"]
    assert cmd[1:3] == ["-gfx", "vulkan"]


def test_build_command_without_capture_args_is_unchanged():
    assert build_command(Path("rn.exe"), Path("s.mab"), Path("o"), Path("o/t.csv"), []) == \
        build_command(Path("rn.exe"), Path("s.mab"), Path("o"), Path("o/t.csv"), [], capture_args=[])
