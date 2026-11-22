/*****************************************************************************
**  fxEmbeddedShaders.cpp
\****************************************************************************/

#include "GraphicsDX11/Fx/fxEmbeddedShaders.hpp"

#include <string.h>

// Generated before compiling by EmbedShaders.ps1 (see GraphicsDX11.vcxproj)
#include "Generated/fxEmbeddedShaders.inc"

//------------------------------------------------------------------------
//------------------------------------------------------------------------
const fxEmbeddedEffect* fxEmbeddedShaders::Find(const std::string& i_Name)
{
	for (int i = 0; i < k_NumEffects; i++)
		if (_stricmp(k_Effects[i].m_Name, i_Name.c_str()) == 0)
			return &k_Effects[i];
	return nullptr;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
std::unique_ptr<fxEffectDX11> fxEmbeddedShaders::CreateDX11(const fxEmbeddedEffect& i_Effect,
	ID3D11Device* i_pDevice, std::string& o_Error)
{
	fxEffectDesc desc;
	if (!fxEffectDesc::Parse(i_Effect.m_Manifest, desc, o_Error))
	{
		o_Error = std::string(i_Effect.m_Source) + ": " + o_Error;
		return nullptr;
	}

	fxBytecodeSource bytecode = [&i_Effect](const std::string& i_Entry, const std::string& i_Profile,
											fxBytecode& o_Bytecode)
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
		return false;
	};
	return fxEffectDX11::Create(i_pDevice, desc, bytecode, o_Error);
}
