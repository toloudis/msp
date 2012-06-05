#ifdef SHDW_PASSDOF_HPP
#error shdwPassDOF.hpp multiply included
#endif
#define SHDW_PASSDOF_HPP

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif 

class shdwPassTraversal;

class shdwPassDOF : public g3dRenderPass
{
public:
	// pass in temp targets for transparency. this could probably be eliminated.
	shdwPassDOF();
	~shdwPassDOF();

	virtual int Render(float i_time);

	void SetSceneInfo(shdwPassTraversal* i_traversal) {m_sceneInfo = i_traversal;}

	static void InitStates();
	static void CleanupStates();

protected:
	shdwPassTraversal* m_sceneInfo;

	int DrawNodeDOF(const g3dSceneNode* i_pNode);
};
