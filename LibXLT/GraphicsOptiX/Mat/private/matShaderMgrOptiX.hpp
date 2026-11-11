/*****************************************************************************
**  matShaderMgrOptiX.hpp
**
**      matShaderMgrOptiX registers and loads the OptiX backend's material
**	and post effects (matShaderMgr).
**
**	Stub: registers no effects and loads none (NULL).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_MATSHADERMGROPTIX_HPP
#error matShaderMgrOptiX.hpp multiply included
#endif
#define GOX_MATSHADERMGROPTIX_HPP

#ifndef MAT_SHADERMGR_HPP
#include "Graphics/Mat/matShaderMgr.hpp"
#endif

#include <map>
#include <string>
#include <vector>

namespace gox
{
	class matShaderMgrOptiX : public matShaderMgrImpl
	{
		public:
			virtual void RegisterEffects(const fsLocator& i_ShaderDir,
										 std::map<std::string, matShaderInfo>& io_ShaderMap);
			virtual void RegisterSelectableShaders(std::vector<matShaderInfo>& o_Materials,
												   std::vector<matShaderInfo>& o_PostEffects);
			virtual void UnloadEffect(const fsLocator& i_PathToShader,
									  std::map<fsLocator, matShaderEffect*>& io_ShaderMap);
			virtual matShaderEffect* LoadEffect(const fsLocator& i_PathToShader,
												std::map<fsLocator, matShaderEffect*>& io_ShaderMap);
	};
}
