/*****************************************************************************
**  gtRender.cpp
**
**      See gtRender.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtRender.hpp"
#include "gtImage.hpp"
#include "gtScene.hpp"
#include "gtTempDir.hpp"

#ifndef MA_FUNCTIONS_HPP
#include "Core/Ma/maFunctions.hpp"
#endif
#ifndef G3D_POSTPROCESSING_HPP
#include "Graphics/G3d/g3dPostProcessing.hpp"
#endif
#ifndef G3D_LIGHTMGR_HPP
#include "Graphics/G3d/g3dLightMgr.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/G3d/g3dScene.hpp"
#endif
#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/G3d/g3dSceneRenderer.hpp"
#endif
#ifndef G3D_SCENERENDERERCREATE_HPP
#include "Graphics/G3d/g3dSceneRendererCreate.hpp"
#endif
#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/G2d/g2dRenderTarget.hpp"
#endif

#include <gtest/gtest.h>

#include <iostream>
#include <memory>

//============================================================================
//	gtPrefsScope
//============================================================================
gtPrefsScope::gtPrefsScope()
	: m_Prefs(g3dPrefs::CurrentPrefs()),
	m_bSingleLight(g3dSingleLightRendering::GetDoSingleLightRendering()),
	m_ShadowQuality(g3dSingleLightRendering::GetShadowQualityOverride()),
	m_bDOFPrep(g3dSingleLightRendering::GetDoDOFPrepPass()),
	m_bTransparent(g3dSingleLightRendering::GetDoTransparentPass()),
	m_bGlow(g3dSingleLightRendering::GetDoGlowPass()),
	m_bAmbientEnvironment(g3dSingleLightRendering::GetDoAmbientEnvironmentPass()),
	m_bCubeReflectionGen(g3dSingleLightRendering::GetDoCubeReflectionGen()),
	m_bPlaneReflectionGen(g3dSingleLightRendering::GetDoPlaneReflectionGen()),
	m_bBaking(g3dSingleLightRendering::GetDoBaking()),
	m_bIsolateReflections(g3dSingleLightRendering::GetDoIsolateReflections()),
	m_pActiveLight(g3dSingleLightRendering::GetActiveLight()),
	m_bFirstLight(g3dSingleLightRendering::IsFirstLight()),
	m_bPostActive(g3dPostProcessing::GetActive()),
	m_PostEffect(g3dPostProcessing::GetPostEffect())
{
}

gtPrefsScope::~gtPrefsScope()
{
	g3dPrefs::CurrentPrefs() = m_Prefs;
	g3dSingleLightRendering::SetDoSingleLightRendering(m_bSingleLight);
	g3dSingleLightRendering::SetShadowQualityOverride(m_ShadowQuality);
	g3dSingleLightRendering::SetDoDOFPrepPass(m_bDOFPrep);
	g3dSingleLightRendering::SetDoTransparentPass(m_bTransparent);
	g3dSingleLightRendering::SetDoGlowPass(m_bGlow);
	g3dSingleLightRendering::SetDoAmbientEnvironmentPass(m_bAmbientEnvironment);
	g3dSingleLightRendering::SetDoCubeReflectionGen(m_bCubeReflectionGen);
	g3dSingleLightRendering::SetDoPlaneReflectionGen(m_bPlaneReflectionGen);
	g3dSingleLightRendering::SetDoBaking(m_bBaking);
	g3dSingleLightRendering::SetDoIsolateReflections(m_bIsolateReflections);
	g3dSingleLightRendering::SetActiveLight(m_pActiveLight);
	g3dSingleLightRendering::SetFirstLight(m_bFirstLight);
	g3dPostProcessing::SetActive(m_bPostActive);
	g3dPostProcessing::SetPostEffect(m_PostEffect);
}

//============================================================================
//	Rendering
//============================================================================
gtPicture gtRenderScene(const gtSceneFunction& i_Build, const std::filesystem::path& i_Dir, const gtRenderOptions& i_Options)
{
	gtPrefsScope prefs;
	//	the same random numbers on both backends (SSAO's random texture)
	maFunctions::ResetRand();
	g3dSingleLightRendering::SetDoSingleLightRendering(i_Options.m_bSingleLight);
	if (i_Options.m_Prefs)
		i_Options.m_Prefs(g3dPrefs::CurrentPrefs());

	const float clear[4] = { 0, 0, 0, 0 };
	gtTarget target(gtBackend::Current(), i_Options.m_Width, i_Options.m_Height, clear);
	gtSceneBuilder builder;
	builder.Camera().SetAspect((float)i_Options.m_Width / (float)i_Options.m_Height);
	g3dLightMgr::SetAmbient(maFloatRGBA(0.1f, 0.15f, 0.2f, 1));
	i_Build(builder, i_Dir);

	std::unique_ptr<g3dSceneRenderer> renderer(i_Options.m_CreateRenderer
		? i_Options.m_CreateRenderer() : g3dSceneRendererCreate::CreateDefaultRenderer());
	for (int frame = 0; frame < i_Options.m_Frames; frame++)
	{
		if (i_Options.m_BeforeFrame)
			i_Options.m_BeforeFrame(builder);
		if (frame > 0)
		{
			target.Target()->MakeCurrent();
			target.Target()->Clear(maFloatRGBA(clear[0], clear[1], clear[2], clear[3]), true, 1.0f);
		}
		renderer->Render(target.Target(), builder.Camera(), builder.Scene(), builder.ViewerLayers(), i_Options.m_SimTime);
	}
	gtPicture picture = target.Read();
	renderer->ReleaseResources();
	renderer.reset();
	return picture;
}

namespace
{
	void compare(const std::string& i_Name, const gtPicture& i_DX11, const gtPicture& i_Vulkan, const gtRenderOptions& i_Options)
	{
		SCOPED_TRACE(i_Name);
		EXPECT_TRUE(i_DX11.Valid());
		EXPECT_TRUE(i_Vulkan.Valid());
		if (!i_DX11.Valid() || !i_Vulkan.Valid())
			return;
		ASSERT_EQ(i_DX11.m_Width, i_Vulkan.m_Width);
		ASSERT_EQ(i_DX11.m_Height, i_Vulkan.m_Height);

		//	the scene covers a good part of the image
		int covered = 0;
		for (size_t i = 0; i < i_DX11.m_Rgba.size(); i += 4)
			covered += (i_DX11.m_Rgba[i] | i_DX11.m_Rgba[i + 1] | i_DX11.m_Rgba[i + 2]) != 0;
		if (i_Options.m_bCheckCoverage)
			EXPECT_GT(covered, i_DX11.m_Width * i_DX11.m_Height / 20) << "DX11 drew almost nothing";

		const gtCompareResult result = gtCompareImages(i_DX11.m_Rgba.data(), i_Vulkan.m_Rgba.data(), i_DX11.m_Width, i_DX11.m_Height,
													   i_Options.m_MaxErr, 1 / 255.0f, i_Options.m_MaxPctOver);
		std::cout << "  " << i_Name << ": " << result.Describe() << std::endl;
		if (!result.m_bPass)
		{
			ADD_FAILURE() << result.Describe() << " (images in "
				<< gtSaveComparison(i_Name, i_DX11.m_Rgba.data(), i_Vulkan.m_Rgba.data(), i_DX11.m_Width, i_DX11.m_Height) << ")";
		}
	}
}

void gtExpectSameImage(const char* i_pTest, const gtSceneFunction& i_Build, const gtRenderOptions& i_Options)
{
	gtExpectSameImages(i_pTest, { gtCase{ std::string(), i_Build, i_Options } });
}

void gtExpectSameImages(const char* i_pTest, const std::vector<gtCase>& i_Cases)
{
	gtTempDir dir;
	std::vector<gtPicture> dx11(i_Cases.size()), vulkan(i_Cases.size());
	auto render_all = [&](std::vector<gtPicture>& o_Pictures)
	{
		for (size_t i = 0; i < i_Cases.size(); i++)
		{
			SCOPED_TRACE(i_Cases[i].m_Name);
			o_Pictures[i] = gtRenderScene(i_Cases[i].m_Build, dir.GetPath(), i_Cases[i].m_Options);
		}
	};
	gtBackend::Run(gtBackend::e_DX11, [&]() { render_all(dx11); });
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { render_all(vulkan); }))
		return;
	for (size_t i = 0; i < i_Cases.size(); i++)
	{
		const std::string name = i_Cases[i].m_Name.empty()
			? std::string(i_pTest) : std::string(i_pTest) + "/" + i_Cases[i].m_Name;
		compare(name, dx11[i], vulkan[i], i_Cases[i].m_Options);
	}
}
