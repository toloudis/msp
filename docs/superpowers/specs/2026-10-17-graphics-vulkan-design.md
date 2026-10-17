# GraphicsVulkan: a Vulkan backend for MachStudio — design

Date: 2026-10-17
Status: draft for review

## Why

MachStudio's renderer is Direct3D 11 (`LibXLT/GraphicsDX11`). The goal is to
modernize the graphics core: add a Vulkan backend, prove it produces the same
images, and eventually retire DX11. Performance matters on both CPU and GPU.
Vulkan must never be slower than DX11, and should become faster. Viewport
beauty renders with several reflective objects are the known hot spot: each
reflection is another pass over the scene's geometry.

The work happens in two phases:

1. **Port.** A faithful port of every GraphicsDX11 renderer to Vulkan, with
   image-comparison tests and timing on both backends from day one.
2. **Optimize.** Structural changes on the Vulkan side only, chosen by
   measuring (see "Phase 2 direction").

This document specifies Phase 1 in full and Phase 2 only as direction.

## Goals

- New static library `LibXLT/GraphicsVulkan` that implements the same
  `LibXLT/Graphics` abstractions that GraphicsDX11 implements.
- A command-line flag chooses the backend when MachStudio starts.
- Every renderer GraphicsDX11 registers has a Vulkan version (list below).
- Renders match DX11 within a small, measured error.
- Timing (GPU per pass, CPU per frame) is available on both backends, so
  "not slower" is a measurement, not a hope. Phase 1 may end somewhat
  slower than DX11; Phase 2 must close the gap (see Decided item 8).
- Works in all six configurations (Debug/Release × Qt/Wx/RenderNode).

## Non-goals (Phase 1)

- Performance restructuring (draw packets, persistent descriptors,
  multi-threaded recording). These belong to Phase 2.
- Changing the look of anything. Any visual difference is a bug.
- Supporting platforms other than Windows, although nothing should
  needlessly block it (Vulkan code goes through the PAC rules like
  everything else).
- A preferences UI for the backend choice.

## Anti-goals (a failure even if it "works")

- **Any edit to GraphicsDX11 rendering behaviour.** DX11 is the reference
  and stays frozen. The only allowed GraphicsDX11 changes are the
  measurement hooks below, compiled out by default.
- **Any edit to the existing shader sources.** Shaders are copied into
  GraphicsVulkan, and the copies stay identical (hair is the one exception;
  see Shaders).
- Silent fallback: if `-gfx vulkan` was asked for and Vulkan can't start,
  MachStudio must say so and exit. It must not quietly render with DX11,
  because that would make a comparison report lie.
- Making the executable depend on Vulkan being installed when DX11 is
  selected.

## Constraints and facts from the code

- **The seam already exists.** Only
  `MachStudio/Source/MainApp/mainInitGraphics.cpp` names GraphicsDX11
  (`GraphicsDX11Layer::Init / InitGraphics / CleanUpGraphics / CleanUp`,
  `GetSystem2D / GetSystem3D`). Everything else uses the abstract hooks in
  `LibXLT/Graphics`, installed by `SetImplementation` calls:
  - `g2dSystem`, `g3dSystem`
  - `g2dResourceCounter`, `g2dFontUtil`, `g2dImageCreate`, `g2dImageSave`,
    `g2dScreenCaptureUtil`
  - `g3dLightMgr`, `matTextureMgr`, `matShaderMgr`
  - `g3dSceneRendererCreate`, `g3dBake`
  - `g3dFragmentCreate` (bump), `mdlFragCreate` (may)
- **Size.** GraphicsDX11 is about 76k lines. `shdw` (the multi-pass
  renderers) is about 25k lines, of which only about 5% touch D3D directly.
- **Draw path.** Scene geometry is drawn in `bumpBumpRenderer::Render`:
  input layout, vertex/index buffers, `g3dStateMgrDX11::SetDeviceStates()`,
  then `matShaderEffect::Begin/BeginPass`, which binds shaders and
  constants, then `DrawIndexed`. Blend, depth and rasterizer state come from
  the `g3d*StateMgr` classes. There are only a handful of draw/dispatch
  sites in total; there are 91 `Map` calls (dynamic constant and vertex
  updates).
