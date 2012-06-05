/*****************************************************************************
**	actnAudioMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/actn/actnAudioMgr.hpp"

#include "FCSupport/fcui/fcuiTimelineMgr.hpp"
#include "FCSupport/fcui/fcuiUtils.hpp"
#include "FCSupport/fcui/fcuiConstants.hpp"
#include "FCSupport/fcmd/fcmdConstants.hpp"
#include "Drivers/Sound/tmlnDriverSound.hpp"
#include "Drivers/Sound/tmlnDriverSoundInfo.hpp"
#include "Features/Import/ImportData.hpp"
#include "Features/Import/ImportUtil.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Systems/PrjLt/Object/prjltObjectMgr.hpp"
#include "Systems/Ptlt/Object/ptltObjectMgr.hpp"

#include "AudioDS/sn/snSoundManager.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "FCSupport/path/pathDirectoryParser.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"


///-----------------------------------------------------------------------
/// constructors
///-----------------------------------------------------------------------
actnAudioMgr::actnAudioMgr()
{
}

///-----------------------------------------------------------------------
/// destructors
///-----------------------------------------------------------------------
actnAudioMgr::~actnAudioMgr()
{
}

///---------------------------------------------------------------------------
///	the current instance of the mode mgr singleton
///---------------------------------------------------------------------------
actnAudioMgr* actnAudioMgr::Instance = NULL;

//--------------------------------------------------------------------
//	Deinitialize/Initialize
//--------------------------------------------------------------------
void actnAudioMgr::Initialize()
{
}
void actnAudioMgr::DeInitialize()
{
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when an audio element needs to 
/// execute an action.
///---------------------------------------------------------------------------
void actnAudioMgr::ProcessAudio(const fsLocator& i_AudioFile)
{
	std::string audio_str;
	snSoundManager::Free();

	//	note: this won't work for UNICODE!
	fsFileUtil::LocatorToANSIFilename(i_AudioFile, audio_str);

	captRenderOutputData& data = captRenderOutputDataUtil::Data();
	data.m_SoundFile.SetString(audio_str.c_str());
	//captRenderOutputDataUtil::SetData(data);

	maTime end_time = fcuiTimelineMgr::Instance->GetEndTime();

	itString objectname = fcuiUtils::GetObjectName();
	nameString obj_name(itStringUtil::GetStdString(objectname).c_str());
	if( m_CurrentSoundName.GetLength() > 0 )
		fcuiUtils::RemoveDriverAtTime(obj_name, itStringUtil::GetStdString(m_CurrentSoundName), maTime::c_ZeroTime);

	tmlnDriver* pAudioDriver = fcuiUtils::CreateDriver("Play Sound", obj_name, maTime::c_ZeroTime, end_time);
	tmlnDriverSound* pDriverSound;
	if(pAudioDriver)
	{
		pDriverSound = dynamic_cast<tmlnDriverSound*>(pAudioDriver);
		if(pDriverSound)
		{
			tmlnDriverSoundInfo* pDriverSoundInfo;
			pDriverSoundInfo = dynamic_cast<tmlnDriverSoundInfo*>(pDriverSound->GetDriverInfo());
			if(pDriverSoundInfo)
			{
				if(i_AudioFile.GetLastName() != itString(fcuiConstants::c_FILE_NONE_AUDIO))
				{
					pDriverSoundInfo->m_SoundName = i_AudioFile;
					pDriverSoundInfo->m_fSoundStartTime = 0.0f;
					pDriverSoundInfo->m_fSoundEndTime = 0.0f;
					pDriverSoundInfo->m_bSetToSoundLength = true;
					m_CurrentSoundName = i_AudioFile.GetLastName();
					m_CurrentSoundName.StripExtension();
				}
				else 
				{
					m_CurrentSoundName = itString("Play Sound");
					pDriverSoundInfo->m_fSoundStartTime = 0.0f;
					pDriverSoundInfo->m_fSoundEndTime = 0.0f;
				}
			}
			pDriverSound->SetDriverInfo(*pDriverSoundInfo);
			m_CurrentSoundName = i_AudioFile.GetLastName();
			m_CurrentSoundName.StripExtension();
		}
	}
}

///---------------------------------------------------------------------------
/// Set the checked item for audio
///---------------------------------------------------------------------------
void actnAudioMgr::SetCheckedItem(const fsLocator& i_AudioFile)
{
	m_CheckedLocator = i_AudioFile;
}

///---------------------------------------------------------------------------
/// Get the checked item for audio
///---------------------------------------------------------------------------
const fsLocator& actnAudioMgr::GetCheckedItem()
{
	return m_CheckedLocator;
}
