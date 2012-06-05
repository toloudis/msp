/*****************************************************************************
**	fcuiConstants.hpp
**
**		general application constants
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef FCUI_CONSTANTS_HPP
#error fcuiConstants.hpp multiply included
#endif
#define FCUI_CONSTANTS_HPP

#ifndef ENV_TYPE_HPP
#include "Core/Env/envType.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/It/itString.hpp"
#endif


//============================================================================
//	Defines
//============================================================================
namespace fcuiConstants
{
	//--------------------------------------------------------------------
	//	product constants
	//--------------------------------------------------------------------
	static const char* c_UI_MODE_ICONS	= "Modes";
	static const char* c_UI_GUI			= "GUI";
	static const char* c_MODES			= "Modes";
	static const char* c_MEDIA			= "Media";
	static const char* c_MODE_CAST		= "Cast";
	static const char* c_MODE_LOCATION	= "Location";
	static const char* c_MODE_ACTION	= "Action";
	static const char* c_MODE_MAKE		= "Shoot";
	static const char* c_MODE_THEATER	= "Theater";
	static const char* c_ACTION_SCENE_CATEGORY	= "Sequences";
	static const char* c_ACTION_LIGHT_CATEGORY	= "Lighting";
	static const char* c_ACTION_TITLECARD_CATEGORY	= "Cards";
	static const char* c_ACTION_AUDIO_CATEGORY	= "Audio";
	static const char* c_ACTION_POSTFX_CATEGORY	= "PostFX";
	static const char* c_ACTION_STATE_SCENE		= "01.Scene";
	static const char* c_ACTION_STATE_LIGHTS	= "02.Lights";
	static const char* c_ACTION_STATE_TITLECARD	= "03.Title Cards";
	static const char* c_ACTION_STATE_AUDIO		= "04.Audio";
	static const char* c_ACTION_STATE_POSTFX	= "05.Post FX";
	static const char* c_LOCATION_STATE_IMAGES	= "01.Images";
	static const char* c_LOCATION_STATE_VIDEO	= "02.Videos";
	static const char* c_LOCATION_STATE_PROPS	= "03.Props";
	static const char* c_LOCATION_STATE_THEMES	= "04.Themes";
	static const char* c_LOCATION_STATE_SETS	= "05.Set";
	static const char* c_LOCATION_STATE_VFX		= "04.VFX";
	static const char* c_BILLBOARD_OBJECT_NAME	= "Titlecards";
	static const char* c_IMAGE_USER_MATERIAL	= "UserContent";

	//	Cameras
	static const char* c_CAMERA_BASENAME		= "Cam_";
	static const char* c_CAMERA_BILLBOARD		= "Cam_Billboard";
	static const char* c_CAMERA_CAST			= "Cam_Cast";
	static const char* c_CAMERA_DIRECTOR		= "Director";

	//	folders
	static const char* c_FOLDER_CACHE_IMAGES	= "Images";
	static const char* c_XML_MAIN_TAG			= "StudioGPU";
	static const char* c_XML_FILE_PATH_TAG		= "File";
	static const char* c_MODE_ICONS				= "ModeIcons";

	//	Files
	static const char* c_FILE_BROWSE_IMAGE		= "Folder.jpg";
	static const char* c_FILE_BROWSE_AUDIO		= "Custom.wav";
	static const char* c_FILE_BROWSE_VIDEO		= "Custom.avi";
	static const char* c_FILE_MOVIEPACK_SCENE	= "Location.mab";
	static const char* c_FILE_NONE_IMAGE		= "None.jpg";
	static const char* c_FILE_NONE_AUDIO		= "None.wav";
	static const char* c_FILE_NONE_VIDEO		= "None.avi";
	static const char* c_FILE_NONE_PROPS		= "None.gxb";
	static const char* c_FILE_NONE_POSTFX		= "None.fx";
	static const char* c_FILE_NONE_MODEL		= "None.gxb";
	static const char* c_FILE_ICON				= "Icon.png";
	static const char* c_FILE_AUDIO_CUSTOM_ICON	= "icon-audio.png";
	static const char* c_FILE_VIDEO_CUSTOM_ICON	= "icon-video.png";

	//	File extensions
	static itString l_EXT_FX("fx");
	static itString l_EXT_GAB("gab");
	static itString l_EXT_GMB("gmb");
	static itString l_EXT_GXB("gxb");
	static itString l_EXT_MTL("mtl");
	static itString l_EXT_DDS("dds");
	static itString l_EXT_JPG("jpg");
	static itString l_EXT_PNG("png");
	static itString l_EXT_AVI("avi");
	static itString l_EXT_XML("xml");
	static itString l_EXT_MAB("mab");
	static itString l_EXT_WAV("WAV");
	static itString l_EXT_MP3("MP3");
	static itString l_EXT_PY("py");
}

