# GraphicsVulkan hair and Phase 1 sign-off — Implementation Plan (Phase 1, Plan 7)

> **Status: STUB.** Written in detail only after Plan 6 is finished and reviewed by the owner. Execute with superpowers:executing-plans, inline (no subagents); stop for review when done.

**Goal:** Hair renders on Vulkan (or is formally deferred), the test scene passes end to end, and Phase 1's exit criteria are demonstrated and written up.

**Spec:** "Decided" 4 (hair), 9 (Phase 1 exit), 10 (proof of parity); "Validation"; "Phase 2 direction".

**Depends on (Plan 6):** every other renderer and service, and the `-cap*` capture flags (`-capSampling`, `-capJitter`, `-capMotionSamples`, `-capPasses`, `-capHardwareAA`) for the full test-scene runs.

## Scope

- The opacity map renderer (moved here by Plan 6's decision 2): `shdwOpacityMapRendererDX11` with `shdwPassOpacity`, which fills a projected light's opacity shadow maps. Their only reader is `shdwPassHair`, so they matter once hair draws. Until then `shdwSceneRendererCreate::CreateOpacityMapRenderer` answers with the forward stand-in (logged `GVK_NOT_YET`), and `shdwSceneRendererVk::ShareBuffers` leaves that stand-in alone (it has no frame buffer; any other partner that isn't a `shdw` renderer asserts, as on DX11). Once it is ported, `ShareBuffers` can drop the stand-in check.
- Hair: `HairRendererVk`, `shdwPassHair`, `hair` package. Hull shaders (`Hair_SH`, `HairDefault`, `OpacityRender` → `Hair_HS`) fail in DXC's SPIR-V backend on `ProcessIsolineTessFactors`; write a Vulkan-only copy of the hull shader with the isoline factor computation by hand, in a file whose name marks it Vulkan-only. If that gets ugly, defer hair with a written reason and a clear runtime message.
- Full test-scene run (`Dume_Demo_testTESS.mab`) with `gfxcompare run`, all frames within tolerance; also the other scenes in the folder as extra coverage.
- Validation sign-off: Debug RenderNode run of the test scene and the full GraphicsTests suite with core + sync validation at zero; one GPU-assisted validation run at zero; best-practices output reviewed and summarized.
- Timing report: per-pass CPU and GPU, Vulkan vs DX11, committed (e.g. `docs/superpowers/reports/`), with the reflection hot spot called out — input to Phase 2.
- `CLAUDE.md`: GraphicsVulkan section (packages, `vkd`, flags `-gfx`, `-gfxValidation`, `-gfxProfile`, validation policy).

## Exit criteria (= Phase 1 exit)

- Every renderer's images match within tolerance (hair matched or formally deferred).
- Timing report exists for both backends.
- Zero validation errors/warnings (core + sync; GPU-assisted run included).
- DX11 behaves exactly as before (DX11 self-comparison still passes).

## Open questions

- Where the Phase 1 report lives and its format (Markdown in repo vs. published page).
