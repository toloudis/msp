/****************************************************************************\
**	cam3dAnimKeys.hpp
**
**		Contains data for animating camera.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef CAM3D_ANIMKEYS_HPP
#error cam3dAnimKeys.hpp multiply included
#endif
#define CAM3D_ANIMKEYS_HPP

#ifndef AN_KEYDATA_HPP
#include "Graphics/an/anKeyData.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif

#include <vector>


//============================================================================
// Keeps data for a script to manipulate camera
//============================================================================
class cam3dAnimKeys 
{
	public:
		//--------------------------------------------------------------------
		// Constructor converts from translation and rotation channels to 
		//	position and target channels. Center of Interest is distance
		//	along view direction and focal length is used to compute
		//	field of view angle.
		// Pointers are passed in order to be able to use NULL 
		//	as "no animation", but this class will make a copy of what 
		//	it needs, so the caller must maintain the data passed in.
		//--------------------------------------------------------------------
		cam3dAnimKeys(const anKeyData<maPoint3d>* i_pTranslationChannel,
						const anKeyData<maRotation>* i_pRotationChannel,
						const anKeyData<float>* i_pCenterOfInterestChannel,
						const anKeyData<float>* i_pFocalLengthChannel,
						const anKeyData<float>* i_pHorizApertChannel,
						const anKeyData<float>* i_pInteraxialSepChannel,
						const anKeyData<float>* i_pZeroParallaxChannel);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~cam3dAnimKeys();

		//--------------------------------------------------------------------
		// Returns true if there is animation data of the given type
		//--------------------------------------------------------------------
		bool HasPositionAnimation() const { return (m_pPositionKeys != NULL); }
		bool HasViewAnimation() const { return (m_pViewKeys != NULL); }
		bool HasTiltAnimation() const { return (m_pTiltKeys != NULL); }
		bool HasFieldOfViewAnimation() const { return (m_pFovKeys != NULL); }
		bool HasFocalLengthAnimation() const { return (m_pFocalLengthKeys != NULL); }
		bool HasHorizontalApertureAnimation() const { return (m_pHorizontalApertureKeys != NULL); }
		bool HasInteraxialSepAnimation() const { return (m_pInteraxialSepKeys != NULL); }
		bool HasZeroParallaxAnimation() const { return (m_pZeroParallaxKeys != NULL); }

		//--------------------------------------------------------------------
		// TestForCut - test to see if the camera did a "cut" during
		// the bracketing keys. It tests this by seeing if the position
		// change is greater than the given threshold.
		//--------------------------------------------------------------------
		bool TestForCut(float i_Time, float i_Threshold) const;

		//--------------------------------------------------------------------
		// GetPosition - get linear interpolated position key data
		//--------------------------------------------------------------------
		maPoint3d GetPosition(float i_Time) const;

		//--------------------------------------------------------------------
		// GetViewVector - get interpolated vector from position to target. 
		//	This should be added to the position to get the target position.
		//--------------------------------------------------------------------
		maVector3d GetViewVector(float i_Time) const;

		//--------------------------------------------------------------------
		// GetFieldOfView - get interpolated field of view angle in degrees
		//--------------------------------------------------------------------
		float GetFieldOfView(float i_Time) const;

		//--------------------------------------------------------------------
		// GetFocalLength - get interpolated focal length
		//--------------------------------------------------------------------
		float GetFocalLength(float i_Time) const;

		//--------------------------------------------------------------------
		// GetHorizontalAperture - get interpolated focal length
		//--------------------------------------------------------------------
		float GetHorizontalAperture(float i_Time) const;

		//--------------------------------------------------------------------
		// GetTilt - get interpolated tilt angle in degrees
		//--------------------------------------------------------------------
		float GetTilt(float i_Time) const;

		//--------------------------------------------------------------------
		// GetInteraxialSeparation - get interpolated interaxial separation
		// in centimeters
		//--------------------------------------------------------------------
		float GetInteraxialSeparation(float i_Time) const;

		//--------------------------------------------------------------------
		// GetZeroParallax - get interpolated zero parallax
		// in centimeters
		//--------------------------------------------------------------------
		float GetZeroParallax(float i_Time) const;

		//--------------------------------------------------------------------
		// Return number of frames in longest animation
		//--------------------------------------------------------------------
		int GetNumFrames();

	private:
		anKeyData<maPoint3d>* m_pPositionKeys;
		anKeyData<maVector3d>* m_pViewKeys;
		anKeyData<float>* m_pTiltKeys;
		anKeyData<float>* m_pFovKeys;
		anKeyData<float>* m_pFocalLengthKeys;
		anKeyData<float>* m_pHorizontalApertureKeys;
		anKeyData<float>* m_pInteraxialSepKeys;
		anKeyData<float>* m_pZeroParallaxKeys;
};
