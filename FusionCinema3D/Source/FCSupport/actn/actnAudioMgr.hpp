//****************************************************************************
//	actnAudioMgr.hpp
//
//	A manager for Audio
//
//	StudioGPU
//	Copyright(C) 2010 - All Rights Reserved
//****************************************************************************
#ifdef ACTN_AUDIOMGR_HPP
#error actnAudioMgr.hpp multiply included
#endif
#define ACTN_AUDIOMGR_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif


//============================================================================
//============================================================================
class actnAudioMgr
{
public:
	///-----------------------------------------------------------------------
	/// constructors
	///-----------------------------------------------------------------------
	actnAudioMgr();

	///-----------------------------------------------------------------------
	/// destructors
	///-----------------------------------------------------------------------
	~actnAudioMgr();

	///-----------------------------------------------------------------------
	/// Return the current instance of the mgr singleton
	///-----------------------------------------------------------------------
	static actnAudioMgr* Instance;

	//--------------------------------------------------------------------
	///	Deinitialize/Initialize
	//--------------------------------------------------------------------
	void Initialize();
	void DeInitialize();

	///---------------------------------------------------------------------------
	/// Perform the mode specific operations when a Audio element needs to 
	/// execute an action.
	///---------------------------------------------------------------------------
	void ProcessAudio(const fsLocator& i_AudioFile);

	///---------------------------------------------------------------------------
	/// Set/Get the checked item for audio
	///---------------------------------------------------------------------------
	void SetCheckedItem(const fsLocator& i_AudioFile);
	const fsLocator& GetCheckedItem();

private:
	fsLocator m_CheckedLocator;
	itString m_CurrentSoundName;
};
