#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/Mat/matMaterial.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "GraphicsDX11/g3d/g3dTransparencySortDX11.hpp"
#include "Graphics/mat/matShaderMgr.hpp"

shdwPassTraversal::shdwPassTraversal()
	: m_pLayer(NULL), m_pCamera(NULL)
{
	m_StateTraverser = new g3dRenderStateTraverser();
	m_transparencySort = new g3dTransparencySortDX11();
	m_HairSortNodes = new g3dTransparencySortDX11();
}
shdwPassTraversal::~shdwPassTraversal()
{
	delete m_StateTraverser;
	delete m_transparencySort;
	delete m_HairSortNodes;
}

void shdwPassTraversal::Traverse( g3dSceneNode* i_pNode, 
			  g3dSceneNode::DrawStyle i_DrawStyle,
			  bool i_bRenderLowRes,
			  bool i_bDoClip)
{

	// FIX : this probably messes up shadows. if scene reuses shadow map in refl map rendering,
	//	and some objs are hidden, they should not cast shadows in the refl map, right?
	if ( g3dSingleLightRendering::GetDoPlaneReflectionGen() && !i_pNode->GetRenderableInPlanarReflection() )
	{
		return;
	}
	if ( g3dSingleLightRendering::GetDoCubeReflectionGen() && !i_pNode->GetRenderableInCubeMapReflection() )
	{
		return;
	}

	// See if we can cull from resolution
	if (!g3dSceneRenderUtil::check_resolution(i_pNode, i_bRenderLowRes))
		return;

	if (i_bDoClip)
	{
		// Check if node is culled
		ClipResult clip_result = g3dSceneRenderUtil::get_box_vis( i_pNode->GetWorldBox() );
		if( clip_result == e_Reject )
		{
			return;
		}
		else if (clip_result == e_NoClip)
		{
			// Box is completely in view, no need to check children
			// because they will be in view also.
			i_bDoClip = false;
		}
	}
 
	// Add the render state to the stack:
	// This allows us to remember lighting state for nodes that need to be 
	// rendered in subsequent passes.
	m_StateTraverser->AddRenderState( i_pNode->GetRenderState(), i_pNode->GetEnvironment(),
		g3dSingleLightRendering::GetDoSingleLightRendering() );

	// Combine draw style
	g3dSceneNode::DrawStyle draw_style = i_DrawStyle;
	if (i_pNode->GetDrawStyle() != g3dSceneNode::e_Inherit)
	{
		draw_style = i_pNode->GetDrawStyle();
	}
	if( g3dPrefs::CurrentPrefs().m_bRenderWireframe )
	{
		draw_style = g3dSceneNode::e_LitWireframe;
	}

	// (Invis in reflections && reflectiongen), means draw always
	bool renderForReflection = (g3dPrefs::CurrentPrefs().m_bEnableInvisibleInReflections &&
		g3dSingleLightRendering::GetDoReflectionGen());

	if( !i_pNode->GetRenderable())
	{
		if (!renderForReflection)
		{
			m_StateTraverser->SubtractRenderState( i_pNode->GetRenderState(), i_pNode->GetEnvironment(),
				g3dSingleLightRendering::GetDoSingleLightRendering() );
			return;
		}
	}

	// Store invisible mask black nodes into list
	if (!i_pNode->GetActiveInRenderLayer())
	{
		if (g3dPrefs::CurrentPrefs().m_bEnableInvisibleMaskBlack)
		{
			const g3dFragment* pFrag = i_pNode->GetFragment();
			matMaterial* pMatOverride = i_pNode->GetMaterial();
			if (pFrag)
			{
				sNodePlusState node;
				node.m_pNode = i_pNode;
				node.m_drawStyle = draw_style;
				node.m_StateCache = m_StateTraverser->GetCurrentStateCache();
				
				// Take this out once we decide to do post alpha fill process
				//m_InvisibleMaskBlackNodes.push_back(node);
				//m_InvisibleMaskBlackNodesBounds.Union(i_pNode->GetWorldBox());

				/*m_transparencySort->AddTransparentNode( i_pNode, 
														m_pCamera->GetPosition(), 
														m_StateTraverser->GetCurrentStateCache(),
														true);
				m_TransparentNodesBounds.Union(i_pNode->GetWorldBox());*/

				// Put this code back we decide to do post alpha fill process
				if( ( pMatOverride && pMatOverride->GetHasTransparency() ) ||
					pFrag->GetMaterial()->GetHasTransparency() )
				{
					/*m_InvisibleMaskTransparentNodes.push_back(node);
					m_InvisibleMaskTransparentNodesBounds.Union(i_pNode->GetWorldBox());*/
					m_transparencySort->AddTransparentNode( i_pNode, 
														m_pCamera->GetPosition(), 
														m_StateTraverser->GetCurrentStateCache(),
														true);
					m_TransparentNodesBounds.Union(i_pNode->GetWorldBox());
				}
				else
				{
					m_InvisibleMaskBlackNodes.push_back(node);
					m_InvisibleMaskBlackNodesBounds.Union(i_pNode->GetWorldBox());
				}
			}

			std::vector<g3dSceneNode*>& children = i_pNode->GetChildren();
			int nkids = children.size();
			for (int i=0; i<nkids; i++)
			{
				// Set the renderable param of the children the same with its parent
				bool renderable = children[i]->GetActiveInRenderLayer();
				children[i]->SetActiveInRenderLayer(false);
				Traverse(children[i], 
					draw_style,
					i_bRenderLowRes,
					i_bDoClip);
				children[i]->SetActiveInRenderLayer(renderable);
			}
		}
		
		// (Invis in reflections && reflectiongen), means ignore the renderable.
		if (!renderForReflection)
		{
			m_StateTraverser->SubtractRenderState( i_pNode->GetRenderState(), i_pNode->GetEnvironment(),
				g3dSingleLightRendering::GetDoSingleLightRendering() );
			return;
		}
	}

	const g3dFragment* pFrag = i_pNode->GetFragment();

	if ( pFrag )
		FillMaterialArray( pFrag->GetMaterial()->GetName() );

	// Check if it has geometry
	if( pFrag && !pFrag->IsShadowHull())
	{
		matMaterial* pMatOverride = i_pNode->GetMaterial();

		if( pFrag->IsHair() )	//add the hair node
		{
			i_pNode->SetDrawStyle( draw_style );	//don't draw wireframe for hair
			m_HairSortNodes->AddTransparentNode( i_pNode, 
				m_pCamera->GetPosition(), 
				m_StateTraverser->GetCurrentStateCache() );
			m_HairSortBounds.Union(i_pNode->GetWorldBox());
		}
		else if (draw_style != g3dSceneNode::e_Solid)
		{
			sNodePlusState node;
			node.m_pNode = i_pNode;
			node.m_drawStyle = draw_style;
			node.m_StateCache = m_StateTraverser->GetCurrentStateCache();
			m_NonSolidNodes.push_back(node);
			m_NonSolidNodesBounds.Union(i_pNode->GetWorldBox());
		}
		// If it is transparent then render it after the opaque
		else if( ( pMatOverride && pMatOverride->GetHasTransparency() ) ||
				pFrag->GetMaterial()->GetHasTransparency() )
		{
			// is it a particle node?
			if (g3dSingleLightRendering::GetDoSingleLightRendering() &&
				g3dPrefs::CurrentPrefs().m_bEnableDeferredTransparency &&
				pFrag->GetDeferTransparency())
			{
				m_transparencySort->AddDeferredNode( i_pNode, 
					m_pCamera->GetPosition(), 
					m_StateTraverser->GetCurrentStateCache() );
				m_TransparentNodesBounds.Union(i_pNode->GetWorldBox());
			}
			else
			{
				m_transparencySort->AddTransparentNode( i_pNode, 
														m_pCamera->GetPosition(), 
														m_StateTraverser->GetCurrentStateCache() );
				m_TransparentNodesBounds.Union(i_pNode->GetWorldBox());
			}


			// store glowing objects for post process glow effect rendering
			if ( g3dPrefs::CurrentPrefs().m_bEnableGlow && 
				((pMatOverride && pMatOverride->GetHasGlow() && (pMatOverride->GlowData().m_GlowAmount != 0)) ||
				(pFrag->GetMaterial()->GetHasGlow() && (pFrag->GetMaterial()->GlowData().m_GlowAmount != 0)))
				)
			{
				sNodePlusState node;
				node.m_pNode = i_pNode;
				node.m_drawStyle = draw_style;
				node.m_StateCache = m_StateTraverser->GetCurrentStateCache();
				m_GlowNodes.push_back(node);
				m_GlowNodesBounds.Union(i_pNode->GetWorldBox());
			}

			// store outlining objects for post process outline effect rendering
			if ( g3dPrefs::CurrentPrefs().m_bEnableOutline && 
				((pMatOverride && pMatOverride->GetHasOutline() ) ||
				(pFrag->GetMaterial()->GetHasOutline()))
				)
			{
				sNodePlusState node;
				node.m_pNode = i_pNode;
				node.m_drawStyle = draw_style;
				node.m_StateCache = m_StateTraverser->GetCurrentStateCache();
				m_OutlineNodes.push_back(node);
				m_OutlineNodesBounds.Union(i_pNode->GetWorldBox());
			}

		}
		// Else render the node
		else
		{
			// If this model receives a shadow and we are gathering shadow
			// fragments, then store this fragment
			if (g3dSingleLightRendering::GetDoSingleLightRendering() 
				/*&& pFrag->GetReceivesShadow()*/ )
			{
				// push back all shadow receiving objects
				sNodePlusState node;
				node.m_pNode = i_pNode;
				node.m_drawStyle = draw_style;
				node.m_StateCache = m_StateTraverser->GetCurrentStateCache();
				m_ShadowNodes.push_back(node);
				m_ShadowNodesBounds.Union(i_pNode->GetWorldBox());
			}
			else
			{
				sNodePlusState node;
				node.m_pNode = i_pNode;
				node.m_drawStyle = draw_style;
				node.m_StateCache = m_StateTraverser->GetCurrentStateCache();
				m_NonShadowNodes.push_back(node);
				m_NonShadowNodesBounds.Union(i_pNode->GetWorldBox());
			}
			
			// store glowing objects for post process glow effect rendering
			if ( g3dPrefs::CurrentPrefs().m_bEnableGlow && 
				((pMatOverride && pMatOverride->GetHasGlow() && (pMatOverride->GlowData().m_GlowAmount != 0)) ||
				(pFrag->GetMaterial()->GetHasGlow() && (pFrag->GetMaterial()->GlowData().m_GlowAmount != 0)))
				)
			{
				sNodePlusState node;
				node.m_pNode = i_pNode;
				node.m_drawStyle = draw_style;
				node.m_StateCache = m_StateTraverser->GetCurrentStateCache();
				m_GlowNodes.push_back(node);
				m_GlowNodesBounds.Union(i_pNode->GetWorldBox());
			}

			// store outlining objects for post process outline effect rendering
			if ( g3dPrefs::CurrentPrefs().m_bEnableOutline && 
				((pMatOverride && pMatOverride->GetHasOutline()) ||
				(pFrag->GetMaterial()->GetHasOutline()))
				)
			{
				sNodePlusState node;
				node.m_pNode = i_pNode;
				node.m_drawStyle = draw_style;
				node.m_StateCache = m_StateTraverser->GetCurrentStateCache();
				m_OutlineNodes.push_back(node);
				m_OutlineNodesBounds.Union(i_pNode->GetWorldBox());
			}

		}
	}

	// Render the children
	std::vector<g3dSceneNode*>& children = i_pNode->GetChildren();
	int nkids = children.size();
	for (int i=0; i<nkids; i++)
		Traverse(children[i], 
			draw_style,
			i_bRenderLowRes,
			i_bDoClip);

	// Remove the render state
	m_StateTraverser->SubtractRenderState( i_pNode->GetRenderState(), i_pNode->GetEnvironment(),
		g3dSingleLightRendering::GetDoSingleLightRendering() );
}

