# GraphicsVulkan materials and `shdw` passes — Implementation Plan (Phase 1, Plan 5)

> **Status: STUB.** Written in detail only after Plans 4a and 4b are finished and reviewed by the owner. Execute with superpowers:executing-plans, inline (no subagents); stop for review when done. Likely the largest plan; may be split into 5a/5b when detailed.

**Goal:** The full beauty path renders: every material effect, the HDR renderer and all its passes, reflections, AO and GI, matching DX11 on synthetic scenes and the test scene's beauty frames.

**Spec:** "Scope" (Decided 5), "Proving parity", "Validation".

**Depends on (Plans 4a and 4b):** textures and render targets (4a); fragments, all the material shader classes, the per-node rendering machinery, the forward renderer, windows rendering scenes and fonts (4b).

**Moved here from Plan 4b (its decision 1):** MachStudio never uses the forward renderer (`shdwPackage` replaces the renderer creator; the default is `shdwHDRRendererDX11`), so test-scene frame parity, Vulkan against DX11, is this plan's. Until it lands, `g3dSceneRendererCreateVk` answers every renderer type with the forward renderer and `GVK_NOT_YET`; this plan replaces that fallback with the real `shdw` creator.

## Scope

- Material effects: the classes are ported (Plan 4b, `effShaderBaseVk` and the `eff*` classes, each checked by `SetupWritesSameConstants`); what remains is drawing each material shader (Lambert, Phong, Blinn variants, reflection, skin, SSS, car paint, cartoon, anisotropic, fresnel, transparent, FC3D variants, testtess) and post effect (black-and-white, gradient map, sepia, sketch) through the `shdw` passes, with an image test each.
- Shadow maps and RSMs get images (they are textures with a size and no image since Plan 4a); cube render targets exist (Plan 4b) and are filled by the cube map renderer here; `matRenderTargetTexture::CopyWithResolve`'s scaled and depth copies.
- `effShaderArray`'s default/lighting/map-normals shaders (loaded from `.o` files for `shdw`'s rasterizer util) and `g3dRenderPass`, which Plan 4b left for the code that uses them.
- `shdw` copied into `gvk` and ported: `shdwHDRRendererVk` and passes — Depth/ZFill, Ambient, Lit, Environment, EnvBackground, Transparent, AlphaFill, Opacity, Glow, DOF, Fog, Outline, PostShader, ToneMap, Normals/MapNormalsToScreen, Velocity, Materials, SSAO, AOVolumes, SSGI, GIVolumes, LPVGI (compute), Traversal; `rtUtil` ray tracing compute; frame buffer manager.
- Reflections: cube-map (`shdwCubeMapRendererVk`, `g3dCubeMapRendererVk`) and planar (`shdwPlanarReflectionRendererVk`); multiple reflective objects (the known hot spot) measured separately.
- Shadow maps, VSM, RSM as used by the beauty path; queries (occlusion) and the one deferred-context use.
- Markers per Plan 1's naming rule so DX11 and Vulkan CSVs line up path for path.

## Exit criteria

- One synthetic GraphicsTests scene per pass/renderer above, Vulkan vs DX11 within tolerance (any loosened tolerance has a written reason).
- Test-scene beauty frames within tolerance via `gfxcompare run` (`--a-args "-gfx dx11" --b-args "-gfx vulkan -gfxValidation"`).
- Timing report per pass for both backends, with the reflection passes called out.
- Zero validation errors/warnings.

## Open questions

- Order of passes to port (suggest: depth → ambient → lit → environment → transparency → reflections → glow/DOF/fog/tone map → AO → GI).
- Split point if the plan is too big for one review.
