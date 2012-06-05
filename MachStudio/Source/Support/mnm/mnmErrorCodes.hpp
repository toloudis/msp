/*****************************************************************************
**  mnmErrorCodes.hpp
**
**      Constants
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef MNM_ERRORCODES_HPP
#error mnmErrorCodes.hpp multiply included
#endif
#define MNM_ERRORCODES_HPP


//============================================================================
// Error codes returned as return value of program itself.
//
// Note: these error codes are just high level categories, it is not 
// necessary to list every exception type here.
//============================================================================
namespace mnmErrorCodes
{
	static const int c_Success				= 0;	// Program completed successfully

	static const int c_AppAlreadyRunning	= -5;	// Application instance already running
	static const int c_UnhandledException	= -10;	// Program ended because of an exception

	static const int c_SceneFileNotFound	= -20;	// Scene file not found to load
	static const int c_CouldNotLoadScene	= -25;	// Asset in scene file was missing, or not enough memory

	static const int c_CaptureAborted		= -30;	// Capture aborted before finished by error 
	static const int c_UserAborted			= -35;	// Capture aborted before finished by user

	static const int c_InitializationError	= -40;	// Could not start application
	static const int c_ErrorCreatingGraphics= -45;	// Could not create graphics device
	static const int c_ErrorLoadShaders		= -46;	// Could not load shaders

}
