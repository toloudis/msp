# MachStudio Pro

MachStudio Pro is a Windows real-time 3D lighting, rendering and
cinematography application. Scenes exported from DCC tools (Maya, Max, FBX,
glTF) are lit, animated on a timeline and rendered on the GPU (Vulkan 1.4,
or Direct3D 11 with `-gfx dx11`). The code is in two trees: `LibXLT/`, the
shared engine and framework libraries, and `MachStudio/`, the application.
[CLAUDE.md](CLAUDE.md) describes the code in detail.

## Building

### What you need

- **Visual Studio 18 (2026)** with the "Desktop development with C++"
  workload (the projects use the v145 toolset, x64 only).
- **vcpkg**, for the third-party packages listed in [vcpkg.json](vcpkg.json).
  Either Visual Studio's bundled copy or a clone of
  <https://github.com/microsoft/vcpkg> (run `bootstrap-vcpkg.bat` in it).
  Make it visible to MSBuild once, from that copy:

  ```
  vcpkg integrate install
  ```

- **Qt 6.9.3** (msvc2022_64), only for the Qt configurations. It is found
  through the `QTDIR` environment variable, else at
  `C:\Qt\6.9.3\msvc2022_64`.

wxWidgets, libpng and zlib are checked in under `3rdParty/`.

### Install the vcpkg packages

From the repository root:

```
vcpkg install
```

This installs every package the solution uses into `vcpkg_installed/` at
the root, once, for every project and configuration (Debug and Release
libraries alike). Run it again after `vcpkg.json` or
`vcpkg-configuration.json` changes. A build does the same step itself if
anything is missing, so skipping it only moves the wait into the first
project that compiles. Packages vcpkg has built before come from its binary
cache instead of being rebuilt. Use the same vcpkg you ran
`vcpkg integrate install` from.

### Build

Open `MachStudio/Source/MachStudio.sln` and pick a configuration:

| Configuration | Builds |
|---|---|
| `Debug Qt` / `Release Qt` | the application with the Qt 6 interface |
| `Debug Wx` / `Release Wx` | the application with the wxWidgets interface |
| `Debug RenderNode` / `Release RenderNode` | the headless command-line renderer |

Or from a PowerShell prompt:

```
msbuild MachStudio/Source/MachStudio.sln "/p:Configuration=Release Qt" /p:Platform=x64 /m
```

The executables land in `MachStudio/MachStudio-x64/` (for example
`MachStudio-x64-Qt-Release.exe`), next to the DLLs and data they need.

The unit tests (`UnitTests/`) build with the solution; CoreTests and
MainAppTests run as part of the build, and a failing test fails it.
