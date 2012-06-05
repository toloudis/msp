/****************************************************************************\
**  fgmtPropertyObject.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Support/fgmt/GUI/fgmtPropertyObject.hpp"

#include "Support/fgmt/fgmtFragmentData.hpp"
#include "Support/fgmt/fgmtScriptObject.hpp"
#include "Support/mnm/mnmThinkMgr.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"

#include "Core/Env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceFinderDir.hpp"
#include "Core/fs/fsResourceFinderJoin.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Graphics/eff/effOcclusionData.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Tool/gpx/gpxFragment.hpp"


//----------------------------------------------------------------------------
// Because of instancing, each fragment property object might control
// multiple g3dFragments
//----------------------------------------------------------------------------
fgmtPropertyObject::fgmtPropertyObject(const std::string& i_Name,
									   fgmtFragmentData& i_Data,
									   const std::vector<g3dFragment*> &i_Fragments,
									   const std::vector<g3dSceneNode*> &i_SceneNodes,
									   const fsLocator& i_TextureDir,
									   fgmtScriptObject* i_Parent)
:	m_Name(i_Name), m_Data(i_Data), m_SceneNodes(i_SceneNodes),
	m_pParent(i_Parent), m_BakedColorPath("Baked Color Texture Path"), m_BakedNormalPath("Baked Normal Texture Path")
//	m_Fragments(i_Fragments), m_pAOTexture(NULL), m_AOTextureLocator(i_TextureDir)
{
	const int num_frags = i_Fragments.size();
	m_FragmentProxies.resize( num_frags );
	for (int i=0; i<num_frags; ++i)
		m_FragmentProxies[i] = new gpxFragment(*i_Fragments[i]);

	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bCastsShadow), "Surface Flags", "Casts shadows") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bReceivesShadow), "Surface Flags", "Receives light from shadow sources") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bShadowHull), "Surface Flags", "Fragment is invisible, but casts shadow") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bDoubleSided), "Surface Flags", "Render both sides of triangles") );

	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseBakedTexture), "Baked Texture", "Enable baked texture");
	//pPUII->SetReadOnly(true);
	AddProperty( pPUII);

	/*m_BakedColorPathUIInfo.reset(new prtyTextBoxUIInfo(&(m_BakedColorPath), "Baked Texture", "Baked Color Texture Path"));
	m_BakedColorPathUIInfo->SetReadOnly(true);
	m_BakedNormalPathUIInfo.reset(new prtyTextBoxUIInfo(&(m_BakedNormalPath), "Baked Texture", "Baked Normal Texture Path"));
	m_BakedNormalPathUIInfo->SetReadOnly(true);*/
/*
	prtyRangedFloatUIInfo* pRFUII = NULL;
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_AOBlendFactor), "Ambient Occlusion", "Blend Factor");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(100);
	AddProperty( pRFUII );

	prtyFileChooserUIInfo* pFCUII = new prtyFileChooserUIInfo(&(m_Data.m_AOUserTextureName), "Ambient Occlusion", "User texture");
	pFCUII->SetInitialDirectory( i_TextureDir );
	AddProperty( pFCUII );
*/
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bAOInherit), "Ambient Occlusion", "Inherit from global settings"));
//	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bIsOccluder), "Ambient Occlusion", "Set as occluder for all AO computations."));
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bReceivesOcclusion), "Ambient Occlusion", "Set to have AO computed for this character"));
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bReceivesGI), "Global Illumination", "Set to have GI computed for this character"));
/*
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bStaticAO), "Ambient Occlusion", "Do not compute per frame. Reuse initial result"));
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_nSamples), "Ambient Occlusion", "Number of samples (higher=better,slower)");
	AddProperty( pPUII );
	prtyComboBoxUIInfo* pCBUII;
	pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_SamplingResolution), "Ambient Occlusion", "Resolution of sample texture (higher=better, slower)");
	pCBUII->AddItem(std::string("256"), 0);
	pCBUII->AddItem(std::string("512"), 1);
	pCBUII->AddItem(std::string("1024"), 2);
	pCBUII->AddItem(std::string("2048"), 3);
	pCBUII->AddItem(std::string("4096"), 4);
	AddProperty( pCBUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_DepthBias), "Ambient Occlusion", "Per-sample depth bias");
	pRFUII->SetMinimum(0);
	pRFUII->SetMaximum(0.5f);
	pRFUII->SetDecimalPlaces(3);
	pRFUII->SetNumTicks(100);
	AddProperty( pRFUII );
	pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_AOTextureResolution), "Ambient Occlusion", "Default resolution of AO target texture");
	pCBUII->AddItem(std::string("256"), 0);
	pCBUII->AddItem(std::string("512"), 1);
	pCBUII->AddItem(std::string("1024"), 2);
	pCBUII->AddItem(std::string("2048"), 3);
	pCBUII->AddItem(std::string("4096"), 4);
	AddProperty( pCBUII );
	pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_AODistanceCutoff), "Ambient Occlusion", "Distances greater will not occlude");
	AddProperty( pPUII );
*/
	m_Data.m_bCastsShadow.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateFlags));
	m_Data.m_bReceivesShadow.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateFlags));
	m_Data.m_bShadowHull.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateFlags));
	m_Data.m_bDoubleSided.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateFlags));
	m_Data.m_bUseBakedTexture.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateBakeFlag));
	m_Data.m_BakedTextureFormat.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateBakeFlag));
	m_Data.m_BakedTextureLocation.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateBakeFlag));

