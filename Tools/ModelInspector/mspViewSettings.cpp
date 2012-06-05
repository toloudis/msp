/*****************************************************************************
**  mspViewSettings.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "mspViewSettings.hpp"

//============================================================================
// Default value for static properties
//============================================================================
prtyBoolean mspViewSettings::sm_ViewWireframe("View Wireframe", false);
prtyBoolean mspViewSettings::sm_PauseAnimation("Pause Animation", false);
prtyFloat mspViewSettings::sm_CurrentFrame("Current Frame", 0);

prtyFileName mspViewSettings::sm_ModelAnimFilename("Model Anim Filename", itString());
prtyFloat mspViewSettings::sm_ModelAnimShift("Model Anim Shift", 0);
prtyFloat mspViewSettings::sm_ModelAnimNumFrames("Model Anim Num Frames", 0);

prtyFileName mspViewSettings::sm_CameraAnimFilename("Camera Anim Filename", itString());
prtyFloat mspViewSettings::sm_CameraAnimShift("Camera Anim Shift", 0);
prtyFloat mspViewSettings::sm_CameraAnimNumFrames("Camera Anim Num Frames", 0);

prtyBoolean mspViewSettings::sm_UseModelRange("Use Model Range", true);
prtyFloat mspViewSettings::sm_PlaybackBegin("Playback Begin", 0);
prtyFloat mspViewSettings::sm_PlaybackEnd("Playback End", 0);
