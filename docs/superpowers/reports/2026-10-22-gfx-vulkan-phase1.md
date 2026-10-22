# GraphicsVulkan Phase 1 report

2026-10-22, branch `claude/graphics-vulkan`, written in Plan 7 Task 9.

Everything here was measured on one machine: an NVIDIA Quadro RTX 5000 (driver 581.xx, Vulkan 1.4.329) with the Vulkan SDK 1.4.350 layers. Both backends ran on the same adapter in every run (gfxcompare checks this).

Spec: `docs/superpowers/specs/2026-10-17-graphics-vulkan-design.md` ("Decided" 9 and 10, "Proving parity", "Validation"). Roadmap: `docs/superpowers/plans/2026-10-17-graphics-vulkan-roadmap.md`.

## Summary

| Exit criterion (spec, decision 9) | Result |
|---|---|
| Every renderer's images match within tolerance | **Yes.** Test scene: every frame passes gfxcompare's `lod` tolerance (the API level-of-detail difference, below). GraphicsTests: every renderer, pass and material against DX11, with the exception tables below. Hair renders and matches (no deferral). |
| A timing report for both backends | **Yes**, below. On the test scene's frames Vulkan takes 0.89× DX11's GPU time and 0.92× its frame CPU time. Per pass, Vulkan's CPU cost is 1.4–14× DX11's. |
| Vulkan validation reports nothing | **Yes:** 0 errors and 0 warnings in core + synchronization validation, with GPU-assisted validation, and with best practices (notes only, which fail nothing), in GraphicsTests and in RenderNode. |
| DX11 still works exactly as before | **Yes.** The DX11 test-scene frames match the reference rendered after `6848f8d2` (20/20), and every DX11 run matched itself. The DX11 changes made during the port were owner-approved bug fixes, listed below. |

Vulkan stays non-default until Phase 2 (spec decision 9). It is already as fast on these frames, but the CPU cost per pass is the work Phase 2 is for (see Timing).

## Parity: the test scene

The runs used Release RenderNode, headless, frames 296–315 (the owner's choice for sign-off: the 24-pass runs use 296–300). Each Vulkan run used `-gfxValidation`. Each DX11 run was first compared with a second DX11 run.

### Tolerance

The spec's tolerance, gfxcompare's `--tolerance strict`, allows a largest error of 2/255 and at most 0.1% of pixels over 1/255.

DX11 against Vulkan can't meet it on textured frames. Plan 7 Task 1 showed why (`vkdAnisotropicTest`): on a texture seen at an angle, the NVIDIA driver computes a different level of detail in D3D11 and in Vulkan.
- D3D11's is the textbook isotropic one.
- Vulkan's is 0.04–0.07 of a level higher on average.
- Head-on, the two are identical.

Nothing in our setup changes this: not the driver settings, the viewport's Y flip, the binding path, the sampler, the mips or the UVs. The owner ruled it an API difference, to be allowed by a documented tolerance.

It shows in two ways:
- low-level noise over textured surfaces;
- isolated large errors where an alpha-tested texture's cutout lands on a different mip level (the scene's cobweb).

So gfxcompare gained `--tolerance lod` (`989902c4`, documented in its README). A frame passes when:
- at most 0.5% of its pixels are over 4/255;
- at most 0.04% of its pixels are over 16/255;
- its mean error is at most 0.6/255;
- there is no limit on the largest error.

The limits come from these runs. These were the worst frames of any DX11-vs-Vulkan run:

| measure | worst frame | where | `lod` limit |
|---|---|---|---|
| % of pixels over 4/255 | 0.355% | DOF pass (the blur spreads the noise) | 0.5% |
| % of pixels over 16/255 | 0.026% | ILLUM, MTL, REFL (cutout flips) | 0.04% |
| mean error | 0.38/255 | DOF pass | 0.6/255 |
| largest error | 121/255 | ILLUM, MTL (one cutout flip) | none |

**Checked that it still catches a real bug.** The DX11 reflection bug fixed in `6848f8d2`, compared as DX11 frames before against after, fails all three limits on every frame: 4.6% over 4/255, 4.2% over 16/255, mean 2.3/255.

