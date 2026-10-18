# GraphicsVulkan Phase 1 — roadmap of plans

**Spec:** `docs/superpowers/specs/2026-10-17-graphics-vulkan-design.md`

Phase 1 is too large for one plan, and later plans depend on what earlier
ones learn (the descriptor model, the device layer's shape, how faithful
the first images are). Each plan below produces working, testable software
on its own, and every plan must leave Vulkan validation clean (spec, "Validation"). The owner reviews each finished plan before the next is written or started. Only Plan 1 is written in detail. Plans 2–7 are stubs (scope, exit criteria, open questions); each is detailed when the previous one is finished and reviewed, from the spec plus what was learned.

| # | Plan | Delivers | Proven by |
|---|---|---|---|
| 1 | [Measurement and comparison harness](2026-10-17-gfx-vulkan-1-measurement-harness.md) | Command line parsed before graphics init (`mainCommandLine`); `g3dProfiler` (backend-neutral), DX11 GPU timers and markers under `MSP_GFX_PROFILING`, `-gfxProfile`, `UnitTests/GraphicsTests`, `Tools/gfxcompare` | Unit tests; RenderNode DX11-vs-DX11 run of the test scene passes and writes timing CSVs |
| 2 | [GraphicsVulkan skeleton and backend switch](2026-10-17-gfx-vulkan-2-skeleton.md) | Project in all six configurations, `gvk` namespace, `vkd` device (volk, VMA, device selection (Vulkan-only: discrete GPU, most device-local memory), swap chain per window, frames in flight, struct helpers, `VK_CHECK`), `-gfx`/`-gfxValidation`, required-feature check with named failures, Vulkan GPU timers; validation policy (layer required in Debug, debug-utils names and labels, errors assert, counts at exit, `gfxcompare` gate); descriptor-model prototype (`VK_EXT_descriptor_heap` → descriptor buffer → push descriptors) with the choice recorded | Window clears in Qt, Wx and RenderNode; zero validation errors/warnings (core + sync; GPU-assisted for the descriptor prototype); `-gfx vulkan` on the Intel GPU or with a missing feature exits with the reason |
| 3 | [Shaders and effects](2026-10-17-gfx-vulkan-3-shaders.md) | Copied shaders, DXC SPIR-V embed step, SPIRV-Reflect-based `fxEffect`, pipeline key and cache (persisted), constant ring buffer, full-screen quad, fonts; CI `compile_check.py --spirv` | GraphicsTests: full-screen quad and text images match DX11 |
| 4 | [First scene](2026-10-17-gfx-vulkan-4-first-scene.md) | tmesh/bump fragments, textures and texture manager, light manager, default renderer, one material (Blinn) | First synthetic-scene comparison and first test-scene frames (default renderer) |
| 5 | [Materials and `shdw` passes](2026-10-17-gfx-vulkan-5-materials-passes.md) | Remaining material effects; HDR renderer and its passes, glow, DOF, transparency, fog, environment, reflections (cube and planar), AO/SSAO, GI/SSGI/LPV, volumes, tone map | One synthetic test per pass/renderer; test scene beauty frames |
| 6 | [The other renderers and services](2026-10-17-gfx-vulkan-6-renderers-services.md) | Depth, depth map, RSM, opacity map, UV, normal, velocity, materials, illumination, shadow mask, reflection-only, glow-only, pick, bake, screen capture, image save, may fragments | Synthetic tests per renderer; bake and pick tests |
| 7 | [Hair and sign-off](2026-10-17-gfx-vulkan-7-hair-signoff.md) | Hair (hull shader workaround or deferral), full test-scene run, timing report Vulkan vs DX11, CLAUDE.md | Test scene passes within tolerance; timing report committed |

## Status

- **Plan 1: done** (2026-10-17; `0f415493`..`ac724e7b`, flag reference `cec88868`).
- **Plan 2: done** (2026-10-18; `8446cfd8`..`f1daf918`, plus the final-review fix), waiting for the owner's review. Plan 3 isn't written yet.
  - **Descriptor model, decided:** `VK_EXT_descriptor_heap`. Per draw, a binding table in the ring buffer (constant-buffer addresses, then texture, sampler and UAV heap indices by DX11 register slot); its address goes in with `vkCmdPushDataEXT`; DXC's `-fvk-[btsu]-shift` bindings map onto it, so the HLSL stays unchanged. Verified on the Quadro RTX 5000 (driver 596.59) with core, sync and GPU-assisted validation clean (`vkdBindingsTest`). The spec's fallbacks (descriptor buffers, push descriptors) weren't needed. Details in `vkd/vkdBindings.hpp`.
  - **Validation:** 0 errors and 0 warnings in the unit tests and in Qt/Wx/RenderNode start-up and shutdown, Debug and Release (`-gfxValidation`). The Intel UHD P630 is refused (it lacks Vulkan 1.4 and `VK_EXT_descriptor_heap`).
  - **For later plans:**
    - Every hook not yet ported is a stub that logs `GVK_NOT_YET`.
    - Rendering the test scene with `-gfx vulkan` stops at model loading, because `mdlFragCreateVk::CreateFragment` returns empty fragments (subdivision surfaces then find no vertices). Plan 4's real fragments fix this. Stub fragments already match DX11's format: `BumpTex1Vertex`, 16-bit indices unless `g3dIndexPtr::Needs32Bit`.
    - RenderNode with `-new` never exits, on DX11 as well (existing behaviour). Use `-version` for a start-up/shutdown check.
    - The Release DX11 frames are unchanged from Plan 1 (135/135).
