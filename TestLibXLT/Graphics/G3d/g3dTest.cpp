#include <stdio.h>
#include <windows.h>
#include <ddraw.h>
#include <math.h>

#undef CreateFile
#undef CopyFile
#undef DeleteFile
#undef CreateDirectory
#undef DrawText

#include "Core/env/envPlatform.hpp"

#include "Graphics/GraphicsLayer.hpp"
#include "GraphicsDX9/GraphicsDX9Layer.hpp"

#include "Graphics/an/anPackage.hpp"
#include "Core/app/appApplication.hpp"
#include "Core/app/private/appApplicationPAC.hpp"
#include "Core/app/appCharEvent.hpp"
#include "Core/app/appCharEventHandler.hpp"
#include "Core/app/appMouseEvent.hpp"
#include "Core/app/appMouseEventHandler.hpp"
#include "Core/app/appFlowEvent.hpp"
#include "Core/app/appFlowEventHandler.hpp"
#include "Core/app/appPackage.hpp"
#include "GraphicsDX9/bump/bumpPackage.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/dbg/dbgPackage.hpp"
#include "demModeManager.hpp"

#include "demG3dTestAmbientEnvMap.hpp"
//#include "demG3dTestAmbientOcclusion.hpp"
#include "demG3dTestAnims.hpp"
//#include "demG3dTestBumpMap.hpp"
//#include "demG3dTestCubeMap.hpp"
//#include "demG3dTestDepthPeeling.hpp"
#include "demG3dTestDrawStyle.hpp"
//#include "demG3dTestEffectShader.hpp"
//#include "demG3dTestFAOgen.hpp"
#include "demG3dTestFog.hpp"
#include "demG3dTestFX.hpp"
#include "demG3dTestGPUAmbientOcclusion.hpp"
#include "demG3dTestHairShader.hpp"
#include "demG3dTestHDRAmbientEnvMap.hpp"
#include "demG3dTestModelSpace.hpp"
#include "demG3dTestModelMaterial.hpp"
//*** #include "demG3dTestMorph.hpp"
#include "demG3dTestHierarch.hpp"
//#include "demG3dTestIlluminance.hpp"
#include "demG3dTestLights.hpp"
#include "demG3dTestPickBuffer.hpp"
//#include "demG3dTestPixelShader.hpp"
#include "demG3dTestPivot.hpp"
#include "demG3dTestPreLit.hpp"
#include "demG3dTestPrims.hpp"
#include "demG3dTestProjectedLights.hpp"
#include "demG3dTestRenderState.hpp"
#include "demG3dTestSkinShader.hpp"
#include "demG3dTestSubWindow.hpp"
//*** #include "demG3dTestProgressive.hpp"
#include "demG3dTestRenderToTexture.hpp"
//**? #include "demG3dTestSpriteGroup.hpp"
//#include "demG3dTestTextureCompression.hpp"
//#include "demG3dTestTextureReduce.hpp"
//#include "demG3dTestTextures.hpp"
//#include "demG3dTestVertexShader.hpp"
//*** #include "demG3dTestVertexColoring.hpp"

#include "Core/env/envPackage.hpp"
#include "Core/fs/fsPackage.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Graphics/g2d/g2dFontUtil.hpp"
#include "Graphics/g2d/g2dPackage.hpp"
#include "GraphicsDX9/g2d/g2dSystemDX9.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g3d/g3dPackage.hpp"
#include "Graphics/g3d/g3dExceptionX.hpp"
#include "GraphicsDX9/g3d/g3dSceneRendererDX9.hpp"
#include "GraphicsDX9/g3d/g3dSystemDX9.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Core/gf/gfPackage.hpp"
#include "Core/it/itPackage.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "GraphicsDX9/may/mayPackage.hpp"
#include "GraphicsDX9/tmesh/tmeshPackage.hpp"
#include "GraphicsDX9/shdw/shdwShadowSceneRendererD3D.hpp"
#include "GraphicsDX9/shdw/shdwShadowLayerRendererDX9.hpp"
#include "shdwFXTestRendererD3D.hpp"
#include "shdwTestShaderRendererD3D.hpp"

