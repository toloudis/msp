/*****************************************************************************
**	cptrRenderBakeDataUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderBakeDataUtil.hpp"

#include "Core/name/nameString.hpp"
#include "Features/RenderPrefs/rndrPrefsUtil.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Support/capt/captRenderOutputData.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/rlyr/rlyrPassesObject.hpp"
#include "Support/rlyr/rlyrRenderLayer.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"
#include "Support/rprf/rprfPrefsObject.hpp"


//============================================================================
//============================================================================
namespace cptrRenderBakeDataUtil
{
	namespace
	{
		cptrRenderBakeData l_Data;
		nameString l_RenderLayerName("BakeLayer");
		rlyrRenderLayer* l_RenderLayer = NULL;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Init()
	{
		if (!l_RenderLayer)
		{
			l_RenderLayer = rlyrRenderLayerMgr::AddHiddenRenderLayer(l_RenderLayerName);
		}
		captRenderOutputData& data = captRenderOutputDataUtil::Data();
		l_Data.m_OutputDir = data.m_OutputDirectoryRoot;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Cleanup()
	{
		l_RenderLayer = NULL;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ReadData( const fsLocator& i_ConfigFile,
					cptrRenderBakeData& o_Data )
	{
		//cptrRenderBatchDataParser::ReadData( i_ConfigFile, o_Data );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void WriteData( const fsLocator& i_ConfigFile,
					const cptrRenderBakeData& i_Data )
	{
		//cptrRenderBatchDataParser::WriteData(i_ConfigFile, i_Data);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateData(cptrRenderBakeData& i_NewData)
	{
		l_Data = i_NewData;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cptrRenderBakeData& Data()
	{
		return l_Data;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	rlyrRenderLayer* GetBakeLayer()
	{
		return l_RenderLayer;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	nameString GetBakeLayerName()
	{
		return l_RenderLayerName;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ApplyBakeDataOnRenderLayer()
	{
		rprfPrefsObject* prefObj = l_RenderLayer->GetRenderPrefs();
		
		prefObj->m_ActualPrefs.m_bEnableShadows = l_Data.m_bIsShadows.GetValue();
		prefObj->m_ActualPrefs.m_bEnableEnvironment = l_Data.m_bIsEnvironment.GetValue();
		prefObj->m_ActualPrefs.m_bEnableLitPass = l_Data.m_bIsLit.GetValue();
		prefObj->m_ActualPrefs.m_bHDRAA = l_Data.m_bHDRAA.GetValue();
		prefObj->m_ActualPrefs.m_bAOInvalid = l_Data.m_bIsAOVolume.GetValue();
		prefObj->m_ActualPrefs.m_bGIInvalid = l_Data.m_bIsLPVGI.GetValue();

		// Set the default renderpref
		// The render preferences in this layer is set to default value
		prefObj->m_ActualPrefs.m_bMultipassOn = true;
		rndrPrefsUtil::SetMultipassRendering(prefObj->m_ActualPrefs.m_bMultipassOn);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ApplyBakeDataOnRenderPasses()
	{
		rlyrPassesObject* passObj = l_RenderLayer->GetRenderPasses();

		passObj->m_Data.m_Beauty = l_Data.m_bIsBakeColor.GetValue();
		passObj->m_Data.m_Normals = l_Data.m_bIsBakeNormal.GetValue();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void AdjustRenderPref(rlyrPassesObject::ePassType type)
	{
		rprfPrefsObject* prefObj = l_RenderLayer->GetRenderPrefs();

		switch (type)
		{
		case rlyrPassesObject::e_Beauty:
			prefObj->m_ActualPrefs.m_RendererType = g3dSceneRendererTypes::e_HDR;
			break;
		case rlyrPassesObject::e_Normals:
			prefObj->m_ActualPrefs.m_RendererType = g3dSceneRendererTypes::e_Normals;
			break;
		}
	}

}	// end of namespace
