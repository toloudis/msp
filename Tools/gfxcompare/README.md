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
python Tools/gfxcompare/gfxcompare.py run --exe MachStudio/MachStudio-x64-RenderNode-Release/MachStudio-x64-RenderNode-Release.exe ^
    --scene C:\path\scene.mab --out out\cmp [--a-args "..."] [--b-args "..."]
```

- `--scene` defaults to the `MSP_GFX_TEST_SCENE` environment variable.
- `--a-args` / `--b-args` are extra RenderNode arguments for each run (for
  example `-gfx dx11` and `-gfx vulkan`).
- `run` reads each render's stderr: the report shows the backend and
  adapter (`Graphics backend: <name> on <adapter>`) and, for Vulkan, the
  validation counts (`Vulkan validation: N errors, M warnings, P
  best-practices notes`; older builds print no notes). It fails
  (exit 1) if either render reports an error or a warning (best-practices
  notes, from `MSP_GFX_VK_VALIDATION=bestpractices`, are reported but fail
  nothing), or if A and B
  ran on different adapters (names are compared without a leading vendor
  name: DXGI says "NVIDIA Quadro RTX 5000" where Vulkan says "Quadro RTX
  5000"). Release builds validate only with
  `-gfxValidation`; without it RenderNode prints `Vulkan validation: off`
  and the report says so.
- `--format PNG|PFM|BMP|JPG|TIF`, `--width`, `--height` set the capture;
  otherwise the scene's own capture settings are used.
- `--cap-args "..."` adds capture settings to both renders, after `-render`
  (they are scene settings, which loading the scene would replace), for
  example `--cap-args "-capSampling 3 -capJitter gaussian 1.5"` or
  `--cap-args "-capPasses BTY,Z,NOR"` (see MachStudio's `-help`). With
  several passes each frame number has one file per pass; the pass name is
  in the file name, so they are matched by name as usual. The frames go to `out\cmp\a` and
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
This is `--tolerance strict`, for two renders by the same backend.

`--tolerance lod` is for DX11 against Vulkan. The NVIDIA driver computes a
texture's level of detail differently per API where the texture is seen at
an angle (GraphicsTests' `vkdAnisotropicTest`; roadmap, "Open
investigations"). That gives two kinds of difference, and neither is a bug
of ours:
- low-level noise over textured surfaces (about 3% of pixels over 1/255 in
  the test scene's beauty frames);
- isolated large errors where an alpha-tested texture's cutout flips (the
  cobweb: up to 121/255 on a few hundred pixels).

A frame passes `lod` when:
- at most 0.5% of its pixels are over 4/255;
- at most 0.04% of its pixels are over 16/255;
- its mean error is at most 0.6/255;
- there is no limit on the largest error.

The limits were measured on Release RenderNode runs (Plan 7 Task 9: testTESS
beauty, `-capSampling 2`, all 24 GPU passes, and testAO; frames 296-315, the
passes 296-300). The worst frames were:
- 0.35% over 4/255 (DOF, where the blur spreads the noise);
- 0.026% over 16/255 (ILLUM, MTL, REFL: the cutout flips);
- mean 0.38/255 (DOF).

A real difference still fails. The DX11 reflection bug fixed in `6848f8d2`
(frames before and after the fix) gives 4.6% of pixels over 4/255, 4.2%
over 16/255, mean 2.3/255.

Any limit can be changed with a flag given after the preset: `--max-err`,
`--pixel-err`, `--max-pct-over`, `--strong-err`, `--max-pct-strong`,
`--max-mean-err`. The report states the tolerance it used.

Frames are matched by file name; a frame missing from either side fails.
Frames can be PNG (8 or 16 bit), PFM, BMP, JPG or TIFF.
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
