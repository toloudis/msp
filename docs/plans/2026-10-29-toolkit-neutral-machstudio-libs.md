# Building MachStudio's libraries once per Debug/Release

Plan for moving MachStudio's toolkit-specific code (wx, Qt, and the
RenderNode "no toolkit" variant) into one new per-toolkit library, so
Support, Systems, Drivers and Features build once per Debug/Release.
That's the move LibXLT's libraries already made, applied to MachStudio.

## Why

- Every MachStudio library has the six configurations
  (Debug/Release × Qt/Wx/RenderNode), so each is compiled once per
  toolkit. Support, Systems, Drivers and Features come to about 240k
  lines, of which about 35k are GUI code (wxGUI ~20k, qtGUI ~15k; a
  build compiles only one of the two). Each extra toolkit build
  recompiles roughly 200k lines that come out identical.
- The gain is in switching toolkits, full rebuilds, and checking a change
  in all six configurations on one machine. Incremental builds of the one
  configuration being worked on get no faster. CI gains nothing as it
  stands: each matrix job runs on its own runner and builds everything
  (see "CI" at the end).

## What varies today

Files outside `wxGUI/`/`qtGUI/` that test `USE_QT`, `USE_WXWIDGETS`,
`MSP_GUI_*` or `BATCH_MODE`:

| Group | Projects | Toolkit-dependent code outside the GUI folders |
|---|---|---|
| Already neutral | Drivers, AO, Billboard, Cameras (`cmra`), Fog, GI, PrjLt, PtLt, Transforms (~280 .cpp) | None. Their configurations differ only by the `BATCH_MODE` define, which their code never reads |
| Nearly neutral | Character | An empty `#if` block (`chtrExpressionsDialogUtil.cpp`) and a `wxwidgets.props` import |
| Lightly mixed | Support (7 files), Systems/Common (4), Environments, Groups, Layers, LightSets (1 each) | Mostly `*DialogUtil.cpp` glue: calls into a dialog inside `#if defined(USE_WXWIDGETS) \|\| defined(USE_QT)` |
| Heavily mixed | Features (40 files, ~390 conditionals) | About 18 `*DialogUtil.cpp` glue files, about 22 other files (Capture 11, RenderPanels 8, Prefs 5, ...), and the only `BATCH_MODE` uses outside MainApp (Capture, 3 places) |
| Per-toolkit by nature | MainApp, ToolUIWx, ToolUIQt, SecuritySoftwareShield | Unchanged; they keep six configurations |

Facts the plan relies on:

- The MachStudio libraries have no ProjectReferences. MainApp references
  all of them and links them, so moving code between libraries can't
  create a cycle in the build graph. A neutral library calling a function
  defined in the toolkit library is resolved when MainApp links, the way
  `Tool/gui` is declared in Tool and implemented by the ToolUI libraries.
- `Directory.Build.targets` already drops `wxGUI/**` and `qtGUI/**`
  sources by path for the other toolkit (both, for `None`). Its paths are
  rooted at `MachStudio/Source`, so they also apply to files a project
  lists from another folder.
- Neutral configurations (plain `Debug`/`Release`) get
  `MspGuiToolkit=None` from `Directory.Build.props`, no wx or Qt include
  paths, and none of the toolkit macros. A neutral library that still
  needs a toolkit header won't compile, which is the check we want.
- No `wxGUI`/`qtGUI` source registers itself through a static
  initializer (checked by grep). Every GUI object is reached through a
  call, so the linker won't silently drop any of them from a static
  library.

## Headers whose declarations change with the toolkit

Only these, outside MainApp. Each is included only from `.cpp` files in
the mixed projects:

| Header | What varies | Plan |
|---|---|---|
| `Support/ptm/ptmControlFactoryTimeline.hpp` | The class's base is `pwxControlFactory` or `pqtControlFactory` | Moves to the UI library with its `.cpp` (compiling it into a neutral library would be a one-definition-rule violation) |
| `Support/gsup/gsupTreeCtrlUtil.hpp` | Functions taking wx or Qt tree types | Moves to the UI library with its `.cpp` |
| `Systems/Common/GUI/cmmObjectDialogUtil.hpp` | The `TabPage` typedef and `AddTabPage`/`RemoveTabPage`/`HasTabPage` | Split: the tab-page functions go to a new UI-side header (e.g. `cmmObjectDialogTabs.hpp`); the rest stays neutral |
| `Features/Prefs/prefsQuickMgr.hpp` | An empty `#if` block | Delete the block |

The `*DialogUtil.hpp` headers that neutral code includes are already
toolkit-free (checked for Groups, fgmt and pyth; each phase checks the
rest).

## Design

### One new library: `MachStudioUI`

