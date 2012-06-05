/****************************************************************************\
**  snSoundJob2DStreamedPACDSound.hpp
**
**      snSoundJob2DStreamedPACDSound.hpp implements the windows portion of the
**	snSoundJob2DStreamedPAC.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef SN_SOUNDJOB2DSTREAMEDPACDSOUND_HPP
#error snSoundJob2DStreamedPACDSound.hpp multiply included
#endif
#define SN_SOUNDJOB2DSTREAMEDPACDSOUND_HPP

#ifndef SN_SOUNDJOB2DPACDSOUND_HPP
#include "AudioDS/sn/private/snSoundJob2DPACDSound.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include <fsLocator.hpp>
#endif



//================================================================================
//	Forward references
//================================================================================
class gfFileBin;


//================================================================================
//================================================================================
class snSoundJob2DStreamedPAC : public snSoundJob2DPAC
{
	public:
	//----------------------------------------------------------------------------
	//	Constructors and Destructors
	//----------------------------------------------------------------------------

		//========================================================================
		//	default and copy constructors
		//========================================================================
		snSoundJob2DStreamedPAC();
		snSoundJob2DStreamedPAC( const snSoundJob2DStreamedPAC& i_CopyFrom );

		//========================================================================
		//========================================================================
		~snSoundJob2DStreamedPAC();


	//----------------------------------------------------------------------------
	//	Functions
	//----------------------------------------------------------------------------

		//========================================================================
		//	Start()
		//
		//	Start playing a sound
		//
		//		1. make sure everything's ready
		//		2. Set a flag to signal this sound CAN be started
		//		3. the Think() function takes care of it from there
		//========================================================================
		virtual void	Start( float i_SimulationTime );

		//========================================================================
		//	Restart()
		//
		//	Restart playing a sound
		//========================================================================
		virtual void	Restart( float i_SimulationTime );

		//========================================================================
		//	Think()
		//
		//	Take care of a sound that is active each game loop.
		//
		//		1. check if it is finished or not (it may be on a sound list
		//		   somewhere)
		//		2. check if everything is set up correctly
		//		3. check on the status
		//		4. based on the status do things
		//		5. check to see if we have a time limit
		//		6. do FadeChanging (if applicable)
		//		7. do PanChanging (if applicable)
		//		8. do FrequencyChanging (if applicable)
		//		9. do other effects (if applicable)
		//========================================================================
		virtual void	Think( float i_SimulationTime );

		//========================================================================
		//	Pause()
		//========================================================================
		virtual void	Pause();

		//========================================================================
		//	Resume()
		//========================================================================
		virtual void	Resume( float i_SimulationTime );

		//========================================================================
		//	Reload()
		//
		//		Reload the raw sound data
		//========================================================================
		virtual void	Reload();

		//========================================================================
		//	Unload()
		//
		//		Pause the sound and unload the raw sound data
		//========================================================================
		virtual void	Unload();


	private:
		//========================================================================
		//	CreateBuffer()
		//========================================================================
		void	CreateBuffer();

		//========================================================================
		//	FillBuffer()
		//
		//	Fill the sound buffer from the file stream.  Read in the specified
		//	number of bytes.
		//========================================================================
		void	FillBuffer( unsigned int i_BytesToRead );

		//========================================================================
		//	PlayBuffer()
		//
		//	Start the buffer playing.
		//========================================================================
		void	PlayBuffer( float i_SimulationTime );

		//========================================================================
		//	Init()
		//========================================================================
		void	Init();

	//----------------------------------------------------------------------------
	//	Flag Accessor functions
	//----------------------------------------------------------------------------

		//========================================================================
		//	DoneReading
		//========================================================================
		bool	IsDoneReading() const					{ return m_Flag.bDoneReading; };
		void	SetDoneReading( const bool Value )		{ m_Flag.bDoneReading = Value; };

		//========================================================================
		//	FoundEnd
		//========================================================================
		bool	IsFoundEnd() const						{ return m_Flag.bFoundEnd; };
		void	SetFoundEnd( const bool Value )			{ m_Flag.bFoundEnd = Value; };

		//========================================================================
		//	FileChanged
		//========================================================================
		bool	IsFileChanged() const					{ return m_Flag.bFileChanged; };
		void	SetFileChanged( const bool Value )		{ m_Flag.bFileChanged = Value; };


		//========================================================================
		//========================================================================
		struct
		{
			bool	bDoneReading		:1;
			bool	bFoundEnd			:1;
			bool	bFileChanged		:1;		// the filename has been set to another file
			bool	bAlmostDone			:1;
		} m_Flag;

		//	count of the number of times this stream has looped
		//  resets to 0 on calls to SetLoops
		int		m_LoopCount;

		// the number of times this stream should loop before stopping
		//
		int		m_LoopTarget;

		// offset into directsound buffer where last write ended?
		//
		int		m_LastRead;		

		// offset of the last valid, non-silence data written into
		// the buffer...needed so we can tell when we're done playing
		// that data and its o.k. to stop the sound
		int		m_EndOfStream;

		// a running total of the amount of data read from the file and
		// written to the buffer...needed so we can tell when it's time
		// to go back to the beginning of the file for looping sounds
		// and so we can tell when looping sounds are near the end of a loop
		int		m_BytesWritten;
		
		float	m_StreamUpdateLast;			// the interval, in milliseconds, since our last buffer update
		float	m_StreamUpdateRate;			// the time, in milliseconds, of an update
		float	m_StreamUpdateBytes;		// the size (in bytes) of an update

		float	m_StreamBufferInSeconds;			// how many seconds the stream buffer holds
		int		m_NumberOfPlayNotifications;

		unsigned long	m_dwBufferSize;			// Size of the entire buffer 
		unsigned long	m_dwNotifySize;			// size of each notification period.
		unsigned long	m_dwNextWriteOffset;	// Offset to next buffer segment 
		unsigned long	m_dwProgress;			// Used with above to show prog. 
		unsigned long	m_dwLastPlayPosInBuffer;	// the last play position returned by GetCurrentPos().
		unsigned long	m_dwCurrentPlayPosInBuffer;	// the current play position returned by GetCurrentPos().
		unsigned long	m_dwFinalWritePosInBuffer;	// end of sound data

		// This structure keeps all the data that the TimeFunc callback uses in one
		// place.  In this implementation, that means the global data segement.  This
		// is setup so that if you wanted to put your callback in a DLL, all you'd need
		// to do is pass the address of this structure as a parameter.
		//
		typedef struct waveinfoca_tag
		{
			WAVEFORMATEX         *pwfx;				// Wave Format data structure
			HMMIO                hmmio;				// MM I/O handle for the WAVE
			MMCKINFO             mmck;				// Multimedia RIFF chunk 
			MMCKINFO             mmckInRIFF;	    // Use in opening a WAVE file 
		} WAVEINFOCA, *LPWAVEINFOCA;

		WAVEINFOCA				wiWave;
};


//------------------------------------------------------------------------
//	In-line functions
//------------------------------------------------------------------------


//========================================================================
//========================================================================
