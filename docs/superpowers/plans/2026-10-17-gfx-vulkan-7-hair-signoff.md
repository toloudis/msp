# GraphicsVulkan hair and Phase 1 sign-off — Implementation Plan (Phase 1, Plan 7)

**Status:** implemented (2026-10-22; `dae13d18`..the docs commit); hair matched, not deferred. Waiting for the owner's Phase 1 sign-off. Report: `docs/superpowers/reports/2026-10-22-gfx-vulkan-phase1.md`; rulings in the ledger.

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task by task, inline in the session (the owner asked for no subagents). Steps use checkbox (`- [ ]`) syntax for tracking. As in 5a, 5b and 6, **stop for the owner's review after each task**. The owner clears context between tasks, so resume from the plan's ledger (`.superpowers/sdd/2026-10-17-gfx-vulkan-7-hair-signoff/progress.md`). Read the 5a, 5b and 6 ledgers' rulings first. When the plan is done, stop: Phase 1 sign-off is the owner's call.

**Goal:** Hair renders on Vulkan and matches DX11 (or is formally deferred, by the rule in decision 1). The test scene passes. Validation is clean in every mode, the timing report is written, and Phase 1's exit criteria are shown and written up.

**Architecture:** The plan has three parts:
- **The open parity gap first** (Task 1). The anisotropic investigation from the roadmap's "Open investigations" decides whether the test scene can pass at all.
- **Hair** (Tasks 2–6), bottom up:
  - structured-buffer shader inputs in `vkd`;
  - the hull shader's isoline tessellation factors, in a Vulkan-only file;
  - hair fragments and the hair renderer;
  - `shdwPassHair`;
  - the opacity map renderer, whose maps only hair reads.

  Each step is ported from DX11 and changed only where it touches D3D, as before.
- **Sign-off** (Tasks 7–10):
  - the unconsumed-output warning the owner asked to fix at the end of the port;
  - validation in every mode;
  - the test-scene runs and the timing report;
  - the docs.

**Tech Stack:** C++17 (MSVC x64), Vulkan 1.4 C API via volk, VMA, DXC; SPIRV-Tools (new, vcpkg `spirv-tools`, Task 7); GoogleTest; Python (`Tools/gfxcompare`).

**Spec:** `docs/superpowers/specs/2026-10-17-graphics-vulkan-design.md`:
- "Decided" 4 (hair), 9 (Phase 1 exit) and 10 (proof of parity);
- "Shaders" (the hair exception);
- "Proving parity";
- "Validation";
- "Phase 2 direction".

Also:
- Roadmap: `docs/superpowers/plans/2026-10-17-graphics-vulkan-roadmap.md` (Plan 6 status; "Open investigations").
- From Plan 6:
  - every renderer but the opacity map;
  - the `-cap*` flags;
  - headless RenderNode;
  - `fxEffectVk`'s image-destroy hooks;
  - the test helpers `gtRenderOptions`, `gtExpectSameImage(s)`, `gtExpectEffectShows`, `gtPrefsScope`, `gtReadTexture`, `gtSceneBuilder::ProjectedLight` / `RenderTargets` / `LightsCastShadows`, `gtScene::CheckerTexture(..., i_Blur)`.

## Decisions made in this plan (please review)

1. **The hair hull shader: a Vulkan-only wrapper, and a rule for deferring.**
   - **The problem.** `Hair_HS` (in `SupportHair.hlsli`, used by `Hair_SH`, `HairDefault` and `OpacityRender`) calls `ProcessIsolineTessFactors`, which DXC's SPIR-V backend doesn't implement. It is the only failing stage.
   - **New files** live in `GraphicsVulkan/Eff/private/ShadersVk/`, outside the byte-identical copy that `fxShaderCopyTest` checks:
     - `IsolineTessFactors.vulkan-only.hlsli` defines `gvkProcessIsolineTessFactorsFractionalOdd`, ported from Microsoft's reference tessellator. That is `CHLSLTessellator::IsolineHLSLProcessTessFactors` for `fractional_odd`, MIT-licensed, in Mesa's `src/gallium/auxiliary/tessellator/`; DXC's own DXIL lowering of the intrinsic is a second reference. The file then has `#define ProcessIsolineTessFactors gvkProcessIsolineTessFactorsFractionalOdd`.
     - One wrapper per effect, `<Effect>.vulkan-only.hlsl`. Each includes the header and then the unchanged `../Shaders/<path>.hlsl`.
   - **How it is built.** `EmbedShadersVk.ps1`'s `$Deferred` list becomes a `$VulkanOnlySource` map ("`Materials/Hair_SH.hlsl|Hair_HS`" → its wrapper). Only the `Hair_HS` stage is compiled from the wrapper; every other stage still comes from the copy.
   - **Why only `fractional_odd`.** `Hair_HS` is the only caller, and it is `fractional_odd`. The function name says so.
   - **What must match exactly.** The call passes `.y` (interpolation count) as the "detail" factor and `.x` (tessellation) as the "density" factor, into `Edges[0]` and `Edges[1]`. That order is kept as is, since it is what DX11 draws.
   - **Defer instead** if either of these happens; the owner decides:
     - (a) Task 3's raw test can't match D3D11 exactly with a function of about 60 lines or fewer;
     - (b) DX11's own hair doesn't render in GraphicsTests (Task 4 Step 1), which leaves nothing to match.

     Deferring means: the hair renderer draws nothing and logs once "Hair isn't drawn on Vulkan (deferred: <reason>); use -gfx dx11". The reason goes in the roadmap, and Tasks 4–6 shrink to the opacity map renderer for non-hair geometry.
