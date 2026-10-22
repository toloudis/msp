# CLAUDE.md

Guidance for working in this repository. It covers the two source trees,
`LibXLT/` (the shared engine/framework libraries) and `MachStudio/` (the
MachStudio Pro application).

MachStudio Pro is a Windows real-time 3D lighting, rendering and cinematography
application (originally StudioGPU / Extra Large Technology, 2003–2010). Users
load scenes exported from DCC tools (Maya, Max, FBX, glTF), light them, animate
cameras/lights/objects on a timeline, and render frames or sequences on the GPU
(Direct3D 11), with export to RenderMan.

## Building

- Visual Studio, **x64 only**. Main solution: `MachStudio/Source/MachStudio.sln`
  (contains the MachStudio projects plus the LibXLT projects it uses).
- There are six configurations, Debug/Release × **Qt / Wx / RenderNode**; the
  **configuration selects the GUI toolkit** (see the comments in
  `Directory.Build.props` and `MachStudio/Source/Directory.Build.targets`):
  - `Debug Qt` / `Release Qt` — Qt 6 UI (`LibXLT/ToolUIQt`, `qtGUI/` folders).
    Qt is found via `MspQtDir`, then `QTDIR`, else `C:\Qt\6.9.3\msvc2022_64`
    (`LibXLT/ToolUIQt/qt6.props`). `windeployqt` runs after linking.
  - `Debug Wx` / `Release Wx` — wxWidgets UI (`LibXLT/ToolUIWx`, `wxGUI/`
    folders, `3rdParty/WxWidgets`).
  - `Debug RenderNode` / `Release RenderNode` — headless command-line renderer
    (e.g. for a render farm) with **no GUI toolkit**: everything is compiled
    with `BATCH_MODE`, which turns off `USE_WXWIDGETS`/`USE_QT`; MainApp is a
    console exe (`_tmain` in `WinMain.cpp`) that hides the native window it
    creates, doesn't render the viewport, and exits unless the command line
    starts a capture. ToolUIWx still builds (its `wui` package supplies the
    `Tool/gui` implementations, with wx compiled out); neither wx nor Qt is
    linked. Its settings are the Wx configuration's plus `BATCH_MODE`, minus
    the wx property sheet; keep them in step when changing Wx settings.
  - A build sees only its own toolkit: `wxGUI/**` sources are compiled only in
    Wx builds and `qtGUI/**` sources only in Qt builds (RenderNode compiles
    neither). Any toolkit-specific UI code must live in a matching `qtGUI/` or
    `wxGUI/` folder, and both variants should be kept in step. Code outside
    those folders reaches dialog code only under `#ifndef BATCH_MODE` (or
    `USE_WXWIDGETS`/`USE_QT`), and a `wxGUI/` header keeps its
    `#include <wx/...>` lines inside `#ifdef USE_WXWIDGETS` so toolkit-neutral
    code can include it in any build.
  - Projects that don't depend on the toolkit (Core, Graphics,
    GraphicsDX11, GraphicsVulkan, Tool, Input, InputTPC, AudioDS,
    ImportExport and the UnitTests projects) have only plain `Debug` and
    `Release` configurations; `MachStudio.sln` maps the three toolkit
    configurations onto them, so they build once per Debug/Release, into
    `bin\x64\Debug\` or `Release\`, and are shared by the Qt, Wx and
    RenderNode builds. Build through the solution (a project built on its
    own passes its configuration name to its ProjectReferences). Their
    code must not use `BATCH_MODE`, `USE_QT`, `USE_WXWIDGETS` or
    `MSP_GUI_*`; a project that does keeps the six configurations. Don't
    run two builds of the same base configuration (e.g. `Debug Qt` and
    `Debug Wx`) at the same time: they write the same files.
- Executables land in `MachStudio/MachStudio-x64/` (e.g.
  `MachStudio-x64-Qt-Release.exe`, `MachStudio-x64-RenderNode-Release.exe`),
  alongside runtime data and DLLs.
- Third-party libraries come from `3rdParty/`, from vcpkg, or (Qt) from a
  separate install; see Dependencies below.
- Many directories still contain obsolete `.vcproj` (VS2005/2008), `.vspscc`
  and `.8.00.old` files. Only `.vcxproj` files are live; a `System` folder
  with only a `.vcproj` is not built by the current solution.
- Tests: GoogleTest unit tests live in `UnitTests/` (see Unit tests below).
  The older harnesses in `MachStudio/TestSource/` (obsolete `.vcproj`s;
  mostly interactive demos or print-and-eyeball programs) are not built;
  what was worth keeping has been ported to `UnitTests/`, and they are due
  to be deleted.
- Graphics profiling: `MSP_GFX_PROFILING` (defined in Debug, and in Release
  when built with `/p:MspGfxProfiling=true`; see `Directory.Build.props`)
  turns on the `G3D_PROFILE_*` markers (`Graphics/G3d/g3dProfiler.hpp`) in
  the renderers and passes and the GPU timers; without it they compile to
  nothing. MachStudio's `-gfxProfile <file.csv>` then writes CPU/GPU time
  and draw count per scope per frame. Markers are named after the class
  without its package prefix and `DX11` suffix (`shdwPassLit` → `PassLit`).

### Build files

Dependencies between projects are plain ProjectReferences in the
`.vcxproj` files (visible in Visual Studio). The shared MSBuild files only
hold settings:

| File | Applies to | What it does |
|---|---|---|
| `Directory.Build.props` (repo root) | every project, automatically | Works out `MspGuiToolkit` (Qt/Wx/None) and `MspBaseConfiguration` (Debug/Release) from the configuration name; wx include dirs; defines `MSP_GFX_PROFILING` in Debug or with `MspGfxProfiling=true`. `3rdParty/WxWidgets/build/msw/wx_setup.props` reads `MspBaseConfiguration` |
| `MachStudio/Source/Directory.Build.targets` | MachStudio projects, automatically | Drops `wxGUI/` or `qtGUI/` sources for the other toolkit; imports `qt6.props` and runs `windeployqt` for Qt |
| `LibXLT/ToolUIQt/qt6.props` | Qt builds (ToolUIQt, and MachStudio projects through the targets above) | Qt include dirs, defines and libraries (no moc/uic step) |
| `3rdParty/WxWidgets/wxwidgets.props` | Wx builds of the projects that import it (ToolUIWx, Support, Features, MainApp, ...) | wx libraries |
| `vcpkg.json` next to a project | that project | Its vcpkg packages (see Dependencies) |

### Unit tests

- `UnitTests/CoreTests/CoreTests.vcxproj` is a console exe of GoogleTest tests
  for `LibXLT/Core`, in `MachStudio.sln`. It links `Core` through a
  ProjectReference and gets gtest from its own vcpkg manifest. Tests sit in
  folders named after the package they test (`Ch/chChunkTest.cpp`,
  `Ma/maMatrixTest.cpp`, ...); helpers use the `ct` prefix (`ctTempDir` for a
  per-test temp directory, `ctMath` for approximate vector/matrix checks).
- `UnitTests/MainAppTests` tests toolkit-neutral MainApp code
  (`mainCommandLine`); MainApp is an exe, so the sources under test are
  compiled straight into the test exe (and marked `NotUsing` for MainApp's
  precompiled header). Helpers use the `mat` prefix.
- `UnitTests/GraphicsTests` tests `LibXLT/Graphics` code that needs no GPU
  (`g3dProfiler`) and `LibXLT/GraphicsVulkan` (`Vk/`); it links Core,
  Graphics, GraphicsVulkan and GraphicsDX11, the reference the Vulkan
  port is compared with (`Dx11/gtD3D11`: a D3D11 device on the hardware
  adapter, else WARP). Helpers use the `gt` prefix. The Vulkan tests run
  with validation on (`gtVulkan` fails a test on any validation message)
  and are skipped when there is no usable Vulkan device; the tests that
  need only reflection run everywhere. CI builds them but doesn't run them
  (`/p:MspRunGraphicsTests=false` turns the post-build run off): GitHub's
  runners have no GPU they can rely on. `gtBackend` runs the
  same test code under the whole GraphicsDX11 layer and then the whole
  GraphicsVulkan layer, as MachStudio starts them, so a test can compare
  what Graphics-level calls (`matTextureMgr`, ...) give on each, all in
  one process (the run starts COM once, as MachStudio does: DirectXTex
  keeps one WIC factory per process). Parameterized suites with many
  small tests share one device per suite (`Vk/gtVulkanSuite`, or a
  `SetUpTestSuite` D3D11 device) and still check validation per test.
  Image comparisons
  (`gtImage`, gfxcompare's metrics and tolerance) write `dx11.png`,
  `vulkan.png` and `diff.png` to
  `UnitTests/bin/x64/<Debug|Release>/GraphicsTestsImages/<test>/` when they
  fail. A custom build step compiles `Vk/Shaders/*.hlsl` with DXC. The
  test run keeps the user data folder (`gfPaths` `e_UserDataPath`) in a
  temp directory, one per shard.
- `CoreTests.vcxproj` is self-contained: its build settings (include paths,
  runtime, output in `UnitTests/bin/<Platform>/<Debug|Release>/`) are in the
  project file, not a shared props file. A new test project can start as a
  copy of it.
- The test projects, like the libraries they test, have only `Debug` and
  `Release` (they don't use a GUI toolkit); every solution configuration
  builds and runs them once per Debug/Release.
- `CoreTestsMain.cpp` initializes `CoreLayer` once and installs a
  `dbgMsg::SetAssertHandler` hook so a failed `DBG_ASSERT` becomes a test
  failure instead of the abort/continue dialog. `ctPlatform` turns CRT
  assert and crash dialogs off, so runs never wait for input.
- Run: `UnitTests/bin/x64/<Debug|Release>/CoreTests.exe` (gtest flags such as
  `--gtest_filter=ma*` work); Visual Studio's Google Test adapter should also
  list them in Test Explorer.
- The tests run as part of the build: the test project's Post-Build Event
  (Project Properties → Build Events) runs the exe after linking and writes
  `<TargetName>.xml` next to it; a failing test fails the build.
  GraphicsTests' step runs it through `UnitTests/RunShards.ps1`: up to 8
  gtest shards at once (`GTEST_TOTAL_SHARDS`/`GTEST_SHARD_INDEX`), their
  output printed in turn and their XML merged into the one report. MainApp
  has an ordinary ProjectReference to each test project (nothing linked),
  so MachStudio isn't built until the tests pass. A new test project gets
  the same reference in `MainApp.vcxproj` and in `GraphicsTests.vcxproj`
  (so it isn't copying its vcpkg DLLs into the shared output folder while
  GraphicsTests' shards have them loaded), and may reference whatever
  libraries it tests. CI builds `MainApp;CoreTests;MainAppTests;GraphicsTests`
  in every configuration, runs every test project but GraphicsTests, and
  uploads the XML reports.
- Core headers have `#error` "multiply included" guards, so guard their
  includes in test files as everywhere else (`#ifndef MA_PLANE_HPP ...`).

### Dependencies

**From `3rdParty/`** (checked in, prebuilt; `3rdParty/thridParty.txt` has old
build notes):

| Library | Location | Used for |
|---|---|---|
| wxWidgets 3.2 | `WxWidgets/` (headers, import libs; `wxwidgets.props`) | Wx UI builds only (DLLs in `MachStudio-x64/`) |
| libpng 1.6 | headers `Png/lpng162`, lib/DLL `Libs/x64/libpng16` | image loading (`g2dSurfaceLoaderDX11`) |
| zlib + minizip | headers `Png/zlib-1.2.5`, lib/DLL `Libs/x64/zlibwapi` (zlib 1.2.8 sources in `zlib/`) | vertex-animation compression (`vtx`), zip (`cmpr`) |
| Python 3.9 | `Python/include` (headers only, not in git) | embedded Python; compiled out unless `PYTHON_ENABLED` is defined, which no live configuration does |

Unused leftovers in `Libs/x64`: `jpeg.lib`, the 2011
`fbxsdk` libs (FBX import/export needs `USE_FBX_IMPORTEXPORT`, which no
configuration defines), `python26/27`, `ATITessellation*`.

**From vcpkg** (manifest mode, `VcpkgEnableManifest`; each project installs into
its own `vcpkg_installed/`):

| Manifest | Packages | Used for |
|---|---|---|
| `LibXLT/Graphics/vcpkg.json` | OpenEXR (+ Imath, libdeflate) | writing `.exr` render output (`g2dScanlineOpenEXRUtil`, `g2dOpenEXROutStream`, `g2dHDRUtil`) |
| `LibXLT/GraphicsDX11/vcpkg.json` | DirectXTex, nlohmann-json, libtiff (+ libjpeg-turbo, liblzma, zlib) | DDS/texture loading and image saving; parsing `*.effect.json` effect descriptions (`fxEffectDesc`); TIFF loading (`g2dSurfaceLoaderDX11`; MainApp links `tiff.lib`/`tiffd.lib` from here). glTF import bundles its own `json.hpp`/`tiny_gltf.h` |
| `LibXLT/GraphicsVulkan/vcpkg.json` | vulkan-headers, volk, vulkan-memory-allocator, spirv-reflect, spirv-tools, directx-dxc, stb, libtiff | the Vulkan backend: volk loads `vulkan-1.dll` at run time (nothing to link or ship); DXC compiles shaders at build time; SPIRV-Tools (static) removes unconsumed shader outputs when pipelines are made (`vkdLinkStages`); stb_image and libtiff decode textures (`matImageDecode`; DDS and TGA are read by ports of DirectXTex's readers, no DirectXTex or WIC). Its `vcpkg-configuration.json` has a newer baseline than the other manifests (Vulkan 1.4 headers with `VK_EXT_descriptor_heap`). libtiff and the DLLs it brings are pinned with `overrides` to the same versions in this manifest, GraphicsDX11's and GraphicsTests', because MachStudio loads one `tiff.dll`; zlib is pinned to MainApp's (curl's) version. Change all three together |
| `MachStudio/Source/MainApp/vcpkg.json` | curl (+ zlib) | version check (`mainCheckAppVersion`). Linked dynamically: `libcurl.dll` (and its `zlib1.dll`) ships next to the exe |
| `LibXLT/ToolUIWx/vcpkg.json` | (none) | |
| `UnitTests/CoreTests/vcpkg.json`, `UnitTests/MainAppTests/vcpkg.json`, `UnitTests/GraphicsTests/vcpkg.json` | gtest (GraphicsTests: also stb, for PNG output, and libtiff) | unit tests (`gmock.dll`, which includes gtest, is copied next to the test exe) |

The library packages live under the library projects, so MainApp adds their
`vcpkg_installed\...\lib` dirs to its link paths and its
`CopyLibraryVcpkgDlls` target copies their DLLs into `MachStudio-x64/`.
`ImportExport` still has an include path to `3rdParty\FBX`, which no longer
exists (FBX support is compiled out; see the leftovers note above).

**Qt 6** is a separate install (see the toolkit notes above), not in
`3rdParty/` or vcpkg.

## Code conventions

- **Package prefixes.** Code is organized into small "packages", each with a
  short lowercase prefix used for its folder, file names and class names, e.g.
  `Core/Fs/fsFileUtil.hpp` defines `fsFileUtil`; `Support/ltst/ltstLightSetMgr.hpp`
  defines `ltstLightSetMgr`. Prefix tables are given below.
- **Public vs private.** A package's public headers sit in its folder;
  implementation (`.cpp`) and internal headers live in its `private/` subfolder.
- **Include guards** are the "multiply included" style, and includers guard the
  include as well:
  ```cpp
  #ifdef FS_FILEUTIL_HPP
  #error fsFileUtil.hpp multiply included
  #endif
  #define FS_FILEUTIL_HPP

  #ifndef ENV_PLATFORM_HPP
  #include "Core/env/envPlatform.hpp"
  #endif
  ```
  Newer files sometimes use `#pragma once`. Includes are rooted at the library
  or app source dir (`"Core/..."`, `"Graphics/..."`, `"Support/..."`, `"Systems/..."`).
- **Init / CleanUp.** Each package has an `xxxPackage` class with static
  `Init()` and `CleanUp() throw()`; each library has an `XxxLayer`
  (`CoreLayer`, `GraphicsLayer`, `SupportLayer`, `FeaturesLayer`, ...) that
  initializes all its packages in order.
- **PAC (Platform Abstraction Component).** Platform-specific code is isolated
  behind `xxxPAC.hpp` headers that document the required interface and
  `#include` the per-platform implementation, selected by `ENV_WINDOWS` etc.
  from `Core/Env/envPlatform.hpp`:
  `appTimePAC.hpp` → `appTimePACWin.hpp`. Windows (10 and later) is the only
  implemented platform; the PAC layer is kept so macOS or Linux could be added
  later (`ENV_MACOS`/`ENV_LINUXOS` exist as placeholders). Don't add code for
  older Windows versions. **Windows headers and APIs belong only in `*PACWin*`
  (or other `*Win*`) files**; all OS-specific code should be behind PACs.
- **Common suffixes / patterns** (especially in MachStudio):
  - `...Interest` — observer/listener interface (e.g. `docDocumentInterest`,
    `sel3dSelectInterest`) registered with a manager to receive notifications.
  - `...Mgr` — singleton-style manager for a kind of object.
  - `...Data` / `...DataParser` — plain data plus its file (chunk) reader/writer.
  - `...DocumentChunk` / `...DocumentInterest` — the section of the scene
    document a system owns, and its load/save hooks.
  - `...Object` — a `prtyObject` (property-bearing object) shown in property
    grids; `...ScriptObject` / `...ScriptData` / `...Python` — Python bindings.
  - `...Operation` / `...ActualOperations` — undoable operations
    (`Core/undo`, `Tool/cma`); `...Commands` — UI commands bound to menus,
    toolbars and hotkeys.
  - `...DialogUtil` / `...DialogDataUtil` — toolkit-neutral dialog logic; the
    widgets themselves are in `qtGUI/` and `wxGUI/`.
  - `...ExceptionX` / `...ErrorCodes` — package exceptions and error codes.
  - `tmlnDriver...` — timeline drivers (animated channels); each has `Info`
    (type registration) and `Parser` (persistence) companions.

## LibXLT — shared engine and application framework

`LibXLT/` ("Extra Large Technology") is a set of static libraries layered
roughly Core → Graphics → GraphicsDX11 (or GraphicsVulkan) → Tool → ToolUI*.

### Core (`LibXLT/Core`)
Platform layer and fundamental services; no graphics.

| Package | Purpose |
|---|---|
| `App` (`app`) | Application object, event loop, char/mouse/flow events and handlers, app modes (`appModeMgr`), sim/real time |
| `Ch` (`ch`) | Chunked file I/O: binary, text and XML readers/writers and the chunk parser registry |
| `Dbg` (`dbg`) | Asserts, logging, debug message streams, system info |
| `Env` (`env`) | Platform detection (`envPlatform.hpp`), errors/exceptions, threads, strings, pools, shared assets, package init/cleanup plumbing |
| `Fs` (`fs`) | File system: `fsLocator` paths, file streams, resource finders, absolute path resolution, file change notification, resource tracking, AVI files |
| `Geo` (`geo`) | Geometry: KD-tree, pick rays, ray intersection, twin-edge (half-edge) meshes |
| `Gf` (`gf`) | Generic file layer (bin/txt/XML file types, translation between versions, standard directories/paths) |
| `It` (`it`) | Internationalization: locales and localized strings |
| `Ma` (`ma`) | Math: vectors, matrices, rotations, planes, boxes, colors, gradients, sampling, random |
| `Scrty` (`scrty`) | Abstract copy-protection/licensing interface (implemented by the `Security*` libraries) |
| `dm` | Finite state machine (`dmFSM`, `dmState`) |
| `name` | Named-object registry and name-change notification |
| `prty` | Property system: typed properties (bool, color, enum, file path, ...) plus UI-hint "UIInfo" classes that drive the property grids and dialogs |
| `rel` | Object handles, references and single/multiple relationships between objects |
| `undo` | Undo manager, undo operations and multi-operation blocks |

### Graphics (`LibXLT/Graphics`)
API-independent scene, model and rendering abstractions (the DX11 code
implements these).

| Package | Purpose |
|---|---|
| `An` (`an`) | Animation primitives: key, frame, constant and multi-state animations |
| `Cam` (`cam`) | Cameras and orbit/target camera manipulators |
| `Eff` (`eff`) | Effect/material parameter data and parsers (Blinn, Lambert, glow, DOF, fur, hair, displacement, bake, ...) |
| `Ent` (`ent`) | Entities: model/animation templates and instances, LOD models, scene and scene world |
| `G2d` (`g2d`) | 2D resources: images, render targets, depth buffers, fonts, HDR and OpenEXR output |
| `G3d` (`g3d`) | 3D renderer abstractions: base renderer, fragments, lights, render layers, baking, jitter |
| `Mat` (`mat`) | Materials, material layers, shaders and shader manager, textures and texture manager, MetaFX |
| `Sc` (`sc`) | Scene graph objects: LOD/animatable/compound objects, billboards, particle generators, transform controls |
| `Scr` (`scr`) | Screen-space 2D drawing: images, primitives, text |
| `emdl` | Entity Model import (static, hierarchical, character and vertex-animated templates) |
| `hair` | Hair import and fragment creation |
| `mdl` | Model file import: fragments, materials, animation import, camera animation writer |
| `mtr` | Material templates, material saving, texture lookup, thumbnails |
| `prt` | Particle system: emitters (point, block, circle, cone, spiral), generators, manager |
| `rt` | Simple CPU ray tracing engine |
| `smdl` | Surface models: skinned meshes (CPU/GPU skinning), bezier/PN patches and tessellation, bones, hair surfaces |
| `sprt` | Sprites and sprite groups |
| `vtx` | Vertex animation / geometry cache compression (delta, normal and zlib encoders) |

### GraphicsDX11 (`LibXLT/GraphicsDX11`)
The live Direct3D 11 renderer.

| Package | Purpose |
|---|---|
| `Eff` | DX11 shader effects (Phong, glow, blur, DOF, outline, occlusion, particles, billboards, bake, ...); HLSL in `Eff/private/Shaders` |
| `Fx` | Effect framework: effect descriptions, embedded shaders, material instances |
| `G2d` | DX11 device/window/swap chain, textures, render targets, full-screen quad, debug display |
| `G3d` | DX11 renderers and state managers (blend, depth-stencil), light manager, fog, depth/cube/AO renderers |
| `Mat` | DX11 textures (mip, cube, volume, render target), shadow maps, reflective shadow maps, DDS/HDR loaders |
| `bump` | Bump/normal-mapped tri-mesh fragments and renderer |
| `hair` | DX11 hair renderer |
| `may` | Maya import fragment creation |
| `shdw` | Multi-pass "shadow" renderers: depth, HDR, glow, materials, baking, AO and GI previews, frame buffer manager |
| `tmesh` | Triangle mesh fragments, vertex/index buffers, BVH |

### GraphicsVulkan (`LibXLT/GraphicsVulkan`)
The Vulkan renderer, being ported from GraphicsDX11 (plan:
`docs/superpowers/plans/2026-10-17-graphics-vulkan-roadmap.md`; design:
`docs/superpowers/specs/2026-10-17-graphics-vulkan-design.md`). MachStudio
uses it with `-gfx vulkan`; DX11 stays the default. Namespace `gvk`,
include guards `GVK_*`, class names end in `Vk`. `GraphicsVulkanLayer` has
the same calls as `GraphicsDX11Layer` (its `Init` can fail; `GetInitError`
says why) and installs a Vulkan implementation for every Graphics hook.

| Package | Purpose |
|---|---|
| `vkd` | Thin wrappers over the Vulkan C API (volk, VMA): device and adapter choice, command stream and timeline-semaphore submission, validation policy, images of every shape and format (`vkdImage`: 1D/2D/3D, arrays, cubes, mips, multisampling, 3D images rendered layer by layer; upload, readback, mip generation) and buffers (`vkdBuffer`), DXGI formats to Vulkan ones (`vkdFormat`; the two X8 formats become A8 with alpha 255), swap chains, the per-frame ring buffer, resource binding (`vkdBindings`), SPIR-V reflection reported as D3D11 reflection (`vkdReflect`), shader modules (`vkdShader`), occlusion queries (`vkdOcclusionQuery`, sample-exact as D3D11's), the pipeline cache (`vkdPipelineCache`, keyed by the full state, saved to `VulkanPipelineCache.bin` in the user data folder; it drops the outputs a pixel shader doesn't take from the stage before it, `vkdLinkStages`), and `vkdContext`, the DX11-style immediate context ported code draws through (state, render targets, dynamic rendering with as many layers as the bound views have, for a geometry shader's `SV_RenderTargetArrayIndex`, the barriers DX11 placed implicitly) |
| `Fx` | `fxEffectVk` (GraphicsDX11's `fxEffectDX11` on SPIR-V: same parameter names, offsets, defaults and registers; an image destroyed while bound to an effect is unbound through `vkdDevice`'s image-destroy hooks and reads as the null texture, where D3D11's view would keep it alive), the embedded effects (`fxEmbeddedShadersVk`), `fxMaterialInstance`, and the Effects-shaped API (`fxEffectApi.hpp`) with Vulkan types |
| `Eff` | The shaders (`Eff/private/Shaders`, a byte-identical copy of GraphicsDX11's), `effPlainEffectVk`, and the material shader classes: `effShaderBaseVk`, the `eff*` classes (Phong, Solid, Textured, Glow, Blur, DOF, Outline, Occlusion, Particle, Billboard, Bake, Ramp, Mask Alpha, Light Glow, Strand Hair), `effShaderParamsVk`, and `effShaderArray`, which registers them all as DX11's does |
| `G2d` | 2D system and windows (swap chain plus depth buffer; scenes render into the acquired swap-chain image, or into an offscreen image with `SetRenderResolution`; `DrawImage`/`DrawRect` in `g2dWindowDrawUtilVk`), textures and render-target textures (`g2dTextureVk`, `g2dRenderTargetTextureVk`), `g2dFullscreenQuad`, the bitmap font (`g2dFontUtilVk`: window and render-target text), frame capture (`g2dScreenCaptureUtilVk`: a window's back buffer, a render target or a texture read back into a CPU image), CPU images and image loading (`g2dImageVk`, `g2dImageCreateVk`), image saving (`g2dImageSaveVk`: EXR, HDR and TGA through Graphics' writers; PNG, BMP, JPG and TIFF through `Mat`'s `matImageEncode`, in the layouts DX11's WIC path writes; DDS, PPM, DIB and PFM throw, as on DX11), the resource counter (`g2dResourceCounterVk`: DX11's sums by category) and the debug overlay (`g2dDebugDisplay`). A window whose native client area isn't the size it was given renders offscreen at that size (D3D11's `ResizeBuffers`). In RenderNode (`g3dGfxOptions::m_bHeadless`, set by `mainInitGraphics` in `BATCH_MODE` builds) windows have no surface or swap chain: they render into an offscreen image, and `Present` only submits |
| `Mat` | Textures: `matTextureMgrVk` (DX11's texture manager: loading, creating, reloading, destroying) and the texture classes (plain, mip, static cube, volume, render target) on `vkdImage`. Decoding is portable (`matImageDecode`: stb_image, libtiff, and ports of DirectXTex's DDS and TGA readers, and Graphics' RGBE reader for HDR; no DirectXTex or WIC), as are the mip filters (`matMipFilter`: DirectXTex's triangle and box filters, WIC's Fant scaler emulated); GraphicsTests checks both against DirectXTex. Cube render targets (`matCubeRenderTargetTexture`, reflection maps) and shadow maps (`matShadowMap`, cleared to 0 when made) are real, filled by the `shdw` cube map renderer and the depth map renderer, as are reflective shadow maps (`matReflectiveShadowMap`, LPV GI's), filled by the RSM renderer. Texture saving (`SaveTextureToFile`, DDS through a port of DirectXTex's DDS writer) and `MergeTransparentTextures` are DX11's. `matImageEncode` writes BMP, JPG (stb_image_write at WIC's default quality), PNG, TIFF (libtiff) and DDS without WIC |
| `G3d` | 3D system, light manager, GPU timer for the profiler, the blend / depth-stencil / rasterizer state holders and `g3dStateMgrVk` (DX11's, taking Vulkan enums); the per-node rendering machinery (`g3dVkUtil`, `g3dSceneRenderUtil`, `g3dRenderStateCache`, `g3dSceneGlobal`, `g3dDrawStyleUtilVk`, `g3dTransparencySortVk`, `g3dFogVk`), vertex and index buffers (`g3dVkBufferUtil`: static ones device-local, dynamic ones written to the ring per use), the render passes (`g3dRenderPass`) with the camera- and screen-space ones (`g3dRenderCameraSpaceObjects`, `g3dRenderScreenSpaceObjects`), the depth map renderer `g3dDepthMapRendererVk` (shadow maps), the RSM renderer `g3dRSMRendererVk`, the pick renderer `g3dPickBufferRendererVk`, the texture renderer `g3dTextureRendererVk`, and the plain forward renderer `g3dSceneRendererVk` |
| `shdw` | The multi-pass renderers, ported from GraphicsDX11's: `shdwSceneRendererCreate` (installed by `shdwPackage`; default and HDR give `shdwHDRRendererVk`, simple gives `shdwShadowLayerRendererVk`, plus the depth map, RSM, planar reflection, cube map, pick and texture renderers, and the opacity map renderer `shdwOpacityMapRendererVk`; every type is real), the preview renderers (depth, normal, materials, glow, reflections only, shadow mask and illumination (`shdwShadowsOnlyRendererVk`), AO and GI previews, velocity map, UV), the baker (`shdwBakingRenderer`, installed by `shdwPackage` as `g3dBake`'s implementation), the HDR renderer (tone mapping, bloom and star with `GlareDefVk`, HDR AA, depth of field, debug modes, matte), the frame buffer manager, and the passes: traversal, ZFill, depth, alpha fill, ambient, environment and its background, lit, transparent (sorted, depth-peeled with occlusion queries, deferred), SSAO, glow, outline, post shaders, tone map, DOF, fog, AO volumes, normals, materials, velocity, screen-space GI, LPV GI (radiance volumes drawn slice by slice from a geometry shader), hair (`shdwPassHair`: every transparency mode, depth peeling with occlusion queries) and opacity (`shdwPassOpacity`: a light's 1, 4 or 8 opacity shadow maps, or its opacity volume written from the pixel shader, which hair reads for its shadows). `ShareBuffers` asserts, as DX11's, on a partner that isn't a `shdw` renderer |
| `tmesh`, `bump` | Triangle-mesh fragments and their vertex/index buffers; bump-mapped fragments, the fragment creator and the bump renderer, installed by `bumpPackage` as on DX11 (32-bit indices above 65,535 vertices; meshes split by the draw-call size limit) |
| `may` | Model-import fragment creation (`mayFragCreate`), so scenes load, hair included |
| `hair` | Hair fragments (`hairModelFragVk`: the strands' materials and control points in two structured buffers, `vkdBuffer`s read as `g_HairMaterials`/`g_HairGeometry`; none for a hair object without strands, which draws nothing) and `HairRendererVk` (no vertex input, a 1-point patch list or a line strip, one of Hair_SH's four passes), installed by `hairPackage` as on DX11. Every Graphics hook now has its real class |

- **Target:** Vulkan 1.4 with `VK_EXT_descriptor_heap`, unified image
  layouts (everything stays in `GENERAL`), dynamic rendering, sync2,
  timeline semaphores. The adapter is chosen without DXGI: a discrete GPU
  first, then the most device-local memory; `MSP_GFX_VK_ADAPTER=<name
  substring>` overrides it. A device that lacks something fails `Init`
  with the missing features named.
- **Shaders:** GraphicsDX11's HLSL, copied unchanged (GraphicsTests
  checks the copy byte for byte), compiled by DXC (vcpkg `directx-dxc`)
  to SPIR-V at build time by `Fx/private/EmbedShadersVk.ps1` and embedded,
  as GraphicsDX11 does with fxc. Everything that makes them work on
  Vulkan is a DXC option; the same options are in the GraphicsTests
  shader step and `Tools/fx2hlsl/compile_check.py --spirv`, so change all
  three together. The one exception: DXC's SPIR-V backend lacks
  `ProcessIsolineTessFactors`, which the hair hull shader (`Hair_HS`)
  calls, so that stage is compiled from a wrapper in `Eff/private/ShadersVk/`
  (`*.vulkan-only.*`, the only Vulkan-only shader code) that defines it as
  fxc does and includes the unchanged file (`vkdIsolineTest` checks it
  against D3D11; `compile_check.py --vulkan-only-dir`). Shaders
  GraphicsDX11 compiles from strings at run time are copied into `.hlsl` files when ported (`G2d/private/Shaders`).
- **Resource binding:** shaders keep their DX11 register slots. Each draw
  writes a table into the ring buffer (constant-buffer addresses, then
  texture, sampler and UAV heap indices) and pushes its address with
  `vkCmdPushDataEXT`; DXC's `-fvk-[btsu]-shift 0/100/200/300` bindings map
  onto it. The layout is in `vkd/vkdBindings.hpp`.
- **Validation:** `VK_LAYER_KHRONOS_validation` always runs in Debug
  builds, and in Release with `-gfxValidation`. Errors `DBG_ASSERT` (Debug)
  and are logged and counted; warnings are logged and counted. A
  successful port means zero of either. At exit the log (and RenderNode's
  stderr) says `Vulkan validation: N errors, M warnings, P best-practices
  notes`, or `off`. `MSP_GFX_VK_VALIDATION` (a comma list, read with
  `MSP_GFX_VK_ADAPTER` by `vkdReadEnvironment`, in MachStudio and the
  tests) adds `gpu` (GPU-assisted validation; its vertex-fetch bounds check
  is off, a layer false positive, see `vkdDevice.cpp`) and/or
  `bestpractices` (Khronos and NVIDIA advice, counted as notes, which fail
  nothing); either turns validation on, Release included.
  `vkdValidation` logs as information (not counted) only the layer's
  notes about its own setup; each has its reason in the code.
  `WARNING-Shader-OutputNotConsumed` (a vertex, domain or geometry
  shader output the pixel shader doesn't take, inherent in the unchanged
  shaders, which share vertex shaders between pixel shaders) doesn't
  arise: `vkdPipelineCache` links the stages as D3D11 does, removing those
  outputs from a copy of the SPIR-V (`vkd/private/vkdLinkStages.hpp`,
  SPIRV-Tools); the image doesn't change.

### Other LibXLT libraries

| Library | Purpose |
|---|---|
| `Area18` | Experimental OpenGL (gl3w/GLEW) renderer from 2009: GL device/context, shaders, meshes, a render-graph engine (`rndr`), fontstash text, test scenes. In the solution but not built in any solution configuration, and not linked into MachStudio. |
| `AudioDS` (`sn`) | Sound: DirectSound 2D/streamed playback, MP3 via DirectShow, Redbook audio; used by sound timeline drivers |
| `ImportExport` | Scene import/export: `fbx` (FBX SDK import/export), `gltf` (glTF import), `rman` (RenderMan export/live) |
| `Input` (`in`) | Device-independent input: device manager, keyboard, mouse, gamepad, joystick, tablet, virtual joystick. The keyboard and mouse are fed by the GUI toolkit (`tqtInputFeed` / `twxInputFeed`: key and button events plus a per-frame cursor/button sample); no gamepad backend exists |
| `InputTPC` | Tablet PC (pen/ink) PAC implementation for `Input` |
| `SecuritySoftwareShield` (`scrtys`) | Licensing via SoftwareShield; the one linked into MachStudio |
| `SecurityAladdin` (`scrtya`) | Licensing via Aladdin HASP dongles (obsolete) |
| `SecurityMatrix` (`scrtym`) | Licensing via Matrix dongles (obsolete) |

### Tool (`LibXLT/Tool`)
Toolkit-neutral application framework for 3D editing tools.

| Package | Purpose |
|---|---|
| `api3d` | High-level scene object API (objects, entities, geometry, lights, billboards, particle generators, AO, bake, import) |
| `cam3d` | Camera manager, camera animation keys/import, Maya-style and ortho pan manipulators |
| `cma` | Command and operation framework (simple/toggle commands, command manager, undoable operations) |
| `ctxm` | Context menus |
| `doc` | Documents: document types, single/custom document managers, chunked documents and document interests |
| `gpx` | Thread-safe proxies of graphics objects (camera, character, environment, effects, ...) used between UI and render threads |
| `gui` | Abstract GUI interfaces: main window, menus, toolbars, status bar, property grid/dialogs, file dialogs, message box, progress, splash, cursors. Implemented by a ToolUI library |
| `icn` | Icon sets, layers and scaling |
| `pick3d` | 3D picking (pick buffer, pick lists, pick interests) |
| `rstk` | Resource tracker data (files the scene depends on) |
| `sel3d` | 3D selection manager, select interests, selection undo |
| `tma3d` | Render view, viewer manager, cursor manager, screen utilities |

### ToolUI libraries — `gui` implementations
Each provides the same set of pieces under different prefixes.

| Library | Toolkit | Packages |
|---|---|---|
| `ToolUIQt` | Qt 6 (live) | `qui` (gui implementations), `tqt` (app/menus/toolbars/panes/dialogs), `tqc` (custom controls: color pickers, gradient editors, sliders, file/dir pickers), `pqt` (property-type → control bindings for property grids) |
| `ToolUIWx` | wxWidgets (live) | `wui`, `twx`, `twc`, `pwx` — the wx counterparts of the above |

## MachStudio (`MachStudio/Source`)

The application, built from LibXLT plus the following projects.

### MainApp
Entry point and application startup: `WinMain.cpp`, `mnmApp`, and the
`mainInit*` steps (library, security, graphics, main window, document,
application), version checks, path resolution, Python startup
(`mainPython`), MRU list. `qtGUI/` and `wxGUI/` hold the main form, time
slider, drop target and resolve-path dialog for each toolkit. `Commands/` and
`Data/` hold app-level commands and document chunks. `MainForm.*`,
`AssemblyInfo.cpp` and `*.resX` are leftovers of the WinForms UI.

The command line is parsed first thing in `DoWinMain`, before the main
window and graphics exist, by `mainCommandLine::Parse` (all flags; tested in
`UnitTests/MainAppTests`): graphics options (`-gfx dx11|vulkan`,
`-gfxValidation`, `-gfxProfile`) go to `mainInitGraphics`, which starts the
chosen graphics layer and logs `Graphics backend: <name> on <adapter>`;
`-NoSplash`/`-show` are applied at once; and the actions (`-file`,
`-render`, `-cap*`, `-script`, ...) run after start-up, in order, in
`mnmApp::RunCommandLine`. `-render <scene>` loads the scene, which replaces
the capture settings, so `-cap*` flags go after `-render`
(`-capStartFrame`/`-capEndFrame`, a frame window for every render in the
run, are the exception: they aren't scene settings). `-capSampling`,
`-capJitter`, `-capMotionSamples`, `-capPasses` and `-capHardwareAA` reach
the sampling, pass and anti-aliasing settings RenderNode has no other way
to change (Python is compiled out); their values are checked while parsing
(a bad one is dropped with a warning), and `-capPasses` names passes by the
scene's pass names and sets them in every render layer (RenderMan passes,
such as COLBL, are skipped with a warning: no GPU renderer makes them). The full
command line reference (what each flag does) is at the top of
`MainApp/mainCommandLine.hpp`; `-help` prints the same text
(`mainCommandLine::HelpText()`), and a test checks it lists every flag.

### Support
Shared application services used by Features and Systems.

| Package | Purpose |
|---|---|
| `brsh` | Paint brush tools and brush color dialog |
| `cams` | Camera manager, Director's Cut camera sequencing, camera follow |
| `capt` | Render output settings and render progress |
| `cmpr` | Compression and zip utilities |
| `cmps` | The 3D manipulator "compass" (translate/rotate/scale gizmo) and world axis |
| `dyn` | Dynamic channel controls and transform keys used by drivers |
| `evmt` | Environments (environment maps / image-based lighting) |
| `fbx` | FBX export manager |
| `fgmt` | Fragments (mesh pieces) — properties, selection, highlighting, AO data |
| `fsys` | File listing and file info utilities |
| `grps` | Object groups |
| `gsup` | GUI support helpers (tree views, info boxes, find file, re-attach) |
| `ltst` | Light sets, light isolation and light influence |
| `lyer` | Object layers |
| `mexp` | Maya ASCII exporter and channel recorders |
| `mnm` | Application package/project data, paths, autosave, base document data |
| `mode` | Application modes and mode manager |
| `rman` | RenderMan export manager |
| `mtrl` | Material types (Blinn, Lambert, glow, fur, hair, displacement, ...) and their dialogs |
| `pfx` | Post-processing effects |
| `pnt` | Point objects and selection |
| `ptm` | Timeline time-edit property controls |
| `pyth` | Embedded Python API: modules exposing scenes, objects, dialogs, groups, environments, event callbacks |
| `rlyr` | Render layers and render passes |
| `rmp` | Ramp textures |
| `rprf` | Render preference categories |
| `spln` | Splines/curves and curve point editing |
| `swl` | "Global Environment" settings: lights every object with one environment (overriding per-object environments) and can draw it as the viewport background |
| `tmln` | The timeline: channels (float, color, boolean, filename, ...), blending, driver base classes |
| `vis` | Visibility manager |
| `xfrm` | Transform nodes/groups and the surface tree view |
| `xtra` | User-added extra properties on objects |

### Features
User-facing features, each a package with commands, dialogs and modes.

| Folder | Purpose |
|---|---|
| `Capture` (`cptr`) | Rendering: frame, batch and bake render modes, accumulation buffer, motion blur sampling |
| `Channels` (`chnl`) | Channel editor: creating, moving and deleting drivers on channels |
| `Debug` | Debug/Python console dialog |
| `Export` | Export utilities (FBX) |
| `FilmGates` (`fgt`) | Film gate / safe-frame overlays |
| `Import` | Scene import dialog and logic |
| `Keyframing` (`keyf`) | Keyframe creation |
| `LoadPrefs` | Load preferences dialog |
| `MaterialBrowse` (`mbrw`) | Material swatch browser and painting |
| `ObjectManip` (`mnp`) | Object manipulation mode: translate, rotate, scale, free-form, placement |
| `Playback` (`plbk`) | Playback mode and controls |
| `Prefs` | Application preferences, quick prefs, layouts, hotkeys |
| `ProjectSetup` (`prj`) | Project setup data |
| `PythonObject` (`pyt`) | Python-defined features/dialogs |
| `RenderLayers` (`rdr`) | Render layers dialog |
| `RenderPanels` (`rpn`) | Viewport panels: layouts, panel grid, camera choice, focus commands |
| `RenderPrefs` (`rndr`) | Render preferences |
| `Reports` (`rpt`) | Scene inventory and memory reports |
| `ResourceTracker` | Resource tracker dialog (missing/used files) |
| `SceneSetup` | Scene setup dialog and data |
| `UndoHistory` | Undo history dialog |

### Systems
Each System is its own project and owns one kind of scene element end to end:
data + parser + document chunk (`Data/`), scene objects and managers
(`Object/`), dialogs (`GUI/`), timeline drivers (`Timeline/`), undo
operations (`Undo/`), Python bindings, and RenderMan/FBX export interests.
Entry point is `xxxSystem.hpp`.

| Folder (project) | Prefix | Purpose |
|---|---|---|
| `AmbientOcclusion` (AO) | `ao` | Ambient occlusion settings |
| `Billboard` | `bill` | Camera-facing billboard objects |
| `Cameras` (cmra) | `cmra` | Cameras, lens tables, camera cues, capture drivers |
| `Character` | `chtr` | Characters: skinned/animated models, expressions, animation drivers |
| `Common` | `cmm` | Shared system infrastructure: driver creators for each channel type, splines, templates, time data, common GUI |
| `Environments` | `envt` | Environment maps and their textures |
| `Fog` | `fog` | Fog/atmosphere |
| `GlobalIllumination` (GI) | `gi` | Global illumination settings |
| `Groups` | `grup` | Groups dialog and data |
| `Layers` | `lyrs` | Layers dialog and visibility |
| `LightSets` | `lset` | Light sets dialog and data |
| `PrjLt` | `prjlt` | Projected lights (spot/directional projectors with textures) |
| `PtLt` | `ptlt` | Point lights, fill lights |
| `Transforms` | `trfn` | Transform hierarchy editing |

Not built by the current solution (only old `.vcproj` files; kept from
earlier versions): `DirLt` (directional lights), `DirectorsCut`
(`dcut`, camera cut sequencing), `Particles` (`prtcl`), `Props` (`prop`,
props incl. AI movement drivers), `Sets` (`sets`), `Sky`, `Storyboards`
(`sbrd`), `TimeOfDay` (`tod`).

### Drivers
Concrete timeline drivers (`tmlnDriver*`) that animate channels: `Animation`,
`Attach` (attach/orient to another object), `Color`, `ColorFlicker`,
`ConnectChannel` (link one channel to another), `Enable`, `FileName`,
`FilePath`, `TextureFileName` (static and animated), `Float`, `Orientation`,
`Position`, `Sound`, `Spline` (move along a path), `UserScript` (Python
expression drivers), `Video`.

### How animation works: script objects, channels, drivers, keys
Everything animated on the timeline goes through `Support/tmln`:

- **Script object** (`tmlnScriptObject`) — any animatable scene thing (light,
  camera, character, transform, ...). It **owns** its drivers and a
  `tmlnChannelSet` of channels, and registers with `tmlnTimelineMgr`.
- **Channel** (`tmlnChannel` and typed subclasses `tmlnChannelFloat`,
  `Color`, `Boolean`, `Position`, `Orientation`, `FileName`, ...) — one
  animatable property, usually wrapping a `prty` property
  (`tmlnChannelFloatProperty` wraps a `prtyFloat`). It holds an *original*
  value used when no driver is active (`Reset()`) and lists, without owning,
  the drivers that write to it. Channels have a priority (evaluation order)
  and can be locked.
- **Driver** (`tmlnDriver`) — a clip on a channel with begin/end time that
  writes the channel's value in `Operate(time)`. Concrete drivers are in
  `Drivers/` (constant value, flicker, animation clip, spline path, attach,
  Python expression, sound, video, ...). The gap before a driver is filled by
  its **blend type** (`e_NoBlending`, `e_Previous`, `e_BlendTime`,
  `e_Overwrite`, `e_SmoothBlend`), blending from the previous driver's value;
  smooth blending uses the `tmlnBlendDriver<V>` begin/end values and
  gradients for a cubic Hermite ease with ease-in/out weights.
- **Keyframes are just drivers.** There is no separate key/curve type: a key
  is a short (zero-length) static-value driver, e.g. `tmlnDriverFloat` or
  `dynDriverTransformKey`, and interpolation between keys is the blending
  above. `Features/Keyframing` (`keyfKeyframeUtil`, `keyfKeyCreator`) sets
  keys: for each channel whose value differs from what the drivers produce
  (`HasValueVariation()`), it calls `AlterKey()` on a driver at the current
  time, or creates a new key via the creator's `CreateKeyForChannel()`.
  Auto Key does this every frame from `keyfThinkInterest`.
- **Creation and persistence.** `tmlnDriverCreator`s (registered with
  `tmlnCreator`; per-channel-type ones in `Systems/Common/DriverCreator`
  (`cmm`), transforms in `Support/dyn`) list which drivers an object
  supports, create them by name, as keys or from saved data. Each driver
  type saves as a `...Info` struct (times, blend, ease, channel name, plus
  its own values) written by its `...Parser` chunk parser.
- **Evaluation.** When time changes (`modeModeTime`, playback, capture,
  Python), `tmlnTimelineMgr::Update(t)` calls each script object's
  `tmlnChannelSet::Update`, which gathers the drivers active at `t` per
  channel, resets channels where needed and calls `Operate`. Drivers that
  depend on other animated results (e.g. Attach) report
  `NeedsDelayedOperate()` and run last, after transforms are flushed.
  `tmlnTimeLine` holds the current time, range, FPS and time format.
- **UI.** `Features/Channels` (`chnl`) is the channel/trax editor for
  creating, moving and deleting drivers; driver edits go through undoable
  operations.

### Record (not built)
`rcd` — live recording of float channels from input devices into keys.

### Other MachStudio folders
- `MachStudio-x64/` — run directory: built EXEs, DLLs, Qt plugins, and the
  runtime `Data` (configs, effects, icons).
- `wxFormBuilder/` — `.fbp` sources for the generated wx dialog base classes
  (`...DialogBase.h`); regenerate rather than hand-edit those.
- `Qt Dialogs/` — Qt Designer `.ui` sources.
- `TestSource/` — old standalone test harnesses, not built (see Building).

## Other top-level directories (not covered in depth)
- `3rdParty/` — vendored libraries (wxWidgets, Python, png, zlib, tiff, ...).
- `Tools/` — standalone tools and DCC exporters (Max/Maya/Softimage/Rhino/
  SketchUp/FBX exporters, MatStudio, LODStudio, ParticleStudio, shader
  compiler, ...). Mostly legacy. Live: `Tools/fx2hlsl` (shader compile
  checks) and `Tools/gfxcompare` (renders a scene twice with RenderNode and
  compares frames and `-gfxProfile` timings; see its README).
- `Controls/`, `TestQt/`, `DevTools/` — legacy .NET controls, experiments and
  project wizards.