//	m_Data.m_bIsOccluder.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateAOOccluder));
//	m_Data.m_bSiblingOccludeOnly.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateAOOccluder));
//	m_Data.m_bSelfOccludeOnly.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateAOOccluder));

//	m_Data.m_bAOInherit.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateAOData));
//	m_Data.m_nSamples.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateAOData));
//	m_Data.m_SamplingResolution.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateAOData));
//	m_Data.m_DepthBias.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateAOData));
//	m_Data.m_bStaticAO.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateAOData));
//	m_Data.m_AOBlendFactor.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateAOData));
//	m_Data.m_AODistanceCutoff.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateAOData));

	m_Data.m_bReceivesOcclusion.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateAOTexture));
	m_Data.m_bReceivesGI.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateGITexture));
//	m_Data.m_AOTextureResolution.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateAOTexture));

//	m_Data.m_AOUserTextureName.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateAOTextureName));
}

fgmtPropertyObject::~fgmtPropertyObject()
{
	// delete the proxies
	envSTLHelpers::DeleteContainer(m_FragmentProxies);

	// by now the fragments are dead, along with their effocclusiondata.
	// we stored the ao textures here so they can be freed now.
	//matTextureMgr::ReleaseTexture(m_pAOTexture);
}


//============================================================================
// sel3dObject - virtual function overrides
//============================================================================

//--------------------------------------------------------------------
// Set Parent pointer to use when creating selectable property objects
//--------------------------------------------------------------------
//void fgmtPropertyObject::SetParent(sel3dObject* i_pParent)
//{
//	m_pParent = i_pParent;
//}

//------------------------------------------------------------------------
// Get parent object of this object in order to define relationships
//	between icons and their affected objects.
//------------------------------------------------------------------------
//virtual 
//sel3dObject* fgmtPropertyObject::GetParentObject() const
//{
//	return m_pParent;
//}

//------------------------------------------------------------------------
// Get the name of the object.
//------------------------------------------------------------------------
//virtual 
std::string fgmtPropertyObject::GetDisplayName() const
{
	return m_Name;	
}

//--------------------------------------------------------------------
//	GetWorldBox returns a box which completely encloses the fragment.
//--------------------------------------------------------------------
maAxisBox fgmtPropertyObject::GetWorldBox() const
{
	maAxisBox bbox;
	const int num_nodes = m_SceneNodes.size();
	for (int i=0; i<num_nodes; i++)
		bbox.Union( m_SceneNodes[i]->GetWorldBox() );
	return bbox;
}

//------------------------------------------------------------------------
// scene specific data folder where AO textures will be stored.
//------------------------------------------------------------------------
void fgmtPropertyObject::SetAOTextureLocator(const fsLocator& i_AOTextureLocator)
{
	m_AOTextureLocator = i_AOTextureLocator;
}

//------------------------------------------------------------------------
// model material texture folder where AO textures will be stored
//------------------------------------------------------------------------
void fgmtPropertyObject::SetModelTextureLocator(const fsLocator& i_ModelTextureLocator)
{
	m_ModelTextureLocator = i_ModelTextureLocator;
}

