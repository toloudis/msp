"""Render a scene twice with MachStudio's RenderNode and compare the frames and timings.

  gfxcompare.py run --exe EXE [--scene MAB] --out DIR [--a-args "..."] [--b-args "..."]
                    [--start-frame N] [--end-frame N]
  gfxcompare.py compare DIR_A DIR_B --out DIR [--start-frame N] [--end-frame N]

Both write DIR/report.md (failing frames first, difference images in
DIR/diff/, per-scope timing medians with B/A ratios) and exit 0 only if
every frame is within tolerance. run also fails if either render reports
Vulkan validation messages, or if A and B ran on different adapters.
See README.md.
"""
from __future__ import annotations

import argparse
import os
import shlex
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

import rendernode  # noqa: E402
from imgcompare import FrameResult, Tolerance, compare_dirs  # noqa: E402
from timing import compare_timing, load_csv, summarize  # noqa: E402

TIMING_CSV = "timing.csv"


def _fmt(v, spec=".3f"):
    return "-" if v is None else format(v, spec)


def _frame_table(results: list[FrameResult]) -> list[str]:
    lines = ["| frame | result | max err (/255) | mean err (/255) | % over | reason |",
             "|---|---|---|---|---|---|"]
    for r in sorted(results, key=lambda r: (r.passed, r.name)):
        lines.append(f"| {r.name} | {'pass' if r.passed else '**FAIL**'} | {r.max_err * 255:.2f} | "
                     f"{r.mean_err * 255:.3f} | {r.pct_over:.3f} | {r.reason} |")
    return lines


def _timing_table(dir_a: Path, dir_b: Path) -> list[str]:
    csv_a, csv_b = dir_a / TIMING_CSV, dir_b / TIMING_CSV
    if not (csv_a.exists() and csv_b.exists()):
        return ["No timing: run with a profiling build (MSP_GFX_PROFILING) to get timing.csv in both folders."]
    rows = compare_timing(summarize(load_csv(csv_a)), summarize(load_csv(csv_b)))
    lines = ["Medians per scope path, in ms; ratio is B/A (below 1 means B is faster).", "",
             "| path | A cpu | B cpu | cpu B/A | A gpu | B gpu | gpu B/A | A draws | B draws |",
             "|---|---|---|---|---|---|---|---|---|"]
    for r in rows:
        lines.append(f"| {r['path']} | {_fmt(r['a_cpu'])} | {_fmt(r['b_cpu'])} | {_fmt(r['cpu_ratio'], '.2f')} | "
                     f"{_fmt(r['a_gpu'])} | {_fmt(r['b_gpu'])} | {_fmt(r['gpu_ratio'], '.2f')} | "
                     f"{_fmt(r['a_draws'], '.0f')} | {_fmt(r['b_draws'], '.0f')} |")
    return lines


def _window_text(frames) -> str:
    first, last = frames
    return f"frames {'the first' if first is None else first} to {'the end' if last is None else last}"


def compare(dir_a: Path, dir_b: Path, out: Path, tol: Tolerance, header: list[str] | None = None,
            problems: list[str] | None = None, frames=None) -> int:
    """problems: reasons the run fails besides the frames (validation messages, adapters).
    frames: (first, last) frame numbers to compare, None for all."""
    out.mkdir(parents=True, exist_ok=True)
    results = compare_dirs(dir_a, dir_b, tol, out / "diff", frames)
    failed = [r for r in results if not r.passed]
    problems = problems or []
    ok = bool(results) and not failed and not problems
    lines = ["# gfxcompare report", ""] + (header or []) + [
        f"- A: `{dir_a}`", f"- B: `{dir_b}`",
        f"- Tolerance: max error {tol.max_err * 255:.2f}/255; at most {tol.max_pct_over}% of pixels "
        f"over {tol.pixel_err * 255:.2f}/255",
        f"- Result: **{'PASS' if ok else 'FAIL'}** ({len(results) - len(failed)} of {len(results)} frames within tolerance)"]
    if frames is not None:
        lines.append(f"- Compared: {_window_text(frames)} (others ignored)")
    lines += [f"- **FAIL:** {p}" for p in problems]
    lines += ["", "## Frames", ""]
    lines += _frame_table(results) if results else ["No frames found."]
    lines += ["", "## Timing", ""] + _timing_table(dir_a, dir_b) + [""]
    (out / "report.md").write_text("\n".join(lines), encoding="utf-8")
    print(f"{'PASS' if ok else 'FAIL'}: {len(results) - len(failed)}/{len(results)} frames within tolerance; "
          f"report: {out / 'report.md'}")
    for p in problems:
        print(f"FAIL: {p}")
    return 0 if ok else 1


def _frames(args):
    return None if args.start_frame is None and args.end_frame is None else (args.start_frame, args.end_frame)


def _tolerance(args) -> Tolerance:
    return Tolerance(max_err=args.max_err / 255.0, pixel_err=args.pixel_err / 255.0, max_pct_over=args.max_pct_over)


