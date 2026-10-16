# Unit tests: work-in-progress status

Handoff note for resuming this work on another machine. Delete it once the
branch is merged.

Branch: `claude/unit-tests` (based on `claude/toolkit-input` at `a12c4b6f`).

## Done

Commits on the branch:

1. `e40b73fb` Add an assert handler hook to dbgMsg
   (`dbgMsg::SetAssertHandler`; the assert stream is also cleared after each assert)
2. `3cdab3df` Fix Int8/UInt8 round trips in text chunk and XML files
   (`chTxtWriter::Write(Int8/UInt8)`, `fsXMLReader::Convert(Int8/UInt8)`).
   Kept separate so it can be reviewed or dropped on its own.
3. `2c69c51e` Add GoogleTest unit tests for LibXLT Core
   (`UnitTests/`, solution entry, CI steps, `.gitignore`, `CLAUDE.md`)

What exists (details are in CLAUDE.md under "Unit tests"):

- `UnitTests/CoreTests/CoreTests.vcxproj`: 79 gtest tests for `LibXLT/Core`,
  gtest from `UnitTests/CoreTests/vcpkg.json`. In `MachStudio.sln`; built in
  Debug/Release x Qt/Wx.
- `UnitTests/UnitTests.props` / `.targets`: shared settings, plus the optional
  run-after-build target (`MspRunUnitTests=true`).
- CI (`.github/workflows/build-windows.yml`): builds `"MainApp;CoreTests"`, runs
  `CoreTests.exe --gtest_output=xml:...`, uploads the XML.

Ported from the old harnesses: `TestTerawatt/App/chTest`,
`TestTerawatt/Math/geo/geotest`, `TestTerawatt/Math/ma`, and
`TestLibXLT/Core/{Dbg,Env,Fs,Gf,It,Rel}`. The ones that only printed
results now have expected values.

## Verified

- Debug Wx: 79/79 pass.
- Release Wx, built with `/p:MspRunUnitTests=true`: 78 pass, 1 skipped
  (`dbgMsgAssert.HandlerReceivesEachFailure`; DBG_ASSERT is compiled out of
  release builds). The tests ran from the post-build hook.

## Not verified yet

- Debug Qt / Release Qt builds of CoreTests (not built locally; CI will be
  the first).
- The CI workflow itself (not pushed yet). Watch the new "Run unit tests" and
  "Upload unit test results" steps.
- Visual Studio Test Explorer discovering the tests.
- That a failing test fails the build when `MspRunUnitTests=true` (the Exec
  task should do this, but it hasn't been tried).

## Next steps

1. Push the branch, check CI on all four configurations, and open a PR against
   `dev/update2024`.
2. Delete the old test code (`MachStudio/TestSource/`, `TestLibXLT/`,
   `TestTerawatt/`). Everything worth keeping has been ported.
3. Decide on the unfixed bugs below.
4. Next test targets: `MachStudio/Source/Support/tmln` (channel blending,
   smooth-blend easing, driver evaluation order) and the `...DataParser` chunk
   parsers. That needs a second test project (e.g. `UnitTests/MachStudioTests`)
   importing `UnitTests.props`/`.targets` and linking more libraries.

## Found but not fixed

- `LibXLT/Core/Fs/private/fsXMLParser.cpp:159` and
  `LibXLT/Tool/gui/private/guiXMLTextReader.cpp:119` have the same Int8/UInt8
  reading bug that commit 2 fixed in `fsXMLReader`.
- `chWriter::Write(const itString&)` (`Core/Ch/private/chWriter.cpp`) asserts on
  an explicit trailing NUL, which itStrings no longer have, and indexes `[-1]`
  on an empty string. Callers use `Write(str.GetString())` instead.
- `Core/Gf/private/gfFileTxt.cpp` passes plain (possibly negative) `char`s to
  `isspace`/`isdigit`/`isalpha`, which is undefined for non-ASCII bytes. The CRT
  asserts on it in debug builds.
- `Core/Dbg/dbgAssert.hpp`/`.cpp` are dead code (the header starts with
  `#error`), so the old `DBG_ASSERT0`..`9` macros no longer exist.

## Things that bit during this work

- Core headers use `#error` "multiply included" guards, so test files must
  guard their includes (`#ifndef MA_PLANE_HPP` ...).
- `MachStudio.sln` uses CRLF line endings, so edit it with a script that
  preserves them.
- The CI shell is PowerShell, so an msbuild `/t:A;B` argument must be quoted.
- vcpkg's x64-windows gtest is a DLL (`GTEST_LINKED_AS_SHARED_LIBRARY=1`), and
  the exe resolves gtest from `gmock.dll`, which is copied next to it.
- `maRotation`'s `a * b` applies `b` first; `maMatrix4x4`'s `A * B` applies `A`
  first (row vectors). The tests pin both down.