- **Shaders.** Effects are plain HLSL described by `*.effect.json`
  (techniques, passes, per-stage entry points and profiles, sampler states),
  under `GraphicsDX11/Eff/private/Shaders`. `EmbedShaders.ps1` compiles them
  with fxc at build time into `Fx/private/Generated/fxEmbeddedShaders.inc`.
  `fxEffectDX11` uses `D3DReflect` to find constants and resources. A few
  shaders (fonts, full-screen quad, tone map, ray tracing, LPV) are compiled
  at run time with `D3DCompile`.
- **Shader feasibility, checked.** All `*.effect.json` stages were compiled
  with the Vulkan SDK's DXC (`-spirv`, SM 6.0, register shifts, no source
  edits): 753 of 756 compile. The 3 failures are the hair hull shaders
  (`Hair_SH`, `HairDefault`, `OpacityRender` → `Hair_HS`), because DXC's
  SPIR-V backend doesn't implement `ProcessIsolineTessFactors`.
  `Tools/fx2hlsl/compile_check.py` already supports `--spirv`.
- Shaders use geometry, hull and domain shaders, compute, UAVs, MSAA
  resolve, `GenerateMips`, queries, and one deferred context.
- **Rendering runs on a render thread** (`g3dThreadControl`). Several
  windows (render panels) each have their own swap chain.
- **Startup order.** `mnmApp::ParseCommandLine` runs *after* graphics are
  initialized (`WinMain.cpp`), so the backend flag must be read earlier.
- **Environment.** The Vulkan SDK 1.4.350 (with SPIR-V-capable DXC) is
  installed on the development machine; the GPU is a Quadro RTX 5000.
  CI (`windows-latest`) has no GPU.

## Decided

These were decided with the project owner:

1. **Two phases:** port first, then optimize on the Vulkan side only.
2. **Copy, don't share.** GraphicsVulkan copies whatever code and shaders it
   needs from GraphicsDX11. Nothing is factored out of GraphicsDX11.
3. **Shaders are copied unchanged** and compiled by DXC to SPIR-V (SM 6).
   All compatibility work is done with compiler options, not source edits.
4. **Hair** may use any hack that works (for example a Vulkan-only copy of
   the hull shader with the isoline factor computation written by hand), or
   be deferred if the hack becomes ugly.
5. **Scope:** every renderer GraphicsDX11 has — default, HDR, simple,
   texture, depth map, RSM, opacity map, cube map reflection, planar
   reflection, ambient occlusion, global illumination (including LPV and
   the AO/GI volumes), depth, UV, pick, shadow mask, illumination, normal,
   velocity map, materials, reflection only, glow — plus bake, fonts, image
   save, screen capture, and the bump, may and hair fragment paths.
6. **Backend switch:** a command-line flag. No preferences UI.
7. **Measurement from day one, on both backends.** DX11 measurement code is
   allowed if it compiles out (or has no measurable cost) in release builds.
8. **Phase 1 exit:** every renderer's images match within tolerance, the
   timing report exists for both backends, and DX11 still works exactly as
   before. Vulkan may be somewhat slower at the end of Phase 1. Vulkan does
   not become the default backend until Phase 2 makes it at least as fast
   as DX11 on the test scene, on both CPU and GPU.
9. **Proof of parity:**
   - The owner's animated test scene, rendered by RenderNode with each
     backend over the same frames and compared per frame.
   - Synthetic-scene image tests in GoogleTest, comparing the two backends.

## Design

### Library layout

- `LibXLT/GraphicsVulkan/GraphicsVulkan.vcxproj`, a static library in
  `MachStudio.sln`, built in all six configurations, referenced by MainApp.
- It mirrors GraphicsDX11's packages (`G2d`, `G3d`, `Mat`, `Eff`, `Fx`,
  `shdw`, `bump`, `tmesh`, `hair`, `may`), with the usual
  public/`private/` split and `GraphicsVulkanLayer` with the same five
  functions as `GraphicsDX11Layer`.
