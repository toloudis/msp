# GraphicsVulkan HDR beauty path — Implementation Plan (Phase 1, Plan 5a)

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task, inline in the session (the owner asked for no subagents). Steps use checkbox (`- [ ]`) syntax for tracking. When the plan is done, stop for the owner's review before Plan 5b (`2026-10-17-gfx-vulkan-5b-dof-fog-ao-gi.md`) is started.

**Goal:** MachStudio's default renderer, `shdwHDRRendererDX11`, and everything the test scene's beauty frames go through run on Vulkan, so the test scene renders on `-gfx vulkan` within tolerance of DX11, frame by frame.

**Architecture:** Plan 5 is split in two; this is the first half.
- `shdw` is copied into `gvk` as Plans 2–4 copied their packages: the pass logic stays as it is, and each D3D call becomes its `vkdContext` / `vkdImage` / `g3dVkUtil` equivalent.
- `shdwPackage` installs `shdw`'s renderer creator over `g3dSceneRendererCreateVk`, as GraphicsDX11's does. This ends Plan 4b's forward-renderer stand-in for the renderer types ported here.
- What this half ports:
  - the HDR renderer with its opaque passes, tone mapping, bloom and star;
  - shadow maps and the depth map renderer;
  - transparency (sorted, depth-peeled, deferred);
  - SSAO, glow, outline, the post effects and matte rendering;
  - the simple renderer (`shdwShadowLayerRendererDX11`) and the planar and cube-map reflection renderers built on it;
  - an image test for every material shader.
- Plan 5b adds DOF, fog, AO volumes, SSGI, LPV GI and the HDR renderer's remaining branches.

Which passes come first is measured, not guessed. Plan 1's DX11 timing CSV of the test scene (135 frames × 16 jitter samples) shows what a beauty frame runs: `HDRRenderer` with `PassZFill`, `PassEnvironment`, `PassLit`, `PassSSAO` (with its `PassDepth`), `PassAlphaFill`, `PassDepth` and `PassGlow`, tone mapping, and `PlanarReflection` → `ShadowLayerRenderer`. There is no shadow-map, cube-map, DOF, fog or GI marker. 5a covers all of that, plus the passes the same code paths reach with other scene settings (transparency, outline, cube maps, shadows, post effects).

**Tech Stack:** C++17 (MSVC x64), Vulkan 1.4 C API via volk, VMA, DXC, GoogleTest, Python (`Tools/gfxcompare`).

**Spec:** `docs/superpowers/specs/2026-10-17-graphics-vulkan-design.md`, sections "Decided" 5 and 10, "Proving parity", "Measurement", "Validation".
- Roadmap: `docs/superpowers/plans/2026-10-17-graphics-vulkan-roadmap.md`.
- From Plans 3, 4a and 4b, read before starting (their Interfaces blocks and rulings):
  - `vkdContext` (state, render targets, the implicit DX11 barriers), `vkdImage` (`GenerateMips`), `fxEffectVk`, `effShaderBaseVk` and the `eff*` classes;
  - `matTextureMgrVk`, `matRenderTargetTexture` (`Resolve`, `CopyWithResolve`), `matCubeRenderTargetTexture`;
  - `g3dVkUtil` (fully ported in 4b, including `CopyDepth`, `CopyTexToTarget`, `BlendBuffers`), `g3dSceneRenderUtil`, `g3dRenderStateCache`, `g3dSceneGlobal`, `g3dFogVk`, `g3dDrawStyleUtilVk`, `g3dTransparencySortVk`, `g3dLightMgrVk`;
  - `g2dFullscreenQuad`, `gtBackend`, `gtScene` / `gtSceneBuilder`, `gtTarget`, `gtImage`.

## Decisions made in this plan (please review)

1. **Plan 5 is split into 5a (this plan) and 5b**, each with its own review, as Plan 4 was. The split follows what the test scene uses. 5a ends with test-scene parity, the first time Vulkan's frames are compared with DX11's. 5b takes the passes the test scene never runs: DOF, fog, AO volumes, SSGI, LPV GI with RSMs and layered rendering into volume textures, and the HDR renderer's reflection-generation and file pass-buffer branches.
2. **The creator, as on DX11.** `gvk::shdwPackage::Init` installs `gvk::shdwSceneRendererCreate` over `g3dSceneRendererCreateVk` (which stays, as `g3dSceneRendererDX11Create` stays in GraphicsDX11). After 5a:
   - it answers default, HDR, simple, depth map, cube map and planar reflection with the real renderers;
   - Plan 6's types (texture, RSM until 5b, opacity map, AO, GI, depth, UV, shadow mask, illumination, normal, velocity, materials, reflection-only, glow) keep the forward renderer and their `GVK_NOT_YET` line;
   - the pick renderer stays the stub.

   The passes only those renderers use go with them to Plan 6: `shdwPassOpacity`, `shdwPassMaterials` and `shdwPassVelocity`. `shdwPassNormals` is ported in 5b, where AO volumes need it.
