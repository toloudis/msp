/*****************************************************************************
**	mspViewSettingsObject.hpp
**
**	 Property object for editing animation settings.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MSP_VIEWSETTINGSOBJECT_HPP
#error mspViewSettingsObject.hpp multiply included
#endif
#define MSP_VIEWSETTINGSOBJECT_HPP

#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif 

class prtyPropertyCallback;
class prtyFloatEditUIInfo;

//============================================================================
//============================================================================
class mspViewSettingsObject : public prtyObject
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	mspViewSettingsObject();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~mspViewSettingsObject();

private:
	//--------------------------------------------------------------------
	// property callbacks
	//--------------------------------------------------------------------
	void UseModelRangeChanged(prtyProperty *i_pProperty, bool i_bDirty);

	shared_ptr<prtyPropertyCallback> m_pUseModelRangeCallback;
	shared_ptr<prtyPropertyCallback> m_pModelShiftCallback;

	prtyFloatEditUIInfo* m_pPlaybackBeginUIInfo;
	prtyFloatEditUIInfo* m_pPlaybackEndUIInfo;
};
