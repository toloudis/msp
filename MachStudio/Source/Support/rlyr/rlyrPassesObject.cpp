/********************************************************************************************\
**  rlyrPassesObject.cpp
**
**		See .hpp for details
**
**  studio|gpu
\********************************************************************************************/
#include "Support/rlyr/rlyrPassesObject.hpp"

#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"

//------------------------------------------------------------------------
//------------------------------------------------------------------------
rlyrPassesObject::rlyrPassesObject()
{
	Init();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
rlyrPassesObject::rlyrPassesObject(const rlyrPassesData& i_Data)
{
	m_Data = i_Data;
	Init();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrPassesObject::Init()
{
	prtyCheckBoxUIInfo* pPUII = NULL;

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_Beauty), "Passes", "Beauty pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_Diffuse), "Passes", "Diffuse pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_DiffEnv), "Passes", "Diffuse Environment pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_DiffLit), "Passes", "Diffuse Lights pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_Specular), "Passes", "Specular pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_SpecEnv), "Passes", "Specular Environment pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_SpecLit), "Passes", "Specular Lights pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_Emissive), "Passes", "Emissive pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_AOOnly), "Passes", "Ambient Occlusion pass");
	AddProperty( pPUII );
	
	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_GI), "Passes", "Global Illumination pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_Depth), "Passes", "Depth pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_ShadowMask), "Passes", "Shadow mask pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_IlluminationOnly), "Passes", "Illumination only pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_Normals), "Passes", "Normals pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_DirtyMatte), "Passes", "Dirty matte pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_Wireframe), "Passes", "Wireframe pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_Materials), "Passes", "Materials pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_ReflectionsOnly), "Passes", "Reflections pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_Velocity), "Passes", "Velocity pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_Bloom), "Passes", "Bloom pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_Star), "Passes", "Star pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_CameraDOF), "Passes", "Camera DOF pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_Preview), "Passes", "Preview pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_Glow), "Passes", "Glow pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_RmanColorBleed), "Passes", "Color Bleed pass");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_MrayFinalGather), "Passes", "Final Gather pass");
	AddProperty( pPUII );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrPassesObject::CollectPasses(std::vector<rlyrPassesObject::ePassType>& o_Passes)
{
	if (m_Data.m_Beauty.GetValue())
		o_Passes.push_back(e_Beauty);
	if (m_Data.m_Diffuse.GetValue())
		o_Passes.push_back(e_Diffuse);
	if (m_Data.m_Specular.GetValue())
		o_Passes.push_back(e_Specular);
	if (m_Data.m_AOOnly.GetValue())
		o_Passes.push_back(e_AOOnly);
	if (m_Data.m_GI.GetValue())
		o_Passes.push_back(e_GlobalIllumination);
	if (m_Data.m_Depth.GetValue())
		o_Passes.push_back(e_Depth);
	if (m_Data.m_ShadowMask.GetValue())
		o_Passes.push_back(e_ShadowMask);
	if (m_Data.m_IlluminationOnly.GetValue())
		o_Passes.push_back(e_IlluminationOnly);
	if (m_Data.m_Normals.GetValue())
		o_Passes.push_back(e_Normals);
	if (m_Data.m_DirtyMatte.GetValue())
		o_Passes.push_back(e_DirtyMatte);
	if (m_Data.m_Wireframe.GetValue())
		o_Passes.push_back(e_Wireframe);
	if (m_Data.m_Materials.GetValue())
		o_Passes.push_back(e_Materials);
	if (m_Data.m_ReflectionsOnly.GetValue())
		o_Passes.push_back(e_ReflectionsOnly);
	if (m_Data.m_Velocity.GetValue())
		o_Passes.push_back(e_Velocity);
	if (m_Data.m_Bloom.GetValue())
		o_Passes.push_back(e_Bloom);
	if (m_Data.m_Star.GetValue())
		o_Passes.push_back(e_Star);
	if (m_Data.m_CameraDOF.GetValue())
		o_Passes.push_back(e_CameraDOF);
	if (m_Data.m_Preview.GetValue())
		o_Passes.push_back(e_Preview);
	if (m_Data.m_Emissive.GetValue())
		o_Passes.push_back(e_Emissive);
	if (m_Data.m_SpecEnv.GetValue())
		o_Passes.push_back(e_SpecEnv);
	if (m_Data.m_SpecLit.GetValue())
		o_Passes.push_back(e_SpecLit);
	if (m_Data.m_DiffEnv.GetValue())
		o_Passes.push_back(e_DiffEnv);
	if (m_Data.m_DiffLit.GetValue())
		o_Passes.push_back(e_DiffLit);
	if (m_Data.m_Glow.GetValue())
		o_Passes.push_back(e_Glow);
	if (m_Data.m_RmanColorBleed.GetValue())
		o_Passes.push_back(e_RmanColorBleed);
	if (m_Data.m_MrayFinalGather.GetValue())
		o_Passes.push_back(e_MrayFinalGather);
}

