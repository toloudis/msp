#include "matShaderMgrGL.h"

#include "Area18/mat/matShaderBaseGL.hpp"
#include "Area18/shdr/shdrPipeline.hpp"
#include "Area18/shdr/shdrShader.hpp"
#include "Area18/shdr/shdrUtil.hpp"

matShaderMgrGL::matShaderMgrGL(void)
{
}


matShaderMgrGL::~matShaderMgrGL(void)
{
}

matShaderInfo matShaderMgrGL::registerShader(const std::string& shaderName)
{
	matShaderInfo info;
	info.m_Name = shaderName.c_str();
	info.m_DataTemplate = NULL;
	info.m_pEffect = NULL;

//	BinaryShaderLoader<EFF_TYPE>* loader = new BinaryShaderLoader<EFF_TYPE>(i_ShaderBits, i_ShaderSizeBytes);
//	loader->LoadShader(i_Name, io_ShaderMap);
//	delete loader;

	shdrPipeline* effect = new shdrPipeline();//NULL, shaderName);
	effect->AttachVS(mVsGeneric);
	effect->AttachPS(mFsTest);

	matShaderEffect* pEffect = NULL;
	if (effect != NULL)
	{
		pEffect = new matShaderBaseGL(fsLocator(), effect, shaderName);
		//DBG_LOG( "Built in shader = " << i_Name.c_str());
		info.m_pEffect = pEffect;
	}
	return info;
}

void matShaderMgrGL::RegisterEffects(const fsLocator &i_ShaderDir,
			std::map<std::string, matShaderInfo>& io_ShaderMap)
{

	// TODO: compile on a particular device?

	std::vector<const fsLocator*> vshaders;
	fsLocator loc1(itString("D:/Projects/SourceCode/LibXLT/Area18/shdr/shaders/Globals.h"));
	vshaders.push_back(&loc1);
	fsLocator loc2(itString("D:/Projects/SourceCode/LibXLT/Area18/shdr/shaders/vsDefault.glsl"));
	vshaders.push_back(&loc2);
	GLuint vsp = shdrUtil::CompileShaderFromFiles( vshaders, GL_VERTEX_SHADER);
	mVsGeneric = new shdrShader(vsp);

	std::vector<const fsLocator*> fshaders;
	fsLocator loc3(itString("D:/Projects/SourceCode/LibXLT/Area18/shdr/shaders/fsDefault.glsl"));
	fshaders.push_back(&loc3);
	GLuint fsp = shdrUtil::CompileShaderFromFiles( fshaders, GL_FRAGMENT_SHADER);
	mFsTest = new shdrShader(fsp);

	matShaderInfo i = registerShader("Simple.fx");
	if (i.m_pEffect != NULL) {
		io_ShaderMap["Simple.fx"] = i;
		io_ShaderMap["Solid.fx"] = i;
		io_ShaderMap["Phong.fx"] = i;
		io_ShaderMap["Blinn.fx"] = i;
	}
}
void matShaderMgrGL::RegisterUserShaders(const fsLocator& i_ShaderDir, 
			std::vector<matShaderInfo>& o_Shaders)
{
}

void matShaderMgrGL::UnloadEffect(const fsLocator& i_PathToShader,
		std::map<fsLocator, matShaderEffect*>& io_ShaderMap)
{
}

matShaderEffect* matShaderMgrGL::LoadEffect(const fsLocator& i_PathToShader,
		std::map<fsLocator, matShaderEffect*>& io_ShaderMap)
{
	return NULL;
}