- **Name collisions.** Both libraries link into one executable, and many
  GraphicsDX11 classes have backend-neutral names (`shdwPackage`,
  `tmeshFrag`, `bumpBumpRenderer`, `fxEffectDesc`, ...). Copied code goes
  inside one C++ namespace (`gvk`) so it can keep its names; include guards
  get a `GVK_` prefix. Classes that GraphicsDX11 names `...DX11` become
  `...Vk`. *(Builder's call; the requirement is no ODR or link clashes.)*
- A new internal package, `vkd` ("Vulkan device"), replaces
  `g2dDX11Global`. Ported code uses it instead of calling Vulkan directly
  wherever practical:
  - instance, physical device, logical device and queues; one surface and
    swap chain per `g2dWindow`;
  - frames in flight, command buffers, synchronization (sync2), deferred
    destruction;
  - memory through Vulkan Memory Allocator;
  - **a pipeline cache keyed by the full state**: shader set, vertex
    layout, topology, blend, depth/stencil, rasterizer, render target
    formats and sample count. Phase 1 looks pipelines up at draw time. The
    key is designed so Phase 2 can look them up once at object setup
    instead, without changing callers. A `VkPipelineCache` is saved to disk
    to keep startup fast;
  - **a per-frame ring buffer for constants and dynamic vertex data**, which
    replaces `Map(WRITE_DISCARD)`;
  - resource binding by DX-style slot (b/t/s/u) using push descriptors, so
    the ported code keeps DX11's binding model;
  - the state managers (`g3dBlendStateMgr`, `g3dDepthStencilStateMgr`,
    `g3dRasterizerStateMgr`, `g3dStateMgr`) become small state holders that
    feed the pipeline key.
- **Minimum device:** Vulkan 1.3 (dynamic rendering, synchronization2)
  plus `VK_KHR_push_descriptor`, geometry and tessellation shaders.
- **Loading:** Vulkan is loaded dynamically (volk), so the executable runs
  on a machine without a Vulkan driver as long as DX11 is selected.
- Conventions that keep images identical: negative viewport height for
  D3D's Y direction, depth range 0..1, same pixel centers, same texture
  formats where Vulkan has them (any substitution is documented in code and
  tested).

### Shaders

- The HLSL and `*.effect.json` files are copied to
  `GraphicsVulkan/Eff/private/Shaders`, unchanged.
- A build step (a copy of `EmbedShaders.ps1` that calls DXC with `-spirv`)
  compiles every stage to SPIR-V and embeds it the way the DX11 build does.
  `_5_0` profiles become `_6_0`; register spaces are mapped with
  `-fvk-{b,t,s,u}-shift`; `-fvk-use-dx-layout` keeps constant buffer
  offsets identical to fxc's.
- Reflection uses SPIRV-Reflect in place of `D3DReflect`. The rest of
  `fxEffect` and `fxMaterialInstance` is ported as-is, so parameter names,
  indices and the `matShaderEffect` interface behave exactly as on DX11.
- Shaders compiled at run time (fonts, full-screen quad, tone map, ray
  tracing, LPV) are compiled to SPIR-V at build time too, if they don't
  depend on run-time values. If one does, it is compiled at run time with
  `dxcompiler.dll`. *(Builder's call.)*
- Hair: see Decided item 4. The hack lives only in GraphicsVulkan's copy,
  in a file whose name makes clear it is a Vulkan-only shader.
- CI gains a `compile_check.py --compiler dxc --spirv` step over
  GraphicsVulkan's effects.

### Backend switch

- Flag: `-gfx dx11` or `-gfx vulkan`, default `dx11`.
- Because `ParseCommandLine` runs after graphics start, the flag is read by
  a small pre-scan of the command line before `mainInitGraphics` is
  constructed, in all three variants (Qt, Wx, RenderNode). It is also
  listed in `-help`.
- `mainInitGraphics.cpp` calls `GraphicsDX11Layer` or
  `GraphicsVulkanLayer` through a small backend selector.
- If Vulkan can't start (no driver, missing features), MachStudio reports
  exactly why (a message box, or stderr in RenderNode) and exits with an
  error. It never falls back silently.
- The active backend is written to the log and shown in the window title or
  status bar, so screenshots and reports can't be mixed up. *(Builder's
  call on where it is shown.)*

### Measurement

- A small backend-neutral interface in `LibXLT/Graphics` (for example
  `g3dProfiler`): scoped CPU timers and GPU timer markers by pass name, with
  per-frame totals. Both backends implement the GPU side (DX11 timestamp
  and disjoint queries; Vulkan timestamp query pools).
- Pass markers are placed at the same logical points in both libraries:
  per renderer, per `shdw` pass, per reflection target, plus frame total,
  CPU submit time and draw count.
- In GraphicsDX11 the markers are macros that compile to nothing unless
  `MSP_GFX_PROFILING` is defined. It is defined in Debug and in a Release
  build only when asked for (an MSBuild property). In GraphicsVulkan the
  same switch applies.
- Output: a command-line flag `-gfxProfile <file.csv>` writes one row per
  frame per pass. RenderNode runs of the test scene therefore produce a
  timing CSV per backend that can be compared directly.

### Proving parity

**1. Test scene, through RenderNode** (primary sign-off)

- The owner supplies a non-trivial animated scene.
- A script (`Tools/gfxcompare/`) runs RenderNode twice over the same frame
  range (`-gfx dx11`, then `-gfx vulkan`) to lossless output (EXR or PNG),
  then compares frame by frame and writes a report: per-frame max
  per-channel error, mean error, percentage of pixels over a threshold, a
  difference image for each failing frame, and the two timing CSVs side by
  side.
- Tolerance: start strict (for example max channel error ≤ 2/255 on 8-bit
  output, with ≤ 0.1% of pixels over 1/255) and loosen per feature only
  with a written reason. DXC and fxc compile floating-point code
  differently, so exact equality is not expected. *(Builder's call on the
  exact numbers, recorded in the report.)*

**2. Synthetic-scene image tests**

- New GoogleTest project `UnitTests/GraphicsTests`, set up like CoreTests
  (self-contained project, vcpkg gtest, post-build run, ProjectReference
  from MainApp).
- Each test builds a small scene in code, renders it offscreen through each
  backend, and compares the results with the same metrics as above. The
  set covers each renderer and each material effect at least once: lit
  spheres per material, planar and cube-map reflection, projected and point
  light shadows, AO, GI, glow, DOF, tessellated displacement, transparency,
  depth/normal/UV/velocity outputs, pick buffer, fonts.
- If no Vulkan device is present (CI), the Vulkan side of each test is
  skipped, not failed. The DX11 side still runs. *(Builder's call: DX11
  runs on WARP in CI if it works; a CPU Vulkan implementation such as
  SwiftShader may be added later.)*
- A failing comparison saves both images and the difference image next to
  the XML report.

### Build and dependencies

- vcpkg manifest `LibXLT/GraphicsVulkan/vcpkg.json`: `volk`,
  `vulkan-headers`, `vulkan-memory-allocator`, `spirv-reflect`,
  `directx-dxc` (the build-time SPIR-V compiler, and `dxcompiler.dll` if
  run-time compilation is needed). This keeps CI free of a Vulkan SDK
  install. *(Builder's call; the Vulkan SDK via `VULKAN_SDK` is the
  fallback if a package is missing or too old.)*
- MainApp adds the library's `vcpkg_installed` paths and copies its DLLs,
  as it does for GraphicsDX11.
- Validation layers are enabled in Debug builds when present
  (`-gfxValidation` forces them on in any build).
- `CLAUDE.md` gains a GraphicsVulkan section and the new flags.

### Order of work (suggested)

1. Measurement interface and DX11 markers; the comparison script running
   DX11 against itself (proves the harness).
2. GraphicsVulkan skeleton, backend switch, `vkd` device, swap chain, clear
   screen in the Qt, Wx and RenderNode builds.
3. Shader build step and reflection; full-screen quad and fonts.
4. tmesh/bump fragments, textures, the default renderer and one material
   type; first image comparison passes.
5. The remaining material effects and the `shdw` passes, one renderer at a
   time, each landing with its synthetic test.
6. Bake, pick, screen capture and image save; hair last.
7. Test scene passes; timing report for Vulkan vs. DX11.

## Phase 2 direction (not specified here)

Chosen by measurement once Phase 1 is complete, all Vulkan-side only:

- Per-object draw packets built once (pipeline, descriptors, buffers) and
  replayed per pass. Area18's `rndrDrawCall`/`rndrPass` list is the sketch
  of this idea.
- Persistent descriptor sets and constant data that only change when the
  object changes, instead of per-draw uploads.
- Recording reflection and shadow passes on several threads with secondary
  command buffers.
- Less render-target round-tripping between `shdw` passes.

## Left to the builder

Items marked *(Builder's call)* above, plus: internal class and file names
inside GraphicsVulkan, how much of `vkd` to build before porting begins, the
synthetic scene contents, and the report format of the comparison tool.
