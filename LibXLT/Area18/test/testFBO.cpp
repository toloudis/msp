#include "testFBO.h"
#include "GLProgram.h"
#include "GLShader.h"

#include "Graphics/cam/camCamera.hpp"
#include "Graphics/g3d/g3dType.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Area18/mesh/meshTriMeshFrag.hpp"
#include "Area18/mesh/meshRenderer.hpp"

testFBO::testFBO(void)
:	m_pProgram(NULL),
	m_pVertSh(NULL),
	m_pFragSh(NULL)
	, mDepthBuf(0)
	, mColorTex(0)
{
}

testFBO::~testFBO(void)
{
	delete m_pProgram;
	delete m_pVertSh;
	delete m_pFragSh;
}

void checkFBStatus()
{
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		switch(status) {
		case GL_FRAMEBUFFER_UNDEFINED: DBG_ERROR("target is the default framebuffer, but the default framebuffer does not exist."); break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: DBG_ERROR("any of the framebuffer attachment points are framebuffer incomplete."); break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: DBG_ERROR("the framebuffer does not have at least one image attached to it."); break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: DBG_ERROR("the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAWBUFFERi."); break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: DBG_ERROR("GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER."); break;
		case GL_FRAMEBUFFER_UNSUPPORTED: DBG_ERROR("the combination of internal formats of the attached images violates an implementation-dependent set of restrictions.");  break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: DBG_ERROR("the value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; if the value of GL_TEXTURE_SAMPLES is the not same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES."); 
			DBG_ERROR("also returned if the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures.");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: DBG_ERROR("any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target."); break; 
		}
	}
}

void testFBO::doResize(int w, int h)
{
	glDeleteRenderbuffers(1, &mDepthBuf);
	glGenRenderbuffers(1, &mDepthBuf);
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, w, h);
	CHECKGLERROR();

	glDeleteTextures(1, &mColorTex);
	glGenTextures(1, &mColorTex);
	glBindTexture(GL_TEXTURE_2D, mColorTex);
//	float* pixels = new float[4*w*h];
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, pixels);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	CHECKGLERROR();

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBO);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBuf);
	CHECKGLERROR();
	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mColorTex, 0);
	CHECKGLERROR();

	checkFBStatus();
}


void testFBO::Setup()
{

	glGenFramebuffers(1, &mFBO);

	doResize(1,1);

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
	m_pFrag = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(1, 1, 1, 1);
//	m_pFrag = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(2, 2, 1, 1);
}

void testFBO::draw()
{
	glViewport(0,0,width(), height());
	camCamera c;
	c.SetClip(0.1f, 10);
	c.SetAspect(width(), height());
	Render(&c);
}

void testFBO::Render(camCamera* i_pCamera)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBO);
	GLenum fboBuf[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, fboBuf);
	glViewport(0,0,width(), height());

//	float color[] = {1,1,0,1};
//	float depthclear[] = {1};
//	glClearBufferfv(GL_COLOR, GL_DRAW_BUFFER0, color);
//	glClearBufferfv(GL_DEPTH, 0, depthclear);
	glClearColor(1,1,0,1);
	glClearDepth(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw something
	i_pCamera->LookAt(maPoint3d(0,0,5), maPoint3d(0,0,-1), maVector3d(0,1,0));
	m_pProgram->Use();
	maMatrix4x4 mcam;
	i_pCamera->GetCameraMatrix(mcam);
	m_pProgram->SetUniformMatrix("ModelviewMatrix", mcam.Ptr(), 4, 4, false);
	maMatrix4x4 mproj;
	i_pCamera->GetProjectionMatrix(mproj);
	m_pProgram->SetUniformMatrix("ProjectionMatrix", mproj.Ptr(), 4, 4, false);
	int n = meshTriMeshFrag::GetRenderer()->Render( (meshTriMeshFrag*)m_pFrag, NULL, NULL, NULL);

	// make the default context's framebuffer be current
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	GLenum windowBuf[] = {GL_BACK_LEFT};
	glDrawBuffers(1, windowBuf);
	glViewport(0,0,width(), height());

	// draw something to window, to display the rendered fbo thing.

	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// let's try making our fbo the read framebuffer, and blit.
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mFBO);
	glBlitFramebuffer(0,0,width(),height(),0,0,width(),height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

	// now draw the same fb in the lower 1/4 of the window
	glBlitFramebuffer(0,0,width(),height(),0,0,width()/4,height()/4, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}
