/****************************************************************************\
**	keyfThinkInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Features/Keyframing/keyfThinkInterest.hpp"

#include "Features/Keyframing/keyfKeyframeUtil.hpp"

#include "Input/in/inDeviceMgr.hpp"
#include "Tool/tma3d/tma3dCursorMgr.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
keyfThinkInterest::keyfThinkInterest()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
keyfThinkInterest::~keyfThinkInterest()
{
}


//--------------------------------------------------------------------
//	Think
//--------------------------------------------------------------------
//virtual 
void keyfThinkInterest::Think( )
{
	inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();
	if (keyfKeyframeUtil::IsAutoKey())
	{
		// Disable auto-key when ALT or CTRL is down
		if (  (    !pKeyboard->IsDown(inKeys::e_LALT) 
				&& !pKeyboard->IsDown(inKeys::e_RALT)
				&& !pKeyboard->IsDown(inKeys::e_RCTRL)
				&& !pKeyboard->IsDown(inKeys::e_LCTRL)) )
		{
			const bool quiet_mode = true;
			const bool bAllChannelsWithDrivers = false;
			keyfKeyframeUtil::DoKeyframe(quiet_mode, bAllChannelsWithDrivers);
		}
	}

	// Hotkey, just a single 'K' press to set a key.
	//if (tma3dCursorMgr::IsCursorOverView())
	//{
	//	if ( pKeyboard->IsReleased(inKeys::e_K) )
	//	{
	//		keyfKeyframeUtil::DoKeyframe();
	//	}
	//}
}
