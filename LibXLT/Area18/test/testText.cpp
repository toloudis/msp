#include "testText.h"
#include "GLProgram.h"
#include "GLShader.h"

#include "Graphics/cam/camCamera.hpp"
#include "Graphics/g3d/g3dType.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Area18/Area18Layer.hpp"
#include "Area18/ogl/oglTextDraw.h"
#include "Area18/mesh/meshTriMeshFrag.hpp"
#include "Area18/mesh/meshRenderer.hpp"

testText::testText(void)
:	m_pProgram(NULL),
	m_pVertSh(NULL),
	m_pFragSh(NULL),
	mTweakBar(NULL)
{
}

testText::~testText(void)
{
	TwDeleteBar(mTweakBar);
	delete m_pProgram;
	delete m_pVertSh;
	delete m_pFragSh;
}

void testText::Setup()
{
	mTweakBar = TwNewBar("Test Tweakbar");
	float alpha = 0;
	TwSetParam(mTweakBar, NULL, "alpha", TW_PARAM_FLOAT, 1, &alpha);
//	char s1[64] = "a static string"; // sizeof(s1) is 64
//	TwAddVarRW(mTweakBar, "s1", TW_TYPE_CSSTRING(sizeof(s1)), s1, ""); // must pass s1 (not &s1)

	mText = new oglTextDraw(Area18Layer::GetDevice(0));

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
//	m_pProgram->BindAttribLocation(1, "InNormal"); 
//	m_pProgram->BindAttribLocation(2, "InUV"); 
//	m_pProgram->BindAttribLocation(3, "InBinormal"); 
//	m_pProgram->BindAttribLocation(4, "InBitangent"); 

	m_pProgram->Link();

	m_pProgram->Use(); 

//	m_pFrag = g3dPrimitiveFragmentUtil::CreateTexturedSphere( 0.5, 8, 8 );
//	m_pFrag = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(	1, 1, 1, 1);

	float* vert = new float[12]; // vertex array 
	float* col = new float[9]; // color array 

	float z = -1.0f;
	vert[0] = 0.0f; vert[1] = 0.8f; vert[2] =z; vert[3] = 1; 
	vert[4] =-0.8f; vert[5] =-0.8f; vert[6] =z; vert[7] = 1; 
	vert[8] = 0.8f; vert[9] =-0.8f; vert[10] =z; vert[11] = 1;

	col[0] = 1.0f; col[1] = 0.0f; col[2] = 0.0f; 
	col[3] = 0.0f; col[4] = 1.0f; col[5] = 0.0f; 
	col[6] = 0.0f; col[7] = 0.0f; col[8] = 1.0f; 

	/* Allocate and assign a Vertex Array Object to our handle */
    glGenVertexArrays(1, &m_vaoID);
    /* Bind our Vertex Array Object as the current used object */
    glBindVertexArray(m_vaoID);

	GLushort inds[3] = {0,1,2};

		glGenBuffers(1, &m_vboID);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
		glBufferData(GL_ARRAY_BUFFER, 3 * (sizeof(float)*4), vert, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, 0);
		glEnableVertexAttribArray(0);
		
		glGenBuffers(1, &m_eboID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*sizeof(GLushort), inds, GL_STATIC_DRAW);
		
	delete [] vert; 
	delete [] col;

	mCamera = new camCamera;
	mCamera->SetClip(0.1f, 10);
	mCamera->SetAspect(width(), height());
	mCamera->LookAt(maPoint3d(0,0,5), maPoint3d(0,0,-1), maVector3d(0,1,0));
}

void testText::draw()
{
	glViewport(0,0,width(), height());
	mCamera->SetClip(0.1f, 10);
	mCamera->SetAspect(width(), height());
	Render(mCamera);
	TwDraw();
}

void testText::Render(camCamera* i_pCamera)
{
	glClearColor(0.5,0.5,1,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//glOrtho(0,width(),0,height(),-1,1);
//	mText->Draw( "Hello world.",
//			0,0,
//			4,
//			maFloatRGBA(1,1,1,1));

//	/* Bind our modelmatrix variable to be a uniform called mvpmatrix in our shaderprogram */
//	maMatrix4x4 mcam;
//	i_pCamera->GetCameraMatrix(mcam);
//	m_pProgram->SetUniformMatrix("ModelviewMatrix", mcam.Ptr(), 4, 4, false);
//
//	maMatrix4x4 mproj;
//	i_pCamera->GetProjectionMatrix(mproj);
//	m_pProgram->SetUniformMatrix("ProjectionMatrix", mproj.Ptr(), 4, 4, false);
//
//    glBindVertexArray(m_vaoID);
//	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboID);
//
//	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, (void*)0);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);

	//int n = meshTriMeshFrag::GetRenderer()->Render( (meshTriMeshFrag*)m_pFrag, NULL, NULL, NULL);
}