3. **Not ported, because nothing in MachStudio reaches it on DX11.** A grep in Task 1 confirms each one before it is left out:

   | DX11 code | Why it is unreachable |
   |---|---|
   | `rtUtil`, `rasterizerUtil` | used only by the next two renderers |
   | `shdwRayTraceRenderer`, `shdwTestRendererDX11`, `shdwSinglePassRendererDX11` | no creator returns them (commented-out lines in `CreateDefaultRenderer`) |
   | `effShaderArray::GetDefaultEffect`, `GetDefaultLightingEffect`, `GetMapNormalsToScreen` (the `.o` files) | used only by `rasterizerUtil`. This drops the stub's "default/lighting/map-normals shaders" item. |
   | `shdwPassMapNormalsToScreen` | never constructed |
   | `g3dVSMRendererDX11` | `USE_VSM_SHADOWS` is off |
   | `g3dCubeMapRendererDX11` | only `g3dSceneRendererDX11Create` returns it, and `shdw`'s creator replaces that |
   | `g3dRenderCameraSpaceObjects`, `g3dRenderScreenSpaceObjects` | only their `InitStates` run |
   | the motion-blur branch and `shdwPassVelocity` | `ENABLE_MOTIONBLUR` is off. The velocity pass goes to Plan 6 with the velocity map renderer. |
   | `shdwPassGIVolumes` | `ENABLE_GI_VOLUME` is off. It goes to Plan 6 with the GI preview renderer. |

   The stub's "one deferred-context use" doesn't exist: GraphicsDX11 never creates a deferred context, and the only mention is a comment in `g2dDX11GlobalWin.cpp`. The stub's "LPVGI (compute)" is also wrong. LPV draws into 3D textures through a geometry shader (`SV_RenderTargetArrayIndex`), and there is no compute on the beauty path. That work is in 5b.
4. **Occlusion queries** (transparency's depth peeling counts the samples each peel draws and stops at zero, spinning on `GetData`) become `vkdOcclusionQuery`.
   - A query pool of one query, reset on the host with `vkResetQueryPool`. This needs Vulkan 1.2's `hostQueryReset`, which joins the required features.
   - `Begin` / `End` are recorded in the command stream, inside dynamic rendering.
   - `GetResult` flushes the context and waits for the result: the same stall DX11 has, kept in Phase 1.
5. **Hair stays out until Plan 7.** `HAIR_SUPPORTED` is on, so the HDR renderer runs `shdwPassHair` when `m_bEnableHair` is set. Its Vulkan copy is a stub that draws nothing and logs `GVK_NOT_YET` once. The test scene's beauty frames never run it: there is no `PassHair` marker in Plan 1's CSV.
6. **Tests render through the creator, not through named classes.** The scene parity tests call `g3dSceneRendererCreate::CreateDefaultRenderer()` (and the other `Create*` calls) on whichever backend `gtBackend` has up. Both backends then run the same test code end to end, creator included, as MachStudio does.
   - The render-and-compare helpers that `g3dSceneRendererVkTest` keeps to itself move into `gtRender` (Task 1).
   - `g3dPrefs` and `g3dSingleLightRendering` are set per test by a guard that restores them. MachStudio runs with single-light rendering on; that is the guard's default.

## Global Constraints

- **No GraphicsDX11 changes**, and no changes to shader sources. DX11 output is unchanged: the Plan 1 harness on the test scene, DX11 against Plan 1's frames, must give 135/135.
- **Code layout:**
  - All GraphicsVulkan code is in namespace `gvk`.
  - Include guards start with `GVK_`, and classes GraphicsDX11 names `...DX11` are `...Vk`. `shdw` classes without the suffix (`shdwPassLit`, `shdwFrameBufferMgr`, ...) keep their names.
  - Packages follow GraphicsDX11's layout (`LibXLT/GraphicsVulkan/shdw/`, its `private/`); includes are guarded.
  - No `d3d11.h`, `dxgiformat.h`, DirectXTex or WIC in GraphicsVulkan.
- **Ported code** keeps DX11's structure, names, comments and `D3DPERF` points (as `vkd` debug labels where Plan 3's code already does). Each D3D call becomes its `vkdContext` / `vkdImage` / `vkdBuffer` / `g3dVkUtil` equivalent. A behaviour difference needs a ledgered ruling.
- **Markers:** every `G3D_PROFILE_SCOPE` in the DX11 file is in the Vulkan file, at the same point and with the same name, so the two backends' CSV paths line up (`Target/HDRRenderer/PassSSAO/PassDepth`, ...). The name is the class name without its package prefix and without `DX11`/`Vk`.
- **Validation:** zero errors and zero warnings (core and synchronization) in every test and run. `vkdValidation`'s notice list grows only with a ledgered ruling and a written reason.
- **Image tolerance:** the spec's. Max channel error ≤ 2/255, and at most 0.1% of pixels over 1/255, on all four channels. Loosen it only per test case, with the measured error and a written reason in that test's exception table.
- **Tests** follow the speed rules from the test speed-up:
  - group a test's cases under one `gtBackend::Run` per backend;
  - share a device per suite where a suite has many small tests;
  - a new test project is referenced from `GraphicsTests.vcxproj` (none is planned).
