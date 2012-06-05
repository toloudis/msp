/*****************************************************************************
**	api3dAmbientOcclusion.cpp
**
**	Creates and owns lights
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Core/dbg/dbgLog.hpp"
#include "Core/dbg/dbgMsg.hpp"
#include "Graphics/eff/effOcclusionData.hpp"
#include "Graphics/g3d/g3dAmbientOcclusion.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dSceneTraverse.hpp"
#include "Tool/api3d/api3dAmbientOcclusion.hpp"
#include "Tool/gui/guiProgressDialog.hpp"


//============================================================================
//============================================================================
namespace api3dAmbientOcclusion
{

namespace
{
	//--------------------------------------------------------------------
	// collect relevant occlusion nodes into lists
	//--------------------------------------------------------------------
	class AONodeProcessor : public g3dSceneNodeProcessor
	{
	public:
		AONodeProcessor(){}
		~AONodeProcessor(){Clear();}

		// NOTE THESE ARE REFERENCES TO THE OWNER'S DATA PASSED TO CONSTRUCTOR
		nodeList m_aoNodes;
		nodeList m_occluderNodes;

		virtual bool ProcessNode(g3dSceneNode* i_pNode) 
		{
			// Check if node is renderable
			if( !i_pNode->GetRenderable() )
			{
				return false;
			}

			const g3dFragment* pFrag = i_pNode->GetFragment();

			// Check if it has geometry
			if( pFrag && !pFrag->IsShadowHull())
			{
				if (pFrag->GetCastsOcclusion())
					m_occluderNodes.push_back(i_pNode);
				if (pFrag->GetReceivesOcclusion())
					m_aoNodes.push_back(i_pNode);
			}
			return true;
		}

		void Clear()
		{
			m_aoNodes.clear();
			m_occluderNodes.clear();
		}
	};
	AONodeProcessor l_SceneDatabase;

}	// end of namespace


//--------------------------------------------------------------------
// Initialize
//--------------------------------------------------------------------
void Initialize()
{
}

//--------------------------------------------------------------------
// DeInitialize
//--------------------------------------------------------------------
void DeInitialize()
{
}

//--------------------------------------------------------------------
//  Compute AO
//--------------------------------------------------------------------
void  ComputeAmbientOcclusion(g3dSceneNode* i_Root)
{
	// gather scene graph elements into sorted lists
	l_SceneDatabase.Clear();
	g3dSceneTraverse::Traverse(i_Root, &l_SceneDatabase);

	const g3dSceneNode* pNode = NULL;
	const g3dFragment* pFrag = NULL;

	int n = l_SceneDatabase.m_aoNodes.size();
	guiProgressDialog::Show("Ambient Occlusion", "Computing occlusion for fragments...");
	for (int i = 0; i < n; i++)
	{
		if (!guiProgressDialog::SetPercentage((float)i/n))
			break;

		pNode = l_SceneDatabase.m_aoNodes[i];
		pFrag = pNode->GetFragment();

		DBG_ASSERT0(pFrag->GetReceivesOcclusion(), "Fragment is not an occlusion receiver.")
		if (pFrag->GetOcclusionData()->m_TextureName == "")
		{
			// either the fragment is designated for a one-time recalc or else we must be doing a global recalc.
			if (g3dPrefs::CurrentPrefs().m_bAOInvalid || pFrag->GetOcclusionData()->m_bAOInvalid) 
			{
				//DBG_LOG2("Computing node %04d of %04d AO receivers", i+1,n);
				g3dAmbientOcclusion::RenderOcclusionToTexture(pNode, l_SceneDatabase.m_occluderNodes);
			}
			else
			{
				//DBG_LOG("  Node is valid. skipped.");
			}
		}
		else
		{
			//DBG_LOG("  Node has user texture. skipped.");
		}
	}
	guiProgressDialog::Hide();

	g3dAmbientOcclusion::ReleaseResources();

	// we're done: ao state is valid
	g3dPrefs::CurrentPrefs().m_bAOInvalid = false;
	// we're done: recalc is not requested.
	g3dPrefs::CurrentPrefs().m_RecalcAORequested = false;
}

}	// end of namespace