**What could still pass.** A real regression that changes fewer than about 0.04% of the pixels (about 800 at 1920×1080) and leaves the mean low. GraphicsTests, at the strict tolerance on small scenes, is what covers that.

### Results

| run | DX11 vs DX11 (strict) | DX11 vs Vulkan (strict) | DX11 vs Vulkan (`lod`) | Vulkan validation |
|---|---|---|---|---|
| `testTESS` beauty, 296–315 | 20/20 | 0/20: max 10–31/255, 3.1–3.2% over 1/255 | **20/20** | 0 / 0 |
| `testAO` (DOF + SSAO), 296–315 | 20/20 | 0/20: max 5–8/255, 0.69% | **20/20** | 0 / 0 |
| supersampling 2 (`-capSampling 2 -capJitter off`), 296–315 | 20/20 | 0/20: max 7–19/255, 0.92–1.00% | **20/20** | 0 / 0 |
| all 24 GPU passes (`-capPasses`), 296–300 | 115/120 (GI, below) | 39/120 | **120/120** | 0 / 0 |
| timing runs (no validation), both scenes, twice | | | 80/80 | off |

The DX11 `testTESS` frames match the DX11 reference after `6848f8d2` (strict, 20/20).

Per pass, DX11 vs Vulkan, worst frame of 296–300:

| passes | strict | what differs |
|---|---|---|
| AO, EMI, MAT, SHDW, SPECENV, STAR, VEL, Z | 5/5 (AO 4/5: max 4/255 on one frame) | nothing, or within 1–2/255 |
| BLOOM, DIFFENV, PREV, WIRE | fail, max 1–8/255 | low-level noise, under beauty's |
| BTY, DIFF, DIFFLIT, SPEC, SPECLIT, GLOW | fail, max 13–19/255, up to 3.1% over 1/255 | the level-of-detail noise, as beauty |
| DOF | fail, max 21/255, 6.7% over 1/255 | the same noise, spread by the blur |
| ILLUM, MTL, NOR, REFL | fail, max 69–121/255, up to 0.026% over 16/255 | cutout flips on the cobweb's hard-edged alpha. A soft cutout matches within 1/255 (`CutoutMatchesDX11`). |
| GI | fail, max 46/255, 0.0008% over 16/255 | the noise, plus DX11's own nondeterminism: DX11 differs from itself by 2–6/255 on 0.002% of pixels |

**Not covered by the scenes:** AO volumes, SSGI and LPV (no scene here runs them; `Dume_Demo_stress001_AOtiles` isn't used, at the owner's request), hair, and the opacity maps. GraphicsTests covers them. The `COLBL` pass (RenderMan color bleeding) has no GPU renderer and crashed RenderNode on both backends; `-capPasses` now skips it with a warning (see "Items for the owner").

## Parity: GraphicsTests

`UnitTests/GraphicsTests` renders small scenes through the whole GraphicsDX11 layer and the whole GraphicsVulkan layer (`gtBackend`) and compares them at the spec's tolerance.
- Each parity test also checks on DX11 that its feature changes the image, so a test can't pass against a Vulkan stand-in that draws nothing.
- Totals at Plan 7 Task 8: Debug 923/923, Release 922/922, all six configurations. Validation was 0/0 in each layer run; the only messages were the tests' own deliberate ones.
- A Debug suite after this task's profiler change is in the ledger.
- Since 2026-10-22 CI builds GraphicsTests but no longer runs it (`afc32ed0`): GitHub's runners have no GPU the tests can rely on.

