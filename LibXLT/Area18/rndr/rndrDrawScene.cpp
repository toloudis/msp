/****************************************************************************\
**	rndrDrawScene.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Area18/rndr/rndrDrawScene.hpp"

//#include "Area18/g2d/g2dDevice.hpp"
#include "Area18/g3d/g3dSceneGlobal.hpp"
#include "Area18/g3d/g3dSceneRenderUtil.hpp"
#include "Area18/mesh/meshRenderer.hpp"
#include "Area18/mesh/meshTriMeshFrag.hpp"
#include "Area18/rndr/rndrSceneTraverse.hpp"
#include "Area18/shdr/shdrDefaultVS.hpp"
//#include "Area18/shdr/shdrColorHeadlightPS.hpp"
#include "Area18/shdr/shdrPipeline.hpp"
#include "Area18/shdr/shdrShaders.hpp"

#include "Graphics/g3d/g3dScene.hpp"

//	The reason a macro is used here (instead of a function, an inline function, or a template inline function)
//	is that I need the __FILE__ and __LINE__ macros to resolve to useful values
#define CHECK_D3D_ERROR(op_result, error_string) \
			if( op_result != D3D_OK )	\
			{	\
				g2dDX10Global::PrintDXError(op_result);	\
				DBG_ASSERT0(false, error_string);	\
			}	\

namespace
{

	maFloatRGBA l_Color(1,0,0,1);

int DrawNode(const g3dSceneGlobal& i_SceneGlobal, const g3dSceneNode* i_pNode, oglDevice* i_pDevice)
{
//	boost::shared_ptr<shdrDefaultVS> pVS = i_pDevice->m_Shaders->m_DefaultVS;
//	boost::shared_ptr<shdrColorHeadlightPS> pPS = i_pDevice->m_Shaders->m_ColorHeadlightPS;

//	pVS->SetWorldTransform(i_pNode->GetTotalTransform());
//	pVS->SetWorldViewProjectionTransform(i_pNode->GetTotalTransform()*i_SceneGlobal.GetCameraProjectionTransform());
//	pPS->SetEyePos(maVector4d(i_SceneGlobal.GetCameraPos()));
//	pPS->SetColor(maFloatRGBA(1,0,0,1));

//	shdrPipeline sp;
//	sp.Attach(pVS.get());
//	sp.Attach(pPS.get());

	meshTriMeshFrag* f = (meshTriMeshFrag*)i_pNode->GetFragment(); 
	return f->GetRenderer()->Render( f, i_pNode->GetMaterial(), NULL, i_pDevice );
	//return g3dRendererMgr::Render( i_pNode, i_pNode->GetMaterial(), NULL );
}

int DrawNodeList(const g3dSceneGlobal& i_SceneGlobal, const nodeCacheList& i_NodeList, oglDevice* i_pDevice)
{
	int npoly = 0;
	for (int i = 0; i < i_NodeList.size(); i++)
	{
		npoly += DrawNode(i_SceneGlobal, i_NodeList[i].m_pNode, i_pDevice);
	}

	return npoly;
}
}//namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rndrDrawScene::rndrDrawScene()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rndrDrawScene::~rndrDrawScene()
{
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy plus additional 
//	viewer specific layers.
//	Returns the number of triangles rendered
//--------------------------------------------------------------------
int rndrDrawScene::Render(const camCamera& i_Camera,
						  const g3dScene &i_Scene,
						  oglDevice* i_pDevice)
{//PROFILE("Render");
	g3dSceneGlobal sceneGlobal;
	//sceneGlobal.m_FrameTime = i_fSimTime;

	int npoly = 0;
	int n = i_Scene.GetNumLayers();
	for (int i = 0; i < n; i++)
	{
		// Set the camera and projection transform
		g3dSceneRenderUtil::SetViewingTransforms(i_Camera, sceneGlobal, i_Scene.GetLayer(i)->GetModelSpace());

		rndrSceneTraverse traverser;
		traverser.TraverseLayer(i_Scene.GetLayer(i), &i_Camera, sceneGlobal);

		npoly += DrawNodeList(sceneGlobal, traverser.GetShadowNodes(), i_pDevice);
	}


	return npoly;
}



