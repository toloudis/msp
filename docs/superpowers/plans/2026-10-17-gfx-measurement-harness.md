# Graphics measurement and comparison harness — Implementation Plan (Phase 1, Plan 1)

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Before any Vulkan code exists, be able to (a) time every renderer and pass on the GPU and CPU, and (b) render the test scene twice with RenderNode and compare the frames and timings, proven by DX11 compared against itself.

**Architecture:** A backend-neutral profiler (`g3dProfiler`) lives in `LibXLT/Graphics/G3d`. It records nested scopes per frame and gets GPU times from a backend hook (`g3dProfilerGpu`); the DX11 hook uses timestamp queries. Markers are macros that compile to nothing unless `MSP_GFX_PROFILING` is defined. MachStudio turns profiling on with `-gfxProfile <file.csv>`, read by a pre-scan before graphics start. A Python tool, `Tools/gfxcompare`, runs RenderNode, compares image folders and summarizes timing CSVs.

**Tech Stack:** C++ (MSVC, x64), GoogleTest via vcpkg, D3D11 timestamp/disjoint queries, Python 3 with numpy, Pillow and pytest.

**Spec:** `docs/superpowers/specs/2026-10-17-graphics-vulkan-design.md` (sections "Measurement" and "Proving parity"). Roadmap: `docs/superpowers/plans/2026-10-17-graphics-vulkan-roadmap.md`.

## Global Constraints

- GraphicsDX11 rendering behaviour must not change. The only GraphicsDX11 edits allowed are measurement hooks, and they compile to nothing unless `MSP_GFX_PROFILING` is defined.
- `MSP_GFX_PROFILING` is defined in Debug configurations, and in Release only when the MSBuild property `MspGfxProfiling` is `true`.
- No shader source edits.
- Code conventions from `CLAUDE.md`: package prefixes, public header plus `private/` implementation, "multiply included" include guards (and guarded includes), `Init`/`CleanUp` patterns, Windows APIs only in `*Win*`/`*PACWin*` files (D3D code in GraphicsDX11 is already Windows-only).
- Starting comparison tolerance: max per-channel error ≤ 2/255, and ≤ 0.1% of pixels with error > 1/255. Exact equality is not required.
- Test scene: `C:\Users\dmt\Desktop\Dume_Demo\Dume_Demo_testTESS.mab`, outside the repo, never used in CI; tools take it as an argument or from `MSP_GFX_TEST_SCENE`.
- Git: work on branch `claude/graphics-vulkan`; stage files by explicit path only (never `git add -u`/`-A`); don't push.
- Commit messages end with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.

## Review Focus

1. **Command-line hand-off:** `mnmApp::ParseCommandLine` treats an unrecognized first token as a scene path for the whole line. `-gfxProfile out.csv -render scene.mab` must still render `scene.mab`, so ParseCommandLine must consume `-gfxProfile` and its value (Task 3 test via RenderNode run in Task 4).
2. **Long command lines:** `ParseCommandLine` copies into `char pLine[1024]` with `strcpy`. Harness command lines with long absolute paths must not overflow. The buffer is sized to the line (Task 3), and `gfxcompare` refuses to build a command line over 30,000 characters (Task 6 test).
3. **GPU results arrive late:** timestamps are read back frames later. Shutdown must still write every recorded frame. Frames whose GPU data never resolves are written with an empty `gpu_ms` and never dropped (Task 2 test `FlushWritesUnresolvedFramesWithEmptyGpu`).
4. **Mismatched outputs:** a frame missing from one folder, images of different sizes, or NaN/Inf in float images must fail the comparison with a clear reason, not crash or pass (Task 5 tests).
5. **Disjoint GPU timing:** if the DX11 disjoint query reports `Disjoint == TRUE` (clock changed), that frame's GPU times are unusable and are written empty, not as garbage (Task 4 step).

---

## File Structure

