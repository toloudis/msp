# GraphicsVulkan Phase 2 — open items from Phase 1

Phase 1 ended on 2026-10-22 (Plan 7; report: [`docs/superpowers/reports/2026-10-22-gfx-vulkan-phase1.md`](../reports/2026-10-22-gfx-vulkan-phase1.md)). This file lists every issue that came up during Phase 1 and is still unresolved. Phase 2 plans are written from it and from the spec's "Phase 2 direction" (`docs/superpowers/specs/2026-10-17-graphics-vulkan-design.md`).

Sources: the roadmap's status notes, the Phase 1 report, and the plan ledgers in `.superpowers/sdd/2026-10-17-gfx-vulkan-*/progress.md`. Ledger references have the form "4a T4", meaning Plan 4a, Task 4.

**Phase 2's exit (spec decision 9):** Vulkan becomes the default backend only when it is at least as fast as DX11 on the test scene, on both CPU and GPU. At the end of Phase 1 its GPU time is lower (0.89×) but its per-pass CPU time is 1.4–14× DX11's.

## 1. Performance (the Phase 2 work itself)

Ordered as the Phase 1 timing suggests (report, "Phase 2, ranked by these numbers").

- [ ] **Per-object draw packets built once and replayed per pass, with persistent descriptors and constant data** that change only when the object changes. This targets the per-draw CPU cost: ZFill is 4–14× DX11's, AlphaFill 7–9× (it sets its effect's transparency state per node). Best practices' `Pipeline-SortAndBind` (the same pipeline bound twice in a frame) points the same way: sort draws by pipeline. Area18's `rndrDrawCall`/`rndrPass` is the spec's sketch.
- [ ] **Record reflection and shadow passes on several threads** (secondary command buffers). The planar reflection is Vulkan's largest single CPU cost: 2.7–2.9 ms, 3.8–3.9× DX11, and its ZFill alone is 12–14×. Measure again after the first item.
- [ ] **Less render-target round-tripping between `shdw` passes.** Vulkan is already faster on the GPU, so this has the least evidence behind it.
- [ ] **Memory priorities and pageable device-local memory**: best practices' `NVIDIA-AllocateMemory-SetPriority` (render targets highest) and `NVIDIA-CreateDevice-PageableDeviceLocalMemory` (`VK_EXT_pageable_device_local_memory`).
- [ ] **`vkAllocateMemory` returns `VK_ERROR_OUT_OF_DEVICE_MEMORY` while the test scene's DDS textures load** (RenderNode, best practices `Error-Result`). The frames are correct. The likely cause is VMA trying a full heap (e.g. the small host-visible device-local one) and falling back. Investigate before trying larger scenes (7 T8).
- [ ] **`NVIDIA-BindPipeline-SwitchTessGeometryMesh`** notes: switching between tessellation/geometry pipelines and plain ones. Look at it while sorting draws.
- [ ] **Measure a lighter scene, or the interactive viewport.** On `testTESS`/`testAO` the GPU time (72% SSAO) dwarfs the passes' CPU time, so CPU gains won't show there. Phase 2 needs a scene where they do.
- [ ] **A true draw-call count.** The profiler's draws column counts only DX11's points (bump, hair, font, full-screen quad, LPV), on both backends, so it undercounts both (7 T9 ruling). Phase 2's per-draw work needs the real number, at least on Vulkan.

## 2. DX11 behaviour found during the port (Vulkan matches it)

Vulkan copies each of these, because DX11 is the reference. Fixing one means the same minimal change on both backends, with the owner's approval.

