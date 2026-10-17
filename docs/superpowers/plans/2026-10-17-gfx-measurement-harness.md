# Graphics measurement and comparison harness — Implementation Plan (Phase 1, Plan 1)

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task, inline in the session (the owner asked for no subagents). Steps use checkbox (`- [ ]`) syntax for tracking. When the plan is done, stop for the owner's review before Plan 2 is written.

**Goal:** Before any Vulkan code exists, be able to (a) time every renderer and pass on the GPU and CPU, and (b) render the test scene twice with RenderNode and compare the frames and timings, proven by DX11 compared against itself. Along the way, parse the whole command line before graphics start, so later plans can choose the backend there.

**Architecture:** A generic tokenizer (`appCommandLine`, Core) feeds a MachStudio command-line parser (`mainCommandLine`, MainApp) that runs first thing in `DoWinMain`; its actions run after initialization, as before. A backend-neutral profiler (`g3dProfiler`) lives in `LibXLT/Graphics/G3d`. It records nested scopes per frame and gets GPU times from a backend hook (`g3dProfilerGpu`); the DX11 hook uses timestamp queries. Markers are macros that compile to nothing unless `MSP_GFX_PROFILING` is defined. A Python tool, `Tools/gfxcompare`, runs RenderNode, compares image folders and summarizes timing CSVs.

**Tech Stack:** C++ (MSVC, x64), GoogleTest via vcpkg, D3D11 timestamp/disjoint queries, Python 3 with numpy, Pillow and pytest.

**Spec:** `docs/superpowers/specs/2026-10-17-graphics-vulkan-design.md` (sections "Backend switch", "Measurement", "Proving parity"). Roadmap: `docs/superpowers/plans/2026-10-17-graphics-vulkan-roadmap.md`.

## Global Constraints

- GraphicsDX11 rendering behaviour must not change. The only GraphicsDX11 edits allowed are measurement hooks, and they compile to nothing unless `MSP_GFX_PROFILING` is defined.
- `MSP_GFX_PROFILING` is defined in Debug configurations, and in Release only when the MSBuild property `MspGfxProfiling` is `true`.
- No shader source edits.
- Command-line actions keep today's order and meaning; only *when* parsing happens changes. Deliberate changes, listed in the commit message: `-NoSplash` and `-show` take effect before the splash/window appear; a flag's value is one token or one double-quoted string (today an unquoted value could swallow the rest of the line up to a quote).
- Code conventions from `CLAUDE.md`: package prefixes, public header plus `private/` implementation, "multiply included" include guards (and guarded includes), Windows APIs only in `*Win*`/`*PACWin*` files (D3D code in GraphicsDX11 is already Windows-only).
- Starting comparison tolerance: max per-channel error ≤ 2/255, and ≤ 0.1% of pixels with error > 1/255. Exact equality is not required.
- Test scene: `C:\Users\dmt\Desktop\Dume_Demo\Dume_Demo_testTESS.mab`, outside the repo, never used in CI; tools take it as an argument or from `MSP_GFX_TEST_SCENE`.
- Git: branch `claude/graphics-vulkan`; stage files by explicit path only (never `git add -u`/`-A`); don't push. Commit messages end with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.

## Review Focus

1. **Double-click launch:** when Windows starts MachStudio with just a scene path (possibly quoted, possibly with spaces), the whole line must still be opened as a file, exactly as today (Task 2 tests `WholeLineIsPathWhenFirstTokenUnknown`, `QuotedWholeLinePath`).
2. **Unknown flags after real ones:** `-render a.mab -bogus` must render `a.mab` and ignore `-bogus` with a log line, not reinterpret the line as a path (Task 2 test `UnknownTokenAfterActionIsIgnored`).
3. **GPU results arrive late:** timestamps are read back frames later. Shutdown must still write every recorded frame; frames whose GPU data never resolves are written with an empty `gpu_ms`, never dropped (Task 4 test `FlushWritesUnresolvedFramesWithEmptyGpu`).
4. **Mismatched outputs:** a frame missing from one folder, images of different sizes, or NaN/Inf in float images must fail the comparison with a clear reason, not crash or pass (Task 7 tests).
5. **Disjoint GPU timing:** if the DX11 disjoint query reports `Disjoint == TRUE`, that frame's GPU times are written empty, not as garbage (Task 6 step 1).

---

## File Structure

