/****************************************************************************\
**  g3dSingleLightRendering.hpp
**
**      The g3dSingleLightRendering configures the use of
**  rendering passes to combine lighting effects.  Stencil
**	shadows and per-pixel bump mapping both work by having
**	only one light active at a time.  These passes are
**	combined using an additive blend.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_SINGLELIGHTRENDERING_HPP
#error g3dSingleLightRendering.hpp multiply included
#endif
#define G3D_SINGLELIGHTRENDERING_HPP

#include <vector>


//============================================================================
//============================================================================
class g3dLight;


//============================================================================
//============================================================================
namespace g3dSingleLightRendering
{
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	enum ShadowQualityOverride
	{
		SQ_NONE,
		SQ_LOW,
		SQ_MED,
		SQ_HIGH,
		SQ_VERY_HIGH
	};

	//------------------------------------------------------------------------
	// Returns true if shaders are successfully loaded, so that
	// single light rendering is possible on this machine.
	//------------------------------------------------------------------------
	//bool ShadersAvailable();

	//------------------------------------------------------------------------
	// If true, activates single light rendering for stencil shadows
	// and bump mapping.  This is false by default.
	//------------------------------------------------------------------------
	bool GetDoSingleLightRendering();
	void SetDoSingleLightRendering(bool i_Val);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	ShadowQualityOverride GetShadowQualityOverride();
	void SetShadowQualityOverride(ShadowQualityOverride i_Val);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool GetDoDOFPrepPass();
	void SetDoDOFPrepPass(bool i_val);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool GetDoTransparentPass();
	void SetDoTransparentPass(bool i_Val);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool GetDoGlowPass();
	void SetDoGlowPass(bool i_Val);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool GetDoAmbientEnvironmentPass();
	void SetDoAmbientEnvironmentPass(bool i_Val);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool GetDoCubeReflectionGen();
	void SetDoCubeReflectionGen(bool i_Val);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool GetDoPlaneReflectionGen();
	void SetDoPlaneReflectionGen(bool i_Val);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool GetDoReflectionGen();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool GetDoBaking();
	void SetDoBaking(bool i_Val);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool GetDoIsolateReflections();
	void SetDoIsolateReflections(bool i_Val);

	//--------------------------------------------------------------------
	// GetActiveLight - return pointer to currently active shadow light.
	//	This will return NULL if not currently in a shadow casting render
	//--------------------------------------------------------------------
	const g3dLight*	GetActiveLight();
	void SetActiveLight(const g3dLight* i_Light);

	//--------------------------------------------------------------------
	// IsFirstLight is used for multi pass rendering
	// should only be set true for the first light rendered
	//--------------------------------------------------------------------
	bool IsFirstLight();
	void SetFirstLight( bool i_bEnable );
}


