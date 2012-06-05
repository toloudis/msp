/****************************************************************************\
**	shdwPassNormals.cpp
**
**		Render pass to draw normal vectors x,y,z to target r,g,b
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef SHDW_PASSNORMALS_HPP
#error shdwPassNormals.hpp multiply included
#endif
#define SHDW_PASSNORMALS_HPP

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif 

class shdwPassTraversal;
struct sNodePlusState;

class shdwPassNormals : public g3dRenderPass
{
public:
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	shdwPassNormals();

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	shdwPassNormals(g2dRenderTarget* i_pDepthTarget, const camCamera* i_pCamera);

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	~shdwPassNormals();

	//----------------------------------------------------------------------------------------
	// This function initializes the globle materials used in RenderNode func
	//----------------------------------------------------------------------------------------
	static void InitializeMaterial();	

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	virtual int Render(float i_time);

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	void SetSceneInfo(shdwPassTraversal* i_traversal) {m_sceneInfo = i_traversal;}

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	static int RenderNode(const sNodePlusState& i_Node, bool i_isTanSpace = true, bool i_bRenderPositions = false);

	//----------------------------------------------------------------------------------------
	// This function renders a single node with the special hair shader
	//----------------------------------------------------------------------------------------
	int RenderHairNode(const sNodePlusState& i_Node);

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	static void InitStates();
	static void CleanupStates();

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	void SetRenderPositions(bool i_bWritePositions);

protected:
	shdwPassTraversal* m_sceneInfo;
	const camCamera* m_pCamera;
	bool m_RenderPositions;
};
