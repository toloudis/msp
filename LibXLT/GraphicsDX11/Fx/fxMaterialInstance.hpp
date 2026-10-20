/*****************************************************************************
**  fxMaterialInstance.hpp
**
**      The values one material gives an effect's material parameters: its
**      own copy of the effect's "MaterialParams" constant buffer (register
**      b4 in the material binding model, see Materials/Globals.hlsli).
**
**      Several materials share one effect. Each keeps its block here, and
**      the block is put in place before the material draws, so nothing one
**      material sets leaks into the next. In D3D11 that means copying the
**      block into the effect's buffer; in D3D12/Vulkan the block becomes the
**      instance's own constant buffer view, bound as the material root
**      parameter / descriptor set, and nothing is copied.
\****************************************************************************/

#ifdef FX_MATERIALINSTANCE_HPP
#error fxMaterialInstance.hpp multiply included
#endif
#define FX_MATERIALINSTANCE_HPP

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

class fxEffectDX11;

class fxMaterialInstance
{
public:
	// The name of the per-material constant buffer in converted materials.
	static const char* const k_MaterialBuffer;

	// Starts from the effect's defaults. Invalid if the effect has no such
	// buffer. i_RendererVariables are variables of the block that the
	// renderer sets (found by their semantic: the view matrix, the
	// reflection map flags): Restore leaves them with the value the renderer
	// wrote last, as the effect did before materials had their own blocks.
	// Names not in the block are ignored.
	fxMaterialInstance(fxEffectDX11* i_pEffect,
					   const std::vector<std::string>& i_RendererVariables = std::vector<std::string>(),
					   const std::string& i_Buffer = k_MaterialBuffer);

	bool IsValid() const { return m_Buffer >= 0; }

	// Put this material's values in place, before writing parameters.
	void Restore() const;
	// Keep what was written, after writing parameters.
	void Capture();

private:
	fxEffectDX11* m_pEffect;
	int m_Buffer;
	std::vector<uint8_t> m_Data;
	// Byte ranges (offset, size) of the renderer's variables
	std::vector<std::pair<uint32_t, uint32_t>> m_RendererRanges;
};
