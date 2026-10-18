# GraphicsVulkan shaders and effects — Implementation Plan (Phase 1, Plan 3)

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task, inline in the session (the owner asked for no subagents). Steps use checkbox (`- [ ]`) syntax for tracking. When the plan is done, stop for the owner's review before Plan 4 is written in detail.

**Goal:** Every effect stage (except the 3 hair hull shaders) compiles from unchanged HLSL to SPIR-V at build time. `fxEffectVk` reflects it exactly as `fxEffectDX11` does and draws through a DX11-style immediate context. Full-screen effect passes then render the same pixels as DX11.

**Architecture:**
- The shader tree is copied byte-for-byte.
- A build script (`EmbedShadersVk.ps1`, a DXC copy of `EmbedShaders.ps1`) embeds the SPIR-V.
- `fxEffectVk` is `fxEffectDX11`'s logic with these substitutions:
  - SPIRV-Reflect replaces `D3DReflect`.
  - `vkdShader` modules replace `ID3D11*Shader`.
  - Ring-buffer uploads replace `Map(WRITE_DISCARD)`.
  - Its fxEffect-API wrapper `effPlainEffectVk` is ported alongside it, so Plan 4 can port `effShaderBaseDX11` mechanically.
- A new `vkdContext` plays the part of `ID3D11DeviceContext`:
  - It holds shaders, vertex layout, topology, blend/depth/rasterizer state, viewports, render targets and slot bindings.
  - At each draw it looks up a pipeline in `vkdPipelineCache`, which is keyed by the full state and persisted to disk. It then writes the binding table and draws inside dynamic rendering, inserting the barriers DX11 inserted implicitly.
- The `g3d*StateMgr` holders and `g2dFullscreenQuad` are ported on top of it.
- GraphicsTests links GraphicsDX11 so each test can run the same effect on both backends and compare.

**Tech Stack:** C++17 (MSVC x64), Vulkan 1.4 C API via volk, VMA, SPIRV-Reflect, DXC (vcpkg `directx-dxc`, same baseline as Plan 2), Direct3D 11 (tests only, as the reference), GoogleTest, `stb` (`stb_image_write`, tests only).

**Spec:** `docs/superpowers/specs/2026-10-17-graphics-vulkan-design.md`, sections "Shaders", "Library layout" (pipeline cache, ring buffer, state managers), "Modern Vulkan", "Validation", "Proving parity → Synthetic-scene image tests". Roadmap: `docs/superpowers/plans/2026-10-17-graphics-vulkan-roadmap.md`.

From Plan 2 (done) it uses:
- `vkdDevice`, `vkdRingBuffer`, `vkdBindings` (descriptor heap, binding table; `-fvk-[btsu]-shift 0/100/200/300`)
- `vkdImage`, `vkdGlobal`
- the `gtVulkan` fixture
- the stubs (`matShaderMgrVk` etc.)

## Decisions made in this plan (please review)

1. **Fonts move to Plan 4.**
   - DX11 draws text as an `scrText` sprite: a `tmeshFrag` built through `g3dFragmentCreate` and textured from `matTextureMgr::LoadTexture`.
   - Both of those are Plan 4 ports, so the text parity test moves with them.
   - Plan 3's image tests use full-screen effect passes and the textured quad instead.
   - The roadmap rows for Plans 3 and 4 are updated in Task 7.
2. **The shader directory holds exact copies only.** It gets the `.hlsl`, `.hlsli` and `.effect.json` files. The DXC script lives in `Fx/private/`, outside that directory, so a byte-for-byte test can cover the whole directory. GraphicsDX11's fxc script isn't copied, since nothing would use it.
3. **The run-time compiled shaders in Plan 3's scope are compiled at build time.** These are the full-screen quad VS and the textured-quad PS. Their source strings are copied verbatim into `G2d/private/Shaders/*.hlsl` (Vulkan-only files) and compiled by the same DXC options; their names say where they came from. The other run-time shaders move with their ports:
   - fonts → Plan 4
   - the ray-tracing compute shaders (`rtUtil`, which loads from a hard-coded 2009 path) → Plan 5 or 6, with `shdw`
   - `PSMapNormalsToScreen` (DXBC only, no source) → its pass's plan
4. **Readback is a staging-buffer copy** (`vkdImage::ReadBack`, existing). This settles the stub's open question.
5. **Pipeline persistence uses `VkPipelineCache`**, not `VK_KHR_pipeline_binary`.
   - It's simpler, and the spec allows either.
   - The cache file is `<gfPaths e_UserDataPath>/VulkanPipelineCache.bin`. With no valid user-data path there's no persistence, and that's logged.
   - Phase 2 can switch to pipeline binaries if measurement asks for it.
6. **Hazards are handled by coarse barriers in `vkdContext`.** It inserts one global `VkMemoryBarrier2` (all writes so far → all later reads and writes):
   - when the render targets change (rendering ends)
   - around transfers (clears outside rendering, copies, readback)
   - after a draw or dispatch that had UAVs bound

   Within one rendering scope, attachment writes are ordered by rasterization order, as in DX11. Synchronization validation is the referee; Phase 2 can narrow the barriers.
7. **The state holders take Vulkan enums.** `gvk::g3dBlendStateMgr` and the others keep DX11's function names but take `VkBlendFactor`, `VkCompareOp` and so on. `d3d11.h` never enters GraphicsVulkan, and ported call sites translate the enum names.
8. **The fxEffect API (`gvk::fxEffect`) keeps every class, method and name** of `fxEffectApi.hpp`. The only changes are the D3D types:
   - `HRESULT` → `bool` (true = success)
   - `LPCSTR` → `const char*`; `UINT` → `uint32_t`
   - `D3D_SHADER_VARIABLE_CLASS`/`_TYPE` → `gvk::fxVarClass`/`fxVarType`, with the same numeric values
   - `ID3D11ShaderResourceView*`/`ID3D11UnorderedAccessView*` → `vkdImage*`
   - `Apply(flags, context)` → `Apply(flags)`, which uses the global context
   - `fxEffectPassDesc::pIAInputSignature` is dropped, because Vulkan matches vertex inputs by semantic when the pipeline is created
