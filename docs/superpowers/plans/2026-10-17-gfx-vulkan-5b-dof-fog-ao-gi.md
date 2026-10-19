# GraphicsVulkan DOF, fog, AO volumes and GI — Implementation Plan (Phase 1, Plan 5b)

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task, inline in the session (the owner asked for no subagents). Steps use checkbox (`- [ ]`) syntax for tracking. Start only after Plan 5a (`2026-10-17-gfx-vulkan-5a-hdr-beauty.md`) is done and reviewed; read its ledger's rulings first. When the plan is done, stop for the owner's review before Plan 6 is written in detail.

**Goal:** The rest of the HDR renderer runs on Vulkan and matches DX11: depth of field, fog, AO volumes, screen-space GI, LPV GI with reflective shadow maps, and the render-pass buffers that captures write. After this plan nothing the beauty path can reach falls back.

**Architecture:** This is the second half of Plan 5. On 5a's `shdw` package, HDR renderer, frame buffers and test helpers (`gtRender`, `gtSceneBuilder::RenderTargets`), it ports the passes the test scene never runs:
- `shdwPassDOF` and the HDR renderer's `RenderDOF`
- `shdwPassFog`
- `shdwPassAOVolumes`
- `shdwPassSSGI`
- `shdwPassLPVGI`, with RSMs, the RSM renderer, and 3D render targets drawn layer by layer from a geometry shader

Code is copied and changed only where it touches D3D, as before. Each task deletes its passes' stand-ins from 5a's `stub/shdwPassStubsVk`, which is empty and removed at the end. Every test compares DX11 and Vulkan through `gtExpectSameImage(s)`.

**Tech Stack:** C++17 (MSVC x64), Vulkan 1.4 C API via volk, VMA, DXC, GoogleTest, Python (`Tools/gfxcompare`).

**Spec:** `docs/superpowers/specs/2026-10-17-graphics-vulkan-design.md`, sections "Decided" 5 and 10, "Proving parity", "Validation".
- Roadmap: `docs/superpowers/plans/2026-10-17-graphics-vulkan-roadmap.md`.
- From Plan 5a:
  - `shdwHDRRendererVk`, `shdwFrameBufferMgr`, `shdwPassTraversal`, `shdwPassDepth`, the creator;
  - `gtRenderOptions`, `gtExpectSameImage(s)`, `gtPrefsScope`, and `gtSceneBuilder::ProjectedLight` / `RenderTargets` / `GlobalEnvironment`.

## Decisions made in this plan (please review)

1. **Layered rendering into volume textures.** LPV writes its 3D textures (`R32G32B32A32_FLOAT`) from a geometry shader that picks the slice with `SV_RenderTargetArrayIndex`. On Vulkan:
   - the 3D `vkdImage` is created with `VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT`;
   - its render-target view is a 2D-array view of all slices;
   - `vkdContext` renders with `layerCount` = the view's layer count (today it is always 1).

   No shader changes: DXC maps the semantic to `Layer`, which geometry shaders may write in core Vulkan. The same view serves D3D11's per-slice render-target views when the code binds one slice.
2. **Where the RSM renderer goes.** `g3dRSMRendererDX11` is listed in Plan 6's stub. It moves here, because LPV is its only user on the beauty path (MachStudio's projected lights have it commented out). The creator's `CreateRSMRenderer` becomes real, and `matTextureMgrVk::CreateReflectiveShadowMap` loses its `GVK_NOT_YET`.
3. **Branches ported but not tested, because nothing reaches them.** In the HDR renderer, the reflection-generation branch (`GetDoReflectionGen`) is copied with the rest of `Render` but has no test: nothing calls `g3dSingleLightRendering::SetDoReflectionGen`. A grep in Task 4 confirms that before the ruling is recorded.

   The file pass buffers *are* reached: capture render passes (`cmraDriverRenderPass`) set `g3dPassBuffers::SetDoingFileRefl`, and cameras give beauty and reflection buffers. They are tested in Task 4.
4. **Extra scenes.** The owner's folder has `Dume_Demo_testAO.mab` and `Dume_Demo_stress001_AOtiles.mab`. They get a short-window `gfxcompare` run here as extra coverage (spec, "Proving parity"), after a DX11-against-DX11 run shows they are deterministic.

## Global Constraints

Plan 5a's Global Constraints apply unchanged:
- no GraphicsDX11 or shader edits;
- DX11 still 135/135 against Plan 1's frames;
- `gvk`, `GVK_` guards and DX11's layout;
- ported code keeps DX11's structure, and differences need ledgered rulings;
- markers at the same points with the same names;
- validation 0/0;
- the spec's image tolerance, loosened only per case with a reason;
- the test speed rules;
- branch `claude/graphics-vulkan`, explicit staging, no push, the `Co-Authored-By` line.

## Review Focus