void shdwPassTraversal::TraverseLayer(float i_time, g3dLayer* i_pLayer,
	const camCamera* i_pCamera, bool i_bDoClipping)
{
	m_pCamera = i_pCamera;
	m_pLayer = i_pLayer;

	// init a new state traverser here
	if (m_StateTraverser == NULL)
		m_StateTraverser = new g3dRenderStateTraverser();

	g3dSceneNode::DrawStyle draw_style = g3dPrefs::CurrentPrefs().m_DrawStyle; //g3dSceneNode::e_Solid;
	bool bLowRes = g3dPrefs::CurrentPrefs().m_bLowResolution; //false;
	Traverse(m_pLayer->GetRootNode(),
		draw_style,
		bLowRes,
		i_bDoClipping);
	g3dDrawStyleUtilDX11::RestoreNormalDrawStyle();
/*
	// sort by material(shader) and other render properties!
	// I tested this and there was really no appreciable difference in performance on a heavy scene.
	// Therefore I have commented it out but leave it in for future use.
struct NodeSorter
{
	//std::vector<sNodePlusState>
	inline bool operator()(const sNodePlusState &lhs,
						 const sNodePlusState &rhs) const
	{
		// simply arrange nodes by effect pointer - like shaders group together.
		matShaderEffect* rS = matShaderMgr::GetEffect(*rhs.m_pNode->GetFragment()->GetMaterial());
		matShaderEffect* lS = matShaderMgr::GetEffect(*lhs.m_pNode->GetFragment()->GetMaterial());
		return lS < rS;
	}
} nodeSorter;

	std::sort(m_ShadowNodes.begin(), m_ShadowNodes.end(), nodeSorter);
	std::sort(m_NonShadowNodes.begin(), m_NonShadowNodes.end(), nodeSorter);
	std::sort(m_NonSolidNodes.begin(), m_NonSolidNodes.end(), nodeSorter);
	std::sort(m_GlowNodes.begin(), m_GlowNodes.end(), nodeSorter);
*/
}

