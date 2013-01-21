/****************************************************************************\
**	rndrSceneTraverse.hpp
**
**	Encapsulate the traversal of the scene graph, sorting and collecting
**	nodes that are important for later render passes.
**
** Area17
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef RNDR_SCENETRAVERSE_HPP
#error rndrSceneTraverse.hpp multiply included
#endif
#define RNDR_SCENETRAVERSE_HPP

#ifndef G3D_RENDERSTATECACHE_HPP
#include "Area18/g3d/g3dRenderStateCache.hpp"
#endif

#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif

#include <vector>

class camCamera;
class g3dLayer;
class g3dSceneGlobal;

// Store render state cache for each shadow node
struct sNodePlusState
{
	sNodePlusState() : m_pNode( NULL ), m_drawStyle( g3dSceneNode::e_Inherit ), m_StateCache( 0 ){}
	const g3dSceneNode* m_pNode;
	g3dSceneNode::DrawStyle m_drawStyle;
	g3dRenderStateCache m_StateCache;
};
typedef std::vector<sNodePlusState> nodeCacheList;

class rndrSceneTraverse
{
public:
	rndrSceneTraverse();
	~rndrSceneTraverse();

	// be sure to Clear() before traversing!
	void TraverseLayer(const g3dLayer* i_pLayer,
		const camCamera* i_pCamera, const g3dSceneGlobal& i_SceneGlobal, bool i_bDoClipping = true);

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

	g3dRenderStateTraverser* GetRenderStateCache();

	g3dLayer* GetLayer();
	const camCamera* GetCamera();

protected:
	void Traverse( g3dSceneNode* i_pNode, 
			  const g3dSceneGlobal& i_SceneGlobal,
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

	// opaque nodes are invisible but still need to render alpha channel
	nodeCacheList m_InvisibleMaskBlackNodes;

	// transparent nodes are invisible but still need to render alpha channel
	//nodeCacheList m_InvisibleMaskTransparentNodes;

	// cached sets of lighting state
	g3dRenderStateTraverser* m_StateTraverser;

	const camCamera*			m_pCamera;
};
