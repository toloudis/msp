# GraphicsVulkan skeleton and backend switch — Implementation Plan (Phase 1, Plan 2)

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task, inline in the session (the owner asked for no subagents). Steps use checkbox (`- [ ]`) syntax for tracking. When the plan is done, stop for the owner's review before Plan 3 is written.

**Goal:** `-gfx vulkan` starts MachStudio (Qt, Wx and RenderNode) on a new GraphicsVulkan library that creates the Vulkan device and a swap chain per window and clears them, with zero validation messages; DX11 stays the default and unchanged. Along the way, prove the descriptor model the port will use.

**Architecture:** A new static library `LibXLT/GraphicsVulkan` (everything in C++ namespace `gvk`) with a device package `vkd` (volk, VMA, device chosen by the DX11 adapter's LUID, an immediate-context-style command stream, frames in flight on a timeline semaphore, deferred destruction, a per-frame ring buffer, slot-based resource binding on `VK_EXT_descriptor_heap`, validation policy). `g2dSystemVk`/`g2dWindowVk`/`g3dSystemVk` implement the Graphics abstractions; every other `SetImplementation` hook gets a Vulkan class whose not-yet-ported methods are safe stubs marked `GVK_NOT_YET`, so the whole app runs and later plans fill them in. `mainInitGraphics` picks the layer from `-gfx`.

**Tech Stack:** C++17 (MSVC x64), Vulkan 1.4 C API via volk, Vulkan Memory Allocator, DXC (SPIR-V) for one test shader, GoogleTest; vcpkg baseline `a0b1c8d3a477c1cb4813d8e127a56961707ca42b` (vulkan-headers/volk/spirv-reflect 1.4.350.0, VMA 3.4.0, directx-dxc 2026-05-27 — matching the installed Vulkan SDK 1.4.350).

**Spec:** `docs/superpowers/specs/2026-10-17-graphics-vulkan-design.md` — "Library layout", "Modern Vulkan", "Backend switch", "Validation", "Build and dependencies". Roadmap: `docs/superpowers/plans/2026-10-17-graphics-vulkan-roadmap.md`. Plan 1 (done) supplies `mainCommandLine`, `g3dGfxOptions`, `g3dProfiler`/`g3dProfilerGpu`, GraphicsTests, `gfxcompare`.

## Global Constraints

- **No GraphicsDX11 changes** in this plan, and no shader source changes.
- `-gfx dx11` and no `-gfx` flag behave exactly as before (DX11 self-comparison from Plan 1 still passes).
- All GraphicsVulkan code is in namespace `gvk`; include guards start with `GVK_`; classes GraphicsDX11 names `...DX11` are named `...Vk` here. Packages and file layout follow CLAUDE.md (public headers in the package folder, `.cpp` in `private/`, includes rooted `"GraphicsVulkan/..."`, guarded includes).
- Vulkan C API only, loaded with volk; nothing links `vulkan-1.lib`. No Vulkan-Hpp.
- Windows APIs (HWND, DXGI) only in `*Win*` files.
- Baseline device: Vulkan 1.4 plus `VK_EXT_descriptor_heap` (or the fallback chosen in Task 4), `VK_KHR_unified_image_layouts`, `VK_KHR_calibrated_timestamps`, `VK_EXT_memory_budget`, geometry and tessellation shaders, dynamic rendering, synchronization2, timeline semaphores. A missing one is reported by name and MachStudio exits; never a silent fallback to DX11.
- **Validation (spec "Validation"):** Debug builds always enable `VK_LAYER_KHRONOS_validation` with synchronization validation; missing layer in Debug → report and exit. Release enables it with `-gfxValidation`. Errors → `DBG_ASSERT`; warnings counted; counts printed at exit (log, and stderr in RenderNode). Zero errors and zero warnings is the bar for every test and run in this plan. Suppression only for a confirmed layer false positive, with the message ID and reason in a comment.
- Swap chains match DX11: `VK_FORMAT_R8G8B8A8_UNORM` colour, `VK_FORMAT_D24_UNORM_S8_UINT` depth-stencil, no vsync (present mode `IMMEDIATE`, else `MAILBOX`, else `FIFO`, logged).
- The Vulkan device is the adapter DX11 would use: `IDXGIFactory6::EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE)`, matched by LUID (`VkPhysicalDeviceIDProperties::deviceLUID`).
- Git: branch `claude/graphics-vulkan`; stage explicit paths only; don't push. Commit messages end with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.

## Review Focus

1. **Silent wrong GPU or backend:** on this two-GPU machine, Vulkan must pick the Quadro, never the Intel UHD; if the LUID match fails it must say so and exit, not take device 0 (Task 2 test `NoLuidMatchIsAnError`, Task 3 log check).
2. **Window resize and minimize:** resizing a render panel, or minimizing the main window (0×0 surface), must not crash, leak, or produce validation errors; swap chains are recreated on `VK_ERROR_OUT_OF_DATE_KHR`/`VK_SUBOPTIMAL_KHR` and skipped while 0×0 (Task 5 test `ResizeAndZeroSizeAreClean`, Task 7 manual).
3. **Shutdown order:** closing MachStudio with windows open must wait for the GPU, destroy swap chains before surfaces before the device, and leave no validation "object not destroyed" messages (Task 3 test `DestroyIsValidationClean`, Task 7 manual exit check).
4. **Two threads touching the queue:** windows are created on the main thread and presented on the render thread; every queue submit/present goes through `vkdDevice`'s queue lock (Task 3 test `SubmitFromTwoThreads`).
5. **A stub that crashes instead of degrading:** with `-gfx vulkan -new`, every hook MachStudio calls at start-up returns a usable object; a stub never returns NULL where the DX11 version doesn't (Task 6 test `AllHooksInstalled` + Task 7 manual run with the log checked for `GVK_NOT_YET` lines).

---

## File Structure

| Path (under `LibXLT/GraphicsVulkan/`) | Responsibility |
|---|---|
| `GraphicsVulkan.vcxproj`, `.filters`, `vcpkg.json`, `vcpkg-configuration.json` | Static library, six configurations; vcpkg packages at the baseline above |
| `GraphicsVulkanLayer.hpp/.cpp` | `Init(void* hWnd)`, `CleanUp()`, `InitGraphics()`, `CleanUpGraphics()`, `GetSystem2D()`, `GetSystem3D()`, plus `GetInitError()` |
| `vkd/vkdCheck.hpp` | `VK_CHECK(expr)`, `vkdResultName(VkResult)`, `GVK_NOT_YET(name)` |
| `vkd/vkdInfo.hpp` | Struct helpers (`vkdImageCreateInfo2D(...)`, `vkdImageBarrier(...)`, `vkdRenderingInfo(...)`, ...) |
| `vkd/vkdAdapter.hpp`, `private/vkdAdapter.cpp`, `private/vkdAdapterWin.cpp` | Pure device selection + feature check; DXGI LUID lookup (Win) |
| `vkd/vkdDevice.hpp`, `private/vkdDevice.cpp` | Instance, debug messenger, device, queue + lock, VMA, command stream, frames in flight, deferred destruction, naming |
| `vkd/vkdValidation.hpp`, `private/vkdValidation.cpp` | Debug-utils callback, error/warning counters, assert on error |
| `vkd/vkdRingBuffer.hpp`, `private/vkdRingBuffer.cpp` | Per-frame upload ring (host-visible, device-address) |
| `vkd/vkdBindings.hpp`, `private/vkdBindings.cpp` | DX-style slot binding (b/t/s/u) → descriptor heap + per-draw binding table; shader mapping info for pipelines |
| `vkd/vkdImage.hpp`, `private/vkdImage.cpp` | Image + view + VMA allocation; readback to CPU |
| `vkd/vkdSurfaceWin.cpp` (private) | `vkCreateWin32SurfaceKHR` |
| `G2d/g2dSystemVk.hpp`, `G2d/g2dWindowVk.hpp`, `G2d/g2dDepthStencilBufferVk.hpp` + `private/` | 2D system, window (surface + swap chain), depth buffer |
| `G3d/g3dSystemVk.hpp`, `G3d/g3dProfilerGpuVk.hpp` + `private/` | 3D system; GPU timer for `g3dProfiler` |
| `stub/` (`gvk` classes per hook, see Task 6) | Vulkan classes for the remaining hooks, methods marked `GVK_NOT_YET` until ported |
| `UnitTests/GraphicsTests/Vk/*` | Vulkan tests (`gtVulkan` fixture skips without a device) |
| `UnitTests/GraphicsTests/Vk/Shaders/gtBindingProto.hlsl` | Test shader for the descriptor prototype (DXC → SPIR-V header at build) |

`stub/` holds the hook classes only until each is ported; a later plan moves a class into its real package (`Mat/`, `shdw/`, ...) when it implements it.

---

### Task 1: `-gfx` and `-gfxValidation`

**Files:** Modify `LibXLT/Graphics/G3d/g3dGfxOptions.hpp`, `MachStudio/Source/MainApp/mainCommandLine.hpp/.cpp`; Test `UnitTests/MainAppTests/mainCommandLineTest.cpp`.

**Interfaces — Produces:**
```cpp
struct g3dGfxOptions
{
    enum Backend { e_DX11, e_Vulkan };
    Backend     m_Backend = e_DX11;   // -gfx dx11|vulkan
    bool        m_bValidation = false;// -gfxValidation (Release; Debug always validates)
    std::string m_ProfileFile;
};
```
`-gfx` takes `dx11` or `vulkan` (case-insensitive); any other value leaves `e_DX11` and adds a warning naming the bad value. Both flags count as parsed data. Both appear in `HelpText()` (Graphics section) and in the reference comment in `mainCommandLine.hpp`.

- [ ] **Step 1: Failing tests:** `GfxVulkan` (`-gfx vulkan -new` → `e_Vulkan`, actions `{New}`), `GfxDx11CaseInsensitive` (`-gfx DX11` → `e_DX11`), `GfxBadValueWarns` (`-gfx metal` → `e_DX11`, one warning containing `metal`), `GfxValidation` (`-gfxValidation` → true), `GfxFirstMeansNotAPath` (`-gfx vulkan C:\x.mab` → no `e_OpenPath`; the path is an ignored token with a warning), and extend `HelpListsEveryFlag` with `-gfx` and `-gfxValidation`.
- [ ] **Step 2: Build MainAppTests; they fail to compile** (`m_Backend` missing).
- [ ] **Step 3: Implement.**
- [ ] **Step 4: MainAppTests pass.**
- [ ] **Step 5: Commit** — "Add -gfx and -gfxValidation".

---

### Task 2: GraphicsVulkan project and device selection logic

**Files:** Create the project files, `GraphicsVulkanLayer.hpp/.cpp` (empty bodies for now), `vkd/vkdCheck.hpp`, `vkd/vkdAdapter.hpp`, `vkd/private/vkdAdapter.cpp`; Modify `MachStudio/Source/MachStudio.sln`, `MachStudio/Source/MainApp/MainApp.vcxproj` (ProjectReference + vcpkg lib path + DLL copy like the other library manifests), `UnitTests/GraphicsTests/GraphicsTests.vcxproj` (reference GraphicsVulkan); Test `UnitTests/GraphicsTests/Vk/vkdAdapterTest.cpp`.

**Interfaces — Produces:**
```cpp
namespace gvk {
struct vkdAdapterInfo                       // one VkPhysicalDevice, as plain data
{
    std::string m_Name;
    uint8_t     m_Luid[8];
    bool        m_bLuidValid;
    uint32_t    m_ApiVersion;
    std::vector<std::string> m_Extensions;  // supported device extensions
    std::vector<std::string> m_Features;    // supported feature names we check (see vkdRequiredFeatures)
};
// Index of the adapter whose LUID equals i_Luid, or -1 (with o_Error naming all adapters seen).
int vkdSelectAdapter(const std::vector<vkdAdapterInfo>& i_Adapters, const uint8_t i_Luid[8], std::string& o_Error);
// Names of what i_Adapter lacks: "Vulkan 1.4" if m_ApiVersion < 1.4, each missing extension, each missing feature.
std::vector<std::string> vkdMissingRequirements(const vkdAdapterInfo& i_Adapter);
const std::vector<std::string>& vkdRequiredExtensions();   // exact list from Global Constraints (+ VK_KHR_swapchain)
const std::vector<std::string>& vkdRequiredFeatures();     // "geometryShader", "tessellationShader", "dynamicRendering", "synchronization2", "timelineSemaphore", "descriptorHeap", "unifiedImageLayouts", "bufferDeviceAddress"
}
```
`vkdCheck.hpp`: `VK_CHECK(expr)` — on failure `DBG_ASSERT` with the expression text and `vkdResultName(result)`, and evaluates to the `VkResult`; `GVK_NOT_YET("Class::Method")` — `DBG_LOG`s `GVK_NOT_YET: Class::Method` the first time each call site runs.

- [ ] **Step 1: Create the project.** Static library, six configurations, settings copied from `GraphicsDX11.vcxproj` (include roots, runtime, warning level, output dirs), no PCH; vcpkg manifest `{"name":"graphicsvulkan","dependencies":["vulkan-headers","volk","vulkan-memory-allocator","spirv-reflect","directx-dxc"]}` with `vcpkg-configuration.json` baseline `a0b1c8d3a477c1cb4813d8e127a56961707ca42b`; defines `VK_NO_PROTOTYPES` and `VK_USE_PLATFORM_WIN32_KHR` (the latter only consumed in `*Win*` files). Add to the solution (all six) and as a MainApp ProjectReference; MainApp links the GraphicsVulkan vcpkg lib dir and its `CopyLibraryVcpkgDlls` copies any GraphicsVulkan DLLs (dxcompiler is not needed at run time in this plan; copy nothing that isn't used).
- [ ] **Step 2: Failing tests** in `vkdAdapterTest.cpp` (pure data, no GPU): `SelectsByLuid` (Intel + Quadro, Quadro's LUID → 1), `NoLuidMatchIsAnError` (→ -1, error lists both names), `InvalidLuidIsSkipped` (`m_bLuidValid=false` never matches), `CompleteAdapterHasNothingMissing`, `Vulkan13IsMissing14` (`"Vulkan 1.4"` reported), `MissingExtensionNamed` (drop `VK_EXT_descriptor_heap` → named), `MissingFeatureNamed` (drop `tessellationShader` → named).
- [ ] **Step 3: Build GraphicsTests; fails** (no header).
- [ ] **Step 4: Implement `vkdAdapter.cpp`.**
- [ ] **Step 5: GraphicsTests pass; MainApp still builds in Debug Qt and Release RenderNode.**
- [ ] **Step 6: Commit** — "Add GraphicsVulkan project and device selection logic".

---

### Task 3: `vkdDevice` — instance, device, validation, command stream

**Files:** Create `vkd/vkdDevice.hpp`, `vkd/vkdValidation.hpp`, `vkd/vkdInfo.hpp`, `vkd/vkdImage.hpp`, `vkd/private/vkdDevice.cpp`, `vkdValidation.cpp`, `vkdImage.cpp`, `vkdAdapterWin.cpp`; Test `UnitTests/GraphicsTests/Vk/gtVulkan.hpp` (fixture), `Vk/vkdDeviceTest.cpp`.

**Interfaces — Consumes:** Task 2. **Produces:**
```cpp
namespace gvk {
struct vkdDeviceOptions
{
    bool m_bValidation;          // forced true in Debug
    bool m_bGpuAssisted;         // GPU-assisted validation (tests / sign-off only)
    std::string m_AdapterOverride; // from env MSP_GFX_VK_ADAPTER (name substring), Debug/testing only; empty = DX11's adapter
};
struct vkdValidationStats { int m_Errors; int m_Warnings; int m_Infos; };

bool vkdHighPerformanceLuid(uint8_t o_Luid[8]);   // vkdAdapterWin.cpp: DXGI EnumAdapterByGpuPreference(0, HIGH_PERFORMANCE)

class vkdDevice
{
public:
    static vkdDevice* Create(const vkdDeviceOptions& i_Options, std::string& o_Error); // NULL + reason on failure
    ~vkdDevice();                         // waits idle, runs all deferred destruction, destroys in reverse order

    VkInstance Instance() const; VkPhysicalDevice Physical() const; VkDevice Device() const;
    VkQueue Queue() const; uint32_t QueueFamily() const; VmaAllocator Allocator() const;
    const VkPhysicalDeviceProperties& Properties() const;
    const vkdAdapterInfo& Adapter() const;

    // Immediate-context-style command stream (DX11's immediate context semantics):
    VkCommandBuffer Commands();           // the open command buffer, opened on first use
    uint64_t Submit(const std::vector<VkSemaphoreSubmitInfo>& i_Wait = {},
                    const std::vector<VkSemaphoreSubmitInfo>& i_Signal = {}); // closes and submits it; returns its timeline value
    void WaitFor(uint64_t i_TimelineValue);
    void WaitIdle();
    uint64_t CompletedValue() const;

    void DeferDestroy(std::function<void()> i_Destroy); // runs once the GPU has finished all work submitted so far
    std::mutex& QueueLock();             // held around every vkQueueSubmit2 / vkQueuePresentKHR
    void SetName(VkObjectType i_Type, uint64_t i_Handle, const char* i_Name);
    void BeginLabel(VkCommandBuffer, const char* i_Name); void EndLabel(VkCommandBuffer);

    vkdValidationStats ValidationStats() const;
};
}
```
Rules: up to 3 command buffers in flight (`c_FramesInFlight = 3`); `Commands()` reuses the oldest slot after `WaitFor` its value; instance API version 1.4; debug-utils messenger always installed when validation is on; `vkdValidation` counts by severity, `DBG_ASSERT`s on errors with the message ID and text, logs warnings; synchronization validation enabled through `VkLayerSettingsCreateInfoEXT` (`validate_sync = true`; `gpuav_enable` when `m_bGpuAssisted`). `vkdImage`: 2D image + view on VMA (`Create2D(device, w, h, format, usage, name)`), `ReadBack(...)` → `std::vector<uint8_t>` via a staging buffer and `Submit`+`WaitFor`.

- [ ] **Step 1: Fixture `gtVulkan`** (GoogleTest fixture): creates a `vkdDevice` with validation (and the test name as object-name prefix) or calls `GTEST_SKIP()` with the reason when `Create` fails because no Vulkan device/driver/layer exists; in `TearDown` asserts `ValidationStats()` is all zero, then deletes the device.
- [ ] **Step 2: Failing tests** in `vkdDeviceTest.cpp`: `CreatesOnTheDx11Adapter` (device LUID == `vkdHighPerformanceLuid`; name contains "Quadro" on the dev machine — log only, not asserted), `ClearAndReadBack` (64×64 R8G8B8A8, clear to (0.25,0.5,0.75,1) with `vkCmdClearColorImage`, read back → every pixel (64,128,191,255) ±1), `ValidationErrorIsCaught` (swap in a counting handler with `dbgMsg::SetAssertHandler` for the test's duration so the expected assert isn't a failure; record a deliberately invalid call, e.g. clear an image created without `TRANSFER_DST` usage; expect exactly one assert through the test assert hook and `m_Errors == 1`, then reset the stats so TearDown passes), `DeferredDestroyRunsAfterGpu`, `SubmitFromTwoThreads` (two threads each submit 100 clears under `Commands()`/`Submit()` → no validation messages, all values complete), `DestroyIsValidationClean` (create/destroy device with images alive in deferred destruction → zero messages).
- [ ] **Step 3: Build; fail.** **Step 4: Implement.** **Step 5: Tests pass on the Quadro with zero validation messages** (Debug Qt) and **skip cleanly** when run with `MSP_GFX_VK_ADAPTER=nomatch` (simulated missing device → `Create` returns the reason).
- [ ] **Step 6: Commit** — "vkdDevice: instance, device on the DX11 adapter, validation, command stream".

---

### Task 4: Ring buffer and descriptor model prototype

**Files:** Create `vkd/vkdRingBuffer.hpp`, `vkd/vkdBindings.hpp`, `private/vkdRingBuffer.cpp`, `private/vkdBindings.cpp`; Test `Vk/vkdRingBufferTest.cpp`, `Vk/vkdBindingsTest.cpp`, `Vk/Shaders/gtBindingProto.hlsl` (+ GraphicsTests custom build step: DXC from the GraphicsVulkan vcpkg tools, `-spirv -fspv-target-env=vulkan1.3 -fvk-use-dx-layout -fvk-b-shift 0 0 -fvk-t-shift 100 0 -fvk-s-shift 200 0 -fvk-u-shift 300 0`, `-Fh` header into the intermediate dir).

**Interfaces — Produces:**
```cpp
namespace gvk {
class vkdRingBuffer                 // replaces Map(WRITE_DISCARD) for constants and dynamic vertices
{
public:
    vkdRingBuffer(vkdDevice& i_Device, VkDeviceSize i_SizePerFrame);   // c_FramesInFlight regions, host-visible, device-address
    struct Allocation { void* m_pCpu; VkDeviceAddress m_Gpu; VkBuffer m_Buffer; VkDeviceSize m_Offset; };
    Allocation Allocate(VkDeviceSize i_Size, VkDeviceSize i_Alignment = 256);  // grows (new block) instead of overwriting in-flight data
};

class vkdBindings                   // DX11-style slots, fed to shaders through the descriptor heap
{
public:
    explicit vkdBindings(vkdDevice& i_Device);
    uint32_t AddImageView(VkImageView i_View);   // heap index (resource heap); RemoveImageView frees it after the GPU is done
    uint32_t AddSampler(const VkSamplerCreateInfo& i_Info);   // sampler heap index (deduplicated by state)
    void SetConstantBuffer(uint32_t i_Slot, VkDeviceAddress i_Address, VkDeviceSize i_Size); // b0..b15
    void SetTexture(uint32_t i_Slot, uint32_t i_HeapIndex);   // t0..t63
    void SetSampler(uint32_t i_Slot, uint32_t i_HeapIndex);   // s0..s15
    void SetUav(uint32_t i_Slot, uint32_t i_HeapIndex);       // u0..u7
    void BindHeaps(VkCommandBuffer);                // vkCmdBindResourceHeapEXT / vkCmdBindSamplerHeapEXT (once per command buffer)
    void Apply(VkCommandBuffer, vkdRingBuffer&);    // writes the per-draw binding table to the ring, pushes its address (vkCmdPushDataEXT, 8 bytes)
    // Pipeline-side: the mapping for shaders compiled with the shifts above (set 0; b at 0, t at 100, s at 200, u at 300)
    const VkShaderDescriptorSetAndBindingMappingInfoEXT& ShaderMapping() const;
};
}
```
**Binding table layout** (per draw, 256-byte aligned in the ring): 16 × 8-byte constant-buffer addresses, then 64 × 4-byte texture heap indices, 16 × 4-byte sampler heap indices, 8 × 4-byte UAV heap indices. Shader mapping: constant buffers `VK_DESCRIPTOR_MAPPING_SOURCE_INDIRECT_ADDRESS_EXT` (push offset 0, address offset 8·slot), textures/UAVs `..._HEAP_WITH_INDIRECT_INDEX_EXT` (push offset 0, address offset into the index arrays), samplers through the sampler fields of the same mapping. Slot limits (16/64/16/8) cover every register the shaders use (b0–b5, t0–t38, s0–s13, u0–u7).

**Decision procedure (record the outcome in `vkdBindings.hpp`'s header comment and the commit message):** the heap/indirect design above is used if `vkdBindingsTest` passes on the Quadro with zero core, sync **and GPU-assisted** validation messages. If the extension or the validation layer can't support it cleanly, fall back to `VK_EXT_descriptor_buffer` with the same slot API, then to push descriptors (b/t/s/u in separate sets, ≤ 32 each); `vkdBindings`' public API stays the same in every case.

- [ ] **Step 1: Failing tests:** `vkdRingBufferTest`: `AllocationsAreAlignedAndDistinct`, `FrameRegionsDontOverlapInFlight` (allocate in 3 consecutive submitted frames without waiting → no overlap), `GrowsWhenFull`. `vkdBindingsTest` (`gtVulkan` fixture, GPU-assisted validation on): `gtBindingProto.hlsl` draws a full-screen triangle into a 16×16 target and outputs `tex0.Sample(s0, uv) * cb0.scale + tex1.Load(int3(0,0,0)) * cb1.bias` with `cb0` at b0, `cb1` at b3, `tex0` at t0, `tex1` at t37, `s0` at s13 — expected colour checked per pixel; `TwoDrawsDifferentBindings` (second draw rebinds t0 and b0, result differs as expected); `SamplerDedup` (same state → same index).
- [ ] **Step 2: Build; fail.** **Step 3: Implement** (minimal pipeline creation for the test lives in the test file; Plan 3 builds the real pipeline cache).
- [ ] **Step 4: Tests pass with zero validation messages including GPU-assisted.** If not, apply the decision procedure and re-run.
- [ ] **Step 5: Commit** — "vkd: ring buffer and slot binding on descriptor heaps (prototype verified)".

---

### Task 5: Windows, swap chains, 2D/3D systems, GPU timer

**Files:** Create `G2d/g2dSystemVk.hpp`, `G2d/g2dWindowVk.hpp`, `G2d/g2dDepthStencilBufferVk.hpp`, `G3d/g3dSystemVk.hpp`, `G3d/g3dProfilerGpuVk.hpp`, their `private/*.cpp`, `vkd/private/vkdSurfaceWin.cpp`; Test `Vk/g2dWindowVkTest.cpp`, `Vk/g3dProfilerGpuVkTest.cpp`, and a hidden-window helper in `UnitTests/GraphicsTests/gtPlatformWin.cpp` (`void* gtPlatform::CreateHiddenWindow(int w, int h)`, `DestroyHiddenWindow`, `ResizeHiddenWindow`).

**Interfaces — Produces:** `gvk::g2dSystemVk : public g2dSystem` (all `CreateAppWindow`/`CreateSubWindow` variants create a `g2dWindowVk` on the given HWND, or on a new hidden HWND for the size-only variants; `CreateFullScreen*` create a borderless window of that size — no exclusive full-screen; `ClearManagedResources` no-op), `gvk::g2dWindowVk : public g2dWindow` (surface + swap chain + D24S8 depth buffer; `Clear`/`ClearDepthStencil` record clears into `vkdDevice::Commands()`; `BeginScene`/`EndScene`/`MakeCurrent` track the current target for later plans; `Present` = acquire → copy/blit nothing in this plan (clears go straight to the acquired image) → `Submit` waiting on acquire and signalling render-done → `vkQueuePresentKHR`, all under `QueueLock()`; out-of-date/suboptimal → recreate; 0×0 → skip; `DrawText`/`DrawRect`/`DrawImage`/debug overlay → `GVK_NOT_YET`; frame-rate averages computed as in `g2dWindowDX11`), `gvk::g3dSystemVk : public g3dSystem` (`GetVideoAdapterName` = device name, `GetVideoMemory` = device-local heap budget in MB via `VK_EXT_memory_budget`), `gvk::g3dProfilerGpuVk : public g3dProfilerGpu` (timestamp query pool per frame slot, calibrated timestamps; same contract as `g3dProfilerGpuDX11`, compiled only with `MSP_GFX_PROFILING`).

Note: with dynamic rendering and unified image layouts, a clear of the acquired swap chain image needs only the acquire semaphore wait and memory barriers — no layout transitions except `PRESENT_SRC` handling as the extension specifies.

- [ ] **Step 1: Failing tests:** `ClearsAndPresents` (hidden 320×240 window: Clear(red) + Present ×5 → no errors; the last acquired image read back before present is red — add `g2dWindowVk::DebugReadBackNextFrame()` for this, Debug/test only), `ResizeAndZeroSizeAreClean` (resize to 640×480, to 0×0 — Present skips — and back; zero validation messages), `TwoWindowsInterleaved` (two windows, alternate presents), `GpuTimerMeasuresClear` (profiler frame with one scope around 16 clears of a 2048² image → `gpu_ms > 0`, valid).
- [ ] **Step 2: Build; fail.** **Step 3: Implement.** **Step 4: Pass, zero validation messages.**
- [ ] **Step 5: Commit** — "GraphicsVulkan: windows and swap chains, 2D/3D systems, GPU timer".

---

### Task 6: Every other hook, as Vulkan classes with safe stubs

**Files:** Create in `LibXLT/GraphicsVulkan/stub/` one header + `private/` .cpp per hook: `g2dResourceCounterVk`, `g2dFontUtilVk`, `g2dImageCreateVk` (+ `g2dImageVk`), `g2dImageSaveVk`, `g2dScreenCaptureUtilVk`, `g3dLightMgrVk`, `matTextureMgrVk` (+ `matTextureVk`), `matShaderMgrVk` (+ `matShaderEffectVk`), `g3dSceneRendererCreateVk` (+ `g3dSceneRendererVk`, `g3dPickRendererVk`), `g3dBakeVk`, `g3dFragmentCreateVk` (+ `g3dFragmentVk`), `mdlFragCreateVk`; installed by `g2dSystemVk`/`g3dSystemVk`/`GraphicsVulkanLayer` exactly where the DX11 equivalents install theirs (`g2dSystemDX11.cpp:403-412`, `g3dSystemDX11.cpp:203-214`, `shdwPackage.cpp:82,163`, `bumpPackage.cpp:48`, `mayPackage.cpp:49`), and uninstalled (`SetImplementation(NULL)`) in reverse order at clean-up. Test: `Vk/GraphicsVulkanLayerTest.cpp`.

**Stub rules:** every pure virtual is implemented; methods that create or look up an object return a real `gvk` object of the right type that holds whatever was passed in (names, sizes, CPU data) and answers queries from it (e.g. `g2dImageVk` keeps its pixels; `matTextureVk` its file name and size; `g3dFragmentVk` its bounding box from the vertex data passed in); renderers render nothing (the target was already cleared by `g3dTargetRenderer`); every method body that doesn't do the real work starts with `GVK_NOT_YET("Class::Method")`. Never return NULL where the DX11 version returns an object. Plans 3–7 replace stubs; Plan 7's exit includes "no `GVK_NOT_YET` left".

- [ ] **Step 1: Failing test** `AllHooksInstalled` (`gtVulkan` fixture): after `GraphicsVulkanLayer::Init(hiddenWnd)`, `GraphicsLayer::InitGraphics(GetSystem2D(), GetSystem3D())`, `GraphicsVulkanLayer::InitGraphics()`: for each hook, `X::SetImplementation(NULL)` returns non-NULL (then restore it); `g3dSceneRendererCreate::CreateRenderer(t)` returns non-NULL for every `g3dSceneRendererTypes` value; after `CleanUpGraphics()`/`CleanUp()` every hook is NULL again and validation is clean. Also `StubsAnswerFromTheirData` (create a 4×4 image through `g2dImageCreate`, read a pixel back; create a fragment from a 3-vertex buffer, bounding box matches).
- [ ] **Step 2: Build; fail.** **Step 3: Implement.** **Step 4: Pass.**
- [ ] **Step 5: Commit** — "GraphicsVulkan: Vulkan classes for every graphics hook (stubs marked GVK_NOT_YET)".

---

### Task 7: Backend switch in MachStudio

**Files:** Modify `MachStudio/Source/MainApp/mainInitGraphics.cpp/.hpp`, `MainApp.vcxproj`, `Tools/gfxcompare/rendernode.py`, `gfxcompare.py` (+ tests); maybe the window-title/status code that already shows the product name (find where `SetTitle`/status panels are set; keep the change in MainApp).

**Interfaces — Consumes:** Task 1 (`m_Backend`, `m_bValidation`), `GraphicsVulkanLayer` (Tasks 2–6). **Produces:** `mainInitGraphics` calls one backend through a small table (`Init`, `InitGraphics`, `CleanUpGraphics`, `CleanUp`, `GetSystem2D/3D`, name `"dx11"`/`"vulkan"`), used for the profiler backend name, the log line `Graphics backend: <name> on <adapter>`, and the main window title suffix ` [Vulkan]` (nothing added for DX11, so DX11 looks exactly as before). If `GraphicsVulkanLayer::Init` fails: `guiMessageBox` (or stderr in `BATCH_MODE`) with `GraphicsVulkanLayer::GetInitError()`, and the app exits with a non-zero code. At shutdown the Vulkan layer prints `Vulkan validation: N errors, M warnings` to the log (and stderr in RenderNode).

- [ ] **Step 1: gfxcompare validation gate (TDD, Python):** `rendernode.run` returns the process's stderr; new `rendernode.validation_counts(stderr) -> tuple[int, int] | None` parses `Vulkan validation: N errors, M warnings`; `gfxcompare run` fails (exit 1, reason in `report.md`) if either run reports any. Tests: `test_validation_counts_parsed`, `test_validation_counts_absent_for_dx11`, `test_cli_run_fails_on_validation_messages` (fake exe via `sys.executable -c` writing an image and the line).
- [ ] **Step 2:** Implement the switch. Build all six configurations; all unit tests pass.
- [ ] **Step 3: Manual checks** (Debug and Release, Qt and Wx): `-gfx vulkan -new` → main window and every render panel show the background clear colour, title ends in `[Vulkan]`; resize panels, change the panel layout, minimize/restore the main window; quit → log shows `Vulkan validation: 0 errors, 0 warnings` and no crash. `-gfx dx11 -new` and no flag → unchanged. RenderNode: `-gfx vulkan -new` starts and exits with code 0 and the validation line on stderr. `MSP_GFX_VK_ADAPTER=UHD` (Intel) → exits with a message naming the missing requirements (Vulkan 1.4, `VK_EXT_descriptor_heap`, ...). Record the `GVK_NOT_YET` names logged in a `-gfx vulkan -new` session in the commit message.
- [ ] **Step 4:** Plan 1 harness still passes for DX11: `gfxcompare run` DX11 vs DX11 on the test scene (Release RenderNode, `MspGfxProfiling=true`).
- [ ] **Step 5: Commit** — "Choose the graphics backend with -gfx; Vulkan starts and clears its windows".

---

### Task 8: Docs and CI

**Files:** Modify `CLAUDE.md`, `.github/workflows/build-windows.yml`, the spec only if a decision changed (descriptor model), `docs/superpowers/plans/2026-10-17-graphics-vulkan-roadmap.md` (Plan 2 row: done; note the descriptor decision).

- [ ] **Step 1: CLAUDE.md:** GraphicsVulkan section in LibXLT (packages `vkd`, `G2d`, `G3d`, `stub`; namespace `gvk`; `GVK_NOT_YET`; descriptor model; validation policy; `-gfx`, `-gfxValidation`, `MSP_GFX_VK_ADAPTER`), the dependencies table row for `LibXLT/GraphicsVulkan/vcpkg.json` and its newer baseline, and `UnitTests/GraphicsTests` Vulkan tests skipping without a device.
- [ ] **Step 2: CI:** nothing to add for the build (MainApp references GraphicsVulkan); confirm the vcpkg cache key covers the new manifest (it hashes every `vcpkg.json`/`vcpkg-configuration.json`); GraphicsTests' Vulkan tests skip on the runner (no GPU) — check the XML shows them skipped, not failed, on the first CI run after push (owner pushes; note it in the report).
- [ ] **Step 3: Commit** — "Document GraphicsVulkan; roadmap: Plan 2 done".
- [ ] **Step 4: Stop for review.** Report: what was built, the descriptor-model decision and evidence, validation counts, the `GVK_NOT_YET` list, anything surprising. Don't write Plan 3 until the owner says so.
