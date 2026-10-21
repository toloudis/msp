/*****************************************************************************
**  gtRender.hpp
**
**      Render-and-compare helpers for the scene parity tests: a synthetic
**      scene (gtSceneBuilder) rendered by a scene renderer into an RGBA8
**      render-target texture on whichever backend is up, and the same
**      scene rendered on DX11 and on Vulkan and compared with gfxcompare's
**      tolerance (gtImage). By default the renderer comes from the creator
**      (g3dSceneRendererCreate::CreateDefaultRenderer), as MachStudio's
**      viewport gets it, so both backends run the same code end to end.
**
**	Extra Large Technology
\****************************************************************************/
#pragma once

#include "gtTarget.hpp"

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/G3d/g3dPrefs.hpp"
#endif
#ifndef G3D_SINGLELIGHTRENDERING_HPP
#include "Graphics/G3d/g3dSingleLightRendering.hpp"
#endif

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

class effShaderParams;
class g3dLight;
class g3dSceneRenderer;
class gtSceneBuilder;

//	Builds a scene into the builder (on the backend that is up); the
//	directory is a temporary one for files the scene needs (textures)
typedef std::function<void(gtSceneBuilder&, const std::filesystem::path&)> gtSceneFunction;

struct gtRenderOptions
{
	int		m_Width = 256, m_Height = 256;

	//	Renders into a window (gtSceneWindow: BeginScene, Clear, Render,
	//	EndScene and Present each frame) instead of a render-target texture:
	//	the preview renderers draw only into windows (they cast the target
	//	to the backend's window class, as MachStudio's viewport gives them)
	bool	m_bInWindow = false;

	//	The renderer (empty: g3dSceneRendererCreate::CreateDefaultRenderer)
	std::function<g3dSceneRenderer*()>			m_CreateRenderer;

	//	Edits the preferences for the run (they are restored after it)
	std::function<void(g3dPrefs::g3dRenderPrefs&)>	m_Prefs;

	//	g3dSingleLightRendering's flag for the run (MachStudio runs with it on)
	bool	m_bSingleLight = true;

	//	Frames rendered; the last is kept (tone-map adaptation and
	//	reflections need more than one)
	int		m_Frames = 1;

	float	m_SimTime = 1.25f;

	//	Runs before each frame (shadow maps and reflections)
	std::function<void(gtSceneBuilder&)>		m_BeforeFrame;

	//	Runs after the last frame, on the backend that is up, while the
	//	scene is still there (to read its render targets back)
	std::function<void(gtSceneBuilder&)>		m_AfterRender;

	//	The comparison checks that DX11 drew on a good part of the image
	//	(off for images that are mostly black by design: debug buffers)
	bool	m_bCheckCoverage = true;

	//	The tolerance (gtCompareImages'): loosen it only for a case in its
	//	test's exception table, with the measured error and the reason
	float	m_MaxErr = 2 / 255.0f;
	float	m_MaxPctOver = 0.1f;
};

//	Saves g3dPrefs::CurrentPrefs(), g3dSingleLightRendering's flags,
//	g3dPostProcessing's effect and g3dPassBuffers' file-reflection flag,
//	and restores them when it goes
class gtPrefsScope
{
	public:
		gtPrefsScope();
		~gtPrefsScope();

	private:
		gtPrefsScope(const gtPrefsScope&) = delete;
		gtPrefsScope& operator = (const gtPrefsScope&) = delete;

		g3dPrefs::g3dRenderPrefs	m_Prefs;
		bool						m_bSingleLight;
		g3dSingleLightRendering::ShadowQualityOverride	m_ShadowQuality;
		bool						m_bDOFPrep;
		bool						m_bTransparent;
		bool						m_bGlow;
		bool						m_bAmbientEnvironment;
		bool						m_bCubeReflectionGen;
		bool						m_bPlaneReflectionGen;
		bool						m_bBaking;
		bool						m_bIsolateReflections;
		const g3dLight*				m_pActiveLight;
		bool						m_bFirstLight;
		bool						m_bPostActive;
		shared_ptr<effShaderParams>	m_PostEffect;
		bool						m_bDoingFileRefl;
};

//	i_Build's scene rendered on the backend that is up, into an
//	m_Width x m_Height target cleared to (0, 0, 0, 0) (a window cleared to
//	black, if m_bInWindow); the builder's viewer
//	layers are passed to Render
gtPicture gtRenderScene(const gtSceneFunction& i_Build, const std::filesystem::path& i_Dir, const gtRenderOptions& i_Options);

//	Renders on both backends and compares: the DX11 image must cover a good
//	part of the target, and the two must match; on a mismatch the images
//	are saved (gtSaveComparison) under i_pTest
void gtExpectSameImage(const char* i_pTest, const gtSceneFunction& i_Build, const gtRenderOptions& i_Options = gtRenderOptions());

struct gtCase
{
	std::string			m_Name;
	gtSceneFunction		m_Build;
	gtRenderOptions		m_Options;
};

//	gtExpectSameImage for several cases, all rendered under one
//	gtBackend::Run per backend; each is reported as <test>/<case>
void gtExpectSameImages(const char* i_pTest, const std::vector<gtCase>& i_Cases);

//	The feature shows: on DX11, the scene with it (i_With) differs from the
//	scene without it (i_Without) by more than 8/255 in at least
//	i_MinPixels pixels; saves both if not (with as dx11, without as
//	vulkan). Guards against a parity test that passes because DX11 draws
//	nothing for the feature in the test scene.
void gtExpectEffectShows(const char* i_pName, const gtSceneFunction& i_With, const gtRenderOptions& i_WithOptions,
						 const gtSceneFunction& i_Without, const gtRenderOptions& i_WithoutOptions, int i_MinPixels);
