/********************************************************************************************\
**  ptclLevel.hpp
**
**      Keeps track of particle generators in the world.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#ifdef	PTCL_LEVEL_HPP
#error	ptclLevel.hpp included recursively.
#endif
#define	PTCL_LEVEL_HPP

#ifndef PTCL_CALLBACKS_HPP
#include "ptclCallbacks.hpp"
#endif

#ifndef PRT_PARTICLEGENERATOR_HPP
#include "Graphics/prt/prtParticleGenerator.hpp"
#endif

#ifndef AN_3STATEANIMATION_HPP
#include "Graphics/an/an3StateAnimation.hpp"
#endif


//============================================================================================
//	forward references
//============================================================================================
class fsLocator;


//============================================================================================
//	ptclLevel Functions
//============================================================================================
namespace ptclLevel
{
	//----------------------------------------------------------------------------
	//	Initialize()
	//----------------------------------------------------------------------------
	void	Initialize();

	//----------------------------------------------------------------------------
	//	DeInitialize()
	//----------------------------------------------------------------------------
	void	DeInitialize();

	//----------------------------------------------------------------------------
	//	SetParticleChangedCallback
	//----------------------------------------------------------------------------
	void	SetParticleChangedCallback(ptclParticleChangedCallback *i_Callback);

	//----------------------------------------------------------------------------
	//	Think
	//----------------------------------------------------------------------------
	void	Think();

	//----------------------------------------------------------------------------
	//	Clear removes all URo pieces to start new level
	//----------------------------------------------------------------------------
	void	Clear();

	//------------------------------------------------------------------------
	//	Save saves a definition from a given locator
	//------------------------------------------------------------------------
	void	Save(const fsLocator& i_Locator);

	//------------------------------------------------------------------------
	//	Load()
	//------------------------------------------------------------------------
	void	Load(const fsLocator& i_Locator);

	//------------------------------------------------------------------------
	//	LoadAnimation()
	//------------------------------------------------------------------------
	void	LoadAnimation(const fsLocator& i_Locator);

	//------------------------------------------------------------------------
	//	FocusCamera() - put camera target at particle center
	//------------------------------------------------------------------------
	void	FocusCamera();

	//------------------------------------------------------------------------
	//	Return directory from which to load textures
	//------------------------------------------------------------------------
	const fsLocator&	GetTextureDir();

	//
	//	Particle Generator Template
	//

	//------------------------------------------------------------------------
	//	Set the alpha profile values
	//------------------------------------------------------------------------
	void SetParticleGeneratorTemplateAlphaProfile(	float i_fBeginAlpha, 
													float i_fMiddleAlphaTimeStart, 
													float i_fMiddleAlphaTimeEnd, 
													float i_fMiddleAlpha, 
													float i_fEndAlpha );

	//
	//	Particle Generator
	//

	//------------------------------------------------------------------------
	//	return the particle generator
	//------------------------------------------------------------------------
	prtParticleGenerator* GetParticleGenerator();

	//------------------------------------------------------------------------
	//	replace the current generator based on the particle template
	//------------------------------------------------------------------------
	void ReplaceGenerator();

	//------------------------------------------------------------------------
	//	Get the alpha profile values
	//------------------------------------------------------------------------
	void GetParticleGeneratorAlphaProfile(	float& o_fBeginAlpha, 
											float& o_fMiddleAlphaTimeStart, 
											float& o_fMiddleAlphaTimeEnd, 
											float& o_fMiddleAlpha, 
											float& o_fEndAlpha );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const anTypedAnimation<float>* GetParticleGeneratorAlphaProfile();

	//------------------------------------------------------------------------
	//	Set the alpha profile values
	//------------------------------------------------------------------------
	void SetParticleGeneratorAlphaProfile(	float i_fBeginAlpha, 
											float i_fMiddleAlphaTimeStart, 
											float i_fMiddleAlphaTimeEnd, 
											float i_fMiddleAlpha, 
											float i_fEndAlpha );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetParticleGeneratorAlphaProfile(const anTypedAnimation<float>* i_pAlpha);

	//------------------------------------------------------------------------
	//	Set the texture scale mode
	//------------------------------------------------------------------------
	void SetParticleGeneratorTextureScaleMode( int i_ScaleMode );

	//------------------------------------------------------------------------
	//	Set the texture alpha blending render mode
	//------------------------------------------------------------------------
	void SetParticleGeneratorTextureAlphaBlendingMode( int i_AlphaBlendingRenderMode );

	//------------------------------------------------------------------------
	//	Set the texture UVA mode
	//------------------------------------------------------------------------
	void SetParticleGeneratorTextureUVAMode( int i_UVAMode );

	//------------------------------------------------------------------------
	//	Set the particle generator texture.
	//
	//	input: the string should be the entire path plus filename.
	//
	//	Note: this will unload the current texture and load the new one
	//------------------------------------------------------------------------
	void SetParticleGeneratorTexture( std::string& i_TextureFilename );

	//------------------------------------------------------------------------
	//	show the ground or not.
	//------------------------------------------------------------------------
	void ShowGround( bool i_bRenderGround );
	bool IsShowGround();
};