| File | Responsibility |
|---|---|
| `LibXLT/Core/App/appCommandLine.hpp`, `private/appCommandLine.cpp` | Generic tokenizer: whitespace-separated tokens, double-quoted strings; flag matching with `-`/`/` |
| `MachStudio/Source/MainApp/mainCommandLine.hpp`, `mainCommandLine.cpp` | MachStudio's flags → `mainCommandLine` struct (graphics options, early settings, ordered actions). Toolkit-neutral, depends only on Core and the `g3dGfxOptions` header |
| `LibXLT/Graphics/G3d/g3dGfxOptions.hpp` | Plain struct of graphics start-up options (`m_ProfileFile` now; Plan 2 adds backend and validation) |
| `MachStudio/Source/MainApp/mnmApp.cpp/.hpp` | `ParseCommandLine` becomes `RunCommandLine(const mainCommandLine&)`: actions only |
| `MachStudio/Source/MainApp/WinMain.cpp`, `mainInitGraphics.cpp/.hpp` | Parse first; apply early settings; hand graphics options to graphics init; profiler start/stop |
| `UnitTests/MainAppTests/*` | New GoogleTest project: compiles `mainCommandLine.cpp` directly, links Core |
| `LibXLT/Graphics/G3d/g3dProfiler.hpp`, `private/g3dProfiler.cpp` | Frames, nested scopes, draw counts, CPU timing, pending-GPU queue, CSV; `g3dProfilerGpu` hook; RAII scopes; `G3D_PROFILE_*` macros |
| `LibXLT/GraphicsDX11/G3d/g3dProfilerGpuDX11.hpp`, `private/g3dProfilerGpuDX11.cpp` | DX11 timestamp/disjoint query implementation of `g3dProfilerGpu` |
| `UnitTests/GraphicsTests/*` | New GoogleTest project for Graphics code that needs no GPU |
| `Directory.Build.props` | Defines `MSP_GFX_PROFILING` |
| `Tools/gfxcompare/` | `gfxcompare.py` (CLI), `imgcompare.py`, `timing.py`, `rendernode.py`, `tests/`, `requirements.txt`, `README.md` |
| `.github/workflows/build-windows.yml` | Build and run the new test projects; run gfxcompare's pytest |

A new test project is a copy of `UnitTests/CoreTests` (see CLAUDE.md, "Unit tests"): new `ProjectGuid`, own `vcpkg.json` (gtest) and `vcpkg-configuration.json`, a `<prefix>Platform` copy of `ctPlatform.hpp`/`ctPlatformWin.cpp`, a main like `CoreTestsMain.cpp`, added to `MachStudio.sln` in all six configurations, referenced (not linked) from `MainApp.vcxproj`, and `vcpkg_installed/` kept out of git as for CoreTests.

---

### Task 1: `appCommandLine` tokenizer (Core)

**Files:**
- Create: `LibXLT/Core/App/appCommandLine.hpp`, `LibXLT/Core/App/private/appCommandLine.cpp`
- Modify: `LibXLT/Core/Core.vcxproj` (+ `.filters`), `UnitTests/CoreTests/CoreTests.vcxproj` (+ filters)
- Test: `UnitTests/CoreTests/App/appCommandLineTest.cpp`

**Interfaces:**
- Produces:
  ```cpp
  class appCommandLine
  {
  public:
      // Splits on spaces/tabs/newlines. "..." is one token with the quotes removed
      // (spaces inside kept; an unterminated quote runs to the end).
      static std::vector<std::string> Tokenize(const std::string& i_Line);
      // True if i_Token is "-<i_Name>" or "/<i_Name>" (exact, case-sensitive).
      static bool IsFlag(const std::string& i_Token, const char* i_Name);
  };
  ```

- [ ] **Step 1: Write the failing tests** in `appCommandLineTest.cpp`:

```cpp
TEST(appCommandLine, EmptyGivesNoTokens)        { EXPECT_TRUE(appCommandLine::Tokenize("").empty()); }
TEST(appCommandLine, SplitsOnWhitespace)        { EXPECT_EQ((std::vector<std::string>{"-a","b","c"}), appCommandLine::Tokenize("  -a\tb \n c ")); }
TEST(appCommandLine, QuotedKeepsSpaces)         { EXPECT_EQ((std::vector<std::string>{"-file","C:\\my dir\\a.mab","-new"}), appCommandLine::Tokenize("-file \"C:\\my dir\\a.mab\" -new")); }
TEST(appCommandLine, UnterminatedQuoteRunsToEnd){ EXPECT_EQ((std::vector<std::string>{"-file","a b"}), appCommandLine::Tokenize("-file \"a b")); }
TEST(appCommandLine, EmptyQuotesGiveEmptyToken){ EXPECT_EQ((std::vector<std::string>{"-x",""}), appCommandLine::Tokenize("-x \"\"")); }
TEST(appCommandLine, IsFlagAcceptsDashAndSlash){ EXPECT_TRUE(appCommandLine::IsFlag("-render","render")); EXPECT_TRUE(appCommandLine::IsFlag("/render","render")); }
TEST(appCommandLine, IsFlagIsExact)             { EXPECT_FALSE(appCommandLine::IsFlag("-renderX","render")); EXPECT_FALSE(appCommandLine::IsFlag("render","render")); EXPECT_FALSE(appCommandLine::IsFlag("-Render","render")); }
```

- [ ] **Step 2: Build CoreTests (Debug Qt); it fails** with C1083 for `Core/App/appCommandLine.hpp`.
Run: `msbuild MachStudio/Source/MachStudio.sln /t:CoreTests /p:Configuration="Debug Qt" /p:Platform=x64 /m`

- [ ] **Step 3: Implement `appCommandLine`.**

- [ ] **Step 4: Build again; the post-build test run passes** (`[  PASSED  ]`, no failures).

- [ ] **Step 5: Commit**

```bash
git add LibXLT/Core/App/appCommandLine.hpp LibXLT/Core/App/private/appCommandLine.cpp LibXLT/Core/Core.vcxproj LibXLT/Core/Core.vcxproj.filters UnitTests/CoreTests/App/appCommandLineTest.cpp UnitTests/CoreTests/CoreTests.vcxproj UnitTests/CoreTests/CoreTests.vcxproj.filters
git commit -m "Add appCommandLine tokenizer"
```

