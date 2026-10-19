# GraphicsVulkan remaining renderers and services — Implementation Plan (Phase 1, Plan 6)

> **Status: STUB.** Written in detail only after Plan 5 is finished and reviewed by the owner. Execute with superpowers:executing-plans, inline (no subagents); stop for review when done.

**Goal:** Every other renderer and service that GraphicsDX11 registers works on Vulkan, so nothing in MachStudio falls back or fails under `-gfx vulkan` (hair excepted until Plan 7).

**Spec:** "Scope" (Decided 5), "Constraints and facts" (the `SetImplementation` hooks), "Validation".

**Depends on (Plans 5a and 5b):** `shdw` passes, the HDR renderer, the simple renderer (`shdwShadowLayerRendererVk`), the depth map and RSM renderers, `shdwPassNormals`.

## Scope

- Renderers: texture, depth, opacity map, UV, normal, velocity map (with `shdwPassVelocity`), materials, illumination, shadow mask, reflection-only, glow-only, shadows-only, GI preview (with `shdwPassGIVolumes`, compiled out of the HDR renderer: Plan 5a decision 3), AO preview; pick renderer (`g3dPickRenderer`, pick buffer readback).
- Services: `g3dBake` (baking renderer), `g2dImageCreate` (loading files), the rest of `g2dImage` (resampling `CopyImage`, `DrawImage`, the sampled/greyscale/TGA/R16G16B16A16 `GetPixels`), `g2dImageSave`'s BMP, JPG, TIFF and DDS, texture saving and `MergeTransparentTextures`, `g2dResourceCounter` (video memory via memory budget), `g3dSystem::GetVideoAdapterName/GetVideoMemory`.
- Done in Plan 5a/5b: simple renderer, depth map (shadow maps), RSM. Not ported (unreachable on DX11, Plan 5a decision 3): single-pass, ray-trace and test renderers.
- Done in Plan 4b, because the test scene needed them: the `may` package (Maya fragment creator), `g2dScreenCaptureUtil` (all of it), RGBA-float `GetPixels`, and `g2dImageSave` for EXR, HDR, TGA and PNG (stb_image_write).
- Capture paths in `Features/Capture` (accumulation, motion blur sampling, batch/bake modes) exercised under Vulkan.
- Debug display (`g2dDebugDisplay`) and on-screen text overlays.
- Offscreen windows for RenderNode (owner's question during Plan 3): in `BATCH_MODE` a `g2dWindowVk` can render into a plain `vkdImage` (plus depth) with no surface or swap chain, `Present` only submitting, switched on through `g3dGfxOptions`. RenderNode then needs no desktop or display for Vulkan and capture timings leave presentation out. GraphicsVulkan and one option only; DX11 is unchanged. (Removing the hidden HWNDs from RenderNode altogether is a MainApp change DX11 would share: after Phase 1.)

## Exit criteria

- Synthetic GraphicsTests per renderer above; bake output and pick results identical to DX11 (pick IDs exactly equal).
- A capture with motion blur and accumulation on the test scene matches DX11 within tolerance.
- Grep check: every `SetImplementation` hook DX11 installs is installed by GraphicsVulkan (except hair).
- Zero validation errors/warnings.

## Open questions

- Which capture modes the test scene exercises; whether another scene (`Dume_Demo_testAO.mab`, `stress001_AOtiles.mab`) is needed for AO/bake coverage.
