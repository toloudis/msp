"""Tests for imgcompare: per-frame image comparison and the folder comparison."""
import struct
import sys
from pathlib import Path

import numpy as np
import pytest
from PIL import Image

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from imgcompare import Tolerance, compare_dirs, compare_images, diff_image, frame_number, load_image  # noqa: E402

L = 1.0 / 255.0


def flat(value=0.5, shape=(8, 8, 3)):
    return np.full(shape, value, dtype=np.float32)


def write_png(path, array_u8):
    Image.fromarray(array_u8).save(path)


def write_pfm(path, array, little_endian=True):
    h, w, c = array.shape
    header = ("PF" if c == 3 else "Pf") + "\n%d %d\n%s\n" % (w, h, "-1.0" if little_endian else "1.0")
    rows = np.flipud(array).astype("<f4" if little_endian else ">f4")
    with open(path, "wb") as f:
        f.write(header.encode("ascii"))
        f.write(rows.tobytes())


def test_identical_images_pass():
    r = compare_images(flat(), flat(), Tolerance(), "f")
    assert r.passed and r.reason == ""
    assert r.max_err == 0.0 and r.mean_err == 0.0 and r.pct_over == 0.0


def test_one_level_difference_passes():
    r = compare_images(flat(0.5), flat(0.5) + L, Tolerance(), "f")
    assert r.passed
    assert r.max_err == pytest.approx(L, abs=1e-6)
    assert r.pct_over == 0.0


def test_three_levels_fails_max():
    b = flat()
    b[2, 3, 1] += 3 * L
    r = compare_images(flat(), b, Tolerance(), "f")
    assert not r.passed
    assert "max_err" in r.reason


def test_many_pixels_over_fail_pct():
    a = flat(shape=(100, 100, 3))
    b = a.copy()
    b[:1, :, 0] += 2 * L          # 100 of 10000 pixels (1%) off by 2 levels
    r = compare_images(a, b, Tolerance(), "f")
    assert not r.passed
    assert r.pct_over == pytest.approx(1.0)
    assert "pct_over" in r.reason


def test_size_mismatch_fails():
    r = compare_images(flat(shape=(4, 4, 3)), flat(shape=(4, 5, 3)), Tolerance(), "f")
    assert not r.passed
    assert "size" in r.reason


def test_nan_fails():
    b = flat()
    b[0, 0, 0] = np.nan
    r = compare_images(flat(), b, Tolerance(), "f")
    assert not r.passed
    assert "non-finite" in r.reason


def test_alpha_ignored_unless_both_have_it():
    rgb = flat()
    rgba = np.concatenate([flat(), flat(0.0, (8, 8, 1))], axis=2)
    assert compare_images(rgb, rgba, Tolerance(), "f").passed
    rgba2 = np.concatenate([flat(), flat(1.0, (8, 8, 1))], axis=2)
    assert not compare_images(rgba, rgba2, Tolerance(), "f").passed


def test_png_8bit_loads_normalized(tmp_path):
    p = tmp_path / "a.png"
    write_png(p, np.full((2, 3, 3), 255, dtype=np.uint8))
    img = load_image(p)
    assert img.shape == (2, 3, 3) and img.dtype == np.float32
    assert img.max() == pytest.approx(1.0)


def test_png_16bit_loads_normalized(tmp_path):
    p = tmp_path / "a.png"
    Image.fromarray(np.full((2, 3), 65535, dtype=np.uint16)).save(p)
    img = load_image(p)
    assert img.max() == pytest.approx(1.0)


def test_pfm_roundtrip(tmp_path):
    p = tmp_path / "a.pfm"
    data = np.arange(2 * 3 * 3, dtype=np.float32).reshape(2, 3, 3) / 10.0
    write_pfm(p, data)
    img = load_image(p)
    assert np.array_equal(img, data)          # top row first


def test_pfm_big_endian(tmp_path):
    p = tmp_path / "a.pfm"
    data = np.arange(2 * 2 * 3, dtype=np.float32).reshape(2, 2, 3)
    write_pfm(p, data, little_endian=False)
    assert np.array_equal(load_image(p), data)


