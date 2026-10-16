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
4. `9eb24341` Add unit test work status note for handoff (this file)
5. `e537ba82` Fix number reading and writing in text and XML files (see
   "Number reading and writing" below)
6. Run the unit tests in the build before MachStudio (see "Tests run before
   MachStudio" below; also updates this file)

All pushed to `origin/claude/unit-tests`.

What exists (details are in CLAUDE.md under "Unit tests"):

- `UnitTests/CoreTests/CoreTests.vcxproj`: 128 gtest tests for `LibXLT/Core`,
  gtest from `UnitTests/CoreTests/vcpkg.json`. In `MachStudio.sln`; built in
  Debug/Release x Qt/Wx. Self-contained (no shared props/targets); its
  Post-Build Event runs the tests.
- `MainApp.vcxproj` has a ProjectReference to CoreTests (nothing linked;
  Qt/Wx configurations only), so MachStudio isn't built until the tests
  pass.
- CI (`.github/workflows/build-windows.yml`): builds `"MainApp;CoreTests"`
  (which runs the tests) and uploads the XML reports.

### Tests run before MachStudio in every build

- CoreTests' Post-Build Event (Project Properties → Build Events) runs the
  tests after every link and writes `CoreTests.xml` next to the exe; a
  failing test fails the build. There is no opt-out. CI's separate "Run
  unit tests" step is gone; the upload step picks up that XML.
- `MainApp.vcxproj` references CoreTests, so MachStudio waits for the tests
  and isn't built if they fail. The libraries still build in parallel
  with the tests. Test projects may reference whatever libraries they
  test; a new one gets the same reference from MainApp.
- `UnitTests.props` and `UnitTests.targets` are gone: their settings are in
  `CoreTests.vcxproj` (a new test project can start as a copy). An
  earlier version made every project wait for the tests through a props
  file imported from `Directory.Build.props`; it was dropped in favour of
  the one visible ProjectReference. CLAUDE.md has a "Build files" table
  listing what each remaining shared MSBuild file does.
- Checked in Debug Wx: with a deliberately failing test, a MainApp build
  stopped at CoreTests (the libraries finished in parallel; MainApp never
  compiled), and a CoreTests build failed in the Post-Build Event with the
  failure reported at the test's file and line. With it removed, Debug Wx
  and Release Wx CoreTests builds ran and passed the tests.
- The first build of CoreTests on a machine installs gtest through vcpkg.

Ported from the old harnesses: `TestTerawatt/App/chTest`,
`TestTerawatt/Math/geo/geotest`, `TestTerawatt/Math/ma`, and
`TestLibXLT/Core/{Dbg,Env,Fs,Gf,It,Rel}`. The ones that only printed
results now have expected values.

### Number reading and writing

Every number type now reads back exactly from every text and XML format.
Before this, the following were broken:

| Where | Bug |
|---|---|
| `chXMLWriter` (scene documents, materials) | Int8/UInt8 written as the raw byte, so flags such as double-sided read back as 0 |
| `chTxtReader`, `chXMLReader` | integers read with `atoi`: UInt32 above 2^31 and 64-bit values came back wrong |
| `chTxtReader`, `chXMLReader` | Float64 read through a `float` |
| `chTxtWriter`, `chXMLWriter`, `fsXMLWriter` | floats written with an ostream's 6 significant digits |
| `gfFileTxt::ReadToken` | split `1e-05` into `1e` and `-05` (and `-inf`), shifting every later value |
| `fsXMLParser::Convert(Int8/UInt8)` | read one character, not the number |
| `gfFileTxt`, `gfFileXML` | plain `char` passed to `isspace` etc. (undefined for non-ASCII; debug CRT asserts) |

Changes:

- New `Core/Env/envNumberText.hpp`: `ToText`/`FromText` for all ten envType
  number types. Floats are written with `std::to_chars` (the fewest digits
  that read back exactly, so `0.1` stays `0.1`) and read with
  `std::from_chars` (locale-independent). Integers are read through
  `strtoll`/`strtoull`, so out-of-range values still wrap the way
  `atoi` + cast did. All the text/XML readers and writers above, and
  `guiXMLTextReader` (Tool), now use it.
- `chTxtReader`/`chXMLReader` read a one-character, non-numeric Int8/UInt8
  token as the raw byte, so most files saved with the old writers still
  load. Bytes that are whitespace, digits, `"` or `#` can't be recovered.
- `gfFileTxt` tokenizer: keeps an exponent's sign (`1e-05`, `1e+38`) and
  `-inf`/`-nan` in one token; both tokenizers take `unsigned char`.
- `fsXMLData` (the node-type enum) moved into its own `Core/Fs/fsXMLData.hpp`;
  `fsXMLReader.hpp` and `fsXMLParser.hpp` each defined it, so they couldn't
  be included together.
- Tests: `ctNumbers.hpp` (limits, signed zero, denormals, infinities, NaNs
  and full-precision values for each type), `Env/envNumberTextTest.cpp`,
  `Ch/chNumberTest.cpp` (binary, text and XML chunk files, plus the raw-byte
  fallback and exponent tokens), and a typed round trip in
  `Fs/fsXMLTest.cpp` (fsXMLWriter → fsXMLReader and fsXMLParser). With the
  old `chXMLWriter` and `gfFileTxt`, the Int8, UInt8, Float32, Float64 and
  exponent tests fail.

## Verified

- Debug Wx: 128/128 pass (run by the Post-Build Event).
- Release Wx: 127 pass, 1 skipped (`dbgMsgAssert.HandlerReceivesEachFailure`;
  DBG_ASSERT is compiled out of release builds). Tool and ToolUIWx also
  built in Release Wx. MainApp didn't build here (see `afxres.h` below), so
  its use of `fsXMLParser` (`mainCheckAppVersion.cpp`) hasn't been
  compiled.
- Earlier (79 tests, before the number work): Debug Wx 79/79; Release Wx
  78 pass, 1 skipped (`dbgMsgAssert.HandlerReceivesEachFailure`; DBG_ASSERT
  is compiled out of release builds).

## Not verified yet

- Debug Qt / Release Qt builds of CoreTests (not built locally; CI will be
  the first).
- The CI workflow itself (not pushed yet). Watch the build step (which now
  runs the tests) and the "Upload unit test results" step.
- Visual Studio Test Explorer discovering the tests.
- Building in the Visual Studio IDE (checked with msbuild only).
- Loading real scene/material XML files saved by older versions (the
  raw-byte fallback is tested only on generated files).

## Next steps

1. Check CI on all four configurations (the first run with these commits),
   then open a PR against `dev/update2024`.
2. Delete the old test code (`MachStudio/TestSource/`, `TestLibXLT/`,
   `TestTerawatt/`). Everything worth keeping has been ported.
3. Decide on the unfixed bugs below.
4. Next test targets: `MachStudio/Source/Support/tmln` (channel blending,
   smooth-blend easing, driver evaluation order) and the `...DataParser` chunk
   parsers. That needs a second test project (e.g. `UnitTests/MachStudioTests`)
   (start from a copy of `CoreTests.vcxproj`) that links more libraries,
   with a ProjectReference to it from MainApp.
   `guiXMLTextReader::Convert` (Tool) is only covered through
   envNumberText until then.

## Found but not fixed

- `chWriter::Write(const itString&)` (`Core/Ch/private/chWriter.cpp`) asserts on
  an explicit trailing NUL, which itStrings no longer have, and indexes `[-1]`
  on an empty string. Callers use `Write(str.GetString())` instead.
- `Core/Dbg/dbgAssert.hpp`/`.cpp` are dead code (the header starts with
  `#error`, and neither file is in `Core.vcxproj`), so the old
  `DBG_ASSERT0`..`9` macros no longer exist.
- Big-endian support (`ENV_ENDIAN` in `envPlatform.hpp`, the byte swapping
  in `gfFileUtil.hpp`, `gfFileBin::e_BigEndian`, the
  `gfFileUtil.EndianRoundTrip` test) can go: only little-endian platforms
  need supporting. `envPlatform.hpp` still maps `ENV_MAC` to big-endian (a
  PowerPC-era leftover; Macs have been little-endian since 2006). Nothing
  selects `ENV_MAC` today, but a macOS port that did would byte-swap
  every binary file.
- `mainCheckAppVersion.cpp` frees a `new[]` buffer with `delete`.
- The dbgMsg tests print `(1) ****ERROR**** : ...` lines (the error stream
  echoing to stdout), which now appear in every build that runs the tests.
  Harmless, but noisy; the tests could redirect that stream.
- Files saved by pre-2015 MSVC runtimes may hold floats as `1.#INF` or
  `1.#QNAN`; these read as 1 (as they did with `atof`).

## Things that bit during this work

- Core headers use `#error` "multiply included" guards, so test files must
  guard their includes (`#ifndef MA_PLANE_HPP` ...).
- Most source and project files use CRLF line endings (`MachStudio.sln`,
  `.vcxproj`, most `.cpp`/`.hpp`). Git Bash's `sed -i` silently converts a
  whole file to LF, so edit them with the editor or a script that keeps
  CRLF.
- The projects use the v145 toolset: build with the Visual Studio 18
  MSBuild (`C:\Program Files\Microsoft Visual Studio\18\...`), not 2022's.
  On this machine VS 18 has no MFC headers, so MainApp's `app.rc` fails
  (`afxres.h` not found); the libraries and CoreTests build fine.
- The CI shell is PowerShell, so an msbuild `/t:A;B` argument must be quoted.
  From Git Bash, msbuild's `/p:` and `/t:` arguments get mangled into paths;
  run msbuild from PowerShell.
- vcpkg's x64-windows gtest is a DLL (`GTEST_LINKED_AS_SHARED_LIBRARY=1`), and
  the exe resolves gtest from `gmock.dll`, which is copied next to it.
- `maRotation`'s `a * b` applies `b` first; `maMatrix4x4`'s `A * B` applies `A`
  first (row vectors). The tests pin both down.
- `gfFileXML::WriteHeader` also writes an `<SGPU>` root tag, which readers
  must read as a chunk (as `docDocumentWithChunks` does) before the
  document's own chunks.
