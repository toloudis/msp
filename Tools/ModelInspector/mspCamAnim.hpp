/*****************************************************************************
**	mspCamAnim.hpp
**
**	 mspCamAnim represents the camera animation.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MSP_CAMANIM_HPP
#error mspCamAnim.hpp multiply included
#endif
#define MSP_CAMANIM_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif 

//============================================================================
//============================================================================
namespace mspCamAnim 
{
	//--------------------------------------------------------------------
	// Clear
	//--------------------------------------------------------------------
	void  Clear();

	//--------------------------------------------------------------------
	// LoadAnimation
	//--------------------------------------------------------------------
	bool  LoadAnimation(const fsLocator &i_AnimLocator );

	//--------------------------------------------------------------------
	// Returns true if animation is loaded
	//--------------------------------------------------------------------
	bool HasAnimation();

	//--------------------------------------------------------------------
	// Alters camera based on animation for the given time
	//--------------------------------------------------------------------
	void Animate(float i_Frame);

}