| suite (tests) | covers | exceptions to the strict tolerance |
|---|---|---|
| `shdwMaterialsVkTest` (2, 66 renders) | every material shader, lit spheres, tessellation | **The level-of-detail difference** on hard checkers: max 52/255 on 2.03% of pixels (mean ≤ 0.12/255), allowed 64/255 on 2.5%. SpecularFresnel: 4–5 isolated pixels up to 228/255 at the sphere tops, allowed any max on 2.5%. Skin, BlinnSkin and testtess `tess_on` draw nothing on DX11 or on Vulkan. |
| `shdwHDRRendererVkTest` (14) | HDR renderer, tone map, bloom, star, HDR AA, debug modes, matte, pass buffers, buffer sharing | Debug mode 6 (bright pass): 2/255 on 0.21%. The buffer is quarter size, shown 4× enlarged, so each texel counts 16 pixels. Allowed 0.3%. |
| `shdwPreviewRenderersVkTest` (12) | depth, normal, materials, glow, reflections only, shadow mask, illumination, AO and GI previews, UV, texture, cutouts | AO preview (SSAO): one silhouette pixel, 3/255, the same on every run. |
| `matTextureMgrVkTest` (8) | texture loading, creation, reload, mips | JPEG sources: WIC's and stb_image's decoders differ by up to 2/255 (5/255 read through sRGB). |
| `g2dImageSaveVkTest` (7) | image saving in every format | JPG (two encoders), WIC's approximate sRGB curve in 16-bit PNG/TIFF and 64-bit BMP, and DirectXTex's Software tag. Each is measured and bounded in the file's header. |
| `shdwVelocityVkTest` (2) | velocity map renderer and pass | DX11 writes NaNs on the first velocity frame. The test requires the same NaN texels on Vulkan (an item for the owner). |
| `vkdAnisotropicTest` (4) | raw D3D11 against raw Vulkan, no MachStudio code | Only head-on is asserted (within 1/255). The angled cases print the level-of-detail difference. |
| `hairVkTest` (8), `shdwOpacityVkTest` (3) | hair fragments and renderer at the tessellation limits, odd strand shapes, more than 65,535 control points, morphed hair; the hair pass in every transparency mode; opacity shadow maps | none: hair images exact or within 1/255, opacity maps 0/255 |
| `shdwAoGiVkTest` (7), `shdwDofFogVkTest` (6), `shdwBakeVkTest` (2), `shdwScreenPassesVkTest` (6), `shdwTransparentVkTest` (4), `shdwReflectionVkTest` (4), `shdwShadowVkTest` (3), `g3dPickVkTest` (2), `g3dSceneRendererVkTest` (7) | AO volumes, SSGI, LPV and RSMs, DOF, fog, baking, the screen passes, transparency (sorted, depth-peeled, deferred), reflections, shadows, picking | none |
| `fxParity` (60), `fxEffectVkReflection` (360), `effPlainEffectVkTest` (240), `fxShaderCopy`, `vkd*`, `g2d*` and the rest (≈300) | effects and reflection, the shader copy, the `vkd` layer, windows, fonts, capture | none |

## Validation (Plan 7 Task 8)

| run | core + sync | `MSP_GFX_VK_VALIDATION=gpu` | `bestpractices` |
|---|---|---|---|
| GraphicsTests Debug, whole suite | 0 / 0 | 0 / 0 | 0 / 0, 1842 notes |
| RenderNode Debug, `testTESS` 296–300 | 0 / 0, no asserts | 0 / 0 | 0 / 0, 34 notes |
| RenderNode Debug, 24 passes, 296–300 | 0 / 0 | – | 0 / 0, 34 notes |
| RenderNode Release `-gfxValidation`, every run above | 0 / 0 | – | – |

There is one exception, in GPU-assisted validation: its vertex-fetch bounds check is off. In SDK 1.4.350 it checks every indexed draw against the last draw's vertex buffer, a false positive. `vkdBufferGpuAvTest` is the minimal repro.

Best-practices notes are inputs to Phase 2. They don't fail anything. By message ID:

| ID | count in GraphicsTests / RenderNode | asks for |
|---|---|---|
| `NVIDIA-AllocateMemory-SetPriority` | 1110 / 10 | a memory priority per allocation (render targets highest) |
| `Pipeline-SortAndBind` [AMD] | 792 / 10 | sort draws by pipeline; the same pipeline is bound more than once a frame |
| `NVIDIA-CreateDevice-PageableDeviceLocalMemory` | 228 / 1 | enable `VK_EXT_pageable_device_local_memory` |
| `NVIDIA-BindPipeline-SwitchTessGeometryMesh` | 42 / 3 | group tessellation and geometry-shader draws |
| `Error-Result` | 1 / 10 | GraphicsTests: a deliberate corrupt cache file. RenderNode: `vkAllocateMemory` returns `VK_ERROR_OUT_OF_DEVICE_MEMORY` while the scene's textures load; the frames are correct (probably VMA trying a full heap and falling back; not investigated). |
| `NVIDIA-CreatePipelineLayout-SeparateSampler`, `vkCreatePipelines-multiple-pipelines-caches`, `SyncObjects-HighNumberOfSemaphores`, `NVIDIA-AllocateMemory-ReuseAllocations`, `NVIDIA-Zcull-LessGreaterRatio`, `Failure-Result` | 1–8 each / 0 | combined image samplers; one pipeline cache; fewer semaphores; reuse freed allocations; one depth test direction per depth target; a handled `OUT_OF_DATE` present |

