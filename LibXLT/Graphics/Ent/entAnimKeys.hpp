/*****************************************************************************
**  entAnimKeys.hpp
**
**       entAnimKeys is the base class for holders of animation data
**	for derived implementations
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef ENT_ANIMKEYS_HPP
#error entAnimKeys.hpp multiply included
#endif
#define ENT_ANIMKEYS_HPP

#ifndef G3D_CONSTANTS_HPP
#include "Graphics/G3d/g3dConstants.hpp"
#endif 

//============================================================================
//============================================================================
class entAnimKeys
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	entAnimKeys() 
		: m_StartTime(0), 
		  m_FramesPerSecond(g3dConstants::c_fDefaultFrameRate) {}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~entAnimKeys() {};

	//--------------------------------------------------------------------
	//	StartTime - time at which animation was located when exported.
	//		Frame animation needs to start at 0, so this time
	//		restores the location of the animation within the timeline.
	//--------------------------------------------------------------------
	inline float	GetStartTime() const;
	inline void		SetStartTime(float i_StartTime);

	//--------------------------------------------------------------------
	// Return preferred frame rate to play animation
	//--------------------------------------------------------------------
	inline void SetFramesPerSecond(float i_Fps);
	inline float GetFramesPerSecond() const;

private:
	float m_StartTime;
	float m_FramesPerSecond;
};


//--------------------------------------------------------------------
//	StartTime - time at which animation was located when exported.
//		Frame animation needs to start at 0, so this time
//		restores the location of the animation within the timeline.
//--------------------------------------------------------------------
inline float entAnimKeys::GetStartTime() const
{
	return m_StartTime;
}
inline void	entAnimKeys::SetStartTime(float i_StartTime)
{
	m_StartTime = i_StartTime;
}

//--------------------------------------------------------------------
// Return preferred frame rate to play animation
//--------------------------------------------------------------------
inline float entAnimKeys::GetFramesPerSecond() const
{
	return m_FramesPerSecond;
}

//--------------------------------------------------------------------
// Return preferred frame rate to play animation
//--------------------------------------------------------------------
inline void entAnimKeys::SetFramesPerSecond(float i_Fps)
{
	m_FramesPerSecond = i_Fps;
}
