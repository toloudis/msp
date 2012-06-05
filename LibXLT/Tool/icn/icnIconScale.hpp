/*****************************************************************************\
**	icnIconScale.hpp
**
**		Manages the global scaling for a scene.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef ICN_ICONSCALE_HPP
#error icnIconScale.hpp multiply included
#endif
#define ICN_ICONSCALE_HPP

#ifndef MA_POINT3D_HPP
#include "Core/Ma/maPoint3d.hpp"
#endif 

#include <string>


//============================================================================
//	Forward References
//============================================================================
class camCamera;
class icnIconScaleInterest;


//============================================================================
//============================================================================
class icnIconScaleInterest
{
	public:
		//--------------------------------------------------------------------
		//	GlobalScaleChanged - notification that global scale has changed.
		//--------------------------------------------------------------------
		//virtual void GlobalScaleChanged( float i_Scale ) = 0;

		//--------------------------------------------------------------------
		//	UpdateIconScale - function that sets the icons scale.
		//--------------------------------------------------------------------
		virtual void UpdateIconScale( int i_IconLayerIndex, const camCamera& i_Camera, float i_Scale ) = 0;

};

//============================================================================
//============================================================================
namespace icnIconScale
{
	//--------------------------------------------------------------------
	// Enumeration of the scaling options
	//--------------------------------------------------------------------
	enum IconScaleLevel
	{
		e_Small = 0,
		e_Medium,
		e_Large
	};

	//--------------------------------------------------------------------
	// Set scale of icons based on an enumeration
	//--------------------------------------------------------------------
	void SetIconScaleLevel(IconScaleLevel i_IconScaleLevel);
	IconScaleLevel GetIconScaleLevel();

	//--------------------------------------------------------------------
	//	GlobalScale - a scaling factor for the scene, used to alter
	//		the size of icons and other "magic" numbers.
	//--------------------------------------------------------------------
	//void SetGlobalScale( float i_Scale );
	//float GetGlobalScale();

	//--------------------------------------------------------------------
	// Update scale of camera-relative icons to the given camera
	// for a render area with the given screen width.
	//--------------------------------------------------------------------
	void UpdateIconsScale( int i_IconLayerIndex, const camCamera& i_Camera, int i_ScreenWidth );

	//--------------------------------------------------------------------
	// GetIconScaleForPosition - Computes scale for an icon at the
	// given position based on the given camera view. This scale should
	// keep the icon the same size in all camera views.
	//--------------------------------------------------------------------
	float GetIconScaleForPosition( const maPoint3d& i_Position,
								   const camCamera& i_Camera );

	//--------------------------------------------------------------------
	// Apply global scale to value, returning new scaled value
	//--------------------------------------------------------------------
	//float Scale(float i_Value);

	//--------------------------------------------------------------------
	//	RegisterScaleInterest() - add a Scale interest to the system
	//--------------------------------------------------------------------
	void RegisterScaleInterest( icnIconScaleInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	UnRegisterScaleInterest() - remove a Scale interest from the system.
	//
	//	Note: this will NOT delete the Scale interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterScaleInterest( icnIconScaleInterest* i_pInterest );

};
