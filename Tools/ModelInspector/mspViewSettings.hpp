/*****************************************************************************
**	mspViewSettings.hpp
**
**	 Holds viewing information as properties in order to 
**	allow registering callbacks for changes.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MSP_VIEWSETTINGS_HPP
#error mspViewSettings.hpp multiply included
#endif
#define MSP_VIEWSETTINGS_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif 
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif 
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif 


//============================================================================
//============================================================================
class mspViewSettings
{
public:
	static prtyBoolean sm_ViewWireframe;
	static prtyBoolean sm_PauseAnimation;
	static prtyFloat sm_CurrentFrame;

	// Information about the animation for the model
	static prtyFileName sm_ModelAnimFilename;
	static prtyFloat sm_ModelAnimShift;
	static prtyFloat sm_ModelAnimNumFrames;

	// Information about the camera animation
	static prtyFileName sm_CameraAnimFilename;
	static prtyFloat sm_CameraAnimShift;
	static prtyFloat sm_CameraAnimNumFrames;

	// Information about the playback range
	static prtyBoolean sm_UseModelRange;
	static prtyFloat sm_PlaybackBegin;
	static prtyFloat sm_PlaybackEnd;

	//accessor functions which cmaCommandToggle can use
	static void SetPauseAnimation( bool i_bVal )
	{
		sm_PauseAnimation.SetValue( i_bVal );
	}
	static bool GetPauseAnimation( )
	{
		return sm_PauseAnimation.GetValue(  );
	}
	static void SetViewWireframe( bool i_bVal )
	{
		sm_ViewWireframe.SetValue( i_bVal );
	}
	static bool GetViewWireframe( )
	{
		return sm_ViewWireframe.GetValue(  );
	}
};
