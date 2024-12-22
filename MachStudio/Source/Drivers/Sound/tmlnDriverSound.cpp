/*****************************************************************************
**	tmlnDriverSound.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Drivers/Sound/tmlnDriverSound.hpp"

//	
#include "Drivers/Sound/tmlnDriverSoundInfo.hpp"
#include "Drivers/Sound/tmlnDriverSoundParser.hpp"

//	App
#include "Features/Channels/chnlDialogUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/tmln/tmlnAdapterPosition.hpp"

//	Library
#include "AudioDS/sn/snExceptionX.hpp"
#include "AudioDS/sn/snSoundJob2D.hpp"
#include "AudioDS/sn/snSoundManager.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Tool/gui/guiMessageBox.hpp"

//
#include <assert.h>


//============================================================================
//============================================================================
namespace tmlnDriverSoundNS
{
	const float c_SOUND_CATCHUP_GAP = 0.015f * 2;
}

bool tmlnDriverSound::sm_bSkipSounds = false;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverSound::tmlnDriverSound(tmlnAdapterGetPosition &i_Adapter,
								 const fsLocator& i_InitSearchDirRelative)
:	m_Adapter(i_Adapter), 
	m_pPlayingSound(NULL),
	m_PauseCounter(0L),
	m_bStarted(false),
	m_AvgFrameRate( (int)(ceil(g3dConstants::c_fDefaultFrameRate))),
	m_bNeedsLoad(true),
	m_SoundSearchDir(i_InitSearchDirRelative),
	m_SoundName("Sound Name"),
	m_fSoundStartTime("Sound Start Time", 0.0f),
	m_fSoundEndTime("Sound End Time", 0.0f),
	m_bSetToSoundLength("Set to Sound Length",false)
{
	// DEBUG ONLY
	//std::string filename;
	//fsFileUtil::LocatorToANSIFilename(i_InitSearchDirRelative, filename);
	//DBG_LOG("Driver Sound - relative dir (" << filename.c_str() << ")"  );

	// TODO UIINFO
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyFloatEditUIInfo(&(m_fSoundStartTime), "Audio", "Seconds to skip at the start of sound (within sound file)");
	AddProperty( pPUII );
	pPUII = new prtyFloatEditUIInfo(&(m_fSoundEndTime), "Audio", "Seconds to skip at the end of sound (within sound file)");
	AddProperty( pPUII );
	//pPUII = new prtyPropertyUIInfo(&(m_SoundSearchDir), "Audio", "Directory to search for sounds");
	//AddProperty( pPUII );
	prtyFileChooserUIInfo *pFCUII = new prtyFileChooserUIInfo(&(m_SoundName), "Audio", "Name of the sound file");
	pFCUII->SetDirectoryCategory("Sounds");
	AddProperty( pFCUII );
	pPUII = new prtyCheckBoxUIInfo(&(m_bSetToSoundLength), "Audio", "Flag to set driver to length of sound.");
	AddProperty( pPUII );

	//	property callbacks
	m_fSoundStartTime.AddCallback(new prtyCallbackWrapper<tmlnDriverSound>(this, &tmlnDriverSound::PropertyChanged));
	m_fSoundEndTime.AddCallback(new prtyCallbackWrapper<tmlnDriverSound>(this, &tmlnDriverSound::PropertyChanged));
	m_bSetToSoundLength.AddCallback(new prtyCallbackWrapper<tmlnDriverSound>(this, &tmlnDriverSound::PropertyChanged));
	m_SoundName.AddCallback(new prtyCallbackWrapper<tmlnDriverSound>(this, &tmlnDriverSound::SoundFileChanged));
	//m_SoundSearchDir.AddCallback(new prtyCallbackWrapper<tmlnDriverSound>(this, &tmlnDriverSound::PropertyChanged));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverSound::~tmlnDriverSound()
{
	if (m_pPlayingSound != 0)
	{
		snSoundManager::DeleteSoundJob(m_pPlayingSound);
	}
}

//----------------------------------------------------------------------------
// Set all drivers to skip the loading of all sounds in order to
//	speed loading when sounds aren't needed.
//----------------------------------------------------------------------------
//static 
void tmlnDriverSound::SetSkipSounds(bool i_bSkip)
{
	sm_bSkipSounds = i_bSkip;
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string tmlnDriverSound::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();

	std::string str = "Sound: ";
	if (m_SoundName.GetValue().GetNumNames())
		str += itStringUtil::GetStdString(m_SoundName.GetValue().GetLastName());
	desc += str;
	return desc;
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  tmlnDriverSound::GetDriverInfo() const
{
	tmlnDriverSoundInfo *pInfo = new tmlnDriverSoundInfo(tmlnDriverSoundParser::GetChunkName());

	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_SoundName			= this->m_SoundName.GetValue();
	pInfo->m_fSoundStartTime	= this->m_fSoundStartTime.GetValue();
	pInfo->m_fSoundEndTime		= this->m_fSoundEndTime.GetValue();
	pInfo->m_bSetToSoundLength	= this->m_bSetToSoundLength.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void tmlnDriverSound::SetDriverInfo(const tmlnDriverSoundInfo& i_Info )
{
	// Set base info
	this->SetBaseDriverInfo(i_Info);

	// Store local info
	this->m_SoundName.SetValue(i_Info.m_SoundName);
	if (m_SoundName.GetValue().GetNumNames() > 0)
	{
		this->SetName(itStringUtil::GetStdString(m_SoundName.GetValue().GetLastName()).c_str());
	}
	this->m_fSoundStartTime.SetValue(i_Info.m_fSoundStartTime);
	this->m_fSoundEndTime.SetValue(i_Info.m_fSoundEndTime);
	this->m_bSetToSoundLength.SetValue(i_Info.m_bSetToSoundLength);

	if (m_bNeedsLoad || i_Info.m_bNeedsLoad)
	{
		load_sound(i_Info);
	}

	this->SetEndTime( calculate_endtime() );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
maTime tmlnDriverSound::calculate_duration()
{
	//	if no sound, leave the duration that is already there.
	//
	if ( m_pPlayingSound == NULL )
		return (GetEndTime() - GetBeginTime()); //0.0f;

	//	check if flag set to set the driver to the sound length
	//TIME - this math is being done in seconds
	float sound_length;
	if (m_bSetToSoundLength.GetValue())
	{
		sound_length = m_pPlayingSound->GetTimeLength();
	}
	else
	{
		sound_length = (GetEndTime() - GetBeginTime()).AsSeconds();
	}

	sound_length -= m_fSoundStartTime.GetValue();
	sound_length -= m_fSoundEndTime.GetValue();
	if (sound_length < 0) sound_length = 1;
	if (sound_length > m_pPlayingSound->GetTimeLength()) 
		sound_length = m_pPlayingSound->GetTimeLength();

	return maTime::FromSeconds(sound_length);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
maTime tmlnDriverSound::calculate_endtime()
{
	maTime end_time = GetBeginTime() + calculate_duration();
	return end_time;
}

//--------------------------------------------------------------------
//  Update position of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverSound::Operate(const maTime& i_Time)
{
	if ( m_pPlayingSound == 0 )
		return;

	bool bDriverActiveTime = (IsWithin(i_Time));
	if ( bDriverActiveTime )
	{
		//	see if the time isn't moving
		//
		if ( m_LastOperateTime == i_Time )
		{
			m_PauseCounter++;
			if ( m_PauseCounter > 3 )
			{
				//	time isn't moving so try to pause the sound.
				if (   ( !m_pPlayingSound->IsPaused() )
					&& ( m_pPlayingSound->IsPlaying() ) 
					)
				{
					//DBG_LOG("||--PAUSE");
					m_pPlayingSound->Pause();
				}
			}
		}
		else
		{
			m_PauseCounter = 0;

			//	if the sound is playing
			if ( m_pPlayingSound->IsPlaying() )
			{
				if ( m_pPlayingSound->IsPaused() )
				{
					//DBG_LOG("+>--RESUME");
					m_pPlayingSound->Resume(i_Time.AsSeconds());
					m_PauseCounter = 0;
				}

				//	if the difference in time is too great then skip the sound ahead a bit.
				//
				if ( !m_pPlayingSound->IsPaused() )
				{
					//	Get the elapsed play time for the sound and check 
					//	against the current elapsed time
					//
					//TIME - this math is being done in seconds
					float duration = (this->m_pPlayingSound->GetTimeLength());
					float fCurrentSoundPercent	= m_pPlayingSound->GetPositionInSoundByPercent();
					float fCurrentTimePercent	= m_fSoundStartTime.GetValue() + (i_Time - GetBeginTime()).AsSeconds() / duration;
					float fTimeGap = fabs( (fCurrentTimePercent - fCurrentSoundPercent) );
					if ( fCurrentTimePercent > 1.0f )
					{
						fCurrentTimePercent = 1.0f;
					}

					//DBG_LOG7("time %5.3f adj.time %5.3f duration(%9.4f) time gap (%7.3f) gap threshold(%7.3f) tpct(%7.3f) spct(%7.3f)", i_Time, (i_Time + m_fSoundStartTime - GetBeginTime()), duration, fTimeGap, tmlnDriverSoundNS::c_SOUND_CATCHUP_GAP, fCurrentTimePercent, fCurrentSoundPercent);

					//	if the playback gap lags rendering *or*
					//	if someone reset the sound to the beginning
					//	then set the sound to the correct place.
					//
					if (   (fTimeGap > tmlnDriverSoundNS::c_SOUND_CATCHUP_GAP)
						|| (m_LastOperateTime > i_Time ) )
					{
						//DBG_LOG3("---->    time%%%5.3f   sound%%%5.3f  diff(%9.4f)", fCurrentTimePercent, fCurrentSoundPercent, fabs( (fCurrentTimePercent - fCurrentSoundPercent) ) );
						//DBG_LOG2("---->lasttime %5.3f   time  %5.3f", m_LastOperateTime, i_Time );
						//DBG_LOG1("---->duration %5.3f", duration );

						m_pPlayingSound->SetPositionInSoundByPercent( fCurrentTimePercent );
					}
				}
			}
			else
			{
				//DBG_LOG("[>--START");
				m_pPlayingSound->Start(i_Time.AsSeconds());

				//	jump the time ahead to the user defined start time
				float duration = (this->m_pPlayingSound->GetTimeLength());// - GetBeginTime());
				float fCurrentTimePercent = m_fSoundStartTime.GetValue() / duration;
				m_pPlayingSound->SetPositionInSoundByPercent( fCurrentTimePercent );
				//DBG_LOG2("START---->    time%%%5.3f   dur(%9.4f)", fCurrentTimePercent, duration );

				m_PauseCounter = 0;
			}
		}
	}
	else
	{
		//	not in the driver timeframe so stop the playing
		//
		if (   (m_pPlayingSound != 0)
			&& (m_pPlayingSound->IsPlaying()) )
		{
			//DBG_LOG("[]--STOP");
			m_pPlayingSound->Stop();
			m_bStarted = false;
		}
	}

	m_LastOperateTime = i_Time;
	this->MarkDirty();
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA tmlnDriverSound::GetClipFillColor() const
{
	return maFloatRGBA( 1.0f, 0.5569f, 0.5569f, 1.0f );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float tmlnDriverSound::GetSoundStartTime() const
{
	return m_fSoundStartTime.GetValue();
}
void tmlnDriverSound::SetSoundStartTime(float i_fStartTime)
{
	m_fSoundStartTime.SetValue(i_fStartTime);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float tmlnDriverSound::GetSoundEndTime() const
{
	return m_fSoundEndTime.GetValue();
}
void tmlnDriverSound::SetSoundEndTime(float i_fEndTime)
{
	m_fSoundEndTime.SetValue(i_fEndTime);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool tmlnDriverSound::GetSetToSoundLength() const
{
	return m_bSetToSoundLength.GetValue();
}
void tmlnDriverSound::SetSetToSoundLength(bool i_bSetToLength)
{
	m_bSetToSoundLength.SetValue(i_bSetToLength);
}


//--------------------------------------------------------------------
//	PerformSplit - Split up the details of this driver between itself
//	and the driver passed in.
//--------------------------------------------------------------------
//virtual 
void tmlnDriverSound::PerformSplit( tmlnDriver* io_pDriverAtEnd, const maTime& i_fTime )
{
	float percent = tmlnDriver::CalculatePercent( i_fTime );
	//DBG_LOG1("Performing SPLIT - percent (%6.2f)", percent);

	//	cast the 2nd driver and check it is the same type
	//
	tmlnDriverSound* pSecondDriver = dynamic_cast<tmlnDriverSound*>(io_pDriverAtEnd);
	DBG_ASSERT( pSecondDriver != 0, "Cannot split with 2 different type of drivers" );

	this->SetSetToSoundLength(false);
	pSecondDriver->SetSetToSoundLength(false);
	
	//	split the custom parts of this driver
	//
	float startsoundtime, endsoundtime, duration;
	startsoundtime	= this->GetSoundStartTime();
	endsoundtime	= this->GetSoundEndTime();
	duration		= this->GetDuration().AsSeconds();
	//DBG_LOG3("sound start (%6.2f) end (%6.2f) duration (%6.2f)", startsoundtime, endsoundtime, duration);
	float midsoundtime;
	midsoundtime = duration * percent + startsoundtime;
	//DBG_LOG1("mid - 2nd sound start (%6.2f)", midsoundtime);
	pSecondDriver->SetSoundStartTime(midsoundtime);
	midsoundtime = (float)(duration * (1.0 - percent) + endsoundtime);
	//DBG_LOG1("mid - 1st sound end (%6.2f)", midsoundtime);
	this->SetSoundEndTime(midsoundtime);

	////	split the time + duration
	////
	//float starttime, endtime;
	//starttime	= this->GetBeginTime();
	//endtime		= this->GetEndTime();
	//DBG_LOG3("driver start (%6.2f) end (%6.2f) duration (%6.2f)", starttime, endtime, duration);
	//float midtime = starttime + duration * percent;
	//DBG_LOG1("mid - 2nd driver start (%6.2f)", midtime);
	//pSecondDriver->SetBeginTime( midtime );
	//this->SetEndTime( midtime );
	//DBG_LOG1("mid - 1st driver end (%6.2f)", midtime);

	//	set the driver times accordingly
	tmlnDriver::PerformSplit(io_pDriverAtEnd, i_fTime);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverSound::load_sound(const tmlnDriverSoundInfo& i_Info)
{
	// Clean up before setting
	if (m_pPlayingSound != 0)
	{
		m_pPlayingSound->Stop();
		snSoundManager::DeleteSoundJob(m_pPlayingSound);
		m_pPlayingSound = NULL;
	}

	// If we are skipping all sounds, return now without trying.
	if (sm_bSkipSounds)
		return;

	// Set flag right away so we don't keep trying to load a sound that doesn't exist
	m_bNeedsLoad = false;

	//	if no sound, don't continue
	//if (   (m_SoundSearchDir.GetValue().GetNumNames() == 0)
	//	|| (i_Info.m_SoundName.GetLength() == 0))
	if (i_Info.m_SoundName.GetNumNames() == 0)
	{
		//m_bNeedsLoad = false;
		return;
	}

	// Create sound
	std::unique_ptr<snSoundJob2D> sound( new snSoundJob2D );
	fsLocator s_locator;
	if (i_Info.m_SoundName.GetNumNames() == 1)
	{
		s_locator = gfPaths::GetAppPath();
		s_locator.Push(m_SoundSearchDir);
		s_locator.Push(i_Info.m_SoundName.GetLastName());
	}
	else
	{
		s_locator = i_Info.m_SoundName;
	}

	if (!fsFileUtil::FileExists(s_locator))
	{
		// New location is Data/Sounds
		s_locator = gfPaths::GetPath(mnmPaths::e_DataStock);
		s_locator.Push("Sounds");
		s_locator.Push(i_Info.m_SoundName.GetLastName());
	}

	sound->SetFilename(s_locator);
	sound->SetTypeMask( snSoundManager::SOUNDTYPE_EFFECT );

	// DEBUG ONLY
	//std::string filename;
	//fsFileUtil::LocatorToANSIFilename(s_locator, filename);
	//std::string sounddir;
	//fsFileUtil::LocatorToANSIFilename(m_SoundSearchDir.GetValue(), sounddir);
	//DBG_LOG3("Loading sound file (%s) sound dir (%s) sound name (%s)", filename.c_str(), sounddir.c_str(), i_Info.m_SoundName.GetString() );

	// Load sound
	//
	try
	{
		sound->Load();
	}
	catch ( const envExceptionX& i_Ex )
	{
		std::string msg = "Problem loading sound, " + i_Ex.GetErrorMessage();
		DBG_ERROR(msg);
		guiMessageBox::Show(msg.c_str(), "Sound Failure", guiMessageBox::e_OKOnly);
		return;
	}

	m_pPlayingSound = sound.release();
	snSoundManager::AddSoundJob(m_pPlayingSound);

	m_pPlayingSound->SetDeleteWhenFinished(false);
	snSoundManager::SetupSoundJob(m_pPlayingSound);

	//m_bNeedsLoad = false;

	//LoadSound();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverSound::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
		
	if (this->m_SoundName.GetValue().GetNumNames() > 0)
	{
		//	calculate the duration and set the end time
		//
		maTime duration = calculate_duration();

		SetEndTime( GetBeginTime() + duration );

		// update gui
		chnlDialogUtil::UpdateDriver(this);
	}

	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverSound::SoundFileChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	tmlnDriverSoundInfo *pInfo = dynamic_cast<tmlnDriverSoundInfo*>(GetDriverInfo());

	if ( pInfo == NULL )
		return;

	itString ext;
	pInfo->m_SoundName.GetLastName().GetExtension(ext);
	itStringUtil::ToLower(ext);

	if (ext == itString("wav") || ext == itString("mp3"))
	{
		maTime duration = calculate_duration();

		SetEndTime( GetBeginTime() + duration );

		// update gui
		chnlDialogUtil::UpdateDriver(this);
		m_bNeedsLoad = true;
		load_sound(*pInfo);
		//SetDriverInfo(*pInfo);

		this->MarkDirty();
	}
	delete pInfo;
}