---

### Task 2: `mainCommandLine` parser and MainAppTests

**Files:**
- Create: `LibXLT/Graphics/G3d/g3dGfxOptions.hpp` (header only; add to `Graphics.vcxproj` + filters)
- Create: `MachStudio/Source/MainApp/mainCommandLine.hpp`, `mainCommandLine.cpp` (add to `MainApp.vcxproj` + filters)
- Create: `UnitTests/MainAppTests/` (new test project, see File Structure; prefix `mat`; compiles `..\..\MachStudio\Source\MainApp\mainCommandLine.cpp` as a source file, links Core; include roots `LibXLT` and `MachStudio/Source`)
- Test: `UnitTests/MainAppTests/mainCommandLineTest.cpp`

**Interfaces:**
- Consumes: `appCommandLine` (Task 1).
- Produces:
  ```cpp
  struct g3dGfxOptions
  {
      std::string m_ProfileFile;   // empty: profiling off
  };

  struct mainCommandLine
  {
      enum ActionKind { e_File, e_LastFile, e_Help, e_Batch, e_Render, e_Script,
                        e_CapFormat, e_CapCompress, e_CapCamera, e_CapWidth, e_CapHeight,
                        e_CapOutputFile, e_CapOutputPath, e_CapOutputPathRoot,
                        e_New, e_License, e_Version, e_OpenPath };
      struct Action { ActionKind m_Kind; std::string m_Value; };

      g3dGfxOptions       m_Gfx;
      bool                m_bNoSplash   = false;
      bool                m_bShowWindows = false;
      std::vector<Action> m_Actions;     // command-line order

      static mainCommandLine Parse(const std::string& i_Line);
  };
  ```