| File | Responsibility |
|---|---|
| `LibXLT/Graphics/G3d/g3dGfxOptions.hpp`, `private/g3dGfxOptions.cpp` | Parse graphics flags from the raw command line (`-gfxProfile` now; Plan 2 adds `-gfx`, `-gfxValidation`) |
| `LibXLT/Graphics/G3d/g3dProfiler.hpp`, `private/g3dProfiler.cpp` | Frames, nested scopes, draw counts, CPU timing, pending-GPU queue, CSV writing; `g3dProfilerGpu` hook interface; RAII scope types; `G3D_PROFILE_*` macros |
| `LibXLT/GraphicsDX11/G3d/g3dProfilerGpuDX11.hpp`, `private/g3dProfilerGpuDX11.cpp` | DX11 timestamp/disjoint query implementation of `g3dProfilerGpu` |
| `UnitTests/GraphicsTests/*` | New GoogleTest project (copy of CoreTests setup) testing Graphics code that needs no GPU |
| `Directory.Build.props` | Defines `MSP_GFX_PROFILING` |
| `MachStudio/Source/MainApp/WinMain.cpp`, `mainInitGraphics.cpp`, `mnmApp.cpp` | Pre-scan, profiler start/stop, consume `-gfxProfile`, fix buffer |
| `Tools/gfxcompare/` | `gfxcompare.py` (CLI), `imgcompare.py`, `timing.py`, `rendernode.py`, `tests/`, `requirements.txt`, `README.md` |
| `.github/workflows/build-windows.yml` | Build GraphicsTests; run gfxcompare's pytest |

---

### Task 1: GraphicsTests project and `g3dGfxOptions`

**Files:**
- Create: `UnitTests/GraphicsTests/GraphicsTests.vcxproj`, `GraphicsTests.vcxproj.filters`, `GraphicsTestsMain.cpp`, `vcpkg.json`, `vcpkg-configuration.json`, `gtPlatform.hpp`, `gtPlatformWin.cpp`
- Create: `LibXLT/Graphics/G3d/g3dGfxOptions.hpp`, `LibXLT/Graphics/G3d/private/g3dGfxOptions.cpp`
- Test: `UnitTests/GraphicsTests/G3d/g3dGfxOptionsTest.cpp`
- Modify: `LibXLT/Graphics/Graphics.vcxproj` (+ `.filters`), `MachStudio/Source/MachStudio.sln`, `MachStudio/Source/MainApp/MainApp.vcxproj` (ProjectReference to GraphicsTests, like CoreTests)

**Interfaces:**
- Produces:
  ```cpp
  struct g3dGfxOptions
  {
      std::string m_ProfileFile;  // empty: profiling off
      static g3dGfxOptions Parse(const std::string& i_CommandLine);
  };
  ```
  Tokens are separated by spaces/tabs; a token may be double-quoted (quotes removed, spaces kept). Flags accept `-` or `/` prefix. A flag missing its value leaves the field empty. Unknown tokens are ignored.

- [ ] **Step 1: Create GraphicsTests by copying CoreTests' setup.** Copy `CoreTests.vcxproj` (new `ProjectGuid`, `RootNamespace`/`TargetName` `GraphicsTests`), `vcpkg.json`, `vcpkg-configuration.json`; copy `ctPlatform.hpp`/`ctPlatformWin.cpp` as `gtPlatform.hpp`/`gtPlatformWin.cpp` (class `gtPlatform`); `GraphicsTestsMain.cpp` is `CoreTestsMain.cpp` with those names. Add a ProjectReference to `LibXLT/Graphics/Graphics.vcxproj` as well as Core, and Graphics' include root. Add the project to `MachStudio.sln` in all six configurations, and a ProjectReference to it from `MainApp.vcxproj` (nothing linked), as CoreTests has.

- [ ] **Step 2: Write the failing tests** in `UnitTests/GraphicsTests/G3d/g3dGfxOptionsTest.cpp`:

```cpp
TEST(g3dGfxOptions, EmptyLineHasNoProfileFile)
{ EXPECT_EQ("", g3dGfxOptions::Parse("").m_ProfileFile); }

TEST(g3dGfxOptions, ReadsProfileFile)
{ EXPECT_EQ("out.csv", g3dGfxOptions::Parse("-gfxProfile out.csv -render a.mab").m_ProfileFile); }

TEST(g3dGfxOptions, ReadsQuotedPathWithSpaces)
{ EXPECT_EQ("C:\\my dir\\t.csv", g3dGfxOptions::Parse("-render a.mab -gfxProfile \"C:\\my dir\\t.csv\"").m_ProfileFile); }

TEST(g3dGfxOptions, AcceptsSlashPrefix)
{ EXPECT_EQ("t.csv", g3dGfxOptions::Parse("/gfxProfile t.csv").m_ProfileFile); }

TEST(g3dGfxOptions, MissingValueLeavesEmpty)
{ EXPECT_EQ("", g3dGfxOptions::Parse("-render a.mab -gfxProfile").m_ProfileFile); }

TEST(g3dGfxOptions, DoesNotMatchLongerFlag)
{ EXPECT_EQ("", g3dGfxOptions::Parse("-gfxProfileX t.csv").m_ProfileFile); }
```

