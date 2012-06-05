/*****************************************************************************
**	tmlnDriverSound.hpp
**
**	Derived driver class which plays localized sounds
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef TMLN_DRIVERSOUND_HPP
#error tmlnDriverSound.hpp multiply included
#endif
#define TMLN_DRIVERSOUND_HPP

#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef MA_RUNNINGAVERAGE_HPP
#include "Core/ma/maRunningAverage.hpp"
#endif

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif


//============================================================================
//============================================================================
class tmlnAdapterGetPosition;
class tmlnDriverSoundInfo;
class snSoundJob2D;
class fsLocator;


//============================================================================
//============================================================================
class tmlnDriverSound : public tmlnDriver
{

public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverSound(tmlnAdapterGetPosition &i_Adapter, const fsLocator& i_InitSearchDirRelative);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~tmlnDriverSound();

	//----------------------------------------------------------------------------
	// Set all drivers to skip the loading of all sounds in order to
	//	speed loading when sounds aren't needed.
	//----------------------------------------------------------------------------
	static void SetSkipSounds(bool i_bSkip);

	//--------------------------------------------------------------------
	// Return string for description of driver (include its current state)
	// to display in the gui when the mouse hovers over the clip.
	//--------------------------------------------------------------------
	std::string GetHoverDescription();

	//--------------------------------------------------------------------
	//	relative path (from project root) of place to look for sound
	//--------------------------------------------------------------------
	inline const fsLocator& GetSoundDir();

	//--------------------------------------------------------------------
	//	relative path (from project root) of place to look for sound
	//--------------------------------------------------------------------
	inline void SetSoundDir(const fsLocator& i_SoundDir);

	//--------------------------------------------------------------------
	//  GetDriverInfo - return data structure representing state of
	//		this driver suitable for writing to a file.
	//	The returned value should be created with "new" and will
	//		be deleted by the caller.
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo*  GetDriverInfo() const;

	//--------------------------------------------------------------------
	// Set internal variables from data structure
	//--------------------------------------------------------------------
	void SetDriverInfo(	const tmlnDriverSoundInfo& i_Info );

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(const maTime& i_Time);

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	float GetSoundStartTime() const;
	void SetSoundStartTime(float i_fStartTime);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	float GetSoundEndTime() const;
	void SetSoundEndTime(float i_fEndTime);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	bool GetSetToSoundLength() const;
	void SetSetToSoundLength(bool i_bSetToLength);

protected:
	//--------------------------------------------------------------------
	//	PerformSplit - Split up the details of this driver between itself
	//	and the driver passed in.
	//--------------------------------------------------------------------
	virtual void PerformSplit( tmlnDriver* io_pDriverAtEnd, const maTime& i_fTime );

private:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	maTime calculate_endtime();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	maTime calculate_duration();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void load_sound(const tmlnDriverSoundInfo& i_Info);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SoundFileChanged(prtyProperty *i_pProperty, bool i_bDirty);

private:
	tmlnAdapterGetPosition &m_Adapter;
	snSoundJob2D* m_pPlayingSound;

	fsLocator		m_SoundSearchDir;
	prtyFilePath	m_SoundName;
	prtyFloat		m_fSoundStartTime;
	prtyFloat		m_fSoundEndTime;
	prtyBoolean		m_bSetToSoundLength;

	maTime	m_LastOperateTime;
	int		m_PauseCounter;
	bool	m_bStarted;
	bool	m_bNeedsLoad;
	maRunningAverage m_AvgFrameRate;

	static bool sm_bSkipSounds;
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline const fsLocator& tmlnDriverSound::GetSoundDir()
{
	return m_SoundSearchDir;
}
//--------------------------------------------------------------------
//	relative path (from project root) of place to look for sound
//--------------------------------------------------------------------
inline void tmlnDriverSound::SetSoundDir(const fsLocator& i_SoundDir)
{
	m_SoundSearchDir =i_SoundDir;
}

