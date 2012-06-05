/****************************************************************************\
**	shdwPassTraversal.hpp
**
**	Encapsulate the traversal of the scene graph, sorting and collecting
**	nodes that are important for later render passes.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef SHDW_PASSTRAVERSAL_HPP
#error shdwPassTraversal.hpp multiply included
#endif
#define SHDW_PASSTRAVERSAL_HPP

#ifndef G3D_RENDERSTATECACHE_HPP
#include "GraphicsDX11/g3d/g3dRenderStateCache.hpp"
#endif

#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif

#include <vector>

class camCamera;
class g3dLayer;
class g3dTransparencySortDX11;

// Store render state cache for each shadow node
struct sNodePlusState
{
	sNodePlusState() : m_pNode( NULL ), m_drawStyle( g3dSceneNode::e_Inherit ), m_StateCache( 0 ){}
	const g3dSceneNode* m_pNode;
	g3dSceneNode::DrawStyle m_drawStyle;
	g3dRenderStateCache m_StateCache;
};
typedef std::vector<sNodePlusState> nodeCacheList;

class shdwPassTraversal
{
public:
	shdwPassTraversal();
	~shdwPassTraversal();

	// be sure to Clear() before traversing!
	void TraverseLayer(float i_time, g3dLayer* i_pLayer,
		const camCamera* i_pCamera, bool i_bDoClipping = true);

	// call this once per frame to empty out data after rendering
	// (should it be a destructor call? no - keep mem allocated for vectors and state traverser)
    void Clear();

	const nodeCacheList& GetShadowNodes();
	maAxisBox m_ShadowNodesBounds;
	const nodeCacheList& GetNonShadowNodes();
	maAxisBox m_NonShadowNodesBounds;
	const nodeCacheList& GetNonSolidNodes();
	maAxisBox m_NonSolidNodesBounds;
	const nodeCacheList& GetGlowNodes();
	maAxisBox m_GlowNodesBounds;
	const nodeCacheList& GetOutlineNodes();
	maAxisBox m_OutlineNodesBounds;
	const nodeCacheList& GetInvisibleMaskBlackNodes();
	maAxisBox m_InvisibleMaskBlackNodesBounds;
	/*const nodeCacheList& GetInvisibleMaskTransparentNodes();
	maAxisBox m_InvisibleMaskTransparentNodesBounds;*/

	g3dRenderStateTraverser* GetRenderStateCache();
	g3dTransparencySortDX11* GetTransparentNodes();
	maAxisBox m_TransparentNodesBounds;
	g3dTransparencySortDX11* GetHairNodes();
	maAxisBox m_HairSortBounds;

	g3dLayer* GetLayer();
	const camCamera* GetCamera();

	std::vector<std::string>* GetMaterialArray();
	void ClearMaterialArray();
	void FillMaterialArray(std::string i_MatName);

protected:
	void Traverse( g3dSceneNode* i_pNode, 
			  g3dSceneNode::DrawStyle i_DrawStyle,
			  bool i_bRenderLowRes,
			  bool i_bDoClip );


	// opaque (non transparent) shadow nodes
    nodeCacheList m_ShadowNodes;

	// opaque (non transparent) non-shadowed nodes
    nodeCacheList m_NonShadowNodes;

	// nodes with draw style not e_Solid
	nodeCacheList m_NonSolidNodes;

	// nodes with glow effect
	nodeCacheList m_GlowNodes;

	// nodes with outline effect
	nodeCacheList m_OutlineNodes;

	// nodes with hair effect
	g3dTransparencySortDX11* m_HairSortNodes;

	// opaque nodes are invisible but still need to render alpha channel
	nodeCacheList m_InvisibleMaskBlackNodes;

	// transparent nodes are invisible but still need to render alpha channel
	//nodeCacheList m_InvisibleMaskTransparentNodes;

	// cached sets of lighting state
	g3dRenderStateTraverser* m_StateTraverser;

	// renderable collection of transparent nodes 
	g3dTransparencySortDX11* m_transparencySort;

	// collection of all materials in scene
	std::vector<std::string> m_Materials;

	g3dLayer*			m_pLayer;
	const camCamera*			m_pCamera;
};
