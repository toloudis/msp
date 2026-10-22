# Unit tests branch: status

Handoff note for continuing this work on another machine: what is left to
do, and what is currently known about the state of the code. Delete it once
the branch is merged. How the unit tests and the build fit together is in
CLAUDE.md ("Building", "Build files", "Unit tests").

Branch: `claude/unit-tests` (based on `claude/toolkit-input` at `a12c4b6f`).

## To do

1. Run CI on the latest commit and open a PR against `dev/update2024`. The
   workflow doesn't run on pushes to this branch: start it by hand (Actions
   → "Windows build" → Run workflow, or
   `gh workflow run build-windows.yml --ref claude/unit-tests`).
2. Finish moving off the hand-made shared MSBuild files: make the toolkit
   choice explicit per configuration in each `.vcxproj`, then delete the root
   `Directory.Build.props` and `MachStudio/Source/Directory.Build.targets`.
   - `wxGUI/`/`qtGUI/` sources: per-file Excluded From Build in the other
     toolkit's configurations (and RenderNode), instead of the targets file
     removing them.
   - Qt settings: an explicit `qt6.props` import in each MachStudio
     project's Qt configurations; `windeployqt` as MainApp's Post-Build
     Event in Qt.
   - `$(MspWxIncludeDirs)`, the `$(MspGuiToolkit)` conditions and
     `$(MspBaseConfiguration)`: literal per-configuration values.
   - `3rdParty/WxWidgets/build/msw/wx_setup.props` reads
     `$(MspBaseConfiguration)` (a local edit to the wxWidgets file), so wx
     depends on the root props file. Switching those conditions to VS's
     own `$(UseDebugLibraries)` would remove that dependency; decide
     whether editing this 3rdParty file is acceptable.
   - Check the result by comparing what MSBuild evaluates for every project
     and configuration (compile items with their settings, project
     references, target names, output dirs) before and after.
3. Add Debug/Release RenderNode to the CI matrix.
4. Delete the old test code (`MachStudio/TestSource/`, `TestLibXLT/`,
   `TestTerawatt/`). Everything worth keeping has been ported to
   `UnitTests/`.
5. Decide on the known issues below.
6. Next test targets: `MachStudio/Source/Support/tmln` (channel blending,
   smooth-blend easing, driver evaluation order) and the `...DataParser` chunk
   parsers. That needs a second test project (e.g. `UnitTests/MachStudioTests`,
   starting from a copy of `CoreTests.vcxproj`) that links more libraries,
   with a ProjectReference to it from MainApp. `guiXMLTextReader::Convert`
   (Tool) is only covered through `envNumberText` until then.

## Not verified yet

- A local MainApp build in the Qt and Wx configurations (it has only been
  linked here in RenderNode; CI builds it in Qt and Wx). Qt 6.9.3 is
  installed on this machine.
- RenderNode rendering a scene from the command line. Only the no-argument
  start-up has been run: it prints "MachStudio Pro in batch mode", creates
  the D3D11 device, loads the shaders and exits with code 0.
- Visual Studio Test Explorer discovering the tests.
- Building in the Visual Studio IDE (only msbuild has been used).
- Loading real scene/material XML files saved by older versions. The
  chunk readers' fallback for Int8/UInt8 values the old writers stored as
  raw characters is only tested on generated files.

## Known issues

- `chWriter::Write(const itString&)` (`Core/Ch/private/chWriter.cpp`) asserts on
  an explicit trailing NUL, which itStrings no longer have, and indexes `[-1]`
  on an empty string. Callers use `Write(str.GetString())` instead.
- `Core/Dbg/dbgAssert.hpp`/`.cpp` are dead code (the header starts with
  `#error`, and neither file is in `Core.vcxproj`).
- Big-endian support (`ENV_ENDIAN` in `envPlatform.hpp`, the byte swapping
  in `gfFileUtil.hpp`, `gfFileBin::e_BigEndian`, the
  `gfFileUtil.EndianRoundTrip` test) can go: only little-endian platforms
  need supporting. `envPlatform.hpp` maps `ENV_MAC` to big-endian (a
  PowerPC-era leftover; Macs have been little-endian since 2006). Nothing
  selects `ENV_MAC` today, but a macOS port that did would byte-swap
  every binary file.
- The dbgMsg tests print `(1) ****ERROR**** : ...` lines (the error stream
  echoing to stdout), which appear in every build that runs the tests.
  Harmless, but noisy; the tests could redirect that stream.
- Files saved by pre-2015 MSVC runtimes may hold floats as `1.#INF` or
  `1.#QNAN`; these read as 1.
- CI warns that its actions (`checkout`, `cache`, `upload-artifact`,
  `setup-msbuild`) target the deprecated Node.js 20; they still run.

## Working notes

- Core headers use `#error` "multiply included" guards, so test files must
  guard their includes (`#ifndef MA_PLANE_HPP` ...).
- Most source and project files use CRLF line endings (`MachStudio.sln`,
  `.vcxproj`, most `.cpp`/`.hpp`). Git Bash's `sed -i` silently converts a
  whole file to LF, so edit them with the editor or a script that keeps
  CRLF.
- The projects use the v145 toolset: build with the Visual Studio 18
  MSBuild (`C:\Program Files\Microsoft Visual Studio\18\...`), not 2022's.
  VS 18 on this machine has no MFC component (nothing needs it).
- From Git Bash, msbuild's `/p:` and `/t:` arguments get mangled into paths;
  run msbuild from PowerShell. The CI shell is PowerShell too, so an
  msbuild `/t:A;B` argument must be quoted.
- Scripts in Git Bash heredocs lose backslashes (`\\` becomes `\`); write
  Python that handles Windows paths to a file first.
- vcpkg's x64-windows gtest is a DLL (`GTEST_LINKED_AS_SHARED_LIBRARY=1`), and
  the exe resolves gtest from `gtest.dll`, which is copied next to it.
- `maRotation`'s `a * b` applies `b` first; `maMatrix4x4`'s `A * B` applies `A`
  first (row vectors). The tests pin both down.
- `gfFileXML::WriteHeader` also writes an `<SGPU>` root tag, which readers
  must read as a chunk (as `docDocumentWithChunks` does) before the
  document's own chunks.
