/*****************************************************************************
**  demG3dTestTessellation.hpp
**
**		This mode displays a demonstration/test of the different model space
**	concepts used in the Terawatt 3d engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestTessellation.hpp"

#include "Core/app/appApplication.hpp"
#include "Core/app/appCharEvent.hpp"
#include "Core/app/appTime.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "GraphicsDX11/shdw/shdwSinglePassRendererDX11.hpp"

namespace
{
};

//====================================================================
//====================================================================
demG3dTestTessellation::demG3dTestTessellation(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Renderer(NULL), m_OldRenderer(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestTessellation::~demG3dTestTessellation()
{
}

//====================================================================
//====================================================================
void demG3dTestTessellation::Initialize()
{
	demG3dTestMode::Initialize();

	// create scene (boilerplate)
	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root)); // scene owns layers
	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());
	// create scene renderer (boilerplate)
	m_Renderer = new shdwSinglePassRendererDX11();//g3dSceneRendererCreate::CreateSimpleRenderer();
	m_OldRenderer = m_Viewer.GetRenderer();
	m_Viewer.SetRenderer(m_Renderer);

	// first part of test is: let's load some various textures.

//	matShaderEffect* eff = matShaderMgr::GetSpecialEffect("default");

	// this test depends on a working shader that can show the texture!
//	m_RectMat = new matMaterial("Billboard.fx");
	m_RectMat = new matMaterial("testtess.fx");
	effPhongData* pData = dynamic_cast<effPhongData*>(m_RectMat->GetEffectData());
	pData->m_TextureDiffuse = NULL;

	m_RectMat->DisplacementData().m_TessellationValue = 2;
	
	g3dPrefs::CurrentPrefs().m_bUseHardwareTessellation = true;

	//	pData->m_Color = maFloatRGBA(1,1,1,1);

	// now let's put them in materials to draw on a quad.
	m_RectFragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(50, 50, 2, 2);
	m_RectFragment->SetMaterial(m_RectMat);
	m_RectFragment->SetReceivesShadow(true);
	m_RectNode = new g3dSceneNode();
	m_RectNode->SetFragment(m_RectFragment);

	// tess shows up in wireframe
	m_RectNode->SetDrawStyle(g3dSceneNode::e_LitWireframe);
	g3dPrefs::CurrentPrefs().m_bRenderWireframe = true;

	maMatrix4x4 xform;
	xform.MakeRotateX( -maConstants::c_fPI_Div_2 );
	m_RectNode->SetTransform(xform);

	m_Root->AddChild(m_RectNode);


//			maPoint3d camPos(58.311f, 115.497f, 84.529f);
//			maPoint3d camTarg(1.524f, 75.954f, 1.806f);
//			maPoint3d camUp(0,1,0);
//			l_Camera.LookAt(camPos, camTarg, camUp);


	// set up display info
	m_Viewer.SetTextMessage(0, itString("Keys A, S, D."));
//	m_Viewer.SetTextMessage(1, itString("The moving spheres are coincident with the positions of two point lights.  The scene is"));
//	m_Viewer.SetTextMessage(2, itString("also illuminated by a directional light.  The ground material and the sphere material"));
//	m_Viewer.SetTextMessage(3, itString("both show specular reflections, whereas the moving spheres only have diffuse reflection."));
//	m_Viewer.SetTextMessage(4,  itString("Also, the moving spheres show a non-textured translucent material."));
//	m_Viewer.SetTextMessage(5, itString("Also, the texture on the rectangle is mip-mapped.  (Try moving the camera forward and back.)"));
//	m_Viewer.SetTextMessage(6,  itString("('i', 'j', 'k', 'm', '-', and '=' move the camera)"));
//	m_Viewer.SetTextMessage(7,  itString("Press space to go to the next section"));
}

//====================================================================
//	Think
//====================================================================
void demG3dTestTessellation::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	//	render
	m_Scene->UpdateWorldData();
	m_Viewer.Render(frame_time);
	m_Viewer.Present();

	//	clear color
/*	m_Window.Clear(m_BGColor);
	m_Window.DrawImage(0,0, *m_Image);
	m_Window.DrawImage(128,128, 0,0,20,20, *m_Image);
	// draw text
	int top = 0;//30;
	int left = 0;//10;
	int lineNum = 0; 
	int y = (lineNum * 15) + top;
	int x = left;
	m_Window.DrawText( x, y, m_Font, itString("I am DirectX 10!"), m_TextColor );

	m_Window.Present();
*/
	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestTessellation::DeInitialize()
{

	g3dPrefs::CurrentPrefs().m_bUseHardwareTessellation = false;

	m_Viewer.SetRenderer(m_OldRenderer);
	delete m_Renderer;

	//	cleanup fragments
	delete m_RectFragment;
	delete m_RectMat;

	for (int i = 0; i < m_Textures.size(); i++)
		matTextureMgr::ReleaseTexture(m_Textures[i]);

	m_Viewer.ClearTextMessages();
	delete m_Root;
	delete m_Scene;

	demG3dTestMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demG3dTestTessellation::ReceiveCharEvent(appCharEvent& i_Event)
{
	static float i = 2;
	switch( i_Event.GetChar() )
	{

		case itString::CharType('a'):
		case itString::CharType('A'):
			{
				i -= 0.25f;
				m_RectMat->DisplacementData().m_TessellationValue = i;
			}
		break;
		case itString::CharType('s'):
		case itString::CharType('S'):
			{
				i += 0.25f;
				m_RectMat->DisplacementData().m_TessellationValue = i;
			}
		break;

		case itString::CharType('d'):
		case itString::CharType('D'):
			{
			}
		break;
	}

	demG3dTestMode::ReceiveCharEvent(i_Event);
}