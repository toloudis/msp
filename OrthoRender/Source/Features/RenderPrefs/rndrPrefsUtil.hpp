/****************************************************************************\
**	rndrPrefsUtil.hpp
**
**		A utility for turning on and off multipsass rendering. This
**	also sets the state of the headlight.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef RNDR_PREFSUTIL_HPP
#error rndrPrefsUtil.hpp multiply included
#endif
#define RNDR_PREFSUTIL_HPP


//============================================================================
//	Forward References
//============================================================================


//============================================================================
//============================================================================
namespace rndrPrefsUtil
{
	//--------------------------------------------------------------------
	//	SetMultipassRendering - Turn on or off multipass lighting.
	//--------------------------------------------------------------------
	void SetMultipassRendering(bool i_bShadows);
};

