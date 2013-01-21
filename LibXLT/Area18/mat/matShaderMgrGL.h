#pragma once
#include "graphics\mat\matshadermgr.hpp"

class shdrCgPipeline;

class matShaderMgrGL :
	public matShaderMgrImpl
{
public:
	matShaderMgrGL(void);
	virtual ~matShaderMgrGL(void);

	virtual void RegisterEffects(const fsLocator &i_ShaderDir,
			std::map<std::string, matShaderInfo>& io_ShaderMap);
	virtual void RegisterUserShaders(const fsLocator& i_ShaderDir, 
			std::vector<matShaderInfo>& o_Shaders);

	virtual void UnloadEffect(const fsLocator& i_PathToShader,
		std::map<fsLocator, matShaderEffect*>& io_ShaderMap);

	virtual matShaderEffect* LoadEffect(const fsLocator& i_PathToShader,
		std::map<fsLocator, matShaderEffect*>& io_ShaderMap);

private:
	shdrCgPipeline* mBlinn;

	matShaderInfo registerShader(const std::string& shaderName);

};

