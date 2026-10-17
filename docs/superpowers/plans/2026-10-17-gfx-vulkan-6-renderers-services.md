# GraphicsVulkan remaining renderers and services — Implementation Plan (Phase 1, Plan 6)

> **Status: STUB.** Written in detail only after Plan 5 is finished and reviewed by the owner. Execute with superpowers:executing-plans, inline (no subagents); stop for review when done.

**Goal:** Every other renderer and service that GraphicsDX11 registers works on Vulkan, so nothing in MachStudio falls back or fails under `-gfx vulkan` (hair excepted until Plan 7).

**Spec:** "Scope" (Decided 5), "Constraints and facts" (the `SetImplementation` hooks), "Validation".

**Depends on (Plan 5):** `shdw` passes and the HDR renderer.

## Scope

- Renderers: simple, texture, depth, depth map, RSM, opacity map, UV, normal, velocity map, materials, illumination, shadow mask, reflection-only, glow-only, shadows-only, GI preview, AO preview, single-pass; pick renderer (`g3dPickRenderer`, pick buffer readback).
- Services: `g3dBake` (baking renderer), `g2dScreenCaptureUtil`, `g2dImageSave`, `g2dImageCreate`, `g2dResourceCounter` (video memory via memory budget), `g3dSystem::GetVideoAdapterName/GetVideoMemory`.
- `may` package: Maya fragment creator (`mdlFragCreate::SetImplementation`).
- Capture paths in `Features/Capture` (accumulation, motion blur sampling, batch/bake modes) exercised under Vulkan.
- Debug display (`g2dDebugDisplay`) and on-screen text overlays.

## Exit criteria

- Synthetic GraphicsTests per renderer above; bake output and pick results identical to DX11 (pick IDs exactly equal).
- A capture with motion blur and accumulation on the test scene matches DX11 within tolerance.
- Grep check: every `SetImplementation` hook DX11 installs is installed by GraphicsVulkan (except hair).
- Zero validation errors/warnings.

## Open questions

- Which capture modes the test scene exercises; whether another scene (`Dume_Demo_testAO.mab`, `stress001_AOtiles.mab`) is needed for AO/bake coverage.
