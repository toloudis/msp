# GraphicsVulkan shaders and effects — Implementation Plan (Phase 1, Plan 3)

> **Status: STUB.** Written in detail only after Plan 2 is finished and reviewed by the owner. Execute with superpowers:executing-plans, inline (no subagents); stop for review when done.

**Goal:** Every material and built-in effect compiles to SPIR-V from unchanged HLSL, is reflected and bound through the same `matShaderEffect` interface as DX11, and the first simple images (full-screen quad, text) match DX11.

**Spec:** "Shaders", "Library layout" (pipeline cache, ring buffer, binding), "Modern Vulkan", "Validation".

**Depends on (Plan 2):** `vkd` device, chosen descriptor model, swap chain, GraphicsTests offscreen setup, validation gate.

## Scope

- Copy `GraphicsDX11/Eff/private/Shaders` (HLSL, `*.hlsli`, `*.effect.json`, `EmbedShaders.ps1`) to `GraphicsVulkan/Eff/private/Shaders`, unchanged (a test compares the copies byte-for-byte with the DX11 originals, except the hair file added in Plan 7).
- Build step: DXC `-spirv`, `_5_0` → `_6_0` profiles, `-fvk-{b,t,s,u}-shift` mapping to the chosen descriptor layout, `-fvk-use-dx-layout`, negative-viewport convention (no `-fvk-invert-y` if the viewport flip is used); embed as `Fx/private/Generated/fxEmbeddedShadersVk.inc`. Hair hull shaders excluded until Plan 7.
- `fxEffectDesc` (JSON) copied; `fxEffectVk` with SPIRV-Reflect replacing `D3DReflect`, keeping parameter names/indices/`matShaderEffect` behaviour identical; sampler states from the JSON.
- `vkd` pipeline key and cache: shader set, vertex layout, topology class, blend, depth/stencil, rasterizer, RT formats, samples; persisted (`VK_KHR_pipeline_binary` or `VkPipelineCache`); extended dynamic state for viewport/scissor/cull/depth.
- Per-frame constant/dynamic-vertex ring buffer replacing `Map(WRITE_DISCARD)`.
- State holders replacing `g3dBlendStateMgr`, `g3dDepthStencilStateMgr`, `g3dRasterizerStateMgr`, `g3dStateMgr`.
- Run-time-compiled shaders (fonts, full-screen quad, tone map, ray tracing, LPV): precompile where possible; otherwise run-time DXC.
- Ports: `g2dFullscreenQuad`, `g2dFontUtil` (fonts), `g2dImageCreate`/`g2dImageSave`/`g2dScreenCaptureUtil`/`g2dResourceCounter` hooks as needed for readback.
- CI: `compile_check.py --compiler dxc --spirv` over GraphicsVulkan's effects.

## Exit criteria

- All effect stages except the 3 hair hull shaders compile to SPIR-V in the build; CI check passes.
- GraphicsTests: full-screen quad and text images, Vulkan vs DX11, within tolerance.
- Reflection test: for every effect, parameter names, indices, constant-buffer offsets and sizes match the DX11 reflection.
- Zero validation errors/warnings.

## Open questions

- Readback path for tests and capture (staging buffer copy vs. host image copy).
- Whether `$Globals` constant buffers need special handling in reflection.