The layer stops repeating an ID after 10 per instance, so RenderNode's counts saturate.

## Timing

The timing runs used Release RenderNode built with `MspGfxProfiling=true` and no validation, with the machine otherwise idle. Each scene ran on each backend twice, frames 296–315.
- For each scope, each run gives the median over its frames; the tables show the mean of the two runs' medians.
- Times are in ms per output frame.
- The two runs' GPU times agreed within 1–9% per scope (one scope: 12%; `testAO`'s tiny transparent pass: 34%), so a ratio within about ±5% of 1 is noise.
- **Draws** count the same points on both backends: bump, hair, font, full-screen quad and LPV draws. This task fixed Vulkan counting every draw on top of those (`81661fe6`), which had made its column read 2×. The column is therefore DX11's count of these draws, not every draw call.
- **The frame's CPU time** (`Target`, about 270–290 ms) is mostly outside any pass: the passes' own CPU time (the "pass CPU" sum) is 5–16 ms. The rest is time the profiler doesn't break down, such as waiting for the GPU and the capture's own work, and is about the same on both backends.

### `testTESS`

| scope | DX11 CPU | Vulkan CPU | CPU Vk/DX11 | DX11 GPU | Vulkan GPU | GPU Vk/DX11 | draws |
|---|---|---|---|---|---|---|---|
| **frame** (`Target`) | 288.4 | 266.0 | **0.92** | 349.9 | 312.9 | **0.89** | 1753 |
| HDR renderer | 287.6 | 263.0 | 0.91 | 345.2 | 310.6 | 0.90 | 1598 |
| SSAO | 1.59 | 3.12 | 1.96 | 252.4 | 224.0 | 0.89 | 706 |
| &nbsp;&nbsp;its depth pass | 0.33 | 0.62 | 1.89 | 4.96 | 2.57 | 0.52 | 140 |
| Glow | 0.31 | 1.01 | 3.30 | 53.0 | 48.1 | 0.91 | 80 |
| AlphaFill | 0.45 | 4.27 | **9.4** | 9.78 | 8.60 | 0.88 | 148 |
| Depth | 0.29 | 0.58 | 2.00 | 7.54 | 6.00 | 0.80 | 140 |
| Environment | 0.53 | 0.94 | 1.76 | 6.67 | 5.32 | 0.80 | 140 |
| Lit | 0.55 | 1.03 | 1.90 | 6.39 | 4.60 | 0.72 | 181 |
| ZFill | 0.32 | 1.48 | **4.65** | 5.74 | 3.80 | 0.66 | 140 |
| Transparent | 0.11 | 0.17 | 1.60 | 0.07 | 0.10 | 1.39 | 30 |
| PlanarReflection | 0.78 | 2.94 | **3.8** | 4.62 | 2.23 | 0.48 | 155 |
| &nbsp;&nbsp;its ZFill | 0.14 | 1.95 | **13.9** | 1.10 | 0.31 | 0.29 | 43 |
| &nbsp;&nbsp;its Lit | 0.23 | 0.39 | 1.67 | 1.74 | 0.76 | 0.44 | 63 |
| &nbsp;&nbsp;its Environment | 0.25 | 0.38 | 1.49 | 1.61 | 0.86 | 0.53 | 43 |
| pass CPU, sum of the rows above but the nested ones | 4.9 | 15.5 | 3.2 | | | | |

### `testAO`

