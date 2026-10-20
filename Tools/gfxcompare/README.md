# gfxcompare

Renders a scene twice with MachStudio's headless RenderNode build and
compares the two sets of frames and the per-pass timings. Used to show that
a change to the renderer (later: the Vulkan backend against DX11) produces
the same images, and how fast.

## Setup

Python 3.10 or later:

```
python -m venv .venv
.venv\Scripts\python -m pip install -r Tools/gfxcompare/requirements.txt
```

For timings, build RenderNode with profiling markers:

```
msbuild MachStudio/Source/MachStudio.sln /t:MainApp "/p:Configuration=Release RenderNode" /p:Platform=x64 /p:MspGfxProfiling=true
```

(Debug builds always have them.)

## Commands

Render twice and compare:

```
python Tools/gfxcompare/gfxcompare.py run --exe MachStudio/MachStudio-x64/MachStudio-x64-RenderNode-Release.exe ^
    --scene C:\path\scene.mab --out out\cmp [--a-args "..."] [--b-args "..."]
```

- `--scene` defaults to the `MSP_GFX_TEST_SCENE` environment variable.
- `--a-args` / `--b-args` are extra RenderNode arguments for each run (for
  example `-gfx dx11` and `-gfx vulkan`).
- `run` reads each render's stderr: the report shows the backend and
  adapter (`Graphics backend: <name> on <adapter>`) and, for Vulkan, the
  validation counts (`Vulkan validation: N errors, M warnings`). It fails
  (exit 1) if either render reports any validation message, or if A and B
  ran on different adapters (names are compared without a leading vendor
  name: DXGI says "NVIDIA Quadro RTX 5000" where Vulkan says "Quadro RTX
  5000"). Release builds validate only with
  `-gfxValidation`; without it RenderNode prints `Vulkan validation: off`
  and the report says so.
- `--format PNG|PFM`, `--width`, `--height` set the capture; otherwise the
  scene's own capture settings are used. The frames go to `out\cmp\a` and
  `out\cmp\b`, each with its `timing.csv`.
- `--start-frame N` / `--end-frame N` render only those frames (absolute
  frame numbers, as in the file names; both included), through RenderNode's
  `-capStartFrame` / `-capEndFrame`, and compare only those. A short run is
  much quicker than the scene's whole range, and frames keep their numbers,
  so they can be compared with a full render (below). A window that starts
  after the scene's first frame also renders the frame before it (not
  saved), so motion-dependent effects match a full render.

Compare two folders of frames that already exist:

```
python Tools/gfxcompare/gfxcompare.py compare DIR_A DIR_B --out out\cmp [--start-frame N] [--end-frame N]
```

With `--start-frame` / `--end-frame`, frames outside the window are ignored
(a frame inside it missing from either side still fails), so a short run can
be compared with a reference that has the whole sequence.

Both write `report.md` (failing frames first, difference images in
`diff/`, timing medians per scope with B/A ratios) and exit 0 only if every
frame is within tolerance (1 if not, 2 if a render failed or no scene was
given).

## Tolerance

A frame passes when its largest per-channel error is at most `--max-err`
(default 2, in 1/255 units) and at most `--max-pct-over` percent (default
0.1) of its pixels have an error above `--pixel-err` (default 1/255).
Frames are matched by file name; a frame missing from either side fails.
A frame's number is the digits at the end of its file name.
Difference images are white at 8/255 error.

## Timing CSV

RenderNode's `-gfxProfile <file.csv>` writes one row per profiler scope per
frame:

`backend,frame,sim_time,path,depth,cpu_ms,gpu_ms,draws`

`path` joins scope names from the frame root (`Target/HDRRenderer/PassLit`);
times and draw counts include child scopes; `gpu_ms` is empty when the GPU
time is unknown. A capture with accumulation or motion blur renders several
profiler frames per output frame.

## Tests

```
python -m pytest Tools/gfxcompare/tests -q
```
