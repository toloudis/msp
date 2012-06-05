#ifdef SHDW_PASSAMBIENT_HPP
#error shdwPassAmbient.hpp multiply included
#endif
#define SHDW_PASSAMBIENT_HPP

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif 

class shdwPassTraversal;
struct sNodePlusState;

class shdwPassAmbient : public g3dRenderPass
{
public:
	shdwPassAmbient();
	~shdwPassAmbient();

	virtual int Render(float i_time);

	void SetSceneInfo(shdwPassTraversal* i_traversal) {m_sceneInfo = i_traversal;}

	static int RenderNode(const sNodePlusState& i_Node, g3dRenderStateTraverser* i_pRenderStateCache);

	static void InitStates();
	static void CleanupStates();

protected:
	shdwPassTraversal* m_sceneInfo;
};