| scope | DX11 CPU | Vulkan CPU | CPU Vk/DX11 | DX11 GPU | Vulkan GPU | GPU Vk/DX11 | draws |
|---|---|---|---|---|---|---|---|
| **frame** (`Target`) | 288.6 | 265.6 | **0.92** | 358.2 | 320.6 | **0.90** | 1672 |
| HDR renderer | 287.9 | 262.8 | 0.91 | 348.2 | 314.4 | 0.90 | 1562 |
| SSAO | 1.78 | 3.02 | 1.69 | 249.8 | 221.8 | 0.89 | 706 |
| Glow | 0.14 | 0.46 | 3.38 | 49.7 | 46.0 | 0.93 | 42 |
| AlphaFill | 0.54 | 3.93 | **7.2** | 10.5 | 9.15 | 0.87 | 148 |
| Depth | 0.33 | 0.56 | 1.71 | 9.08 | 7.23 | 0.80 | 140 |
| Environment | 0.61 | 0.95 | 1.56 | 7.46 | 5.98 | 0.80 | 140 |
| DOF | 0.64 | 1.16 | 1.81 | 7.14 | 5.94 | 0.83 | 146 |
| ZFill | 0.35 | 1.46 | **4.23** | 6.09 | 4.18 | 0.69 | 140 |
| Lit | 0.17 | 0.24 | 1.40 | 0.25 | 0.22 | 0.91 | 41 |
| PlanarReflection | 0.68 | 2.65 | **3.9** | 9.63 | 6.06 | 0.63 | 110 |
| &nbsp;&nbsp;its ZFill | 0.16 | 1.94 | **12.4** | 4.46 | 2.50 | 0.56 | 43 |

The full tables, with every scope, are in each run's `report.md` (gfxcompare).

### What the numbers say

- **GPU: Vulkan is faster in every large pass** (0.48–0.93×) and slower only in the tiny transparent pass (0.04–0.10 ms). SSAO alone is 72% of the frame on both backends, the same shader on both, so the frame ratio (0.89–0.90) mostly reflects SSAO's 0.89.
- **CPU: Vulkan spends 3.2× DX11's CPU time in the passes** (15.5 against 4.9 ms on `testTESS`). On these frames that is small next to the frame's 310–360 ms of GPU time. Two hot spots, as in 5a and 5b:
  - **The planar reflection:** 3.8–3.9× in all, and its ZFill 12–14× (43 draws: 1.95 ms on Vulkan against 0.14–0.16 ms on DX11).
  - **AlphaFill:** 7–9× (148 draws: about 4 ms against 0.5 ms). It sets its effect's transparency state per node.
  
  The pattern is per-draw CPU cost on Vulkan: setting effect state, writing the binding table and constants into the ring, and pipeline lookups. It grows with how often a pass changes state between draws.

### Phase 2, ranked by these numbers

The spec's "Phase 2 direction" items, in the order the numbers suggest:

1. **Per-object draw packets built once and replayed per pass**, together with **persistent descriptors and constant data** that change only when the object does. These go after the per-draw CPU cost that the ZFill (4–14×) and AlphaFill (7–9×) ratios measure, and they matter in every pass. Best practices' `Pipeline-SortAndBind` points the same way (sort draws by pipeline).
2. **Recording reflection and shadow passes on several threads.** The planar reflection is the largest single CPU cost on Vulkan (2.7–2.9 ms). Item 1 shrinks it first; threading is worth measuring after that.
3. **Less render-target round-tripping between `shdw` passes.** Vulkan is already faster on the GPU, so this has the least evidence behind it on these scenes.

Not in the spec's list, but suggested by best practices: memory priorities, and `VK_EXT_pageable_device_local_memory`. The `OUT_OF_DEVICE_MEMORY` results while textures load are worth a look before larger scenes are tried.

On these scenes the GPU time (mostly SSAO) is far larger than the passes' CPU time, so CPU work won't change their frame time much. A lighter scene, or the interactive viewport, is where the CPU numbers would show.

## What isn't ported, and why

**Unreachable on DX11, so not ported** (Plan 5a decision 3 and Plan 6 decision 1; each confirmed by a grep):

