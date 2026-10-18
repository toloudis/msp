# GraphicsVulkan textures — Implementation Plan (Phase 1, Plan 4a)

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task, inline in the session (the owner asked for no subagents). Steps use checkbox (`- [ ]`) syntax for tracking. When the plan is done, stop for the owner's review before Plan 4b (`2026-10-17-gfx-vulkan-4b-first-scene.md`) is started.

**Goal:** Every texture GraphicsDX11 can load or create exists on Vulkan and samples the same as on DX11, decoded by portable code instead of DirectXTex and WIC; libtiff comes from vcpkg.

**Architecture:** Plan 4 is split in two. This half (4a) builds what Plan 4b's fragments, materials and renderer stand on:
- libtiff from vcpkg instead of `3rdParty/` (decision 4)
- `vkdImage` grown into a general image (mips, cubes, volumes, arrays, compressed formats, uploads, blits), `vkdBuffer`, `vkdFormat`
- portable texture decoding and DirectXTex's mip filter, tested on the CPU against DirectXTex
- the texture manager and render-target textures (`Mat`, the `G2d` texture and render-target classes)
- `gtBackend`, the harness that runs a test under the DX11 layer and then under the Vulkan layer

Ported code is copied and changed only where it touches D3D, as in Plans 2 and 3. Each GPU test drives both backends through their own `GraphicsXxxLayer`, one after the other in the same test, because both install the same Graphics hooks.

**Tech Stack:** C++17 (MSVC x64), Vulkan 1.4 C API via volk, VMA, stb_image and libtiff (vcpkg), tinyddsloader (vendored in `3rdParty/`), GoogleTest. DirectXTex only in tests, to make inputs and expected results.

**Spec:** `docs/superpowers/specs/2026-10-17-graphics-vulkan-design.md`, sections "Library layout", "Modern Vulkan" (formats, host image copy), "Proving parity", "Validation".
- Roadmap: `docs/superpowers/plans/2026-10-17-graphics-vulkan-roadmap.md`.
- From Plan 3 (done), this plan uses: `vkdContext`, `vkdPipelineCache`, `vkdShader`, `fxEffectVk`/`effPlainEffectVk`, the state holders, `g2dFullscreenQuad`, and the test helpers (`gtD3D11`, `gtVulkanContext`, `gtImage`).
- Plan 4b uses what this plan produces: `vkdImage`, `vkdBuffer`, `vkdFormat`, `matTextureMgrVk`, `matRenderTargetTexture`, `g3dVkTextureUtil`, `gtBackend`.

## Decisions made in this plan (please review)

