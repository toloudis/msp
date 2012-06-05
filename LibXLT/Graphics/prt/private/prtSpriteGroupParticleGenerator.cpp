/*****************************************************************************
**	prtSpriteGroupParticleGenerator.cpp
**
**		prtSpriteGroupParticleGenerator is the base class for Terawatt particle
**	generators which are implemented with a g3dSpriteGroupModel.  It provides
**	some common functionality for controlling the g3dSpriteGroupModel.
**	
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/prt/prtSpriteGroupParticleGenerator.hpp"

#include "Graphics/eff/effParticleData.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matUVATexture.hpp"
#include "Graphics/sprt/sprtSpriteGroupFrag.hpp"


//--------------------------------------------------------------------
//	The i_CreationTime is the simulation time at which the particle
//	generator was created.
//--------------------------------------------------------------------
prtSpriteGroupParticleGenerator::prtSpriteGroupParticleGenerator(float i_CreationTime)
:	prtParticleGenerator(i_CreationTime),
	m_pFragment(NULL),
	m_pMaterial(NULL),
	m_ScaleMode(prtSpriteGroupParticleGenerator::e_Exponential),
	m_RenderMode(prtSpriteGroupParticleGenerator::e_Multiplicative),
	m_UVAMode(prtSpriteGroupParticleGenerator::e_ScaleToLifetime),
	m_nUVAFrames( 0 ),
	m_bRenderStreaks(false)
{
	m_pMaterial = new matMaterial("Particle.fx");

	m_pFragment = new sprtSpriteGroupFrag( m_pMaterial );
	m_pFragment->SetReceivesShadow(true);
	m_pFragment->SetCastsShadow(true);
	m_pFragment->SetDeferTransparency(true);
	m_pFragment->SetModelSpaceVertices(false);
	GetBase()->SetCastsShadow(true);
	GetBase()->SetFragment(m_pFragment);
	GetBase()->SetGPUPickable( false );

	SetRenderMode(prtSpriteGroupParticleGenerator::e_Multiplicative);
	SetParameter(e_InitialScale, 1.0f);
	SetParameter(e_ScaleCoeff, 1.0f);
	SetParameter(e_MinStartAngle, 0.0f);
	SetParameter(e_MaxStartAngle, 0.0f);
	SetParameter(e_MinAngularVelocity, 0.0f);
	SetParameter(e_MaxAngularVelocity, 0.0f);
	SetParameter(e_MinAngularAcceleration, 0.0f);
	SetParameter(e_MaxAngularAcceleration, 0.0f);
	//SetParameter(e_TextureFilename, "");
	// defaults to time of a single frame, doesn't need to be exactly one frame:
	SetParameter(e_StreakLength, 1.0f / 24.0f ); 
	SetParameter(e_StreakTaper, 1.0f);
	SetParameter(e_StreakFade, 1.0f);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtSpriteGroupParticleGenerator::~prtSpriteGroupParticleGenerator()
{
	GetBase()->SetFragment(NULL);
	delete m_pFragment;
	delete m_pMaterial;
	m_pMaterial = NULL;
}

//--------------------------------------------------------------------
//	GetNumParameters returns the number of parameters used by the
//	particle generator.  This function should be overridden by 
//	child classes which add parameters.
//--------------------------------------------------------------------
int prtSpriteGroupParticleGenerator::GetNumParameters() const
{
	return e_NextParameter;
}

//--------------------------------------------------------------------
//	GetMatrix returns the matrix of the object
//--------------------------------------------------------------------
void prtSpriteGroupParticleGenerator::GetMatrix( maMatrix4x4& o_Matrix ) const
{
	const maPoint3d& scale = GetScale();
	const maRotation& orientation = GetOrientation();
	const maPoint3d& position = GetPosition();

	// Compute matrix
	o_Matrix.MakeScale( scale.GetX(), scale.GetY(), scale.GetZ() );
	o_Matrix *= orientation.GetMatrix();
	o_Matrix.TranslateBy( position.GetX(), position.GetY(), position.GetZ() );
}

//--------------------------------------------------------------------
//	PreRender is called by the scScene for each object before it
//	is rendered.
//--------------------------------------------------------------------
void prtSpriteGroupParticleGenerator::Animate(float i_SimulationTime)
{
//	DBG_ASSERT(m_pMaterial->GetTexture(0), "Texture not set before PreRender");

	prtParticleGenerator::Animate(i_SimulationTime);

	// Set streak values from paramters into fragment to be accessible by renderer
	this->m_pFragment->SetStreakFade( GetParameter(e_StreakFade ) );
	this->m_pFragment->SetStreakTaper( GetParameter(e_StreakTaper ) );
}

//--------------------------------------------------------------------
//	SetTexture sets the texture which will be used to render all
//	particles.  This could be a UVA Texture, in which case its
//	behavior will be governed by the UVA Mode see below.  The 
//	particle generator is not considered to own the texture.
//--------------------------------------------------------------------
void prtSpriteGroupParticleGenerator::SetTexture(matTexture* i_Texture)
{
	m_pMaterial->TypedData<effParticleData>()->m_TextureDiffuse = i_Texture;

	matUVATexture* pUVATexture = dynamic_cast<matUVATexture*>(i_Texture);
	if( pUVATexture )
	{
		m_nUVAFrames = pUVATexture->GetNumFrames();
	}
}

//--------------------------------------------------------------------
//	Remove all the textures for this generators material.
//--------------------------------------------------------------------
void prtSpriteGroupParticleGenerator::RemoveTextures()
{
	m_pMaterial->RemoveTextures();
	m_pMaterial->TypedData<effParticleData>()->m_TextureDiffuse = NULL;
}

//--------------------------------------------------------------------
//	Scale mode determines how the particle uses the e_ScaleCoeff to
//	scale over it's lifetime.  For linear scale, the particle
//	scale is given by e_InitialScale * e_ScaleCoeff * time.
//	For exponential scale, the scale is given by
//	e_InitialScale * e_ScaleCoeff ^ time.  Exponential scale is
//	cooler, and is the default.
//--------------------------------------------------------------------
void prtSpriteGroupParticleGenerator::SetScaleMode(ScaleMode i_Mode)
{
	m_ScaleMode = i_Mode;
}

prtSpriteGroupParticleGenerator::ScaleMode prtSpriteGroupParticleGenerator::GetScaleMode() const
{
	return m_ScaleMode;
}

//--------------------------------------------------------------------
//	Render mode determines how the particle pixel colors are combined
//	with the background when drawing.  Additive mode is more
//	appropriate for particles that appear to be emissive.
//	Multiplicative mode is more appropriate for particles that appear
//	to be translucent.  The default is e_Multiplicative.
//--------------------------------------------------------------------
void prtSpriteGroupParticleGenerator::SetRenderMode(RenderMode i_Mode)
{
	m_RenderMode = i_Mode;

	if( i_Mode == prtSpriteGroupParticleGenerator::e_Multiplicative )
	{
		m_pMaterial->SetAdditive( false );
		m_pMaterial->TypedData<effParticleData>()->m_ColorAmbient = (maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
		m_pMaterial->TypedData<effParticleData>()->m_ColorDiffuse = (maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
		m_pMaterial->TypedData<effParticleData>()->m_ColorEmissive = (maFloatRGBA(0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		m_pMaterial->SetAdditive( true );
		m_pMaterial->TypedData<effParticleData>()->m_ColorAmbient = (maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
		m_pMaterial->TypedData<effParticleData>()->m_ColorDiffuse = (maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
		m_pMaterial->TypedData<effParticleData>()->m_ColorEmissive = (maFloatRGBA(0.0f, 0.0f, 0.0f, 0.0f));
	}
}

prtSpriteGroupParticleGenerator::RenderMode prtSpriteGroupParticleGenerator::GetRenderMode() const
{
	return m_RenderMode;
}

//--------------------------------------------------------------------
//	UVA mode determines how the particle system uses the frames of
//	a UVA.  This setting has no effect if the texture given to the
//	prtSpriteGroupParticleGenerator is not a matUVATexture.  e_ScaleToLifetime
//	causes the entire UVA animation to be played once over the course
//	of the lifetime of any single particle.  e_RandomFrame causes
//	a single frame of the UVA to be selected randomly for each
//	particle.  This frame is used to draw the particle over it's
//	entire lifetime.
//--------------------------------------------------------------------
void prtSpriteGroupParticleGenerator::SetUVAMode(UVAMode i_Mode)
{
	m_UVAMode = i_Mode;
}

prtSpriteGroupParticleGenerator::UVAMode prtSpriteGroupParticleGenerator::GetUVAMode() const
{
	return m_UVAMode;
}

//--------------------------------------------------------------------
// Streak rendering renders a stretched polygon to represent the
// motion of a particle over time.
//--------------------------------------------------------------------
void prtSpriteGroupParticleGenerator::SetRenderStreaks(bool i_bStreaks)
{
	m_pFragment->SetRenderStreaks( i_bStreaks );
}
bool prtSpriteGroupParticleGenerator::GetRenderStreaks() const
{
	return m_pFragment->GetRenderStreaks();
}

//--------------------------------------------------------------------
// enable shadow casting
//--------------------------------------------------------------------
void prtSpriteGroupParticleGenerator::SetCastsShadow( bool i_bCastShadow )
{
	m_pFragment->SetCastsShadow(i_bCastShadow);
}

//--------------------------------------------------------------------
// set parameters for translucent shadow map dithering
//--------------------------------------------------------------------
void prtSpriteGroupParticleGenerator::SetShadowDithering( bool i_bUseDithering, float i_DitherAlphaBias )
{
	m_pFragment->SetShadowDithering(i_bUseDithering, i_DitherAlphaBias);
}

//--------------------------------------------------------------------
// enable rendering for reflection maps
//--------------------------------------------------------------------
void prtSpriteGroupParticleGenerator::SetRenderableInReflections( bool i_bShowInPlanarReflections, bool i_bShowInCubeReflections )
{
	GetBase()->SetRenderableInPlanarReflection(i_bShowInPlanarReflections);
	GetBase()->SetRenderableInCubeMapReflection(i_bShowInCubeReflections);
}

