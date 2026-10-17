# GraphicsVulkan first scene — Implementation Plan (Phase 1, Plan 4)

> **Status: STUB.** Written in detail only after Plan 3 is finished and reviewed by the owner. Execute with superpowers:executing-plans, inline (no subagents); stop for review when done.

**Goal:** Real scene geometry renders through the default renderer with one material (Blinn), and the first synthetic and test-scene frames match DX11.

**Spec:** "Library layout", "Proving parity", "Validation".

**Depends on (Plan 3):** effects, pipeline cache, ring buffer, state holders, readback.

## Scope

- `tmesh` (vertex/index buffers, fragments, BVH as needed) and `bump` (fragment creator via `g3dFragmentCreate::SetImplementation`, `bumpBumpRenderer`, vertex declarations → vertex input state); draw path mirrors `bumpBumpRenderer::Render` (tessellation pass selection, sub-fragments, draw-call size limit).
- Textures: `matTextureVk`, mip/plain/cube/volume/render-target textures, DDS/HDR/PNG/TIFF loading (DirectXTex decode, Vulkan upload), `matTextureMgr` hook; `GenerateMips` equivalent with blits.
- `g3dLightMgr` hook (light manager), `matShaderMgr` hook.
- `g3dSceneRendererCreate` implementation registering the default renderer (others return a clear "not yet implemented" until Plans 5–6, logged once).
- Render targets / depth-stencil buffers (`g2dRenderTarget`, `g2dDepthStencilBuffer`) on dynamic rendering.
- Profiling markers following Plan 1's naming rule (class name minus prefix and `Vk`).

## Exit criteria

- GraphicsTests synthetic scenes (lit sphere and plane with Blinn, textured; one with tessellated displacement) match DX11 within tolerance.
- `gfxcompare run` on the test scene with the default renderer forced (via render prefs or a test scene variant) produces comparable frames; differences explained or fixed.
- Timing CSV for both backends on the same frames.
- Zero validation errors/warnings.

## Open questions

- How to force a specific renderer for test-scene comparisons (prefs flag vs. scene file variant).
- Texture format substitutions Vulkan needs (e.g. 24-bit formats) and their test.
