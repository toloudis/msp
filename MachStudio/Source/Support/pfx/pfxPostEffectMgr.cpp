/*****************************************************************************
**	pfxPostEffectMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 20010 - All Rights Reserved
\****************************************************************************/
#include "Support/pfx/pfxPostEffectMgr.hpp"

#include "Support/pfx/pfxPostEffectObject.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/eff/effShaderParams.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/g3d/g3dPostProcessing.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gui/guiMessageBox.hpp"

#include <string>
#include <vector>


//============================================================================
//============================================================================
namespace pfxPostEffectMgr
{
	namespace
	{
		static std::vector<pfxPostEffectObject*> l_PfxObjects;
		const std::string l_DefaultPfxPath("PostEffect/Sepia.fx");
		const std::string l_DefaultPfxName("Sepia.fx");
	}

	//------------------------------------------------------------------------
	//  CleanUp
	//------------------------------------------------------------------------
	void  CleanUp()
	{
		if (!l_PfxObjects.empty())
		{
			delete l_PfxObjects[e_ViewportPfx];
			l_PfxObjects.clear();
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Init()
	{
		//	create the prefs objects if not already created
		//
		if (l_PfxObjects.size() == 0)
		{
			l_PfxObjects.resize(e_PfxNum);
			l_PfxObjects[e_ViewportPfx] = new pfxPostEffectObject(l_DefaultPfxName, l_DefaultPfxPath);
		}
		/*shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("PostEffect/BlackAndWhite.fx"), matShaderMgr::GetSpecialEffect("BlackAndWhite.fx"));
		g3dPostProcessing::SetActive(true);
		g3dPostProcessing::SetPostEffect(p);*/
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const std::string& GetDefaultPfxName()
	{
		return l_DefaultPfxName;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const std::string& GetDefaultPfxPath()
	{
		return l_DefaultPfxPath;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ApplyPostEffect(pfx_Object i_ObjectID)
	{
		if (l_PfxObjects[i_ObjectID])
			l_PfxObjects[i_ObjectID]->ApplyPostEffect();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtyObject* GetDataObject(pfx_Object i_ObjectID)
	{
		return l_PfxObjects[i_ObjectID];
	}
}
