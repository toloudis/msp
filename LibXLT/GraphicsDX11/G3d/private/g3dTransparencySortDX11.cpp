/****************************************************************************\
**	g3dTransparencySortDX11.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g3d/g3dTransparencySortDX11.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
//#include "GraphicsDX11/g3d/g3dFogDX11.hpp"	//DX11 port temp disable
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "GraphicsDX11/G3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/G3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"

#include <vector>
#include <algorithm>

namespace
{
	g3dBlendStateMgr::BlendState* stp_NoBlend = NULL;
	g3dBlendStateMgr::BlendState* stp_Blend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* dsp_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* dsp_Test_LessE_NS = NULL;
}

bool g3dTransparencySortDX11::s_bDoSplitting = false;
bool g3dTransparencySortDX11::s_bDoExtraAlphaTestPass = false;
unsigned int g3dTransparencySortDX11::s_alphaRef = 0x000000ff;


g3dTransparencySortDX11::g3dTransparencySortDX11()
:	l_nTranspNodes(0)
{
}
g3dTransparencySortDX11::~g3dTransparencySortDX11()
{
	ClearTransparentNodes();
}

//------------------------------------------------------------------------
//	AddDeferredNode - stores the transparent node for sorting and
//		rendering when RenderTransparentNodes() is called later.
//------------------------------------------------------------------------
void g3dTransparencySortDX11::AddDeferredNode( g3dSceneNode* i_pNode, 
												 const maPoint3d &i_CameraPos,
												 g3dRenderStateCache i_CacheState)
{
	// Calculate the the distance to the camera for sorting
	const maMatrix4x4& total_transform = i_pNode->GetTotalTransform();

	maPoint3d position = i_pNode->GetFragment()->GetBoundingBox().GetCenter();
	total_transform.Transform( position );

//	maPoint4d p4(position.GetX(), position.GetY(), position.GetZ(), 1);
//	g3dSceneGlobal::g_Camera.Transform(p4);

	l_TranspNode.m_fDistance = ( position - i_CameraPos ).LengthSqr();
	l_TranspNode.m_pSceneNode = i_pNode;
	l_TranspNode.m_RenderStateCache = i_CacheState;

	if (i_pNode->GetFragment()->GetComponentSort())
	{
		// FIX: [bga] - const cast in order to sort the components of the fragments.
		// This could maybe be a const implementation that uses "mutable"?
		// Or, this could all be implemented in a way that returns indices 
		// instead of altering the existing ones?
		g3dFragment* pFragment = const_cast<g3dFragment*>(i_pNode->GetFragment());
		pFragment->ComponentSort(total_transform, i_CameraPos);
	}


	// Store the node info for rendering later
	l_DeferredNodes.push_back( l_TranspNode );
}

//------------------------------------------------------------------------
//	AddTransparentNode - stores the transparent node for sorting and
//		rendering when RenderTransparentNodes() is called later.
//------------------------------------------------------------------------
void g3dTransparencySortDX11::AddTransparentNode( g3dSceneNode* i_pNode, 
												 const maPoint3d &i_CameraPos,
												 g3dRenderStateCache i_CacheState,
												 bool i_bIsMask)
{		
	// Calculate the the distance to the camera for sorting
	const maMatrix4x4& total_transform = i_pNode->GetTotalTransform();

	maPoint3d position = i_pNode->GetFragment()->GetBoundingBox().GetCenter();
	total_transform.Transform( position );

//	maPoint4d p4(position.GetX(), position.GetY(), position.GetZ(), 1);
//	g3dSceneGlobal::g_Camera.Transform(p4);

	l_TranspNode.m_fDistance = ( position - i_CameraPos ).LengthSqr();
	l_TranspNode.m_pSceneNode = i_pNode;
	l_TranspNode.m_RenderStateCache = i_CacheState;
	l_TranspNode.m_bIsMask = i_bIsMask;

	if (i_pNode->GetFragment()->GetComponentSort())
	{
		// FIX: [bga] - const cast in order to sort the components of the fragments.
		// This could maybe be a const implementation that uses "mutable"?
		// Or, this could all be implemented in a way that returns indices 
		// instead of altering the existing ones?
		g3dFragment* pFragment = const_cast<g3dFragment*>(i_pNode->GetFragment());
		pFragment->ComponentSort(total_transform, i_CameraPos);
	}


	// Store the node info for rendering later
	if (!i_bIsMask)
		l_TranspNodes.push_back( l_TranspNode );
	else
		l_TranspMaskNodes.push_back( l_TranspNode );

	l_TranspPlusMaskNodes.push_back( l_TranspNode );
}


//------------------------------------------------------------------------
//	RenderTransparentNodes - renders the nodes added in calls
//	to AddTransparentNode() since the last call to this function.
//  The nodes are sorted and D3D states are set appropriately.
//  At the end of this function, the node list is cleared.
//------------------------------------------------------------------------
int g3dTransparencySortDX11::RenderTransparentNodes( const g3dRenderStateTraverser& i_StateTraverser )
{
	int nTriangles = 0;

	TranspNodeVector& transpNodes = (s_bDoSplitting) ? l_SortedTranspSubNodes : l_TranspNodes;
	TranspNodeVector::const_iterator it, end = transpNodes.end();
	for (it = transpNodes.begin(); it != end; ++it)
	{
		i_StateTraverser.SetRenderState( it->m_RenderStateCache );
//		g3dFogDX11::EnableFog( it->m_pSceneNode->GetFogged() );		//DX11 port temp disable
		g3dDrawStyleUtilDX11::SetDrawStyle( it->m_pSceneNode->GetDrawStyle() );
		nTriangles += g3dSceneRenderUtil::DrawNodeAmbient(it->m_pSceneNode, 
			i_StateTraverser.GetEnvironmentState( it->m_RenderStateCache ));
	}

	g3dDrawStyleUtilDX11::RestoreNormalDrawStyle();
	
	{//PROFILE("debug");
		// Display debug information for number of triangles
		//char num[64];
		//sprintf(num, "transp nodes: %d", l_nTranspNodes );
		//g2dScreen::SetDebugInfo(3, num );
	}

	return nTriangles;
}

//------------------------------------------------------------------------
// Quick check for trivial rejection
//------------------------------------------------------------------------
bool g3dTransparencySortDX11::HaveTransparentNodes()
{
	return (!l_TranspNodes.empty());
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void g3dTransparencySortDX11::SortTransparentNodes(bool i_bIsIncludeMask)
{
	TranspNodeVector& targetNodes =  i_bIsIncludeMask ? l_TranspPlusMaskNodes : l_TranspNodes;

	std::sort( targetNodes.begin(), targetNodes.end() );

	// check bounding boxes of consecutive triangle-sorted nodes for overlap
	// if overlap is found, split node fragment into sub-fragments based on bounding box overlap
	// later on, the render loop will analyze subfragments and intersperse them in depth order
	if (!s_bDoSplitting)
		return;

	int nTransNodes = targetNodes.size();
	if (nTransNodes < 2)
	{
		if (nTransNodes == 1)
			MakeSubNode(targetNodes[0].m_pSceneNode, targetNodes[0].m_RenderStateCache,
				targetNodes[0].m_fDistance, 0, l_SortedTranspSubNodes);
		return;
	}

	maAxisBox wldBoxCur, wldBoxLast, camBoxCur, camBoxLast;

	// empty out the old subnodes
	int i;
	for (i = 0; i < l_SortedTranspSubNodes.size(); i++)
	{
		delete l_SortedTranspSubNodes[i].m_pSceneNode;
	}
	l_SortedTranspSubNodes.clear();

	std::vector<bool> hasBeenSplit(nTransNodes);
	for (i = 0; i < nTransNodes; i++)
		hasBeenSplit[i]=false;

	g3dSceneNode* curNode = NULL;
	g3dSceneNode* lastNode = targetNodes[0].m_pSceneNode;
	for (i = 1; i < nTransNodes; i++, lastNode=curNode, camBoxLast=camBoxCur, wldBoxLast=wldBoxCur)
	{
		curNode = targetNodes[i].m_pSceneNode;

		DBG_ASSERT(curNode != NULL, "NULL node in transparency sort");
		DBG_ASSERT(lastNode != NULL, "NULL node in transparency sort");

		wldBoxCur = curNode->GetWorldBox();
		camBoxCur = g3dDX11Util::XForm(wldBoxCur, g3dSceneGlobal::GetCameraTransform());

		if (curNode->GetFragment()->GetComponentSort() && 
			lastNode->GetFragment()->GetComponentSort())
		{
			if (camBoxCur.Overlaps(camBoxLast))
			{
				maAxisBox camBoxIsect;
				int isec = camBoxCur.GetIntersection(camBoxLast, camBoxIsect);

				// just split both fragments, unless they are already split!
				if (!hasBeenSplit[i-1])
				{
					lastNode->GetFragment()->Split(camBoxIsect);
					MakeSubNodes(lastNode, targetNodes[i-1].m_RenderStateCache, l_SortedTranspSubNodes);
					hasBeenSplit[i-1]=true;
				}
				if (!hasBeenSplit[i])
				{
					curNode->GetFragment()->Split(camBoxIsect);
					MakeSubNodes(curNode, targetNodes[i].m_RenderStateCache, l_SortedTranspSubNodes);
					hasBeenSplit[i]=true;
				}

			}
		}
	}
	// now add in any remaining trans nodes that aren't already split
	for (i = 0; i < nTransNodes; i++)
	{
		if (!hasBeenSplit[i])
		{
			DBG_ASSERT(targetNodes[i].m_pSceneNode->GetFragment()->GetSubFragments().size() < 1, "BAD SUBFRAGMENT COUNT, expected 0");
			MakeSubNode(targetNodes[i].m_pSceneNode, targetNodes[i].m_RenderStateCache,
				targetNodes[i].m_fDistance, 0, l_SortedTranspSubNodes);
		}
	}
	std::sort( l_SortedTranspSubNodes.begin(), l_SortedTranspSubNodes.end() );

}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void g3dTransparencySortDX11::ClearTransparentNodes()
{
	l_TranspNodes.clear();
	l_TranspMaskNodes.clear();
	l_TranspPlusMaskNodes.clear();
	l_DeferredNodes.clear();

	if (s_bDoSplitting)
	{
		for (int i = 0; i < l_SortedTranspSubNodes.size(); i++)
		{
			l_SortedTranspSubNodes[i].m_pSceneNode->GetFragment()->SubFragments().clear();
			delete l_SortedTranspSubNodes[i].m_pSceneNode;
		}
		l_SortedTranspSubNodes.clear();
	}
}

//------------------------------------------------------------------------
// As an alternative to RenderNoSortNoClear, you can ask to have a
// function called on each node and do your own rendering.
// Default behavior would call g3dRendererMgr::Render( i_pSceneNode )
// on each node..
//------------------------------------------------------------------------
int g3dTransparencySortDX11::RenderUserFunc(RenderTransparentNodeFunc i_OpaqueFunction, 
										   RenderTransparentNodeFunc i_Function, 
											const g3dRenderStateTraverser& i_StateTraverser)
{
	int nTriangles = 0;
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"g3dTransparencySortDX11::RenderUserFunc" );

	TranspNodeVector& transpNodes = (s_bDoSplitting) ? l_SortedTranspSubNodes : l_TranspNodes;
	std::vector<TranspNode>::iterator it, end = transpNodes.end();

	for (it = transpNodes.begin(); it != end; ++it)
	{
//		g3dFogDX11::EnableFog( it->m_pSceneNode->GetFogged() );		//dx11 port temp disable
		nTriangles += (*i_Function)(it->m_pSceneNode, it->m_RenderStateCache, 
			i_StateTraverser);
	}

	D3DPERF_EndEvent();
	return nTriangles;
}
int g3dTransparencySortDX11::RenderUserFuncDeferred(RenderTransparentNodeFunc i_Function, 
											const g3dRenderStateTraverser& i_StateTraverser)
{
	int nTriangles = 0;
	TranspNodeVector& transpNodes = (s_bDoSplitting) ? l_SortedTranspSubNodes : l_TranspNodes;
	std::vector<TranspNode>::iterator it, end = transpNodes.end();
	// lastly, if we have deferred nodes:
	// fill z buffer with the transp nodes just drawn.
	// then draw deferred nodes.
	if ( g3dPrefs::CurrentPrefs().m_bEnableDeferredTransparency && !l_DeferredNodes.empty() )
	{

		// 1. redraw transp nodes with z write and no color write.
		if (!g3dSingleLightRendering::GetDoDOFPrepPass())
		{
			g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_LessE_NS );
		
			// This chunk of blendstate setting has never been tested
			g3dBlendStateMgr::SetBlendState( stp_NoBlend );

			for (it = transpNodes.begin(); it != end; ++it)
			{
				nTriangles += (*i_Function)(it->m_pSceneNode, it->m_RenderStateCache, i_StateTraverser);
			}

			g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_LessE_NS );
			// 2. restore color writes and prepare to draw deferred nodes.

// This chunk of blendstate setting has never been tested
			g3dBlendStateMgr::SetBlendState( stp_Blend );
		}

		std::vector<TranspNode>::iterator itP, endP = l_DeferredNodes.end();
		for (itP = l_DeferredNodes.begin(); itP != endP; ++itP)
		{
            nTriangles += (*i_Function)(itP->m_pSceneNode, itP->m_RenderStateCache, i_StateTraverser);
		}
	}

	return nTriangles;
}

const TranspNodeVector& g3dTransparencySortDX11::GetTransparentNodes(bool i_bIsIncludeMask)
{
	if (i_bIsIncludeMask)
		return l_TranspPlusMaskNodes;
	return l_TranspNodes;
}

const TranspNodeVector& g3dTransparencySortDX11::GetTransparentMaskNodes()
{
	return l_TranspMaskNodes;
}

void g3dTransparencySortDX11::MakeSubNodes(g3dSceneNode* i_curNode, g3dRenderStateCache i_CacheState, 
										  std::vector<TranspNode>& o_subNodes)
{
	// Calculate the the distance to the camera for sorting
	const maMatrix4x4& total_transform = i_curNode->GetTotalTransform();

	// make transpnodes out of the sub-fragment scenenodes... then they can be re-sorted with the overlapping subnodes.
	g3dFragment* frag = i_curNode->GetFragment();
	const std::vector<g3dFragment::sSubFrag>& subFragments = frag->GetSubFragments();
	for (int i = 0; i < subFragments.size(); i++)
	{
		maPoint3d position = subFragments[i].m_bounds.GetCenter();
		total_transform.Transform( position );

		float dist = ( position - g3dSceneGlobal::GetCameraPos() ).LengthSqr();
		MakeSubNode(i_curNode, i_CacheState, dist, i, o_subNodes);
	}
}

// make one subnode from a given fragment
void g3dTransparencySortDX11::MakeSubNode(g3dSceneNode* i_curNode, g3dRenderStateCache i_CacheState, 
										 float i_dist, int iSubNode, 
										 std::vector<TranspNode>& o_subNodes)
{
	g3dFragment* frag = i_curNode->GetFragment();
	DBG_ASSERT(iSubNode <= frag->GetSubFragments().size(), "Bad subnode index in transparency sort MakeSubNode");

	g3dSceneNode* node = new g3dSceneNode(frag, i_curNode->GetTransform(), iSubNode);

	l_TranspNode.m_fDistance = i_dist;
	l_TranspNode.m_pSceneNode = node;
	l_TranspNode.m_RenderStateCache = i_CacheState;

	o_subNodes.push_back(l_TranspNode);
}

void g3dTransparencySortDX11::InitStates()
{
	stp_NoBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 0 );
	stp_Blend = new g3dBlendStateMgr::BlendState( true, TRUE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	dsp_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_KEEP, OP_KEEP );
	dsp_Test_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_KEEP, OP_KEEP );
}

void g3dTransparencySortDX11::CleanupStates()
{
	SAFE_DELETE( stp_NoBlend );
	SAFE_DELETE( stp_Blend );
	SAFE_DELETE( dsp_Test_Write_LessE_NS );
	SAFE_DELETE( dsp_Test_LessE_NS );
}