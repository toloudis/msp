#include "testPrimitive.h"
#include "GLProgram.h"
#include "GLShader.h"

#include "Core/App/appTime.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/g3d/g3dType.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Area18/mesh/meshTriMeshFrag.hpp"
#include "Area18/mesh/meshRenderer.hpp"

testPrimitive::testPrimitive(void)
:	m_pProgram(NULL),
	m_pVertSh(NULL),
	m_pFragSh(NULL),
	mCamera(NULL),
	mFrameNumber(0),
	mLastTime(0)
{
	mLastTime = appTime::getTimeNs();
}

testPrimitive::~testPrimitive(void)
{
	delete m_pProgram;
	delete m_pVertSh;
	delete m_pFragSh;
	delete mCamera;
}

void testPrimitive::Setup()
{
	m_pVertSh = new oglShader(GL_VERTEX_SHADER); 
	m_pFragSh = new oglShader(GL_FRAGMENT_SHADER); 

	char* shaderDir = "D:\\dev\\CompletelyDifferent\\LibXLT\\Area18\\test\\";
	std::string vs = shaderDir;
	vs += "minimal.vert";
	m_pVertSh->Load(vs.c_str());
	std::string fs = shaderDir;
	fs += "minimal.frag";
	m_pFragSh->Load(fs.c_str()); 

	m_pVertSh->Compile();

	m_pFragSh->Compile();

	m_pProgram = new oglProgram(); 
	m_pProgram->AttachShader(m_pVertSh); 
	m_pProgram->AttachShader(m_pFragSh); 
	m_pProgram->BindAttribLocation(0, "InPosition"); 
	m_pProgram->Link();

	m_pProgram->Use(); 

	mRect = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(1, 1, 1, 1);
	mSphere = g3dPrimitiveFragmentUtil::CreateTexturedSphere(1, 16,16);
	mCone = g3dPrimitiveFragmentUtil::CreateCone( 1, 2, 16);
	mCylinder = g3dPrimitiveFragmentUtil::CreateCylinder( 1, 1, 2, 16 );
	mCube = g3dPrimitiveFragmentUtil::CreateTexturedCube(1);

	mCamera = new camCamera;
	mCamera->SetClip(0.1f, 10);
	mCamera->SetAspect(width(), height());

	matShaderParamUI val("Red", matShaderParamUI::e_Slider, 1.0, 0.0, 1.0);
	this->mTestUI.mUIList.push_back(val);
	matShaderParamUI val2("Color", matShaderParamUI::e_ColorPicker, 1.0, 0.0, 1.0);
	this->mTestUI.mUIList.push_back(val2);
}

void testPrimitive::draw()
{
	glViewport(0,0,width(), height());
	mCamera->SetClip(0.1f, 10);
	mCamera->SetAspect(width(), height());
	Render(mCamera);
	mFrameNumber++;
}

void testPrimitive::Render(camCamera* i_pCamera)
{
	int64_t t = appTime::getTimeNs();
	double dt = (t - mLastTime)/1000000.0;
	mLastTime = t;

//	glDisable(GL_CULL_FACE);
	glClearColor(1,1,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	i_pCamera->LookAt(maPoint3d(0,0,5), maPoint3d(0,0,-1), maVector3d(0,1,0));

	m_pProgram->Use();

	maMatrix4x4 mproj;
//	mproj.Identity();
	i_pCamera->GetProjectionMatrix(mproj);
	m_pProgram->SetUniformMatrix("ProjectionMatrix", mproj.Ptr(), 4, 4, false);

	float v[4] = {1,0,0,1};
	v[0] = this->mTestUI.mUIList[0].m_fval[0];

	v[0] = this->mTestUI.mUIList[1].m_fval[0];
	v[1] = this->mTestUI.mUIList[1].m_fval[1];
	v[2] = this->mTestUI.mUIList[1].m_fval[2];
	v[3] = this->mTestUI.mUIList[1].m_fval[3];
	m_pProgram->SetUniform("color", v, 4, 1);
	int n;

	maMatrix4x4 mcam;
//	mcam.Identity();
	i_pCamera->GetCameraMatrix(mcam);

	//float osc = sin(mLastTime/100000000.0) * 0.5f;
	float osc = sin(appTime::GetTime()) * 0.5f;
	m_pProgram->SetUniformMatrix("ModelviewMatrix", (mcam * maMatrix4x4::MakeTranslate(4+osc,0,0)).Ptr(), 4, 4, false);
	n = meshTriMeshFrag::GetRenderer()->Render( (meshTriMeshFrag*)mRect, NULL, NULL, NULL);

	m_pProgram->SetUniformMatrix("ModelviewMatrix", (mcam * maMatrix4x4::MakeTranslate(-4+osc,0,0)).Ptr(), 4, 4, false);
	n = meshTriMeshFrag::GetRenderer()->Render( (meshTriMeshFrag*)mSphere, NULL, NULL, NULL);

	m_pProgram->SetUniformMatrix("ModelviewMatrix", (mcam * maMatrix4x4::MakeTranslate(0,4+osc,0)).Ptr(), 4, 4, false);
	n = meshTriMeshFrag::GetRenderer()->Render( (meshTriMeshFrag*)mCone, NULL, NULL, NULL);

	m_pProgram->SetUniformMatrix("ModelviewMatrix", (mcam * maMatrix4x4::MakeTranslate(0,-4+osc,0)).Ptr(), 4, 4, false);
	n = meshTriMeshFrag::GetRenderer()->Render( (meshTriMeshFrag*)mCylinder, NULL, NULL, NULL);
}