//------------------------------------------------------------------------
// model material texture folder where baked textures will be stored
//------------------------------------------------------------------------
void fgmtPropertyObject::SetBakedTextureLocator(const fsLocator& i_BakedTextureLocator,
												const std::string& i_BakedTextureFormat)
{
	//m_BakedTextureLocator = i_BakedTextureLocator;
	//m_BakedTextureFormat = i_BakedTextureFormat;
}

//----------------------------------------------------------------------------
// property callbacks
//----------------------------------------------------------------------------
void fgmtPropertyObject::UpdateFlags(prtyProperty *i_pProperty, bool i_bDirty)
{
	const int num_frags = m_FragmentProxies.size();
	for (int i=0; i<num_frags; i++)
	{
		m_FragmentProxies[i]->SetCastsShadow( m_Data.m_bCastsShadow.GetValue() );
		m_FragmentProxies[i]->SetReceivesShadow( m_Data.m_bReceivesShadow.GetValue() );
		m_FragmentProxies[i]->SetShadowHull( m_Data.m_bShadowHull.GetValue() );
		m_FragmentProxies[i]->SetDoubleSided( m_Data.m_bDoubleSided.GetValue() );
	}
}

void fgmtPropertyObject::UpdateBakeFlag(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (m_Data.m_bUseBakedTexture.GetValue())
	{
		if (m_BakedColorPathUIInfo.get())
			GetListContainer().Remove(shared_ptr<prtyPropertyUIInfo>(m_BakedColorPathUIInfo));
		if (m_BakedNormalPathUIInfo.get())
			GetListContainer().Remove(shared_ptr<prtyPropertyUIInfo>(m_BakedNormalPathUIInfo));

		m_BakedColorPathUIInfo.reset(new prtyTextBoxUIInfo(&(m_BakedColorPath), "Baked Texture", "Baked Color Texture Path"));
		m_BakedColorPathUIInfo->SetMultiline(true);
		//m_BakedColorPathUIInfo->SetReadOnly(true);
		m_BakedNormalPathUIInfo.reset(new prtyTextBoxUIInfo(&(m_BakedNormalPath), "Baked Texture", "Baked Normal Texture Path"));
		m_BakedNormalPathUIInfo->SetMultiline(true);
		//m_BakedNormalPathUIInfo->SetReadOnly(true);

		std::string color_path, normal_path, path, color_path_full, normal_path_full;
		m_BakedColorPath = "";
		m_BakedNormalPath = "";
		fsFileUtil::LocatorToANSIFilename(m_Data.m_BakedTextureLocation.GetValue(), path);
		for (int i = 0; i < m_SceneNodes.size(); i++)
		{
			color_path = path + std::string("\\") + m_SceneNodes[i]->GetBakeName() + std::string("_Baked_Color.") + m_Data.m_BakedTextureFormat.GetValue();
			normal_path = path + std::string("\\") + m_SceneNodes[i]->GetBakeName() + std::string("_Baked_Normals.") + m_Data.m_BakedTextureFormat.GetValue();
			if (fsFileUtil::FileExists(fsLocator(itString(color_path.c_str()))))
				color_path_full += color_path + std::string("\n");
			if (fsFileUtil::FileExists(fsLocator(itString(normal_path.c_str()))))
				normal_path_full += normal_path + std::string("\n");
		}
		m_BakedColorPath = color_path_full;
		m_BakedNormalPath = normal_path_full;
		AddProperty(m_BakedColorPathUIInfo);
		AddProperty(m_BakedNormalPathUIInfo);

		mnmThinkMgr::CallFunctionDelayed(&cmmObjectDialogUtil::UpdateDialog);
	}
	else
	{
		if (m_BakedColorPathUIInfo.get())
			GetListContainer().Remove(shared_ptr<prtyPropertyUIInfo>(m_BakedColorPathUIInfo));
		if (m_BakedNormalPathUIInfo.get())
			GetListContainer().Remove(shared_ptr<prtyPropertyUIInfo>(m_BakedNormalPathUIInfo));
		/*m_BakedColorPathUIInfo.reset();
		m_BakedNormalPathUIInfo.reset();*/

		mnmThinkMgr::CallFunctionDelayed(&cmmObjectDialogUtil::UpdateDialog);
		//RemoveProperty(m_BakedColorPathUIInfo);
		//RemoveProperty(m_BakedNormalPathUIInfo);
	}

	m_pParent->SwitchMaterial(m_Data.m_bUseBakedTexture.GetValue(), m_Name);
	const int num_frags = m_FragmentProxies.size();
	for (int i=0; i<num_frags; i++)
	{
		m_FragmentProxies[i]->SetUseBakedTexture(m_Data.m_bUseBakedTexture.GetValue());
	}
}

