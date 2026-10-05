/*****************************************************************************
**  fxEmbeddedShaders.hpp
**
**      The plain-HLSL effects built into the library. At build time
**      EmbedShaders.ps1 compiles every *.effect.json under
**      Eff/private/Shaders with fxc and embeds the manifests and bytecode;
**      this looks them up by effect name, the manifest file name without
**      .effect.json (e.g. "Blur").
\****************************************************************************/

#ifdef FX_EMBEDDEDSHADERS_HPP
#error fxEmbeddedShaders.hpp multiply included
#endif
#define FX_EMBEDDEDSHADERS_HPP

#ifndef FX_EFFECTDX11_HPP
#include "GraphicsDX11/Fx/fxEffectDX11.hpp"
#endif

#include <cstddef>
#include <memory>
#include <string>

struct fxEmbeddedBytecode
{
	const char* m_Entry;
	const char* m_Profile;
	const unsigned char* m_pData;
	size_t m_Size;
};

struct fxEmbeddedEffect
{
	const char* m_Name;			// effect name, e.g. "Blur"
	const char* m_Source;		// manifest path relative to the shader root
	const char* m_Manifest;		// .effect.json text
	const fxEmbeddedBytecode* m_pBytecode;
	int m_NumBytecode;
};

namespace fxEmbeddedShaders
{
	extern const fxEmbeddedEffect k_Effects[];
	extern const int k_NumEffects;

	// nullptr if there is no embedded effect of that name
	const fxEmbeddedEffect* Find(const std::string& i_Name);

	// Parse the manifest and build the D3D11 effect.
	std::unique_ptr<fxEffectDX11> CreateDX11(const fxEmbeddedEffect& i_Effect,
		ID3D11Device* i_pDevice, std::string& o_Error);
}