def test_compare_dirs_reports_missing(tmp_path):
    a, b = tmp_path / "a", tmp_path / "b"
    a.mkdir(); b.mkdir()
    img = np.full((4, 4, 3), 128, dtype=np.uint8)
    for d in (a, b):
        write_png(d / "f1.png", img)
    write_png(a / "f2.png", img)
    results = compare_dirs(a, b, Tolerance(), None)
    by_name = {r.name: r for r in results}
    assert set(by_name) == {"f1.png", "f2.png"}
    assert by_name["f1.png"].passed
    assert not by_name["f2.png"].passed and by_name["f2.png"].reason == "missing in B"


def test_compare_dirs_is_case_insensitive_on_extension(tmp_path):
    a, b = tmp_path / "a", tmp_path / "b"
    a.mkdir(); b.mkdir()
    img = np.full((4, 4, 3), 128, dtype=np.uint8)
    write_png(a / "F_0001.PNG", img)
    write_png(b / "F_0001.PNG", img)
    results = compare_dirs(a, b, Tolerance(), None)
    assert len(results) == 1 and results[0].passed


def test_frame_number_is_the_trailing_digits():
    assert frame_number("Scene_CAM01_Master_BTY_0296.PNG") == 296
    assert frame_number("f1.png") == 1
    assert frame_number("beauty.png") is None


def test_compare_dirs_frame_window_ignores_frames_outside(tmp_path):
    # A is a full render (frames 1-5), B a short one (2-3): only 2-3 are compared
    a, b = tmp_path / "a", tmp_path / "b"
    a.mkdir(); b.mkdir()
    img = np.full((4, 4, 3), 128, dtype=np.uint8)
    for n in range(1, 6):
        write_png(a / f"f_{n:04d}.png", img)
    for n in (2, 3):
        write_png(b / f"f_{n:04d}.png", img)
    results = compare_dirs(a, b, Tolerance(), None, frames=(2, 3))
    assert [r.name for r in results] == ["f_0002.png", "f_0003.png"]
    assert all(r.passed for r in results)


def test_compare_dirs_frame_window_still_fails_missing_frames(tmp_path):
    a, b = tmp_path / "a", tmp_path / "b"
    a.mkdir(); b.mkdir()
    img = np.full((4, 4, 3), 128, dtype=np.uint8)
    for n in range(1, 6):
        write_png(a / f"f_{n:04d}.png", img)
    write_png(b / "f_0002.png", img)
    results = compare_dirs(a, b, Tolerance(), None, frames=(2, None))
    by_name = {r.name: r for r in results}
    assert set(by_name) == {"f_0002.png", "f_0003.png", "f_0004.png", "f_0005.png"}
    assert by_name["f_0003.png"].reason == "missing in B"


def test_compare_dirs_writes_diff_for_failures_only(tmp_path):
    a, b, d = tmp_path / "a", tmp_path / "b", tmp_path / "diff"
    a.mkdir(); b.mkdir()
    img = np.full((4, 4, 3), 128, dtype=np.uint8)
    bad = img.copy(); bad[0, 0, 0] = 200
    write_png(a / "ok.png", img); write_png(b / "ok.png", img)
    write_png(a / "bad.png", img); write_png(b / "bad.png", bad)
    compare_dirs(a, b, Tolerance(), d)
    assert sorted(p.name for p in d.iterdir()) == ["bad.png"]


def test_diff_image_scales_to_white():
    a = flat()
    b = a.copy(); b[1, 1, 2] += 8 * L       # 4 * max_err (2/255) and more
    img = diff_image(a, b)
    assert img.dtype == np.uint8 and img.shape == (8, 8, 3)
    assert tuple(img[1, 1]) == (255, 255, 255)
    assert tuple(img[0, 0]) == (0, 0, 0)


@pytest.mark.parametrize("ext", [".bmp", ".jpg", ".tif", ".tiff"])
def test_compare_dirs_reads_capture_formats(tmp_path, ext):
    # MachStudio's -capFormat BMP, JPG and TIF frames
    a, b = tmp_path / "a", tmp_path / "b"
    a.mkdir(); b.mkdir()
    img = np.full((4, 4, 3), 128, dtype=np.uint8)
    write_png(a / f"f_0001{ext}", img)
    write_png(b / f"f_0001{ext}", img)
    results = compare_dirs(a, b, Tolerance(), None)
    assert len(results) == 1 and results[0].passed
    assert load_image(a / f"f_0001{ext}").shape == (4, 4, 3)
