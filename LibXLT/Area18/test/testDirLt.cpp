#include "testDirLt.h"
#include "GLProgram.h"
#include "GLShader.h"

#include "Core/App/appTime.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/g3d/g3dType.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Area18/mesh/meshTriMeshFrag.hpp"
#include "Area18/mesh/meshRenderer.hpp"

#include <boost/filesystem/path.hpp>

testDirLt::testDirLt(void)
:	mProgram(NULL),
	m_pVertSh(NULL),
	m_pFragSh(NULL),
	mCamera(NULL),
	mFrameNumber(0),
	mLastTime(0)
{
	mLastTime = appTime::getTimeNs();
}

testDirLt::~testDirLt(void)
{
	delete mProgram;
	delete m_pVertSh;
	delete m_pFragSh;
	delete mCamera;
}

void testDirLt::Setup()
{
	m_pVertSh = new oglShader(GL_VERTEX_SHADER); 
	m_pFragSh = new oglShader(GL_FRAGMENT_SHADER); 
	mLambert = new oglShader(GL_FRAGMENT_SHADER);
	mDirLt = new oglShader(GL_FRAGMENT_SHADER);

	boost::filesystem::path shaderDir("D:\\dev\\CompletelyDifferent\\LibXLT\\Area18\\test\\");
	m_pVertSh->Load((shaderDir / "minimal.vert").string());
	m_pFragSh->Load((shaderDir / "singleLight.frag").string());
	mLambert->Load((shaderDir / "lambert.frag").string());
	mDirLt->Load((shaderDir / "dirlt.frag").string());

	m_pVertSh->Compile();

	m_pFragSh->Compile();
	mLambert->Compile();
	mDirLt->Compile();

	mProgram = new oglProgram(); 
	mProgram->AttachShader(m_pVertSh); 
	mProgram->AttachShader(m_pFragSh); 
	mProgram->AttachShader(mLambert);
	mProgram->AttachShader(mDirLt);
	mProgram->BindAttribLocation(0, "InPosition"); 
	mProgram->BindAttribLocation(1, "InNormal");
	mProgram->BindAttribLocation(2, "InUV");
	mProgram->Link();

	mProgram->Use(); 

	mRect.reset(g3dPrimitiveFragmentUtil::CreateTexturedRectangle(1, 1, 1, 1));
	mSphere.reset(g3dPrimitiveFragmentUtil::CreateTexturedSphere(1, 16,16));
	mCone.reset(g3dPrimitiveFragmentUtil::CreateCone( 1, 2, 16));
	mCylinder.reset(g3dPrimitiveFragmentUtil::CreateCylinder( 1, 1, 2, 16 ));
	mCube.reset(g3dPrimitiveFragmentUtil::CreateTexturedCube(1));

	mCamera = new camCamera;
	mCamera->SetClip(0.1f, 10);
	mCamera->SetAspect(width(), height());

	matShaderParamUI val0("Color", matShaderParamUI::e_ColorPicker, 1.0, 0.0, 1.0);
	this->mTestUI.mUIList.push_back(val0);
	matShaderParamUI val1("LtColor", matShaderParamUI::e_ColorPicker, 1.0, 0.0, 1.0);
	this->mTestUI.mUIList.push_back(val1);
	matShaderParamUI val2("LtDir", matShaderParamUI::e_Direction, 1.0, 0.0, 1.0);
	this->mTestUI.mUIList.push_back(val2);
}

void testDirLt::draw()
{
	glViewport(0,0,width(), height());
	mCamera->SetClip(0.1f, 10);
	mCamera->SetAspect(width(), height());
	Render(mCamera);
	mFrameNumber++;
}

void testDirLt::Render(camCamera* i_pCamera)
{
	int64_t t = appTime::getTimeNs();
	double dt = (t - mLastTime)/1000000.0;
	mLastTime = t;

//	glDisable(GL_CULL_FACE);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	i_pCamera->LookAt(maPoint3d(0,0,5), maPoint3d(0,0,-1), maVector3d(0,1,0));

	mProgram->Use();

	maMatrix4x4 mproj;
//	mproj.Identity();
	i_pCamera->GetProjectionMatrix(mproj);
	mProgram->SetUniformMatrix("ProjectionMatrix", mproj.Ptr(), 4, 4, false);
	CHECKGLERROR();

//	float v[4] = {1,0,0,1};
//	v[0] = this->mTestUI.mUIList[0].m_fval[0];
//
//	v[0] = this->mTestUI.mUIList[1].m_fval[0];
//	v[1] = this->mTestUI.mUIList[1].m_fval[1];
//	v[2] = this->mTestUI.mUIList[1].m_fval[2];
//	v[3] = this->mTestUI.mUIList[1].m_fval[3];
//	mProgram->SetUniform("color", v, 4, 1);
	int n;

	maMatrix4x4 mcam;
//	mcam.Identity();
	i_pCamera->GetCameraMatrix(mcam);
	maMatrix4x4 mnor = mcam;
	mnor.Invert();
	mnor.Transpose();
	mProgram->SetUniformMatrix("NormalMatrix", mnor.Ptr(), 4, 4, false);
	CHECKGLERROR();

	mProgram->SetUniform("color", mTestUI.mUIList[0].m_fval, 4, 1);
	CHECKGLERROR();
	mProgram->SetUniform("ltColor", mTestUI.mUIList[1].m_fval, 4, 1);
	CHECKGLERROR();
	mProgram->SetUniform("ltDir", mTestUI.mUIList[2].m_fval, 3, 1);
//	uniform mat4 ltModelView;
//	uniform mat4 ltProjection;
	CHECKGLERROR();

	//float osc = sin(mLastTime/100000000.0) * 0.5f;
	float osc = sin(appTime::GetTime()) * 0.5f;
	mProgram->SetUniformMatrix("ModelviewMatrix", (mcam * maMatrix4x4::MakeTranslate(4+osc,0,0)).Ptr(), 4, 4, false);
	n = meshTriMeshFrag::GetRenderer()->Render( (meshTriMeshFrag*)mRect.get(), NULL, NULL, NULL);

	mProgram->SetUniformMatrix("ModelviewMatrix", (mcam * maMatrix4x4::MakeTranslate(-4+osc,0,0)).Ptr(), 4, 4, false);
	n = meshTriMeshFrag::GetRenderer()->Render( (meshTriMeshFrag*)mSphere.get(), NULL, NULL, NULL);

	mProgram->SetUniformMatrix("ModelviewMatrix", (mcam * maMatrix4x4::MakeTranslate(0,4+osc,0)).Ptr(), 4, 4, false);
	n = meshTriMeshFrag::GetRenderer()->Render( (meshTriMeshFrag*)mCone.get(), NULL, NULL, NULL);

	mProgram->SetUniformMatrix("ModelviewMatrix", (mcam * maMatrix4x4::MakeTranslate(0,-4+osc,0)).Ptr(), 4, 4, false);
	n = meshTriMeshFrag::GetRenderer()->Render( (meshTriMeshFrag*)mCylinder.get(), NULL, NULL, NULL);
}


