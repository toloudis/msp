/****************************************************************************\
**	shdwPassLit.hpp
**
**	Render lit objects.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef SHDW_PASSLIT_HPP
#error shdwPassLit.hpp multiply included
#endif
#define SHDW_PASSLIT_HPP

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

#ifndef SHDW_PASSTRAVERSAL_HPP
#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"
#endif

#include <vector>

class shdwPassLit : public g3dRenderPass
{
public:
	shdwPassLit();
	~shdwPassLit();

	virtual int Render(float i_time);

	void SetSceneInfo(shdwPassTraversal* i_traversal) {m_sceneInfo = i_traversal;}

	static int RenderNode(const sNodePlusState& i_Node, const std::vector<g3dLight*>& i_Lights, g3dRenderStateTraverser* i_RenderStateCache);

	static void InitStates();
	static void CleanupStates();

protected:
	shdwPassTraversal* m_sceneInfo;

	void LightingLoopPerNode(const std::vector<g3dLight*>& i_Lights, const nodeCacheList& i_Nodes, g3dRenderStateTraverser* i_RenderStateCache);
	void LightingLoopPerLight(const std::vector<g3dLight*>& i_Lights, const nodeCacheList& i_Nodes, g3dRenderStateTraverser* i_RenderStateCache);
};
