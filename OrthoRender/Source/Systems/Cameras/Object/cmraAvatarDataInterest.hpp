/*****************************************************************************
**  cmraAvatarDataInterest.hpp
**
**      the interest for when Avatar camera data changes
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CMRA_AVATARDATAINTEREST_HPP
#error cmraAvatarDataInterest.hpp multiply included
#endif
#define CMRA_AVATARDATAINTEREST_HPP

#ifndef ORTHO_AVATARDATAINTEREST_HPP
#include "Features/Capture/orthoAvatarDataInterest.hpp"
#endif

//----------------------------------------------------------------------------
//	forward references
//----------------------------------------------------------------------------
class tmlnDriver;

//============================================================================
//============================================================================
class cmraAvatarDataInterest : public orthoAvatarDataInterest
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraAvatarDataInterest();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmraAvatarDataInterest();

	//--------------------------------------------------------------------
	// Only add the capture driver for the camera
	//--------------------------------------------------------------------
	virtual void AddAnimation( const std::string& i_AnimFile, const int i_Divisions, const std::vector<std::string>& i_Cameras );

	//--------------------------------------------------------------------
	//	Delete All Drivers
	//--------------------------------------------------------------------
	virtual void DeleteSceneDrivers();

	//--------------------------------------------------------------------
	//	Change Camera
	//--------------------------------------------------------------------
	void CameraChange( const std::string& i_Camera, const cmraCameraChangeData& i_Data );
};
