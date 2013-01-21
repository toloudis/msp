#include "matShaderMgrGL.h"

#include "Area18/mat/matShaderBaseGL.hpp"
#include "Area18/shdr/shdrPipeline.hpp"

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
	matShaderInfo i = registerShader("Blinn.fx");
	if (i.m_pEffect != NULL) {
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

