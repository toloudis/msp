#include "Area18/test/testAdditiveLightsScene.hpp"

#include "Area18/g3d/g3dSceneGlobal.hpp"
#include "Area18/g3d/g3dSceneRenderUtil.hpp"
#include "Area18/mesh/meshRenderer.hpp"
#include "Area18/mesh/meshTriMeshFrag.hpp"
#include "Area18/rndr/rndrNode.h"
#include "Area18/rndr/rndrDrawCall.h"
#include "Area18/ogl/oglDevice.hpp"
#include "Area18/ogl/oglTexture2d.h"
#include "Area18/shdr/shdrLambertPS.h"
#include "Area18/shdr/shdrDefaultVS.hpp"
#include "Area18/shdr/shdrPipeline.hpp"
#include "Area18/shdr/shdrShaders.hpp"
#include "Area18/Area18Layer.hpp"

#include "Core/Fs/fsLocator.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"

testBlendRenderPass::testBlendRenderPass(oglContext* iDevice)
	: rndrPass(iDevice)
{
}
testBlendRenderPass::~testBlendRenderPass()
{
}
void testBlendRenderPass::pushGraphicsState()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
}
void testBlendRenderPass::popGraphicsState()
{
	glDisable(GL_BLEND);
}

testAdditiveLightsScene::testAdditiveLightsScene(oglContext* i_pDevice)
: mDefaultRenderer(i_pDevice)
{
	m_pCam = new camCamera;
	// right handed camera would be looking in -Z direction
	// left handed camera would be looking in +Z direction
	m_pCam->LookAt(maPoint3d(0,0,3), maPoint3d(0,0,0), maPoint3d(0,1,0));

	m_pSphere = (meshTriMeshFrag*)g3dPrimitiveFragmentUtil::CreateTexturedSphere(0.5, 16, 16);

//	mTexture.reset(rndrTexture::createFromFile(itString("E:\\MPW-49993.jpeg"), i_pDevice));
	mTexture.reset(oglTexture2d::createFromFile("E:\\LarryBird-1.png", Area18Layer::GetDevice(0)));
	mRoot = new rndrNode(NULL, mDefaultRenderer);
}

testAdditiveLightsScene::~testAdditiveLightsScene(void)
{
	delete m_pCam;
	delete m_pSphere;

	delete mRoot;
}

void testAdditiveLightsScene::GetBounds(maAxisBox& o_Bounds)
{
	o_Bounds = m_pSphere->GetBoundingBox();
}

void testAdditiveLightsScene::Render(camCamera* i_pCamera)
{
	// Set the camera and projection transform
	g3dSceneGlobal sceneGlobal;
	g3dSceneRenderUtil::SetViewingTransforms(*i_pCamera, sceneGlobal);

	// update lights from ui:
	mPointLight0.SetPosition(maPoint3d(1,0,0));
	mPointLight0.SetIntensity(maFloatRGBA(1,1,1,1));

	mPointLight1.SetPosition(maPoint3d(1,1,0));
	mPointLight1.SetIntensity(maFloatRGBA(1,1,1,1));

	boost::shared_ptr<shdrDefaultVS> pVS = Area18Layer::GetDevice(0)->m_Shaders->m_DefaultVS;
	//boost::shared_ptr<shdrColorPS> pPS = Area18Layer::GetDevice(0)->m_Shaders->m_ColorPS;
	boost::shared_ptr<shdrLambertPS> pPS = Area18Layer::GetDevice(0)->m_Shaders->m_LambertPS;

	maMatrix4x4 o2w;
//	o2w.ScaleBy(50,50,50);
//	o2w.TranslateBy(1.5f, 75, 1.8f);
//	maMatrix4x4 w2o = o2w;
//	w2o.Invert();

	// put data into VS
	cbVS vsData;
	vsData.SetWorldTransform(o2w*sceneGlobal.GetCameraTransform());
	vsData.SetProjectionTransform(sceneGlobal.GetProjectionTransform());

	mRoot->clearGraph();

	std::vector<rndrDrawCall*> dcs;

	// put material data into PS
	shdrLambertParams psData;
	psData.SetEyePos(maVector4d(sceneGlobal.GetCameraPos()));
	psData.SetColor(maFloatRGBA(1,0,0,1));
	psData.SetTexture(mTexture);
	// put light data into PS
	psData.SetLight(&mPointLight0);

	shdrPipeline sp;
	sp.Attach(pVS.get());
//	sp.mVSData = &vsData;
	sp.Attach(pPS.get());
//	sp.mPSData = &psData;

	rndrDrawCall dc;
	dc.mFrag = m_pSphere;
	dc.mEffect = &sp;
	dcs.push_back(&dc);
	
	shdrLambertParams psData2;
	psData2.SetEyePos(maVector4d(sceneGlobal.GetCameraPos()));
	psData2.SetColor(maFloatRGBA(1,0,0,1));
	psData2.SetTexture(mTexture);
	// put light data into PS
	psData2.SetLight(&mPointLight1);

	shdrPipeline sp2;
	sp2.Attach(pVS.get());
//	sp2.mVSData = &vsData;
	sp2.Attach(pPS.get());
//	sp2.mPSData = &psData2;

	rndrDrawCall dc2;
	dc2.mFrag = m_pSphere;
	dc2.mEffect = &sp2;
	dcs.push_back(&dc2);
	
	mRoot->addDrawCalls(dcs);

	mRoot->execute();
#if 0
	//////////////////////////////////////
	//////////////////////////////////////
	IFW1Factory *pFW1Factory;
	HRESULT hResult = FW1CreateFactory(FW1_VERSION, &pFW1Factory);
	
	IFW1FontWrapper *pFontWrapper;
	hResult = pFW1Factory->CreateFontWrapper(i_pDevice->m_pDevice, L"Arial", &pFontWrapper);
	
//	pFontWrapper->DrawString(
//		i_pDevice->m_pDeviceContext,
//		L"Text",// String
//		12.0f,// Font size
//		100.0f,// X position
//		50.0f,// Y position
//		0xff0099ff,// Text color, 0xAaBbGgRr
//		0// Flags
//	);
	
	pFontWrapper->Release();
	pFW1Factory->Release();
	//////////////////////////////////////
	//////////////////////////////////////
#endif
}

