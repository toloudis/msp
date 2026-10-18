# GraphicsVulkan first scene — Implementation Plan (Phase 1, Plan 4b)

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task, inline in the session (the owner asked for no subagents). Steps use checkbox (`- [ ]`) syntax for tracking. Start only after Plan 4a (`2026-10-17-gfx-vulkan-4a-textures.md`) is done and reviewed. When the plan is done, stop for the owner's review before Plan 5 is written in detail.

**Goal:** Real scene geometry with real materials and textures renders on Vulkan. Synthetic scenes and text match DX11 within tolerance, and MachStudio's viewport and RenderNode show the test scene on Vulkan.

**Architecture:** Plan 4 is split in two; this is the second half. On Plan 3's `vkdContext`, `fxEffectVk` and pipeline cache, and Plan 4a's images, buffers and textures, it ports:
- fragments (`tmesh`, `bump`)
- the material shader classes (`Eff`)
- the per-node rendering machinery (`G3d`: util, render state traversal, fog, draw style, transparency sort)
- the plain forward renderer `g3dSceneRendererDX11`
- windows that render scenes, and fonts

Code is copied and changed only where it touches D3D, as in Plans 2, 3 and 4a. Each test drives both backends with Plan 4a's `gtBackend`.

**Tech Stack:** C++17 (MSVC x64), Vulkan 1.4 C API via volk, VMA, DXC, GoogleTest.

