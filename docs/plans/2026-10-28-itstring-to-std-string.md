# Replacing itString with UTF-8 std::string

Plan for removing `Core/It/itString` in favor of `std::string` holding
UTF-8, and then making `fsLocator` hold a `std::filesystem::path`. The aim
is to move MachStudio toward building on macOS and Linux.

## Why std::string and not std::wstring

- `wchar_t` is 2 bytes on Windows and 4 bytes on macOS and Linux.
  `itString` is already `std::basic_string<wchar_t>`, and the binary
  chunk writer stops the build if `wchar_t` isn't 2 bytes
  (`chBinWriter.cpp`: `#error "UTF16 file format needs to be converted to
  wchar_t"`). `std::wstring` would keep that problem.
- UTF-8 `std::string` is the usual choice for code that runs on all three
  platforms, and the codebase already leans that way: about 11,200
  `std::string` uses against about 400 `std::wstring`.
- `std::u16string` would match the file format, but nothing else
  (Qt aside) speaks it, so every use would still need converting.

## What itString is today

- It's a thin wrapper over `std::basic_string<wchar_t>`. Most of its
  methods are renamed `std::string` calls (`GetLength`, `Clear`,
  `HasSubString`, `StartsWith`, `GetExtension`, ...). The only real
  logic is converting from `char*` as UTF-8 and writing to an `ostream`
  as UTF-8 (`envString::UTF8ToWideChar` / `WideCharToUTF8`).
- It's used about 3,000 times in roughly 750 files across LibXLT and
  MachStudio, plus about 110 times in UnitTests and 420 in Tools (mostly
  legacy, not built). The heaviest users are Support (533), Systems
  (455), Features (277) and Core/Fs (232).
- A lot of the code just converts back and forth: about 229
  `itString(x.c_str())` and 226 `itStringUtil::GetStdString(...)` calls.

## The parts that need design

1. **File format: the `ch` readers and writers move to UTF-8.** Today
   binary chunks store strings as null-terminated 16-bit UTF-16 code
   units (`chBinReader::Read(itString&)`, `chBinWriter::Write(const
   envType::WChar*)`), and the text and XML writers truncate each
   character to one byte (`chTxtWriter::Write` and `chXMLWriter::Write`
   assert in Debug that the text is ASCII and corrupt it in Release).
   After this work, every `ch` reader and writer takes and gives UTF-8
   `std::string`, and every new file stores UTF-8:
   - **Binary files** (every `gfFileBin` file: `.mab` documents,
     materials, models, animations, particles, project setup,
     expressions, ...) write strings as null-terminated UTF-8 bytes.
     Old files must still load, so a file says which encoding it uses
     (see "Marking binary files" below), and `chBinReader` reads UTF-16
     from old files and converts it to UTF-8. Reading UTF-16 is
     permanent: old scenes, and files written by the legacy exporters
     in `Tools/`, stay in that format.
   - **Text and XML files** write UTF-8 directly. They need no marker:
     old files could only hold ASCII (the writers asserted it), and
     ASCII is valid UTF-8.
   - **`chMemWriter`** writes the same bytes as `chBinWriter` (now
     UTF-8). `iscnRefLedger` compares an object's saved bytes against a
     snapshot, but both are made in the same session by `chMemWriter`,
     and the `SRCR` chunk saves only states and file stamps, not bytes,
     so the change doesn't turn references into copies.
   - Chunk sizes are recorded in the chunk headers when written, so
     strings changing length on disk needs nothing else.
   - The explicit-length forms (`Read(itString&, int i_Length)`,
     `Write(const WChar*, int i_Length)`) count UTF-16 code units today;
     in UTF-8 they count bytes. Check their callers
     (`chChunkParserUtil::Write(chWriter&, const itString&)` writes the
     name with its null this way).

   **Marking binary files.** Every `gfFileBin` file starts with the
   signature `TWFB` (or the older `GWFB`) and a 4-byte header
   (`gfFileBin::Header`: `m_Mode`, `m_Major`, `m_Minor`, `m_Rev`).
   `m_Mode` is a deprecated endian byte that `WriteHeader` always sets
   to 2, and the version bytes are used differently by each file type
   (documents write 0.0.0; particles keep their own version in
   `m_Minor`), so neither is free to mean "UTF-8". So the marker is a
   new signature, **`PWFB`** (petawatt, after `GWFB` gigawatt and `TWFB`
   terawatt File Binary). New files start with `PWFB`;
   `gfFileBin::ReadHeader` accepts all three signatures and remembers
   whether strings are UTF-8, and `chBinReader` asks the file. An older
   MachStudio build refuses a new file with `gfInvalidFileBinX` instead
   of loading it with garbled names. (Setting `m_Mode` to 3 was
   considered and dropped: older builds ignore `m_Mode`, so they would
   load a new file and misread every string in it.)

   `ReadHeader` is the one place the encoding is decided.
   Check that every place that makes a `chBinReader` has read the
   header first (the document, material, model, animation, particle and
   texture loaders call `ReadHeader`; `vtxVertexFileSeeker` needs
   checking), and treat a file whose header wasn't read as UTF-16.
