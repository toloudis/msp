#ifdef SHDW_PASSENVIRONMENT_HPP
#error shdwPassEnvironment.hpp multiply included
#endif
#define SHDW_PASSENVIRONMENT_HPP

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif 

class shdwPassTraversal;
struct sNodePlusState;

class shdwPassEnvironment : public g3dRenderPass
{
public:
	shdwPassEnvironment();
	~shdwPassEnvironment();

	virtual int Render(float i_time);

	void SetSceneInfo(shdwPassTraversal* i_traversal) {m_sceneInfo = i_traversal;}

	static int RenderNode(const sNodePlusState& i_Node, g3dRenderStateTraverser* i_pRenderStateCache);

	static void InitStates();
	static void CleanupStates();

protected:
	shdwPassTraversal* m_sceneInfo;
};