//--------------------------------------------------------------------
// Modify prefs flags relevant to the given pass type
//--------------------------------------------------------------------
void rlyrPassesObject::AdjustPrefs(ePassType i_Pass, g3dPrefs::g3dRenderPrefs& o_Prefs)
{
	switch (i_Pass)
	{
	case e_Beauty:
		o_Prefs.m_bEnableGlow = true;
		o_Prefs.m_bEnableDiffuseLighting = true;
		o_Prefs.m_bEnableSpecularLighting = true;
		o_Prefs.m_bEnableEnvironment = true;
		o_Prefs.m_bEnableLitPass = true;
		//o_Prefs.m_bEnableSSAO = false;
		//o_Prefs.m_bEnableSSGI = false;
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_HDR;
		break;
	case e_Diffuse:
		o_Prefs.m_bEnableGlow = false;
		o_Prefs.m_bEnableDiffuseLighting = true;
		o_Prefs.m_bEnableSpecularLighting = false;
		o_Prefs.m_bEnableEnvironment = true;
		o_Prefs.m_bEnableLitPass = true;
		o_Prefs.m_bEnableReflection = false;
		o_Prefs.m_bEnableSSAO = false;
		o_Prefs.m_bEnableSSGI = false;
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_HDR;
		break;
	case e_DiffEnv:
		o_Prefs.m_bEnableGlow = false;
		o_Prefs.m_bEnableDiffuseLighting = true;
		o_Prefs.m_bEnableSpecularLighting = false;
		o_Prefs.m_bEnableEnvironment = true;
		o_Prefs.m_bEnableLitPass = false;
		o_Prefs.m_bEnableReflection = false;
		o_Prefs.m_bEnableSSAO = false;
		o_Prefs.m_bEnableSSGI = false;
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_HDR;
		break;
	case e_DiffLit:
		o_Prefs.m_bEnableGlow = false;
		o_Prefs.m_bEnableDiffuseLighting = true;
		o_Prefs.m_bEnableSpecularLighting = false;
		o_Prefs.m_bEnableEnvironment = false;
		o_Prefs.m_bEnableLitPass = true;
		o_Prefs.m_bEnableReflection = false;
		o_Prefs.m_bEnableSSAO = false;
		o_Prefs.m_bEnableSSGI = false;
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_HDR;
		break;
	case e_Specular:
		o_Prefs.m_bEnableGlow = false;
		o_Prefs.m_bEnableDiffuseLighting = false;
		o_Prefs.m_bEnableSpecularLighting = true;
		o_Prefs.m_bEnableEnvironment = true;
		o_Prefs.m_bEnableLitPass = true;
		o_Prefs.m_bEnableReflection = false;
		o_Prefs.m_bEnableSSAO = false;
		o_Prefs.m_bEnableSSGI = false;
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_HDR;
		break;
	case e_SpecEnv:
		o_Prefs.m_bEnableGlow = false;
		o_Prefs.m_bEnableDiffuseLighting = false;
		o_Prefs.m_bEnableSpecularLighting = true;
		o_Prefs.m_bEnableEnvironment = true;
		o_Prefs.m_bEnableLitPass = false;
		o_Prefs.m_bEnableReflection = false;
		o_Prefs.m_bEnableSSAO = false;
		o_Prefs.m_bEnableSSGI = false;
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_HDR;
		break;
	case e_SpecLit:
		o_Prefs.m_bEnableGlow = false;
		o_Prefs.m_bEnableDiffuseLighting = false;
		o_Prefs.m_bEnableSpecularLighting = true;
		o_Prefs.m_bEnableEnvironment = false;
		o_Prefs.m_bEnableLitPass = true;
		o_Prefs.m_bEnableReflection = false;
		o_Prefs.m_bEnableSSAO = false;
		o_Prefs.m_bEnableSSGI = false;
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_HDR;
		break;
	case e_Emissive:
		o_Prefs.m_bEnableGlow = false;
		o_Prefs.m_bEnableDiffuseLighting = false;
		o_Prefs.m_bEnableSpecularLighting = false;
		o_Prefs.m_bEnableEnvironment = true;
		o_Prefs.m_bEnableLitPass = false;
		o_Prefs.m_bEnableReflection = false;
		o_Prefs.m_bEnableSSAO = false;
		o_Prefs.m_bEnableSSGI = false;
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_HDR;
		break;
	case e_AOOnly:
		o_Prefs.m_bEnableDiffuseLighting = false;
		o_Prefs.m_bEnableSpecularLighting = false;
		o_Prefs.m_bEnableEnvironment = false;
		o_Prefs.m_bEnableLitPass = false;
		o_Prefs.m_bEnableReflection = false;
		o_Prefs.m_bEnableSSAO = true;
		o_Prefs.m_bEnableSSGI = false;
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_AmbientOcclusion;
		break;
	case e_GlobalIllumination:
		o_Prefs.m_bMultipassOn = true;
		o_Prefs.m_bRenderMatte = false;
		o_Prefs.m_bRenderWireframe = false;
		o_Prefs.m_bEnableDiffuseLighting = true;
		o_Prefs.m_bEnableSpecularLighting = true;
		o_Prefs.m_bEnableEnvironment = true;
		o_Prefs.m_bEnableLitPass = true;
		o_Prefs.m_bEnableSSAO = false;
		o_Prefs.m_bEnableSSGI = true;
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_GlobalIllumination;
		break;
	case e_Depth:
		o_Prefs.m_bEnableSSAO = false;
		o_Prefs.m_bEnableSSGI = false;
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_Depth;
		break;
	case e_ShadowMask:
		o_Prefs.m_bEnableSSAO = false;
		o_Prefs.m_bEnableSSGI = false;
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_ShadowMask;
		break;
	case e_IlluminationOnly:
		o_Prefs.m_bEnableSSAO = false;
		o_Prefs.m_bEnableSSGI = false;
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_IlluminationOnly;
		break;
	case e_Normals:
		o_Prefs.m_bEnableSSAO = false;
		o_Prefs.m_bEnableSSGI = false;
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_Normals;
		break;
	case e_DirtyMatte:
		o_Prefs.m_bRenderMatte = true;
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_DirtyMatte;
		break;
	case e_Wireframe:
		o_Prefs.m_bRenderWireframe = true;
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_Wireframe;
		break;
	case e_Materials:
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_Materials;
		break;
	case e_ReflectionsOnly:
		o_Prefs.m_bEnableSSAO = false;
		o_Prefs.m_bEnableSSGI = false;
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_ReflectionOnly;
		break;
	case e_Velocity:
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_VelocityMap;
		break;
	case e_Bloom:
		o_Prefs.m_HDRDebugMode = 8;
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_HDR;
		break;
	case e_Star:
		o_Prefs.m_HDRDebugMode = 9;
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_HDR;
		break;
	case e_CameraDOF:
		o_Prefs.m_HDRDebugMode = 4;
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_HDR;
		break;
	case e_Preview:
		o_Prefs.m_bEnableGlow = false;
		o_Prefs.m_bMultipassOn = false;
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_HDR;
		break;
	case e_Glow:
		o_Prefs.m_bEnableGlow = true;
		o_Prefs.m_bMultipassOn = true;
		o_Prefs.m_bEnableDiffuseLighting = false;
		o_Prefs.m_bEnableSpecularLighting = true;
		o_Prefs.m_bEnableEnvironment = false;
		o_Prefs.m_bEnableLitPass = true;
		o_Prefs.m_bEnableReflection = false;
		o_Prefs.m_bEnableSSAO = false;
		o_Prefs.m_bEnableSSGI = false;
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_Glow;
		break;
	case e_RmanColorBleed:
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_RmanColorBleed;
		break;
	case e_MrayFinalGather:
		o_Prefs.m_RendererType = g3dSceneRendererTypes::e_MrayFinalGather;
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rlyrPassesData rlyrPassesObject::GetPassesData()
{
	return m_Data;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrPassesObject::UpdateVisibility(int i_RenderEngine)
{
	// Make sure they're all visible
	m_Data.m_Beauty.SetVisible(true);
	m_Data.m_AOOnly.SetVisible(true);
	m_Data.m_Depth.SetVisible(true);
	m_Data.m_ShadowMask.SetVisible(true);
	m_Data.m_IlluminationOnly.SetVisible(true);
	m_Data.m_Normals.SetVisible(true);
	m_Data.m_DirtyMatte.SetVisible(true);
	m_Data.m_Wireframe.SetVisible(true);
	m_Data.m_Materials.SetVisible(true);
	m_Data.m_ReflectionsOnly.SetVisible(true);
	m_Data.m_Velocity.SetVisible(true);
	m_Data.m_Diffuse.SetVisible(true);
	m_Data.m_Specular.SetVisible(true);
	m_Data.m_Bloom.SetVisible(true);
	m_Data.m_Star.SetVisible(true);
	m_Data.m_CameraDOF.SetVisible(true);
	m_Data.m_Preview.SetVisible(true);
	m_Data.m_Emissive.SetVisible(true);
	m_Data.m_SpecEnv.SetVisible(true);
	m_Data.m_SpecLit.SetVisible(true);
	m_Data.m_DiffEnv.SetVisible(true);
	m_Data.m_DiffLit.SetVisible(true);
	m_Data.m_GI.SetVisible(true);
	m_Data.m_Glow.SetVisible(true);
	m_Data.m_RmanColorBleed.SetVisible(false);
	m_Data.m_MrayFinalGather.SetVisible(false);

	bool origColorBleedVal = m_Data.m_RmanColorBleed.GetValue();
	bool origFinalGatherVal = m_Data.m_MrayFinalGather.GetValue();
	m_Data.m_RmanColorBleed.SetValue(false);
	m_Data.m_MrayFinalGather.SetValue(false);


	// Mask out if rman chosen
	if ( i_RenderEngine == g3dSceneRenderEngineCreate::e_RmanPrman )
	{
		m_Data.m_DirtyMatte.SetValue(false);
		m_Data.m_Wireframe.SetValue(false);
		m_Data.m_Materials.SetValue(false);
		m_Data.m_Velocity.SetValue(false);
		m_Data.m_Bloom.SetValue(false);
		m_Data.m_Star.SetValue(false);
		m_Data.m_CameraDOF.SetValue(false);
		m_Data.m_Preview.SetValue(false);
		m_Data.m_Depth.SetValue(false);
		m_Data.m_Glow.SetValue(false);
		m_Data.m_GI.SetValue(false);
		m_Data.m_MrayFinalGather.SetValue(false);
		m_Data.m_RmanColorBleed.SetValue(origColorBleedVal);

		m_Data.m_DirtyMatte.SetVisible(false);
		m_Data.m_Wireframe.SetVisible(false);
		m_Data.m_Materials.SetVisible(false);
		m_Data.m_Velocity.SetVisible(false);
		m_Data.m_Bloom.SetVisible(false);
		m_Data.m_Star.SetVisible(false);
		m_Data.m_CameraDOF.SetVisible(false);
		m_Data.m_Preview.SetVisible(false);	
		m_Data.m_Depth.SetVisible(false);
		m_Data.m_Glow.SetVisible(false);		
		m_Data.m_GI.SetVisible(false);
		m_Data.m_RmanColorBleed.SetVisible(true);
		m_Data.m_MrayFinalGather.SetVisible(false);
	}

	// Mask out if mray chosen
	else if ( i_RenderEngine == g3dSceneRenderEngineCreate::e_MentalRay )
	{
		m_Data.m_Depth.SetValue(false);
		m_Data.m_DirtyMatte.SetValue(false);
		m_Data.m_Wireframe.SetValue(false);
		m_Data.m_Materials.SetValue(false);
		m_Data.m_Velocity.SetValue(false);
		m_Data.m_Bloom.SetValue(false);
		m_Data.m_Star.SetValue(false);
		m_Data.m_CameraDOF.SetValue(false);
		m_Data.m_Preview.SetValue(false);
		m_Data.m_Glow.SetValue(false);
		m_Data.m_GI.SetValue(false);
		m_Data.m_MrayFinalGather.SetValue(origFinalGatherVal);
		m_Data.m_RmanColorBleed.SetValue(false);

		m_Data.m_Depth.SetVisible(false);
		m_Data.m_DirtyMatte.SetVisible(false);
		m_Data.m_Wireframe.SetVisible(false);
		m_Data.m_Materials.SetVisible(false);
		m_Data.m_Velocity.SetVisible(false);
		m_Data.m_Bloom.SetVisible(false);
		m_Data.m_Star.SetVisible(false);
		m_Data.m_CameraDOF.SetVisible(false);
		m_Data.m_Preview.SetVisible(false);
		m_Data.m_Glow.SetVisible(false);		
		m_Data.m_GI.SetVisible(false);
		m_Data.m_RmanColorBleed.SetVisible(false);
		m_Data.m_MrayFinalGather.SetVisible(true);
	}
}
