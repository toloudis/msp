/*****************************************************************************
**  demG3dTestTextureReduce.hpp
**
**		This mode displays a demonstration/test of the different model space
**	concepts used in the Terawatt 3d engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestTextureReduce.hpp"

#include "appTime.hpp"
#include "demModeManager.hpp"
#include "fsLocator.hpp"
#include "g2dRGBColor.hpp"
#include "g3dScene.hpp"
#include "g3dSceneNode.hpp"
#include "g3dViewer.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dLightManager.hpp"
#include "g3dLayer.hpp"
//#include "matPlainTexture.hpp"
#include "g3dPrimitiveFragmentUtil.hpp"
#include "matTextureManager.hpp"
//#include "matTextureManagerPACWin.hpp"
#include "maConstants.hpp"
#include "gfPaths.hpp"

namespace
{
	
//========================================================================
//	matTextureAdjuster provides an interface to be overridden at the game level
//	(when necessary) to adjust the loading of a texture
//========================================================================
class ReduceBy1 : public matTextureAdjuster
{
public:
	//========================================================================
	//	GetReduce returns the amount the texture should be reduced in each dimension
	//========================================================================
	virtual inline void GetReduce(const fsLocator& i_Locator, int& o_WidthReduce, int& o_HeightReduce)
					{o_WidthReduce = 1; o_HeightReduce = 1;}
};

//========================================================================
//	matTextureAdjuster provides an interface to be overridden at the game level
//	(when necessary) to adjust the loading of a texture
//========================================================================
class ReduceBy2 : public matTextureAdjuster
{
public:
	//========================================================================
	//	GetReduce returns the amount the texture should be reduced in each dimension
	//========================================================================
	virtual inline void GetReduce(const fsLocator& i_Locator, int& o_WidthReduce, int& o_HeightReduce)
					{o_WidthReduce = 2; o_HeightReduce = 2;}
};

}

//====================================================================
//====================================================================
demG3dTestTextureReduce::demG3dTestTextureReduce(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestTextureReduce::~demG3dTestTextureReduce()
{
}

//====================================================================
//====================================================================
void demG3dTestTextureReduce::Initialize()
{
	demG3dTestMode::Initialize();

	m_Root = new g3dSceneNode();
	g3dLayer *screen_layer = new g3dLayer(m_Root, g3dLayer::e_ZSort, g3dLayer::e_Screen);
	m_Scene = new g3dScene(screen_layer); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	//	make fragments
	m_Rect1Fragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(0.5f, 0.5f, 3, 3);
	m_Rect2Fragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(0.5f, 0.5f, 3, 3);
	m_Rect3Fragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(0.5f, 0.5f, 3, 3);

	//	setup fragment materials
	m_Rect1Mat.SetDiffuse(maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f));
	m_Rect2Mat.SetDiffuse(maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f));
	m_Rect2Mat.SetDiffuse(maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f));

	m_Rect1Mat.SetAmbient(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_Rect2Mat.SetAmbient(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_Rect2Mat.SetAmbient(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));

	// make a nice texture
	fsLocator locator = gfPaths::GetGamePath(gfPaths::e_ExePath);
	locator.Push("data");
	locator.Push(itString("green003.png"));
	m_Texture1 = matTextureManager::LoadTexture(locator);
	matTextureManagerPAC::SetTextureAdjuster(new ReduceBy1);
	m_Texture2 = matTextureManager::LoadTexture(locator);
	matTextureManagerPAC::SetTextureAdjuster(new ReduceBy2);
	m_Texture3 = matTextureManager::LoadTexture(locator);
	matTextureManagerPAC::SetTextureAdjuster(new matTextureAdjuster);

	m_Rect1Mat.AddTextureTop(m_Texture1);
	m_Rect2Mat.AddTextureTop(m_Texture2);
	m_Rect3Mat.AddTextureTop(m_Texture3);

	m_Rect1Fragment->SetMaterial(&m_Rect1Mat);
	m_Rect2Fragment->SetMaterial(&m_Rect2Mat);
	m_Rect3Fragment->SetMaterial(&m_Rect3Mat);

	//	make models
	maMatrix4x4 matrix;

	matrix.MakeTranslate(-0.6f, 0, 0);
	m_Rect1Model = new g3dSceneNode(m_Rect1Fragment, matrix);
	m_Root->AddChild(m_Rect1Model);
	matrix.MakeTranslate(0.0, 0, 0);
	m_Rect2Model = new g3dSceneNode(m_Rect2Fragment, matrix);
	m_Root->AddChild(m_Rect2Model);
	matrix.MakeTranslate(+0.6f, 0, 0);
	m_Rect3Model = new g3dSceneNode(m_Rect3Fragment, matrix);
	m_Root->AddChild(m_Rect3Model);

	//	set lights
	m_Light1 = g3dLightManager::CreateDirectionalLight();
	m_Light2 = g3dLightManager::CreateDirectionalLight();

	m_Light1->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	m_Light2->SetDirection(maVector3d(-0.5f, -0.8f, -0.0f));

	m_Light1->SetIntensity(maFloatRGBA(0.5f, 0.3f, 0.2f, 1.0f));
	m_Light2->SetIntensity(maFloatRGBA(0.0f, 0.6f, 0.6f, 1.0f));

	m_Light1->Enable();
	m_Light2->Enable();

	g3dLightManager::SetAmbient(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
}

//====================================================================
//	Think
//====================================================================
void demG3dTestTextureReduce::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	//	render
	m_Viewer.Render(frame_time);

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestTextureReduce::DeInitialize()
{
	//	cleanup lights
	g3dLightManager::DestroyLight(m_Light1);
	g3dLightManager::DestroyLight(m_Light2);

	//	cleanup models
	//g3dRenderer::RemoveModel(m_Rect1Model);
	//g3dRenderer::RemoveModel(m_Rect2Model);
	//g3dRenderer::RemoveModel(m_Rect3Model);

	//	cleanup fragments
	delete m_Rect1Fragment;
	delete m_Rect2Fragment;
	delete m_Rect3Fragment;
	
	//	release textures
	matTextureManager::ReleaseTexture(m_Texture1);
	matTextureManager::ReleaseTexture(m_Texture2);
	matTextureManager::ReleaseTexture(m_Texture3);

	delete m_Root;
	delete m_Scene;

	demG3dTestMode::DeInitialize();
}
