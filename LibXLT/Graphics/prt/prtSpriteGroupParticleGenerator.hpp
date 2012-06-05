/*****************************************************************************
**	prtSpriteGroupParticleGenerator.hpp
**
**		prtSpriteGroupParticleGenerator is a base class for Terawatt particle
**	generators which are implemented with a g3dSpriteGroupModel.  It provides
**	some common functionality for controlling the g3dSpriteGroupModel.
**	
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PRT_SPRITEGROUPPARTICLEGENERATOR_HPP
#error prtSpriteGroupParticleGenerator.hpp multiply included
#endif
#define PRT_SPRITEGROUPPARTICLEGENERATOR_HPP

#ifndef PRT_PARTICLEGENERATOR_HPP
#include "Graphics/prt/prtParticleGenerator.hpp"
#endif


//============================================================================
//============================================================================
class matMaterial;
class matTexture;
class sprtSpriteGroupFrag;


//============================================================================
//============================================================================
class prtSpriteGroupParticleGenerator : public prtParticleGenerator
{
	public:
		//--------------------------------------------------------------------
		//	The i_CreationTime is the simulation time at which the particle
		//	generator was created.
		//	If e_DefaultParticleLayer is set for the render layer, the
		//	particles will be created in the default particle layer (set in
		//	prtParticleGenerator).  If this is done the default particle
		//	layer must have been set previously.
		//--------------------------------------------------------------------
		prtSpriteGroupParticleGenerator(float i_CreationTime);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~prtSpriteGroupParticleGenerator();

		//--------------------------------------------------------------------
		//	These parameters can be changed using the "SetParameter"
		//	function from the prtParticleGenerator.
		//--------------------------------------------------------------------
		enum Parameter
		{
			e_InitialScale = prtParticleGenerator::e_NextParameter,	//	default 1.0
			e_ScaleCoeff,											//	default 1.0
			e_MinStartAngle,										//	default 0.0
			e_MaxStartAngle,										//	default 0.0
			e_MinAngularVelocity,									//	at the start of the lifetime; default 0.0
			e_MaxAngularVelocity,									//	default 0.0
			e_MinAngularAcceleration,								//	default 0.0
			e_MaxAngularAcceleration,								//	default 0.0
			e_StreakLength,											//  default 1/24 (time measurement)
			e_StreakTaper,											//	default 1.0
			e_StreakFade,											//	default 1.0
			e_NextParameter
		};

		//--------------------------------------------------------------------
		//	GetNumParameters returns the number of parameters used by the
		//	particle generator.  This function should be overridden by 
		//	child classes which add parameters.
		//--------------------------------------------------------------------
		virtual int GetNumParameters() const;

		//--------------------------------------------------------------------
		//	GetMatrix returns the matrix of the object
		//--------------------------------------------------------------------
		virtual void GetMatrix( maMatrix4x4& o_Matrix ) const;

		//--------------------------------------------------------------------
		//	Animate is called by the scScene for each object before it
		//	is rendered.
		//--------------------------------------------------------------------
		virtual void Animate(float i_SimulationTime);

		//--------------------------------------------------------------------
		//	SetTexture sets the texture which will be used to render all
		//	particles.  This could be a UVA Texture, in which case its
		//	behavior will be governed by the UVA Mode; see below.  The 
		//	particle generator is not considered to own the texture.
		//--------------------------------------------------------------------
		void SetTexture(matTexture* i_Texture);

		//--------------------------------------------------------------------
		//	Remove all the textures for this generators material.
		//--------------------------------------------------------------------
		void RemoveTextures();

		//--------------------------------------------------------------------
		//	Scale mode determines how the particle uses the e_ScaleCoeff to
		//	scale over it's lifetime.  For linear scale, the particle
		//	scale is given by e_InitialScale * e_ScaleCoeff * time.
		//	For exponential scale, the scale is given by
		//	e_InitialScale * e_ScaleCoeff ^ time.  Exponential scale is
		//	cooler, and is the default.
		//--------------------------------------------------------------------
		enum ScaleMode
		{
			e_Linear = 0,
			e_Exponential
		};

		void SetScaleMode(ScaleMode i_Mode);
		ScaleMode GetScaleMode() const;

		//--------------------------------------------------------------------
		//	Render mode determines how the particle pixel colors are combined
		//	with the background when drawing.  Additive mode is more
		//	appropriate for particles that appear to be emissive.
		//	Multiplicative mode is more appropriate for particles that appear
		//	to be translucent.  The default is e_Multiplicative.
		//--------------------------------------------------------------------
		enum RenderMode
		{
			e_Additive = 0,
			e_Multiplicative
		};

		void SetRenderMode(RenderMode i_Mode);
		RenderMode  GetRenderMode() const;

		//--------------------------------------------------------------------
		//	UVA mode determines how the particle system uses the frames of
		//	a UVA.  This setting has no effect if the texture given to the
		//	prtParticleGenerator is not a matUVATexture.  e_ScaleToLifetime
		//	causes the entire UVA animation to be played once over the course
		//	of the lifetime of any single particle.  e_RandomFrame causes
		//	a single frame of the UVA to be selected randomly for each
		//	particle.  This frame is used to draw the particle over it's
		//	entire lifetime.  e_TUV will automatically use the animation rate
		//	stored in the UVA texture.
		//	MUST ADD NEW ENTRIES AT THE END DUE TO FILE I/O.
		//--------------------------------------------------------------------
		enum UVAMode
		{
			e_ScaleToLifetime = 0,
			e_RandomFrame,
			e_TUV
		};

		void SetUVAMode(UVAMode i_Mode);
		UVAMode GetUVAMode() const;

		//--------------------------------------------------------------------
		//	GetNumUVAFrames returns the number of frames in the UVA texture or
		//	0 if it has normal texture
		//--------------------------------------------------------------------
		inline int GetNumUVAFrames() const;

		//--------------------------------------------------------------------
		// Streak rendering renders a stretched polygon to represent the
		// motion of a particle over time.
		//--------------------------------------------------------------------
		void SetRenderStreaks(bool i_bStreaks);
		bool GetRenderStreaks() const;

		//--------------------------------------------------------------------
		//	DeleteAllParticles just take a guess what this one does. go ahead, guess
		//--------------------------------------------------------------------
		virtual void DeleteAllParticles() = 0;

		//--------------------------------------------------------------------
		//	ClearParticleAccumulation clears the variable that tracks the particle
		//	accumulation, essentially giving particle generation a clean slate
		//	to work with. Good for clearing out undesired accumulation that may
		//	occur during long pausessuch as mode changes
		//--------------------------------------------------------------------
		virtual void ClearParticleAccumulation() = 0;

		//--------------------------------------------------------------------
		// enable shadow casting
		//--------------------------------------------------------------------
		virtual void SetCastsShadow( bool i_bCastShadow );

		//--------------------------------------------------------------------
		// set parameters for translucent shadow map dithering
		//--------------------------------------------------------------------
		void SetShadowDithering( bool i_bUseDithering, float i_DitherAlphaBias );

		//--------------------------------------------------------------------
		// enable rendering for reflection maps
		//--------------------------------------------------------------------
		void SetRenderableInReflections( bool i_bShowInPlanarReflections, bool i_bShowInCubeReflections );

	protected:
		//--------------------------------------------------------------------
		//	GetFragment - returns the sprite group fragment
		//--------------------------------------------------------------------
		inline sprtSpriteGroupFrag* GetFragment();

		inline matMaterial* GetMaterial();

	private:
		matMaterial* m_pMaterial;
		sprtSpriteGroupFrag* m_pFragment;
		ScaleMode m_ScaleMode;
		RenderMode m_RenderMode;
		UVAMode m_UVAMode;
		int m_nUVAFrames;
		bool m_bRenderStreaks;
};

//--------------------------------------------------------------------
//	GetNumUVAFrames returns the number of frames in the UVA texture or
//	0 if it has normal texture
//--------------------------------------------------------------------
inline int prtSpriteGroupParticleGenerator::GetNumUVAFrames() const
{
	return m_nUVAFrames;
}

//--------------------------------------------------------------------
//	GetSpriteGroupFrag - returns the sprite group fragment
//--------------------------------------------------------------------
inline sprtSpriteGroupFrag* prtSpriteGroupParticleGenerator::GetFragment()
{
	return m_pFragment;
}

inline matMaterial* prtSpriteGroupParticleGenerator::GetMaterial()
{
	return m_pMaterial;
}