2. **Windows calls.** About 15 Win32 `W` calls in `*PACWin*` files take
   `GetString()` directly (`CreateFileW`, `FindFirstFileW`,
   `MoveFileExW`, `SetWindowTextW`, ...). Each converts to UTF-16 at the
   call. Keeping these conversions inside PAC files is also what a
   macOS or Linux port needs.
3. **GUI.** About 65 Qt wide-string conversions (`fromWCharArray`,
   `toStdWString`, `fromStdWString`, `toWCharArray`) and about 27
   `wxString(...GetString())` calls. These become `QString::fromUtf8` /
   `toStdString` and `wxString::FromUTF8` / `ToStdString(wxConvUTF8)`.
4. **Narrow file opens are already broken for non-ASCII paths.** About
   127 `fopen`/`ifstream`/`ofstream` calls take `std::string` paths, and
   today those use the ANSI code page. Adding
   `<activeCodePage>UTF-8</activeCodePage>` to the manifest (Windows 10
   1903 and later) makes the narrow C runtime and Win32 `A` calls take
   UTF-8. That fixes an existing bug and is the first step of the
   migration.
5. **Index meaning changes.** `GetLength()` and `operator[]` count UTF-16
   code units today and would count bytes. That matters wherever code
   moves through text one character at a time: text entry in `Input/in`
   (114 uses), `InsertCharAt` / `RemoveCharAt`, `IsStringASCII`, and
   `itStringUtil::ToLower`. Lowercasing only ASCII is fine for file
   extensions and keywords; anything that edits text under a cursor
   needs to step by code point.
6. **Existing bugs.** `chTxtReader::Read(itString&)` copies the token
   byte by byte into the wide string, and the text and XML writers
   truncate each character to a byte, so non-ASCII text in text and XML
   chunks is already corrupted. Moving the readers and writers to UTF-8
   fixes both.

## Step 1: Groundwork

Nothing changes type yet; this makes the rest safe.

**Done (2026-10-28, not yet reviewed):**

- `/utf-8` in `Directory.Build.props`. No project replaced
  `AdditionalOptions` without `%(AdditionalOptions)`, so every project
  gets it.
- 43 source files in LibXLT and MachStudio were Windows-1252; they are
  now UTF-8 (same characters; line endings kept). Only comments, and
  string literals in `mnmConstants.hpp` (the `™`/`©` product strings, of
  which only the wide ones are used, and they come out the same) and
  `prjltProjectedLightObject.cpp` (a `´` in a UI label, which now
  decodes correctly).
- The UTF-8 code page: one manifest, `Utf8CodePage.manifest` at the repo
  root, merged by `Directory.Build.props` into every project whose
  `ConfigurationType` is `Application` (MachStudio and the four test
  exes), rather than added to each project. MainApp's
  `AdditionalManifestFiles` now separate with `;` (they used a space,
  which would have run the two file names together).
- `envString::UTF8ToUTF16` / `UTF16ToUTF8` (portable, `std::u16string`),
  tested in `CoreTests/Env/envStringTest.cpp`; the `std::wstring` ones
  already existed (`UTF8ToWideChar`, `WideCharToUTF8`, a PAC).