- `MachStudio/Source/UI/MachStudioUI.vcxproj`, a static library with the
  six configurations, in `MachStudio.sln`, referenced by MainApp like the
  other libraries.
- It compiles, in place, the MachStudio sources that depend on the
  toolkit:
  - every `wxGUI/` and `qtGUI/` source (the targets file already picks
    one toolkit's sources);
  - the glue: the `.cpp` files outside those folders that test the
    toolkit macros, such as `grupDialogUtil.cpp`;
  - the headers in the table above that move.
- **Files don't move on disk.** Only their project changes, so the
  package layout, include paths and the wxGUI/qtGUI rule in CLAUDE.md
  stay as they are. The project lists them with relative paths
  (`..\Systems\Groups\GUI\grupDialogUtil.cpp`, ...), grouped in filters
  by package so Solution Explorer stays navigable.
- In RenderNode the library is built with no toolkit (`BATCH_MODE`), so
  the glue compiles to the empty bodies it compiles to today. Headless
  behaviour doesn't change.
- I recommend one library rather than SupportUI/SystemsUI/FeaturesUI:
  several would add solution clutter and gain nothing, since only MainApp
  links them.

### The rule for neutral libraries

Support, Systems/*, Drivers and Features follow the rule CLAUDE.md
already states for the LibXLT neutral projects: no `BATCH_MODE`,
`USE_QT`, `USE_WXWIDGETS` or `MSP_GUI_*`, no wx or Qt headers. Code that
needs the toolkit calls a glue function whose declaration involves no
toolkit types, defined in `MachStudioUI`.

### Turning a mixed file into neutral code plus glue

For a file that isn't glue but tests the macros (`fgmtScriptObject.cpp`,
`cmmSystem.cpp`, `rpnOperations.cpp`, ...):

1. Move each toolkit-dependent block into a function of that package's
   existing `*DialogUtil` (or a new one, `xxxDialogUtil`, following the
   naming convention), with toolkit-free parameters.
2. Make the original file call it; remove its conditional includes.
3. The `*DialogUtil.cpp` moves to `MachStudioUI` (if it isn't there
   already), with the moved block under the same `#if` it had.

### `BATCH_MODE` in Features/Capture

- `cptrPackage.cpp` (two dialog guards: tags info, Photoshop export):
  the calls move into glue. The glue's body is empty in RenderNode, as
  the guarded code is today. Where a command shouldn't be registered at
  all in RenderNode, the glue reports whether dialogs are available
  (e.g. `cptrDialogUtil::HasDialogs()`, `false` in RenderNode).
- `cptrModeRender.cpp` (`c_MULTITHREADED_CAPTURE`): becomes a run-time
  check of `g3dGfxOptions::m_bHeadless`, which `mainInitGraphics`
  already sets in RenderNode. That's the one behaviour that moves from
  compile time to run time; the RenderNode check below covers it.

## Phases

Each phase ends with the six configurations building and the unit tests
passing, and stops for review.

### Phase 1: the already-neutral projects (no code changes)

1. Drivers, AO, Billboard, Cameras, Fog, GI, PrjLt, PtLt, Transforms:
   replace the six configurations with `Debug` and `Release`, with
   output in `bin\x64\Debug|Release\lib\` (the paths LibXLT's neutral
   projects use, `bin\$(Platform)\$(Configuration)\...`, become the
   shared ones).
2. Character: delete the empty block in `chtrExpressionsDialogUtil.cpp`
   and the `wxwidgets.props` imports, then the same change. If it turns
   out to need wx after all, it waits for Phase 2.
3. `MachStudio.sln`: map `Debug Qt|Wx|RenderNode` → `Debug` and the
   Release ones → `Release` for those projects, as for Core and the
   other LibXLT neutral projects.
4. Check: building `Debug Qt` then `Debug Wx` reports these projects up
   to date the second time.

About 315 .cpp files stop being built three times.

Done 2026-10-29 (awaiting review): all six configurations build, unit
tests pass; the Qt builds compile the 315 files, the Wx and RenderNode
builds after them compile none. Character needed only the unused
`cmmObjectDialogUtil.hpp` include and the empty block removed; Character
and Transforms also lost an unused `$(MspWxIncludeDirs)`.

### Phase 2: `MachStudioUI`; Support and the remaining Systems

1. Create `MachStudioUI.vcxproj` (start from `Support.vcxproj`'s six
   configurations and its wx/Qt settings: the `wxwidgets.props` import
   and the targets file's Qt import) and add it to the solution and to
   MainApp's ProjectReferences.
2. Move into it from Support: all `wxGUI/`/`qtGUI/` sources, the glue
   (`brshColorDialogUtil`, `fgmtDialogUtil`, `pythDialogUtil`), and
   `gsupTreeCtrlUtil` and `ptmControlFactoryTimeline`. Make the non-glue
   files neutral (`fgmtScriptObject.cpp`'s UV editor, `rmpDialogMgr.cpp`).
   Check which files include the moved headers (`dynDialogUtil`,
   `mtrlDialogUtil`, `mtrlPropertyObject`, `mtrlSetOperation`,
   `mtrlShaderObject`, `mtrlCommands`, `xtraCommands`, `xtraOperations`,
   `fgmtPropertyObject`): each one either moves with them or stops
   including them.
3. The same for Systems/Common (`cmmObjectDialogUtil` split,
   `cmmSystemDialogUtil`, `cmmSplineCommands`, `cmmCommands`,
   `cmmSystem`, `cmmSelectInterest`, `cmmSplineDialogUtil`), then
   Environments, Groups, Layers and LightSets (one `*DialogUtil.cpp`
   each, plus their GUI folders).
4. Switch Support, Common, Environments, Groups, Layers and LightSets to
   `Debug`/`Release` and drop their `wxwidgets.props` imports.
5. Check, by hand in Qt and Wx (the owner): the object property dialog
   with its Groups, Layers, LightSets and Environments tabs; the fragment
   UV editor; the timeline time-edit controls in a property grid; the
   brush color dialog.

### Phase 3: Features, one folder at a time

1. Move the Features `wxGUI/`/`qtGUI/` sources and the `*DialogUtil.cpp`
   glue into `MachStudioUI`, a folder at a time (Capture, Channels,
   Debug, Import, LoadPrefs, MaterialBrowse, Playback, Prefs,
   ProjectSetup, RenderLayers, RenderPanels, ResourceTracker,
   SceneSetup, UndoHistory), building after each.
2. Make the non-glue files neutral: Capture (`cptrModeRender`,
   `cptrModeRenderFrame`, `cptrPackage`, `cptrRenderRmanLiveMgr`,
   `cptrRenderRmanLiveUtil`, `cptrRenderUtil`), Channels (`chnlCommands`,
   `chnlPrefsInterest`), `dbgConsoleDataUtil`, `FeaturesLayer`,
   `prefsCommands`, `PrefsMgr`, `prefsQuickMgr`, `prjPython`,
   `rdrLayersDataUtil`, RenderPanels (`rpnCommandChangeLayout`,
   `rpnCommandFocusAllPanels`, `rpnCommandFocusCamera`, `rpnCommands`,
   `rpnDocumentChunk`, `rpnOperations`, `rpnPackage`).
   RenderPanels is the largest piece: its layout and focus code talks to
   the panel windows directly; expect a small `rpnDialogUtil` interface
   (layout change, focus, panel count) rather than one function per
   block.
3. The `BATCH_MODE` changes in Capture (above).
4. Switch Features to `Debug`/`Release` and drop its `wxwidgets.props`
   import.
5. Check, by hand in Qt and Wx (the owner): render options, batch, bake,
   progress and stats dialogs; panel layouts and focus commands;
   preferences and quick prefs; channel editor; import dialog. In
   RenderNode: a short `-render` of the test scene (frames 296–315),
   compared with a RenderNode build from before the change.

### Wrap-up

- CLAUDE.md: the neutral-project list in Building, a `MachStudioUI` entry
  under MachStudio, and the rule that toolkit-specific code outside the
  GUI folders belongs to `MachStudioUI`.
- A CI step that greps the neutral MachStudio projects' sources for the
  toolkit macros and wx/Qt includes and fails on any.

## Risks

- **Late errors.** A glue function missing from `MachStudioUI` shows up
  as an unresolved external when MainApp links, not when the library
  compiles. Building after each folder keeps that manageable.
- **Behaviour under RenderNode.** Today RenderNode compiles Support and
  Features with `BATCH_MODE`; after the change only `MachStudioUI` and
  MainApp do. Any `BATCH_MODE` use missed in a neutral library would be
  a compile-time difference that silently disappears. The CI grep and the
  Phase 3 RenderNode comparison catch that.
- **Navigation.** A package's dialog code shows under `MachStudioUI` in
  Solution Explorer, not under the package's project (same folder on
  disk).
- **Concurrent builds.** As for the LibXLT libraries: two builds of the
  same base configuration (e.g. `Debug Qt` and `Debug Wx`) must not run
  at the same time, since they'd write the same neutral libraries.

## CI

`build-windows.yml` builds each of the six configurations on its own
runner, so each job still builds every neutral library. Getting the gain
in CI would need a job per Debug/Release that builds the neutral
libraries and hands them to the toolkit jobs as artifacts. That's
possible but outside this plan; decide after Phase 3 whether it's worth
it.
