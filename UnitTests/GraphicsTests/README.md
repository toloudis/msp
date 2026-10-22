# GraphicsTests

GoogleTest tests for `LibXLT/Graphics` and the Vulkan backend
(`LibXLT/GraphicsVulkan`), most of them comparing Vulkan's results with
GraphicsDX11's (see CLAUDE.md, "Unit tests").

**No build runs these tests.** Building MachStudio (or this project) only
compiles them; CoreTests and MainAppTests still run as part of the build.
Run GraphicsTests yourself after changing graphics code, as below. They need
a GPU: the Vulkan tests are skipped when there is no usable Vulkan device,
and the D3D11 reference falls back to WARP.

## 1. Build

The exe is built with the solution: any `Debug *` configuration builds
`UnitTests\bin\x64\Debug\GraphicsTests.exe`, any `Release *` one builds
`UnitTests\bin\x64\Release\GraphicsTests.exe`. To build only the tests (and
the libraries they use), from the repo root in a VS Developer PowerShell:

```powershell
msbuild MachStudio\Source\MachStudio.sln /t:GraphicsTests "/p:Configuration=Release Qt" /p:Platform=x64 /m
```

Use `"Debug Qt"` for the Debug exe. The build also copies the DLLs the exe
needs next to it, so it runs from any folder.

## 2. Run all the tests (recommended)

`UnitTests\RunShards.ps1` runs the exe as several processes at once, each
taking a share of the tests (gtest sharding), then prints each shard's
output in turn and merges their XML reports into one. Most of the run is
device and layer start-up, so this is much faster than one process. From
the repo root:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File UnitTests\RunShards.ps1 `
    -Exe UnitTests\bin\x64\Release\GraphicsTests.exe `
    -Xml UnitTests\bin\x64\Release\GraphicsTests.xml `
    --gtest_brief=1
```

This is the command the build's post-build step used to run (as
`-Exe "$(TargetPath)" -Xml "$(OutDir)$(TargetName).xml"`). Arguments:

| Argument | |
|---|---|
| `-Exe <path>` | The test exe (required) |
| `-Xml <path>` | Where to write the merged XML report (required) |
| `-Shards <n>` | How many processes to run at once; default 8, or the core count if lower (more gave no gain on a 12-core machine) |
| anything else | Passed to every shard: gtest flags, e.g. `--gtest_brief=1` (print only failures) or `--gtest_filter=...` |

It ends with a line such as `922 tests in 8 shards, 64.2 s: 0 failed`, and
its exit code is 0 only if every shard passed (a failed or crashed shard is
named, with its output above).

## 3. Run some of the tests

Run the exe directly for a few tests: one process, output in order, and
the way to debug a test (set the exe as the debug target in Visual Studio,
with the same arguments).

```powershell
UnitTests\bin\x64\Release\GraphicsTests.exe --gtest_filter=vkdImageTest.*
UnitTests\bin\x64\Release\GraphicsTests.exe --gtest_filter=shdw*:hairVkTest.*
UnitTests\bin\x64\Release\GraphicsTests.exe --gtest_filter=*-shdw*        # all but the shdw suites
```

`--gtest_filter` also works through `RunShards.ps1`. Useful gtest flags:

| Flag | |
|---|---|
| `--gtest_filter=<patterns>` | Only tests matching `Suite.Test` patterns (`*` and `?`; `:` separates patterns; after `-`, patterns to leave out) |
| `--gtest_list_tests` | List the tests (with the filter applied) without running them |
| `--gtest_brief=1` | Print only failures and the summary |
| `--gtest_repeat=<n>`, `--gtest_shuffle` | Run repeatedly / in random order, to find flaky or order-dependent tests |
| `--gtest_break_on_failure` | Stop in the debugger at the first failure |
| `--gtest_output=xml:<file>` | Write an XML report (`RunShards.ps1` does this itself) |

Suites are named after what they test: `vkd*` (Vulkan wrappers), `fx*`,
`eff*`, `g2d*`, `g3d*`, `mat*`, `shdw*` (the multi-pass renderers), `hair*`,
`bump*`, `gt*` (the test helpers themselves). Visual Studio's Test Explorer
(Google Test adapter) also lists and runs them.

## Environment variables

Read by the tests as MachStudio reads them:

| Variable | |
|---|---|
| `MSP_GFX_VK_ADAPTER=<name substring>` | The GPU to run the Vulkan tests on (by default a discrete GPU first, then the most device-local memory) |
| `MSP_GFX_VK_VALIDATION=gpu,bestpractices` | Adds GPU-assisted validation (`gpu`) and/or best-practices notes (`bestpractices`), either or both. Ordinary validation is always on in the tests: any validation message fails the test |

```powershell
$env:MSP_GFX_VK_VALIDATION = 'gpu'
UnitTests\bin\x64\Debug\GraphicsTests.exe --gtest_filter=vkd*
Remove-Item Env:MSP_GFX_VK_VALIDATION
```

## When a test fails

- An image comparison that fails writes `dx11.png`, `vulkan.png` and
  `diff.png` to
  `UnitTests\bin\x64\<Debug|Release>\GraphicsTestsImages\<test>\`.
- A `DBG_ASSERT` that fires is reported as a test failure (no dialog).
- The tests keep the user data folder (the Vulkan pipeline cache) in
  `%TEMP%\MspGraphicsTests\`, one folder per shard; delete it to start
  from an empty cache.