- [ ] **Step 3: Build GraphicsTests (Debug Qt) and confirm the build fails** because `g3dGfxOptions.hpp` doesn't exist.

Run: `msbuild MachStudio/Source/MachStudio.sln /t:GraphicsTests /p:Configuration="Debug Qt" /p:Platform=x64 /m`
Expected: error C1083 for `Graphics/G3d/g3dGfxOptions.hpp`.

- [ ] **Step 4: Implement `g3dGfxOptions::Parse`** in `LibXLT/Graphics/G3d/private/g3dGfxOptions.cpp` (add both files to `Graphics.vcxproj` and filters).

- [ ] **Step 5: Build again; the post-build run passes.**
Expected: `[  PASSED  ] 6 tests.` and `UnitTests/bin/x64/Debug Qt/GraphicsTests.xml` written.

- [ ] **Step 6: Commit**

```bash
git add UnitTests/GraphicsTests LibXLT/Graphics/G3d/g3dGfxOptions.hpp LibXLT/Graphics/G3d/private/g3dGfxOptions.cpp LibXLT/Graphics/Graphics.vcxproj LibXLT/Graphics/Graphics.vcxproj.filters MachStudio/Source/MachStudio.sln MachStudio/Source/MainApp/MainApp.vcxproj
git commit -m "Add GraphicsTests and g3dGfxOptions (-gfxProfile)"
```
(Don't stage `UnitTests/GraphicsTests/vcpkg_installed`; check `.gitignore` covers it as for CoreTests.)

---

### Task 2: `g3dProfiler` core

**Files:**
- Create: `LibXLT/Graphics/G3d/g3dProfiler.hpp`, `LibXLT/Graphics/G3d/private/g3dProfiler.cpp`
- Test: `UnitTests/GraphicsTests/G3d/g3dProfilerTest.cpp`
- Modify: `LibXLT/Graphics/Graphics.vcxproj` (+ filters), `UnitTests/GraphicsTests/GraphicsTests.vcxproj` (+ filters)

**Interfaces:**
- Consumes: nothing from earlier tasks.
- Produces (all in `g3dProfiler.hpp`):
  ```cpp
  class g3dProfilerGpu   // implemented per backend
  {
  public:
      virtual ~g3dProfilerGpu() {}
      virtual void BeginFrame(uint64_t i_FrameId) = 0;
      virtual int  BeginScope() = 0;             // returns a token, 0..n-1 in order of BeginScope within the frame
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
      static void Stop();                         // resolves (waiting) and writes everything, closes the file
      static bool IsEnabled();                    // true between Start and Stop
      static void SetGpu(g3dProfilerGpu* i_pGpu); // not owned; NULL = CPU only
      static void BeginFrame(float i_SimTime);
      static void EndFrame();
      static bool IsInFrame();
      static void PushScope(const char* i_Name); // name must be a string literal (pointer kept)
      static void PopScope();
      static void AddDraws(int i_Count);          // added to every open scope (inclusive counts)
  };

  class g3dProfileScope       { public: explicit g3dProfileScope(const char* i_Name); ~g3dProfileScope(); };
  class g3dProfileFrameScope  { public: g3dProfileFrameScope(const char* i_Name, float i_SimTime); ~g3dProfileFrameScope(); };
  // FrameScope: if no frame is open it begins and ends a frame (root scope named i_Name);
  // otherwise it is an ordinary nested scope.

  #ifdef MSP_GFX_PROFILING
  #define G3D_PROFILE_SCOPE(name)            g3dProfileScope g3dProfileScope_##__LINE__(name)
  #define G3D_PROFILE_FRAME_SCOPE(name, t)   g3dProfileFrameScope g3dProfileFrameScope_##__LINE__(name, t)
  #define G3D_PROFILE_DRAWS(n)               g3dProfiler::AddDraws(n)
  #else
  #define G3D_PROFILE_SCOPE(name)            ((void)0)
  #define G3D_PROFILE_FRAME_SCOPE(name, t)   ((void)0)
  #define G3D_PROFILE_DRAWS(n)               ((void)0)
  #endif
  ```
  (Use a two-level macro concatenation so `__LINE__` expands.) Every entry point returns immediately when `!IsEnabled()`.
- CSV format, one row per scope per frame, written in frame order:
  `backend,frame,sim_time,path,depth,cpu_ms,gpu_ms,draws`
  `path` joins scope names with `/` from the root (`Target/HDR/Lit`); `depth` is 0 for the root; `cpu_ms` is wall time between push and pop (`std::chrono::steady_clock`); `gpu_ms` is empty when no GPU hook is set or the frame's data is invalid/unresolved; `frame` counts from 0. Numbers are written with 4 decimals.

- [ ] **Step 1: Write the failing tests** in `g3dProfilerTest.cpp`, using a `FakeGpu : g3dProfilerGpu` that returns `1.0 + token` ms per scope, resolves each frame on the next `Resolve` call, and a helper that reads the CSV back as rows (temp file per test, as `ctTempDir` does in CoreTests).

```cpp
TEST(g3dProfiler, WritesHeader)           // Start + Stop, no frames → file is exactly the header line
TEST(g3dProfiler, NestedScopesGivePaths)  // frame "Target" { "HDR" { "Lit" } } → paths Target, Target/HDR, Target/HDR/Lit; depths 0,1,2
TEST(g3dProfiler, GpuTimesMatchTokens)    // with FakeGpu: gpu_ms 1.0000, 2.0000, 3.0000 in scope order
TEST(g3dProfiler, DrawsAreInclusive)      // AddDraws(5) in Lit, AddDraws(2) in HDR → Lit 5, HDR 7, Target 7
TEST(g3dProfiler, FrameScopeNestsInsideOpenFrame) // FrameScope inside a frame adds a scope, not a new frame (one frame id)
TEST(g3dProfiler, NoGpuLeavesGpuEmpty)    // SetGpu(NULL) → gpu_ms column empty, cpu_ms > 0
TEST(g3dProfiler, InvalidGpuFrameLeavesGpuEmpty) // FakeGpu reports o_bValid=false → gpu_ms empty
TEST(g3dProfiler, FlushWritesUnresolvedFramesWithEmptyGpu) // FakeGpu that never resolves; 3 frames; Stop → 3 frames written, gpu empty
TEST(g3dProfiler, DisabledIsNoOp)         // without Start: BeginFrame/PushScope/AddDraws don't crash; IsInFrame() false
TEST(g3dProfiler, SimTimeAndBackendWritten) // Start(path,"dx11"), BeginFrame(1.5f) → backend dx11, sim_time 1.5000
```

- [ ] **Step 2: Build; the tests fail to compile** (no `g3dProfiler.hpp`).

- [ ] **Step 3: Implement `g3dProfiler`** in `g3dProfiler.cpp`. Keep finished frames (scope list with CPU times and draws) in a deque until `Resolve` returns their GPU times, then write them; on `Stop`, call `Resolve(true, ...)` until it returns false and write the rest with empty `gpu_ms`. `DBG_ASSERT` on unbalanced Push/Pop and on `EndFrame` with open scopes.

- [ ] **Step 4: Build; all GraphicsTests pass.**

- [ ] **Step 5: Commit**

```bash
git add LibXLT/Graphics/G3d/g3dProfiler.hpp LibXLT/Graphics/G3d/private/g3dProfiler.cpp UnitTests/GraphicsTests/G3d/g3dProfilerTest.cpp LibXLT/Graphics/Graphics.vcxproj LibXLT/Graphics/Graphics.vcxproj.filters UnitTests/GraphicsTests/GraphicsTests.vcxproj UnitTests/GraphicsTests/GraphicsTests.vcxproj.filters
git commit -m "Add g3dProfiler: per-frame CPU/GPU scope timing to CSV"
```

---

### Task 3: Profiling switch, shared markers, and MachStudio wiring

**Files:**
- Modify: `Directory.Build.props`
- Modify: `LibXLT/Graphics/G3d/private/g3dTargetRenderer.cpp`, `g3dCubicReflectionTargetRenderer.cpp`, `g3dPlanarReflectionTargetRenderer.cpp`
- Modify: `MachStudio/Source/MainApp/WinMain.cpp` (`DoWinMain`), `mainInitGraphics.cpp`, `mnmApp.cpp`

**Interfaces:**
- Consumes: `g3dGfxOptions::Parse` (Task 1); `g3dProfiler::Start/Stop`, `G3D_PROFILE_*` (Task 2).
- Produces: `mainInitGraphics` gains `static void SetGfxOptions(const g3dGfxOptions&)`, called by `DoWinMain` before the `mainInitGraphics` object is constructed. Profiler scope names used by later tasks/plans: root `"Target"`, reflections `"CubeReflection"`, `"PlanarReflection"`.

- [ ] **Step 1: Define `MSP_GFX_PROFILING`** in `Directory.Build.props`: an `ItemDefinitionGroup` adding it to `ClCompile/PreprocessorDefinitions` when `'$(MspBaseConfiguration)' == 'Debug' or '$(MspGfxProfiling)' == 'true'`. Extend the header comment to say so.

- [ ] **Step 2: Add the shared markers.** First statement of `g3dTargetRenderer::Render(float, const std::vector<g3dLayer*>&, bool)`: `G3D_PROFILE_FRAME_SCOPE("Target", i_fSimTime);`. First statement of `g3dCubicReflectionTargetRenderer::Render` / `g3dPlanarReflectionTargetRenderer::Render`: `G3D_PROFILE_SCOPE("CubeReflection");` / `G3D_PROFILE_SCOPE("PlanarReflection");`.

- [ ] **Step 3: Wire MachStudio.**
  - `DoWinMain`: first thing, `mainInitGraphics::SetGfxOptions(g3dGfxOptions::Parse(envString::WideCharToUTF8(lpCmdLine)));`.
  - `mainInitGraphics` constructor, right after `GraphicsDX11Layer::InitGraphics()`: if the options' `m_ProfileFile` is non-empty, `g3dProfiler::Start(file, "dx11")`; if `MSP_GFX_PROFILING` is not defined, `DBG_LOG` (and `std::cerr` in `BATCH_MODE`) that profiling is compiled out, and don't start. Destructor, before `GraphicsDX11Layer::CleanUpGraphics()`: `g3dProfiler::Stop()`.
  - `mnmApp::ParseCommandLine`: add a `-gfxProfile`/`/gfxProfile` branch that consumes the value token (with `seps_tok1`, like `-capOutputFile`) and sets `bParsedAnyData = true`. Replace `char pLine[1024]` + `strcpy` with a `std::vector<char>` sized `i_lpCmdLine.size() + 1`. Add `-gfxProfile <file.csv>` to the `-help` text.

- [ ] **Step 4: Build `MainApp;GraphicsTests;CoreTests` in Debug Qt, Release Qt, Debug RenderNode, Release RenderNode, Debug Wx, Release Wx.**
Expected: all succeed; unit tests pass.

- [ ] **Step 5: Smoke-test in Debug Qt:** run `MachStudio-x64-Qt-Debug.exe -gfxProfile %TEMP%\p.csv`, orbit the viewport a few seconds, quit.
Expected: `p.csv` has the header, `backend` `dx11`, rows with path `Target`, empty `gpu_ms` (no GPU hook yet), and the app started normally (didn't try to open `-gfxProfile ...` as a scene).

- [ ] **Step 6: Commit**

```bash
git add Directory.Build.props LibXLT/Graphics/G3d/private/g3dTargetRenderer.cpp LibXLT/Graphics/G3d/private/g3dCubicReflectionTargetRenderer.cpp LibXLT/Graphics/G3d/private/g3dPlanarReflectionTargetRenderer.cpp MachStudio/Source/MainApp/WinMain.cpp MachStudio/Source/MainApp/mainInitGraphics.cpp MachStudio/Source/MainApp/mainInitGraphics.hpp MachStudio/Source/MainApp/mnmApp.cpp
git commit -m "Wire -gfxProfile and MSP_GFX_PROFILING into MachStudio"
```

---

### Task 4: DX11 GPU timers and DX11 markers

**Files:**
- Create: `LibXLT/GraphicsDX11/G3d/g3dProfilerGpuDX11.hpp`, `LibXLT/GraphicsDX11/G3d/private/g3dProfilerGpuDX11.cpp`
- Modify: `LibXLT/GraphicsDX11/GraphicsDX11.vcxproj` (+ filters), `GraphicsDX11Layer.cpp`
- Modify (one marker line each): every class in GraphicsDX11 that overrides `g3dSceneRenderer::Render` or `g3dRenderPass::Render`; the draw/dispatch sites in `bump/private/bumpBumpRenderer.cpp`, `G2d/private/g2dFontUtilDX11.cpp`, `G2d/private/g2dFullscreenQuad.cpp`, `hair/private/HairRendererDX11.cpp`, `shdw/private/rtUtil.cpp`, `shdw/private/shdwPassLPVGI.cpp`

**Interfaces:**
- Consumes: `g3dProfilerGpu`, `g3dProfiler::SetGpu`, `G3D_PROFILE_SCOPE`, `G3D_PROFILE_DRAWS` (Task 2).
- Produces: `class g3dProfilerGpuDX11 : public g3dProfilerGpu` (constructor takes nothing; uses `g2dDX11Global::g_pDevice`/`g_pDeviceContext`). Scope naming rule that Plan 2+ copies for Vulkan: **the class name without its package prefix and without a `DX11` suffix** — `shdwHDRRendererDX11` → `"HDRRenderer"`, `shdwPassLit` → `"PassLit"`, `g3dDepthMapRendererDX11` → `"DepthMapRenderer"`.

- [ ] **Step 1: Implement `g3dProfilerGpuDX11`.** Per frame: one `D3D11_QUERY_TIMESTAMP_DISJOINT`, a pair of `D3D11_QUERY_TIMESTAMP` per scope (pooled, grown on demand). Keep up to 4 frames in flight; if a 5th begins before the oldest resolves, resolve the oldest with waiting. `Resolve` polls with `GetData(..., D3D11_ASYNC_GETDATA_DONOTFLUSH)` (or spins with flush when `i_bWait`); if `Disjoint` is TRUE, set `o_bValid = false`. Whole file inside `#ifdef MSP_GFX_PROFILING`.

- [ ] **Step 2: Install it.** In `GraphicsDX11Layer::InitGraphics`, under `#ifdef MSP_GFX_PROFILING`, create it and `g3dProfiler::SetGpu`; in `CleanUpGraphics`, `SetGpu(NULL)` and delete (after MainApp's `g3dProfiler::Stop`, which already ran).

- [ ] **Step 3: Add markers.** First statement of each overriding `Render` named above: `G3D_PROFILE_SCOPE("<name by the rule>");`. After each `Draw*`/`Dispatch` call: `G3D_PROFILE_DRAWS(1);`. List the classes touched in the commit message body.

- [ ] **Step 4: Prove the markers compile out.** Build Release Qt without `MspGfxProfiling`; run `dumpbin /symbols` (or check the `.obj` list) to confirm `g3dProfilerGpuDX11` isn't compiled, and `findstr /C:"PassLit"` over `MachStudio-x64-Qt-Release.exe` finds nothing.
Expected: no hits.

- [ ] **Step 5: Real run, Debug RenderNode, on the test scene:**

Run: `MachStudio-x64-RenderNode-Debug.exe -gfxProfile %TEMP%\dx11.csv -capFormat PNG -capOutputPath %TEMP%\dx11a -render "C:\Users\dmt\Desktop\Dume_Demo\Dume_Demo_testTESS.mab"`
Expected: PNG frames in `%TEMP%\dx11a`; CSV rows under `Target/HDRRenderer/...` (or whichever renderer the scene uses) with non-empty `gpu_ms`, draw counts > 0, and per-frame root `gpu_ms` roughly matching the sum of its children. Record in the commit message the exact flag order that works, the output file naming pattern and the frame count; Task 6 relies on them.

- [ ] **Step 6: Commit**

```bash
git add LibXLT/GraphicsDX11/G3d/g3dProfilerGpuDX11.hpp LibXLT/GraphicsDX11/G3d/private/g3dProfilerGpuDX11.cpp LibXLT/GraphicsDX11/GraphicsDX11.vcxproj LibXLT/GraphicsDX11/GraphicsDX11.vcxproj.filters LibXLT/GraphicsDX11/GraphicsDX11Layer.cpp <each marker file>
git commit -m "DX11 GPU timers and profiling markers (compiled out by default)"
```

---

### Task 5: `gfxcompare` image and timing comparison (Python)

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
      name: str; passed: bool; reason: str   # reason '' when passed
      max_err: float; mean_err: float; pct_over: float
  def load_image(path: Path) -> np.ndarray          # float32 HxWxC in [0,1] for PNG (8/16-bit), raw floats for .pfm
  def compare_images(a: np.ndarray, b: np.ndarray, tol: Tolerance, name: str) -> FrameResult
  def diff_image(a: np.ndarray, b: np.ndarray) -> np.ndarray   # uint8 HxWx3 heat map of per-pixel max channel error, scaled so max_err*4 is white
  def compare_dirs(dir_a: Path, dir_b: Path, tol: Tolerance, diff_dir: Path | None) -> list[FrameResult]
      # matches files by name (.png/.pfm); a name in only one folder → FrameResult(passed=False, reason='missing in A'/'missing in B')
  # timing.py
  def load_csv(path: Path) -> list[dict]
  def summarize(rows: list[dict]) -> dict[str, dict]   # path → {"cpu_median","gpu_median","draws_median","count"}; gpu_median None if no gpu values
  def compare_timing(a: dict, b: dict) -> list[dict]   # per path present in either: a/b medians and ratio b/a (None if missing)
  ```
  Errors are measured per channel on RGB (alpha ignored unless both images have it; then RGBA).

- [ ] **Step 1: Write the failing tests:**

```python
def test_identical_images_pass(): ...            # max_err 0, passed
def test_one_level_difference_passes(): ...      # every pixel off by 1/255 → max_err 1/255, pct_over 0, passed
def test_three_levels_fails_max(): ...           # one pixel off by 3/255 → passed False, 'max_err' in reason
def test_many_pixels_over_fail_pct(): ...        # 1% of pixels off by 2/255 → passed False, 'pct_over' in reason
def test_size_mismatch_fails(): ...              # 4x4 vs 4x5 → passed False, 'size' in reason
def test_nan_fails(): ...                        # float image containing NaN → passed False, 'non-finite' in reason
def test_png_16bit_loads_normalized(tmp_path): ...
def test_pfm_roundtrip(tmp_path): ...            # write a little-endian PFM (negative scale), load_image returns same floats, rows not flipped
def test_compare_dirs_reports_missing(tmp_path): ...  # f1 in both, f2 only in A → 2 results, f2 'missing in B'
def test_compare_dirs_writes_diff_for_failures_only(tmp_path): ...
def test_summarize_medians(): ...                # three rows for Target cpu 1,2,9 → cpu_median 2
def test_summarize_empty_gpu_is_none(): ...
def test_compare_timing_ratio(): ...             # a gpu 2.0, b gpu 3.0 → ratio 1.5; path only in b → ratio None
```

- [ ] **Step 2: Run them and see them fail.**
Run: `python -m pytest Tools/gfxcompare/tests -q`
Expected: import errors.

- [ ] **Step 3: Implement `imgcompare.py` and `timing.py`.** PFM: header `PF`/`Pf`, width height, scale (negative = little-endian), rows stored bottom-to-top — flip to top-to-bottom.

- [ ] **Step 4: Run the tests; all pass.**

- [ ] **Step 5: Commit**

```bash
git add Tools/gfxcompare/imgcompare.py Tools/gfxcompare/timing.py Tools/gfxcompare/requirements.txt Tools/gfxcompare/tests
git commit -m "gfxcompare: image and timing comparison"
```

---

### Task 6: `gfxcompare` CLI, RenderNode runs, DX11-vs-DX11 proof, CI and docs

**Files:**
- Create: `Tools/gfxcompare/rendernode.py`, `Tools/gfxcompare/gfxcompare.py`, `Tools/gfxcompare/README.md`
- Test: `Tools/gfxcompare/tests/test_rendernode.py`, `Tools/gfxcompare/tests/test_cli.py`
- Modify: `.github/workflows/build-windows.yml`, `CLAUDE.md`

**Interfaces:**
- Consumes: Task 5's functions; the flag order and output naming recorded in Task 4's commit.
- Produces:
  ```python
  # rendernode.py
  def build_command(exe: Path, scene: Path, out_dir: Path, profile_csv: Path,
                    extra_args: list[str], fmt: str = "PNG",
                    width: int | None = None, height: int | None = None) -> list[str]
      # raises ValueError if the joined command line exceeds 30000 characters
  def run(cmd: list[str], timeout_s: float) -> None      # raises RuntimeError with exit code and stderr tail on failure, or if no image was written
  ```
  CLI:
  - `gfxcompare.py run --exe EXE [--scene MAB] --out DIR [--a-args "..."] [--b-args "..."] [--format PNG|PFM] [--width W --height H] [--timeout S]`: renders into `DIR/a` and `DIR/b` (each with `timing.csv`), then compares. `--scene` defaults to `$MSP_GFX_TEST_SCENE`; error if neither is set. `--a-args`/`--b-args` default to empty (Plan 2 passes `-gfx dx11` / `-gfx vulkan`).
  - `gfxcompare.py compare DIR_A DIR_B --out DIR`: comparison only.
  - Both write `DIR/report.md` (per-frame table, failing frames first, linked diff images in `DIR/diff/`; timing table per path with A, B and ratio for CPU and GPU) and exit 0 only if every frame passed.
  - Tolerance flags `--max-err`, `--pixel-err`, `--max-pct-over` (in 1/255 units for the first two), with the defaults from Global Constraints; the report states the tolerance used.

- [ ] **Step 1: Write the failing tests:** `test_build_command_order` (cap flags and `-gfxProfile` before `-render`, extra args first — matching Task 4's recorded order), `test_build_command_too_long_raises`, `test_cli_compare_exit_codes(tmp_path)` (identical folders → 0; one differing frame → 1 and `report.md` lists it first), `test_cli_run_requires_scene(monkeypatch)` (no `--scene`, no env → exit code 2 with a message).

- [ ] **Step 2: Run; they fail.** `python -m pytest Tools/gfxcompare/tests -q`

- [ ] **Step 3: Implement `rendernode.py` and `gfxcompare.py` (argparse).**

- [ ] **Step 4: Run; all pass.**

- [ ] **Step 5: DX11 against DX11 on the test scene (Release RenderNode, with `/p:MspGfxProfiling=true`):**

Run: `python Tools/gfxcompare/gfxcompare.py run --exe MachStudio/MachStudio-x64/MachStudio-x64-RenderNode-Release.exe --scene C:\Users\dmt\Desktop\Dume_Demo\Dume_Demo_testTESS.mab --out %TEMP%\gfx-self`
Expected: exit 0; every frame `max_err` 0 or within tolerance; timing table with A/B ratios near 1.0. **If frames differ between two DX11 runs, stop and report to the user** (the renderer isn't deterministic — e.g. random jitter seeds) rather than loosening the tolerance.

- [ ] **Step 6: CI.** In `build-windows.yml`: build `MainApp;CoreTests;GraphicsTests` (wherever CoreTests is listed), upload `GraphicsTests.xml` with the other reports, and add a step `python -m pip install -r Tools/gfxcompare/requirements.txt` + `python -m pytest Tools/gfxcompare/tests -q`.

- [ ] **Step 7: Docs.** `CLAUDE.md`: GraphicsTests in "Unit tests"; `MSP_GFX_PROFILING`/`MspGfxProfiling` and `-gfxProfile` in "Building"; `Tools/gfxcompare` under "Other top-level directories". `Tools/gfxcompare/README.md`: setup, the two commands, tolerance, CSV columns.

- [ ] **Step 8: Commit**

```bash
git add Tools/gfxcompare/rendernode.py Tools/gfxcompare/gfxcompare.py Tools/gfxcompare/README.md Tools/gfxcompare/tests/test_rendernode.py Tools/gfxcompare/tests/test_cli.py .github/workflows/build-windows.yml CLAUDE.md
git commit -m "gfxcompare CLI; DX11 self-comparison of the test scene; CI and docs"
```
