# CLAUDE.md

Guidance for working in this repository. It covers the two live source trees,
`LibXLT/` (the shared engine/framework libraries) and `MachStudio/` (the
MachStudio Pro application), plus the dead `FusionCinema3D/` and `OrthoRender/`
apps for reference.

MachStudio Pro is a Windows real-time 3D lighting, rendering and cinematography
application (originally StudioGPU / Extra Large Technology, 2003–2010). Users
load scenes exported from DCC tools (Maya, Max, FBX, glTF), light them, animate
cameras/lights/objects on a timeline, and render frames or sequences on the GPU
(Direct3D 11), with export to RenderMan.

## Building

- Visual Studio, **x64 only**. Main solution: `MachStudio/Source/MachStudio.sln`
  (contains the MachStudio projects plus the LibXLT projects it uses).
- The **solution configuration selects the GUI toolkit** (see the comments in
  `Directory.Build.props` and `MachStudio/Source/Directory.Build.targets`):
  - `Debug Qt` / `Release Qt` — Qt 6 UI (`LibXLT/ToolUIQt`, `qtGUI/` folders).
    Qt is found via `MspQtDir`, then `QTDIR`, else `C:\Qt\6.9.3\msvc2022_64`
    (`LibXLT/ToolUIQt/qt6.props`). `windeployqt` runs after linking.
  - `Debug Wx` / `Release Wx` — wxWidgets UI (`LibXLT/ToolUIWx`, `wxGUI/`
    folders, `3rdParty/WxWidgets`). Every other configuration also builds Wx.
  - A build sees only its own toolkit: `wxGUI/**` sources are removed from Qt
    builds and `qtGUI/**` sources from Wx builds. Any toolkit-specific UI code
    must live in a matching `qtGUI/` or `wxGUI/` folder, and both variants
    should be kept in step.
- Executables land in `MachStudio/MachStudio-x64/` (e.g.
  `MachStudio-x64-Qt-Release.exe`), alongside runtime data and DLLs.
- Third-party libraries come from `3rdParty/`, from vcpkg, or (Qt) from a
  separate install; see Dependencies below.
- Many directories still contain obsolete `.vcproj` (VS2005/2008), `.vspscc`
  and `.8.00.old` files. Only `.vcxproj` files are live; a `System` folder
  with only a `.vcproj` is not built by the current solution.
- Tests: there is no automated test suite. `MachStudio/TestSource/` holds a few
  old standalone harnesses (`chnlTest`, `ltstTest`, `splnTest`) that use the
  obsolete `.vcproj` format.

### Dependencies

**From `3rdParty/`** (checked in, prebuilt; `3rdParty/thridParty.txt` has old
build notes):

| Library | Location | Used for |
|---|---|---|
| wxWidgets 3.2 | `WxWidgets/` (headers, import libs; `wxwidgets.props`) | Wx UI builds only (DLLs in `MachStudio-x64/`) |
| libpng 1.6 | headers `Png/lpng162`, lib/DLL `Libs/x64/libpng16` | image loading (`g2dSurfaceLoaderDX11`) |
| libtiff 4.0.6 | headers `tiff-4.0.6/`, lib/DLL `Libs/x64/tiff` | image loading |
| zlib + minizip | headers `Png/zlib-1.2.5`, lib/DLL `Libs/x64/zlibwapi` (zlib 1.2.8 sources in `zlib/`) | vertex-animation compression (`vtx`), zip (`cmpr`) |
| Python 3.9 | `Python/include` (headers only, not in git) | embedded Python; compiled out unless `PYTHON_ENABLED` is defined, which no live configuration does |

Unused leftovers in `Libs/x64`: `jpeg.lib`, the 2011
`fbxsdk` libs (FBX import/export needs `USE_FBX_IMPORTEXPORT`, which only the
dead `Gold` config defines), `python26/27`, `ATITessellation*`.

**From vcpkg** (manifest mode, `VcpkgEnableManifest`; each project installs into
its own `vcpkg_installed/`):

| Manifest | Packages | Used for |
|---|---|---|
| `LibXLT/Graphics/vcpkg.json` | OpenEXR (+ Imath, libdeflate) | writing `.exr` render output (`g2dScanlineOpenEXRUtil`, `g2dOpenEXROutStream`, `g2dHDRUtil`) |
| `LibXLT/GraphicsDX11/vcpkg.json` | DirectXTex, nlohmann-json | DDS/texture loading and image saving; parsing `*.effect.json` effect descriptions (`fxEffectDesc`). glTF import bundles its own `json.hpp`/`tiny_gltf.h` |
| `MachStudio/Source/MainApp/vcpkg.json` | curl (+ zlib) | version check (`mainCheckAppVersion`). Linked dynamically: `libcurl.dll` (and its `zlib1.dll`) ships next to the exe |
| `LibXLT/ToolUIWx/vcpkg.json` | (none) | |

