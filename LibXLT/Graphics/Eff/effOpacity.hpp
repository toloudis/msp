/*****************************************************************************
**  effOpacity.hpp
**
**      effOpacity is a material's opacity as the passes that draw it with
**	their own shader read it (depth, ZFill, alpha fill, shadow maps, pick,
**	RSM, the preview passes): a map, multiplied into a value, and for a
**	glTF MASK material the cutoff below which pixels are discarded. For a
**	Pbr.fx material the map is the base color map read through its alpha
**	channel; for the others it is the transparency map (its red channel).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef EFF_OPACITY_HPP
#error effOpacity.hpp multiply included
#endif
#define EFF_OPACITY_HPP

class matMaterial;
class matTexture;


//============================================================================
//============================================================================
struct effOpacity
{
	matTexture* m_pMap = nullptr;	// multiplies m_Value; null: none
	bool m_bMapAlpha = false;		// read m_pMap's alpha (PBR's base color map), else its red
	float m_Value = 1.0f;
	bool m_bCutOut = false;			// MASK: discard below m_Cutoff
	float m_Cutoff = 0.5f;

	//------------------------------------------------------------------------
	// The alpha test reference a pass uses for this material, given the one
	// it uses otherwise: for MASK, raised to just below the cutoff, so both
	// pass shader forms (`ref >= alpha` discards, `clip(alpha - ref)`) keep
	// alpha == cutoff
	//------------------------------------------------------------------------
	float AlphaTestRef(float i_PassRef) const;

	//------------------------------------------------------------------------
	// What i_Material gives the passes: nothing (opaque) unless it is
	// transparent (GetHasTransparency) or MASK. With an AlphaMode: OPAQUE
	// nothing; MASK and BLEND the base color map (m_bMapAlpha) and the base
	// color's alpha, MASK with its cutoff. Without: its OpacityTexture /
	// Opacity parameters, or its effect data's transparency texture and value.
	//------------------------------------------------------------------------
	static effOpacity Of(const matMaterial& i_Material);
};
