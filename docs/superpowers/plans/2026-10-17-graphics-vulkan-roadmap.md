# GraphicsVulkan Phase 1 — roadmap of plans

**Spec:** `docs/superpowers/specs/2026-10-17-graphics-vulkan-design.md`

Phase 1 is too large for one plan, and later plans depend on what earlier
ones learn (the descriptor model, the device layer's shape, how faithful
the first images are). Each plan below produces working, testable software
on its own. Only Plan 1 is written in detail; each later plan is written
when the previous one is finished, from the spec plus what was learned.

| # | Plan | Delivers | Proven by |
|---|---|---|---|
| 1 | Measurement and comparison harness (`2026-10-17-gfx-measurement-harness.md`) | `g3dProfiler` (backend-neutral), DX11 GPU timers and markers under `MSP_GFX_PROFILING`, `-gfxProfile`, `UnitTests/GraphicsTests`, `Tools/gfxcompare` | Unit tests; RenderNode DX11-vs-DX11 run of the test scene passes and writes timing CSVs |
| 2 | GraphicsVulkan skeleton and backend switch | Project in all six configurations, `gvk` namespace, `vkd` device (volk, VMA, LUID device match, swap chain per window, frames in flight, struct helpers, `VK_CHECK`), `-gfx`/`-gfxValidation`, required-feature check with named failures, Vulkan GPU timers; descriptor-model prototype (`VK_EXT_descriptor_heap` → descriptor buffer → push descriptors) with the choice recorded | Window clears in Qt, Wx and RenderNode; validation-clean; `-gfx vulkan` on the Intel GPU or with a missing feature exits with the reason |
| 3 | Shaders and effects | Copied shaders, DXC SPIR-V embed step, SPIRV-Reflect-based `fxEffect`, pipeline key and cache (persisted), constant ring buffer, full-screen quad, fonts; CI `compile_check.py --spirv` | GraphicsTests: full-screen quad and text images match DX11 |
| 4 | First scene | tmesh/bump fragments, textures and texture manager, light manager, default renderer, one material (Blinn) | First synthetic-scene comparison and first test-scene frames (default renderer) |
| 5 | Materials and `shdw` passes | Remaining material effects; HDR renderer and its passes, glow, DOF, transparency, fog, environment, reflections (cube and planar), AO/SSAO, GI/SSGI/LPV, volumes, tone map | One synthetic test per pass/renderer; test scene beauty frames |
| 6 | The other renderers and services | Depth, depth map, RSM, opacity map, UV, normal, velocity, materials, illumination, shadow mask, reflection-only, glow-only, pick, bake, screen capture, image save, may fragments | Synthetic tests per renderer; bake and pick tests |
| 7 | Hair and sign-off | Hair (hull shader workaround or deferral), full test-scene run, timing report Vulkan vs DX11, CLAUDE.md | Test scene passes within tolerance; timing report committed |
