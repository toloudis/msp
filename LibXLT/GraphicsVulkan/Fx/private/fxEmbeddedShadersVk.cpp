/*****************************************************************************
**  fxEmbeddedShadersVk.cpp
**
**      see fxEmbeddedShadersVk.hpp
\****************************************************************************/
#include "GraphicsVulkan/Fx/fxEmbeddedShadersVk.hpp"

#include <sstream>
#include <string.h>

// Generated before compiling by EmbedShadersVk.ps1 (see GraphicsVulkan.vcxproj)
#include "Generated/fxEmbeddedShadersVk.inc"

namespace gvk
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const fxEmbeddedEffect* fxEmbeddedShadersVk::Find(const std::string& i_Name)
	{
		for (int i = 0; i < k_NumEffects; i++)
		{
			if (_stricmp(k_Effects[i].m_Name, i_Name.c_str()) == 0)
			{
				return &k_Effects[i];
			}
		}
		return nullptr;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	fxBytecodeSource fxEmbeddedShadersVk::BytecodeSource(const fxEmbeddedEffect& i_Effect)
	{
		return [&i_Effect](const std::string& i_Entry, const std::string& i_Profile, fxBytecode& o_Bytecode)
		{
			for (int i = 0; i < i_Effect.m_NumBytecode; i++)
			{
				const fxEmbeddedBytecode& b = i_Effect.m_pBytecode[i];
				if (i_Entry == b.m_Entry && i_Profile == b.m_Profile)
				{
					o_Bytecode.m_pData = b.m_pData;
					o_Bytecode.m_Size = b.m_Size;
					return true;
				}
			}
			//	a deferred stage of this effect's .hlsl: "<dir>/<hlsl>|<entry>"
			const std::string source = i_Effect.m_Source;
			const std::string dir = source.substr(0, source.find_last_of('/') + 1);
			for (int i = 0; i < k_NumDeferred; i++)
			{
				const std::string deferred = k_Deferred[i];
				const size_t bar = deferred.find('|');
				if (deferred.substr(bar + 1) == i_Entry && deferred.compare(0, dir.size(), dir) == 0)
				{
					o_Bytecode = fxBytecode();
					return true;
				}
			}
			return false;
		};
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	std::unique_ptr<fxEffectVk> fxEmbeddedShadersVk::CreateVk(const fxEmbeddedEffect& i_Effect, vkdDevice* i_pDevice,
															  std::string& o_Error)
	{
		fxEffectDesc desc;
		if (!fxEffectDesc::Parse(i_Effect.m_Manifest, desc, o_Error))
		{
			o_Error = std::string(i_Effect.m_Source) + ": " + o_Error;
			return nullptr;
		}
		std::vector<std::string> bools;
		std::istringstream names(i_Effect.m_BoolNames ? i_Effect.m_BoolNames : "");
		for (std::string name; names >> name; )
		{
			bools.push_back(name);
		}
		std::unique_ptr<fxEffectVk> effect = fxEffectVk::Create(i_pDevice, desc, BytecodeSource(i_Effect), o_Error, bools);
		if (!effect)
		{
			o_Error = std::string(i_Effect.m_Source) + ": " + o_Error;
		}
		return effect;
	}
}
