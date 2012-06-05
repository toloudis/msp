/*****************************************************************************
**	rmpTextureMgr.hpp
**
**	Control rendering ramp texture
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef RMP_TEXTUREMGR_HPP
#error rmpTextureMgr.hpp multiply included
#endif
#define RMP_TEXTUREMGR_HPP

#ifndef RMP_DATA_HPP
#include "Support/rmp/rmpData.hpp"
#endif


//============================================================================
//============================================================================
class effParamTexture;
class matTexture;
class maGradient;
class rmpObject;

//============================================================================
//============================================================================
namespace rmpTextureMgr
{
	
	//--------------------------------------------------------------------
	// Setup the initial operations for the ramp texture
	//--------------------------------------------------------------------
	void SetupRampTexture(effParamTexture* i_pParamTexture);

	//--------------------------------------------------------------------
	// cleanup the ramp texture
	//--------------------------------------------------------------------
	void CleanupRampTexture();

	//--------------------------------------------------------------------
	//  Enable Ramp Texture
	//--------------------------------------------------------------------
	//void EnableRampTexture(const maGradient& i_Gradient, const int i_Shape, 
	//					const int i_Interpolation, const int i_texSize, matTexture* i_RampTexture);
	void EnableRampTexture(const rmpData& i_RampData, matTexture* i_RampTexture);

	//--------------------------------------------------------------------
	//  Create ramp texture
	//--------------------------------------------------------------------
	matTexture* CreateRampTexture(const int i_Size);

	//--------------------------------------------------------------------
	//  Release ramp texture
	//--------------------------------------------------------------------
	void ReleaseRampTexture(matTexture* io_texture);

	//--------------------------------------------------------------------
	// update function for the ramp control
	//--------------------------------------------------------------------
	void UpdateRampTexture(bool i_bDirty);

	//--------------------------------------------------------------------
	// property update function for the ramp control
	//--------------------------------------------------------------------
	void UpdateRampData(prtyProperty *i_pProperty, bool i_bDirty);

	//--------------------------------------------------------------------
	//  remove the render target in render queue if the render target
	//	is from given texture
	//  Note: MUST call this function before changes to ramp texture (size)
	//--------------------------------------------------------------------
	//void CleanTargetRendererInRenderQueue(matTexture* i_RampTexture);

	//--------------------------------------------------------------------
	//  Setup ramp scene
	//--------------------------------------------------------------------
	void Initialize();

	//--------------------------------------------------------------------
	//  Cleanup ramp scene
	//--------------------------------------------------------------------
	void DeInitialize();

}	// end of namespace