The library packages live under the library projects, so MainApp adds their
`vcpkg_installed\...\lib` dirs to its link paths and its
`CopyLibraryVcpkgDlls` target copies their DLLs into `MachStudio-x64/`.
Some projects still reference folders that no longer exist
(`3rdParty\OpenEXR`, `3rdParty\DirectXTex`, `3rdParty\FBX`,
`D:\FreeImage`); these paths are stale.

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
roughly Core → Graphics → GraphicsDX11 → Tool → ToolUI*.

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

### Other LibXLT libraries

| Library | Purpose |
|---|---|
| `Area18` | Experimental OpenGL (gl3w/GLEW) renderer from 2009: GL device/context, shaders, meshes, a render-graph engine (`rndr`), fontstash text, test scenes. In the solution but not built in any solution configuration, and not linked into MachStudio. |
| `AudioDS` (`sn`) | Sound: DirectSound 2D/streamed playback, MP3 via DirectShow, Redbook audio; used by sound timeline drivers |
| `ImportExport` | Scene import/export: `fbx` (FBX SDK import/export), `gltf` (glTF import), `rman` (RenderMan export/live) |
| `Input` (`in`) | Device-independent input: device manager, keyboard, mouse, gamepad, joystick, tablet, virtual joystick |
| `InputDI` | DirectInput PAC implementations of keyboard/mouse/gamepad for `Input` |
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
| `ToolUIManaged` | C++/CLI WinForms (dead) | `mui`, `tma`, `prtym`; the original .NET UI; not in the current solution |

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
OrthoRender/earlier versions): `DirLt` (directional lights), `DirectorsCut`
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
- `TestSource/` — old standalone test harnesses (see Building).

## Dead applications (reference only)

Both are built on the same LibXLT + MachStudio architecture, use obsolete
`.vcproj` solutions (VS2005/2008, Win32) and are not maintained. Don't update
them when changing shared code.

### FusionCinema3D (`FusionCinema3D/`)
A consumer "make your own 3D movie" app built on the MachStudio engine with a
Qt 4 front end (`MainApp/qtfc3d.*`) around the wx/LibXLT core. It reused
MachStudio's `Features`, `Support`, `Systems` and `Drivers` projects directly
and added `Source/FCSupport`:

| Package | Purpose |
|---|---|
| `fcmd` | The wizard-like workflow modes: New Movie → Template → Cast → Location → Action → Make → Theater (playback) |
| `actn` | Action-step managers: scene, lights, audio, post effects, title cards |
| `anim` | Animated UI pixmaps and transitions for the mode screens |
| `cast` / `loc` | Choosing characters (cast) and sets (location) |
| `fcdc` | Movie document data: timeline items and title items |
| `fcui` | Custom UI: forms, panels, button layouts, the movie timeline |
| `fio` | Title card I/O |
| `ovrly` | On-screen overlay controls (color, rotate, zoom) |
| `evnt` / `path` | Event data; asset directory parsing |

`fusioncinema3d-Win32/` is its run directory (Data, Media, Shaders, DLLs).

### OrthoRender (`OrthoRender/`)
A headless/server render node derived from an earlier fork of MachStudio
(it has its own copies of `Drivers`, `Features`, `Record`, `Support`,
`Systems`, built against the DX9 renderer `GraphicsDX9`, which no longer
exists in `LibXLT`). It received render jobs over ActiveMQ ("LightWait"
messaging, `MainApp/LightWait*`), applied them to a scene and rendered:

- `Features/Requests` — remote command manager and an XML command parser;
  request tasks for scene, objects, modification, render, job info and CMS
  requests.
- `Features/Capture/orthoAvatarData*`, `orthoModeRender` — customizable
  "avatar" characters (parts by category, attached to joints, with colors)
  rendered on request.
- `MainApp/s3.cpp`, `LightWaitUpload` — uploading rendered images to Amazon S3.
- `Features/EONReality` — export to EON Reality; `Features/MayaExport`,
  `Features/SelectList` — Maya export and selection lists.
- It includes the Systems MachStudio no longer builds (DirLt, DirectorsCut,
  Particles, Props, Sets, Sky, Storyboards, TimeOfDay).

## Other top-level directories (not covered in depth)
- `3rdParty/` — vendored libraries (wxWidgets, Python, png, zlib, tiff, ...).
- `Tools/` — standalone tools and DCC exporters (Max/Maya/Softimage/Rhino/
  SketchUp/FBX exporters, MatStudio, LODStudio, ParticleStudio, shader
  compiler, ...). Mostly legacy.
- `Controls/`, `TestForms/`, `TestApps/`, `TestQt/`, `TestLibXLT/`,
  `TestTerawatt/`, `DevTools/` — legacy .NET controls, experiments and
  project wizards.
