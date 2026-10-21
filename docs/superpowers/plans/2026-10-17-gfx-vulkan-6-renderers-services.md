# GraphicsVulkan remaining renderers and services — Implementation Plan (Phase 1, Plan 6)

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task by task, inline in the session (the owner asked for no subagents). Steps use checkbox (`- [ ]`) syntax for tracking. As in 5a and 5b, **stop for the owner's review after each task**; the owner clears context between tasks, so resume from the plan's ledger (`.superpowers/sdd/2026-10-17-gfx-vulkan-6-renderers-services/progress.md`). Read the 5a and 5b ledgers' rulings first. When the plan is done, stop for review before Plan 7 is written in detail.

**Goal:** Every renderer and service that GraphicsDX11 installs works on Vulkan and matches DX11, so nothing in MachStudio falls back or does nothing under `-gfx vulkan`. Hair is the exception and waits for Plan 7, and so does the hair-only opacity map renderer (decision 2).

**Architecture:** The port follows Plans 5a and 5b: DX11's code is copied and changed only where it touches D3D. The work is in four groups:
- **CPU image services** (Tasks 1–3): `g2dImage`, image loading, image and texture saving, the resource counter, and the debug overlay. They move out of `stub/` into `G2d/` and `Mat/`.
- **Renderers** (Tasks 4–8): each preview renderer the creator still answers with the forward stand-in, plus the texture, UV, pick and bake renderers.
- **RenderNode without a desktop** (Task 9).
- **The capture paths, end to end through RenderNode** (Task 10), which needs a few new `-cap*` flags (decision 5).

Each test compares DX11 and Vulkan through `gtBackend` with the spec's tolerance, as in 5b.