//void fgmtPropertyObject::UpdateAOOccluder(prtyProperty *i_pProperty, bool i_bDirty)
//{
//	// occluder status change means all fragments are potentially invalid.
////	if (!m_Data.m_bAOInherit.GetValue())
//	{
//		m_Fragments->SetCastsOcclusion( m_Data.m_bIsOccluder.GetValue() );
//		m_Fragments->GetOcclusionData()->m_bSelfOcclude = m_Data.m_bSelfOccludeOnly.GetValue();
//		m_Fragments->GetOcclusionData()->m_bSiblingOcclude = m_Data.m_bSiblingOccludeOnly.GetValue();
//
//		// tell the AO engine that all fragments really need recalc.
//		g3dPrefs::CurrentPrefs().m_bAOInvalid = true;
//	}
//}

//void fgmtPropertyObject::UpdateAOData(prtyProperty *i_pProperty, bool i_bDirty)
//{
//	m_Fragments->GetOcclusionData()->m_bInheritParent = m_Data.m_bAOInherit.GetValue();
////	if (!m_Data.m_bAOInherit.GetValue())
//	{
//		m_Fragments->GetOcclusionData()->m_nSamples = m_Data.m_nSamples.GetValue();
//		m_Fragments->GetOcclusionData()->m_SamplingResolution = m_Data.m_SamplingResolution.GetValue();
//		m_Fragments->GetOcclusionData()->m_DepthBias = m_Data.m_DepthBias.GetValue();
//		m_Fragments->GetOcclusionData()->m_BlendFactor = m_Data.m_AOBlendFactor.GetValue();
//		m_Fragments->GetOcclusionData()->m_bStatic = m_Data.m_bStaticAO.GetValue();
//		m_Fragments->GetOcclusionData()->m_DistanceCutoff = m_Data.m_AODistanceCutoff.GetValue();
//	}
//
//	// since something changed, invalidate ao computation for next ao calc pass.
//	//m_Fragments->GetOcclusionData()->m_bAOInvalid = true;
//}

//void fgmtPropertyObject::UpdateAOTextureName(prtyProperty *i_pProperty, bool i_bDirty)
//{
//	// look at both aotexture and aousertexture
//	// usertexture overrides ao recalc and aotexture.
//	itString newName = m_Data.m_AOUserTextureName.GetValue(); 
//	std::string sNewName = itStringUtil::GetStdString(newName);
//	std::string sOldName = m_Fragments->GetOcclusionData()->m_TextureName;
//	if (sNewName != sOldName)
//	{
//		m_Fragments->GetOcclusionData()->m_TextureName = sNewName;
//// disable baked AO textures in favor of SSAO
////		ResetAOTexture();
//		// if texture failed to load, then restore prior value.
//		if (m_pAOTexture == NULL)
//		{
//			DBG_WARNING("Could not load AO texture " << sNewName.c_str());
//			m_Fragments->GetOcclusionData()->m_TextureName = sOldName;
//
//			prtyFileName* pPrty = dynamic_cast<prtyFileName*>(i_pProperty);
//			DBG_ASSERT(pPrty != NULL, "Bad prty type for AO texture name");
//			DBG_ASSERT(pPrty == &(m_Data.m_AOUserTextureName), "Wrong prty address for AO texture name");
//			// This line isn't changing the shader value back in the
//			// user interface because it thinks the callback is from its local change.
//			pPrty->SetValue(itString(sOldName.c_str()));
//			// So, ask for a new refresh from the ObjectDialog
//			mnmThinkMgr::CallFunctionDelayed(&cmmObjectDialogUtil::UpdateDialog);
//		}
//	}
//
//	// since something changed, invalidate ao computation for next ao calc pass.
//	m_Fragments->GetOcclusionData()->m_bAOInvalid = true;
//}