namespace
{

bool l_bWindowed = true;

}

//====================================================================
//====================================================================
class MyApp	:	public appApplication,
				public appFlowEventHandler
{
	public:

		MyApp();
		~MyApp();

		//====================================================================
		//	Override this function to get appStartEvents.
		//====================================================================
		virtual void ReceiveStartEvent(appStartEvent& i_Event);

		//====================================================================
		//	Override this function to get appStopEvents.
		//====================================================================
		virtual void ReceiveStopEvent(appStopEvent& i_Event);

		//====================================================================
		//	Override this function to get appSuspendEvents.
		//====================================================================
		virtual void ReceiveSuspendEvent(appSuspendEvent& i_Event);

		//====================================================================
		//	Override this function to get appResumeEvents.
		//====================================================================
		virtual void ReceiveResumeEvent(appResumeEvent& i_Event);

		//====================================================================
		//====================================================================
		virtual void Think();

private:
	g2dSystemDX9* m_pSystem;
	g3dSystem* m_pSystem3D;
	g3dSceneRenderer* m_pRenderer;
	g3dViewer* m_pViewer;
};

MyApp::MyApp()  : m_pSystem(NULL), m_pSystem3D(NULL), m_pRenderer(NULL), m_pViewer(NULL)
{
}

MyApp::~MyApp() 
{
}

void MyApp::Think()
{
	if( demModeManager::IsEmpty() )
		this->Exit();
	else
		demModeManager::Think();
}

void MyApp::ReceiveStartEvent(appStartEvent& i_Event)
{
	DBG_LOG0("Start Event");

	m_pSystem = new g2dSystemDX9();

	// This window is owned by the system
	g2dWindow *window = NULL;
	if( l_bWindowed )
		window = m_pSystem->CreateAppWindow(640, 480, 100, 100);
	else
		window = m_pSystem->CreateFullScreen(1024, 768, 16);

	matShaderMgr::SetUseShaderArray(true);
	m_pSystem3D = new g3dSystemDX9();
	DBG_WARNING1("Video Hardware Name: %s", m_pSystem3D->GetVideoAdapterName());

	GraphicsLayer::InitGraphics();
	GraphicsDX9Layer::InitGraphics();

	// set up renderer and viewer for this window
	//m_pRenderer = new g3dSceneRendererDX9();
	m_pRenderer = new shdwShadowLayerRendererDX9();
	m_pViewer = new g3dViewer(window, m_pRenderer);
	m_pViewer->SetBackgroundColor( g2dRGBColor(0x30, 0x80, 0xa0) );


	// The following modes work. Some may be commented out temporarily
	// in order to work on a specific mode. Comment them all in
	// to see the full test.
//	demModeManager::Push(new demG3dTestFAOgen(*m_pViewer));
	
//	demModeManager::Push(new demG3dTestDepthPeeling(*m_pViewer));

//	demModeManager::Push(new demG3dTestGPUAmbientOcclusion(*m_pViewer));
	demModeManager::Push(new demG3dTestSkinShader(*m_pViewer));
	demModeManager::Push(new demG3dTestHairShader(*m_pViewer));

	demModeManager::Push(new demG3dTestHDRAmbientEnvMap(*m_pViewer));
	demModeManager::Push(new demG3dTestAmbientEnvMap(*m_pViewer));

//	demModeManager::Push(new demG3dTestFX(*m_pViewer, new shdwFXTestRendererD3D()));
//	demModeManager::Push(new demG3dTestFX(*m_pViewer, new shdwTestShaderRendererD3D()));

	demModeManager::Push(new demG3dTestSubWindow(*m_pSystem, *m_pViewer));
	demModeManager::Push(new demG3dTestProjectedLights(*m_pViewer));
	demModeManager::Push(new demG3dTestModelMaterial(*m_pViewer));
	demModeManager::Push(new demG3dTestFog(*m_pViewer));
	demModeManager::Push(new demG3dTestPivot(*m_pViewer));
	demModeManager::Push(new demG3dTestHierarch(*m_pViewer));
	demModeManager::Push(new demG3dTestModelSpace(*m_pViewer));
	demModeManager::Push(new demG3dTestRenderToTexture(*m_pViewer));
	demModeManager::Push(new demG3dTestPickBuffer(*m_pViewer));
	demModeManager::Push(new demG3dTestPreLit(*m_pViewer));
	demModeManager::Push(new demG3dTestPrims(*m_pViewer));
	demModeManager::Push(new demG3dTestAnims(*m_pViewer));
	demModeManager::Push(new demG3dTestDrawStyle(*m_pViewer));
	demModeManager::Push(new demG3dTestRenderState(*m_pViewer));
	demModeManager::Push(new demG3dTestLights(*m_pViewer));

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////

	// The following modes do not work. They were either not ported 
	// during a revision of the graphics engine, or they do not
	// represent a feature of the engine anymore.

	// ambientocclusion and effectshader rely on the ability to
	// register and load a specific shader outside the "shader array"...

	//demModeManager::Push(new demG3dTestAmbientOcclusion(*m_pViewer));
	//demModeManager::Push(new demG3dTestEffectShader(*m_pViewer));
	//demModeManager::Push(new demG3dTestPixelShader(*m_pViewer));
	//demModeManager::Push(new demG3dTestVertexShader(*m_pViewer));
	//demModeManager::Push(new demG3dTestBumpMap(*m_pViewer));
	//*** demModeManager::Push(new demG3dTestProgressive(*m_pViewer));
	//**? demModeManager::Push(new demG3dTestSpriteGroup(*m_pViewer));
	//*** demModeManager::Push(new demG3dTestMorph(*m_pViewer));
	//demModeManager::Push(new demG3dTestCubeMap(*m_pViewer));
	//demModeManager::Push(new demG3dTestTextureCompression(*m_pViewer));
	//demModeManager::Push(new demG3dTestTextureReduce(*m_pViewer));
	//demModeManager::Push(new demG3dTestTextures(*m_pViewer));
	//*** demModeManager::Push(new demG3dTestVertexColoring(*m_pViewer);
}