1. **DOF at its limits.** Focal distances behind the camera, near equal to far, maximum blur 0, and `m_MaxFarBlur` = -1 (DOF skipped). Each must look as on DX11, with no NaN smear. Task 1 tests `DOFLimitsMatchDX11`.
2. **Fog with nothing behind it.** Fog over the cleared background (depth 1), and fog with SSAO on, since both clear and reuse `DepthBuffer()`. Task 1 tests `FogOverBackgroundMatchesDX11` and `FogWithSSAOMatchesDX11`.
3. **Volume textures rebuilt.** A GI volume or RSM size changed between frames, so the 3D textures are recreated while the previous frame may still read them. Task 3 tests `LPVResizeBetweenFramesIsClean`.
4. **GI with no light that can make an RSM.** No projected light, a light outside the scene, or a light with shadows off. LPV must add nothing, as on DX11, not garbage from an uninitialized volume. Task 3 tests `LPVWithoutRSMLightMatchesDX11`.
5. **AO and GI together**, with SSAO or AO volumes, SSGI or LPV, and HDR AA on. They share scratch textures and `DepthBuffer()`, and each pass clears what the next restores. Task 4 tests `AOAndGICombinationsMatchDX11`.

---

## File Structure

| Path (under `LibXLT/GraphicsVulkan/` unless stated) | Responsibility |
|---|---|
| `shdw/shdwPassDOF`, `shdw/shdwPassFog` | DOF (with the prep pass) and fog |
| `shdw/shdwPassNormals`, `shdw/shdwPassAOVolumes`, `shdw/shdwPassSSGI`, `shdw/shdwPassLPVGI` | Normal/position buffers, AO volumes, screen-space GI, LPV GI |
| `shdw/shdwHDRRendererVk` | `RenderDOF` (5a's stub replaced) |
| `Mat/matReflectiveShadowMap`, `G3d/g3dRSMRendererVk` | RSMs and their renderer (decision 2) |
| `vkd/vkdImage`, `vkd/vkdContext` | Layered render targets (decision 1) |
| `Mat/matVolumeTexture` | Render-target volume textures for LPV |
| `UnitTests/GraphicsTests/Vk/shdwDofFogVkTest.cpp`, `shdwAoGiVkTest.cpp`, `vkdContextTest.cpp` (extend) | Tests |

---

### Task 1: DOF and fog

**Files:**
- Create: `shdw/shdwPassDOF` and `shdw/shdwPassFog`.
- Modify:
  - `shdwHDRRendererVk::RenderDOF` (DOF reserve, horizontal and full blur targets, the restore blend states);
  - debug mode 4;
  - `shdwPackage` (`InitStates` / `CleanupStates`).
- Test: create `Vk/shdwDofFogVkTest.cpp`.

**Interfaces — Consumes:** `camCamera::SetDOFParams` (camDOFData) and the scene's fog settings, set in the test's scene function.

- [ ] **Step 1: Failing tests:**
  - `DOFMatchDX11`: spheres at five depths, with near and far blur.
  - `DOFLimitsMatchDX11` (Review Focus 1): a case each for:
    - focal planes behind the camera;
    - near = far;
    - max blur 0;
    - `m_MaxFarBlur` −1.
  - `DOFDebugModeMatchesDX11`: mode 4.
  - `FogMatchDX11`: one case per `fogParams::m_nMode` that `shdwPassFog`'s switch handles, each with depth start/end, altitude start/end, both densities and a coloured fog, the camera tilted so the fog orientation matters.
  - `FogOverBackgroundMatchesDX11` and `FogWithSSAOMatchesDX11` (Review Focus 2).
- [ ] **Step 2:** Build; fail.
- [ ] **Step 3:** Implement.
- [ ] **Step 4:** Pass.
- [ ] **Step 5:** Commit — "GraphicsVulkan: depth of field and fog".

---

### Task 2: AO volumes and SSGI

**Files:**
- Create:
  - `shdw/shdwPassNormals` (view-space normals, and positions with `SetRenderPositions`). AO volumes draw through it; Plan 6's normal renderer reuses it.
  - `shdw/shdwPassAOVolumes` (the `m_bAOInvalid` path: normal and position passes, the reduced-resolution AO buffer, the `g_ACos` lookup texture);
  - `shdw/shdwPassSSGI` (multi-horizon GI, blur, depth peeling, its `CleanUp`).
- Modify: `shdwPackage`.
- Test: create `Vk/shdwAoGiVkTest.cpp`.

- [ ] **Step 1: Failing tests:**
  - `AOVolumesMatchDX11`: spheres resting on a plane, SSAO on with `m_bAOInvalid` on. Cases: `m_AOResolutionReduce` 0 and 1, and HDR AA on (the depth-only branch).
  - `SSGIMatchDX11`: coloured walls around white spheres. Cases: blur off and on, depth peeling off and on.
- [ ] **Step 2:** Build; fail.
- [ ] **Step 3:** Implement.
- [ ] **Step 4:** Pass.
- [ ] **Step 5:** Commit — "GraphicsVulkan: AO volumes and screen-space GI".

---

### Task 3: LPV GI, RSMs, layered rendering

**Files:**
- Modify:
  - `vkd/vkdImage.hpp/.cpp`: a `m_bLayeredTarget` desc flag for 3D images, giving `2D_ARRAY_COMPATIBLE` and an all-slices 2D-array view;
  - `vkd/vkdContext.cpp`: `layerCount` from the bound views, all equal or the draw is refused with a log line, as D3D11 requires;
  - `Mat/matVolumeTexture` (render-target volumes);
  - `matTextureMgrVk::CreateReflectiveShadowMap`;
  - `shdwSceneRendererCreate::CreateRSMRenderer`.
- Create: `Mat/matReflectiveShadowMap`, `G3d/g3dRSMRendererVk`, `shdw/shdwPassLPVGI` (with its vertex buffer and input layout as a `vkdVertexLayout`).
- Test: extend `Vk/vkdContextTest.cpp`; extend `Vk/shdwAoGiVkTest.cpp`.

**Interfaces — Produces:** `vkdImageDesc::m_bLayeredTarget` (bool, default false), legal only for 3D images; `vkdImage::LayeredView()`.

- [ ] **Step 1: Failing tests:**
  - `vkdContextTest.LayeredRenderingMatchesD3D11`: a geometry shader from the test shaders (`Vk/Shaders`) writes slice *i* with colour *i* into an 8×8×4 volume, on both APIs. Every slice is read back and must be equal.
  - `RSMMatchesDX11`: a projected light's RSM, all `e_NumRSMRenderTarget` colour targets (`R32G32B32A32_FLOAT`, as DX11's `matReflectiveShadowMap`), each read back and compared (floats within 1e-5 relative).
  - `LPVGIMatchDX11`: a coloured floor and a wall lit by a projected light, `m_bGIInvalid` on.
  - `LPVWithoutRSMLightMatchesDX11` (Review Focus 4).
  - `LPVResizeBetweenFramesIsClean` (Review Focus 3): the RSM size and the GI grid changed between frames; validation 0/0, and each frame compared.