- [ ] **NaN velocities on the first velocity frame.** The previous camera defaults to a `camCamera` that gives NaN velocities. `shdwVelocityVkTest` requires the same NaN texels on Vulkan. The owner deferred the fix until after the port (6 T6).
- [ ] **`shdwPassOpacity` clears only the first opacity map.** `matRenderTargetTexture::Clear` clears its own view, so maps 1–7 of OSM16/32 accumulate frame over frame under additive blending. Found by reading the code, not measured. Proposed fix: clear every target in `Render` (7 T6).
- [ ] **DX11's GI pass isn't deterministic**: 2–6/255 on 0.002% of pixels from run to run. It is left out of strict DX11-vs-DX11 checks (6 T10, 7 T9).
- [ ] **DX11's forward renderer never clears its transparency list.** The static `g3dTransparencySortDX11` keeps every transparent node it has seen and redraws them all on later renders, freed or not, so a second DX11 render in one process crashes. Vulkan clears its list. GraphicsTests renders each DX11 reference in a death-test child process because of it (4b T3).
- [ ] **DX11 can crash loading the font bitmap again** in a later run in the same process. This is order-dependent and its cause wasn't traced. The font tests run their DX11 half in a death-test child (4b T5).
- [ ] **DX11 texture-load crashes**: a hand-written 8×8 32-bit TGA (descriptor 0x28) crashed DX11's loader; a no-mip load asks DirectXTex for a one-level chain, which it rejects; a TGA or HDR it can't decode is dereferenced. Vulkan loads the texture or reports it missing (4a T4, roadmap Plan 4a).
- [ ] **Skin, BlinnSkin and testtess draw nothing with hardware tessellation on**, on either backend. Their `PTess` pass is left over from the old mesh-texture tessellation; for Skin and BlinnSkin it is a vertex shader only. Not investigated on DX11 (5a T7).
- [ ] **Hair: interpolation counts 0, 1, 64 and 100 draw the same image**, and tessellation 0, 0.5 and 1 also draw the same image. Vulkan matches each exactly. Interpolated strands probably need clumping settings that the default `Hair_SH` parameters don't give. Not investigated (7 T4).
- [ ] **Hair behaviour kept as DX11's:**
  - morphed hair is never re-uploaded (DX11's buffers are immutable), so `UpdateVertices` doesn't move the strands;
  - the simple renderer draws no hair;
  - the GI preview's hair pass output never reaches the image.

  Decide whether any of these should change (report, "What isn't ported").
- [ ] **Motion blur is compiled out.** `ENABLE_MOTIONBLUR` is off in the HDR renderer, and motion blur is commented out in `cptrRenderThread`, so `-capMotionSamples` changes only the saved setting (roadmap Plan 6).
- [ ] **DDS capture output fails on both backends**: the capture aborts with "Unknown image file type", exits 1 and writes no files (6 T10).
- [ ] **RenderNode with `-new` never exits**, on DX11 as well. Use `-version` for a start-up and shutdown check (roadmap Plan 2).
- [ ] **Small leaks and one undefined behaviour** reported in Debug test runs (4a final review):
  - Core's `fsResourceTracker` `l_InterestList` (8 bytes) is freed only by MainApp's `fsResourceTracker::CleanUp`;
  - one 80-byte block from GraphicsDX11's `matRenderTargetTexture::Make` after a volume draw (`CubeAndVolumeMatchDX11`'s DX11 run);
  - `g2dResetHandler` has no virtual destructor, but `DestroyResetHandlers` deletes through the base pointer (every current handler has a trivial destructor).

## 3. Vulkan, validation and tests

- [ ] **The GPU-assisted validation false positive.** In SDK 1.4.350, GPU-AV checks every indexed draw against the last draw's vertex buffer. `gpuav_vertex_attribute_fetch_oob` is off (reason in `vkdDevice.cpp`), and `vkdBufferGpuAvTest` is the minimal repro. Filing it with the Khronos validation layers is outward-facing and waits for the owner. Turn the check back on once a fixed layer ships (7 T8).
- [ ] **Intermittent `g2dWindowVkScene.HeadlessWindowMatchesSwapchainWindow`.** It failed once under shard load: the swap-chain window's capture at a render resolution (64×48) was empty. It passes alone and hasn't recurred. Likely suspect: the offscreen render-resolution capture racing the frame. Not investigated (7 T1).
- [ ] **CI doesn't run GraphicsTests** (`afc32ed0`). GitHub's runners have no GPU the tests can rely on. The spec expected CI to run the DX11 side on WARP and skip Vulkan. Options: a self-hosted GPU runner, or a WARP-only subset.
- [ ] **Coarse and fine derivatives.** D3D11's `ddx`/`ddy` are coarse and DXC's SPIR-V ones are fine (`OpDPdx`). Two live shader spots use them: `Support.hlsli` (`filterwidth`/`filterarea`) and `DepthMap.hlsl`. They are unchanged. Two possible fixes: a SPIR-V pass that makes them coarse, or `ddx_coarse` in the shaders. The second needs both copies changed, because `fxShaderCopyTest` keeps them byte-identical (7 T1; roadmap "Open investigations").
- [ ] **The level-of-detail difference at an angle** (closed as an API difference in 7 T1). Tests and gfxcompare allow it with a documented tolerance (`shdwMaterialsVkTest`; gfxcompare `--tolerance lod`). The owner planned to test anisotropic filtering further. Plan 7 Task 1's steps 5–6 (a test-scene rerun and the single-pixel outliers) were not done separately; every frame now passes `lod`.
- [ ] **`-fspv-preserve-interface` with trimming left 22 `OutputNotConsumed` warnings.** The flag was reverted; `vkdLinkStages` alone gives 0, so this only matters if the flag comes back (7 T7).
- [ ] **`GVK_NOT_YET` left in two places**, neither reached by DX11 doing real work (7 T10):
  - `matTextureLoader::LoadResource`, the `DEMO_VERSION` watermark (Windows resources; `DEMO_VERSION` is defined nowhere);
  - instance data step rates above 1 in `vkdPipelineCache` (no layout uses per-instance data).

  Port each if its feature is ever turned on.
