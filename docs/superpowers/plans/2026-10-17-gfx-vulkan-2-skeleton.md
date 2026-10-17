# GraphicsVulkan skeleton and backend switch — Implementation Plan (Phase 1, Plan 2)

> **Status: STUB.** Written in detail only after Plan 1 is finished and reviewed by the owner. This file records scope, exit criteria and open questions so the sequence is visible. Execute with superpowers:executing-plans, inline (no subagents); stop for review when done.

**Goal:** `-gfx vulkan` starts MachStudio on a new GraphicsVulkan library that creates the device and a swap chain per window and clears them, validation-clean, in all six configurations, with the descriptor model chosen by prototype.

**Spec:** `docs/superpowers/specs/2026-10-17-graphics-vulkan-design.md` — "Library layout", "Modern Vulkan", "Backend switch", "Validation", "Build and dependencies". Roadmap: `2026-10-17-graphics-vulkan-roadmap.md`.

**Depends on (Plan 1):** `mainCommandLine` / `g3dGfxOptions` (gains backend and validation fields here), `g3dProfiler` + `g3dProfilerGpu` (Vulkan implementation added here), `gfxcompare` (gains the validation-count gate here), GraphicsTests.

## Scope

- `LibXLT/GraphicsVulkan/GraphicsVulkan.vcxproj`: static library in `MachStudio.sln`, all six configurations, referenced by MainApp; vcpkg manifest (`vulkan-headers`, `volk`, `vulkan-memory-allocator`, `spirv-reflect`, `directx-dxc`), or the Vulkan SDK 1.4.350 if packages lag (header must know `VK_EXT_descriptor_heap`).
- `gvk` namespace for all copied/ported code; `GVK_` include-guard prefix; `...DX11` classes become `...Vk`.
- `GraphicsVulkanLayer` with the same five functions as `GraphicsDX11Layer`; `g2dSystemVk` / `g3dSystemVk` implementing `g2dSystem` / `g3dSystem` (windows only at this stage; other `SetImplementation` hooks come in later plans).
- `vkd` package:
  - instance (volk), debug utils messenger, physical device chosen by matching the DXGI adapter LUID used by DX11 (log the choice; must be the Quadro on the dev machine);
  - Vulkan 1.4 baseline and required-feature check that names each missing feature/extension and exits (no silent fallback);
  - logical device and queues; VMA with memory budget; deferred destruction; frames in flight with timeline semaphores; command buffer per frame;
  - surface + swap chain per `g2dWindow` (Qt/Wx render panels, hidden RenderNode window), resize handling;
  - struct helpers (sType/pNext defaults, create infos, barriers, rendering info) and `VK_CHECK` reporting through `dbg`; object naming and per-pass labels helpers;
  - unified image layouts, dynamic rendering, synchronization2 used from the start.
- Backend selection: `g3dGfxOptions` gains `e_DX11 / e_Vulkan` and `m_bValidation`; `mainCommandLine` parses `-gfx dx11|vulkan` (default `dx11`) and `-gfxValidation`; `mainInitGraphics` chooses the layer; active backend in the log and window title/status bar; profiler started with backend name `"vulkan"`.
- Validation policy (spec "Validation"): layer required in Debug with sync validation; errors → `DBG_ASSERT`; warning/error counts written at exit (log, stderr in RenderNode); `gfxcompare` fails a Vulkan run with any.
- `g3dProfilerGpuVk`: timestamp query pools, calibrated timestamps.
- **Descriptor-model prototype:** a minimal draw binding b/t/s/u slots like the DX11 shaders, implemented with `VK_EXT_descriptor_heap` (mapping set/binding to heap offsets, no shader change); fall back to descriptor buffer, then push descriptors. Check with core + sync + GPU-assisted validation, RenderDoc/Nsight. Record the choice and reason in `vkd` and in this plan's completion notes.

## Exit criteria

- Window clears (a fixed colour) under `-gfx vulkan` in Debug/Release Qt and Wx; RenderNode starts and exits cleanly.
- `-gfx dx11` and no flag behave exactly as before.
- On a missing feature (simulated) or forcing the Intel GPU, `-gfx vulkan` exits with a message naming the reason.
- Zero validation errors/warnings (core + sync); descriptor prototype also clean under GPU-assisted validation.
- New tests: `mainCommandLine` parses `-gfx`/`-gfxValidation`; GraphicsTests skip Vulkan cleanly with no device.

## Open questions (resolve when detailing)

- How GraphicsTests creates an offscreen device and window for both backends (hidden HWND vs. headless) — needed by Plan 3.
- Whether `vkd` exposes C-style free functions or small classes for the helpers.
- CI: install the Vulkan SDK on the runner (headers/DXC only) or rely on vcpkg.
