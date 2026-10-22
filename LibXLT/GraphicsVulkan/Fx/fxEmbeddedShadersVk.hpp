/*****************************************************************************
**  fxEmbeddedShadersVk.hpp
**
**      The plain-HLSL effects built into GraphicsVulkan. At build time
**      EmbedShadersVk.ps1 compiles every *.effect.json under
**      Eff/private/Shaders (GraphicsDX11's shaders, copied unchanged) with
**      DXC to SPIR-V and embeds the manifests and SPIR-V; this looks them up
**      by effect name, the manifest file name without .effect.json (e.g.
**      "Blur"), as GraphicsDX11's fxEmbeddedShaders does.
\****************************************************************************/
#ifdef GVK_FXEMBEDDEDSHADERSVK_HPP
#error fxEmbeddedShadersVk.hpp multiply included
#endif
#define GVK_FXEMBEDDEDSHADERSVK_HPP

#ifndef GVK_FXEFFECTVK_HPP
#include "GraphicsVulkan/Fx/fxEffectVk.hpp"
#endif

#include <cstddef>
#include <memory>
#include <string>

namespace gvk
{
	struct fxEmbeddedBytecode
	{
		const char* m_Entry;
		const char* m_Profile;			// as the manifest gives it, e.g. ps_5_0
		const unsigned char* m_pData;	// SPIR-V
		size_t m_Size;
	};

	struct fxEmbeddedEffect
	{
		const char* m_Name;			// effect name, e.g. "Blur"
		const char* m_Source;		// manifest path relative to the shader root
		const char* m_Manifest;		// .effect.json text
		const fxEmbeddedBytecode* m_pBytecode;
		int m_NumBytecode;
		const char* m_BoolNames;	// names the HLSL declares bool, space-separated (see fxEffectVk::Create)
	};

	namespace fxEmbeddedShadersVk
	{
		extern const fxEmbeddedEffect k_Effects[];
		extern const int k_NumEffects;

		// nullptr if there is no embedded effect of that name
		const fxEmbeddedEffect* Find(const std::string& i_Name);

		// Bytecode lookup for one embedded effect, for fxEffectVk::Create
		fxBytecodeSource BytecodeSource(const fxEmbeddedEffect& i_Effect);

		// Parse the manifest and build the effect (i_pDevice NULL: reflection only).
		std::unique_ptr<fxEffectVk> CreateVk(const fxEmbeddedEffect& i_Effect, vkdDevice* i_pDevice, std::string& o_Error);
	}
}
