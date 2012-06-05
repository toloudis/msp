/****************************************************************************\
**	cam3dAnimKeys.cpp
**
**		Contains data for animating camera.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Tool/cam3d/cam3dAnimKeys.hpp"

#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Tool/cam3d/cam3dUtil.hpp"


//============================================================================
//============================================================================
namespace
{
	const float c_DefaultViewLength = 6.5f;

	// Get view direction from rotation vector, set to distance length
	maVector3d convert_to_vector(const maRotation& i_Rot, float i_Dist)
	{
		maVector3d vector(0,0,-i_Dist);
		i_Rot.RotateVector(vector);
		return vector;
	}

	// Compute the tilt angle (in degrees) needed to match the given rotation
	float compute_tilt(const maRotation& i_Rot, const maVector3d &i_ViewDir)
	{	
		if( i_ViewDir.LengthSqr() == 0.0f )
			return 0.0f;

		// Get the up vector for the Terawatt camera using the view direction
		const maVector3d c_DefaultUp(0,1,0);
		maVector3d left = c_DefaultUp.Cross(i_ViewDir);
		if( left.LengthSqr() == 0.0f )
			left.Set(1, 0, 0);
		else
			left.Normalize();

		maVector3d camera_up = i_ViewDir.Cross(left);
		camera_up.Normalize();

		// Get the up vector from Maya using the maRotation
		maVector3d maya_up(0,1,0);
		i_Rot.RotateVector(maya_up);

		// Dot product of two vectors is the cosine of the angle between them
		float cos_angle = camera_up * maya_up;
		maFunctions::Clamp(cos_angle, -1.0f, 1.0f);
		float angle_radians = acosf(cos_angle);
		if (angle_radians > maConstants::c_fEpsilon)
		{
			// Try to determine if the angle should be positive or negative
			// using the cross product
			maVector3d cross_product = camera_up.Cross(maya_up);
			if (cross_product * i_ViewDir > 0)
			{	
				return angle_radians * maConstants::c_fRadToAngle;
			}
			else
			{	
				return -angle_radians * maConstants::c_fRadToAngle;
			}
		}

		// No difference between the up vectors, so return 0.0 tilt
		return 0.0f;
	}

} // end of anonymous namespace

//--------------------------------------------------------------------
// Constructor converts from translation and rotation channels to 
//	position and target channels. Center of Interest is distance
//	along view direction and focal length is used to compute
//	field of view angle.
// Pointers are passed in order to be able to use NULL 
//	as "no animation", but this class will make a copy of what 
//	it needs, so the caller must maintain the data passed in.
//--------------------------------------------------------------------
cam3dAnimKeys::cam3dAnimKeys(const anKeyData<maPoint3d>* i_pTranslationChannel,
								 const anKeyData<maRotation>* i_pRotationChannel,
								 const anKeyData<float>* i_pCenterOfInterestChannel,
								 const anKeyData<float>* i_pFocalLengthChannel,
								 const anKeyData<float>* i_pHorizApertChannel,
								 const anKeyData<float>* i_pInteraxialSepChannel,
								 const anKeyData<float>* i_pZeroParallaxChannel)
: m_pPositionKeys(NULL), m_pViewKeys(NULL), m_pFovKeys(NULL), m_pTiltKeys(NULL), 
	m_pInteraxialSepKeys(NULL), m_pZeroParallaxKeys(NULL), m_pFocalLengthKeys(NULL),
	m_pHorizontalApertureKeys(NULL)
{
	if (i_pTranslationChannel)
	{
		m_pPositionKeys = new anKeyData<maPoint3d>(*i_pTranslationChannel);
	}
	if (i_pRotationChannel)
	{
		int num_keys = i_pRotationChannel->GetNumKeys();
		float time = 0, tilt = 0;
		maRotation value;
		maVector3d vec;
		float view_len = c_DefaultViewLength;

		m_pViewKeys = new anKeyData<maVector3d>();
		m_pTiltKeys = new anKeyData<float>();

		for (int i=0; i<num_keys; i++)
		{
			i_pRotationChannel->GetKeyData(i, time, value);

//			if (i_pCenterOfInterestChannel)
//			{
//				view_len = i_pCenterOfInterestChannel->GetValue(time);
//				DBG_LOG2("CenterOfInterest: %f - %f", time, view_len);
//			}

			vec = convert_to_vector(value, view_len);
//			DBG_LOG4("Converting view key: %f - %f %f %f", time, vec[0], vec[1], vec[2]);
			m_pViewKeys->AddKey(time, vec);

			tilt = compute_tilt(value, vec);
//			DBG_LOG2("Computing tilt key: %f - %f", time, tilt);
			m_pTiltKeys->AddKey(time, tilt);
		}
	}
		
	if (i_pHorizApertChannel)
	{
		// Make copy of horizontal aperture
		m_pHorizontalApertureKeys = new anKeyData<float>(*i_pHorizApertChannel);
	}

	if (i_pFocalLengthChannel)
	{
		// Make copy of focal length by itself
		m_pFocalLengthKeys = new anKeyData<float>(*i_pFocalLengthChannel);

		// Convert from focal length to FOV
		int num_keys = i_pFocalLengthChannel->GetNumKeys();
		float time = 0, focal_len = 0;

		m_pFovKeys = new anKeyData<float>();
		float fov, hapt = cam3dUtil::c_35mmFilmDimension;
		for (int i=0; i<num_keys; i++)
		{
			i_pFocalLengthChannel->GetKeyData(i, time, focal_len);

			// If the horizontal aperture is changing, then 
			// use it when converting between focal length and field of view.
			// Otherwise, use 35mm default.
			if (i_pHorizApertChannel)
			{
				const float c_InchesToMM = 25.4f;
				hapt = i_pHorizApertChannel->GetValue(time) * c_InchesToMM;
			}

			fov = cam3dUtil::CalculateFieldOfView(focal_len, hapt);
//			DBG_LOG2("Converting fov key: %f - %f", time, fov);
			m_pFovKeys->AddKey(time, fov);
		}
	}

	// Stereo channels may be empty
	if (i_pInteraxialSepChannel)
	{
		m_pInteraxialSepKeys = new anKeyData<float>(*i_pInteraxialSepChannel);
	}
	if (i_pZeroParallaxChannel)
	{
		m_pZeroParallaxKeys = new anKeyData<float>(*i_pZeroParallaxChannel);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cam3dAnimKeys::~cam3dAnimKeys()
{
	if (m_pPositionKeys) delete m_pPositionKeys;
	if (m_pViewKeys) delete m_pViewKeys;
	if (m_pTiltKeys) delete m_pTiltKeys;
	if (m_pFovKeys) delete m_pFovKeys;
	if (m_pFocalLengthKeys) delete m_pFocalLengthKeys;
	if (m_pHorizontalApertureKeys) delete m_pHorizontalApertureKeys;
	if (m_pInteraxialSepKeys) delete m_pInteraxialSepKeys;
	if (m_pZeroParallaxKeys) delete m_pZeroParallaxKeys;
}

//--------------------------------------------------------------------
// TestForCut - test to see if the camera did a "cut" during
// the bracketing keys. It tests this by seeing if the position
// change is greater than the given threshold.
//--------------------------------------------------------------------
bool cam3dAnimKeys::TestForCut(float i_Time, float i_Threshold) const
{
	if (m_pPositionKeys)
	{
		int key1 = 0, key2 = 0;
		m_pPositionKeys->GetBracketingKeyData(i_Time, key1, key2);

		if (key1 != key2)
		{
			float time1 = 0, time2 = 0;
			maPoint3d pos1, pos2;
			m_pPositionKeys->GetKeyData(key1, time1, pos1);
			m_pPositionKeys->GetKeyData(key2, time2, pos2);

			// Time needs to be one frame apart, this means that the
			// animation was baked. If animation wasn't baked, then
			// the large moves are supposed to be interpolated.
			if (time2 - time1 < 1.01)
			{
				// Test if distance is greater than threshold
				// (using squared distance to avoid square root)
				maVector3d diff = pos2 - pos1;
				if (diff.LengthSqr() > i_Threshold*i_Threshold)
				{
					return true;
				}
			}
		}
	}

	return false;
}

//--------------------------------------------------------------------
// GetPosition - get linear interpolated position key data
//--------------------------------------------------------------------
maPoint3d cam3dAnimKeys::GetPosition(float i_Time) const
{
	if (m_pPositionKeys)
		return m_pPositionKeys->GetValue(i_Time);
	else
		return maPoint3d(0,0,0);
}

//--------------------------------------------------------------------
// GetViewVector - get interpolated vector from position to target. 
//	This should be added to the position to get the target position.
//--------------------------------------------------------------------
maPoint3d cam3dAnimKeys::GetViewVector(float i_Time) const
{
	if (m_pViewKeys)
		return m_pViewKeys->GetValue(i_Time);
	else
		return maPoint3d(0,0,-1);
}

//--------------------------------------------------------------------
// GetFieldOfView - get interpolated field of view angle in degrees
//--------------------------------------------------------------------
float cam3dAnimKeys::GetFieldOfView(float i_Time) const
{
	if (m_pFovKeys)
		return m_pFovKeys->GetValue(i_Time);
	else
		return 90.0f;
}

//--------------------------------------------------------------------
// GetFocalLength - get interpolated focal length
//--------------------------------------------------------------------
float cam3dAnimKeys::GetFocalLength(float i_Time) const
{
	if (m_pFocalLengthKeys)
		return m_pFocalLengthKeys->GetValue(i_Time);
	else
		return 35.0f;	// default in Maya

}
//--------------------------------------------------------------------
// GetHorizontalAperture - get interpolated focal length
//--------------------------------------------------------------------
float cam3dAnimKeys::GetHorizontalAperture(float i_Time) const
{
	if (m_pHorizontalApertureKeys)
		return m_pHorizontalApertureKeys->GetValue(i_Time);
	else
		return 1.417f;	// default in Maya (36mm converted to inches)
}

//--------------------------------------------------------------------
// GetTilt - get interpolated tilt angle in degrees
//--------------------------------------------------------------------
float cam3dAnimKeys::GetTilt(float i_Time) const
{
	if (m_pTiltKeys)
		return m_pTiltKeys->GetValue(i_Time);
	else
		return 0.0f;
}

//--------------------------------------------------------------------
// GetInteraxialSeparation - get interpolated interaxial separation
// in centimeters
//--------------------------------------------------------------------
float cam3dAnimKeys::GetInteraxialSeparation(float i_Time) const
{
	if (m_pInteraxialSepKeys)
		return m_pInteraxialSepKeys->GetValue(i_Time);
	else
		return 0.0f;
}

//--------------------------------------------------------------------
// GetZeroParallax - get interpolated zero parallax
// in centimeters
//--------------------------------------------------------------------
float cam3dAnimKeys::GetZeroParallax(float i_Time) const
{
	if (m_pZeroParallaxKeys)
		return m_pZeroParallaxKeys->GetValue(i_Time);
	else
		return 20.0f;
}

//--------------------------------------------------------------------
// Return number of frames in longest animation
//--------------------------------------------------------------------
int cam3dAnimKeys::GetNumFrames()
{
	int max_num_frames = 0;
	if (m_pPositionKeys)
		max_num_frames = (int) m_pPositionKeys->GetLength();
	if (m_pViewKeys && m_pViewKeys->GetLength() > max_num_frames)
		max_num_frames = (int) m_pViewKeys->GetLength();
	if (m_pTiltKeys && m_pTiltKeys->GetLength() > max_num_frames)
		max_num_frames = (int) m_pTiltKeys->GetLength();
	if (m_pFovKeys && m_pFovKeys->GetLength() > max_num_frames)
		max_num_frames = (int) m_pFovKeys->GetLength();
	return max_num_frames;
}