1. **Texture decoding is portable: no DirectXTex or WIC in GraphicsVulkan** (owner's decision). GraphicsDX11 decodes DDS and TGA with DirectXTex, the rest with WIC, and makes mips with DirectXTex's `TEX_FILTER_TRIANGLE`. GraphicsVulkan uses:

   | Files | Decoder |
   |---|---|
   | PNG, JPEG, BMP, GIF (first frame), TGA | stb_image (vcpkg `stb`) |
   | TIFF | libtiff (vcpkg `tiff`, decision 4) |
   | HDR | GraphicsDX11's own RGBE reader, copied (it uses DirectXTex only for the float → half conversion, which becomes a small function of ours) |
   | DDS | tinyddsloader (MIT, one header; not in vcpkg), vendored at `3rdParty/tinyddsloader/` with its licence and a README naming the upstream commit |
   | WMP (JPEG XR) | not supported: the load fails and the caller falls back as on any DX11 load failure, logged once |

   - **What has to match is what DX11 samples, not the format names.** The decoder picks the format DirectXTex would, wherever it changes what a shader reads:
     - grayscale → `R8_UNORM` (or `R16_UNORM`), so it samples as red only;
     - an sRGB chunk or gAMA 1/2.2 in a PNG, or EXIF colour space sRGB in a JPEG → the `_SRGB` format;
     - 16-bit PNG/TIFF → `R16G16B16A16_UNORM`; float TIFF → `R32G32B32A32_FLOAT`;
     - palettized and 24-bit → RGBA8 with alpha 255.

     BGRA where DirectXTex gives BGRA and stb gives RGBA isn't a difference that matters; the decoder may give RGBA.
   - **DDS:** tinyddsloader reports DXGI format numbers (its own enum, no `dxgiformat.h`), mips, cubes, arrays and volumes. BC1–7 go to Vulkan as they are. The legacy formats DirectXTex converts and tinyddsloader doesn't map (24-bit RGB, L8, A8L8, L16, A4L4 and the rest, listed in the decoder with what DirectXTex makes of each) are converted by our code the same way.
   - **Mips and the width/height reduction:** DirectXTex's triangle filter (`GenerateMipMaps` with `TEX_FILTER_TRIANGLE`) and its `Resize` for power-of-two reductions are ported (MIT; credited in the file), with DirectXTex's float conversions, so each level matches DX11's. For sRGB formats the filtering happens in linear space, as DirectXTex does.
   - A DXGI → `VkFormat` table (`vkdFormat`) maps the decoded formats. Where Vulkan lacks a format the substitution is named in the table and tested.
   - Image saving (Plan 6) follows the same rule: stb_image_write and libtiff, no WIC.
2. **Buffers:** static vertex and index buffers are device-local, uploaded through the ring buffer. Buffers DX11 updates with `Map(WRITE_DISCARD)` are written to the ring per use, as `vkdContext::Upload` already does. (`vkdBuffer` is built here; Plan 4b's fragments use it.)
3. **Texture-manager scope.** Calls that belong to other plans keep their stubs: shadow maps, RSMs and cube render targets go to Plan 5 (they come with their passes); `MergeTransparentTextures` and the save-to-file calls go to Plan 6.
4. **libtiff comes from vcpkg, and `3rdParty/` loses its copy** (owner's decision; Task 1).
   - `tiff` (4.7.1 at the current baseline, default features: zip, jpeg, lzma) joins GraphicsDX11's and GraphicsVulkan's manifests. The two manifests have different baselines, and MachStudio loads one `tiff.dll`, so both pin the same version with `overrides` (and the same for the DLLs it brings: `libjpeg-turbo`, `liblzma`, `zlib`).
   - GraphicsDX11's one include (`g2dSurfaceLoaderDX11.hpp`: `"tiff-4.0.6/libtiff/tiffio.h"` → `<tiffio.h>`) changes; this is the only GraphicsDX11 source edit in Plan 4.
   - MainApp links `tiffd.lib` (Debug) / `tiff.lib` (Release) from GraphicsDX11's vcpkg lib directory; `CopyLibraryVcpkgDlls` already copies the DLLs.
   - Deleted: `3rdParty/tiff-4.0.6/`, `3rdParty/Libs/x64/tiff.{dll,lib}`, and the tracked `MachStudio/MachStudio-x64/tiff.dll` (the copy target skips files that exist, so a stale 4.0.6 DLL would never be replaced). The new DLLs are git-ignored like the OpenEXR ones.
5. **Test harness for both backends (`gtBackend`).** One test can run a function under the DX11 layer, then under the Vulkan layer. Each run does the layer's `Init` on a hidden window, `GraphicsLayer::InitGraphics` and the layer's `InitGraphics`, then the clean-ups. The DX11 DLLs the layer needs are copied next to the test exe. Vulkan tests still skip without a device; the DX11 side uses the hardware adapter or WARP.

## Global Constraints

- **No GraphicsDX11 changes** apart from decision 4's include line and manifest, and no changes to shader sources. DX11 output is unchanged: the Plan 1 harness on the test scene, DX11 against Plan 1's frames, must give 135/135.
- **Code layout:**
  - All GraphicsVulkan code is in namespace `gvk`.
  - Include guards start with `GVK_`, and classes GraphicsDX11 names `...DX11` are `...Vk`.
  - Packages follow GraphicsDX11's layout; includes are guarded.
  - No `d3d11.h`, `dxgiformat.h`, DirectXTex or WIC in GraphicsVulkan; DXGI formats are plain `uint32_t` values. The texture loaders and decoders are portable code (no `*Win` names). Tests may use DirectXTex.
- **Ported code** keeps DX11's structure, names and comments. Each D3D call becomes its `vkdContext` / `vkdImage` / `vkdBuffer` equivalent. A behaviour difference needs a ledgered ruling.
- **Validation:** zero errors and zero warnings (core and synchronization) in every test and run. `vkdValidation`'s notice list grows only with a ledgered ruling and a written reason.
- **Image tolerance:** the spec's, as in Plan 3. Max channel error ≤ 2/255, and at most 0.1% of pixels over 1/255.
  - Loosen it only per test case, with a written reason in that test's exception table.
  - Mip levels made by GPU filtering (D3D `GenerateMips` against Vulkan blits) are compared with the same tolerance unless a ruling says otherwise.
- Git: branch `claude/graphics-vulkan`; stage explicit paths only; no push. Commit messages end with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.

## Review Focus

1. **Texture files that WIC and stb_image read differently.** Grayscale, sRGB-tagged, 16-bit, palettized and 24-bit PNGs, sRGB JPEGs, 8-bit/16-bit/float TIFFs, legacy DDS formats, every BC format, cube and volume DDS: each must sample the same on both backends, or name its substitution. Task 3 tests `DecodeMatchesDirectXTex` on the CPU; Task 4 tests `LoadedFormatsMatchDX11` on the GPU, one generated file per case.
2. **Generated mips.** Textures whose mips are made at load time must match per level, including sRGB ones and the width/height reduction. Task 3 tests `TriangleMipsMatchDirectXTex` and `ReduceMatchesDirectXTex`; Task 2 tests `GenerateMipsMatchesD3D` (GPU mips); Task 4 tests `MipChainsMatchDX11`.
3. **Broken texture files.** A truncated or corrupt file of any format must fail the load (and fall back as on DX11), never crash or read out of bounds. Task 3 tests `TruncatedFilesFail`, for every decoder.
4. **Lifetimes.** A texture or buffer destroyed right after a draw that uses it must not be freed while the GPU still reads it. Tasks 2 and 4 test `DestroyAfterDrawIsSafe` under synchronization validation.
5. **A stale libtiff.** A developer machine that still has the 4.0.6 `tiff.dll` in `MachStudio-x64` must end up with the vcpkg one after pulling and building. Task 1 deletes the tracked file and checks the DLL's version after a build.

---

## File Structure

| Path (under `LibXLT/GraphicsVulkan/` unless stated) | Responsibility |
|---|---|
| `vkd/vkdImage.hpp/.cpp` (grow) | 2D/cube/3D/array images, mips, any format; views (whole, one mip, one face/slice); upload of all levels; `GenerateMips` (blits); readback of any level/face |
| `vkd/vkdBuffer.hpp`, `private/vkdBuffer.cpp` | Device-local buffers with upload; `DeferDestroy` lifetime |
| `vkd/vkdFormat.hpp`, `private/vkdFormat.cpp` | DXGI ↔ `VkFormat` (and substitutions), bytes/blocks per format |
| `Mat/matImageDecode.hpp`, `private/matImageDecode{,Stb,Tiff,DDS,HDR}.cpp` | Decode a file into a CPU image (DXGI format number, mips, faces, slices, rows) the way DirectXTex/WIC would (decision 1) |
| `Mat/matMipFilter.hpp`, `private/matMipFilter.cpp` | Port of DirectXTex's triangle-filter mip generation and power-of-two `Resize` |
| `3rdParty/tinyddsloader/` (repo root) | Vendored `tinyddsloader.h`, `LICENSE`, `README.md` (upstream URL and commit) |
| `Mat/` (`matTextureVk`, `matPlainTexture`, `matMipTexture`, `matStaticCubeTexture`, `matVolumeTexture`, `matRenderTargetTexture`, `matTextureMgrVk`, loaders `matTextureLoader{,DDS,HDR,TGA}` on `matImageDecode`) | Port of GraphicsDX11's `Mat` (scope: decision 3) |
| `G2d/` (`g2dTextureVk`, `g2dRenderTargetVk`, `g2dRenderTargetTextureVk`, `g2dDepthStencilBufferVk`) | Ports of the G2d texture and render-target classes |
| `G3d/g3dVkTextureUtil` | `vkdImage* GetImage(const matTexture*)`, the counterpart of `g3dDX11TextureUtil::GetD3DTexture` |
| `stub/` | Stubs replaced in this plan are deleted; the rest stay |
| `UnitTests/GraphicsTests/gtBackend.hpp/.cpp` | Runs a function under the DX11 layer or the Vulkan layer (decision 5) |
| `UnitTests/GraphicsTests/Vk/*Test.cpp` | Tests per task |

---

### Task 1: libtiff from vcpkg (decision 4)

**Files:**
- Modify: `LibXLT/GraphicsDX11/vcpkg.json` (add `tiff`, `overrides`), `LibXLT/GraphicsVulkan/vcpkg.json` (add `tiff`, `stb`, the same `overrides`), `LibXLT/GraphicsDX11/G2d/private/g2dSurfaceLoaderDX11.hpp` (include line only), `MachStudio/Source/MainApp/MainApp.vcxproj` (`tiffd.lib`/`tiff.lib` per configuration), `.gitignore`, `CLAUDE.md` and `3rdParty/thridParty.txt` (dependency tables).
- Delete: `3rdParty/tiff-4.0.6/`, `3rdParty/Libs/x64/tiff.dll`, `3rdParty/Libs/x64/tiff.lib`, `MachStudio/MachStudio-x64/tiff.dll`.
- Test: `UnitTests/GraphicsTests/Vk/tiffVcpkgTest.cpp` (GraphicsTests gets `tiff` in its manifest with the same `overrides`).

- [ ] **Step 1: Failing tests:**
  - `TiffRoundTrip`: write an 8-bit RGBA and a 32-bit float TIFF with libtiff into the test's temp dir, read them back with `TIFFReadScanline`; same bytes.
  - `TiffVersionPinned`: `TIFFGetVersion()` names 4.7.1.
- [ ] **Step 2:** Build; fail (no `tiffio.h`).
- [ ] **Step 3:** Manifests, include line, MainApp link line, deletions, `.gitignore`.
- [ ] **Step 4:** Six-configuration build and tests pass. `MachStudio-x64` has the vcpkg `tiff.dll` (Release) / `tiffd.dll` (Debug) and its dependencies, with 4.7.1 in the file version; nothing links `3rdParty/Libs/x64/tiff`; `git grep tiff-4.0.6` finds nothing.
- [ ] **Step 5:** DX11 harness, DX11 against Plan 1's frames: 135/135. Load a TIFF image in MachStudio Qt (DX11) by hand: it shows.
- [ ] **Step 6:** Commit — "Build: libtiff from vcpkg; remove 3rdParty/tiff-4.0.6".

---

### Task 2: `vkdImage` for every texture shape, `vkdBuffer`, `vkdFormat`

**Files:**
- Modify: `vkd/vkdImage.hpp`, `vkd/private/vkdImage.cpp`
- Create: `vkd/vkdBuffer.hpp`, `vkd/private/vkdBuffer.cpp`, `vkd/vkdFormat.hpp`, `vkd/private/vkdFormat.cpp`
- Test: `Vk/vkdImageTest.cpp`, `Vk/vkdBufferTest.cpp`, `Vk/vkdFormatTest.cpp`

**Interfaces — Produces** (`gvk`):
```cpp
struct vkdImageDesc { VkImageType m_Type = VK_IMAGE_TYPE_2D; VkFormat m_Format; uint32_t m_Width, m_Height, m_Depth = 1;
                      uint32_t m_Mips = 1; uint32_t m_Layers = 1; bool m_bCube = false; VkSampleCountFlagBits m_Samples = VK_SAMPLE_COUNT_1_BIT;
                      VkImageUsageFlags m_Usage; VkImageAspectFlags m_Aspect = VK_IMAGE_ASPECT_COLOR_BIT; };
class vkdImage {   // existing Create2D stays, as a wrapper of Create
    static std::unique_ptr<vkdImage> Create(vkdDevice&, const vkdImageDesc&, const char* i_Name);
    const vkdImageDesc& Desc() const;
    //  A view of one mip / one layer (cube face, array slice), for render targets and per-level reads;
    //  owned by the image, created on first use
    vkdImage* Subresource(uint32_t i_Mip, uint32_t i_Layer);
    //  Depth-only view, for sampling a depth-stencil image
    vkdImage* DepthView();
    void Upload(vkdContext&, uint32_t i_Mip, uint32_t i_Layer, const void* i_pData, size_t i_RowPitch, size_t i_SlicePitch);
    void GenerateMips(vkdContext&);                 // linear blits, level by level, as D3D11 GenerateMips
    std::vector<uint8_t> ReadBack(uint32_t i_Mip = 0, uint32_t i_Layer = 0);   // any format vkdFormatSize knows (blocks for BC)
};
class vkdBuffer {
    static std::unique_ptr<vkdBuffer> Create(vkdDevice&, VkDeviceSize i_Size, VkBufferUsageFlags i_Usage, const char* i_Name);
    void Upload(vkdContext&, VkDeviceSize i_Offset, const void* i_pData, VkDeviceSize i_Size);   // through the ring
    VkBuffer Buffer() const; VkDeviceAddress Address() const; VkDeviceSize Size() const;
};      // destruction deferred until the GPU is done
//  vkdFormat: DXGI_FORMAT values as plain uint32_t (no dxgiformat.h)
VkFormat vkdFormatFromDxgi(uint32_t i_Dxgi, const char** o_pSubstitution = nullptr);   // VK_FORMAT_UNDEFINED if unsupported
uint32_t vkdDxgiFromFormat(VkFormat);
struct vkdFormatInfo { uint32_t m_BlockBytes; uint32_t m_BlockWidth, m_BlockHeight; bool m_bDepth, m_bStencil, m_bCompressed, m_bSrgb; };
vkdFormatInfo vkdFormatInfoOf(VkFormat);
```

- [ ] **Step 1: Failing tests:**
  - `vkdFormatTest`:
    - `DxgiRoundTrip` covers every DXGI format the loaders and render targets use, from a table in the test.
    - `SubstitutionsNamed`: `DXGI_FORMAT_B8G8R8X8_UNORM` → B8G8R8A8 with a reason.
    - `BlockSizes`.
  - `vkdImageTest`:
    - `UploadReadBackEveryShape`: 2D with mips, cube (6 faces), 3D, 2D array; RGBA8, R16F, RGBA32F, BC1 and BC3, where BC is read back as blocks.
    - `GenerateMipsMatchesD3D`: a 256×256 gradient with noise, 9 levels. Each level against D3D11's `GenerateMips` on the same data (gtD3D11), within tolerance.
    - `SubresourceViewsRenderPerFace`: clear each cube face to a different colour through `vkdContext`, then read back.
    - `DepthViewSamples`.
  - `vkdBufferTest`:
    - `UploadThenDraw`.
    - `DestroyAfterDrawIsSafe` (sync validation).
- [ ] **Step 2:** Build; fail.
- [ ] **Step 3:** Implement.
- [ ] **Step 4:** Tests pass with zero validation messages.
- [ ] **Step 5:** Commit — "vkd: images of every shape and format, buffers, DXGI format mapping".

---

### Task 3: Image decoding and mip filtering without DirectXTex (decision 1)

CPU only: no device needed, so these tests run on CI too.

**Files:**
- Create: `3rdParty/tinyddsloader/{tinyddsloader.h,LICENSE,README.md}` (repo root); GraphicsVulkan's include path gets `3rdParty` if it lacks it, and the decoder includes `"tinyddsloader/tinyddsloader.h"`.
- Create: `Mat/matImageDecode.hpp`, `Mat/private/matImageDecode{,Stb,Tiff,DDS,HDR}.cpp` (one `#define STB_IMAGE_IMPLEMENTATION` and one `TINYDDSLOADER_IMPLEMENTATION`, in their own files), `Mat/matMipFilter.hpp`, `Mat/private/matMipFilter.cpp`.
- Modify: `CLAUDE.md` (the `3rdParty/` dependency table gets tinyddsloader; the vcpkg table gets `stb` and `tiff` for GraphicsVulkan).
- Test: `Vk/matImageDecodeTest.cpp`, `Vk/matMipFilterTest.cpp` (inputs made with DirectXTex and libtiff in the test; GraphicsTests already reaches DirectXTex).

**Interfaces — Produces** (`gvk`):
```cpp
struct matDecodedImage {
    uint32_t m_DxgiFormat;                    // what DirectXTex would report, or its named substitute
    uint32_t m_Width, m_Height, m_Depth = 1, m_Mips = 1, m_Layers = 1; bool m_bCube = false;
    struct Sub { size_t m_Offset, m_RowPitch, m_SlicePitch; };
    std::vector<Sub> m_Subs;                  // layer-major, then mip
    std::vector<uint8_t> m_Bytes;
};
enum class matImageFileType { e_DDS, e_TGA, e_HDR, e_BMP, e_JPG, e_PNG, e_TIFF, e_GIF, e_WMP, e_Unknown };   // pick_format's
bool matDecodeImage(const fsLocator&, matImageFileType, matDecodedImage& o_Image, itString* o_pError);
bool matDecodeImageMemory(const void*, size_t, matImageFileType, matDecodedImage&, itString*);   // LoadFromWICMemory's counterpart
// Ports of DirectXTex (TEX_FILTER_TRIANGLE mips; Resize to width>>w, height>>h)
bool matGenerateMips(const matDecodedImage& i_Level0, uint32_t i_Levels /*0 = full chain*/, matDecodedImage& o_Chain);
bool matReduce(const matDecodedImage&, int i_WidthReduce, int i_HeightReduce, matDecodedImage& o_Image);
```

- [ ] **Step 1: Failing tests:**
  - `DecodeMatchesDirectXTex`: for each file in Review Focus 1 (generated by the test), `matDecodeImage` against DirectXTex's `LoadFrom{DDS,TGA,WIC}File` (and GraphicsDX11's RGBE path for HDR): same DXGI format (or the BGRA/RGBA allowance, or a named substitute), same size, mips and faces; bytes equal, except JPEG, which is within 2/255 per channel (WIC's and stb's IDCTs differ; the measured error is printed).
  - `LegacyDdsConverted`: each legacy DDS format in the decoder's table against DirectXTex's conversion.
  - `WmpUnsupported`: fails with an error naming the format.
  - `TruncatedFilesFail`: every test file cut at 0, 16 bytes, half, and length−1, and one with a corrupted header: returns false, no crash; run under the Debug CRT heap checks.
  - `TriangleMipsMatchDirectXTex`: RGBA8, sRGB RGBA8, R8, RGBA16F, RGBA32F; square, non-square, odd sizes; each level within 1/255 (floats: 1e-3 relative) of DirectXTex `GenerateMipMaps(TEX_FILTER_TRIANGLE)`.
  - `ReduceMatchesDirectXTex`: reductions 1 and 2 against `Resize(..., TEX_FILTER_DEFAULT)`; same tolerance.
- [ ] **Step 2:** Build; fail.
- [ ] **Step 3:** Implement.
- [ ] **Step 4:** Pass. Any per-format difference beyond the tolerance gets a ledgered ruling with its measured error.
- [ ] **Step 5:** Commit — "GraphicsVulkan: portable texture decoding (stb_image, libtiff, tinyddsloader) and DirectXTex's mip filter".

---

### Task 4: Textures, render-target textures, and the both-backends harness

**Files:**
- Create: `Mat/*` and `G2d/g2dTextureVk`, `g2dRenderTargetVk`, `g2dRenderTargetTextureVk`, `g2dDepthStencilBufferVk` (replacing Plan 2's depth buffer), `G3d/g3dVkTextureUtil`.
- Delete: the `matTextureMgrVk` / `matTextureVk` stubs. Install the real manager where `g3dSystemVk` installed the stub.
- Test: `UnitTests/GraphicsTests/gtBackend.hpp/.cpp`, `Vk/matTextureMgrVkTest.cpp`. Modify `GraphicsTests.vcxproj` to copy GraphicsDX11's runtime DLLs next to the exe.

**Interfaces — Produces:**
- `matTextureMgrVk`: every `matTextureMgr` call except those listed in decision 3, behaving as `matTextureMgrDX11`, including the reference counting by locator, reload and unload, `GetTotalTextureMemory` from the memory budget, and `GetPoisson`.
- `matRenderTargetTexture`: a `matTexture` and a `g2dRenderTarget`. `MakeCurrent` sets its image and depth buffer on `vkdContext`, and `Clear*` goes through the context.
- `g3dVkTextureUtil::GetImage(const matTexture*) -> vkdImage*`.
- `gtBackend::Run(gtBackend::Kind, const std::function<void()>&)` skips (`GTEST_SKIP`) a backend that isn't available, and returns whether it ran.

- [ ] **Step 1: Failing tests** (each runs the same code under both backends and compares what it reads back):
  - `LoadedFormatsMatchDX11`: the files of Task 3's `DecodeMatchesDirectXTex`, one per case in Review Focus 1. Each is loaded through `matTextureMgr::LoadTexture` (DX11: DirectXTex/WIC; Vulkan: `matImageDecode`), drawn with `g2dFullscreenQuad::DrawTexturedQuad`, and compared.
  - `MipChainsMatchDX11`: each level, drawn at its size.
  - `CubeAndVolumeMatchDX11`: each face; slices of the volume texture, through a test shader.
  - `CreateFillUpdateCopyMatchDX11`.
  - `RenderTargetTextureClearsAndDraws`.
  - `ReloadAndUnload`: the same locator gives the same texture, and a reload changes its content.
  - `DestroyAfterDrawIsSafe`.
- [ ] **Step 2:** Build; fail.
- [ ] **Step 3:** Implement (port).
- [ ] **Step 4:** Pass, with zero validation messages.
- [ ] **Step 5:** Commit — "GraphicsVulkan: textures and render-target textures (Mat, G2d)".

---

### Task 5: Verify, docs, stop for review

**Files:** Modify `CLAUDE.md` (GraphicsVulkan packages: `Mat`, the new `vkd` pieces; the dependency tables if Task 1 or 3 left anything out) and the roadmap (Plan 4a done, with results and notes for 4b).

- [ ] **Step 1:** Six-configuration build with all tests passing; a whole-suite run with GPU-assisted validation: 0/0.
- [ ] **Step 2:** DX11 harness, DX11 against Plan 1's frames: 135/135.
- [ ] **Step 3:** RenderNode and MachStudio Qt start and quit with `-gfx vulkan` (`-version` for RenderNode): `Vulkan validation: 0 errors, 0 warnings`.
- [ ] **Step 4:** Edit the docs; commit — "GraphicsVulkan: docs and roadmap: Plan 4a done".
- [ ] **Step 5: Stop for review.** Report:
  - what was built;
  - the decoder and parity results per format, with measured errors and any exceptions;
  - format substitutions;
  - validation;
  - rulings and deferred minors.

  Don't start Plan 4b until the owner says so.
