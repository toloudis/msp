/*****************************************************************************
**  mnmConstants.hpp
**
**      Constants
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef MNM_CONSTANTS_HPP
#error mnmConstants.hpp multiply included
#endif
#define MNM_CONSTANTS_HPP


//============================================================================
//============================================================================
//#define EON_REALITY 1


//============================================================================
//============================================================================
namespace mnmConstants
{
	//--------------------------------------------------------------------
	//	product constants
	//--------------------------------------------------------------------
#ifdef EON_REALITY
	static const char* c_COMPANY	= "Eon Reality";
	static const char* c_COPYRIGHT	= "Copyright 2008 Eon Reality, Inc.";
	static const char* c_PRODUCT	= "Eon Ultra Composer";
	static const char* c_EXECUTABLE	= "EUComposer.exe";
#else
	static const char* c_COMPANY	= "Extra Large Technology";
	static const char* c_COPYRIGHT	= "Copyright 2003-8 Extra Large Technology, Inc.";
	static const char* c_PRODUCT	= "Mach Studio";
	static const char* c_EXECUTABLE	= "MachStudio.exe";
#endif

	//--------------------------------------------------------------------
	//	Status Panels
	//--------------------------------------------------------------------
	enum eStatusBarPanels
	{
		e_SBPanel_Mode = 0,
		e_SBPanel_Messages,
		e_SBPanel_Key,
		e_SBPanel_Mute,
		e_SBPanel_Particles,
		e_SBPanel_Camera,
		e_SBPanel_Render_Flags,
		e_SBPanel_Subdiv,
		e_SBPanel_Memory,
		e_SBPanel_NumberOfPanels
	};
}
