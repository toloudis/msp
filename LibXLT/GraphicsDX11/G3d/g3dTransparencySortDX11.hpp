/****************************************************************************\
**	g3dTransparencySortDX11.hpp
**
**	g3dTransparencySortDX11 handles the sorting and rendering 
**	of transparent objects as a utility for SceneRenderers.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_TRANSPARENCYSORTDX11_HPP
#error g3dTransparencySortDX11.hpp multiply included
#endif
#define G3D_TRANSPARENCYSORTDX11_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef G3D_RENDERSTATECACHE_HPP
#include "GraphicsDX11/g3d/g3dRenderStateCache.hpp"
#endif

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class g2dRenderTarget;
class g3dSceneNode;
class g3dSceneNodeRenderer;
class g3dRenderStateTraverser;

// Transparent Node - used to store the node to be rendered after the opaque
struct TranspNode
{
	TranspNode::TranspNode() : m_bIsMask(false) {}
	g3dSceneNode* m_pSceneNode;
	g3dRenderStateCache m_RenderStateCache;
	float m_fDistance;
	bool m_bIsMask;

	inline bool operator < ( const TranspNode& i_TranspNode )
	{
		return m_fDistance > i_TranspNode.m_fDistance;
	}
};
typedef std::vector<TranspNode> TranspNodeVector;

class g3dTransparencySortDX11
{
public:
	g3dTransparencySortDX11();
	virtual ~g3dTransparencySortDX11();

	//------------------------------------------------------------------------
	//	AddTransparentNode - stores the transparent node for sorting and
	//		rendering when RenderTransparentNodes() is called later.
	//------------------------------------------------------------------------
	void AddTransparentNode( g3dSceneNode* i_pNode, 
							 const maPoint3d &i_CameraPos,
							 g3dRenderStateCache i_CacheState,
							 bool i_bIsMask = false);

	//------------------------------------------------------------------------
	//	AddDeferredNode - stores the transparent node for sorting and
	//		rendering when RenderTransparentNodes() is called later.
	//------------------------------------------------------------------------
	void AddDeferredNode( g3dSceneNode* i_pNode, 
		const maPoint3d &i_CameraPos,
		g3dRenderStateCache i_CacheState);

	//------------------------------------------------------------------------
	//	RenderTransparentNodes - renders the nodes added in calls
	//	to AddTransparentNode() since the last call to this function.
	//  The nodes are sorted and D3D states are set appropriately.
	//------------------------------------------------------------------------
	int RenderTransparentNodes(const g3dRenderStateTraverser& i_StateTraverser );

	//------------------------------------------------------------------------
	// Quick check for trivial rejection
	//------------------------------------------------------------------------
	bool HaveTransparentNodes();

	//------------------------------------------------------------------------
	// As an alternative to RenderTransparentNodes(), these 2 functions
	// plus RenderUserFunc do the steps of rendering separately. This way you 
	// can render the transparent node list more than once (summing different
	// lighting conditions)
	//------------------------------------------------------------------------
	void SortTransparentNodes(bool i_bIsIncludeMask = false);
	void ClearTransparentNodes();

	//------------------------------------------------------------------------
	// As an alternative to RenderNoSortNoClear, you can ask to have a
	// function called on each node and do your own rendering.
	// Default behavior would call g3dRendererMgr::Render( i_pSceneNode )
	// on each node..
	//------------------------------------------------------------------------
	typedef int (*RenderTransparentNodeFunc)( g3dSceneNode* i_pNode, 
											  g3dRenderStateCache i_CacheState, 
											  const g3dRenderStateTraverser& i_StateTraverser);
	int RenderUserFunc(RenderTransparentNodeFunc i_OpaqueFunction, 
						RenderTransparentNodeFunc i_Function, 
						const g3dRenderStateTraverser& i_StateTraverser);
	int RenderUserFuncDeferred(RenderTransparentNodeFunc i_Function, 
						const g3dRenderStateTraverser& i_StateTraverser);

	const TranspNodeVector& GetTransparentNodes(bool i_bIsIncludeMask = false);
	const TranspNodeVector& GetTransparentMaskNodes();

	static void InitStates();
	static void CleanupStates();

protected:
	int l_nTranspNodes;
	TranspNode l_TranspNode;
	static bool s_bDoSplitting;
	static bool s_bDoExtraAlphaTestPass;
	static unsigned int s_alphaRef;
	TranspNodeVector l_TranspNodes;
	TranspNodeVector l_TranspMaskNodes;
	TranspNodeVector l_TranspPlusMaskNodes;
	TranspNodeVector l_SortedTranspSubNodes;
	TranspNodeVector l_DeferredNodes;

	void MakeSubNodes(g3dSceneNode* i_curNode, g3dRenderStateCache i_statecache, 
		std::vector<TranspNode>& o_subNodes);
	void MakeSubNode(g3dSceneNode* i_curNode, g3dRenderStateCache i_CacheState, 
		float i_dist, int iSubNode, 
		std::vector<TranspNode>& o_subNodes);
};