void shdwPassTraversal::Clear()
{
    m_ShadowNodes.clear();
    m_NonShadowNodes.clear();
	m_NonSolidNodes.clear();
	m_GlowNodes.clear();
	m_OutlineNodes.clear();
	m_InvisibleMaskBlackNodes.clear();
	//m_InvisibleMaskTransparentNodes.clear();

	m_transparencySort->ClearTransparentNodes();
	m_HairSortNodes->ClearTransparentNodes();

	m_ShadowNodesBounds = maAxisBox();
	m_NonShadowNodesBounds = maAxisBox();
	m_NonSolidNodesBounds = maAxisBox();
	m_GlowNodesBounds = maAxisBox();
	m_OutlineNodesBounds = maAxisBox();
	m_InvisibleMaskBlackNodesBounds = maAxisBox();
	//m_InvisibleMaskTransparentNodesBounds = maAxisBox();
	m_TransparentNodesBounds = maAxisBox();
	m_HairSortBounds = maAxisBox();

	delete m_StateTraverser;
	m_StateTraverser = NULL;
}

const nodeCacheList& shdwPassTraversal::GetShadowNodes()
{
	return m_ShadowNodes;
}
const nodeCacheList& shdwPassTraversal::GetNonShadowNodes()
{
	return m_NonShadowNodes;
}
const nodeCacheList& shdwPassTraversal::GetNonSolidNodes()
{
	return m_NonSolidNodes;
}
const nodeCacheList& shdwPassTraversal::GetGlowNodes()
{
	return m_GlowNodes;
}
const nodeCacheList& shdwPassTraversal::GetOutlineNodes()
{
	return m_OutlineNodes;
}
const nodeCacheList& shdwPassTraversal::GetInvisibleMaskBlackNodes()
{
	return m_InvisibleMaskBlackNodes;
}
//const nodeCacheList& shdwPassTraversal::GetInvisibleMaskTransparentNodes()
//{
//	return m_InvisibleMaskTransparentNodes;
//}

g3dRenderStateTraverser* shdwPassTraversal::GetRenderStateCache()
{
	return m_StateTraverser;
}

g3dTransparencySortDX11* shdwPassTraversal::GetTransparentNodes()
{
	return m_transparencySort;
}

g3dTransparencySortDX11* shdwPassTraversal::GetHairNodes()
{
	return m_HairSortNodes;
}


g3dLayer* shdwPassTraversal::GetLayer()
{
	return m_pLayer;
}

const camCamera* shdwPassTraversal::GetCamera()
{
	return m_pCamera;
}

std::vector<std::string>* shdwPassTraversal::GetMaterialArray()
{
	return &m_Materials;
}

void shdwPassTraversal::ClearMaterialArray()
{
	m_Materials.clear();
}

void shdwPassTraversal::FillMaterialArray(std::string i_MatName)
{
	bool foundMat = false;
	for ( int i = 0 ; i < m_Materials.size() ; i++ )
	{
		if ( m_Materials[i] == i_MatName )
		{
			foundMat = true;
			break;
		}
	}
	if ( !foundMat )
		m_Materials.push_back(i_MatName);
}