- Rules (matching today's `mnmApp::ParseCommandLine`):
  - Value flags take the next token: `file, batch, render, capFormat, capCompress, capCamera, capWidth, capHeight, capOutputFile, capOutputPath, capOutputPathRoot, license, gfxProfile`. A value flag at the end of the line is ignored (logged).
  - `script` takes the rest of the original line after the word `script`, trimmed and with surrounding quotes removed, as one `e_Script` value, and parsing stops (today's behaviour; `RunCommandLine` splits off the `.py` and its arguments as today).
  - `lastfile, help, new, version` are actions without values; `NoSplash` sets `m_bNoSplash`; `show` sets `m_bShowWindows`; `gfxProfile` sets `m_Gfx.m_ProfileFile`.
  - "Parsed any data": every flag except `help`, `show`, `version` counts. An unknown token while nothing has counted yet means the whole line is a path: `m_Actions` becomes exactly `{e_OpenPath, line with surrounding quotes removed}` and parsing stops (earlier `help`/`show`/`version` effects are kept, as today). An unknown token after that is logged and ignored.

- [ ] **Step 1: Create MainAppTests** (copy of CoreTests set-up, see File Structure).

- [ ] **Step 2: Write the failing tests** in `mainCommandLineTest.cpp`:

```cpp
TEST(mainCommandLine, Empty)                          // no actions, no gfx profile, flags false
TEST(mainCommandLine, RenderWithCaptureSettingsInOrder)
// "-capFormat PNG -capOutputPath \"C:\\out dir\" -capWidth 640 -render \"C:\\s.mab\""
// → actions CapFormat "PNG", CapOutputPath "C:\\out dir", CapWidth "640", Render "C:\\s.mab" (in that order)
TEST(mainCommandLine, GfxProfileIsNotAnAction)         // "-gfxProfile p.csv -render s.mab" → m_ProfileFile "p.csv"; actions {Render "s.mab"}
TEST(mainCommandLine, NoSplashAndShowAreEarlySettings) // "-NoSplash -show -new" → both true; actions {New}
TEST(mainCommandLine, WholeLineIsPathWhenFirstTokenUnknown) // "C:\\a b\\scene.mab" → {OpenPath "C:\\a b\\scene.mab"}
TEST(mainCommandLine, QuotedWholeLinePath)             // "\"C:\\a b\\scene.mab\"" → {OpenPath "C:\\a b\\scene.mab"}
TEST(mainCommandLine, HelpThenPathStillOpensPath)      // "-help C:\\x.mab" → actions {Help, OpenPath "-help C:\\x.mab"}  (today's quirk, kept)
TEST(mainCommandLine, UnknownTokenAfterActionIsIgnored)// "-render a.mab -bogus" → {Render "a.mab"}
TEST(mainCommandLine, ScriptTakesRestOfLine)           // "-NoSplash -script \"C:\\t.py\" arg1 arg2" → {Script "C:\\t.py\" arg1 arg2"} with leading/trailing quotes stripped as today; m_bNoSplash true
TEST(mainCommandLine, ValueFlagAtEndIsIgnored)         // "-render" → no actions
TEST(mainCommandLine, SlashPrefixWorks)                // "/render a.mab" → {Render "a.mab"}
TEST(mainCommandLine, LicenseAndLastFileAndVersion)    // "-license KEY -lastfile -version" → {License "KEY", LastFile, Version}
```
Before writing `ScriptTakesRestOfLine`'s expected value, read the `-script` branch of `mnmApp::ParseCommandLine` and make the expectation reproduce exactly what that code passes on today.

- [ ] **Step 3: Build MainAppTests (Debug Qt); it fails** (no `mainCommandLine.hpp`).

- [ ] **Step 4: Implement `mainCommandLine::Parse`** and `g3dGfxOptions`.

- [ ] **Step 5: Build again; all MainAppTests pass.**

- [ ] **Step 6: Commit**

```bash
git add LibXLT/Graphics/G3d/g3dGfxOptions.hpp LibXLT/Graphics/Graphics.vcxproj LibXLT/Graphics/Graphics.vcxproj.filters MachStudio/Source/MainApp/mainCommandLine.hpp MachStudio/Source/MainApp/mainCommandLine.cpp MachStudio/Source/MainApp/MainApp.vcxproj MachStudio/Source/MainApp/MainApp.vcxproj.filters UnitTests/MainAppTests MachStudio/Source/MachStudio.sln
git commit -m "Add mainCommandLine: parse MachStudio's command line without acting on it"
```

---

### Task 3: Parse before graphics init; run actions after

**Files:**
- Modify: `MachStudio/Source/MainApp/WinMain.cpp` (`DoWinMain`), `mnmApp.cpp`, `mnmApp.hpp`, `mainInitGraphics.cpp`, `mainInitGraphics.hpp`

**Interfaces:**
- Consumes: `mainCommandLine::Parse` (Task 2).
- Produces:
  - `void mnmApp::RunCommandLine(const mainCommandLine& i_CommandLine)` — replaces `ParseCommandLine`; performs each action in order exactly as the old branch for that flag did (`e_OpenPath` is the old fallback block). `-help` prints every flag, including `-gfxProfile <file.csv>`.
  - `static void mainInitGraphics::SetGfxOptions(const g3dGfxOptions&)` and `static const g3dGfxOptions& mainInitGraphics::GetGfxOptions()`.

- [ ] **Step 1: Restructure.** At the top of `DoWinMain`: `mainCommandLine command_line = mainCommandLine::Parse(envString::WideCharToUTF8(lpCmdLine));` then apply `m_bNoSplash` (`splashscreen::SetSplashFlag(false)`) and `m_bShowWindows` (`appApplicationPAC::SetShowWindows(true)`) — check that both setters are safe before the app object exists (they are static flags; if not, apply them at the earliest safe point before the splash and the main window, and say so in the commit). Call `mainInitGraphics::SetGfxOptions(command_line.m_Gfx)`. Replace the old `mnmApp::ParseCommandLine(cmdline)` call (WinMain.cpp line ~507) with `mnmApp::RunCommandLine(command_line)`, keeping the `cmdline.size() == 0` check that follows working (use `command_line.m_Actions.empty()` or keep the raw string, whichever preserves the behaviour). Delete `ParseCommandLine`.

- [ ] **Step 2: Build `MainApp;CoreTests;MainAppTests` in Debug Qt and Debug RenderNode.** Expected: success, tests pass.

- [ ] **Step 3: Manual checks (Debug Qt unless stated):**
  - `MachStudio-x64-Qt-Debug.exe "C:\Users\dmt\Desktop\Dume_Demo\Dume_Demo_testTESS.mab"` → the scene opens (double-click path).
  - `... -NoSplash -file "C:\Users\dmt\Desktop\Dume_Demo\Dume_Demo_testTESS.mab"` → no splash screen; scene opens.
  - `MachStudio-x64-RenderNode-Debug.exe -help` → lists every flag.
  - `MachStudio-x64-RenderNode-Debug.exe -capFormat PNG -capOutputPath %TEMP%\cl -render "C:\Users\dmt\Desktop\Dume_Demo\Dume_Demo_testTESS.mab"` → frames written to `%TEMP%\cl`, process exits. Record the output file naming pattern and frame count for Task 8.
  Expected: all four behave as described.

- [ ] **Step 4: Commit** (message lists the two deliberate behaviour changes from Global Constraints)

```bash
git add MachStudio/Source/MainApp/WinMain.cpp MachStudio/Source/MainApp/mnmApp.cpp MachStudio/Source/MainApp/mnmApp.hpp MachStudio/Source/MainApp/mainInitGraphics.cpp MachStudio/Source/MainApp/mainInitGraphics.hpp
git commit -m "Parse the command line before graphics init; run its actions afterwards"
```

---

### Task 4: GraphicsTests and `g3dProfiler` core

**Files:**
- Create: `UnitTests/GraphicsTests/` (new test project, prefix `gt`; references Core and Graphics)
- Create: `LibXLT/Graphics/G3d/g3dProfiler.hpp`, `LibXLT/Graphics/G3d/private/g3dProfiler.cpp` (add to `Graphics.vcxproj` + filters)
- Test: `UnitTests/GraphicsTests/G3d/g3dProfilerTest.cpp`

**Interfaces:**
- Produces (all in `g3dProfiler.hpp`):
  ```cpp
  class g3dProfilerGpu   // implemented per backend
  {
  public:
      virtual ~g3dProfilerGpu() {}
      virtual void BeginFrame(uint64_t i_FrameId) = 0;
      virtual int  BeginScope() = 0;             // token: 0..n-1 in order of BeginScope within the frame
      virtual void EndScope(int i_Token) = 0;
      virtual void EndFrame() = 0;
      // Fills GPU ms per token for the oldest finished frame. Returns false if none is ready
      // (with i_bWait, blocks until the oldest submitted frame is ready, if any).
      // o_bValid false = timings unusable (disjoint); the frame is still reported.
      virtual bool Resolve(bool i_bWait, uint64_t& o_FrameId, std::vector<double>& o_ScopeMs, bool& o_bValid) = 0;
  };

  class g3dProfiler
  {
  public:
      static void Start(const std::string& i_CsvPath, const std::string& i_Backend); // opens the file, writes the header
      static void Stop();                         // resolves (waiting), writes everything, closes
      static bool IsEnabled();                    // true between Start and Stop
      static void SetGpu(g3dProfilerGpu* i_pGpu); // not owned; NULL = CPU only
      static void BeginFrame(float i_SimTime);
      static void EndFrame();
      static bool IsInFrame();
      static void PushScope(const char* i_Name); // string literal (pointer kept)
      static void PopScope();
      static void AddDraws(int i_Count);          // added to every open scope (inclusive)
  };

  class g3dProfileScope       { public: explicit g3dProfileScope(const char* i_Name); ~g3dProfileScope(); };
  class g3dProfileFrameScope  { public: g3dProfileFrameScope(const char* i_Name, float i_SimTime); ~g3dProfileFrameScope(); };
  // FrameScope: if no frame is open it begins and ends a frame (root scope i_Name);
  // otherwise it is an ordinary nested scope.

  #ifdef MSP_GFX_PROFILING
  #define G3D_PROFILE_SCOPE(name)            g3dProfileScope G3D_PROFILE_UNIQUE(g3dProfScope_)(name)
  #define G3D_PROFILE_FRAME_SCOPE(name, t)   g3dProfileFrameScope G3D_PROFILE_UNIQUE(g3dProfFrame_)(name, t)
  #define G3D_PROFILE_DRAWS(n)               g3dProfiler::AddDraws(n)
  #else
  #define G3D_PROFILE_SCOPE(name)            ((void)0)
  #define G3D_PROFILE_FRAME_SCOPE(name, t)   ((void)0)
  #define G3D_PROFILE_DRAWS(n)               ((void)0)
  #endif
  ```
  (`G3D_PROFILE_UNIQUE` is a two-level `##` with `__LINE__`.) Every entry point returns immediately when `!IsEnabled()`.
- CSV, one row per scope per frame, in frame order: `backend,frame,sim_time,path,depth,cpu_ms,gpu_ms,draws`. `path` joins names from the root with `/` (`Target/HDRRenderer/PassLit`); `depth` 0 for the root; `cpu_ms` from `std::chrono::steady_clock`; `gpu_ms` empty without a GPU hook or for invalid/unresolved frames; `frame` counts from 0; 4 decimals.

- [ ] **Step 1: Create GraphicsTests** (copy of CoreTests set-up).

- [ ] **Step 2: Write the failing tests**, with a `FakeGpu : g3dProfilerGpu` returning `1.0 + token` ms per scope and resolving each frame on the next `Resolve`, and a helper that reads the CSV back (temp file per test):

```cpp
TEST(g3dProfiler, WritesHeader)                     // Start + Stop, no frames → exactly the header line
TEST(g3dProfiler, NestedScopesGivePaths)            // "Target"{ "HDR"{ "Lit" } } → Target, Target/HDR, Target/HDR/Lit; depths 0,1,2
TEST(g3dProfiler, GpuTimesMatchTokens)              // FakeGpu → gpu_ms 1.0000, 2.0000, 3.0000 in scope order
TEST(g3dProfiler, DrawsAreInclusive)                // AddDraws(5) in Lit, AddDraws(2) in HDR → Lit 5, HDR 7, Target 7
TEST(g3dProfiler, FrameScopeNestsInsideOpenFrame)   // one frame id, extra nested scope
TEST(g3dProfiler, NoGpuLeavesGpuEmpty)              // SetGpu(NULL) → gpu_ms empty, cpu_ms > 0
TEST(g3dProfiler, InvalidGpuFrameLeavesGpuEmpty)    // o_bValid=false → gpu_ms empty
TEST(g3dProfiler, FlushWritesUnresolvedFramesWithEmptyGpu) // never-resolving fake; 3 frames; Stop → 3 frames, gpu empty
TEST(g3dProfiler, DisabledIsNoOp)                   // without Start: calls don't crash; IsInFrame() false
TEST(g3dProfiler, SimTimeAndBackendWritten)         // Start(path,"dx11"), BeginFrame(1.5f) → dx11, 1.5000
```

- [ ] **Step 3: Build GraphicsTests; it fails to compile** (no `g3dProfiler.hpp`).

- [ ] **Step 4: Implement `g3dProfiler`.** Finished frames wait in a deque until `Resolve` returns their GPU times, then are written; `Stop` calls `Resolve(true, ...)` until it returns false and writes the rest with empty `gpu_ms`. `DBG_ASSERT` on unbalanced Push/Pop and on `EndFrame` with open scopes.

- [ ] **Step 5: Build; all GraphicsTests pass.**

- [ ] **Step 6: Commit**

```bash
git add UnitTests/GraphicsTests LibXLT/Graphics/G3d/g3dProfiler.hpp LibXLT/Graphics/G3d/private/g3dProfiler.cpp LibXLT/Graphics/Graphics.vcxproj LibXLT/Graphics/Graphics.vcxproj.filters MachStudio/Source/MachStudio.sln MachStudio/Source/MainApp/MainApp.vcxproj
git commit -m "Add GraphicsTests and g3dProfiler: per-frame CPU/GPU scope timing to CSV"
```

---

### Task 5: Profiling switch, shared markers, profiler start/stop

**Files:**
- Modify: `Directory.Build.props`
- Modify: `LibXLT/Graphics/G3d/private/g3dTargetRenderer.cpp`, `g3dCubicReflectionTargetRenderer.cpp`, `g3dPlanarReflectionTargetRenderer.cpp`
- Modify: `MachStudio/Source/MainApp/mainInitGraphics.cpp`

**Interfaces:**
- Consumes: `mainInitGraphics::GetGfxOptions()` (Task 3); `g3dProfiler`, `G3D_PROFILE_*` (Task 4).
- Produces: scope names later tasks/plans rely on: root `"Target"`, `"CubeReflection"`, `"PlanarReflection"`.

- [ ] **Step 1: Define `MSP_GFX_PROFILING`** in `Directory.Build.props`: an `ItemDefinitionGroup` adding it to `ClCompile/PreprocessorDefinitions` when `'$(MspBaseConfiguration)' == 'Debug' or '$(MspGfxProfiling)' == 'true'`; extend the header comment.

- [ ] **Step 2: Shared markers.** First statement of `g3dTargetRenderer::Render(float, const std::vector<g3dLayer*>&, bool)`: `G3D_PROFILE_FRAME_SCOPE("Target", i_fSimTime);`. First statement of `g3dCubicReflectionTargetRenderer::Render` / `g3dPlanarReflectionTargetRenderer::Render`: `G3D_PROFILE_SCOPE("CubeReflection");` / `G3D_PROFILE_SCOPE("PlanarReflection");`.

- [ ] **Step 3: Start/stop.** In the `mainInitGraphics` constructor after `GraphicsDX11Layer::InitGraphics()`: if `GetGfxOptions().m_ProfileFile` is non-empty, `g3dProfiler::Start(file, "dx11")`; without `MSP_GFX_PROFILING`, log (and `std::cerr` in `BATCH_MODE`) that profiling is compiled out and don't start. In the destructor, before `GraphicsDX11Layer::CleanUpGraphics()`: `g3dProfiler::Stop()`.

- [ ] **Step 4: Build `MainApp;CoreTests;MainAppTests;GraphicsTests` in all six configurations.** Expected: all succeed; tests pass.

- [ ] **Step 5: Smoke test (Debug Qt):** `MachStudio-x64-Qt-Debug.exe -gfxProfile %TEMP%\p.csv`, orbit the viewport a few seconds, quit.
Expected: `p.csv` with header, backend `dx11`, `Target` rows, empty `gpu_ms` (no GPU hook yet).

- [ ] **Step 6: Commit**

```bash
git add Directory.Build.props LibXLT/Graphics/G3d/private/g3dTargetRenderer.cpp LibXLT/Graphics/G3d/private/g3dCubicReflectionTargetRenderer.cpp LibXLT/Graphics/G3d/private/g3dPlanarReflectionTargetRenderer.cpp MachStudio/Source/MainApp/mainInitGraphics.cpp
git commit -m "MSP_GFX_PROFILING switch, shared profiling markers, -gfxProfile start/stop"
```

---

### Task 6: DX11 GPU timers and DX11 markers

**Files:**
- Create: `LibXLT/GraphicsDX11/G3d/g3dProfilerGpuDX11.hpp`, `LibXLT/GraphicsDX11/G3d/private/g3dProfilerGpuDX11.cpp`
- Modify: `LibXLT/GraphicsDX11/GraphicsDX11.vcxproj` (+ filters), `GraphicsDX11Layer.cpp`
- Modify (one marker line each): every GraphicsDX11 class overriding `g3dSceneRenderer::Render` or `g3dRenderPass::Render`; the draw/dispatch sites in `bump/private/bumpBumpRenderer.cpp`, `G2d/private/g2dFontUtilDX11.cpp`, `G2d/private/g2dFullscreenQuad.cpp`, `hair/private/HairRendererDX11.cpp`, `shdw/private/rtUtil.cpp`, `shdw/private/shdwPassLPVGI.cpp`

**Interfaces:**
- Consumes: `g3dProfilerGpu`, `g3dProfiler::SetGpu`, `G3D_PROFILE_SCOPE`, `G3D_PROFILE_DRAWS` (Task 4).
- Produces: `class g3dProfilerGpuDX11 : public g3dProfilerGpu` (default constructor; uses `g2dDX11Global::g_pDevice`/`g_pDeviceContext`). Scope naming rule that the Vulkan plans copy: **the class name without its package prefix and without a `DX11` suffix** — `shdwHDRRendererDX11` → `"HDRRenderer"`, `shdwPassLit` → `"PassLit"`, `g3dDepthMapRendererDX11` → `"DepthMapRenderer"`.

- [ ] **Step 1: Implement `g3dProfilerGpuDX11`.** Per frame one `D3D11_QUERY_TIMESTAMP_DISJOINT` and a pair of `D3D11_QUERY_TIMESTAMP` per scope (pooled, grown on demand); up to 4 frames in flight, resolving the oldest with waiting if a 5th begins. `Resolve` polls `GetData(..., D3D11_ASYNC_GETDATA_DONOTFLUSH)` (flushing and spinning when `i_bWait`); `Disjoint == TRUE` → `o_bValid = false`. Whole file inside `#ifdef MSP_GFX_PROFILING`.

- [ ] **Step 2: Install it** in `GraphicsDX11Layer::InitGraphics` under `#ifdef MSP_GFX_PROFILING` (`g3dProfiler::SetGpu`); in `CleanUpGraphics`, `SetGpu(NULL)` and delete.

- [ ] **Step 3: Markers.** First statement of each overriding `Render`: `G3D_PROFILE_SCOPE("<name by the rule>");`. After each `Draw*`/`Dispatch`: `G3D_PROFILE_DRAWS(1);`. List the touched classes in the commit body.

- [ ] **Step 4: Prove the markers compile out.** Build Release Qt without `MspGfxProfiling`; `findstr /C:"PassLit" MachStudio\MachStudio-x64\MachStudio-x64-Qt-Release.exe` and check `g3dProfilerGpuDX11.obj` has no code (`dumpbin /symbols`).
Expected: no `PassLit` hit; no `g3dProfilerGpuDX11` symbols.

- [ ] **Step 5: Real run (Debug RenderNode) on the test scene:**
`MachStudio-x64-RenderNode-Debug.exe -gfxProfile %TEMP%\dx11.csv -capFormat PNG -capOutputPath %TEMP%\dx11a -render "C:\Users\dmt\Desktop\Dume_Demo\Dume_Demo_testTESS.mab"`
Expected: PNG frames; CSV rows below `Target/...` with non-empty `gpu_ms`, draws > 0, each root's `gpu_ms` ≥ the largest child's.

- [ ] **Step 6: Commit**

```bash
git add LibXLT/GraphicsDX11/G3d/g3dProfilerGpuDX11.hpp LibXLT/GraphicsDX11/G3d/private/g3dProfilerGpuDX11.cpp LibXLT/GraphicsDX11/GraphicsDX11.vcxproj LibXLT/GraphicsDX11/GraphicsDX11.vcxproj.filters LibXLT/GraphicsDX11/GraphicsDX11Layer.cpp <each marker file>
git commit -m "DX11 GPU timers and profiling markers (compiled out by default)"
```

---

### Task 7: `gfxcompare` image and timing comparison (Python)

**Files:**
- Create: `Tools/gfxcompare/imgcompare.py`, `Tools/gfxcompare/timing.py`, `Tools/gfxcompare/requirements.txt` (`numpy`, `Pillow`, `pytest`)
- Test: `Tools/gfxcompare/tests/test_imgcompare.py`, `Tools/gfxcompare/tests/test_timing.py`

**Interfaces:**
- Produces:
  ```python
  # imgcompare.py
  @dataclass
  class Tolerance:
      max_err: float = 2/255
      pixel_err: float = 1/255
      max_pct_over: float = 0.1          # percent
  @dataclass
  class FrameResult:
      name: str; passed: bool; reason: str   # '' when passed
      max_err: float; mean_err: float; pct_over: float
  def load_image(path: Path) -> np.ndarray          # float32 HxWxC; PNG (8/16-bit) normalized to [0,1]; .pfm raw floats
  def compare_images(a: np.ndarray, b: np.ndarray, tol: Tolerance, name: str) -> FrameResult
  def diff_image(a: np.ndarray, b: np.ndarray) -> np.ndarray   # uint8 HxWx3 heat map of per-pixel max channel error; white at 4*max_err
  def compare_dirs(dir_a: Path, dir_b: Path, tol: Tolerance, diff_dir: Path | None) -> list[FrameResult]
      # matched by file name (.png/.pfm); a name in only one folder → passed False, reason 'missing in A'/'missing in B'
  # timing.py
  def load_csv(path: Path) -> list[dict]
  def summarize(rows: list[dict]) -> dict[str, dict]   # path → cpu_median, gpu_median (None if no values), draws_median, count
  def compare_timing(a: dict, b: dict) -> list[dict]   # every path in either; medians and ratio b/a (None if missing)
  ```
  Error is per channel over RGB; alpha only when both images have it.

- [ ] **Step 1: Write the failing tests:**

```python
def test_identical_images_pass(): ...            # max_err 0, passed
def test_one_level_difference_passes(): ...      # all pixels off by 1/255 → max_err 1/255, pct_over 0, passed
def test_three_levels_fails_max(): ...           # one pixel off by 3/255 → failed, 'max_err' in reason
def test_many_pixels_over_fail_pct(): ...        # 1% of pixels off by 2/255 → failed, 'pct_over' in reason
def test_size_mismatch_fails(): ...              # 4x4 vs 4x5 → failed, 'size' in reason
def test_nan_fails(): ...                        # float image with NaN → failed, 'non-finite' in reason
def test_png_16bit_loads_normalized(tmp_path): ...
def test_pfm_roundtrip(tmp_path): ...            # little-endian PFM (negative scale); rows returned top-to-bottom
def test_compare_dirs_reports_missing(tmp_path): ...  # f1 in both, f2 only in A → f2 'missing in B'
def test_compare_dirs_writes_diff_for_failures_only(tmp_path): ...
def test_summarize_medians(): ...                # Target cpu 1,2,9 → cpu_median 2
def test_summarize_empty_gpu_is_none(): ...
def test_compare_timing_ratio(): ...             # gpu 2.0 vs 3.0 → 1.5; path only in b → None
```

- [ ] **Step 2: Run them; they fail.** `python -m pytest Tools/gfxcompare/tests -q` → import errors.

- [ ] **Step 3: Implement `imgcompare.py` and `timing.py`.** PFM: `PF`/`Pf` header, width height, scale (negative = little-endian), rows stored bottom-to-top.

- [ ] **Step 4: Run; all pass.**

- [ ] **Step 5: Commit**

```bash
git add Tools/gfxcompare/imgcompare.py Tools/gfxcompare/timing.py Tools/gfxcompare/requirements.txt Tools/gfxcompare/tests/test_imgcompare.py Tools/gfxcompare/tests/test_timing.py
git commit -m "gfxcompare: image and timing comparison"
```

---

### Task 8: `gfxcompare` CLI, DX11-vs-DX11 proof, CI and docs

**Files:**
- Create: `Tools/gfxcompare/rendernode.py`, `Tools/gfxcompare/gfxcompare.py`, `Tools/gfxcompare/README.md`
- Test: `Tools/gfxcompare/tests/test_rendernode.py`, `Tools/gfxcompare/tests/test_cli.py`
- Modify: `.github/workflows/build-windows.yml`, `CLAUDE.md`

**Interfaces:**
- Consumes: Task 7's functions; the output naming recorded in Task 3.
- Produces:
  ```python
  # rendernode.py
  def build_command(exe: Path, scene: Path, out_dir: Path, profile_csv: Path,
                    extra_args: list[str], fmt: str = "PNG",
                    width: int | None = None, height: int | None = None) -> list[str]
      # order: extra_args, -gfxProfile, -cap* flags, -render <scene>; paths quoted by subprocess
  def run(cmd: list[str], timeout_s: float) -> None   # RuntimeError with exit code and stderr tail on failure, or if no image was written
  ```
  CLI:
  - `gfxcompare.py run --exe EXE [--scene MAB] --out DIR [--a-args "..."] [--b-args "..."] [--format PNG|PFM] [--width W --height H] [--timeout S]`: renders into `DIR/a` and `DIR/b` (each with `timing.csv`), then compares. `--scene` defaults to `$MSP_GFX_TEST_SCENE`; neither set → exit 2 with a message. `--a-args`/`--b-args` default to empty (Plan 2 passes `-gfx dx11` / `-gfx vulkan`).
  - `gfxcompare.py compare DIR_A DIR_B --out DIR`: comparison only.
  - Both write `DIR/report.md` (per-frame table, failing frames first, diff images in `DIR/diff/`; timing table per path with A, B and B/A ratio for CPU and GPU; the tolerance used) and exit 0 only if every frame passed.
  - Tolerance flags `--max-err`, `--pixel-err` (in 1/255 units) and `--max-pct-over`, defaulting to Global Constraints.

- [ ] **Step 1: Write the failing tests:** `test_build_command_order`, `test_cli_compare_exit_codes(tmp_path)` (identical → 0; one differing frame → 1 and listed first in `report.md`), `test_cli_run_requires_scene(monkeypatch)` (exit 2).

- [ ] **Step 2: Run; they fail.**

- [ ] **Step 3: Implement `rendernode.py` and `gfxcompare.py` (argparse).**

- [ ] **Step 4: Run; all pass.**

- [ ] **Step 5: DX11 against DX11 on the test scene** (Release RenderNode built with `/p:MspGfxProfiling=true`):
`python Tools/gfxcompare/gfxcompare.py run --exe MachStudio/MachStudio-x64/MachStudio-x64-RenderNode-Release.exe --scene C:\Users\dmt\Desktop\Dume_Demo\Dume_Demo_testTESS.mab --out %TEMP%\gfx-self`
Expected: exit 0; every frame within tolerance (ideally `max_err` 0); timing ratios near 1.0. **If two DX11 runs differ, stop and report to the owner** (for example random jitter seeds) rather than loosening the tolerance.

- [ ] **Step 6: CI.** In `build-windows.yml`, build `MainApp;CoreTests;MainAppTests;GraphicsTests` wherever `MainApp;CoreTests` is built, upload the new XML reports, and add `python -m pip install -r Tools/gfxcompare/requirements.txt` and `python -m pytest Tools/gfxcompare/tests -q`.

- [ ] **Step 7: Docs.** `CLAUDE.md`: MainAppTests and GraphicsTests in "Unit tests"; `MSP_GFX_PROFILING`/`MspGfxProfiling` and `-gfxProfile` in "Building"; the command line now parsed in `mainCommandLine` before graphics init (MainApp section); `Tools/gfxcompare` under "Other top-level directories". `Tools/gfxcompare/README.md`: setup, the two commands, tolerance, CSV columns.

- [ ] **Step 8: Commit**

```bash
git add Tools/gfxcompare/rendernode.py Tools/gfxcompare/gfxcompare.py Tools/gfxcompare/README.md Tools/gfxcompare/tests/test_rendernode.py Tools/gfxcompare/tests/test_cli.py .github/workflows/build-windows.yml CLAUDE.md
git commit -m "gfxcompare CLI; DX11 self-comparison of the test scene; CI and docs"
```

- [ ] **Step 9: Stop for review.** Report to the owner: what was built, the DX11 self-comparison result and timing table, anything surprising. Don't write Plan 2 until they say so.