- [ ] **Step 2:** Build; fail.
- [ ] **Step 3:** Implement.
- [ ] **Step 4:** Pass.
- [ ] **Step 5:** Commit — "GraphicsVulkan: LPV global illumination, RSMs and layered volume rendering".

---

### Task 4: Pass buffers and combinations

**Files:**
- Test: extend `Vk/shdwAoGiVkTest.cpp` and `Vk/shdwHDRRendererVkTest.cpp`. Library changes only where a test finds a difference.

- [ ] **Step 1: Confirm decision 3:** grep for `SetDoReflectionGen` callers. Expected: none. Ledger it.
- [ ] **Step 2: Failing tests:**
  - `PassBuffersMatchDX11`: a camera with beauty, reflection and AO pass buffers (`camPassBuffersData`; the AO buffer replaces the AO volumes pass), each blend op, and `g3dPassBuffers::SetDoingFileRefl(true)`, with a planar reflector.
  - `AOAndGICombinationsMatchDX11` (Review Focus 5): {SSAO, AO volumes} × {SSGI, LPV} × HDR AA {off, on}, one case each.
  - `g3dSceneRendererVkTest.UnportedTypesFallBack`: RSM leaves the list.
- [ ] **Step 3:** Run; fix what fails.
- [ ] **Step 4:** Pass.
- [ ] **Step 5:** Commit — "GraphicsVulkan: render-pass buffers; AO and GI combinations match DX11".

---

### Task 5: Scenes, timings; docs; stop for review

**Files:**
- Modify:
  - `CLAUDE.md`: the GraphicsVulkan `shdw` row, and `stub`;
  - the roadmap: Plan 5 done, with results and notes for Plan 6;
  - the Plan 6 stub: RSM done here.

- [ ] **Step 1: The test scene again.** Release, all 135 frames, as in Plan 5a Task 8 Step 2. Expected: 135/135, validation 0/0. Nothing this plan changed may move a frame; any change is investigated.
- [ ] **Step 2: Extra scenes (decision 4).**
  - First, DX11 against DX11 over 20 frames, once per scene. If a scene isn't deterministic on DX11 alone, report it and leave it out.
  - Then DX11 against Vulkan over the same 20 frames, with `-gfxValidation`.
  - Report each scene's results, and the passes its CSV shows (whether AO volumes, SSGI or LPV actually run in it).
- [ ] **Step 3: Debug validation run.** RenderNode Debug, `-gfx vulkan`, 5 frames of `Dume_Demo_testAO.mab`: validation 0/0.
- [ ] **Step 4: Timing.** The per-scope B/A medians for the test scene (as in 5a) and for each extra scene. The reflection passes are called out.
- [ ] **Step 5:** Six-configuration build, all tests passing. Then the DX11 harness: 135/135.
- [ ] **Step 6:** Edit the docs; commit — "GraphicsVulkan: docs and roadmap: Plan 5b done".
- [ ] **Step 7: Stop for review.** Report:
  - what was built;
  - parity per test, with any exceptions;
  - the scene runs;
  - the timing tables;
  - validation;
  - the remaining `GVK_NOT_YET` list;
  - rulings and deferred minors.

  Don't write Plan 6 in detail until the owner says so.
