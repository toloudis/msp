/****************************************************************************\
**	shdwPassVelocity.cpp
**
**		Render pass to draw normal vectors x,y,z to target r,g,b
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef SHDW_PASSVELOCITY_HPP
#error shdwPassVelocity.hpp multiply included
#endif
#define SHDW_PASSVELOCITY_HPP

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif 

#ifndef TMESH_FRAG_HPP
#include "GraphicsDX11/tmesh/tmeshFrag.hpp"
#endif

#ifndef SHDW_VELOCITYSTATEMANAGER_HPP
#include "GraphicsDX11/shdw/shdwVelocityStateManager.hpp"
#endif

class effShaderBaseDX11;
class shdwPassTraversal;
class g3dScene;
struct ID3DX11Effect;

class shdwPassVelocity : public g3dRenderPass
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwPassVelocity();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwPassVelocity(g2dRenderTarget* i_pVelocityTarget, 		
					 VelocityStateManager* i_pVelocityStateManager,
					 const camCamera* i_pCamera,
					 const g3dScene* i_Scene);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwPassVelocity();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual int Render(float i_time);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetSceneInfo(shdwPassTraversal* i_traversal) {m_sceneInfo = i_traversal;}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static int RenderNode(const g3dSceneNode* i_pNode);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetupOldMatrix( const g3dSceneNode* pNode, ID3DX11Effect* pEffect  );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetOldTime(float time);
	
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SelectStoringTechnique( const g3dSceneNode* pNode, effShaderBaseDX11* pEffect );

	//------------------------------------------------------------------------
	//	Check if an object has been processed
	//------------------------------------------------------------------------
	int InsertObject(std::string i_Name);

	//------------------------------------------------------------------------
	//	Clear processed objects
	//------------------------------------------------------------------------
	void ClearObjects();

	float m_lastTime;

	static void InitStates();
	static void CleanupStates();

protected:
	
	std::map<std::string,int>	m_ObjectNames;
	shdwPassTraversal*			m_sceneInfo;
	VelocityStateManager*		m_pVelocityStateManager;
	const camCamera*			m_pCamera;
	const g3dScene*				m_Scene;
};