void MyApp::ReceiveStopEvent(appStopEvent& i_Event)
{
	delete m_pViewer;
	delete m_pRenderer;
	g2dFontUtil::ReleaseAllFonts();

	GraphicsDX9Layer::CleanUpGraphics();
	GraphicsLayer::CleanUpGraphics();

	delete m_pSystem3D;
	delete m_pSystem;
}

void MyApp::ReceiveSuspendEvent(appSuspendEvent& i_Event)
{
	DBG_LOG0("Suspend Event");
}

void MyApp::ReceiveResumeEvent(appResumeEvent& i_Event)
{
	DBG_LOG0("Resume Event");
}

//====================================================================
//====================================================================
int WINAPI WinMain(	HINSTANCE hInstance,      // handle to current instance
					HINSTANCE hPrevInstance,  // handle to previous instance
					LPSTR lpCmdLine,          // command line
					int nCmdShow)             // show state
{
	appApplicationPAC::SetHINSTANCE(hInstance);

	envPackage::Init();
	dbgPackage::Init();
	fsPackage::Init();
	itPackage::Init();
	appPackage::Init();
	gfPackage::Init();
	GraphicsLayer::Init();
	GraphicsDX9Layer::Init();

	std::string cmd_line = lpCmdLine;
	if( cmd_line.find("/f") != std::string::npos )
		l_bWindowed = false;
	else
		l_bWindowed = true;
				
	try
	{
		MyApp app;

		app.Run();
	}
	catch( const fsFileDoesntExistX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		DBG_WARNING1("File not found: %s", filename.c_str());
	}
	catch( const envExceptionX& i_Ex )
	{
		DBG_WARNING1("Uncaught exception - error code %x", i_Ex.Index());
		throw;
	}
	catch( ... )
	{
		DBG_WARNING0("Uncaught non-Terawatt exception");
		throw;
	}

	GraphicsDX9Layer::CleanUp();
	GraphicsLayer::CleanUp();
	gfPackage::CleanUp();
	appPackage::CleanUp();
	itPackage::CleanUp();
	fsPackage::CleanUp();
	dbgPackage::CleanUp();
	envPackage::CleanUp();
	return 13;
}