- `CoreTests/Ch/chStringTest.cpp` and the old-format files in
  `CoreTests/Data/` (`README.md` there); `CoreTests/Fs/fsNarrowPathTest.cpp`.
  The non-ASCII text and XML cases are skipped (`GTEST_SKIP`), not
  failing, because a failing test fails the build; step 2 removes the
  skip.
- Not done: saving existing scenes to compare against (needs the
  application, by hand).

- Add the `/utf-8` compiler flag (source and execution character sets
  both UTF-8, so string literals are UTF-8) in `Directory.Build.props`
  (repo root), as `/utf-8 %(AdditionalOptions)` in a `ClCompile` item
  definition, so every project gets it, including new ones. Visual
  Studio has no dedicated project property for it; it shows under
  C/C++ > Command Line.
  - No project sets `/source-charset` or `/execution-charset` today, so
    nothing conflicts.
  - A source file saved in a legacy code page (Windows-1252, say) with
    non-ASCII bytes in it is read as UTF-8 from now on, so its literals
    change. Before turning the flag on, find source files with bytes
    that aren't valid UTF-8 and convert them to UTF-8.
- Add `<activeCodePage>UTF-8</activeCodePage>` to MainApp's manifest
  (`MachStudio/Source/MainApp/DeclareDPIAware.manifest`, or a new one
  next to it) and to the test exes (CoreTests, MainAppTests,
  ImportExportTests, GraphicsTests), so narrow file APIs take UTF-8.
- Add the conversions `envString` lacks: UTF-8 to and from UTF-16 as
  `std::u16string` (for the chunk format) and as `std::wstring` (for
  Win32 and the PACs). Test them in CoreTests (`Env/`), including
  invalid UTF-8 and surrogate pairs.