def _parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(prog="gfxcompare", description=__doc__.split("\n\n")[0])
    sub = p.add_subparsers(dest="command", required=True)

    def tolerance_args(sp):
        d = Tolerance()
        sp.add_argument("--max-err", type=float, default=d.max_err * 255, help="largest allowed channel error, in 1/255 units")
        sp.add_argument("--pixel-err", type=float, default=d.pixel_err * 255, help="per-pixel error counted as 'over', in 1/255 units")
        sp.add_argument("--max-pct-over", type=float, default=d.max_pct_over, help="allowed percent of pixels over --pixel-err")

    def frame_args(sp, what):
        sp.add_argument("--start-frame", type=int, help=f"first frame to {what} (absolute frame number)")
        sp.add_argument("--end-frame", type=int, help=f"last frame to {what} (included)")

    r = sub.add_parser("run", help="render with RenderNode twice (A, B) and compare")
    r.add_argument("--exe", required=True, type=Path, help="RenderNode executable")
    r.add_argument("--scene", type=Path, default=None, help="scene (.mab); default $MSP_GFX_TEST_SCENE")
    r.add_argument("--out", required=True, type=Path)
    r.add_argument("--a-args", default="", help="extra RenderNode arguments for run A")
    r.add_argument("--b-args", default="", help="extra RenderNode arguments for run B")
    r.add_argument("--cap-args", default="",
                   help="extra capture settings for both runs (-capSampling, -capPasses, ...), put after -render")
    r.add_argument("--format", default="PNG", choices=["PNG", "PFM", "BMP", "JPG", "TIF"])
    r.add_argument("--width", type=int)
    r.add_argument("--height", type=int)
    r.add_argument("--timeout", type=float, default=7200, help="seconds per render")
    frame_args(r, "render and compare")
    tolerance_args(r)

    c = sub.add_parser("compare", help="compare two folders of frames")
    c.add_argument("dir_a", type=Path)
    c.add_argument("dir_b", type=Path)
    c.add_argument("--out", required=True, type=Path)
    frame_args(c, "compare; frames outside are ignored")
    tolerance_args(c)
    return p


def main(argv: list[str] | None = None) -> int:
    args = _parser().parse_args(argv)
    tol = _tolerance(args)
    if args.command == "compare":
        return compare(args.dir_a, args.dir_b, args.out, tol, frames=_frames(args))

    scene = args.scene or (Path(os.environ["MSP_GFX_TEST_SCENE"]) if os.environ.get("MSP_GFX_TEST_SCENE") else None)
    if scene is None:
        print("gfxcompare run: no scene; pass --scene or set MSP_GFX_TEST_SCENE", file=sys.stderr)
        return 2
    header = [f"- Scene: `{scene}`", f"- RenderNode: `{args.exe}`",
              f"- A arguments: `{args.a_args}`", f"- B arguments: `{args.b_args}`"]
    if args.cap_args:
        header.append(f"- Capture arguments: `{args.cap_args}`")
    problems, adapters = [], {}
    for label, extra in (("a", args.a_args), ("b", args.b_args)):
        out_dir = (args.out / label).resolve()
        out_dir.mkdir(parents=True, exist_ok=True)
        for old in out_dir.iterdir():
            if old.is_file():
                old.unlink()
        cmd = rendernode.build_command(args.exe.resolve(), scene, out_dir, out_dir / TIMING_CSV,
                                       shlex.split(extra, posix=False), args.format, args.width, args.height,
                                       args.start_frame, args.end_frame, shlex.split(args.cap_args, posix=False))
        print(f"rendering {label.upper()}: {' '.join(cmd)}")
        try:
            stderr = rendernode.run(cmd, args.timeout, out_dir)
        except (RuntimeError, OSError) as e:
            print(f"gfxcompare run: render {label.upper()} failed: {e}", file=sys.stderr)
            return 2
        L = label.upper()
        backend = rendernode.backend_adapter(stderr)
        adapters[label] = backend[1] if backend else None
        header.append(f"- {L} backend: {f'{backend[0]} on {backend[1]}' if backend else 'unknown'}")
        counts = rendernode.validation_counts(stderr)
        if "Vulkan validation: off" in stderr:
            header.append(f"- {L} Vulkan validation: off (run with -gfxValidation to check it)")
        if counts is not None:
            header.append(f"- {L} Vulkan validation: {counts[0]} errors, {counts[1]} warnings")
            if counts != (0, 0):
                problems.append(f"render {L} reported Vulkan validation messages "
                                f"({counts[0]} errors, {counts[1]} warnings)")
    if adapters["a"] and adapters["b"] and not rendernode.same_adapter(adapters["a"], adapters["b"]):
        problems.append(f"A and B ran on different adapters ({adapters['a']} / {adapters['b']})")
    return compare(args.out / "a", args.out / "b", args.out, tol, header, problems, _frames(args))


if __name__ == "__main__":
    sys.exit(main())
