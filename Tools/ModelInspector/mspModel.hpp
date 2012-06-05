/*****************************************************************************
**	mspModel.hpp
**
**	 mspModel represents the currently viewed model and animation.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MSP_MODEL_HPP
#error mspModel.hpp multiply included
#endif
#define MSP_DOCUMENT_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif 


//============================================================================
//============================================================================
class api3dObject;


//============================================================================
//============================================================================
namespace mspModel 
{
	//--------------------------------------------------------------------
	// Clear
	//--------------------------------------------------------------------
	void  Clear();

	//--------------------------------------------------------------------
	// LoadModel
	//--------------------------------------------------------------------
	bool  LoadModel(const fsLocator &i_Locator );

	//--------------------------------------------------------------------
	// LoadAnimation
	//--------------------------------------------------------------------
	bool  LoadAnimation(const fsLocator &i_AnimLocator );

	//--------------------------------------------------------------------
	// ModelFilename
	//--------------------------------------------------------------------
	const fsLocator& GetModelFilename();

	//--------------------------------------------------------------------
	// Returns directory to use as initial location of animation
	//	open file dialog.
	//--------------------------------------------------------------------
	fsLocator GetAnimationDir();

	//--------------------------------------------------------------------
	// FocusCamera - center camera on model
	//--------------------------------------------------------------------
	void FocusCamera();

	//--------------------------------------------------------------------
	// Returns true if model is loaded and is an animatable model
	//--------------------------------------------------------------------
	bool CanLoadAnimation();

	//--------------------------------------------------------------------
	// Returns true if model is loaded.
	//--------------------------------------------------------------------
	bool HasModel();

	//--------------------------------------------------------------------
	// Returns true if a model with subdivision surfaces is loaded.
	//--------------------------------------------------------------------
	bool HasSubdivModel();

	//--------------------------------------------------------------------
	// Returns true if animation is loaded
	//--------------------------------------------------------------------
	bool HasAnimation();

	//--------------------------------------------------------------------
	// Return the frame of the animation for the given time
	//--------------------------------------------------------------------
	float ComputeFrame(float i_Time);
		
	//--------------------------------------------------------------------
	// Return the simulation time such that the animation will play 
	//	the given frame number
	//--------------------------------------------------------------------
	float ComputeTime(float i_Frame);
	
	//--------------------------------------------------------------------
	// Control over low resolution model display
	//--------------------------------------------------------------------
	bool HasLowResModel();
	bool GetUseLowResModel();
	void SetUseLowResModel(bool i_bLowRes);

	//--------------------------------------------------------------------
	// Return current subdivision level being used.
	//--------------------------------------------------------------------
	int GetCurrentSubdivLevel();

	//--------------------------------------------------------------------
	// Set the current subdivision level being used, this should be 
	// a level less than or equal to the return value of 
	// GetMaxSubdivLevel()
	//--------------------------------------------------------------------
	void SetCurrentSubdivLevel(int i_SubdivLevel);

}
