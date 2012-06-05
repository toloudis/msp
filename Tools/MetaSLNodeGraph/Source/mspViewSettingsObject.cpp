/*****************************************************************************
**  mspViewSettingsObject.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "mspViewSettingsObject.hpp"
#include "mspViewSettings.hpp"

#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Tool/gui/guiDialogTabbedMgr.hpp"

namespace
{
	shared_ptr<mspViewSettingsObject*> l_SettingsObject;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mspViewSettingsObject::mspViewSettingsObject()
{
	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;

	// Model animation properties
	pPUII = new prtyTextBoxUIInfo(&(mspViewSettings::sm_ModelAnimFilename), "Model Animation", "Filename of model animation");
	pPUII->SetReadOnly(true);
	AddProperty( pPUII );	
	pPUII  = new prtyFloatEditUIInfo(&(mspViewSettings::sm_ModelAnimShift), "Model Animation", "Start frame offset for model animation");
	AddProperty( pPUII );	
	pPUII  = new prtyFloatEditUIInfo(&(mspViewSettings::sm_ModelAnimNumFrames), "Model Animation", "Number of frames in model animation");
	pPUII->SetReadOnly(true);
	AddProperty( pPUII );	

	// Camera animation properties
	pPUII = new prtyTextBoxUIInfo(&(mspViewSettings::sm_CameraAnimFilename), "Camera Animation", "Filename of model animation");
	pPUII->SetReadOnly(true);
	AddProperty( pPUII );	
	pPUII  = new prtyFloatEditUIInfo(&(mspViewSettings::sm_CameraAnimShift), "Camera Animation", "Start frame offset for model animation");
	AddProperty( pPUII );	
	pPUII  = new prtyFloatEditUIInfo(&(mspViewSettings::sm_CameraAnimNumFrames), "Camera Animation", "Number of frames in model animation");
	pPUII->SetReadOnly(true);
	AddProperty( pPUII );	

	// Playback Range properties
	pPUII  = new prtyCheckBoxUIInfo(&(mspViewSettings::sm_UseModelRange), "Playback Range", "Use model's animation as playback range");
	AddProperty( pPUII );	
	m_pPlaybackBeginUIInfo = new prtyFloatEditUIInfo(&(mspViewSettings::sm_PlaybackBegin), "Playback Range", "Start frame of playback range");
	m_pPlaybackBeginUIInfo->SetReadOnly(true);
	AddProperty( m_pPlaybackBeginUIInfo );	
	m_pPlaybackEndUIInfo = new prtyFloatEditUIInfo(&(mspViewSettings::sm_PlaybackEnd), "Playback Range", "End frame of playback range");
	m_pPlaybackEndUIInfo->SetReadOnly(true);
	AddProperty( m_pPlaybackEndUIInfo );

	m_pUseModelRangeCallback.reset(new prtyCallbackWrapper<mspViewSettingsObject>(this, &mspViewSettingsObject::UseModelRangeChanged));
	mspViewSettings::sm_UseModelRange.AddCallback(m_pUseModelRangeCallback);
	m_pModelShiftCallback.reset(new prtyCallbackWrapper<mspViewSettingsObject>(this, &mspViewSettingsObject::UseModelRangeChanged));
	mspViewSettings::sm_ModelAnimShift.AddCallback(m_pModelShiftCallback);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mspViewSettingsObject::~mspViewSettingsObject()
{
	mspViewSettings::sm_UseModelRange.RemoveCallback(m_pUseModelRangeCallback);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mspViewSettingsObject::UseModelRangeChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	bool bUseRange = mspViewSettings::sm_UseModelRange.GetValue();	
	if (bUseRange)
	{
		float anim_shift = mspViewSettings::sm_ModelAnimShift.GetValue();
		mspViewSettings::sm_PlaybackBegin.SetValue(anim_shift);
		mspViewSettings::sm_PlaybackEnd.SetValue(mspViewSettings::sm_ModelAnimNumFrames.GetValue() + anim_shift);
	}

	m_pPlaybackBeginUIInfo->SetReadOnly(bUseRange);
	m_pPlaybackEndUIInfo->SetReadOnly(bUseRange);

	m_pPlaybackBeginUIInfo->UpdateControl();
	m_pPlaybackEndUIInfo->UpdateControl();
}
