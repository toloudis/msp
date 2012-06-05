/****************************************************************************\
**  snSoundJobMP3PACDShow.cpp
**
**	  snSoundJobMP3PACDShow.cpp implements the DirectSound portion of the
**	snSoundJobMP3PAC.
**
**	NOTE:  This is a HACKED version of an MP3 player.  DirectSound doesn't
**	directly support MP3s so this code uses DirectShow.  It requires the
**	libs: strmiids.lib ole32.lib
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "AudioDS/sn/private/snSoundJobMP3PACDShow.hpp"

#include "AudioDS/sn/snSoundUtil.hpp"
#include "Core/fs/private/fsFileUtilPAC.hpp"
#include "Core/gf/gfFileTranslationMgr.hpp"

#include <control.h>
#include <dshow.h> 
#include <malloc.h>


//------------------------------------------------------------------------------
//	library pragmas
//------------------------------------------------------------------------------
#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"strmiids.lib")
#pragma comment(lib,"winmm.lib")


//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

namespace
{
// DirectShow Graph, Filter & Pins used
IGraphBuilder *	g_pGraphBuilder		= NULL;
IMediaControl *	g_pMediaControl		= NULL;
IMediaSeeking *	g_pMediaSeeking		= NULL;
IBaseFilter   *	g_pSourceCurrent	= NULL;
IBaseFilter   *	g_pSourceNext		= NULL;
IBasicAudio	  * g_pBasicAudio		= NULL;
IMediaEventEx *	g_pEvent			= NULL;
}


//========================================================================
//	default and copy constructors
//========================================================================
snSoundJobMP3PAC::snSoundJobMP3PAC()
{
	Init();
}


//========================================================================
//========================================================================
snSoundJobMP3PAC::snSoundJobMP3PAC( const snSoundJobMP3PAC& i_CopyFrom )
{
	Init();

	*this = i_CopyFrom;
}


//========================================================================
//========================================================================
snSoundJobMP3PAC::~snSoundJobMP3PAC()
{
	Free();
}


//========================================================================
//	Load()
//========================================================================
void	
snSoundJobMP3PAC::Load()
{
	if ( !IsLoaded() )
	{
		HRESULT hr;

		// Initialize COM
		//
		if (FAILED (hr = CoInitialize(NULL)) )
		{
			DBG_ASSERT( 0, "Failed" );
			return;
		}

		// Create DirectShow Graph
		//
		if (FAILED (hr = CoCreateInstance(CLSID_FilterGraph, NULL,
										  CLSCTX_INPROC, IID_IGraphBuilder,
										  reinterpret_cast<void **>(&g_pGraphBuilder))) )
		{
			DBG_ASSERT( 0, "Failed" );
			return;
		}

		// Get the IMediaControl Interface
		//
		if (FAILED (g_pGraphBuilder->QueryInterface(IID_IMediaControl,
									 reinterpret_cast<void **>(&g_pMediaControl))))
		{
			DBG_ASSERT( 0, "Failed" );
			return;
		}

		// Get the IMediaControl Interface
		//
		if (FAILED (g_pGraphBuilder->QueryInterface(IID_IMediaSeeking,
									 reinterpret_cast<void **>(&g_pMediaSeeking))))
		{
			DBG_ASSERT( 0, "Failed" );
			return;
		}

		// Set the owner window to receive event notices.
		//
		g_pGraphBuilder->QueryInterface(IID_IMediaEvent, (void **)&g_pEvent);
		//g_pEvent->SetNotifyWindow((OAHWND)appApplicationPAC::GetHWND(), WM_GRAPHNOTIFY, 0);

		SetLoaded( true );
	}
}


//========================================================================
//	Free()
//========================================================================
void	
snSoundJobMP3PAC::Free()
{
	if ( IsLoaded() )
	{
		SetLoaded( false );
	}

	// Stop playback
	//
	if (g_pMediaControl)
		g_pMediaControl->Stop();

	// Release all remaining pointers
	//
	if ( g_pSourceNext )
	{
		g_pSourceNext->Release();
		g_pSourceNext = NULL;
	}

	if ( g_pSourceCurrent )
	{
		g_pSourceCurrent->Release();
		g_pSourceCurrent = NULL;
	}

	if ( g_pMediaSeeking )
	{
		g_pMediaSeeking->Release();
		g_pMediaSeeking = NULL;
	}

	if ( g_pBasicAudio )
	{
		g_pBasicAudio->Release();
		g_pBasicAudio = NULL;
	}

	if ( g_pMediaControl )
	{
		g_pMediaControl->Release();
		g_pMediaControl = NULL;
	}

	if ( g_pGraphBuilder )
	{
		g_pGraphBuilder->Release();
		g_pGraphBuilder = NULL;
	}

	if ( g_pEvent )
	{
		g_pEvent->Release();
		g_pEvent = NULL;
	}

	CoUninitialize();
}


//========================================================================
//	get all the pins for a filter and return a good one for the
//	given direction (input or output)
//
//========================================================================
IPin *
GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir)
{
	BOOL	   bFound = FALSE;
	IEnumPins  *pEnum;
	IPin	   *pPin;

	pFilter->EnumPins(&pEnum);
	while(pEnum->Next(1, &pPin, 0) == S_OK)
	{
		PIN_DIRECTION PinDirThis;
		pPin->QueryDirection(&PinDirThis);
		if (bFound = (PinDir == PinDirThis))
			break;
		pPin->Release();
	}
	pEnum->Release();
	return (bFound ? pPin : 0);  
}


//========================================================================
//	Start()
//========================================================================
void	
snSoundJobMP3PAC::Start( float i_SimulationTime )
{
	itString	UnicodeFilename;
	HRESULT		hr		= S_OK;
	IPin *		pPin	= NULL;

	fsLocator ExpandedLoc(this->GetFilename());
	gfFileTranslationMgr::ExpandLocator(ExpandedLoc);
	fsFileUtilPAC::LocatorToUnicodeFilename( ExpandedLoc, UnicodeFilename );
	// null-terminating not needed anymore
	//UnicodeFilename += 0;

	if ( fsFileUtilPAC::MustUseANSIFilenames() )
	{
		// Make sure that this file exists
		//
		std::string filename;
		fsFileUtilPAC::LocatorToANSIFilename( ExpandedLoc, filename );
		if ( filename.size() == 0 )
		{
			return;
		}

		DWORD dwAttr = ::GetFileAttributesA(filename.c_str());
		if (dwAttr == (DWORD) -1)
		{
			return;
		} 
	}
	else
	{
		// Make sure that this file exists
		//
		DWORD dwAttr = GetFileAttributesW(UnicodeFilename.GetString());
		if (dwAttr == (DWORD) -1)
		{
			return;
		}
	}

	// OPTIMIZATION OPPORTUNITY
	// This will open the file, which is expensive. To optimize, this
	// should be done earlier, ideally as soon as we knew this was the
	// next file to ensure that the file load doesn't add to the
	// filter swapping time & cause a hiccup.
	//
	// Add the new source filter to the graph. (Graph can still be running)
	//
	hr = g_pGraphBuilder->AddSourceFilter(UnicodeFilename.GetString(), UnicodeFilename.GetString(), &g_pSourceNext);

	// Get the first output pin of the new source filter. Audio sources 
	// typically have only one output pin, so for most audio cases finding 
	// any output pin is sufficient.
	//
	if (SUCCEEDED(hr)) 
	{
		//	Get an output pin
		//
		pPin = ::GetPin( g_pSourceNext, PINDIR_OUTPUT );

		DBG_ASSERT( pPin != NULL, "No output PINS for this computer" );
		if ( pPin == NULL )
		{
			Stop();
			return;
		}
	}
	else
	{
		DBG_ASSERT( 0, "Failed" );
		return;
	}

	// Stop the graph
	//
	if (SUCCEEDED(hr)) 
	{
		hr = g_pMediaControl->Stop();
	}
	else
	{
		DBG_ASSERT( 0, "Failed" );
		return;
	}

	// Break all connections on the filters. You can do this by adding 
	// and removing each filter in the graph
	//
	if (SUCCEEDED(hr)) 
	{
		IEnumFilters *pFilterEnum = NULL;
		IBaseFilter  *pFilterTemp = NULL;

		if (SUCCEEDED(hr = g_pGraphBuilder->EnumFilters(&pFilterEnum))) 
		{
			int iFiltCount	= 0;
			int iPos		= 0;

			// Need to know how many filters. If we add/remove filters during the
			// enumeration we'll invalidate the enumerator
			//
			while (S_OK == pFilterEnum->Skip(1)) 
			{
				iFiltCount++;
			}

			// Allocate space, then pull out all of the filters
			//
			IBaseFilter **ppFilters = reinterpret_cast<IBaseFilter **>
									  (_alloca(sizeof(IBaseFilter *) * iFiltCount));
			pFilterEnum->Reset();

			while (S_OK == pFilterEnum->Next(1, &(ppFilters[iPos++]), NULL));
			SAFE_RELEASE(pFilterEnum);

			for (iPos = 0; iPos < iFiltCount; iPos++) 
			{
				g_pGraphBuilder->RemoveFilter(ppFilters[iPos]);
				
				// Put the filter back, unless it is the old source
				//
				if (ppFilters[iPos] != g_pSourceCurrent) 
				{
					g_pGraphBuilder->AddFilter(ppFilters[iPos], NULL);

					// property pages
					//
					//SupportsPropertyPage( ppFilters[iPos] );
				}
				SAFE_RELEASE(ppFilters[iPos]);
			}
		}
		else
		{
			DBG_ASSERT( 0, "Failed" );
			return;
		}
	}
	else
	{
		DBG_ASSERT( 0, "Failed" );
		return;
	}

	// We have the new ouput pin. Render it
	//
	if (SUCCEEDED(hr)) 
	{
		hr = g_pGraphBuilder->Render(pPin);
		g_pSourceCurrent = g_pSourceNext;
		g_pSourceNext = NULL;
	}

	SAFE_RELEASE(pPin);
	SAFE_RELEASE(g_pSourceNext); // In case of errors

	// Re-seek the graph to the beginning
	//
	if (SUCCEEDED(hr)) 
	{
		LONGLONG llPos = 0;
		hr = g_pMediaSeeking->SetPositions(&llPos, AM_SEEKING_AbsolutePositioning,
										   &llPos, AM_SEEKING_NoPositioning);
	} 
	else
	{
		DBG_ASSERT( 0, "Failed" );
		return;
	}

	// Start the graph
	//
	if (SUCCEEDED(hr)) 
	{
		hr = g_pMediaControl->Run();
	}
	else
	{
		DBG_ASSERT( 0, "Failed" );
		return;
	}

	this->SetFinished( false );

	if ( 0 == m_LoopCurrent )
	{
		m_LoopCurrent	= m_Loops;
	}

	// Release the old source filter.
	//
	SAFE_RELEASE(g_pSourceCurrent)

	return;
}


//========================================================================
//	Stop()
//========================================================================
void	
snSoundJobMP3PAC::Stop()
{
	if ( IsLoaded() )
	{
		HRESULT hr;
		hr = g_pMediaControl->Stop();
	}

	this->SetFinished( true );
}


//========================================================================
//	HandleEvent()
//========================================================================
void	
snSoundJobMP3PAC::HandleEvent( float i_SimulationTime ) 
{
	long evCode;
	LONG_PTR param1, param2;
	HRESULT hr;

	if ( !g_pEvent ) return;

	//	Check if finished
	//
	//if ( (m_LastStateCheck + 200.0) < i_SimulationTime )
	{
		m_LastStateCheck	= i_SimulationTime;

		while (hr = g_pEvent->GetEvent(&evCode, &param1, &param2, 0), SUCCEEDED(hr))
		{ 
			hr = g_pEvent->FreeEventParams(evCode, param1, param2);
			if ((EC_COMPLETE == evCode) || (EC_USERABORT == evCode))
			{
				//HRESULT			result;
				//OAFilterState	state = 1;
				//
				//result = g_pMediaControl->GetState(50, &state );
				//if (state == 0)
				{
					//	The MP3 is done playing.  Determine if it should
					//	start again or finish.
					//
					//DBG_LOG( "LoopCurrent = " << m_LoopCurrent );

					if ( m_LoopCurrent > 0 )
					{
						m_LoopCurrent--;
					}
 
					if ( m_LoopCurrent == 0 )
					{
						//DBG_LOG( "---Finished" );
						this->SetFinished( true );
						this->Stop();
					}
					else
					{
						//DBG_LOG( "---Restart" );
						this->Start( i_SimulationTime );
					}
				}
				//else
				//{
				//	if ( result == VFW_S_STATE_INTERMEDIATE )
				//	{
				//		state = 0;
				//	}
				//	else
				//	if ( result == VFW_S_CANT_CUE )
				//	{
				//		state = 0;
				//	}
				//}
				break;
			} 
		} 
	}
}


//========================================================================
//	Think()
//========================================================================
void	
snSoundJobMP3PAC::Think( float i_SimulationTime )
{
	if ( IsFinished() )
	{
		return;
	}

	if ( !IsLoaded() )
	{
		return;
	}

	//	Adjust the volume
	//
	if ( this->IsVolumeChanged() )
	{
		SetVolume( GetVolume() );
	}

	//	Do the effects
	//
	ThinkEffects( i_SimulationTime );

	//	check for finishing the sound
	//
	HandleEvent( i_SimulationTime );
}


//========================================================================
//	ThinkEffects()
//
//	Take care of a sound's effects that is active each game loop.
//
//		1. do FadeChanging (if applicable)
//		2. do PanChanging (if applicable)
//		3. do FrequencyChanging (if applicable)
//		4. do other effects (if applicable)
//========================================================================
void	
snSoundJobMP3PAC::ThinkEffects( float i_SimulationTime )
{
	// update the volume with the result of a linterp of time to volume
	//
	if ( IsFadeChanging() )
	{
		if (   ( m_VolumeFactor == m_FadeFactor )
			|| ( ( m_FadeStartTime + m_FadeTime ) < i_SimulationTime ) )
		{
			// This sound is done.
			//
			SetFadeChanging( false );
			m_VolumeFactor	= m_FadeFactor;

			//	Officially set the volume
			//
			SetVolume( GetVolume() );

			//	if flag is set, Stop the sound
			//
			//if (   ( IsStopOnFadeFinish() )
			//	|| ( IsFreeOnFadeFinish() ) 
			//	)
			//{
				//	if flag is set, Free the sound
				//
				//if ( IsFreeOnFadeFinish() )
				//{
				//	SetDeleteWhenFinished( true );
				//}
			
			//	Stop();
			//	return;
			//}
		}
		else
		{
			//	calculate the elapsed time so far
			//
			float elapsed = i_SimulationTime - m_FadeStartTime;

			//	Set the volume factor based on this time
			//
			float timeRatio;
			
			if ( m_FadeTime == 0.0f )
			{
				timeRatio = 1.0f;
			}
			else
			{
				timeRatio = ( elapsed / m_FadeTime );
			}

			m_VolumeFactor = m_FadeStartFactor 
				+ ( (m_FadeFactor - m_FadeStartFactor) * timeRatio );

			//	Officially set the volume
			//
			SetVolume( GetVolume() );
		}
	}
}


//========================================================================
//	Pause()
//========================================================================
void	
snSoundJobMP3PAC::Pause()
{
	if ( IsLoaded() )
	{
		HRESULT hr;
		hr = g_pMediaControl->Pause();
	}
}


//========================================================================
//	Resume()
//========================================================================
void	
snSoundJobMP3PAC::Resume( float i_SimulationTime )
{
	if ( IsLoaded() )
	{
		HRESULT hr;
		hr = g_pMediaControl->Run();
	}
}


//========================================================================
//	SetVolume()
//
//	The volume can range from 0.0 (silent) to 1.0 (full-on)
//========================================================================
void	
snSoundJobMP3PAC::SetVolume( const float i_NewVolume )
{
	//	if the volume requested is within range, change the volume.
	//	if not, we need to still re-calculate the volume because some of the factors
	//	may have changed.
	//
	if ( ( i_NewVolume >= 0.0f ) && ( i_NewVolume <= 1.0f ) )
	{
		m_Volume = i_NewVolume;
	}

	// scale volume according to fade in/ fade out
	//
	float ScaledVolume = GetVolumeFinal();

	//	actually set the volume
	//
	HRESULT result;
	result = g_pGraphBuilder->QueryInterface( IID_IBasicAudio, 
									 reinterpret_cast<void **>(&g_pBasicAudio));
	if ( g_pBasicAudio )
	{
		long newVolume;
		newVolume = snSoundUtil::ConvertVolume( ScaledVolume );
		g_pBasicAudio->put_Volume( newVolume );
	}

	//	Once applied, we can reset this
	//
	SetVolumeChanged( false );
}


//========================================================================
//	GetVolume()
//
//	return: The volume can range from 0.0 (silent) to 1.0 (full-on)
//========================================================================
float
snSoundJobMP3PAC::GetVolume() const
{
	return m_Volume;
}


//========================================================================
//	SetVolumeFactor()
//
//	The volume factor (percentage) can range from 0.0 (0%) to 1.0 (100 %)
//========================================================================
void	
snSoundJobMP3PAC::SetVolumeFactor( const float i_Value )
{
	DBG_ASSERT( ((i_Value >= 0.0f) && (i_Value <= 1.0f)), "Value out of range - " << i_Value );

	m_VolumeFactor	= i_Value;

	if ( m_VolumeFactor < 0.0f )
		m_VolumeFactor = 0.0f;

	if ( m_VolumeFactor > 1.0f )
		m_VolumeFactor = 1.0f;

	//	Mark the volume as changed so we can update in the Think()
	//
	SetVolumeChanged( true );
}


//========================================================================
//	GetVolumeFactor()
//
//	return: The volume factor can range from 0.0 (0%) to 1.0 (100 %)
//========================================================================
float		
snSoundJobMP3PAC::GetVolumeFactor() const
{
	return m_VolumeFactor;
}


//========================================================================
//	SetVolumeTypeFactor()
//
//	The volume factor (percentage) can range from 0.0 (0%) to 1.0 (100 %)
//========================================================================
void	
snSoundJobMP3PAC::SetVolumeTypeFactor( const float i_Value )
{
	DBG_ASSERT( ((i_Value >= 0.0f) && (i_Value <= 1.0f)), "Value out of range - " << i_Value );

	m_VolumeTypeFactor	= i_Value;

	if ( m_VolumeTypeFactor < 0.0f )
		m_VolumeTypeFactor = 0.0f;

	if ( m_VolumeTypeFactor > 1.0f )
		m_VolumeTypeFactor = 1.0f;

	//	Mark the volume as changed so we can update in the Think()
	//
	SetVolumeChanged( true );
}


//========================================================================
//	GetVolumeTypeFactor()
//
//	return: The volume factor can range from 0.0 (0%) to 1.0 (100 %)
//========================================================================
float		
snSoundJobMP3PAC::GetVolumeTypeFactor() const
{
	return m_VolumeTypeFactor;
}


//========================================================================
//	SetVolumeGlobalFactor()
//
//	The volume factor (percentage) can range from 0.0 (0%) to 1.0 (100 %)
//========================================================================
void	
snSoundJobMP3PAC::SetVolumeGlobalFactor( const float i_Value )
{
	DBG_ASSERT( ((i_Value >= 0.0f) && (i_Value <= 1.0f)), "Value out of range - " << i_Value );

	m_VolumeGlobalFactor	= i_Value;

	if ( m_VolumeGlobalFactor < 0.0f )
		m_VolumeGlobalFactor = 0.0f;

	if ( m_VolumeGlobalFactor > 1.0f )
		m_VolumeGlobalFactor = 1.0f;

	//	Mark the volume as changed so we can update in the Think()
	//
	SetVolumeChanged( true );
}


//========================================================================
//	GetVolumeGlobalFactor()
//
//	return: The volume factor can range from 0.0 (0%) to 1.0 (100 %)
//========================================================================
float		
snSoundJobMP3PAC::GetVolumeGlobalFactor() const
{
	return m_VolumeGlobalFactor;
}


//========================================================================
//	GetVolumeFinal()
//
//	The final volume value based on the base volume and all factors.
//========================================================================
float		
snSoundJobMP3PAC::GetVolumeFinal() const
{
	float ScaledVolume;

	ScaledVolume = m_Volume * GetVolumeFactor();

	// scale the volume according to the current effects or music volume settings
	//
	//ScaledVolume *= m_VolumeLocalizeFactor;
	ScaledVolume *= GetVolumeTypeFactor();
	ScaledVolume *= GetVolumeGlobalFactor();

	return ScaledVolume;
}


//========================================================================
//	SetFadeTo()
//
//		Fade the sound to a specific level.  This level is a percentage
//	from 0.0 to 1.0.
//========================================================================
void 
snSoundJobMP3PAC::SetFadeTo( float i_TargetLevel, float i_Seconds, float i_SimulationTime )
{
	DBG_ASSERT( ((i_TargetLevel >= 0.0f) && (i_TargetLevel <= 1.0f)), "Target Level out of range - " << i_TargetLevel );

	if ( i_TargetLevel < 0.0f ) i_TargetLevel = 0.0f;
	if ( i_TargetLevel > 1.0f ) i_TargetLevel = 1.0f;

	SetFadeChanging( true );

	m_FadeStartFactor	= m_VolumeFactor;
	m_FadeFactor		= i_TargetLevel;
	m_FadeTime			= i_Seconds;
	m_FadeStartTime		= i_SimulationTime;
}


//========================================================================
//	AddToFade()
//
//		Add a value to the Fade.  This value cannot exceed the Fade
//	percentage from 0.0 to 1.0.
//========================================================================
void 
snSoundJobMP3PAC::AddToFade( float i_TargetLevel, float i_Seconds, float i_SimulationTime )
{
	DBG_ASSERT( ((i_TargetLevel >= 0.0f) && (i_TargetLevel <= 1.0f)), "Target Level out of range - " << i_TargetLevel );

	SetFadeChanging( true );

	m_FadeStartFactor	= m_VolumeFactor;
	m_FadeFactor		= m_FadeFactor + i_TargetLevel;
	if ( m_FadeFactor > 1.0f ) m_FadeFactor = 1.0f;
	if ( m_FadeFactor < 0.0f ) m_FadeFactor = 0.0f;
	m_FadeTime			= i_Seconds;
	m_FadeStartTime		= i_SimulationTime;
}


//========================================================================
//	SetFadeFactor()
//
//	this factor ranges from 0.0 (silence) to 1.0 (full). 
//========================================================================
void	
snSoundJobMP3PAC::SetFadeFactor( float Value )
{
	m_FadeFactor = Value;
}


//========================================================================
//	GetFadeFactor()
//
//	return: this factor ranges from 0.0 (silence) to 1.0 (full). 
//========================================================================
float
snSoundJobMP3PAC::GetFadeFactor() const
{
	return m_FadeFactor;
}


//========================================================================
//	SetLoops()
//========================================================================
void		
snSoundJobMP3PAC::SetLoops( const int i_Value )
{
	m_Loops = i_Value;
	m_LoopCurrent = i_Value;

	if (   ( i_Value == 0 )
		|| ( i_Value == 1 ) )
	{
		SetLooping( false );
	}
	else
	{
		SetLooping( true );
	}
}


//========================================================================
//	GetLoops()
//========================================================================
int			
snSoundJobMP3PAC::GetLoops() const
{
	return m_Loops;
}


//========================================================================
//	Init()
//
//	Initialize this classes data
//========================================================================
void	
snSoundJobMP3PAC::Init()
{
	SetLoaded( false );
	SetPaused( false );
	SetLooping( false );
	SetFinished( false );
	SetDeleteWhenFinished( false );
	SetFadeChanging( false );
	SetVolumeChanged( false );

	m_Volume				= 1.0f;
	m_VolumeFactor			= 1.0f;
	//m_VolumeLocalizeFactor	= 1.0f;
	m_VolumeGlobalFactor	= 1.0f;
	m_VolumeTypeFactor		= 1.0f;

	m_FadeFactor			= 1.0f;
	m_FadeStartFactor		= 0;
	m_FadeTime				= 0;
	m_FadeStartTime			= 0;

	m_LoopCurrent			= 0;

	m_LastStateCheck		= 0;
}