One stub item is already done and gets no task: `g3dSystemVk::GetVideoAdapterName` / `GetVideoMemory` (Plan 2: the adapter's name and device-local bytes).

**Tech Stack:** C++17 (MSVC x64), Vulkan 1.4 C API via volk, VMA, DXC; stb_image / stb_image_write and libtiff (already in GraphicsVulkan's manifest); GoogleTest; Python (`Tools/gfxcompare`).

**Spec:** `docs/superpowers/specs/2026-10-17-graphics-vulkan-design.md`: "Constraints and facts" (the `SetImplementation` hooks), "Decided" 5, 9 and 10, "Proving parity", "Validation".
- Roadmap: `docs/superpowers/plans/2026-10-17-graphics-vulkan-roadmap.md`.
- From 5a and 5b:
  - the `shdw` package, `shdwFrameBufferMgr`, every pass the beauty path uses, `shdwPassNormals`, `shdwVelocityStateManager`, the depth map and RSM renderers;
  - the test helpers `gtRenderOptions` (`m_CreateRenderer`, `m_Frames`, `m_BeforeFrame`, `m_AfterRender`), `gtExpectSameImage(s)`, `gtExpectEffectShows`, `gtPrefsScope`, `gtReadTexture`, `gtSceneBuilder::ProjectedLight` / `RenderTargets` / `GlobalEnvironment`, and `gtScene::CheckerTexture(..., i_Blur)`.

## Decisions made in this plan (please review)

1. **Not ported, because nothing reaches them on DX11** (as 5a's decision 3):
   - `shdwPassGIVolumes`. `ENABLE_GI_VOLUME` is off (`Graphics/G3d/g3dConditionalCompile.hpp`), and the GI preview renderer's only use of the pass is inside `#ifdef ENABLE_GI_VOLUME` and commented out there too. This corrects the stub, which listed it with the GI preview.
   - `shdwAOVolumesRendererDX11`. Nothing creates it; `shdwPackage` only initialises its states.
   - The GI preview's `#ifdef ENABLE_GI_VOLUME` branch. It is copied as DX11 has it (compiled out), not ported.
2. **The opacity map renderer moves to Plan 7.** `shdwOpacityMapRendererDX11` with `shdwPassOpacity` fills a projected light's opacity shadow maps. Their only reader is `shdwPassHair` (grep `GetOpacityShadowMap`), so on Vulkan they matter only once hair draws. `CreateOpacityMapRenderer` keeps the forward stand-in until then.
3. **Image files are written without WIC.** GraphicsVulkan has no WIC or DirectXTex (5a Global Constraints). In their place:
   - BMP and JPG are written with stb_image_write, TIFF with libtiff, and DDS (texture saving only) with a port of DirectXTex's DDS header writer, next to the DDS reader port in `matImageDecode`.
   - **What WIC writes is measured first, and matched.** Task 2 Step 1 records the channels, bit depth and header fields of DX11's files for each source format, and Vulkan writes the same layout.
   - Pixels must decode equal, except JPG: two JPEG encoders never agree byte for byte. JPG gets a measured tolerance in an exception table, with the quality set to WIC's default (0.9 → 90).
   - `g2dImageSave` throws `g2dUnknownImageFileTypeX` for DDS, PPM, DIB and PFM, exactly as DX11's `get_d3d_file_format` does. (The stub was wrong to list DDS here; only texture saving writes DDS.)
4. **The resource counter counts what DX11 counts.** That is the sum of allocations by category, with the vertex and index buffer totals from `g3dVkBufferUtil`. It does not use the memory-budget extension the stub suggested, so the memory report means the same thing on both backends.
5. **New capture flags on MachStudio's command line**, so RenderNode can exercise the capture paths on the test scene (spec "Proving parity" 1). There is no other way to turn them on headless: Python is compiled out.
   - `-capSampling <n>` (`m_nCaptureSampling`)
   - `-capJitter <filter> <width>` (`m_bJitteredSampling` on, `m_FilterFunc` by its enum tag — `Box`, `Gaussian`, `Mitchell`, `Triangle`, `Sinc`, `Lanczos`, `Blackman-Harris`, `Catmull-Rom` — and `m_FilterWidth`)
   - `-capMotionSamples <n>` (`m_nMotionSamplesPerFrame`)
   - `-capPasses <pass,...>` (the current render layer's passes, by `captRenderOutputDataUtil::GetRenderPassName`)
   - `-capHardwareAA on|off` (`m_bHardwareAA`)

   They are toolkit-neutral (`mainCommandLine`, tested in MainAppTests, listed by `-help`), are scene settings like the other `-cap*` flags (they go after `-render`), and change nothing when not given. This is a MainApp change that DX11 shares, but it adds flags only.
6. **Headless RenderNode on Vulkan.** In `BATCH_MODE`, `g2dWindowVk` renders into a plain `vkdImage` color and depth pair at the window's size, with no surface and no swap chain. `Present` submits the frame and does nothing else.
   - It is switched on by a new `g3dGfxOptions::m_bHeadless`, which `mainInitGraphics` sets in `BATCH_MODE` builds.
   - GraphicsDX11 ignores the option, and the hidden HWNDs stay (the owner's ruling during Plan 3).
   - Captures read the offscreen image, as they already do with `SetRenderResolution`.

## Global Constraints

Plan 5a's Global Constraints apply unchanged:
- no GraphicsDX11 or shader edits;
- the DX11 harness still matches Plan 1's frames;
- `gvk`, `GVK_` guards and DX11's layout;
- no `d3d11.h`, DirectXTex or WIC in GraphicsVulkan;
- ported code keeps DX11's structure, and differences need ledgered rulings;
- profiler markers at the same points with the same names;
- validation 0/0;
- the spec's image tolerance, loosened only per case with a reason;
- the test speed rules;
- branch `claude/graphics-vulkan`, explicit staging, no push, the `Co-Authored-By` line.

Also:
- **Scene runs are short:** frames 296–315 of the test scene (owner's standing rule). All 135 frames only when the owner asks.
- **Anisotropic filtering is still open** (roadmap, "Open investigations"). Textured parity scenes use soft textures (`CheckerTexture` with `i_Blur`) or untextured materials, as 5a's tests do. A test that has to use a hard texture records its exception and the reason.
- **MainApp changes are limited to decision 5's flags and decision 6's option.** Nothing else in MachStudio changes.

## Review Focus

1. **Picking at the window's edges and on empty space.** A pick at pixel (0, 0), at (w−1, h−1), and on background, with sub-viewports, must give the same `g3dPickInfo` as DX11. The object ID must be exactly equal, and depth and UV within 1e-5. Background gives ID 0. Task 7 tests `PickEdgesAndBackgroundMatchDX11`.
2. **Saving an image that isn't 8-bit RGBA.** RGBA32F captures (HDR passes), R16G16B16A16 and 24-bit images, saved as PNG, BMP, JPG and TIFF, must give the layout DX11's WIC path gives (measured), or the same exception. Task 2 tests `SaveNonRGBA8MatchesDX11`.
3. **Resampling with a filter at odd sizes.** `CopyImage` with each `maFilter`, a filter width under 1 and over 1, sample rates 1–4, and sizes that don't divide evenly (cptrRenderUtil's supersampling and quadrant paths). Byte-identical to DX11, since it is the same CPU code. Task 1 tests `CopyImageFiltersMatchDX11`.
4. **Velocity with nothing moving, and with the first frame.** The velocity map on the first frame (no previous transforms) and on a frame where nothing moved must match DX11, with no NaNs and no stale data from another renderer. Task 6 tests `VelocityFirstAndStillFramesMatchDX11`.
5. **Baking an object with no UVs, or with UVs outside 0–1.** The bake must write what DX11 writes, or skip as DX11 does, with no crash and no validation message. Task 8 tests `BakeOddUVsMatchesDX11`.

---

## File Structure

| Path (under `LibXLT/GraphicsVulkan/` unless stated) | Responsibility |
|---|---|
| `G2d/g2dImageVk`, `G2d/g2dImageCreateVk`, `G2d/g2dResourceCounterVk`, `G2d/g2dDebugDisplay` | CPU images, loading, the resource counter, the debug overlay (moved out of `stub/g2dStubsVk`; `stub/g2dStubsVk` is deleted) |
| `G2d/private/g2dImageSaveVk.cpp`, `Mat/private/matImageEncode.{hpp,cpp}` | Image saving; BMP, JPG, TIFF and DDS encoders (decision 3) |
| `Mat/private/matTextureMgrVk.cpp` | Texture saving, `MergeTransparentTextures` |
| `shdw/shdw{Depth,Normal,Materials,Glow,ReflectionsOnly,ShadowsOnly,AOPreview,GIPreview,VelocityMap}RendererVk`, `shdw/shdwPassMaterials`, `shdw/shdwPassVelocity`, `shdw/shdwUVRendererVk` | Preview renderers and their passes |
| `G3d/g3dTextureRendererVk`, `G3d/g3dPickBufferRendererVk` | Texture and pick renderers |
| `shdw/shdwBakingRenderer` | `g3dBake` (replaces `g3dBakeVk` in `stub/g3dStubsVk`) |
| `stub/g3dStubsVk` | Afterwards it holds only hair's stand-ins (Plan 7) |
| `G2d/private/g2dWindowVk.cpp`, `Graphics/G3d/g3dGfxOptions.hpp`, `MachStudio/Source/MainApp/mainInitGraphics.cpp` | Headless windows (decision 6) |
| `MachStudio/Source/MainApp/mainCommandLine.{hpp,cpp}`, `mnmApp` | The capture flags (decision 5) |
| `UnitTests/GraphicsTests/Vk/g2dImageVkTest.cpp`, `g2dImageSaveVkTest.cpp`, `g2dResourceCounterVkTest.cpp`, `shdwPreviewRenderersVkTest.cpp`, `shdwVelocityVkTest.cpp`, `g3dPickVkTest.cpp`, `shdwBakeVkTest.cpp` | Tests |

---

### Task 1: Images and image loading

**Files:**
- Create: `G2d/g2dImageVk.hpp`, `G2d/private/g2dImageVk.cpp`, `G2d/g2dImageCreateVk.hpp`, `G2d/private/g2dImageCreateVk.cpp`, by moving them out of `stub/g2dStubsVk`.
- Port the rest of `g2dImageDX11`'s CPU code, which is the same code over `Lock()`ed pixels:
  - `CopyImage` with a filter (`maSampling::GetSamples2D` / `WeightSamples`) and with format conversion;
  - both `DrawImage`s;
  - `GetPixels` with sampling, greyscale and a rectangle;
  - `GetPixelsTGA`;
  - the R16G16B16A16 `GetPixels`, R8G8B8A8 from a 24-bit image, and RGBA32F from every format DX11 converts;
  - `SwapChannels`.
- Port `g2dImageCreateVk::Load` for BMP and PNG, as DX11's: other types throw `g2dUnknownImageFileTypeX`, a missing file throws `fsFileDoesntExistX`. Decode with `matImageDecode` (stb_image), not `g2dSurfaceLoader`.
- Test: create `UnitTests/GraphicsTests/Vk/g2dImageVkTest.cpp`.

**Interfaces — Produces:** `gvk::g2dImageVk` (unchanged public API, plus `Pixels()`) and `gvk::g2dImageCreateVk` in `G2d/`. Tasks 2 and 10 rely on them.

- [ ] **Step 1: Failing tests.** Each runs the same operations on images made by `g2dImageCreate::Make` on each backend, from the same source pixels (a gradient plus noise from `maFunctions::ResetRand`), and compares the bytes. They must be **byte-identical** (same CPU code).
  - `CopyImageFiltersMatchDX11` (Review Focus 3): every `maFilter` that `cptrRenderUtil` offers × filter width {0.5, 1, 2.5} × sample rate {1, 2, 3, 4}, from 257×129 to 64×33, and a plain copy across RGBA8 ↔ RGBA32F.
  - `DrawImageMatchDX11`: both overloads, with clipped and out-of-range rectangles.
  - `GetPixelsVariantsMatchDX11`: sampling 1–4, greyscale, a rectangle, TGA with "None" and with RLE, R16G16B16A16, R8G8B8A8 from 24-bit, RGBA32F from each format.
  - `SwapChannelsMatchesDX11`.
  - `LoadMatchesDX11`: a BMP and a PNG written by the test (stb_image_write) load to the same pixels and format; a `.jpg` throws `g2dUnknownImageFileTypeX` on both; a missing file throws `fsFileDoesntExistX` on both.
- [ ] **Step 2:** Build GraphicsTests (`-p:PostBuildEventUseInBuild=false`) and run the filter `g2dImageVkTest.*`. Expected: FAIL. The Vulkan side logs `GVK_NOT_YET` and gives empty or different bytes.
- [ ] **Step 3:** Implement. Move the classes and delete them from `stub/g2dStubsVk`; the resource counter stays there until Task 3.
- [ ] **Step 4:** Run `g2dImageVkTest.*`, then `g2dCaptureVkTest.*`, which uses `g2dImageVk`. Expected: PASS, with 0 differing bytes and validation 0/0.
- [ ] **Step 5:** Commit — "GraphicsVulkan: g2dImage and image loading".

---

### Task 2: Image and texture saving

**Files:**
- Create: `Mat/private/matImageEncode.{hpp,cpp}`, holding:
  - `matEncodeBMP`, `matEncodeJPG` (stb_image_write);
  - `matEncodeTIFF` (libtiff);
  - `matEncodeDDS` (a port of DirectXTex's `_EncodeDDSHeader` and the plain-pixel `SaveToDDSMemory` path for uncompressed formats).
- Modify:
  - `G2d/private/g2dImageSaveVk.cpp`: BMP, JPG and TIFF; PNG of any format DX11's WIC path accepts; DDS, PPM, DIB and PFM throw `g2dUnknownImageFileTypeX`, as DX11 does;
  - `Mat/private/matTextureMgrVk.cpp`: `SaveTextureToFile`, `SaveTextureToRgbaTiff`, `SaveTextureToRgbaPNG` (read back mip 0 / layer 0, as `CaptureTexture` then `GetImage(0, 0, 0)` does), and `MergeTransparentTextures`, ported over `g3dVkUtil::CopyTexToTarget` with DX11's two blend states.
- Test: create `UnitTests/GraphicsTests/Vk/g2dImageSaveVkTest.cpp`.

**Interfaces:**
- Consumes: Task 1's `g2dImageVk`.
- Produces, in namespace `gvk`:
  - `struct matEncodeImage { int m_Width; int m_Height; int m_Stride; uint32_t m_DxgiFormat; const uint8_t* m_pPixels; };`, where `m_DxgiFormat` is a plain DXGI_FORMAT value, as `vkdFormat` uses;
  - `std::vector<uint8_t> matEncodeBMP(const matEncodeImage&)`, and likewise `matEncodeJPG` (quality 90), `matEncodeTIFF` and `matEncodeDDS`. Each throws `g2dImageSaveX` for a format it can't write.

  Task 8's bake saves through `SaveTextureToFile`.

- [ ] **Step 1: Measure DX11** (decision 3). A throwaway test prints, for each extension (bmp, jpg, png, tif, dds) and each source format a capture or texture can have (RGBA8, BGRA8, RGBA32F, R16G16B16A16, R32F):
  - what DX11's `g2dImageSave::Save` and `SaveTextureToFile` write (bits per pixel, channels, BMP header version and alpha mask, TIFF samples per pixel, extra samples and compression, PNG bit depth, DDS header and DX10 header);
  - or which exception they throw.

  Record the table in the ledger. Write the tests from it, then delete the throwaway test.
- [ ] **Step 2: Failing tests.** Each saves the same content through both backends, checks Step 1's layout from the header, and decodes both files with the same decoder (stb_image, libtiff, `matImageDecode` for DDS). The decoded pixels must be equal.
  - `SaveFormatsMatchDX11`: RGBA8 to BMP, PNG, TIF. JPG is in the exception table with its measured max error and % over.
  - `SaveNonRGBA8MatchesDX11` (Review Focus 2): every Step 1 row that DX11 writes; for the rows DX11 throws on, the same exception type.
  - `SaveThrowsAsDX11`: `.dds`, `.ppm`, `.dib`, `.pfm`, `.xyz` through `g2dImageSave`.
  - `SaveTextureMatchesDX11`: a render-target texture and a plain texture, through `SaveTextureToFile` for each extension, `SaveTextureToRgbaTiff` and `SaveTextureToRgbaPNG`. DDS must be byte-identical.
  - `MergeTransparentTexturesMatchesDX11`: two soft-edged RGBA textures merged into a target, which is then read back. Within the image tolerance.
- [ ] **Step 3:** Build; run `g2dImageSaveVkTest.*`. Expected: FAIL (`GVK_NOT_YET`, `g2dImageSaveX`).
- [ ] **Step 4:** Implement.
- [ ] **Step 5:** Run `g2dImageSaveVkTest.*` and `g2dCaptureVkTest.*`. Expected: PASS, validation 0/0, no `GVK_NOT_YET` from `g2dImageSaveVk` or the texture savers.
- [ ] **Step 6:** Commit — "GraphicsVulkan: BMP, JPG, TIFF and DDS saving; texture saving and merging".

---

### Task 3: Resource counter and debug overlay

**Files:**
- Create:
  - `G2d/g2dResourceCounterVk.hpp` and `G2d/private/g2dResourceCounterVk.cpp` (out of `stub/`, which loses `g2dStubsVk`), with DX11's `AddResource(float i_SizeKBytes, eResourceCategory)` and `RemoveResource`;
  - `G2d/g2dDebugDisplay.hpp` and `G2d/private/g2dDebugDisplay.cpp` (DX11's, copied).
- Modify:
  - every Vulkan class that matches a DX11 class calling `AddResource`/`RemoveResource`: the depth-stencil buffer, render target, render-target texture, texture, and the window's swap chain (category `eWindow`). Each adds and removes the same KB DX11 computes for the same size and format;
  - `g2dWindowVk`: `SetDebugInfo` and the overlay through `g2dDebugDisplay`, drawn in `Present` as DX11's window does; the `GVK_NOT_YET` goes.
- Test: create `UnitTests/GraphicsTests/Vk/g2dResourceCounterVkTest.cpp`; extend `Vk/g2dWindowVkTest.cpp`.

- [ ] **Step 1: Failing tests:**
  - `CountsMatchDX11`: after building 5a's lit-sphere scene with a shadowed projected light, a planar reflector and a 256×256 window, each of the eleven getters gives the same value on both backends. Counts must be equal and KB within 0.5%, with any gap explained in the ledger. Then everything is released and each getter returns to its starting value.
  - `g2dWindowVkTest.DebugOverlayMatchesDX11`: three lines of debug info with the overlay on, read back from the window. Within the image tolerance (the font is Plan 4b's, and already matches).
- [ ] **Step 2:** Build; run. Expected: FAIL (0 from the stub; overlay `GVK_NOT_YET`).
- [ ] **Step 3:** Implement; delete `stub/g2dStubsVk.*` and its entries in the `.vcxproj` and `.filters`.
- [ ] **Step 4:** Run both. Expected: PASS, validation 0/0.
- [ ] **Step 5:** Commit — "GraphicsVulkan: resource counter and debug overlay".

---

### Task 4: Depth, normal, materials, glow and reflection-only renderers

**Files:**
- Create `shdw/`:
  - `shdwDepthRendererVk`, `shdwNormalRendererVk`;
  - `shdwMaterialsRendererVk` with `shdwPassMaterials`;
  - `shdwGlowRendererVk`, `shdwReflectionsOnlyRendererVk`.
- Modify:
  - `shdwSceneRendererCreate` (the five `Create*` calls);
  - `shdwPackage` (`InitStates` / `CleanupStates` for each, as DX11's);
  - `g3dSceneRendererVkTest.UnportedTypesFallBack` (the five types leave the list).
- Test: create `UnitTests/GraphicsTests/Vk/shdwPreviewRenderersVkTest.cpp`.

- [ ] **Step 1: Failing tests.** Each renders through `g3dSceneRendererCreate::Create<Type>Renderer` (`gtRenderOptions::m_CreateRenderer`), with HDR AA {off, on} (`NEW_AA_CODE` paths). Each has a `gtExpectEffectShows` guard that the renderer's image differs from the default renderer's on DX11.
  - `DepthRendererMatchesDX11`: spheres at five depths, plus a transparent one.
  - `NormalRendererMatchesDX11`: a bump-mapped sphere with a soft normal map, and a plain one.
  - `MaterialsRendererMatchesDX11`: one sphere per material type (`shdwMaterialsVkTest`'s list, untextured).
  - `GlowRendererMatchesDX11`: a glowing and a non-glowing sphere.
  - `ReflectionOnlyRendererMatchesDX11`: `shdwReflectionVkTest`'s planar and cube reflectors.
- [ ] **Step 2:** Build; run `shdwPreviewRenderersVkTest.*`. Expected: FAIL; Vulkan draws the forward stand-in.
- [ ] **Step 3:** Implement.
- [ ] **Step 4:** Run `shdwPreviewRenderersVkTest.*` and `g3dSceneRendererVkTest.*`. Expected: PASS, each case within tolerance, validation 0/0.
- [ ] **Step 5:** Commit — "GraphicsVulkan: depth, normal, materials, glow and reflection-only renderers".

---

### Task 5: Shadow mask, illumination, AO preview and GI preview

**Files:**
- Create `shdw/`:
  - `shdwShadowsOnlyRendererVk` (shadow mask `true`, illumination `false`);
  - `shdwAOPreviewRendererVk`;
  - `shdwGIPreviewRendererVk` (decision 1: its `ENABLE_GI_VOLUME` branch stays compiled out; hair through 5a's `shdwPassHair`, which draws nothing until Plan 7).
- Modify: the creator, `shdwPackage`, `UnportedTypesFallBack`.
- Test: extend `Vk/shdwPreviewRenderersVkTest.cpp`.

- [ ] **Step 1: Confirm decisions 1 and 2 by grep.** There must be no `#define ENABLE_GI_VOLUME`; `shdwAOVolumesRendererDX11` must have no `new`; `GetOpacityShadowMap` must be read only by `shdwPassHair`. Ledger the results.
- [ ] **Step 2: Failing tests:**
  - `ShadowMaskRendererMatchesDX11` and `IlluminationRendererMatchesDX11`: 5a's shadowed scene, with a projected light and a point light. Cases: one light, two lights, and shadows off on one light.
  - `AOPreviewRendererMatchesDX11`: 5b's AO scene. Cases: SSAO, AO volumes, and an object with `SetReceivesOcclusion(false)`.
  - `GIPreviewRendererMatchesDX11`: 5b's LPV room. Cases: SSGI, LPV, and LPV with HDR AA.
  - Each case has a DX11 guard that the renderer changes the image.
- [ ] **Step 3:** Build; run. Expected: FAIL (forward stand-in).
- [ ] **Step 4:** Implement.
- [ ] **Step 5:** Run `shdwPreviewRenderersVkTest.*`, `g3dSceneRendererVkTest.*`. Expected: PASS, validation 0/0.
- [ ] **Step 6:** Commit — "GraphicsVulkan: shadow mask, illumination, AO and GI preview renderers".

---

### Task 6: Velocity map, UV and texture renderers

**Files:**
- Create:
  - `shdw/shdwVelocityMapRendererVk` with `shdw/shdwPassVelocity` (on 5a's `shdwVelocityStateManager`);
  - `shdw/shdwUVRendererVk`;
  - `G3d/g3dTextureRendererVk`.
- Modify: the creator, `shdwPackage`, `UnportedTypesFallBack`. Only the opacity map type is left in it (decision 2).
- Test: create `UnitTests/GraphicsTests/Vk/shdwVelocityVkTest.cpp`; extend `Vk/shdwPreviewRenderersVkTest.cpp`.

- [ ] **Step 1: Failing tests:**
  - `VelocityMapMatchesDX11`: three frames (`m_Frames` 3, `m_BeforeFrame` moving one sphere, rotating another and moving the camera). The velocity target is compared with `gtReadTexture` and the image tolerance on its format.
  - `VelocityFirstAndStillFramesMatchDX11` (Review Focus 4): one frame only; then three frames with nothing moving; then the default renderer drawn between two velocity frames. Each is compared, with no NaN on either side.
  - `UVRendererMatchesDX11`: a sphere and a bump-mapped plane with UVs outside 0–1.
  - `TextureRendererMatchesDX11`: the ramp texture path `rmpTextureMgr` uses (`gtScene::RampTexture` into a render-target texture through `CreateTextureRenderer`).
- [ ] **Step 2:** Build; run. Expected: FAIL.
- [ ] **Step 3:** Implement.
- [ ] **Step 4:** Run `shdwVelocityVkTest.*`, `shdwPreviewRenderersVkTest.*`, `g3dSceneRendererVkTest.*`. Expected: PASS, validation 0/0.
- [ ] **Step 5:** Commit — "GraphicsVulkan: velocity map, UV and texture renderers".

---

### Task 7: Pick renderer

**Files:**
- Create: `G3d/g3dPickBufferRendererVk` (DX11's, with `GetPickInfo` reading the 1×1 RGBA32F target back through `vkdImage` readback in place of the staging texture). Delete `g3dPickRendererVk` from `stub/g3dStubsVk`.
- Modify: the creator's `CreatePickRenderer`.
- Test: create `UnitTests/GraphicsTests/Vk/g3dPickVkTest.cpp`. Its helper `gtPick(x, y, mask)` does what `pick3dPickBuffer::DoPickRender` does: the 1×1 RGBA32F render-target texture, the sub-viewport camera, `SetPickViewport`/`RestoreViewport`, and a `g3dTargetRenderer`. GraphicsTests doesn't link Tool.

- [ ] **Step 1: Failing tests.** Compare the `g3dPickInfo` fields: ID exactly equal, depth and UV within 1e-5.
  - `PickMatchesDX11`: a 16×16 grid of picks over a scene of overlapping spheres, a transparent sphere and a billboard, with two pick masks.
  - `PickEdgesAndBackgroundMatchDX11` (Review Focus 1): (0, 0), (w−1, h−1), background (ID 0), and a camera with a sub-viewport.
- [ ] **Step 2:** Build; run `g3dPickVkTest.*`. Expected: FAIL (`GVK_NOT_YET`, ID 0).
- [ ] **Step 3:** Implement.
- [ ] **Step 4:** Run. Expected: PASS, every ID equal, validation 0/0.
- [ ] **Step 5:** Commit — "GraphicsVulkan: pick renderer".

---

### Task 8: Baking

**Files:**
- Create: `shdw/shdwBakingRenderer` (DX11's `g3dBakeImpl`, on 5a and 5b's passes and Task 2's `SaveTextureToFile`).
- Modify: `GraphicsVulkanLayer.cpp` installs it in place of `g3dBakeVk`, which is deleted from `stub/g3dStubsVk`.
- Test: create `UnitTests/GraphicsTests/Vk/shdwBakeVkTest.cpp`.

**Interfaces — Consumes:** Task 2's `matTextureMgrVk::SaveTextureToFile`.

- [ ] **Step 1: Failing tests.** Each runs `g3dBake::Init` and `BakeNextNode` until it returns false, on each backend, into its own temporary folder. The files and the texture name maps must match: the same names, and pixels within the image tolerance once decoded.
  - `BakeMatchesDX11`: a lit sphere and plane, with cases for AO on, GI (LPV) on, both, `i_Res` 128 and 512, `i_TextureReduce` 0 and 1, and output formats png and tga.
  - `BakeOddUVsMatchesDX11` (Review Focus 5): a fragment without UVs and one with UVs in −1–2. Both backends write or skip the same files; there is no crash and no validation message.
- [ ] **Step 2:** Build; run. Expected: FAIL (`g3dBakeVk::Init` `GVK_NOT_YET`, no files).
- [ ] **Step 3:** Implement.
- [ ] **Step 4:** Run `shdwBakeVkTest.*`. Expected: PASS, validation 0/0.
- [ ] **Step 5:** Commit — "GraphicsVulkan: baking".

---

### Task 9: Headless RenderNode on Vulkan

**Files:**
- Modify:
  - `Graphics/G3d/g3dGfxOptions.hpp`: `bool m_bHeadless = false;`, with a comment that only GraphicsVulkan reads it;
  - `MachStudio/Source/MainApp/mainInitGraphics.cpp`: sets it under `#ifdef BATCH_MODE`;
  - `GraphicsVulkanLayer` / `g2dSystemVk`: pass it to each `g2dWindowVk`;
  - `G2d/private/g2dWindowVk.cpp`: in headless mode, no `vkdSwapchain` (held by `std::unique_ptr`, null when headless; `bool HasSwapchain() const` for the test). It renders into an owned color `vkdImage` (the swap chain's format, `COLOR_ATTACHMENT | TRANSFER_SRC | SAMPLED`) and depth image at the window's size, as the offscreen path already does. `Present` submits; resizes recreate the images.
- Test: extend `Vk/g2dWindowVkTest.cpp`.

**Interfaces — Produces:** `g3dGfxOptions::m_bHeadless`, read in Task 10's RenderNode runs.

- [ ] **Step 1: Failing tests:**
  - `HeadlessWindowMatchesSwapchainWindow`: the same scene drawn by a headless window and by a normal one gives 0 differing pixels when captured (`g2dScreenCaptureUtil`). Resize, `SetRenderResolution` and the debug overlay are all checked headless.
  - `HeadlessWindowHasNoSwapchain`: a headless window never constructs a `vkdSwapchain` (`g2dWindowVk::HasSwapchain()` is false, before and after `Present` and a resize), and validation is 0/0.
- [ ] **Step 2:** Build; run. Expected: FAIL (the option doesn't exist).
- [ ] **Step 3:** Implement.
- [ ] **Step 4:** Run the window tests. Then run RenderNode Release with `-gfx vulkan -gfxValidation` on the test scene, frames 296–315. Expected: PASS; 20 frames equal (0/255) to the same run before this task; validation 0/0. The `-gfxProfile` CSV shows no present time.
- [ ] **Step 5:** Commit — "GraphicsVulkan: headless windows for RenderNode".

---

### Task 10: Capture paths through RenderNode; hooks; docs; stop for review

**Files:**
- Modify:
  - `MachStudio/Source/MainApp/mainCommandLine.{hpp,cpp}` and `mnmApp::RunCommandLine`: decision 5's flags, with help text;
  - `UnitTests/MainAppTests` (parsing, and the help test listing every flag);
  - `Tools/gfxcompare` (if a pass's file names need it);
  - `CLAUDE.md` (the GraphicsVulkan rows; `stub` now holds only hair; the new `-cap*` flags in the MainApp paragraph);
  - the roadmap (Plan 6's status) and the Plan 7 stub (the opacity map renderer, decision 2).
- Test: `MainAppTests` and `gfxcompare` runs.

- [ ] **Step 1: Failing tests:** `mainCommandLineTest` cases for each new flag: values, a missing value, an out-of-range value, an unknown pass name, and order relative to `-render`. The help test must list them. Build MainAppTests and run. Expected: FAIL.
- [ ] **Step 2:** Implement the flags. Run MainAppTests. Expected: PASS.
- [ ] **Step 3: Capture runs** on the test scene, frames 296–315, with `gfxcompare run` (DX11 against Vulkan with `-gfxValidation`; headless). Each run is first checked DX11 against DX11, to show it is deterministic; any run that isn't is left out, with the reason ledgered.
  - Supersampling: `-capSampling 2`, and `-capSampling 3 -capJitter gaussian 1.5`.
  - Accumulation with motion blur: `-capMotionSamples 4`, and `-capMotionSamples 4 -capSampling 2`.
  - Every pass: `-capPasses` with each pass type that `cptrRenderThread` maps to a renderer (all but the opacity map).
  - Formats: `-capFormat` BMP, JPG, TIF, and DDS (both must throw, and the run must report it the same way).

  Expected: the same per-frame result as the beauty run gets today. Until the anisotropic investigation, that is the documented anisotropic gap and no new one. A pass whose frames differ more than the beauty pass is investigated, and a passing GraphicsTests case is added or fixed. Validation 0/0 for every run.
- [ ] **Step 4: Hook grep** (exit criterion). Every `SetImplementation` call GraphicsDX11 makes outside comments has a GraphicsVulkan counterpart with a real class, not one from `stub/`, except hair's. Also, `GVK_NOT_YET` appears only in hair and the opacity map code, `matTextureLoader::LoadResource` (the demo watermark, Plan 4a's ruling), and `vkdPipelineCache`'s step rate. Ledger the list.
- [ ] **Step 5: `ShareBuffers`.** With every type but the opacity map now a `shdw` renderer, put DX11's assert back for `shdw` partners and keep 5a Task 8's skip only for the forward stand-in. Test: `shdwHDRRendererVkTest.ShareBuffersWithEveryType`, which shares with every `Create*` type and renders once each, validation 0/0.
- [ ] **Step 6: Debug validation run:** RenderNode Debug, `-gfx vulkan`, headless, frames 296–300 with `-capPasses` set to every pass. Expected: validation 0/0, no asserts.
- [ ] **Step 7:** Build all six configurations; every test passes. Then run the DX11 harness (short): it must still match Plan 1's frames.
- [ ] **Step 8:** Edit the docs and commit — "GraphicsVulkan: docs and roadmap: Plan 6 implemented".
- [ ] **Step 9: Stop for review.** Report:
  - what was built;
  - parity per test, with exceptions (JPG; any anisotropic ones);
  - the capture runs;
  - validation;
  - the remaining `GVK_NOT_YET` list;
  - rulings and deferred minors.

  Don't write Plan 7 in detail until the owner says so.