9. **The vertex-input semantic comes from DXC's stage-input variable names** (`in.var.TEXCOORD0`), not from `-fspv-reflect`. That flag would add two `VK_GOOGLE_*` device requirements. Task 2's test checks every vertex-shader input has such a name.

## Global Constraints

- **No GraphicsDX11 changes, and no changes to shader sources.** The Vulkan shader directory is a byte-identical copy (test in Task 1). The copied run-time shader strings (decision 3) are verbatim.
- DX11 behaviour is unchanged: no flag and `-gfx dx11` behave as before, and the Plan 1 DX11-vs-DX11 harness still passes.
- **Code layout:**
  - All GraphicsVulkan code is in namespace `gvk`.
  - Include guards start with `GVK_`, and classes GraphicsDX11 names `...DX11` are `...Vk`.
  - The package layout is GraphicsDX11's (`Fx/`, `Eff/`, `G2d/`, `G3d/`, `vkd/`): public headers in the package folder, `.cpp` in `private/`, guarded includes.
- Vulkan C API via volk only; no `d3d11.h` or `windows.h` in GraphicsVulkan outside `*Win*` files.
- **DXC options, one set used everywhere** (the build script, the GraphicsTests build step, `compile_check.py --spirv`; a comment in each names the others):
  - `-spirv -fspv-target-env=vulkan1.3 -fvk-use-dx-layout -fvk-use-dx-position-w`
  - `-fvk-support-nonzero-base-instance -fvk-support-nonzero-base-vertex`
  - `-fvk-b-shift 0 0 -fvk-t-shift 100 0 -fvk-s-shift 200 0 -fvk-u-shift 300 0`
  - the profile `<stage>_6_0` (from the manifest's `_5_0`)
  - no `-fvk-invert-y`: Y is flipped by a negative viewport height (the spec's convention)

  If this DXC lacks one of these flags, the build fails. A missing flag is replaced only by a ledgered ruling saying what the replacement preserves.
- **Deferred stages:** `Hair_HS` in `Materials/Hair_SH`, `Special/HairDefault` and `Special/OpacityRender` (DXC can't compile `ProcessIsolineTessFactors`). They're skipped by name and reported at build time. Passes using them exist but don't draw: `GVK_NOT_YET` once, then nothing. Plan 7 handles them.
- **Validation:** zero errors and zero warnings (core and synchronization) in every test and run. Suppression is allowed only for a confirmed layer false positive, with the message ID and the reason.
- **Image tolerance** (spec, "Proving parity"): max channel error ≤ 2/255, and at most 0.1% of pixels over 1/255.
  - Loosen it only per effect and pass, with a written reason in the test's exception table, and report it.
  - A failing comparison writes `dx11.png`, `vulkan.png` and `diff.png` to `UnitTests/bin/x64/<Debug|Release>/GraphicsTestsImages/<test>/`.
- Git: branch `claude/graphics-vulkan`; stage explicit paths only; no push. Commit messages end with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.

## Review Focus

1. **Matrix majorness and shape.** DXC decorates an HLSL `column_major` matrix as SPIR-V `RowMajor` and transposes R×C. A wrong mapping makes every transform silently wrong. Task 2's reflection test compares `m_bColumnMajor`, rows and columns against D3D reflection for every constant of every effect. Task 6's parity tests set a non-symmetric matrix wherever a full-screen pass has one.
2. **Y flip and winding.** With a negative viewport height, text and full-screen passes must not come out upside down, and culling must keep the same faces as DX11. Task 4 tests `ViewportMatchesD3D` (an asymmetric triangle at known pixels) and `CullingMatchesD3D` (clockwise and counter-clockwise triangles under each cull mode, with both `FrontCounterClockwise` values, checked against expected pixels taken from DX11 on the same machine).
3. **A stale or corrupt pipeline-cache file.** A cache file from another driver or GPU, or truncated or garbage bytes, must be ignored and replaced, never crash or log validation errors. Task 3 tests `CorruptCacheFileIsIgnored` and `ForeignHeaderIsIgnored`.
4. **Constants reused across submissions.** An effect applied in frame N and again in frame N+3 with no change must not point at a ring block the GPU has since recycled. Task 5 tests `UnchangedConstantsSurviveRingReuse` (apply, submit and wait four times with no `Set*`, draw, then check the pixels).
5. **Hazards between passes.** Rendering to a target and then sampling it in the next full-screen pass must be ordered. Task 4 tests `RenderThenSampleIsOrdered` with synchronization validation on (zero hazards) and the expected pixels.

---

## File Structure

| Path (under `LibXLT/GraphicsVulkan/` unless stated) | Responsibility |
|---|---|
| `Eff/private/Shaders/**` | Byte-identical copy of GraphicsDX11's `.hlsl`, `.hlsli`, `.effect.json` |
| `Fx/private/EmbedShadersVk.ps1` | DXC → SPIR-V for every manifest stage (minus deferred), writes `Fx/private/Generated/fxEmbeddedShadersVk.inc` (git-ignored like DX11's) |
| `Fx/fxEffectDesc.hpp`, `private/fxEffectDesc.cpp` | Copy of DX11's (namespace `gvk`; nlohmann-json added to `vcpkg.json`) |
| `Fx/fxEffectVk.hpp`, `private/fxEffectVk.cpp` | Reflection (SPIRV-Reflect), constants, resources, samplers, Apply |
| `Fx/fxEmbeddedShadersVk.hpp`, `private/fxEmbeddedShadersVk.cpp` | Lookup by effect name; `CreateVk` |
| `Fx/fxMaterialInstance.hpp/.cpp` | Copy of DX11's, on `fxEffectVk` |
| `Fx/fxEffectApi.hpp`, `Eff/effPlainEffectVk.hpp`, `Eff/private/effPlainEffectVk.cpp` | fxEffect API (decision 8) and its implementation over `fxEffectVk` |
| `vkd/vkdState.hpp` | `vkdBlendState`, `vkdDepthStencilState`, `vkdRasterizerState`, `vkdViewport`, `vkdVertexElement`/`vkdVertexLayout`, D3D11 defaults |
| `vkd/vkdShader.hpp`, `private/vkdShader.cpp` | `VkShaderModule` + id + reflected vertex inputs |
| `vkd/vkdPipelineCache.hpp`, `private/vkdPipelineCache.cpp` | `vkdPipelineKey`, lookup/create, eviction, load/save |
| `vkd/vkdContext.hpp`, `private/vkdContext.cpp` | DX11 immediate-context emulation over dynamic rendering |
| `vkd/vkdImage.hpp/.cpp` (modify) | `SampledIndex()`/`StorageIndex()` heap registration, usages for render targets |
| `G3d/g3dBlendStateMgr.hpp`, `g3dDepthStencilStateMgr.hpp`, `g3dRasterizerStateMgr.hpp`, `g3dStateMgr.hpp` + `private/` | Ported state holders over `vkdContext` |
| `G2d/g2dFullscreenQuad.hpp`, `private/g2dFullscreenQuad.cpp`, `G2d/private/Shaders/FullscreenQuadVS.hlsl`, `TexturedQuadPS.hlsl` | Ported quad helpers (decision 3) |
| `UnitTests/GraphicsTests/Dx11/gtD3D11.hpp`, `gtD3D11.cpp` | Reference-side helpers: a D3D11 device (hardware, else WARP), DX11 globals for `g2dFullscreenQuad`, readback |
| `UnitTests/GraphicsTests/gtImage.hpp/.cpp` | Image comparison (spec metrics) + PNG output on failure |
| `UnitTests/GraphicsTests/Vk/*Test.cpp` | Tests per task |
| `Tools/fx2hlsl/compile_check.py`, `.github/workflows/build-windows.yml` | `--spirv` uses the build's options; CI step |

---

### Task 1: Copied shaders and the SPIR-V build step

**Files:**
- Create: `Eff/private/Shaders/**` (copy), `Fx/private/EmbedShadersVk.ps1`, `.gitignore` entry for `Fx/private/Generated/`
- Modify: `GraphicsVulkan.vcxproj` and `.filters` (`<None>` items for the shaders, an `EmbedVkShaders` target before `ClCompile`, with inputs and output as DX11's)
- Test: `UnitTests/GraphicsTests/Vk/fxShaderCopyTest.cpp`

**Interfaces — Produces:** the generated `fxEmbeddedShadersVk.inc` defines, inside `namespace gvk`, the same shape as DX11's:
- `fxEmbeddedBytecode { m_Entry, m_Profile /* the manifest's, e.g. ps_5_0 */, m_pData /* SPIR-V words as bytes */, m_Size }`
- `fxEmbeddedEffect { m_Name, m_Source, m_Manifest, m_pBytecode, m_NumBytecode }`
- `const fxEmbeddedEffect fxEmbeddedShadersVk::k_Effects[]`, `k_NumEffects`
- `const char* const fxEmbeddedShadersVk::k_Deferred[]` (`"Materials/Hair_SH.hlsl|Hair_HS"`, …), `k_NumDeferred`

The struct declarations themselves are in `Fx/fxEmbeddedShadersVk.hpp` (Task 5 adds the functions).

- [ ] **Step 1: Failing test** `fxShaderCopy.IdenticalToDX11`: walk `LibXLT/GraphicsVulkan/Eff/private/Shaders` recursively.
  - Every file must have a byte-identical counterpart in GraphicsDX11's directory.
  - Every DX11 `.hlsl`/`.hlsli`/`.effect.json` must have a copy.
  - Locate the repo root from the test exe's path, as other GraphicsTests do; don't rely on the working directory.
- [ ] **Step 2: Run; fails** (no directory).
- [ ] **Step 3: Copy the files** (`cp`, not an editor, so the bytes are identical; `.gitattributes` must not rewrite line endings: check `git diff --no-index` shows nothing).
- [ ] **Step 4: Write `EmbedShadersVk.ps1`:**
  - Parameters `-ShaderDir -OutFile -Dxc`.
  - It is `EmbedShaders.ps1` with DXC and the Global Constraints options, `_5_0` → `_6_0`, `namespace gvk { … }` around the output, and the deferred list (skip and `Write-Host` each).
  - Wire the `EmbedVkShaders` target, with DXC from `vcpkg_installed\x64-windows\x64-windows\tools\directx-dxc\dxc.exe`.
- [ ] **Step 5: Build GraphicsVulkan.** Expected: `EmbedShadersVk: 60 effect(s)`, 3 skipped stages, no DXC errors.
  - Record DXC's warnings count in the ledger; warnings are allowed, they come from unchanged HLSL.
  - If DXC reports a `$Globals` buffer for any stage, note which files; Task 2 settles their binding.
- [ ] **Step 6: Run the test; it passes.**
- [ ] **Step 7: Commit:** "GraphicsVulkan: copy the shaders; embed them as SPIR-V with DXC".

---

### Task 2: `fxEffectDesc` copy and `fxEffectVk` reflection, compared with DX11

**Files:**
- Create: `Fx/fxEffectDesc.hpp`, `Fx/private/fxEffectDesc.cpp` (copy, `gvk`), `Fx/fxEffectVk.hpp`, `Fx/private/fxEffectVk.cpp` (reflection half), `Fx/fxEmbeddedShadersVk.hpp/.cpp` (`Find`, `CreateVk`)
- Modify: `vcpkg.json` (+`nlohmann-json`); `GraphicsVulkan.vcxproj` (merge `spirv-reflect-static.lib` into the library, as volk is merged); `UnitTests/GraphicsTests/GraphicsTests.vcxproj`:
  - a ProjectReference to `GraphicsDX11`
  - links `d3d11.lib`, `dxgi.lib`, `d3dcompiler.lib`
  - the GraphicsDX11 vcpkg include path for its headers
- Test: `UnitTests/GraphicsTests/Dx11/gtD3D11.hpp/.cpp`, `Vk/fxEffectVkReflectionTest.cpp`

**Interfaces — Produces** (`gvk`). `fxEffectVk` has `fxEffectDX11`'s public interface with:
- `ID3D11Device*` → `vkdDevice*`
- `ID3D11ShaderResourceView*`/`UnorderedAccessView*` → `vkdImage*`
- `Apply(int tech, int pass)` with no context argument (Task 5)
- `Type::m_Class`/`m_Type` holding `fxVarClass`/`fxVarType` values

Additions:

```cpp
enum fxVarClass { fx_SVC_SCALAR = 0, fx_SVC_VECTOR, fx_SVC_MATRIX_ROWS, fx_SVC_MATRIX_COLUMNS, fx_SVC_OBJECT, fx_SVC_STRUCT };
enum fxVarType  { fx_SVT_VOID = 0, fx_SVT_BOOL = 1, fx_SVT_INT = 2, fx_SVT_FLOAT = 3, fx_SVT_UINT = 19 };  // D3D_SVT_* values

// i_pDevice == NULL: reflection only (no shader modules, samplers or pipelines); used by tests and CI.
static std::unique_ptr<fxEffectVk> Create(vkdDevice* i_pDevice, const fxEffectDesc& i_Desc,
                                          const fxBytecodeSource& i_Bytecode, std::string& o_Error);

struct fxStageBinding { enum Kind { e_ConstantBuffer, e_Texture, e_Sampler, e_Uav } m_Kind;
                        std::string m_Name; uint32_t m_Slot; /* register number */ };
std::vector<fxStageBinding> GetStageBindings(int i_Technique, int i_Pass, fxStage i_Stage) const;
bool IsPassDeferred(int i_Technique, int i_Pass) const;   // uses a k_Deferred stage
```

`fxEmbeddedShadersVk::Find(name)` and `CreateVk(effect, vkdDevice*, o_Error)` mirror DX11's. A stage on the deferred list has no bytecode, which marks its pass deferred instead of failing `Create`.

**Reflection rules.** These are the D3D11 reflection equivalents; the test is the arbiter.
- **Constant buffers:**
  - Name: the cbuffer name, without DXC's `type.` prefix.
  - Size: the block size rounded up to 16.
  - The cross-stage layout checks are as in DX11.
- **Variables:**
  - `m_Offset` is the member offset.
  - `m_Size` is D3D's: for an array, `(n-1)*16 + element size`; otherwise `Type::GetElementSize()`.
  - Matrices: D3D rows = the number of SPIR-V matrix columns, and D3D columns = the SPIR-V column vector size. `m_bColumnMajor` = the member carries `RowMajor` (DXC flips the convention).
  - A `uint` member that the manifest declares `bool` reflects as `fx_SVT_BOOL`.
- **Resources:**
  - textures/buffers (t, binding − 100); samplers (s, − 200); UAVs (u, − 300).
  - Constant-buffer slot = binding.
  - Resource arrays are unsupported (as DX11).
- **`$Globals`:** if Task 1 found any, compile those files with `-fvk-bind-globals <fxc's slot> 0`. That slot comes from DX11's reflection of the same stage, kept as a per-file table in the script, and the test proves it. With none, nothing is needed.
- **Defaults:** `apply_defaults` is copied.
- **Vertex inputs:** each `Location` input of a VS records `{semantic, index, location, format}`, from the variable name `in.var.<SEMANTIC><index>`.

**`gtD3D11` (tests):**
- `ID3D11Device* gtD3D11::Device()` creates on the default hardware adapter, else WARP; it skips the test (`GTEST_SKIP`) only if neither works.
- `const char* AdapterName()`.
- `ReadBack(ID3D11Texture2D*) -> std::vector<uint8_t>`.
- `SetGlobals()` sets `g2dDX11Global::g_pDevice`/`g_pDeviceContext` for the duration of a test.

- [ ] **Step 1: Failing tests** in `fxEffectVkReflectionTest.cpp`. Parameterized over every embedded effect; no Vulkan device needed, DX11 on hardware or WARP.
  - `SameTechniquesAndPasses`.
  - `SameConstants`:
    - for every name in either effect's `GetConstantNames()`, both have it;
    - the buffer name, `m_Offset`, `m_Size`, `m_bColumnMajor`, `m_Columns` and `m_Elements` match;
    - the `Type` matches recursively (class, type, rows, columns, elements, member names and offsets).
  - `SameConstantBuffers`: names, sizes, and `GetConstantBufferDefaults` bytes equal.
  - `SameResourcesSamplersUavs`: name sets.
  - `SameStageBindings`: for every pass and stage, `GetStageBindings` equals D3DReflect of DX11's embedded bytecode for the same entry and profile. The test reflects DX11's bytecode itself, because `fxEffectDX11` keeps its bindings private. Deferred stages are excluded and listed.
  - `VertexInputsNamed`: every VS input of every pass has a semantic.

  Exceptions go in an explicit table in the test, with a reason each; the table starts empty.
- [ ] **Step 2: Build GraphicsTests; it fails** (no `fxEffectVk`).
- [ ] **Step 3: Implement.**
- [ ] **Step 4: Tests pass**, in Debug and Release.
  - Any mismatch is fixed in the reflection mapping, or in a DXC option (ledgered).
  - Never fix it by editing HLSL.
  - Also check that MainApp still links in Debug Qt (GraphicsVulkan now merges SPIRV-Reflect).
- [ ] **Step 5: Device features.** List the SPIR-V capabilities across all modules (spirv-reflect gives them) in the ledger.
  - Map each to the Vulkan feature it needs.
  - Add those, plus `samplerAnisotropy`, `samplerMirrorClampToEdge`, `fillModeNonSolid`, `depthClamp`, `depthBiasClamp`, `independentBlend`, `dualSrcBlend`, `textureCompressionBC` and `imageCubeArray`, to `vkdRequiredFeatures()` and to the device creation.
  - Update `vkdAdapterTest` (complete adapter list) and keep `MissingFeatureNamed` passing.
- [ ] **Step 6: Commit:** "GraphicsVulkan: fxEffectVk reflection with SPIRV-Reflect, identical to DX11's".

---

### Task 3: Pipeline state, shader modules and the persisted pipeline cache

**Files:**
- Create: `vkd/vkdState.hpp`, `vkd/vkdShader.hpp`, `vkd/private/vkdShader.cpp`, `vkd/vkdPipelineCache.hpp`, `vkd/private/vkdPipelineCache.cpp`
- Test: `Vk/vkdPipelineCacheTest.cpp` (minimal test shaders compiled by the existing GraphicsTests DXC step: `Vk/Shaders/gtSolid.hlsl`, VS position+colour, PS colour)

**Interfaces — Produces:**
```cpp
namespace gvk {
// D3D11 descriptions in Vulkan terms; default-constructed = D3D11's default state.
struct vkdBlendTarget { bool m_bEnable = false; VkBlendFactor m_Src = VK_BLEND_FACTOR_ONE, m_Dst = VK_BLEND_FACTOR_ZERO;
                        VkBlendOp m_Op = VK_BLEND_OP_ADD; VkBlendFactor m_SrcAlpha = VK_BLEND_FACTOR_ONE, m_DstAlpha = VK_BLEND_FACTOR_ZERO;
                        VkBlendOp m_OpAlpha = VK_BLEND_OP_ADD; VkColorComponentFlags m_WriteMask = 0xF; };
struct vkdBlendState { bool m_bAlphaToCoverage = false; bool m_bIndependent = false; vkdBlendTarget m_Targets[8]; };
struct vkdStencilFace { VkStencilOp m_Fail = VK_STENCIL_OP_KEEP, m_DepthFail = VK_STENCIL_OP_KEEP, m_Pass = VK_STENCIL_OP_KEEP; VkCompareOp m_Func = VK_COMPARE_OP_ALWAYS; };
struct vkdDepthStencilState { bool m_bDepth = true; bool m_bDepthWrite = true; VkCompareOp m_DepthFunc = VK_COMPARE_OP_LESS;
                              bool m_bStencil = false; uint8_t m_ReadMask = 0xFF, m_WriteMask = 0xFF; vkdStencilFace m_Front, m_Back; };
struct vkdRasterizerState { VkPolygonMode m_Fill = VK_POLYGON_MODE_FILL; VkCullModeFlags m_Cull = VK_CULL_MODE_BACK_BIT;
                            bool m_bFrontCCW = false; int32_t m_DepthBias = 0; float m_DepthBiasClamp = 0, m_SlopeScaledDepthBias = 0;
                            bool m_bDepthClip = true; bool m_bScissor = false; bool m_bMultisample = false; bool m_bAntialiasedLine = false; };
struct vkdViewport { float m_X, m_Y, m_Width, m_Height, m_MinDepth = 0, m_MaxDepth = 1; };   // D3D convention (top-left origin)
struct vkdVertexElement { std::string m_Semantic; uint32_t m_SemanticIndex; VkFormat m_Format; uint32_t m_Slot; uint32_t m_Offset;
                          bool m_bPerInstance = false; uint32_t m_InstanceStep = 0; };
class vkdVertexLayout { public: explicit vkdVertexLayout(std::vector<vkdVertexElement> i_Elements, std::vector<uint32_t> i_Strides);
                        uint64_t Id() const; const std::vector<vkdVertexElement>& Elements() const; uint32_t Stride(uint32_t i_Slot) const; };

class vkdShader {   // one compiled stage
public:
    static std::unique_ptr<vkdShader> Create(vkdDevice* i_pDevice /*NULL: reflection only*/, VkShaderStageFlagBits i_Stage,
                                             const void* i_pSpirv, size_t i_Size, const char* i_Name, std::string& o_Error);
    ~vkdShader();                       // evicts its pipelines, destroys the module after the GPU is done
    uint64_t Id() const; VkShaderStageFlagBits Stage() const; VkShaderModule Module() const; const char* Name() const;
    struct Input { std::string m_Semantic; uint32_t m_Index; uint32_t m_Location; VkFormat m_Format; };
    const std::vector<Input>& Inputs() const;   // VS only
};

struct vkdPipelineKey {             // everything baked into a VkPipeline; the rest is dynamic state
    uint64_t m_Shaders[5];           // vkdShader ids VS HS DS GS PS (0 = none)
    uint64_t m_VertexLayout;         // vkdVertexLayout id (0 = none)
    uint8_t  m_TopologyClass;        // point/line/triangle/patch
    uint8_t  m_PatchControlPoints;
    vkdBlendState m_Blend;           // write masks, factors, ops, alpha-to-coverage
    VkPolygonMode m_Fill; bool m_bDepthClip; bool m_bAntialiasedLine;
    VkFormat m_Colors[8]; uint8_t m_ColorCount; VkFormat m_Depth; VkFormat m_Stencil; uint8_t m_Samples;
    bool operator==(const vkdPipelineKey&) const; size_t Hash() const;
};
class vkdPipelineCache {
public:
    vkdPipelineCache(vkdDevice& i_Device, vkdBindings& i_Bindings, const std::string& i_File /* "" = no persistence */);
    ~vkdPipelineCache();            // saves (if a file was given), destroys pipelines after the GPU is done
    VkPipeline Get(const vkdPipelineKey& i_Key, const vkdShader* const i_Stages[5], const vkdVertexLayout* i_Layout);
    void Evict(uint64_t i_ShaderId);
    bool Save();                    // VkPipelineCache data → file (write to .tmp, rename)
    size_t Count() const; uint64_t Creations() const;
    bool LoadedFromFile() const;    // false when there was no file, or it was rejected
};
}
```

**Pipeline rules:**
- Dynamic states: viewport and scissor with count, cull mode, front face, primitive topology, primitive restart enable, depth test/write/compare, depth bias enable and values, stencil test/op/compare mask/write mask/reference, blend constants.
- `VK_PIPELINE_CREATE_2_DESCRIPTOR_HEAP_BIT_EXT`, with no layout; `vkdBindings::ShaderMapping()` is chained to every stage.
- Dynamic-rendering formats come from the key.
- A VS input that the layout doesn't supply is a pipeline creation error, logged with both names, and `Get` returns `VK_NULL_HANDLE`; D3D's `CreateInputLayout` fails in the same case.
- Every pipeline is named after its shaders (`SetName`).
- **Loading:**
  - Read the file; reject it if it is shorter than `VkPipelineCacheHeaderVersionOne`, or if the header's vendor ID, device ID or `pipelineCacheUUID` differ from the device's. On rejection, log why and start empty.
  - Then `vkCreatePipelineCache` with the data.

- [ ] **Step 1: Failing tests:**
  - `KeyEqualityAndHash`: keys differing in any one field compare unequal.
  - `SameStateSamePipeline`: two `Get`s give one creation.
  - `DynamicStateNeedsNoNewPipeline`: changing only the cull mode or depth func through the context comes in Task 4; here, only the key fields matter.
  - `EvictOnShaderDestroy`.
  - `MissingVertexInputFails`.
  - `SaveAndReload`: create 3 pipelines, `Save` to a `gtTempDir` file. A second cache on the same file reports `LoadedFromFile()`, and its `Get`s succeed.
  - `CorruptCacheFileIsIgnored`: garbage bytes, then a truncated header.
  - `ForeignHeaderIsIgnored`: a valid header with another vendor ID.

  Each loaded-or-rejected case must have zero validation messages and must not crash.
- [ ] **Step 2: Build; fail.** **Step 3: Implement.** **Step 4: Pass** with zero validation messages.
- [ ] **Step 5: Commit:** "vkd: pipeline state, shader modules and a persisted pipeline cache".

---

### Task 4: `vkdContext`, the immediate context

**Files:**
- Create: `vkd/vkdContext.hpp`, `vkd/private/vkdContext.cpp`
- Modify: `vkd/vkdImage.hpp/.cpp` (`SampledIndex()`, `StorageIndex()`: registered in `vkdBindings` on first use, removed when destroyed; `Create2D` callers choose usage), `vkd/vkdGlobal.hpp/.cpp` (`Context()`, `PipelineCache()`), `GraphicsVulkanLayer.cpp` (create and destroy them; the cache file is `gfPaths::GetPath(e_UserDataPath)/VulkanPipelineCache.bin` when `IsPathValid`)
- Test: `Vk/vkdContextTest.cpp`, `Dx11/` reference expectations computed in the same test

**Interfaces — Produces:**
```cpp
namespace gvk {
class vkdContext {
public:
    vkdContext(vkdDevice&, vkdRingBuffer&, vkdBindings&, vkdPipelineCache&);
    void SetShaders(const vkdShader* const i_Stages[5]);      // VS HS DS GS PS; NULL = stage unused
    void SetVertexLayout(const vkdVertexLayout* i_pLayout);
    void SetVertexBuffer(uint32_t i_Slot, VkBuffer i_Buffer, VkDeviceSize i_Offset);
    void SetIndexBuffer(VkBuffer i_Buffer, VkDeviceSize i_Offset, VkIndexType i_Type);
    void SetTopology(VkPrimitiveTopology i_Topology, uint32_t i_PatchControlPoints = 0);
    void SetBlendState(const vkdBlendState& i_State, const float i_Factor[4]);
    void SetDepthStencilState(const vkdDepthStencilState& i_State, uint32_t i_StencilRef);
    void SetRasterizerState(const vkdRasterizerState& i_State);
    void SetViewports(uint32_t i_Count, const vkdViewport* i_pViewports);
    uint32_t GetViewports(vkdViewport* o_pViewports /* 16 */) const;
    void SetScissorRects(uint32_t i_Count, const VkRect2D* i_pRects);
    void SetRenderTargets(uint32_t i_Count, vkdImage* const* i_ppColors, vkdImage* i_pDepth);
    void ClearRenderTarget(vkdImage* i_pTarget, const float i_Rgba[4]);
    void ClearDepthStencil(vkdImage* i_pTarget, bool i_bDepth, float i_Depth, bool i_bStencil, uint8_t i_Stencil);
    vkdBindings& Bindings();                                  // b/t/s/u slots for the next draw
    vkdRingBuffer::Allocation Upload(const void* i_pData, VkDeviceSize i_Size, VkDeviceSize i_Alignment = 256);
    void Draw(uint32_t i_Vertices, uint32_t i_First);
    void DrawIndexed(uint32_t i_Indices, uint32_t i_FirstIndex, int32_t i_BaseVertex);
    void DrawInstanced(...); void DrawIndexedInstanced(...);
    VkCommandBuffer Commands();     // ends rendering (and barriers) for code that records its own commands
    uint64_t Flush();               // ends rendering, submits; returns the timeline value
    void ResetState();              // D3D11 ClearState(): every default above
};
}
```

**Rules:**
- **Initial state** = D3D11's device defaults: the default-constructed `vkd*State`, no targets, viewport 0×0.
- **Rendering:**
  - Rendering begins lazily at the first draw or clear after `SetRenderTargets`, and ends on any `SetRenderTargets`, `Commands()` or `Flush()`.
  - Attachments are in `GENERAL` layout with load `LOAD` and store `STORE`.
  - Clears while rendering use `vkCmdClearAttachments`; otherwise `vkCmdClearColorImage`/`DepthStencilImage`, between barriers.
- **Viewport:** `{x, y + h, w, -h}`, depth unchanged. With `m_bScissor` false, the scissor is the full target.
- **Rasterizer:**
  - `m_bFrontCCW` → `VK_FRONT_FACE_COUNTER_CLOCKWISE`, else `CLOCKWISE` (with the negative viewport, this reproduces D3D; the test proves it).
  - `m_bDepthClip == false` → `depthClampEnable`.
  - Depth bias as given (D24: same units).
- **Primitive restart** is on for strip topologies (D3D11's strip cut).
- **Draw:**
  1. Build the key (the formats come from the bound targets).
  2. Get the pipeline and bind it if it changed.
  3. Set the dynamic state that changed.
  4. `Bindings().Apply`.
  5. Draw.
  6. `G3D_PROFILE_DRAWS(1)`.

  A pipeline lookup failure skips the draw and logs once per key.
- **Barriers:** as in decision 6. Labels: `vkdDevice::BeginLabel` per rendering scope, named after the first target.

- [ ] **Step 1: Failing tests:**
  - `ClearAndDrawTriangle`: 64×64 target, clear to blue, a red full-screen triangle, then readback is red. Also, with a viewport of the left half, only the left half is red.
  - `ViewportMatchesD3D`: an asymmetric triangle with vertices (−1,1), (0,1), (−1,0) in clip space covers the top-left quadrant on both backends. DX11 is drawn with the same HLSL compiled by `D3DCompile` in the test, and the pixels must be equal.
  - `CullingMatchesD3D`: two triangles, one clockwise and one counter-clockwise. Run each cull mode × `FrontCounterClockwise` on both backends; the pixel sets must be equal.
  - `DepthTestAndBias`.
  - `StencilRef`.
  - `ScissorOnlyWhenEnabled`.
  - `RenderThenSampleIsOrdered`: draw into A, then sample A into B, under sync validation; B matches.
  - `ClearOutsideRenderingIsOrdered`.
  - `StateChangesReuseThePipeline`: cull, depth func and stencil changes cause 0 new creations.
  - `ResetStateRestoresDefaults`.
- [ ] **Step 2: Build; fail.** **Step 3: Implement.** **Step 4: Pass**, with zero validation messages including sync hazards.
- [ ] **Step 5: Commit:** "vkd: vkdContext, a DX11-style immediate context over dynamic rendering".

---

### Task 5: `fxEffectVk` draws; the fxEffect API

**Files:**
- Modify: `Fx/fxEffectVk.hpp/.cpp` (shader modules, samplers, Apply)
- Create: `Fx/fxMaterialInstance.hpp/.cpp` (copy, on `fxEffectVk`), `Fx/fxEffectApi.hpp`, `Eff/effPlainEffectVk.hpp`, `Eff/private/effPlainEffectVk.cpp` (port of `effPlainEffect`, decision 8)
- Test: `Vk/fxEffectVkTest.cpp`, `Vk/effPlainEffectVkTest.cpp`

**Interfaces — Consumes:** Tasks 2–4. **Produces:**
- `fxEffectVk::Apply(int i_Technique, int i_Pass)`:
  - sets the pass's shaders on `vkdGlobal::Context()`;
  - uploads each constant buffer the pass reads, and sets it at its slot (`Bindings().SetConstantBuffer(slot, address, size)`);
  - sets textures (`SampledIndex()`, or an internal 1×1 black image for NULL), samplers and UAVs (`StorageIndex()`) at their slots, for the stages that read them.
  - A deferred pass logs `GVK_NOT_YET("<effect>/<technique>/<pass> (hair hull shader)")` and binds nothing; the draw that follows draws nothing, because the context has no VS.
- A constant buffer is uploaded on Apply when either holds: it is dirty, or its last upload was before the current submission (`vkdDevice::PendingValue()` has advanced since). Otherwise the previous address is reused.
- **Samplers**, from the manifest:
  - Filter names → min/mag/mip filter and compare.
  - `ANISOTROPIC` → linear + `anisotropyEnable` (`maxAnisotropy` from the manifest).
  - Addresses: `WRAP`→`REPEAT`, `MIRROR`→`MIRRORED_REPEAT`, `CLAMP`→`CLAMP_TO_EDGE`, `BORDER`→`CLAMP_TO_BORDER`, `MIRROR_ONCE`→`MIRROR_CLAMP_TO_EDGE`.
  - Border (0,0,0,0) → transparent black and (1,1,1,1) → opaque white; any other colour fails `Create` naming it (the manifests use only these two).
  - `MinLOD` → `max(MinLOD, 0)`; `MaxLOD ≥ FLT_MAX` → `VK_LOD_CLAMP_NONE`.
  - Each goes to `Bindings().AddSampler`.
- `effPlainEffectVk`: same public members as `effPlainEffect`, on `fxEffectVk`, with the types from decision 8; `GetEffectVk()` replaces `GetEffectDX11()`.

- [ ] **Step 1: Failing tests:**
  - `fxEffectVkTest`:
    - `AllEffectsCreateOnTheDevice`: every embedded effect is created with zero validation messages; the deferred passes are reported.
    - `SamplerMapping`: a table of manifest states → the expected `VkSamplerCreateInfo` fields.
    - `ConstantsReachTheShader`: Special/Solid, or the simplest effect with a colour constant (found by reflection). Set the colour, Apply, draw a full-screen triangle with the context, and the readback equals the colour.
    - `UnchangedConstantsSurviveRingReuse`.
    - `MaterialInstanceRestores`: two `fxMaterialInstance`s on one effect, each captured with a different colour; restore one, draw, and its colour is the one read back.
    - `TextureAndSamplerBound`.
  - `effPlainEffectVkTest`, over every effect against `effPlainEffect` on the D3D11 reference device (DX11 is linked since Task 2):
    - `SameVariables`: by index, name and semantic, with the annotations (names, types, strings and values) and type descriptions.
    - `SameTechniquesAndPasses`.
    - `InvalidLookupsAreInvalidObjects`.
    - `SettersRoundTrip`: for each scalar/vector/matrix variable, set through the API and get it back on both, with equal bytes in the constant buffer.
- [ ] **Step 2: Build; fail.** **Step 3: Implement.** **Step 4: Pass**, with zero validation messages.
- [ ] **Step 5: Commit:** "GraphicsVulkan: fxEffectVk draws through vkdContext; fxEffect API (effPlainEffectVk)".

---

### Task 6: State holders, the full-screen quad, and image parity with DX11

**Files:**
- Create: `G3d/g3dBlendStateMgr.hpp`, `g3dDepthStencilStateMgr.hpp`, `g3dRasterizerStateMgr.hpp`, `g3dStateMgr.hpp` + `private/*.cpp` (ports: same namespaces and function names, Vulkan enums (decision 7), the same defaults as DX11's `Init()`; the objects are plain descriptions handed to `vkdContext`)
- Create: `G2d/g2dFullscreenQuad.hpp`, `G2d/private/g2dFullscreenQuad.cpp`, `G2d/private/Shaders/FullscreenQuadVS.hlsl` and `TexturedQuadPS.hlsl` (decision 3; embedded by a second invocation of `EmbedShadersVk.ps1` over a manifest `G2d/private/Shaders/Quad.effect.json`, which fixes their sampler state as DX11's code does: linear, clamp, MinLOD 0)
- Modify: `G3d/private/g3dSystemVk.cpp` (Init/CleanUp the state holders and the quad, where `g3dSystemDX11` does)
- Test: `UnitTests/GraphicsTests/gtImage.hpp/.cpp`; `vcpkg.json` (+`stb`); `Vk/g2dFullscreenQuadTest.cpp`, `Vk/fxParityTest.cpp`

**Interfaces — Produces:**
- `gvk::g2dFullscreenQuad::{InitFullscreenQuad, CleanUpFullscreenQuad, DrawFullScreenQuad(uint32_t w, uint32_t h, float offsetX = 0, float offsetY = 0, SCREEN_VERTEX* i_pVerts = NULL), DrawTexturedQuad(vkdImage* i_pTexture, uint32_t w, uint32_t h, float offsetX = 0, float offsetY = 0)}`. These have the same behaviour as DX11's, including saving and restoring the viewport. Custom vertices go through `vkdContext::Upload`.
- `gtImage`:
  - `struct gtCompareResult { float m_MaxErr, m_MeanErr, m_PctOver; bool m_bPass; std::string Describe() const; }`
  - `gtCompareResult gtCompareImages(const uint8_t* a, const uint8_t* b, int w, int h, float maxErr = 2/255.f, float pixelErr = 1/255.f, float maxPctOver = 0.1f)`
  - `void gtSaveComparison(const std::string& i_Test, a, b, w, h)` writes the PNGs (Global Constraints).

- [ ] **Step 1: Failing tests:**
  - `gtImage.Metrics`: hand-made 2×2 images give known max/mean/% values.
  - `g2dFullscreenQuad`, Vulkan against the DX11 reference (`gtD3D11::SetGlobals`, then DX11's own `g2dFullscreenQuad`):
    - `TexturedQuadMatchesDX11`: a 64×64 gradient texture drawn at 128×96 with an offset.
    - `CustomVerticesRestored`: a draw with custom vertices, then a default draw, gives the same pixels as DX11's same sequence.
  - `fxParity.FullScreenPassesMatchDX11`, parameterized over every embedded effect and every technique and pass whose:
    - VS inputs ⊆ {`SV_POSITION`/`POSITION` float4, `TEXCOORD0` float2},
    - PS writes only `SV_Target0`,
    - textures are all 2D, with no UAVs or buffers.

    For each such pass:
    - Bind the same 64×64 RGBA8 gradient to every texture, and keep the manifest defaults.
    - Set every matrix constant to the same non-symmetric matrix and every other float constant to its default (or 0.5 where the default is 0).
    - Draw a 64×64 full-screen quad into a target cleared to (0.1,0.2,0.3,1), with DX11's default states on both.
    - Compare with the tolerance.

    Passes that don't qualify are listed in the test output, with a count. The exception table starts empty; each entry needs a reason. Expected: at least 40 passes compared (record the number).
- [ ] **Step 2: Build; fail.** **Step 3: Implement** the holders and the quad.
- [ ] **Step 4: Run the tests.**
  - A failing pass is investigated: reflection, constants, sampler, Y flip, or DXC-vs-fxc floating point. Fix it, or add an exception with its measured error and reason; record each in the ledger.
  - Also run the whole GraphicsTests suite once with `gtVulkan`'s GPU-assisted validation forced on (a temporary local switch, not committed), and record the result.
- [ ] **Step 5: Commit:** "GraphicsVulkan: state holders and full-screen quad; full-screen effect passes match DX11".

---

### Task 7: CI, docs, roadmap; stop for review

**Files:**
- Modify: `Tools/fx2hlsl/compile_check.py` (`--spirv` adds the Global Constraints options; new `--exclude-entry NAME`, repeatable), `Tools/fx2hlsl/README.md`
- Modify: `.github/workflows/build-windows.yml` (a step in `converted-shaders`):
  - install DXC with `vcpkg install --x-manifest-root=LibXLT/GraphicsVulkan --x-install-root=<temp>`, so the version matches the build;
  - then `compile_check.py --compiler dxc --spirv --exclude-entry Hair_HS` over `LibXLT/GraphicsVulkan/Eff/private/Shaders`.
- Modify: `CLAUDE.md`:
  - GraphicsVulkan packages: `Fx`, `Eff` (plain effects so far), `G2d` quad, `G3d` state holders, and the `vkd` context and pipeline cache;
  - the cache file;
  - GraphicsTests now links GraphicsDX11 as the reference;
  - `GraphicsTestsImages/`.
- Modify: `docs/superpowers/plans/2026-10-17-graphics-vulkan-roadmap.md` (Plan 3 done, with the status notes; fonts moved to Plan 4 in both rows) and `2026-10-17-gfx-vulkan-4-first-scene.md` (adds fonts and the text parity test to its scope).

- [ ] **Step 1:** Run `python Tools/fx2hlsl/compile_check.py --compiler dxc --exe <vcpkg dxc> --spirv --exclude-entry Hair_HS (all Vk manifests)`. Expected: `753 of 753 entry point(s) compiled`.
- [ ] **Step 2:** Edit the docs. Then do a full six-configuration build with all tests passing, and a `-gfx vulkan -new` start and exit (Debug Qt) with `Vulkan validation: 0 errors, 0 warnings`. Run the DX11 harness on the test scene, DX11 vs DX11, and check it is unchanged.
- [ ] **Step 3: Commit:** "GraphicsVulkan: SPIR-V shader check in CI; docs and roadmap: Plan 3 done".
- [ ] **Step 4: Stop for review.** Report:
  - what was built;
  - the reflection and parity results (passes compared, any exceptions with their errors);
  - validation counts;
  - the DXC flags as finally used;
  - the features added;
  - the remaining `GVK_NOT_YET`;
  - surprises;
  - rulings.

  Don't write Plan 4 in detail until the owner says so.
