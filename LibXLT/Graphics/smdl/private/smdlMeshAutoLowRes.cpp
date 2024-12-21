/*****************************************************************************
**	smdlMeshAutoLowRes.hpp
**
**		smdlMeshAutoLowRes manages the visibility of a group of fragments
**	within the scene graph hierarchy.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/private/smdlMeshAutoLowRes.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maRotation.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mdl/mdlSkinInfo.hpp"
#include "Graphics/mdl/mdlSplitFragInfo.hpp"
#include "Graphics/smdl/private/smdlInfluenceSorter.hpp"
#include "Graphics/smdl/private/smdlSubdivNetwork.hpp"
#include "Graphics/smdl/smdlCharacterSkin.hpp"


//--------------------------------------------------------------------
//	smdlMeshAutoLowRes requires the root scene node of the skeleton.
//	It will insert new static meshes into the hierarchy based
//	on the skinned fragments and mark them low-resolution
//--------------------------------------------------------------------
smdlMeshAutoLowRes::smdlMeshAutoLowRes( g3dSceneNode *i_pRootJoint, 
										const std::vector<mdlSkinInfo>& i_SkinnedSurfaces )
:	m_bVisible(true), m_pLowResMat(NULL)
{
	DBG_ASSERT(!i_SkinnedSurfaces.empty(), "Need character skins for this test.");
	if (i_SkinnedSurfaces.empty())
		return;

	// Create a sorter for deciding which triangles 
	// are influenced by which joints
	smdlInfluenceSorter sorter( i_pRootJoint );

	const int num_skins = i_SkinnedSurfaces.size();
	for (int si=0; si<num_skins; ++si)
	{ 
		const mdlSkinInfo &skin_info = i_SkinnedSurfaces[si];
		if (skin_info.m_SkinInfo)
		{
			if (skin_info.m_MeshInfo)
			{
				// If the mesh has an undefined resolution level (0), 
				// then do auto low res.
				if (skin_info.m_MeshInfo->m_ResolutionLevel == 0)
					sorter.Submit(*skin_info.m_MeshInfo, *skin_info.m_SkinInfo);
			}
			else if (skin_info.m_SubdivInfo)
			{
				// Only do auto-gen of low res, if requested on export,
				// this also is set based on a resolution level of 0 when exporting.
				if (skin_info.m_SubdivInfo->m_Flags.m_bAutoGenLowRes)
				{
					smdlSubdivNetwork subdiv_network(*skin_info.m_SubdivInfo, 0, 0);
					mdlFragInfo subdiv_frag = subdiv_network.GetSubdivFragInfo();
					sorter.Submit(subdiv_frag, *skin_info.m_SkinInfo);
				}
			}
		}
	}

	// Create material for low-res models
	m_pLowResMat = new matMaterial("Phong.fx");
	effPhongData* pPhongData = dynamic_cast<effPhongData*>(m_pLowResMat->GetEffectData());
	if (pPhongData)
	{
		pPhongData->m_ColorEmissive.Set(0,0,0,1);
		pPhongData->m_ColorSpecular.Set(0,0,0,1);
		pPhongData->m_ColorAmbient.Set(0.1f, 0.1f, 0.1f, 1);
		pPhongData->m_ColorDiffuse.Set(0.7f, 0.7f, 0.7f, 1);
		pPhongData->m_Transparency = 1;
	}

	// Create static fragments directly into joint hierarchy
	sorter.CreateFragments(m_pLowResMat, m_Fragments, m_ParentNodes);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
smdlMeshAutoLowRes::~smdlMeshAutoLowRes()
{
	// Material and fragments are owned
	delete m_pLowResMat;
	envSTLHelpers::DeleteContainer(m_Fragments);
}

//--------------------------------------------------------------------
//	Visible - set/get whether the given surface is renderable
//--------------------------------------------------------------------
//virtual 
void smdlMeshAutoLowRes::SetVisible(bool i_bVisible)
{
	m_bVisible = i_bVisible;
	std::for_each(m_ParentNodes.begin(), m_ParentNodes.end(), 
		std::bind(std::mem_fn(&g3dSceneNode::SetRenderable), std::placeholders::_1, i_bVisible));
}
//virtual 
bool smdlMeshAutoLowRes::GetVisible() const
{
	return m_bVisible;
}

//--------------------------------------------------------------------
//	Returns true if some of the fragments in this group are
//	marked as the low-resolution model.
//--------------------------------------------------------------------
bool smdlMeshAutoLowRes::HasLowResolution() const
{
	return (!m_Fragments.empty());
}