- CoreTests: chunk tests with strings that are ASCII, accented Latin,
  CJK and an emoji:
  - Save a few small files with today's `chBinWriter` (UTF-16) and
    check them in under `UnitTests/CoreTests/Data/` as old-format
    fixtures. A test reads them back through `chBinReader`; it must
    keep passing after step 2, which is what proves old files still
    load.
  - Round trips through `chBinWriter` / `chBinReader`, `chMemWriter`
    (its bytes equal `chBinWriter`'s), `chTxtWriter` / `chTxtReader`
    and the XML pair. The non-ASCII text and XML cases fail until
    step 2.
- Save a few existing scenes (including one with non-ASCII object names
  and paths) to compare against after step 2.
- A test that opens a file with a non-ASCII name through `fopen` and
  `std::ofstream` with a UTF-8 `std::string` path (passes once the
  manifest is in).

## Step 2: Core

Switch `LibXLT/Core` (`Ch`, `Fs`, `prty`, `App`, `Gf`, `Dbg`, `name`, and
the rest) from `itString` to `std::string`.

**Done (2026-10-28, not yet reviewed):**

- **Change to the design: `chReader::ReadItString`.** One `Read(std::string&)`
  can't serve both kinds of string in old files: narrow strings were
  always single bytes, `itString`s were UTF-16, and nothing in the file
  says which a field is. So `Read(std::string&)` stays narrow (bytes as
  written) and the new `ReadItString(std::string&)` reads what was an
  `itString`: UTF-16 converted to UTF-8 in GWFB/TWFB files, UTF-8 in PWFB
  files (the same as `Read` there); text and XML readers make it `Read`.
  `chChunkParserUtil::ReadItString` likewise. Writers have one
  `Write(const std::string&)`, UTF-8 everywhere. **Step 3 must change
  every read of a field that was an `itString` to `ReadItString`** (a plain
  `Read` compiles and silently misreads old files).
- `gfFileBin` writes `PWFB`, `ReadHeader` accepts all three and sets
  `HasUTF8Strings()`; a file whose header wasn't read counts as UTF-16.
  Every reader in the solution calls `ReadHeader` (`vtxVertexFileSeeker`
  doesn't, but reads only frame data, no strings); production never reads
  a headerless binary file or parses `chMemWriter` bytes, so the rule only
  matters to tests, which now write a header when they read text strings
  back. `mtrMaterialSaver::Write` (copies raw chunks of an old file into a
  new one, which could mix UTF-16 and UTF-8) has no live caller.
- The `WChar` writer overloads are gone, and with them the `#error` for a
  4-byte `wchar_t` (step 4 item). The UTF-16 read stops at the chunk end.
- The two old bugs are fixed: `chChunkParserUtil::Write(itString)` writes
  null-terminated, and `gfFileTxt` writes an empty token as `""`.
- Temporary shims (removed in step 4): `itString` ⇄ `std::string` implicit
  conversions; `chReader::Read(itString&)`, `chWriter::Write(itString)`,
  `chChunkParserUtil` `itString` overloads; `fsLocator` constructors,
  `Push` and `Remove` taking `itString` (they also have `fsLocator`
  overloads, so an `itString` would be ambiguous);
  `fsFileUtil::LocatorToUnicodeString(…, itString&)`; `itStringUtil`
  `std::string` overloads plus `StripExtension`, `ReplaceExtension`,
  `GetExtension`, `GetBase`, `StartsWith`, `EndsWith`; free `==`/`!=`
  between `itString` and `std::string` (so the many comparisons of an
  `fsLocator` name with an `itString` keep compiling).
- Outside Core, only what stopped compiling was changed: calls on an
  `fsLocator` name that is now a `std::string` (`GetExtension`,
  `HasSubString`, `GetString`) in Graphics, Tool, AudioDS, ToolUIQt/Wx
  file pickers and MachStudio (sound and video drivers, paint brush,
  material browser, scene setup, tree views), `vector<itString>`
  substring lists, `std::wstring` file names for the DirectSound and
  DirectShow calls, the pqt/pwx `prtyFileName` converters (now
  `std::string`), and `dbgMsg` includes that `itString.hpp` used to bring
  in. Qt and wx code were changed together. Debug Qt, Wx and RenderNode
  build, and CoreTests, MainAppTests and ImportExportTests pass; Release
  hasn't been built.
- `fsLocator` names, `fsFileUtil` (Unicode and ANSI conversions merged:
  the Unicode ones call the ANSI ones), `fsFileEnum`/`gfFileEnum`
  substrings, `fsFileTimeUtil` strings, resource tracker and finders
  (Graphics' `mtrTextureFinder` overrides follow), `prtyFileName`,
  file-chooser filters, `appApplication` titles, `nameString` are
  `std::string`. `appCharEvent` keeps a `envType::WChar` (step 3 decides
  about code points). Win32 PACs convert with `envString` at each `W` call
  (`fsFileUtilPAC::LocatorToUnicodeFilename` gives `std::wstring`).
- Found by the tests: `fsFileStream::SetFilePos` (Windows) made a relative
  move that left the read buffer from the OS file pointer, which is ahead
  by the read-ahead, so `gfFileTxt::ReadToken`'s two-byte back-up landed
  wrongly when it crossed a 512-byte buffer refill. Fixed; test
  `fsFileStream.MoveBackAcrossTheReadBuffer`.
- Step 3 hazard: `fsFileEnum::EnumerateFiles(loc, target, ".ext")` with a
  plain literal picks the `bool` overload; keep a `std::string`.
- Not done: existing scenes saved and compared by hand.

- `chReader` / `chWriter`: `Read(std::string&)` / `Write(const
  std::string&)` in UTF-8. Every writer writes UTF-8; `gfFileBin`
  writes the signature `PWFB` (see "Marking binary files");
  `chBinReader` reads UTF-8 or, for old files, UTF-16 converted to
  UTF-8, as `ReadHeader` found. Drop the `envType::WChar*` overloads
  once nothing calls them.
  - Comment the signatures where `gfFileBin` defines and checks them
    (`gfFileBin.hpp`'s header comment, which still names only `TWFB`,
    and `ReadHeader`/`WriteHeader` in `gfFileBin.cpp`): `PWFB` files
    store strings as UTF-8; `GWFB` and `TWFB` files store wide strings
    as UTF-16LE.
- Two bugs from before this work, found by the step 1 tests:
  - `chChunkParserUtil::Write(chWriter&, const itString&)` writes the
    string with an explicit length that includes its NUL. Binary files
    don't notice (the reader stops at the NUL), but the text and XML
    writers store the NUL as part of the token, and it reads back as a
    trailing `\0`. With `std::string` the length form should go: write
    null-terminated.
  - The text and XML writers write an empty string as nothing at all
    (`gfFileTxt::WriteToken` quotes only strings with spaces, quotes or
    backslashes), so the next token is read in its place. Write `""`.
  Add the empty string and the `chChunkParserUtil::Write` path to the
  text and XML cases in `chStringTest.cpp` when fixing them.
- Tests: the old-format fixtures from step 1 still load, the round trips
  pass for every string (text and XML included; remove the skip in
  `chStringTest.cpp`), and new binary files
  start with `PWFB`. Existing scenes load and come out the same
  when saved again (same objects, names and paths; the bytes differ
  now, since strings are UTF-8).
- PACs: each `*PACWin*` call that took `GetString()` converts with the
  step 1 helper at the call.
- `fsLocator` names and the `fsFileUtil` conversions
  (`UnicodeStringToLocator`, `LocatorToUnicodeString`,
  `ANSIFilenameToLocator`, `LocatorToANSIFilename`) take and give
  `std::string`; the "Unicode" and "ANSI" pairs become the same thing
  and can be merged (step 5 replaces them anyway).
- `prty` file-name, file-path, texture and video properties hold
  `std::string`.
- So the rest of the solution keeps building, `itString` temporarily
  gets an implicit conversion to and from `std::string` (UTF-8), and
  `itStringUtil` gets `std::string` overloads. These are removed in
  step 4.
- Build, and run CoreTests, MainAppTests and ImportExportTests.

## Step 3: One library at a time

In dependency order: Graphics, GraphicsDX11 and GraphicsVulkan,
ImportExport, Input, AudioDS, Tool, ToolUIQt and ToolUIWx, then
MachStudio's Support, Systems, Features, Drivers and MainApp.

**Done (2026-10-29, not yet reviewed):**

- **Reads first.** Before any type changed, the temporary
  `chReader::Read(itString&)` and `chChunkParserUtil::Read(chReader&,
  itString&)` were renamed `ReadItString`, so the compiler listed every
  read of an `itString` field (27, in Graphics, Drivers, MainApp,
  Support, Systems and one test); each became `ReadItString`, which keeps
  working when the field turns into a `std::string`. A plain `Read` of a
  field that was an `itString` can't remain: it wouldn't have compiled.
- No `itString` is left in LibXLT outside Core, in MachStudio, or in
  UnitTests outside CoreTests (`itStringTest.cpp`, and `chStringTest.cpp`,
  which tests the temporary overloads). Not touched: code no project builds
  (Area18, Record, the Systems folders listed as not built, the WinForms
  `*Form.h` leftovers).
- `envString` gained `StartsWith`, `EndsWith` (an empty substring gives
  false, as `itString`'s did) and `IsASCII`. The file-name members
  (`GetExtension`, `StripExtension`, `ReplaceExtension`, `GetBase`) became
  calls to the `itStringUtil` `std::string` helpers from step 2, left for
  step 4 to place.
- Overloads that became duplicates were removed (`g2dFontUtil` /
  `GetLoadedFontName`, `guiXMLTextReader::Convert`, the `WriteElement`s
  of `guiXMLTextWriter`, `guiXMLTextWriterStd`, `quiXMLTextWriter` and
  `wuiXMLTextWriter`, and the GraphicsTests font fakes).
- Windows calls convert at the call, all in `*Win*` files or Windows-only
  code: `SetWindowTextW` (DX11; Vulkan's `vkdSetWindowTitle` now takes
  UTF-8), D3DCompile, DirectXTex loads, `_wfopen`, DirectWrite's font
  family name, the DXGI adapter name, the RenderNode command line (back
  to a `std::wstring`; it was an `itString` of `TCHAR`s). File streams
  and `std::filesystem` paths take `std::filesystem::u8path(...)` (OpenEXR,
  image saving, image decoding, the pipeline cache, RenderMan RIB files).
- Qt: `fromWCharArray(x.GetString())` → `QString::fromStdString(x)`,
  `toStdWString()` → `toStdString()` (both UTF-8 in Qt 6). wx:
  `wxString::FromUTF8(x)` where a wide string went to wx, and
  `utf8_string()` back (also `pwxDirectory_DirPicker`, which cast a
  `wxString` to `const char*`, and `wxMainDropTarget`).
- `mnmConstants`: `c_PRODUCT_FOR_DISPLAY` is a UTF-8 `const char*`; the
  wide `cw_*` constants became narrow `c_RIGHTS`, `c_64BIT`, `c_32BIT`
  (`cw_COPYRIGHT` and `cw_PRODUCT` duplicated `c_COPYRIGHT` and
  `c_PRODUCT` and are gone).
- Stepping through characters: `scrText` (the bitmap-font text both
  backends draw) still makes one glyph per UTF-16 unit, decoding the
  UTF-8 first, as before. `Input/in`'s 114 uses were the key-name table,
  not text entry. The other loops (render output tags, Maya export names,
  animated file-name patterns, file extension lists) look for ASCII
  characters, which UTF-8 never has inside a multi-byte character, so
  bytes are fine; `InsertCharAt` / `RemoveCharAt` became `insert` /
  `erase` / `pop_back`.
- Old bugs found and fixed:
  - `prtGeneratorUtil` wrote particle texture and geometry names with
    `Write(name.GetString())`, a `wchar_t*`, which since step 2 picked
    `Write(bool)`. Now the names are written.
  - The particle (`prtGeneratorUtil`) and version-1 sound driver
    (`tmlnDriverSoundParser`) readers dropped the last character of the
    name they read, to remove a NULL the reader stopped returning long
    ago (the particle one asserted first). They keep the whole name now.
  - `g2dFontUtilDX11::GetLoadedFontName` gave DirectWrite a buffer one
    short (no room for the NULL).
  - Narrow `std::ofstream`s on wide names went through the C runtime's
    wide-name extension; `g2dScanlineOpenEXRUtil` cut every character of
    the EXR file name to a byte.
- `fsysFileList::GetFilePath(0, ...)` became ambiguous (0 also converts to
  a `std::string`); `envtDefaultEnvironment` calls it through a const
  reference.
- Built: Debug and Release Qt, Wx and RenderNode; CoreTests, MainAppTests
  and ImportExportTests pass in each. No new warnings. GraphicsTests (Debug), the font,
  window, image decode, texture manager, image save, capture and glTF
  tests: 51 pass.

- A rename script does most of each library: `GetLength()` to `size()`,
  `GetString()` to `c_str()`, `Clear()` to `clear()`, `HasSubString(x)`
  to `find(x) != npos`, `StartsWith`/`EndsWith` to helpers in
  `envString` (C++17 has no `starts_with`), `GetStdString(x)` to `x`,
  `itString(x.c_str())` to `x`. Review the diff by hand.
- When `GetString()` starts returning `const char*` instead of
  `const wchar_t*`, the compiler flags every remaining wide-string use.
  Each is either a Windows call (move it behind a PAC, or convert at the
  call if it is already in a `*Win*` file) or a GUI call (Qt and wx, as
  above).
- Review the places that step through characters (`Input/in` text entry,
  `InsertCharAt`, `RemoveCharAt`, cursor positions in edit controls) by
  hand.
- Build every configuration (Qt, Wx, RenderNode, Debug and Release)
  after each library, since `qtGUI/` and `wxGUI/` code is compiled only
  in its own build. Run the unit tests, and the GraphicsTests filters
  for the libraries touched.

## Step 4: Clean up

- Delete `itString.hpp` / `itString.cpp` and the temporary conversions
  from step 2.
- Decided (2026-10-29): the helpers go into `envString`, and
  `itStringUtil` is deleted with `itString`.
- Port the `itStringUtil` helpers that are still used (`CompareNoCase`,
  `StartsWithNoCase`, `TrimSpaces`, `ToLower`, `Breakup_Filename`,
  `Build_Filename`, `GetInt`, `GetDouble`) to `std::string`, in
  `envString` or a renamed `itStringUtil`; delete the rest.
- Remove the `#error` for 4-byte `wchar_t` in `chBinWriter.cpp` (the
  writer no longer writes UTF-16) and the `envType::WChar` typedef if
  nothing uses it. The UTF-16 path in `chBinReader` stays, for old
  files; it reads 16-bit units, never `wchar_t`, so it works on every
  platform.
- `itLocales` / `itLocaleUtil` (locale lookup, a PAC) stay; whether they
  are still needed is a separate question.
- Update CLAUDE.md (the `It` row in the Core table, and the include-guard
  example if it mentions `itString`).

## Step 5: fsLocator on std::filesystem::path

`fsLocator` is used about 4,350 times in about 1,100 files, so keep the
class and change what it holds, rather than replacing it at every use.

- Today it holds `std::vector<itString> m_Names`, an optional directory
  index (`m_Index`, a `gfPaths` category, meaning the names are relative
  to that directory) and a UNC flag (`m_bUNCPath`).
- New storage: one `std::filesystem::path` plus the index. The UNC flag
  goes: `path` recognizes `\\server\share` itself (keep `IsUNCPath()` as
  a query on the path's root name while callers need it).
- The C++ standard is C++17 (`stdcpp17` in every project). In C++17,
  `std::filesystem::u8path(std::string)` builds a path from UTF-8 and
  `path::u8string()` returns UTF-8 as `std::string`. Wrap both in
  `fsLocator` (for example `FromUtf8` / `ToUtf8`), because C++20
  deprecates `u8path` and changes `u8string()` to return
  `std::u8string`; only that wrapper changes when the standard moves.
  Never use `path::string()` (on Windows it converts through the ANSI
  code page).
- Map the name-list API onto the path: `GetNumNames` / `GetName(i)` iterate
  the path's elements, `GetLastName` is `filename()`,
  `ReplaceExtension` is `replace_extension`, `Push` is `/=`, `Pop` is
  `parent_path()`, `operator +` is `/`. Keep these wrappers while there
  are callers, and add `GetPath()` for new code.
- `RemoveAfter`, `RemoveBefore`, `Remove(subdir)`, `FindFirst` and
  `ReplaceName(i)` are name-list editing with no direct `path`
  equivalent; implement them over the path's elements and look at their
  callers, since many may be simpler as `lexically_relative` or
  `parent_path` calls.
- Comparisons: today they compare the name lists (and so are
  case-sensitive and separator-neutral). Keep that behavior explicitly
  (compare element by element), not `path::operator==`, so `std::map`
  and `std::set` keys don't change order. Decide separately whether
  comparisons should ignore case on Windows.
- `fsFileUtil`'s `LocatorTo*Filename` / `*ToLocator` functions become
  `ToUtf8` / `FromUtf8`; `i_bUseDoubleSlashes` callers (scripts and
  RenderMan export) get their own escaping helper.
- The `*PACWin*` file functions take `path.c_str()` (a `wchar_t*` on
  Windows) directly, with no conversion. Portable code can use
  `std::filesystem` (`exists`, `create_directories`, `copy`, `rename`,
  `remove_all`) for `fsFileUtil`'s `FileExists`, `CreateDirectory`,
  `CopyDirectory` and so on, which shrinks the PAC to what has no
  standard equivalent (file change notification, AVI).
- Paths saved in scene files: callers write locators as strings
  (`LocatorToUnicodeString` and friends). Keep those strings the same
  (same separators, relative paths stay relative to their `gfPaths`
  index) so scenes load unchanged; check with the step 1 round-trip
  tests and by loading existing scenes.
- Tests in CoreTests (`Fs/`): construction from UTF-8 (including
  non-ASCII and UNC), the name-list operations, comparison order,
  extension replacement, and the string form of indexed and absolute
  locators matching what the current class produces.