- Git: branch `claude/graphics-vulkan`; stage explicit paths only; no push. Commit messages end with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.

## Review Focus

1. **The window or capture size changes between frames.** `shdwFrameBufferMgr` recreates its surfaces, and the old ones must not be freed while a frame in flight still reads them. A render after a resize must match DX11 at the new size. Task 2 tests `ResizeBetweenFramesMatchesDX11`: 256², 200×150, 256², then 0×0 skipped.
2. **A target read in the same pass that writes it.** Examples: `CopyBackBufInto` reading the current target, glow/outline/post shader chains bouncing between scratch textures, SSAO writing several depth targets at once. D3D11 unbinds a resource bound both ways; Vulkan needs the barrier. Every test runs under synchronization validation. Task 5 tests `EveryPostEffectAtOnceMatchesDX11` with glow, outline, every post effect and matte on together.
3. **Lights the scenes rarely have.** No lights at all; more lights than the light manager's per-pass limit; a projected light whose shadow map was never rendered. These must look as on DX11, not black, NaN or white. Task 2 tests `NoLightsAndManyLightsMatchDX11`; Task 3 tests `ShadowMapNeverRenderedMatchesDX11`.
4. **Reflectors.** Two reflective objects that see each other, a reflector outside the view, and a cube map made once against one made per frame. Each reflection is another pass over the scene (the spec's known hot spot). Task 6 tests `SeveralReflectorsMatchDX11` and prints each backend's GPU time per reflection renderer.
5. **Preference combinations MachStudio can set.** Every optional pass off; HDR anti-aliasing on with transparency; single-light rendering off (material swatches render that way, `mtrlIconUtil`); each HDR debug mode. Task 2 tests `PassesOffMatchDX11`, `AmbientOnlyBranchMatchesDX11` and `DebugModesMatchDX11`; Task 4 tests `HDRAATransparencyMatchDX11`.

---

## File Structure

| Path (under `LibXLT/GraphicsVulkan/` unless stated) | Responsibility |
|---|---|
| `shdw/shdwPackage.hpp`, `private/shdwPackage.cpp` | `Init`/`CleanUp`/`InitGraphics`/`CleanUpGraphics`, the `InitStates`/`CleanupStates` lists of DX11's (for the classes ported so far) |
| `shdw/shdwSceneRendererCreate.hpp/.cpp` | The creator (decision 2) |
| `shdw/shdwSceneRendererVk`, `shdwPassTraversal`, `shdwFrameBufferMgr`, `shdwVelocityStateManager`, `private/GlareDefVk` | Base renderer, node database, frame buffers, the HDR renderer's velocity member (compiled-out pass), star definitions |
| `shdw/shdwHDRRendererVk` | The HDR renderer |
| `shdw/shdwPass*.hpp/.cpp` | Passes: ZFill, Ambient, Environment, EnvBackground, Lit, AlphaFill, Depth, ToneMap (Task 2); Transparent (4); SSAO, Glow, Outline, PostShader (5); `shdwPassHair` as a stub (decision 5) |
| `shdw/shdwShadowLayerRendererVk`, `shdwPlanarReflectionRendererVk`, `shdwCubeMapRendererVk` | Simple renderer (with `g3dRenderWorldSpaceObjects`, `g3dRenderSkyBox`) and the reflection renderers |
| `G3d/g3dRenderPass.hpp/.cpp` | `g3dRenderPass`, `g3dRenderLayer`, `g3dRenderFullScreenQuad`, `g3dRenderAlpha`, `g3dRenderBlur`, `g3dRenderGlow`, `g3dRenderOutline`, `g3dRenderDOF`, `g3dRenderAA`, `g3dRenderText` |
| `G3d/g3dDepthMapRendererVk` | Shadow-map depth renderer |
| `Mat/matShadowMap` | Shadow-map texture and render target; `matTextureMgrVk::CreateShadowMap` |
| `vkd/vkdOcclusionQuery` | Decision 4 |
| `stub/` | The shadow-map stub and the scene-renderer fallback for types ported here go |
| `UnitTests/GraphicsTests/gtRender.hpp/.cpp` | Render-and-compare helpers (decision 6) |
| `UnitTests/GraphicsTests/gtScene.*` | Grows: HDR textures, global environment, projected lights with shadow maps, reflective materials |
| `UnitTests/GraphicsTests/Vk/shdw*Test.cpp`, `vkdOcclusionQueryTest.cpp` | Tests per task |

---

### Task 1: `shdw` scaffolding, `g3dRenderPass`, test helpers

**Files:**
- Create: in `shdw/`, `shdwPackage`, `shdwSceneRendererCreate`, `shdwSceneRendererVk`, `shdwPassTraversal`, `shdwFrameBufferMgr`, `shdwVelocityStateManager` and `private/GlareDefVk`; and `G3d/g3dRenderPass`.
- Modify:
  - `GraphicsVulkanLayer.cpp`: `shdwPackage` init and clean-up where `GraphicsDX11Layer` has them;
  - `GraphicsVulkan.vcxproj(.filters)`.
- Test: create `UnitTests/GraphicsTests/gtRender.hpp/.cpp` and `Vk/shdwScaffoldingVkTest.cpp`. Modify `Vk/g3dSceneRendererVkTest.cpp` (uses `gtRender`; its local helpers go).

**Interfaces — Produces:**
- `typedef std::function<void(gtSceneBuilder&, const std::filesystem::path&)> gtSceneFunction;` (was `SceneFunction` in `g3dSceneRendererVkTest`).
- `struct gtRenderOptions`:
  - `int m_Width = 256, m_Height = 256;`
  - `std::function<g3dSceneRenderer*()> m_CreateRenderer;` (empty: `g3dSceneRendererCreate::CreateDefaultRenderer`)
  - `std::function<void(g3dRenderPrefs&)> m_Prefs;` (edits the defaults for the run)
  - `bool m_bSingleLight = true;`
  - `int m_Frames = 1;` (renders this many frames, keeps the last; tone-map adaptation and reflections need more than one)
  - `float m_SimTime = 1.25f;`
  - `std::function<void(gtSceneBuilder&)> m_BeforeFrame;` (shadow maps and reflections, Tasks 3 and 6)
- `gtPicture gtRenderScene(const gtSceneFunction& i_Build, const std::filesystem::path& i_Dir, const gtRenderOptions& i_Options);` renders on the backend that is up (clear 0,0,0,0).
- `void gtExpectSameImage(const char* i_pTest, const gtSceneFunction& i_Build, const gtRenderOptions& i_Options = gtRenderOptions());` covers both backends, the coverage check, compare, and saving on failure.
- `struct gtCase { std::string m_Name; gtSceneFunction m_Build; gtRenderOptions m_Options; };`
- `void gtExpectSameImages(const char* i_pTest, const std::vector<gtCase>& i_Cases);` runs all cases under one `gtBackend::Run` per backend and reports each case as `<test>/<case>`.
- `class gtPrefsScope`: saves `g3dPrefs::CurrentPrefs()` and the `g3dSingleLightRendering` flags, and restores them in its destructor.
- `gvk::shdwFrameBufferMgr` has DX11's accessors (`HDRRenderTargetTex()`, `HDRAATarget()`, `DepthBuffer()`, `MultiDepthBuffer()`, `TexToneMap()`, ...).
- `gvk::shdwPassTraversal` has DX11's lists (`GetShadowNodes()`, `GetNonShadowNodes()`, `GetNonSolidNodes()`, ...).

- [ ] **Step 1: Confirm decision 3.** Grep MachStudio, Tool, Graphics and GraphicsDX11 for each class in the table (and `CreateDeferredContext`). Expected: no reachable use. Record the result in the ledger. A reachable use moves that class into this plan.
- [ ] **Step 2: Failing tests (`shdwScaffoldingVkTest`):**
  - `FrameBufferSurfacesMatchDX11`: `CreateSurfaces` on a 256² target and a 1920×1080 one, with overscan 0 and 16, and HDR AA off and on. Every surface's width, height, format and sample count must equal DX11's.
  - `TraversalListsMatchDX11`: a scene with opaque, transparent, non-solid and shadow-casting nodes and a hidden one. Each list has the same nodes in the same order (compared by node pointer index in the builder).
  - `RenderLayerMatchesDX11`: a second layer drawn with `g3dRenderLayer` over a cleared target, compared.
  - `g3dSceneRendererVkTest` still passes through `gtRender`.
- [ ] **Step 3:** Build; fail.
- [ ] **Step 4:** Implement.
  - The creator answers every type with the forward fallback for now: Task 2 onwards switch types over.
  - `g3dRenderPass` is copied whole, apart from the classes decision 3 leaves out.
- [ ] **Step 5:** Pass; validation 0/0.
- [ ] **Step 6:** Commit — "GraphicsVulkan: shdw package, creator, traversal, frame buffers, render passes".

---

### Task 2: The HDR renderer, opaque passes and tone mapping

**Files:**
- Create in `shdw/`:
  - `shdwHDRRendererVk` (all of `Render`, `PostProcessing`, `CopyToBackBuf`, `CopyBackBufInto`, `IncludeAlphas`, `LuminanceToGrayscale`, `DrawAlpha`; DOF is left for 5b, see Step 3);
  - the passes `shdwPassZFill`, `shdwPassAmbient`, `shdwPassEnvironment`, `shdwPassEnvBackground`, `shdwPassLit`, `shdwPassAlphaFill`, `shdwPassDepth` (eye-space depths into one or several targets; the frame's `PassDepth` marker, and transparency and SSAO use it) and `shdwPassToneMap` (scaled scene, luminance, adaptation, bright pass, star source, bloom source, bloom, star, tone map);
  - `shdwPassHair` (stub, decision 5).
- Modify:
  - `shdwSceneRendererCreate`: default and HDR are real;
  - `Mat/private/matRenderTargetTexture.cpp`: `CopyWithResolve`'s scaled copy and depth copy through `g3dVkUtil::CopyTexToTarget` / `CopyDepth`, removing both `GVK_NOT_YET`s. The HDR AA path needs them.
- Test: modify `gtScene.hpp/.cpp`, and create `Vk/shdwHDRRendererVkTest.cpp`.

**Interfaces — Produces:**
- `matTexture* gtScene::HdrTexture(const std::filesystem::path& i_Dir, int i_Width, int i_Height, float i_Peak);`: a lat-long RGBE `.hdr` with a gradient sky and a sun spot of value `i_Peak`, loaded through `matTextureMgr`.
- `void gtSceneBuilder::GlobalEnvironment(matTexture* i_pDiffuse, matTexture* i_pSpecular, bool i_bBackground);` sets the scene's `g3dAmbientEnvState` (`m_bEnableSwlEnv`, `m_bEnableSwlEnvBG`) as `swl` does.
- `void gtSceneBuilder::AddLayer(const gtMesh&, matMaterial*, const maMatrix4x4&)`: a node in a second (overlay) layer.

- [ ] **Step 1: Failing tests (`shdwHDRRendererVkTest`):** each through `gtExpectSameImage`/`gtExpectSameImages` with the default renderer.
  - `LitSpheresMatchDX11`: 4b's twelve spheres (Blinn, Phong, Lambert, built-in Phong × non-default, zero, huge), point and directional light.
  - `GlobalEnvironmentMatchDX11`: the same spheres lit by `HdrTexture` diffuse and specular maps, background off.
  - `EnvironmentBackgroundMatchDX11`: background on.
  - `BloomAndStarMatchDX11`: a peak of 50 in view, with 3 frames for luminance adaptation.
  - `AlphaMatchesDX11`: spheres over the transparent clear. Alpha is part of the comparison: `IncludeAlphas` and the alpha fill.
  - `HDRAAMatchDX11`: `m_bHDRAA` on, so MSAA, resolve and depth copy are used.
  - `DebugModesMatchDX11`: `m_HDRDebugMode` 1, 2, 3, 5, 6, 7, 8, 9 (each intermediate buffer shown), one case each. Mode 4 needs DOF (5b) and mode 10 needs SSAO (Task 5).
  - `AmbientOnlyBranchMatchesDX11`: single-light rendering off (Review Focus 5).
  - `PassesOffMatchDX11`: environment, lit pass, transparency, glow, outline, SSAO and reflections all off (Review Focus 5).
  - `NoLightsAndManyLightsMatchDX11`: zero lights, and 12 point lights (Review Focus 3).
  - `OverlayLayersMatchDX11`: a second scene layer, and a viewer layer passed to `Render`.
  - `ResizeBetweenFramesMatchesDX11`: one renderer used at 256², 200×150, 256², then 0×0 skipped. Compared at each size; validation 0/0 (Review Focus 1).
  - `g3dSceneRendererVkTest.UnportedTypesFallBack`: the list loses default and HDR.
- [ ] **Step 2:** Build; fail.
- [ ] **Step 3:** Implement.
  - The renderer is copied whole. The passes it calls that later tasks or Plan 5b port get stand-ins in `stub/shdwPassStubsVk.hpp/.cpp`, with DX11's constructors and `Render` signatures. Each stand-in draws nothing and logs `GVK_NOT_YET: <class>` once. The stand-ins are Transparent, SSAO, Glow, Outline, PostShader, Fog, DOF, AOVolumes, SSGI and LPVGI, and `RenderDOF` with them. Each later task deletes its stand-ins.
  - `shdwPassToneMap`'s scissor rects become `vkdContext::SetScissorRects`.
  - Its `ClearRenderTargetView`s become `ClearRenderTarget`.
  - `PSSetShaderResources` becomes `SetShaderResource`.
  - The SRV `CopyBackBufInto` makes of the current target is that target's `vkdImage`.
  - `LuminanceToGrayscale`'s `D3D11_FILTER` parameter becomes the sampler-state value Plan 3's state holders use.
- [ ] **Step 4:** Pass. A failing case is fixed, or excepted with its measured error and a reason.
- [ ] **Step 5:** Commit — "GraphicsVulkan: HDR renderer, opaque passes, tone mapping, bloom and star".

---

### Task 3: Shadow maps

**Files:**
- Create: `Mat/matShadowMap` (DX11's formats and depth buffer), `G3d/g3dDepthMapRendererVk`.
- Modify:
  - `matTextureMgrVk::CreateShadowMap` (remove its `GVK_NOT_YET`);
  - `shdwSceneRendererCreate::CreateDepthMapRenderer`;
  - `g2dResourceCounterVk` stays a stub (Plan 6).
- Test: modify `gtScene.*`; create `Vk/shdwShadowVkTest.cpp`.

**Interfaces — Produces:**
- `g3dProjectedLight* gtSceneBuilder::ProjectedLight(const maPoint3d& i_Position, const maPoint3d& i_Target, const maFloatRGBA& i_Color, int i_ShadowMapSize);` (0: no shadow). It creates the shadow map with `matTextureMgr::CreateShadowMap`, a `CreateDepthMapRenderer` renderer and a `g3dTargetRenderer` with the light's shadow camera, as `prjltProjectedLightObject` does.
- `void gtSceneBuilder::RenderTargets();` renders the builder's target renderers (shadow maps now, reflections in Task 6) for the current frame. It is used as `gtRenderOptions::m_BeforeFrame`.

- [ ] **Step 1: Failing tests:**
  - `ShadowMapDepthsMatchDX11`: the shadow map read back after `RenderTargets`. Every texel within 1e-6 of DX11's.
  - `ProjectedLightShadowsMatchDX11`: spheres over a plane, a 1024² shadow map. Cases: dithered shadows off and on (`SetupDitheredShadows`), and projected-light frustum culling on.
  - `ShadowMapNeverRenderedMatchesDX11`: the map created, `RenderTargets` not called (Review Focus 3).
- [ ] **Step 2:** Build; fail.
- [ ] **Step 3:** Implement.
- [ ] **Step 4:** Pass.
- [ ] **Step 5:** Commit — "GraphicsVulkan: shadow maps and the depth map renderer".

---

### Task 4: Transparency

**Files:**
- Create: `vkd/vkdOcclusionQuery.hpp/.cpp` (decision 4) and `shdw/shdwPassTransparent`.
- Modify:
  - `shdwPassAlphaFill` (its transparent-pass info, ported in Task 2, is exercised here);
  - `vkd/private/vkdDevice.cpp` (`hostQueryReset` in the required features, named on failure like the others).
- Test: create `Vk/vkdOcclusionQueryTest.cpp` and `Vk/shdwTransparentVkTest.cpp`.

**Interfaces — Produces:** `class vkdOcclusionQuery`:
- `explicit vkdOcclusionQuery(vkdDevice&);`
- `void Begin(vkdContext&);`
- `void End(vkdContext&);`
- `uint64_t GetResult(vkdContext&);` flushes, waits, and returns the samples passed.

- [ ] **Step 1: Failing tests:**
  - `vkdOcclusionQueryTest.CountsSamples`: a 10×10 quad on a 64² target gives 100. The same quad behind a nearer depth gives 0. Reused 3 times. Validation 0/0.
  - `TransparentSortedMatchDX11`: overlapping transparent Phong spheres in front of opaque ones, `m_TransparencyMode` 0.
  - `TransparentDepthPeeledMatchDX11`: mode 1, the same scene plus intersecting transparent planes.
  - `TransparentDeferredMatchDX11`: `m_bEnableDeferredTransparency` on.
  - `HDRAATransparencyMatchDX11`: HDR AA on, sorted and peeled (Review Focus 5).
- [ ] **Step 2:** Build; fail.
- [ ] **Step 3:** Implement.
- [ ] **Step 4:** Pass.
- [ ] **Step 5:** Commit — "GraphicsVulkan: transparency (sorted, depth-peeled, deferred) and occlusion queries".

---

### Task 5: SSAO, glow, outline, post effects, matte

**Files:**
- Create in `shdw/`: `shdwPassSSAO` (multi-horizon AO, bilateral blur, overscan, depth peeling, its `CleanUp`), `shdwPassGlow`, `shdwPassOutline`, `shdwPassPostShader`.
- Modify: `shdwHDRRendererVk` (its matte branch), `shdwPackage` (the new `InitStates`; `shdwPassSSAO::CleanUp` in `CleanUpGraphics`).
- Test: create `Vk/shdwScreenPassesVkTest.cpp`.

- [ ] **Step 1: Failing tests:**
  - `SSAOMatchDX11`, with cases:
    - blur off and on;
    - SSAO depth peeling off and on;
    - overscan 0 and 16;
    - debug mode 10 (the AO buffer).
  - `GlowMatchDX11`: a glow material (glow colour and intensity, and a glow map from `CheckerTexture`).
  - `OutlineMatchDX11`.
  - `PostEffectsMatchDX11`: one case each for BlackAndWhite, GradientMap, Sepia and Sketch, through `g3dPostProcessing`.
  - `MatteMatchesDX11`: `m_bRenderMatte` on, with glow on (glow skips the matte; bloom and star are drawn as grey).
  - `EveryPostEffectAtOnceMatchesDX11`: SSAO, glow, outline, a post effect and transparency on together (Review Focus 2).
- [ ] **Step 2:** Build; fail.
- [ ] **Step 3:** Implement.
- [ ] **Step 4:** Pass.
- [ ] **Step 5:** Commit — "GraphicsVulkan: SSAO, glow, outline, post effects and matte".

---

### Task 6: The simple renderer and reflections

**Files:**
- Create in `shdw/`: `shdwShadowLayerRendererVk` (with `g3dRenderWorldSpaceObjects`, `g3dRenderSkyBox`), `shdwPlanarReflectionRendererVk` and `shdwCubeMapRendererVk`.
  - Both reflection renderers draw through the simple renderer, as on DX11.
  - Their `GenerateMips` becomes `vkdImage::GenerateMips`.
- Modify: `shdwSceneRendererCreate` (simple, cube map and planar reflection are real).
- Test: modify `gtScene.*`; create `Vk/shdwReflectionVkTest.cpp`.

**Interfaces — Produces:** `void gtSceneBuilder::Reflective(g3dSceneNode* i_pNode, bool i_bPlanar, bool i_bPerFrame);`
- gives the node's material a reflection map;
- creates the reflection renderer (`CreatePlanarReflectionRenderer` / `CreateCubeMapRenderer`) and its `g3dPlanarReflectionTargetRenderer` / `g3dCubicReflectionTargetRenderer`, as `mtrlScriptObject` does (black background);
- registers it for `RenderTargets()`.

- [ ] **Step 1: Failing tests:**
  - `SimpleRendererMatchesDX11`: `CreateSimpleRenderer` on the lit spheres, with the environment on.
  - `PlanarReflectionMatchDX11`: a reflective floor (BlinnReflection) under spheres, 2 frames.
  - `CubeMapReflectionMatchDX11`: a reflective sphere among others. Its cube map's six faces and their mips are compared too, and the final image. Cases: made once, and per frame.
  - `SeveralReflectorsMatchDX11` (Review Focus 4): a planar floor, two cube-mapped spheres that see each other, and a planar reflector behind the camera.
    - The image is compared.
    - Each backend's `g3dProfiler` GPU times per reflection renderer are printed, with no assertion; the report quotes them. This needs markers compiled in: Debug, or Release with `MspGfxProfiling=true`. Without them the test prints that timings are off.
- [ ] **Step 2:** Build; fail.
- [ ] **Step 3:** Implement.
- [ ] **Step 4:** Pass.
- [ ] **Step 5:** Commit — "GraphicsVulkan: simple renderer, planar and cube-map reflections".

---

### Task 7: Every material through the HDR renderer

**Files:**
- Test: create `Vk/shdwMaterialsVkTest.cpp`. No library changes are expected. A failure is fixed where it comes from (pass, effect class or state), with a ruling if it's in ported code.

- [ ] **Step 1: Failing tests:**
  - `EveryMaterialMatchesDX11` (one `gtExpectSameImages`, one case per material). The cases are every material shader `matShaderMgr` registers from `Eff/private/Shaders/Materials`, except:
    - `Hair_SH` (Plan 7);
    - `Bake` (Plan 6's baking renderer; it is a baking effect, not a beauty material).

    That is Anisotropic, Blinn, BlinnReflection, BlinnSkin, CarPaint, Cartoon, Default, the FC3D variants, Lambert, Phong, PhongReflection, Phong_wBump, Simple, Skin, SpecularFresnel, the SubSurfaceScatter variants and testtess.

    Each case:
    - a 3×3 grid of spheres with `e_NonDefault` values;
    - `CheckerTexture`s on every texture parameter (a normal-map-like texture where the parameter is a normal map);
    - a point light, a directional light, a shadowed projected light and the global environment;
    - one row each with `e_Zero` and `e_Huge`.

    The test fails if a material registered on DX11 is missing on Vulkan or the other way round.
  - `TessellatedMaterialsMatchDX11`: testtess and the displacement-mapped materials, with `m_bUseHardwareTessellation` off and on.
- [ ] **Step 2:** Run; record failures.
- [ ] **Step 3:** Fix each failing case.
- [ ] **Step 4:** Pass.
- [ ] **Step 5:** Commit — "GraphicsVulkan: every material shader matches DX11 through the HDR renderer".

---

### Task 8: The test scene on Vulkan; docs; stop for review

**Files:**
- Modify `CLAUDE.md`:
  - the GraphicsVulkan table gets a `shdw` row;
  - `stub` and the renderer note change: the forward stand-in is left only for Plan 6's types.
- Modify the roadmap: Plan 5a done, with results and notes for 5b.
- Modify the Plan 6 stub: the simple and depth-map renderers are done here; the GI volumes and velocity pass move there (decision 3).

- [ ] **Step 1: Short parity run, Release.** First build Release RenderNode with `/p:MspGfxProfiling=true`, so the runs write timings (gfxcompare's README; each run's `timing.csv`). Then render frames 296–315:

  ```
  gfxcompare.py run --exe MachStudio/MachStudio-x64/MachStudio-x64-RenderNode-Release.exe --scene <testTESS> --out <dir> --a-args "-gfx dx11" --b-args "-gfx vulkan -gfxValidation" --start-frame 296 --end-frame 315
  ```

  Expected:
  - 20/20 within tolerance;
  - Vulkan validation 0/0;
  - both adapters the Quadro;
  - no `GVK_NOT_YET` from a `shdw` stand-in in the Vulkan log, because the scene's frames run none of 5b's passes. A stand-in that does show up moves that pass into this plan.
  - A failing frame is investigated with its diff image and the HDR debug modes (Task 2) to find the pass. The pass then gets a synthetic case that reproduces it, before it is fixed.
- [ ] **Step 2: Full parity run, Release.** All 135 frames, the same command without the window. Expected: 135/135 and validation 0/0.
- [ ] **Step 3: Debug validation run.** RenderNode Debug, `-gfx vulkan`, frames 296–300: validation 0/0 (core and synchronization).
- [ ] **Step 4: Timing.** Use the per-scope medians (B/A) in Step 2's `report.md`, from the profiling Release build (the report says so). Report:
  - frame totals;
  - every `HDRRenderer/*` pass;
  - `PlanarReflection/*`, called out as the hot spot.
- [ ] **Step 5: MachStudio by hand, Debug Qt and Release Qt, `-gfx vulkan`.**
  - Open the test scene: the viewport shows it through the HDR renderer.
  - Resize, change the panel layout, minimize and restore through the window handle only, as the GUI-check rule says. Ask the owner to orbit and play back, or report those as unchecked.
  - Quit. The log shows `Vulkan validation: 0 errors, 0 warnings`.
  - Record the `GVK_NOT_YET` lines.
- [ ] **Step 6:** Six-configuration build, all tests passing. Then the DX11 harness: DX11 against Plan 1's frames, 135/135.
- [ ] **Step 7:** Edit the docs; commit — "GraphicsVulkan: docs and roadmap: Plan 5a done".
- [ ] **Step 8: Stop for review.** Report:
  - what was built;
  - parity per test, with any exceptions;
  - the test-scene results;
  - the timing table, reflections called out;
  - validation;
  - the remaining `GVK_NOT_YET` list;
  - rulings and deferred minors.

  Don't start Plan 5b until the owner says so.