void fgmtPropertyObject::UpdateAOTexture(prtyProperty *i_pProperty, bool i_bDirty)
{
//	if (!m_Data.m_bAOInherit.GetValue())
	{
		const int num_frags = m_FragmentProxies.size();
		for (int i=0; i<num_frags; i++)
		{
			m_FragmentProxies[i]->SetReceivesOcclusion( m_Data.m_bReceivesOcclusion.GetValue() );
//			m_FragmentProxies[i]->GetOcclusionData()->m_Resolution = m_Data.m_AOTextureResolution.GetValue();
// disable baked AO textures in favor of SSAO
//			ResetAOTexture();

			// since something changed, invalidate ao computation for next ao calc pass.
//			m_FragmentProxies[i]->GetOcclusionData()->m_bAOInvalid = true;
		}
	}
}

void fgmtPropertyObject::UpdateGITexture(prtyProperty *i_pProperty, bool i_bDirty)
{
//	if (!m_Data.m_bAOInherit.GetValue())
	{
		const int num_frags = m_FragmentProxies.size();
		for (int i=0; i<num_frags; i++)
		{
			m_FragmentProxies[i]->SetReceivesGI( m_Data.m_bReceivesGI.GetValue() );
//			m_FragmentProxies[i]->GetOcclusionData()->m_Resolution = m_Data.m_AOTextureResolution.GetValue();
// disable baked AO textures in favor of SSAO
//			ResetAOTexture();

			// since something changed, invalidate ao computation for next ao calc pass.
//			m_FragmentProxies[i]->GetOcclusionData()->m_bAOInvalid = true;
		}
	}
}

//--------------------------------------------------------------------
// delete and re-create AO texture with current settings
//--------------------------------------------------------------------
//void fgmtPropertyObject::ResetAOTexture()
//{
//	// remove old
//	DestroyAOTexture();
//
//	// create new
//	if (m_Fragments->GetReceivesOcclusion())
//	{
//		effOcclusionData* aodata = m_Fragments->GetOcclusionData();
//		// we just destroyed this texture so it had better be null.
//		DBG_ASSERT(aodata->m_TextureDiffuse == NULL, "nonnull ao texture!");
//
//		CreateAOTexture();
//		if (aodata->m_TextureName == "")
//		{
//			// make sure to invalidate for next recompute.
//			aodata->m_bAOInvalid = true;
//		}
//	}
//}
//
//void fgmtPropertyObject::DestroyAOTexture()
//{
//	effOcclusionData* aodata = m_Fragments->GetOcclusionData();
//
//	matTextureMgr::ReleaseTexture(aodata->m_TextureDiffuse);
//
//	aodata->RemoveTextures();
//	m_pAOTexture = NULL;
//}
//
//void fgmtPropertyObject::CreateAOTexture()
//{
//	effOcclusionData* aodata = m_Fragments->GetOcclusionData();
//
//	// prioritize: load texture from scene-specific first. if not there, check in model texture folder.
//	const bool bStrict = true; // needs to return false if file doesn't exist
//	fsResourceFinderDir sceneFinder(m_AOTextureLocator, bStrict);
//	fsResourceFinderDir modelFinder(m_ModelTextureLocator, bStrict);
//	fsResourceFinderJoin joinedFinder(&sceneFinder, &modelFinder);
//
//	// either find texture in file or create empty rendertarget.
//	aodata->ReloadTextures(joinedFinder);
//
//	// innate knowledge of effocclusiondata:
//	m_pAOTexture = aodata->m_TextureDiffuse;
//}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//fsLocator fgmtPropertyObject::LocateAOTexture(bool i_bSceneSpecificFolder)
//{
//	effOcclusionData* aodata = m_Fragments->GetOcclusionData();
//
//	// prioritize: load texture from scene-specific first. if not there, check in model texture folder.
//	const bool bStrict = true; // needs to return false if file doesn't exist
//	fsResourceFinderDir sceneFinder(m_AOTextureLocator, bStrict);
//	fsResourceFinderDir modelFinder(m_ModelTextureLocator, bStrict);
//	fsResourceFinderJoin joinedFinder(&sceneFinder, &modelFinder);
//
//	fsLocator found_loc;
//	if (joinedFinder.FindResource(m_Data.m_AOUserTextureName.GetValue(), found_loc))
//	{
//		return found_loc;
//	}
//
//	// Fallback on a guess where it should be	
//	fsLocator tex_loc = (i_bSceneSpecificFolder)? m_AOTextureLocator:m_ModelTextureLocator;
//	tex_loc.Push(m_Data.m_AOUserTextureName.GetValue());
//	return tex_loc;
//
//}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//void fgmtPropertyObject::CleanUp()
//{
//	DestroyAOTexture();
//}
