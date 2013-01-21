#include "oglTextDraw.h"

#include "oglDevice.hpp"
#include "oglTexture2d.h"
#include "Area18/font/fontstash.h"
#include "Area18/shdr/shdrPipeline.hpp"
#include "Area18/shdr/shdrPS.hpp"
#include "Area18/shdr/shdrVS.hpp"

#include "Core/tinyxml2.h"
#include "Core/Dbg/dbgMsg.hpp"

#include <boost/foreach.hpp>

#include <assert.h>
#include <string>

static char* vertShaderSrc = "\
#version 410\n\
in vec2 vtxin;\n\
in vec2 uvin;\n\
out vec2 uv;\n\
out gl_PerVertex\n\
{\n\
    vec4 gl_Position;\n\
};\n\
void main()\n\
{\n\
	uv = uvin;\n\
	gl_Position = vec4(vtxin,0,1);\n\
}\n\
";

static char* fragShaderSrc = "\
#version 410\n\
in vec2 uv;\n\
uniform sampler2D gFont;\n\
uniform vec4 gColor;// = vec4(1,1,1,1);\n\
out vec4 fragColor;\n\
void main()\n\
{\n\
	fragColor = gColor*texture(gFont, uv);\n\
}\n\
";

oglTextDraw::oglTextDraw(oglDevice* iDevice)
	: mVS(new shdrVS(iDevice->mContext, shdrVS::shdrSource, vertShaderSrc))
	, mPS(new shdrPS(iDevice->mContext, shdrPS::shdrSource, fragShaderSrc))
{
	//  assert for text drawing contract: (see fontstash.c, flush_draw)
	glBindAttribLocation(mVS->GetShader(), 0, "vtxin");
	glBindAttribLocation(mVS->GetShader(), 1, "uvin");
	glUseProgram(mPS->GetShader());
	// gFont is texture unit 0
	GLint i = glGetUniformLocation(mPS->GetShader(), "gFont");
	glUniform1i(i, 0);
	// color is white for now
	i = glGetUniformLocation(mPS->GetShader(), "gColor");
	glUniform4f(i, 1,1,1,1);
	glUseProgram(0);
	CHECKGLERROR();
	mPipeline = new shdrPipeline();
	mPipeline->Attach(mVS);
	mPipeline->Attach(mPS);

	stash = sth_create(512,512);
	if (!stash)
	{
		DBG_LOG("Could not create stash.");
	}
	// Load the remaining truetype fonts directly.
	if (!(droidRegular = sth_add_font(stash,
		"D:\\dev\\CompletelyDifferent\\LibXLT\\Area18\\font\\DroidSerif-Regular.ttf")))
	{
		DBG_LOG("Could not create font.");
	}

	CHECKGLERROR();
}


oglTextDraw::~oglTextDraw(void)
{
	delete mPipeline;
	delete mPS;
	delete mVS;
}

void oglTextDraw::Draw(const std::string& s, int x, int y, float scale, const maFloatRGBA& color)
{
	float sx = 100; 
	float sy = 250;


//	glOrtho(0,width,0,height,-1,1);
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
//	glColor4ub(255,255,255,255);
	glUseProgram(0);

	//mPipeline->Bind(NULL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	sth_begin_draw(stash);
		
	float dx, dy;
	dx = sx; dy = sy;
	sth_draw_text(stash, droidRegular, 24.0f, dx, dy, s.c_str(), &dx);

	sth_end_draw(stash);
}
