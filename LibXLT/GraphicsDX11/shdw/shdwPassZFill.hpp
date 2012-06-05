#ifdef SHDW_PASSZFILL_HPP
#error shdwPassZFill.hpp multiply included
#endif
#define SHDW_PASSZFILL_HPP

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif 

class shdwPassTraversal;
struct sNodePlusState;

class shdwPassZFill : public g3dRenderPass
{
public:
	shdwPassZFill(bool i_writeAlpha = false, bool i_bDrawTransparent = false );
	~shdwPassZFill();

	virtual int Render(float i_time);

	// RenderOneNode is a copy of render function but only take one node to render
	int RenderOneNode(float i_time, const sNodePlusState& i_Node);

	void SetSceneInfo(shdwPassTraversal* i_traversal) {m_sceneInfo = i_traversal;}

	static int RenderNode(const sNodePlusState& i_Node, g3dRenderStateTraverser* i_pRenderStateCache, float i_OverWriteAlpha = 1.0f);
	//----------------------------------------------------------------------------------------
	// This function renders a single node with the special hair shader
	//----------------------------------------------------------------------------------------
	int RenderHairNode(const sNodePlusState& i_Node);

	void SetWriteAlpha(const bool i_bWriteAlpha);
	void SetDrawTransparent(const bool i_bDrawTransparent );

	static void InitStates();
	static void CleanupStates();

protected:
	shdwPassTraversal* m_sceneInfo;
	bool m_isWriteAlpha;
	bool m_bDrawTransparent;
};
