/*****************************************************************************\
**	keyfKeyframeUtil.hpp
**
**		Provides method for handling keyframing from context.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef KEYF_KEYFRAMEUTIL_HPP
#error keyfKeyframeUtil.hpp multiply included
#endif
#define KEYF_KEYFRAMEUTIL_HPP

class tmlnChannel;
class tmlnScriptObject;

//============================================================================
//============================================================================
namespace keyfKeyframeUtil
{
	//--------------------------------------------------------------------
	// Look at the scripted context of the selected objects and
	// either create new keys where needed or alter the existing
	// drivers to match the current channel values.
	//--------------------------------------------------------------------
	void DoKeyframe(bool i_bQuietMode);
	void DoKeyframe(); 

	//--------------------------------------------------------------------
	//	Either alters a existing driver on the given channel or 
	//	creates a key driver for the given channel at the current time.
	//	Returns true if either a driver could be altered or a new
	//	driver could be successfully created.
	//--------------------------------------------------------------------
	bool AlterOrCreateKey(tmlnScriptObject *i_pObject, 
						  tmlnChannel *i_pChannel);

	//--------------------------------------------------------------------
	// In auto-key mode, check every frame for changes to channels and
	// then create or alter drivers immediately.
	//--------------------------------------------------------------------
	void SetAutoKey(bool i_bVal);
	bool IsAutoKey();
};