**Spec:** `docs/superpowers/specs/2026-10-17-graphics-vulkan-design.md`, sections "Library layout", "Proving parity", "Validation".
- Roadmap: `docs/superpowers/plans/2026-10-17-graphics-vulkan-roadmap.md`.
- From Plan 3: `vkdContext`, `vkdPipelineCache`, `vkdShader`, `fxEffectVk`/`effPlainEffectVk` (the fxEffect API), the state holders, `g2dFullscreenQuad`, `gtD3D11`, `gtVulkanContext`, `gtImage`.
- From Plan 4a (read its Interfaces blocks and its ledger's rulings first): `vkdImage`, `vkdBuffer`, `vkdFormat`, `matTextureMgrVk`, `matRenderTargetTexture`, `g3dVkTextureUtil::GetImage`, `gtBackend::Run`.

## Decisions made in this plan (please review)

1. **The renderer ported here is `g3dSceneRendererDX11`, the plain forward renderer.** MachStudio never reaches it: `shdwPackage` replaces GraphicsDX11's renderer creator, and its default renderer is `shdwHDRRendererDX11`, Plan 5's work.
   - So DX11-vs-Vulkan image parity in this plan is between the two forward renderers, in synthetic scenes.
   - Comparing test-scene frames needs the HDR renderer and moves to Plan 5.
   - Until then, the Vulkan creator answers every renderer type it can't build yet (default and HDR included) with the forward renderer, logged once with `GVK_NOT_YET`. MachStudio's viewport and RenderNode then show the scene, with no tone mapping, shadows or post effects. That makes it a smoke test with saved frames, not a parity test.
2. **Buffers** follow Plan 4a's decision 2: static vertex and index buffers are device-local `vkdBuffer`s; buffers DX11 updates with `Map(WRITE_DISCARD)` are written to the ring per use.
3. **All the material shader classes are ported** (`effShaderBaseDX11`, the 15 `eff*` classes, `effShaderArray`'s registration), not only Blinn's. `matShaderMgr` registers every effect at start-up, so a partial port would leave stubs on the main path.
4. **Windows:** `DrawImage`, `DrawRect` and `SetRenderResolution` are ported here (Task 4).

## Global Constraints

- **No GraphicsDX11 changes**, and no changes to shader sources. DX11 output is unchanged: the Plan 1 harness on the test scene, DX11 against Plan 1's frames, must give 135/135.
- **Code layout:**
  - All GraphicsVulkan code is in namespace `gvk`.
  - Include guards start with `GVK_`, and classes GraphicsDX11 names `...DX11` are `...Vk`.
  - Packages follow GraphicsDX11's layout; includes are guarded.
  - No `d3d11.h`, `dxgiformat.h`, DirectXTex or WIC in GraphicsVulkan. Tests may use DirectXTex.
- **Ported code** keeps DX11's structure, names and comments. Each D3D call becomes its `vkdContext` / `vkdImage` / `vkdBuffer` equivalent. A behaviour difference needs a ledgered ruling.
- **Validation:** zero errors and zero warnings (core and synchronization) in every test and run. `vkdValidation`'s notice list grows only with a ledgered ruling and a written reason.
- **Image tolerance:** the spec's, as in Plan 3. Max channel error ≤ 2/255, and at most 0.1% of pixels over 1/255. Loosen it only per test case, with a written reason in that test's exception table.
- Profiling markers follow Plan 1's naming rule (class name without package prefix and `Vk`).
- Git: branch `claude/graphics-vulkan`; stage explicit paths only; no push. Commit messages end with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.

## Review Focus

1. **Lifetimes.** A fragment or effect destroyed right after a draw that uses it must not be freed while the GPU still reads it. Task 1 tests `DestroyAfterDrawIsSafe` under synchronization validation.
2. **Big meshes.** A mesh above 65,535 vertices (32-bit indices) and one split into sub-fragments by the draw-call size limit must draw completely. Task 1 tests `LargeMeshMatchesDX11`.
3. **Windows while scenes render.** Resizing, minimizing and restoring a render panel during scene rendering must stay clean, and the swap-chain image must be acquired once per frame. Task 4 tests `SceneInWindowResizes`; Task 6 checks it by hand.
4. **Materials with values the UI can set but tests rarely use** (zero or very large specular power, fully transparent, textures missing). They must draw as on DX11, not as NaNs or black. Task 2's `SetupWritesSameConstants` includes one such material per class, and Task 3's `LitSpheresMatchDX11` draws them.
5. **Renderer types not ported yet.** Asking for any of them must give the forward renderer and one log line, never a null renderer. Task 3 tests `UnportedTypesFallBack`.

---

## File Structure

| Path (under `LibXLT/GraphicsVulkan/` unless stated) | Responsibility |
|---|---|
| `G2d/` (`g2dWindowDrawUtilVk`, `g2dFontUtilVk`) | Ports of the window-drawing and font classes |
| `tmesh/`, `bump/` | Ports of the fragment packages; `bumpPackage` installs the fragment creator and the bump renderer, as DX11's does |
| `Eff/` | Ports of `effShaderBaseDX11` (as `effShaderBaseVk`), the `eff*` classes, `effShaderParamsVk`, `effShaderArray` |
| `G3d/` (`g3dVkUtil`, `g3dSceneRenderUtil`, `g3dRenderStateCache`, `g3dSceneGlobal`, `g3dFogVk`, `g3dDrawStyleUtilVk`, `g3dTransparencySortVk`, `g3dSceneRendererVk`, `g3dSceneRendererCreateVk`) | Ports of the per-node rendering machinery and the forward renderer |
| `stub/` | Stubs replaced in this plan are deleted; the rest stay |
| `UnitTests/GraphicsTests/gtScene.hpp/.cpp` | Synthetic scenes built through the Graphics API: meshes (sphere, plane, large grid), materials, lights, camera |
| `UnitTests/GraphicsTests/Vk/*Test.cpp` | Tests per task |

---

### Task 1: Fragments (`tmesh`, `bump`)

**Files:**
- Create: `tmesh/*` (vertex and index buffers on `vkdBuffer`, `tmeshFrag`, BVH, `tmeshPackage`) and `bump/*`:
  - vertex declarations → `vkdVertexLayout`
  - `bumpBufferUtil`, `bumpTriMeshBumpFrag`, `bumpFragmentCreate`
  - `bumpBumpRenderer`, registered with `g3dRendererMgr` as DX11's is
  - `bumpPackage`
- Delete: the `g3dFragmentCreateVk` / `g3dFragmentVk` stubs. `GraphicsVulkanLayer` initializes `tmeshPackage` / `bumpPackage` in the order `GraphicsDX11Layer` does.
- Test: `UnitTests/GraphicsTests/gtScene.hpp/.cpp` (meshes only in this task), `Vk/bumpFragmentVkTest.cpp`.

**Interfaces — Produces:** `bumpBumpRenderer::Render` mirrors DX11's:
- the vertex layout from the declaration
- the tessellation pass choice
- sub-fragments
- the draw-call size limit
- `effect->Begin/BeginPass`, then `vkdContext::DrawIndexed`

`gtScene` provides `gtMesh Sphere(int i_Rings, int i_Segments)`, `Plane()`, `Grid(int i_N)` (Grid(300) has more than 65,535 vertices).

- [ ] **Step 1: Failing tests:**
  - `SameVertexAndIndexData`: a sphere created through `g3dFragmentCreate` on both backends. Vertex format, vertex bytes (basis vectors computed), index width and index bytes, bounding box and sub-fragments are equal.
  - `FragmentDrawsMatchDX11`: the sphere drawn with Special/Solid through `bumpBumpRenderer` on both, compared.
  - `LargeMeshMatchesDX11`.
  - `DestroyAfterDrawIsSafe`.
- [ ] **Step 2:** Build; fail.
- [ ] **Step 3:** Implement.
- [ ] **Step 4:** Pass.
- [ ] **Step 5:** Commit — "GraphicsVulkan: tmesh and bump fragments and the bump renderer".

---

### Task 2: The material shader classes (`Eff`)

**Files:**
- Create: `Eff/effShaderBaseVk.hpp/.cpp` (port of `effShaderBaseDX11` on `effPlainEffectVk`), the 15 `eff*` classes, `effShaderParamsVk`, `effShaderArray` (the registration table), `effShaderUtil` where needed.
- Delete: the `matShaderEffectVk` / `matShaderMgrVk` stubs. The real manager is installed where the stub was.
- Test: `Vk/effShaderVkTest.cpp`.

**Interfaces — Produces:**
- Every built-in shader is registered under the same name and data type as DX11.
- Each effect's parameters (`GetParamIndex`, `GetAllParamUIs`, `BuildPrtyObject`) match DX11.
- `SetupMaterial`, `SetupMatrices`, the lighting setup and `SetTexture` write the same constant bytes and bind the same textures as DX11.

- [ ] **Step 1: Failing tests:**
  - `SameShadersRegistered`.
  - `SameParamsAndUI`: every registered effect; parameter names, indices, UI kinds, ranges and defaults.
  - `SetupWritesSameConstants`: Blinn, Phong, Lambert and Solid materials with non-default values (and one edge-value material per class: Review Focus 4), one point light, one directional light and identity-free matrices. Compare the bytes of every constant buffer the default technique's passes read.
  - `TexturesBoundAtSameSlots`.
- [ ] **Step 2:** Build; fail.
- [ ] **Step 3:** Implement.
- [ ] **Step 4:** Pass.
- [ ] **Step 5:** Commit — "GraphicsVulkan: the material shader classes".

---

### Task 3: Per-node rendering machinery and the forward renderer

**Files:**
- Create the `G3d` ports listed in the File Structure: `g3dVkUtil` (the parts of `g3dDX11Util` the renderer, effects and quad helpers use), `g3dSceneRenderUtil`, `g3dRenderStateCache`, `g3dSceneGlobal`, `g3dFogVk`, `g3dDrawStyleUtilVk`, `g3dTransparencySortVk`, `g3dSceneRendererVk`, `g3dSceneRendererCreateVk`.
  - Each has the `InitStates`/`CleanupStates` DX11 has, called from `g3dSystemVk`'s state set-up.
  - `g3dSceneRendererCreateVk` follows decision 1 and replaces the stub creator.
- Extend `gtScene` with scenes, layers, nodes, materials (Blinn, textured Blinn, transparent Phong, testtess displacement), point and directional lights, and a camera.
- Test: `Vk/g3dSceneRendererVkTest.cpp`.

- [ ] **Step 1: Failing tests:** each renders the same scene with `g3dSceneRendererDX11` (DX11) and `g3dSceneRendererVk` into a 256×256 render-target texture, then compares.
  - `LitSpheresMatchDX11`: Blinn, Phong and Lambert, plus the edge-value materials of Review Focus 4; a point and a directional light.
  - `TexturedMatchDX11`.
  - `TransparencyMatchDX11`: sorted transparent nodes.
  - `FogMatchDX11`.
  - `WireframeDrawStyleMatchDX11`.
  - `TessellatedDisplacementMatchDX11`: the `PTess` passes.
  - `UnportedTypesFallBack`: each renderer type the creator can't build yet returns the forward renderer and logs it once.
- [ ] **Step 2:** Build; fail.
- [ ] **Step 3:** Implement.
- [ ] **Step 4:** Pass. A failing comparison is investigated and fixed, or excepted with a reason and its measured error.
- [ ] **Step 5:** Commit — "GraphicsVulkan: per-node rendering and the forward renderer; first scenes match DX11".

---

### Task 4: Windows render scenes

**Files:**
- Modify: `G2d/g2dWindowVk.hpp/.cpp`:
  - `BeginScene` acquires the swap-chain image (once per frame);
  - `MakeCurrent` and `MakeDepthCurrent` set it and the window's depth buffer on `vkdContext`;
  - `Present` submits and presents;
  - an acquire failure or a 0×0 window skips the frame, as Plan 2 did for clears.
- Ports: `DrawImage`, `DrawRect` (`g2dWindowDrawUtilVk`), and `SetRenderResolution` (an offscreen target, then `DrawTexturedQuad`, as `g2dWindowPrimaryDX11`).
- Test: `Vk/g2dWindowVkTest.cpp` (extend).

- [ ] **Step 1: Failing tests:**
  - `SceneInWindowMatchesOffscreen`: a synthetic scene rendered into a hidden window (read back before present) equals the same scene in a render-target texture.
  - `SceneInWindowResizes`: render, resize, render, 0×0, render, restore, render; zero validation messages.
  - `RenderResolutionScales`.
  - `DrawRectAndImageMatchDX11`.
- [ ] **Step 2:** Build; fail.
- [ ] **Step 3:** Implement.
- [ ] **Step 4:** Pass.
- [ ] **Step 5:** Commit — "GraphicsVulkan: windows render scenes".

---

### Task 5: Fonts (moved from Plan 3)

**Files:**
- Create: `G2d/g2dFontUtilVk` (port of `g2dFontUtilDX11`'s bitmap-font path: an `scrText` sprite drawn as a `tmeshFrag` with the font map texture), and its shaders copied verbatim from the run-time strings into `G2d/private/Shaders` (FontVS/FontPS, embedded like the quad's).
- Make window `DrawText` draw through it.
- Delete the font stub.
- Test: `Vk/g2dFontVkTest.cpp`.

- [ ] **Step 1: Failing tests:**
  - `TextMatchesDX11`: a few strings, sizes and colours, at offsets, into a window back buffer and into a render target.
  - `TextSizeMatchesDX11`: `GetTextSizeInPixels`.
- [ ] **Step 2:** Build; fail.
- [ ] **Step 3:** Implement.
- [ ] **Step 4:** Pass.
- [ ] **Step 5:** Commit — "GraphicsVulkan: fonts and window text".

---

### Task 6: MachStudio on the first scene; docs; stop for review

**Files:** Modify `CLAUDE.md` (GraphicsVulkan packages), the roadmap (Plan 4b, and so Plan 4, done, with results and notes for Plan 5) and the Plan 5 stub (test-scene parity moved there, per decision 1).

- [ ] **Step 1: Manual run, Debug Qt and Release Qt, `-gfx vulkan`.** Open the test scene. The viewport shows it through the forward stand-in renderer.
  - Orbit the camera, resize the panels, change the panel layout, minimize and restore.
  - Quit. The log shows `Vulkan validation: 0 errors, 0 warnings`.
  - Record the `GVK_NOT_YET` lines the session logs.
- [ ] **Step 2: RenderNode.** Render the test scene's frame range with `-gfx vulkan -gfxValidation` and `-gfxProfile`.
  - Expected: frames written, validation 0/0, a timing CSV.
  - Keep a few frames for the report (a smoke test, not parity: decision 1).
- [ ] **Step 3:** Six-configuration build with all tests passing. Then the DX11 harness: DX11 against Plan 1's frames, 135/135.
- [ ] **Step 4:** Edit the docs; commit — "GraphicsVulkan: docs and roadmap: Plan 4b done".
- [ ] **Step 5: Stop for review.** Report:
  - what was built;
  - the parity results per test, with any exceptions;
  - format substitutions;
  - validation;
  - the remaining `GVK_NOT_YET` list;
  - the RenderNode smoke frames;
  - rulings and deferred minors.

  Don't write Plan 5 in detail until the owner says so.