2. **Structured buffers bind through the resource heap.** A `StructuredBuffer` at `tN` (the hair's `g_HairMaterials` and `g_HairGeometry`) becomes a read-only storage-buffer descriptor in the same resource heap as images. Its heap index goes into the same `tN` table slot. `vkdBindings`' mapping for t-bindings (`HEAP_WITH_INDIRECT_INDEX`) doesn't change. The heap's stride becomes the larger of the image and buffer descriptor sizes.
3. **Unconsumed shader outputs are removed when a pipeline is made.** The owner decided on 2026-10-18 to fix `WARNING-Shader-OutputNotConsumed` at the end of the port.
   - **How.** `vkdPipelineCache` already has each pipeline's stages together. For each pair, last pre-rasterization stage → pixel shader, it runs SPIRV-Tools' `CreateAnalyzeLiveInputPass` on the pixel shader. Then it runs `CreateEliminateDeadOutputStoresPass`, `CreateRemoveUnusedInterfaceVariablesPass` and dead-variable elimination on a copy of the producer. The trimmed module is cached by the (producer, consumer) pair and passed inline (`VkShaderModuleCreateInfo` chained into the stage, maintenance5).
   - **Why it isn't a shader edit.** The HLSL and the embedded SPIR-V don't change. This is a link step, as D3D11 does implicitly.
   - **The test.** Images must be bit-identical before and after, and the info-level exception in `vkdValidation` goes.
   - **If SPIRV-Tools can't do it cleanly**, the info-level exception stays, with the reason in the code and the roadmap. The owner decides.
4. **Extra validation modes come from an environment variable.** `MSP_GFX_VK_VALIDATION`, a comma list of `gpu` (GPU-assisted) and `bestpractices` (Khronos and NVIDIA best practices), is read wherever `MSP_GFX_VK_ADAPTER` is read (`GraphicsVulkanLayer`, `gtVulkan`, `gtVulkanSuite`), through one function in `vkd`. Either mode also turns validation on.
   - **Best-practices messages are counted on their own** and don't fail anything, as the spec says. The exit line becomes `Vulkan validation: N errors, M warnings, P best-practices notes`. gfxcompare's parser accepts both forms.
   - **No new command-line flag:** these are sign-off and test tools, not user options.
5. **The Phase 1 report is Markdown in the repo** (the stub's open question): `docs/superpowers/reports/2026-10-xx-gfx-vulkan-phase1.md`, with the timing tables made by gfxcompare. Publishing it as a page is offered at the end, not done.
6. **What tolerance the test scene is held to depends on Task 1's outcome**, as the roadmap's investigation plan says:
   - If the gap is ours: fix it, and use the normal tolerance.
   - If it is the driver: a documented, per-feature tolerance for textured frames ("loosen per feature only with a written reason", spec), with the raw test as evidence.

   **Outcome (Task 1):** D3D11 and Vulkan compute a different level of detail for textures seen at an angle (roadmap, "Open investigations"). The owner ruled it an API difference: tests use hard textures with a documented tolerance, and Task 9 gives gfxcompare a higher tolerance for textured frames (a perceptual metric such as FLIP is an option).

## Global Constraints

Plan 6's Global Constraints apply unchanged:
- no GraphicsDX11 or shader edits, except an owner-approved minimal fix (memory: propose it, ask, change both backends the same way, ledger it);
- the DX11 harness still matches the post-`6848f8d2` reference frames;
- `gvk`, `GVK_` guards and DX11's layout;
- no `d3d11.h`, DirectXTex or WIC in GraphicsVulkan;
- ported code keeps DX11's structure, and differences need ledgered rulings;
- profiler markers at the same points with the same names (`PassHair`, `PassOpacity`, `OpacityMapRenderer`);
- validation 0/0;
- the spec's image tolerance, loosened only per case with a reason;
- the test speed rules (share devices per suite, group cases under one `gtBackend::Run` per backend, make textures once);
- branch `claude/graphics-vulkan`, explicit staging, no push, the `Co-Authored-By` line.

Also:
- **Scene runs are short:** frames 296–315 for the first run of each kind, 296–300 for the rest (owner, Plan 6 Task 10). All 135 frames only when the owner asks; Task 9 asks once.
- **Vulkan-only shader code lives only in `Eff/private/ShadersVk/`**, in files named `*.vulkan-only.*` (spec "Shaders": hair is the one exception).
- **Any change to the DXC options or shader sources is made in all three places together:** `EmbedShadersVk.ps1`, the GraphicsTests shader step, and `compile_check.py --spirv` plus its CI line.

## Review Focus

1. **Hair tessellation factors at and past their limits.** `m_HairTessellation` 0, 0.5, 1, 2.5, 63.9, 64, 100, and `m_HairInterpolationCount` 0, 1, 64, 100, must give the same strands as DX11. Clamping and rounding go through the hand-written function and Vulkan's tessellator. Task 3 tests `IsolineFactorsMatchD3D11` (raw); Task 4 tests `HairTessLimitsMatchDX11`.
2. **Odd strand shapes.** One strand; strands with two control points (one segment, so every patch but the first is culled); and a hair object with more than 65,535 control points. Each must match DX11. Task 4 tests `HairOddShapesMatchDX11`.
3. **A hair object with no strands.** DX11 asserts when it makes a zero-size buffer, so this is checked on Vulkan only. There must be no crash, nothing drawn, and no validation message. Task 4 tests `EmptyHairDrawsNothing`.
4. **Hair settings changed between frames.** Tessellation on → off, lines on → off, a shadow type change that recreates a light's opacity maps, and an opacity map destroyed while bound (the image-destroy hooks). Each frame must match DX11, and validation must stay 0/0. Task 6 tests `HairPrefsChangeBetweenFramesMatchesDX11`.
5. **Morphed hair.** `UpdateVertices` changes `m_HairInfo` but DX11 never re-uploads its immutable buffers, so the strands don't move on screen. Vulkan must draw the same thing, which is DX11's behaviour and not a fix. Task 4 tests `UpdatedHairMatchesDX11`.

---

## File Structure

| Path (under `LibXLT/GraphicsVulkan/` unless stated) | Responsibility |
|---|---|
| `vkd/vkdBindings.hpp`, `vkd/private/vkdBindings.cpp`, `Fx/fxEffectApi.hpp`, `Fx/private/fxEffectVk.cpp` | Storage-buffer descriptors in the resource heap; `SetResource(vkdBuffer*)` (decision 2) |
| `Eff/private/ShadersVk/IsolineTessFactors.vulkan-only.hlsli`, `Hair_SH.vulkan-only.hlsl`, `HairDefault.vulkan-only.hlsl`, `OpacityRender.vulkan-only.hlsl`; `Fx/private/EmbedShadersVk.ps1` | The hull shader workaround (decision 1) |
| `hair/hairPackage`, `hair/hairModelFragVk`, `hair/HairRendererVk` | Hair fragments and renderer (replace `stub/g3dStubsVk`; `stub/` is deleted) |
| `may/private/mayFragCreate.cpp` | Real hair fragments |
| `shdw/shdwPassHair`, `shdw/shdwPassOpacity`, `shdw/shdwOpacityMapRendererVk` | The hair pass, the opacity pass and the opacity map renderer |
| `vkd/private/vkdPipelineCache.cpp`, `vkd/private/vkdLinkStages.{hpp,cpp}`, `vkd/private/vkdValidation.cpp`, `vcpkg.json` | Unconsumed outputs trimmed (decision 3) |
| `vkd/vkdDevice.hpp`, `vkd/private/vkdDevice.cpp`, `vkd/vkdValidation.hpp`, `vkd/private/vkdValidation.cpp` | `MSP_GFX_VK_VALIDATION`, best-practices count (decision 4) |
| `UnitTests/GraphicsTests/Vk/vkdAnisotropicTest.cpp`, `vkdIsolineTest.cpp`, `hairVkTest.cpp`, `shdwOpacityVkTest.cpp`, `vkdLinkStagesTest.cpp`; `Vk/Shaders/*` | Tests; `gtSceneBuilder::Hair` in `gtScene.{hpp,cpp}` |
| `Tools/fx2hlsl/compile_check.py`, `.github/workflows/build-windows.yml`, `Tools/gfxcompare` | Vulkan-only sources in the CI check; the new validation line |
| `docs/superpowers/reports/2026-10-xx-gfx-vulkan-phase1.md`, `CLAUDE.md`, the roadmap | Report and docs |

---

### Task 1: Anisotropic filtering, then the single-pixel outliers

This follows the roadmap's investigation plan ("Open investigations"), which is the authority for this task. This task adds the steps that turn it into code.

**Files:**
- Create: `UnitTests/GraphicsTests/Vk/vkdAnisotropicTest.cpp`, and `Vk/Shaders/AnisoQuad.hlsl` (VS and PS). The D3D11 side compiles the same file at run time with `D3DCompile`, as `vkdContextTest` does.
- Modify, by outcome:
  - either `fxEffectVk.cpp` / `vkdImage` view setup (ours);
  - or a test-only switch, `static bool fxSamplerOverrides::s_bAnisotropicAsTrilinear` (off by default), read in both sampler translations: `fxSamplerToVulkan` and DX11's `fxEffectDX11` sampler creation. This is a DX11 change, so ask first.
  - Then `shdwMaterialsVkTest` gets its hard checkers and normal tolerance back.

- [ ] **Step 1: Raw test.** `AnisotropicMatchesD3D11` draws one tilted quad with a hard 8×8 checker (RGBA8, full mip chain) on plain D3D11 (`gtD3D11`) and on plain Vulkan (`vkdContext`), with hand-made samplers. Cases: anisotropy {1, 2, 4, 8, 16} × tilt {60°, 80°} × mips {on, off}. It prints the max error and % over for each case and asserts nothing yet. Build and run it. Ledger the table.
- [ ] **Step 2: Owner check.** Ask the owner to set NVIDIA Control Panel → Texture filtering – Quality = High quality, with anisotropic sample optimization off. Then rerun Step 1 and `shdwMaterialsVkTest`, and ledger both.
- [ ] **Step 3: Decide (decision 6) and implement.** Then make Step 1's test assert:
  - **Raw APIs match, MachStudio doesn't:** bisect the image view and sampler setup against the raw test, fix it, and make Step 1 assert ≤ 1/255.
  - **Raw APIs differ:** add the trilinear switch to both backends, turn it on in the parity tests' fixtures, and make Step 1 assert the measured tolerance per case, as an exception table with the reason.
- [ ] **Step 4:** `shdwMaterialsVkTest` with hard checkers (`i_Blur` 0) and the normal tolerance: PASS. Full GraphicsTests Debug and Release: PASS, validation 0/0.
- [ ] **Step 5: Test scene.** `gfxcompare run` on `testTESS`, frames 296–315, DX11 against Vulkan (`-gfxValidation`). Expected: the anisotropic noise is gone (ours), or it is within the documented tolerance (driver). Ledger every frame that still fails.
- [ ] **Step 6: Single-pixel outliers** (5a: isolated pixels at 3–32/255, deterministic). For the three worst pixels:
  - find the draw that writes each one, with RenderDoc pixel history on both backends;
  - name the cause (alpha-test flip, a derivative at an edge, fxc and DXC float differences);
  - fix what is ours.

  If the rest is compiler float noise, propose a gfxcompare rule to the owner (for example "≤ N isolated pixels over the max per frame") and ledger the reason. Don't loosen anything without the owner's yes.
- [ ] **Step 7:** Update the roadmap's "Open investigations" with the outcome, and commit — "GraphicsVulkan: anisotropic filtering parity".

---

### Task 2: Structured buffers as shader inputs

**Files:**
- Modify:
  - `vkd/vkdBindings.hpp`, `vkd/private/vkdBindings.cpp`: `uint32_t AddBuffer(VkDeviceAddress i_Address, VkDeviceSize i_Size)` (a read-only storage-buffer descriptor via `vkWriteResourceDescriptorsEXT`; returns a resource-heap index). `RemoveImage` is renamed `RemoveResource` (it frees either kind; update its callers). The heap stride becomes `max(image, buffer descriptor size)`.
  - `Fx/fxEffectApi.hpp` / `Fx/private/fxEffectVk.cpp`: `virtual bool SetResource(vkdBuffer* i_pBuffer)` on the shader-resource variable. It fails (returns false, as DX11's type check does) on a texture variable, and the image overload fails on a buffer variable (`vkdReflect`'s `m_bBuffer`).
  - `fxEffectVk` registers an image-destroy-style hook for buffers, so a destroyed buffer reads as unbound: `vkdDevice::AddBufferDestroyHook`, as Plan 6 Task 10 did for images.
- Test: extend `Vk/vkdBindingsTest.cpp` and `Vk/fxEffectVkTest.cpp`; add `Vk/Shaders/StructuredRead.hlsl`.

**Interfaces — Produces:** `vkdBindings::AddBuffer`, `RemoveResource`; `fxShaderResourceVariable::SetResource(vkdBuffer*)`. Task 4's `hairModelFragVk` and `HairRendererVk` rely on them.

- [ ] **Step 1: Failing tests:**
  - `vkdBindingsTest.StructuredBufferReads`: a pixel shader reads `StructuredBuffer<float4>` at t3 and `StructuredBuffer<uint>` at t7 next to a texture at t0, and writes them to a 4×1 target. The values must be exact. Run with core, sync and GPU-assisted validation.
  - `fxEffectVkTest.BufferVariableBindsBuffer`: `SetResource(vkdBuffer*)` on `HairDefault`'s `g_HairGeometry` succeeds, and on a texture variable fails. A destroyed buffer is unbound (reads as zero), and validation is 0/0.
- [ ] **Step 2:** Build GraphicsTests (`-p:PostBuildEventUseInBuild=false`); run `vkdBindingsTest.*:fxEffectVkTest.*`. Expected: FAIL (no `AddBuffer`).
- [ ] **Step 3:** Implement.
- [ ] **Step 4:** Run the two suites, then the whole suite. Expected: PASS, validation 0/0.
- [ ] **Step 5:** Commit — "GraphicsVulkan: structured buffers as shader inputs".

---

### Task 3: Isoline tessellation factors (the Vulkan-only hull shader)

**Files:**
- Create: decision 1's four files in `Eff/private/ShadersVk/`; `UnitTests/GraphicsTests/Vk/vkdIsolineTest.cpp`; `Vk/Shaders/Isoline.hlsl`. That shader has a VS, a `[domain("isoline")] [partitioning("fractional_odd")] [outputtopology("point")]` HS calling `ProcessIsolineTessFactors(raw.y, raw.x, Edges[0], Edges[1])` as `Hair_HS` does, a DS, and a PS. On SPIR-V it includes the Vulkan-only header first (`#ifdef __spirv__`).
- Modify:
  - `Fx/private/EmbedShadersVk.ps1` (`$VulkanOnlySource`; the deferred table becomes empty, then `k_NumDeferred` and `fxEffectVk`'s `m_bDeferred` path are removed);
  - the GraphicsTests shader step (it must find `ShadersVk/`);
  - `Tools/fx2hlsl/compile_check.py`: `--vulkan-only-dir`, which compiles an entry from its wrapper when one exists, in place of `--exclude-entry Hair_HS`;
  - its CI line.

- [ ] **Step 1: Failing test** `IsolineFactorsMatchD3D11`. Raw factors: `x` (tessellation) {0, 0.5, 1, 1.3, 2, 2.5, 7.5, 63.9, 64, 100} × `y` (interpolation) {0, 1, 3, 64, 100}, plus one patch culled with factor −1. Each tessellated point is drawn by the DS at a pixel computed from (patch, u, v) into an RGBA32F target holding (u, v, patch, 1). The D3D11 and Vulkan targets must be equal (positions to 1e-6). This also confirms that DXC maps `SV_TessFactor[0]`/`[1]` to `TessLevelOuter[0]`/`[1]` with D3D11's isoline meaning. Build. Expected: the Vulkan side fails to compile or gives different points.
- [ ] **Step 2:** Port the function (decision 1) and wire the wrapper. Run. Expected: PASS. **If it can't match exactly, stop: this is decision 1's deferral rule (a). Ledger the table and ask the owner.**
- [ ] **Step 3:** Build GraphicsVulkan. Expected: `EmbedShadersVk: 60 effect(s), 0 deferred stage(s)`. `fxShaderCopyTest` still passes, since the copy is unchanged. `fxEffectVkReflectionTest` passes for the three hair effects with their `PTess`/`PLinesTess` passes included.
- [ ] **Step 4:** Run `python Tools/fx2hlsl/compile_check.py --compiler dxc --spirv --vulkan-only-dir LibXLT/GraphicsVulkan/Eff/private/ShadersVk ...` over GraphicsVulkan's effects. Expected: 756 of 756.
- [ ] **Step 5:** Commit — "GraphicsVulkan: Vulkan-only isoline tessellation factors for the hair hull shader".

---

### Task 4: Hair fragments and the hair renderer

**Files:**
- Create:
  - `hair/hairModelFragVk.{hpp,cpp}`: DX11's `hairModelFrag`, with its two structured buffers as `vkdBuffer`s (`STORAGE_BUFFER`) bound through Task 2.
    - `GetPrimitiveType()` returns the `vkdContext` topology: a 1-point patch list, or a line strip.
    - With no strands it makes no buffers, and `HairRendererVk` returns 0 (Review Focus 3).
  - `hair/HairRendererVk.{hpp,cpp}`: DX11's, with no input layout and a draw of `GetNumVertices()` vertices.
  - `hair/hairPackage.{hpp,cpp}` (DX11's, Vulkan types).
- Modify:
  - `GraphicsVulkanLayer.cpp` (`hairPackage::Init/CleanUp` in place of the stand-in);
  - `may/private/mayFragCreate.cpp` (`new hairModelFragVk(i_HairInfo, pMaterial)`; the `GVK_NOT_YET` goes).
  - Delete `stub/` (`g3dStubsVk.*`) and its `.vcxproj`/`.filters` entries.
- Test: `UnitTests/GraphicsTests/gtScene.{hpp,cpp}` gets `g3dSceneNode* gtSceneBuilder::Hair(const gtHairShape& i_Shape, const maMatrix4x4& i_World)`.
  - `struct gtHairShape { int m_nStrands; int m_nVerticesPerStrand; float m_Length; float m_Curl; float m_RootRadius; float m_TipRadius; maFloatRGBA m_RootColor; maFloatRGBA m_TipColor; float m_Opacity; }` describes the hair.
  - Strands grow from a hemisphere, are bent by a fixed gravity curve, and are seeded by `maFunctions::ResetRand`.
  - The material is `Hair_SH.fx`, set as `hairImport` sets it. The fragment comes from `mdlFragCreate::CreateHairFragment`.
  - Create `Vk/hairVkTest.cpp`.

**Interfaces:**
- Consumes: Task 2's `AddBuffer` / `SetResource(vkdBuffer*)`; Task 3's hull shader.
- Produces: `gvk::hairModelFragVk` (DX11's public API, Vulkan types: `vkdBuffer* GetHairMaterials()`, `GetHairGeometry()`) and `gtSceneBuilder::Hair`. Tasks 5 and 6 use them.

- [ ] **Step 1: DX11 guard first** (decision 1, rule b). `HairShowsOnDX11` renders 64 strands × 8 control points with the forward renderer, with `m_bEnableHair` on, and checks with `gtExpectEffectShows` that the hair changes the image. Run it. **If DX11 draws no hair, stop and ask the owner** (deferral).
- [ ] **Step 2: Failing tests.** Each runs through `gtBackend` with both layers and uses `gtExpectSameImage(s)`:
  - `HairMatchesDX11`: forward and simple renderers. Cases: hardware tessellation {off, on} × lines {off, on} (the four passes), tessellation 2.5 with interpolation 3, and a point light.
  - `HairTessLimitsMatchDX11` (Review Focus 1).
  - `HairOddShapesMatchDX11` (Review Focus 2).
  - `EmptyHairDrawsNothing` (Review Focus 3, Vulkan only: the image equals the scene without the node; validation 0/0).
  - `UpdatedHairMatchesDX11` (Review Focus 5).
  - `HairInOtherRenderersMatchesDX11`: the renderers that already route hair nodes (`HAIR_SUPPORTED` blocks). These are depth, normal, materials, shadows-only and GI preview, and the depth map and RSM renderers through a projected light (`gtReadTexture` on the shadow map and RSM targets). Each case has a DX11 guard that the hair changes the image.
- [ ] **Step 3:** Build; run `hairVkTest.*`. Expected: FAIL (the stand-in draws nothing).
- [ ] **Step 4:** Implement.
- [ ] **Step 5:** Run `hairVkTest.*`, then the whole suite. Expected: PASS, validation 0/0, no `GVK_NOT_YET` from hair.
- [ ] **Step 6:** Commit — "GraphicsVulkan: hair fragments and renderer".

---

### Task 5: The hair pass in the HDR renderer

**Files:**
- Modify: `shdw/shdwPassHair.{hpp,cpp}`, replacing the stand-in with DX11's pass:
  - its blend and depth-stencil states (`InitStates` / `CleanupStates`);
  - `Render` (the alpha Z prepass, then the blend pass, by `m_HairTransparencyMode`);
  - `RenderDepthPeeled`, with `vkdOcclusionQuery` as `shdwPassTransparent`'s port uses it;
  - the 256×256×4 R8 opacity volume, as a `vkdImage` with sampled and storage views.
  - The `HairData` setup (opacity maps, depth map) is ported as is; the opacity maps stay null until Task 6.
- Test: extend `Vk/hairVkTest.cpp`.

- [ ] **Step 1: Failing test** `HairPassMatchesDX11`: the HDR renderer, hair over 5a's lit spheres, `m_bEnableHair` on. Cases:
  - transparency mode {0, 1, 2 (depth-peeled, `m_bHairDepthPeel` with 4 layers), 3};
  - HDR AA {off, on};
  - with SSAO and glow on.

  Each has a DX11 guard.
- [ ] **Step 2:** Build; run. Expected: FAIL (`shdwPassHair` `GVK_NOT_YET`).
- [ ] **Step 3:** Implement.
- [ ] **Step 4:** Run `hairVkTest.*`, `shdwHDRRendererVkTest.*`. Expected: PASS, validation 0/0.
- [ ] **Step 5:** Commit — "GraphicsVulkan: hair pass".

---

### Task 6: Opacity map renderer and hair shadows

**Files:**
- Create: `shdw/shdwOpacityMapRendererVk.{hpp,cpp}` and `shdw/shdwPassOpacity.{hpp,cpp}` (DX11's; the `OpacityShadowMap*` and `HairOpacityShadow*` techniques; the volume technique writes the opacity volume through a UAV).
- Modify:
  - `shdw/private/shdwSceneRendererCreate.cpp` (`CreateOpacityMapRenderer` makes it);
  - `shdwPackage` (states);
  - `shdwSceneRendererVk::ShareBuffers` (drop the stand-in exception; DX11's assert is all that's left);
  - `g3dSceneRendererVkTest.UnportedTypesFallBack` (now empty: replace it with `EveryTypeIsShdw`, which checks that every `Create*` type gives a `shdw` renderer);
  - `shdwPassHair` (the opacity maps from Task 5's `HairData` are now real).
- Test: create `Vk/shdwOpacityVkTest.cpp`; the test makes a light's opacity maps as `prjltProjectedLightObject` does (count 1, 4 or 8 by `HAIR_SHADOW_TYPE`, `e_ShadowMap`, LDR format; the L8 3D volume with its half-0/half-255 data).

- [ ] **Step 1: Failing tests:**
  - `OpacityMapsMatchDX11`: a projected light over hair and a transparent sphere, for each `HAIR_SHADOW_TYPE` (OSM4/16/32, DOSM4/16/32, VOLUME4). Every map (and the volume) is read back with `gtReadTexture` and compared within the image tolerance.
  - `HairShadowsMatchDX11`: the HDR renderer with those lights. The final image is compared, with a DX11 guard that hair shadows change it.
  - `HairPrefsChangeBetweenFramesMatchesDX11` (Review Focus 4): `m_Frames` 4, with `m_BeforeFrame` changing one setting per frame.
  - `shdwHDRRendererVkTest.ShareBuffersWithEveryType` now covers the opacity map renderer too.
- [ ] **Step 2:** Build; run `shdwOpacityVkTest.*`. Expected: FAIL (forward stand-in, blank maps).
- [ ] **Step 3:** Implement.
- [ ] **Step 4:** Run `shdwOpacityVkTest.*`, `hairVkTest.*`, `g3dSceneRendererVkTest.*`, `shdwHDRRendererVkTest.*`, then the whole suite. Expected: PASS, validation 0/0. A grep for `GVK_NOT_YET` in GraphicsVulkan finds only the demo watermark and instance step rates.
- [ ] **Step 5:** Commit — "GraphicsVulkan: opacity map renderer and hair shadows".

---

### Task 7: Unconsumed shader outputs (decision 3)

**Files:**
- Modify:
  - `LibXLT/GraphicsVulkan/vcpkg.json` (`spirv-tools`, at the manifest's baseline);
  - `vkd/private/vkdPipelineCache.cpp` (trimmed producer stages);
  - `vkd/private/vkdValidation.cpp` (drop the `WARNING-Shader-OutputNotConsumed` exception and its comment);
  - `CLAUDE.md` (validation paragraph).
- Create: `vkd/private/vkdLinkStages.{hpp,cpp}`, holding `std::vector<uint32_t> vkdTrimUnconsumedOutputs(const std::vector<uint32_t>& i_Producer, const std::vector<uint32_t>& i_Consumer)`. It returns `i_Producer` unchanged when nothing is unconsumed. Also `UnitTests/GraphicsTests/Vk/vkdLinkStagesTest.cpp`.

- [ ] **Step 1: Failing tests:**
  - `vkdLinkStagesTest.TrimsOnlyUnconsumed`: a VS writing locations 0–3 and a PS reading 1 and 3. Afterwards SPIRV-Reflect sees outputs 1 and 3 only, plus `SV_Position`.
  - `vkdLinkStagesTest.NoWarningAcrossEffects`: draw one pass of every effect whose PS reads less than its VS writes, with `OutputNotConsumed` counted as a warning (the exception is removed). The warning count must be 0.
  - `fxParityTest` and `effShaderVkTest` images bit-identical to a run before the change (saved by the test, compared in the same run with trimming off through `vkdPipelineCache::SetTrimOutputs(false)`).
- [ ] **Step 2:** Build; run. Expected: FAIL (warnings counted).
- [ ] **Step 3:** Implement. **If SPIRV-Tools can't remove them cleanly, stop and ask** (decision 3's fallback).
- [ ] **Step 4:** Run the whole suite, Debug and Release. Expected: PASS, validation 0/0, with the info-level exception gone. RenderNode Release on `testTESS`, frames 296–300, with `-gfxValidation`: frames bit-identical to Task 6's run, validation 0/0.
- [ ] **Step 5:** Commit — "GraphicsVulkan: drop unconsumed shader outputs at pipeline creation".

---

### Task 8: Validation modes and the validation sign-off

**Files:**
- Modify:
  - `vkd/vkdDevice.hpp`, `vkd/private/vkdDevice.cpp`: `bool m_bBestPractices` in `vkdDeviceOptions`, turning on `validate_best_practices` and `validate_best_practices_nvidia`. Add `void vkdReadEnvironment(vkdDeviceOptions& io_Options)`, which reads `MSP_GFX_VK_ADAPTER` and `MSP_GFX_VK_VALIDATION`.
  - `vkd/vkdValidation.hpp`, `vkd/private/vkdValidation.cpp`: messages whose ID starts with `BestPractices-` are counted as best-practices notes, not warnings, and the exit line gains `, P best-practices notes`.
  - The three readers of `MSP_GFX_VK_ADAPTER` call `vkdReadEnvironment`.
  - `Tools/gfxcompare` (parse the longer line; tests for both forms).
- Test: extend `Vk/vkdValidationTest.cpp` and `Vk/vkdDeviceTest.cpp`, and Tools/gfxcompare's pytest suite.

- [ ] **Step 1: Failing tests:**
  - `vkdDeviceTest.EnvironmentSelectsValidation`: `gpu`, `bestpractices`, both, empty, and an unknown word (logged, ignored).
  - `vkdValidationTest.BestPracticesCountedApart`: a message with ID `BestPractices-...` fed to the callback is counted as a note, not a warning.
  - gfxcompare: the old line and the new one both parse.
- [ ] **Step 2:** Build; run. Expected: FAIL. Then implement, run, and expect PASS.
- [ ] **Step 3: Sign-off runs.** Ledger each run's counts:
  - GraphicsTests Debug, whole suite (core + sync): 0/0.
  - GraphicsTests Debug, whole suite with `MSP_GFX_VK_VALIDATION=gpu`: 0/0.
  - RenderNode Debug, `-gfx vulkan`, `testTESS` 296–300, core + sync: 0/0, no asserts. The same with `gpu`: 0/0. Every `-capPasses` pass, 296–300: 0/0.
  - The same three runs with `bestpractices`: summarize the notes by message ID with counts and one line each on what they ask for. They are inputs to Phase 2 and don't fail anything.
- [ ] **Step 4:** Commit — "GraphicsVulkan: GPU-assisted and best-practices validation from MSP_GFX_VK_VALIDATION".

---

### Task 9: Test-scene sign-off and the timing report

**Files:**
- Create: `docs/superpowers/reports/2026-10-xx-gfx-vulkan-phase1.md` (decision 5).
- Modify: `Tools/gfxcompare` only if the report needs a table it doesn't write yet (for example per-pass CPU/GPU medians side by side, already in `timing.py`'s `compare_timing`).

- [ ] **Step 1: Ask the owner** whether sign-off wants all 135 frames of `testTESS`, or 296–315 as usual.
- [ ] **Step 2: Parity runs** (Release RenderNode, headless). Each Vulkan run uses `-gfxValidation`, and each DX11 run is first compared with itself:
  - `testTESS` beauty;
  - `testAO`;
  - supersampling 2;
  - every `-capPasses` pass (296–300).

  First give gfxcompare its tolerance for the level-of-detail difference (decision 6's outcome), measured on these runs and documented in its README. Expected: every frame within the tolerance Task 1 settled on (decision 6). Any frame outside it is investigated, and a GraphicsTests case is added or fixed, before sign-off.
- [ ] **Step 3: Timing runs.** No validation; `-gfxProfile`; the machine otherwise idle; `testTESS` and `testAO`, 296–315, each backend twice. Report the median of the two:
  - per-pass CPU and GPU;
  - frame CPU and GPU;
  - draw counts. Vulkan's double count of bump draws (5a) is fixed or footnoted: fix it if it's a profiler bug, ask if it's DX11's.
- [ ] **Step 4: Write the report.** It covers:
  - **Parity:** per scene and per pass, with tolerances and exceptions; and per GraphicsTests suite, with exception tables.
  - **Validation**, from Task 8, including the best-practices summary.
  - **Timing:** tables and ratios. The reflection hot spot is called out (`PlanarReflection` CPU about 4.4× and its ZFill CPU about 16× in 5b, `AlphaFill` CPU about 10×), as the input to Phase 2 with the spec's "Phase 2 direction" items ranked by what the numbers say.
  - **What isn't ported and why:** 5a's and Plan 6's "not ported" lists, the demo watermark, step rates.
  - **Items for the owner to raise**, deferred during the port:
    - DX11's NaNs on the first velocity frame;
    - DX11's GI-pass nondeterminism (up to 4/255 against itself);
    - any hair deferral.
- [ ] **Step 5:** Run the DX11 harness, short: 296–315 against the post-`6848f8d2` reference. Expected: 20/20.
- [ ] **Step 6:** Commit — "GraphicsVulkan: Phase 1 report".

---

### Task 10: Docs, the last checks, stop for sign-off

**Files:**
- Modify:
  - `CLAUDE.md`, GraphicsVulkan section:
    - `hair` package; `stub` row removed;
    - `Eff/private/ShadersVk` and the hair exception;
    - `MSP_GFX_VK_VALIDATION`;
    - the validation paragraph without the `OutputNotConsumed` exception;
    - `spirv-tools` in the vcpkg table.
  - The roadmap: Plan 7 status, "Open investigations" closed, Phase 1 exit.
  - This plan's status line.

- [ ] **Step 1: Hook grep.** Every `SetImplementation` and `g3dRendererMgr::AddRenderer` call in GraphicsDX11 has a real GraphicsVulkan counterpart. `GVK_NOT_YET` appears only in the demo watermark and instance step rates. For each of those two, confirm by grep that DX11 reaches it (or not), and ledger the ruling.
- [ ] **Step 2:** Build all six configurations. Every test passes in each (Debug and Release suites), with validation 0/0.
- [ ] **Step 3:** Edit the docs and commit — "GraphicsVulkan: docs and roadmap: Plan 7 and Phase 1".
- [ ] **Step 4: Stop for sign-off.** Report:
  - the exit criteria one by one, with evidence: images within tolerance (hair matched or deferred), the timing report, validation in every mode, DX11 unchanged;
  - the report's path;
  - rulings and deferred items;
  - an offer to publish the report as a page.

  Vulkan stays non-default until Phase 2 makes it at least as fast (spec decision 9).