| DX11 code | why nothing reaches it |
|---|---|
| `rtUtil`, `rasterizerUtil`, `shdwRayTraceRenderer`, `shdwTestRendererDX11`, `shdwSinglePassRendererDX11` | no creator returns the renderers (commented out in `CreateDefaultRenderer`); the utilities serve only them |
| `effShaderArray::GetDefaultEffect`, `GetDefaultLightingEffect`, `GetMapNormalsToScreen` | used only by `rasterizerUtil` |
| `shdwPassMapNormalsToScreen` | never constructed |
| `g3dVSMRendererDX11` | `USE_VSM_SHADOWS` is off |
| `g3dCubeMapRendererDX11` | only the base creator returns it, and `shdw`'s creator replaces that |
| the motion-blur branch of the HDR renderer | `ENABLE_MOTIONBLUR` is off (`shdwPassVelocity` itself is ported, with the velocity map renderer) |
| `shdwPassGIVolumes`, the GI preview's `ENABLE_GI_VOLUME` branch | `ENABLE_GI_VOLUME` is off |
| `shdwAOVolumesRendererDX11` | nothing creates it |
| the 256×256×4 opacity volume | made into file-local pointers that nothing reads |

**Left as `GVK_NOT_YET`** (logged once if reached):
- **The demo watermark** (`matTextureLoader::LoadResource`, Windows resources). Only the licensing code's demo mode loads it.
- **Instance data with a step rate above 1** (`vkdPipelineCache`). No vertex layout in MachStudio uses one.

**Hair is not deferred.** The hull shader's `ProcessIsolineTessFactors` is the one piece of Vulkan-only shader code (`Eff/private/ShadersVk/`, 6 lines: fxc's own lowering for `fractional_odd`). It matches D3D11 at the factors' limits (`vkdIsolineTest`, `hairVkTest`).

**Behaviour kept as DX11's, not fixed:**
- Morphed hair isn't re-uploaded (DX11's buffers are immutable).
- The simple renderer draws no hair.
- The GI preview's hair pass output never reaches the image.

## Items for the owner

Each of these was deferred or found during the port:

1. **DX11's NaNs on the first velocity frame.** The previous camera defaults to a `camCamera` that gives NaN velocities. Vulkan matches them. The owner deferred the fix to after the port.
2. **DX11's GI pass isn't deterministic.** It differs from itself by 2–6/255 on 0.002% of pixels from run to run.
3. **DX11's `shdwPassOpacity` clears only the first opacity map.** Maps 1–7 of OSM16/32 accumulate frame over frame. This was found by reading the code (Plan 7 Task 6) and not measured. Vulkan does the same.
4. **The `COLBL` (RenderMan color bleeding) pass crashed RenderNode on both backends**: no GPU renderer makes it. Resolved at the owner's request: `-capPasses` skips RenderMan passes with a warning and renders the rest.
5. **The GPU-assisted validation false positive** (vertex fetch bounds against the last draw's buffer). Filing it with the Khronos validation layers is outward-facing, so it hasn't been done.
6. **`vkAllocateMemory` returns `OUT_OF_DEVICE_MEMORY` while textures load** (best practices, RenderNode). The frames are correct; not investigated.
7. **`g2dWindowVkScene.HeadlessWindowMatchesSwapchainWindow` failed once under shard load:** an empty capture from the swap-chain window at a render resolution. It hasn't recurred.
8. **Coarse and fine derivatives.** D3D11's `ddx`/`ddy` are coarse and DXC's SPIR-V ones fine. Two live shader spots use them (`Support.hlsli` `filterwidth`/`filterarea`, `DepthMap.hlsl`). They are not changed; a SPIR-V pass could make them coarse.
9. **CI no longer runs GraphicsTests** (`afc32ed0`). The spec expected CI to run the DX11 side (on WARP) and skip Vulkan. Today the whole project is built but not run.

**DX11 bug fixes made during the port** (owner-approved, the same change on both backends):
- material reflections (`6848f8d2`);
- the alpha-fill pass's stale transparency map;
- `ShareBuffers` with stand-in partners;
- the baker's `CleanUp`;
- the hair pass with HDR AA;
- `GetRawBufferPFD` for the velocity buffer;
- `DrawImage` across formats;
- Skin/BlinnSkin's position semantic.

DX11's test-scene frames changed only with the reflection fix, and the reference was re-rendered then.
