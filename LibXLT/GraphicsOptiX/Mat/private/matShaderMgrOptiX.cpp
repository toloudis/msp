/*****************************************************************************
**  matShaderMgrOptiX.cpp
**
**      See matShaderMgrOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_MATSHADERMGROPTIX_HPP
#include "GraphicsOptiX/Mat/private/matShaderMgrOptiX.hpp"
#endif

#include <cstddef>

namespace gox
{
	void matShaderMgrOptiX::RegisterEffects(const fsLocator& /*i_ShaderDir*/,
											std::map<std::string, matShaderInfo>& /*io_ShaderMap*/)
	{
	}

	void matShaderMgrOptiX::RegisterSelectableShaders(std::vector<matShaderInfo>& /*o_Materials*/,
													  std::vector<matShaderInfo>& /*o_PostEffects*/)
	{
	}

	void matShaderMgrOptiX::UnloadEffect(const fsLocator& /*i_PathToShader*/,
										 std::map<fsLocator, matShaderEffect*>& /*io_ShaderMap*/)
	{
	}

	matShaderEffect* matShaderMgrOptiX::LoadEffect(const fsLocator& /*i_PathToShader*/,
												   std::map<fsLocator, matShaderEffect*>& /*io_ShaderMap*/)
	{
		return NULL;
	}
}
