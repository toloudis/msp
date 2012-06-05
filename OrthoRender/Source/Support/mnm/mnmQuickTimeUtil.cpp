/****************************************************************************\
**  mnmQuickTimeUtil.cpp
**
**      see .hpp
**
**  URL about Linking against qtmlclient.lib on Windows
**  http://lists.apple.com/archives/QuickTime-API/2003/Nov/msg00139.html
**
**	An Introduction to Quicktime
**	http://developer.apple.com/quicktime/qttutorial/movies.html
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/mnm/mnmQuickTimeUtil.hpp"

#include "Core/app/appTimeUtils.hpp"
#include "Core/ch/chDefs.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Graphics/g2d/g2dImage.hpp"
#include "Graphics/g2d/g2dScreenCaptureUtil.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Support/mnm/mnmQuickTimeSound.hpp"

#include <qtml.h>
#include <movies.h>
#include <tchar.h>
#include <Gestalt.h>
#include <wtypes.h>
#include <FixMath.h>
#include <Script.h>
#include <stdlib.h>
#include <TextUtils.h>
#include <NumberFormatting.h>

#include <ImageCompression.h>		// qt

#define USE_QUICKTIME_TIMECODE
#ifdef USE_QUICKTIME_TIMECODE
#include <QuickTimeComponents.h>
#endif


//============================================================================
//============================================================================
#define		kVideoTimeScale 	600
#define		kNoOffset 			0
#define		kMgrChoose			0
#define		kSyncSample 		0
#define		kAddOneVideoSample	1
#define		kTrackStart			0
#define		kMediaStart			0
#define		kTimeCodeTrackSize	(20 << 16)				// initial height of timecode track
#define		kNoVolume			0



//============================================================================
//	CQTMovieFile 
//
//		code taken from quicktime sample app.
//
//	TODO: clean up the code and remove GOTOs.
//============================================================================
class CQTMovieFile
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	CQTMovieFile();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~CQTMovieFile(void);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Close(void);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	HRESULT InitGraphicsWorld(HDC hBackDC,HBITMAP hBackBitmap,LPCTSTR lpszFileName, g2dWindow* i_pWindow);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	HRESULT	AppendNewFrame(float i_fCurrentTime, g2dImage* i_pSurface);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void AppendTimeCodeFrame(float i_fStartTime, float i_fEndTime);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetCodec(const std::string& i_CodeType);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetFramesPerSecond(const float i_FramesPerSecond);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	Movie GetMovie();

private:
	CGrafPtr	m_savedPort;
	GDHandle	m_savedGD;
	GWorldPtr	m_gWorld;
	Track		m_pVideoTrack;
	Media		m_pVideoMedia;
	Track		m_pTypeTrack;
	Track		m_pTimeCodeTrack;
	Media		m_pTimeCodeMedia;
	MediaHandler m_pTimeCodeMediaHandler;
	Movie		m_pMovie;
	Str255		m_FileName;
	short		m_resRefNum;
	Rect		m_trackFrame;
	Handle		m_compressedData;
	Ptr			m_compressedDataPtr;
	ImageDescriptionHandle m_imageDesc;

	int		m_CodecType;
	int		m_CodecQuality;
	float	m_fFramesPerSecond;
	float	m_fCurrentTime;
	float	m_fLastTime;

	TCHAR	m_szErrMsg[256];

	g2dWindow* m_pWindow;
	HDC		m_hBackDC;
	HBITMAP	m_hBackBitmap;
	int		nAppendFuncSelector;		//0=Dummy	1=FirstTime	2=Usual

	void	ReleaseMemory();
	HRESULT	AppendFrameFirstTime(g2dImage* i_pSurface);
	HRESULT	AppendFrameUsual(g2dImage* i_pSurface);
	HRESULT	AppendDummy(g2dImage* i_pSurface);
	HRESULT	(CQTMovieFile::*pAppendFrame[3])(g2dImage* i_pSurface);

#ifdef USE_QUICKTIME_TIMECODE
	OSErr QTTC_AddTimeCodeToMovie(Movie theMovie, Track i_pTypeTrack, float i_fStartTime, float i_fEndTime);
#endif
};


//#define codecMinQuality         0x000L      /* minimum valid value */
//#define codecLowQuality         0x100L      /* low-quality reproduction */
//#define codecNormalQuality      0x200L      /* normal-quality repro */ 
//#define codecHighQuality        0x300L      /* high-quality repro */ 
//#define codecMaxQuality         0x3FFL      /* maximum-quality repro */
//#define codecLosslessQuality  

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CQTMovieFile::CQTMovieFile() 
:	m_CodecType(kJPEGCodecType),
	m_CodecQuality(codecNormalQuality),
	m_fFramesPerSecond(g3dConstants::c_fDefaultFrameRate),
	m_fLastTime(-1.0f),
	m_fCurrentTime(0.0f)
{
	m_pMovie = NULL;
	m_pVideoTrack = NULL;
	m_pVideoMedia = NULL;
	m_resRefNum = 0;
	m_gWorld = NULL;
	m_savedGD = NULL;	
	m_savedPort = NULL;
	m_imageDesc = NULL;
	m_compressedData = NULL;;
	m_compressedDataPtr = NULL;
	m_pTimeCodeTrack = NULL;
	m_pTimeCodeMedia = NULL;
	m_pTimeCodeMediaHandler = NULL;

	pAppendFrame[0] = &CQTMovieFile::AppendDummy;
	pAppendFrame[1] = &CQTMovieFile::AppendFrameFirstTime;
	pAppendFrame[2] = &CQTMovieFile::AppendFrameUsual;
	nAppendFuncSelector=0;		//Point to Dummy Function

	if (InitializeQTML(0) != noErr)
	{
		TerminateQTML();
		MessageBox(NULL,_T("Unable to Initialize Quick Time Environment"),_T("Error"),MB_OK|MB_ICONERROR);
		return;
	}

	EnterMovies();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
HRESULT CQTMovieFile::InitGraphicsWorld( HDC hBackDC, HBITMAP hBackBitmap, LPCTSTR lpszFileName, g2dWindow* i_pWindow )
{
	m_pWindow = i_pWindow;

	OSErr		err = noErr;
	FSSpec		fileSpec;
	char		szAbsFileName[MAX_PATH];

	if (_fullpath(szAbsFileName,lpszFileName,MAX_PATH) == NULL)
	{
		_tcscpy(m_szErrMsg,_T("\tInvalid output Filename \n\nPlease use Absolute output Path"));
		goto TerminateQuickTimeConstructor;
	}

	BITMAPINFO	bmpInfo;
	bmpInfo.bmiHeader.biBitCount=0;
	bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);

	// Retrieve the bitmap's color format, width, and height.
	//BITMAP bmp;
    //if (!GetObject(hBackBitmap, sizeof(BITMAP), (LPSTR)&bmp))
	//{
	//	_tcscpy(m_szErrMsg,_T("Cannot get the bitmap information"));
	//	goto TerminateQuickTimeConstructor;		
	//}
	GetDIBits( hBackDC, hBackBitmap, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS ); 

	//	movie track width + height
	Fixed mtwidth, mtheight;
	mtwidth = FixRatio(bmpInfo.bmiHeader.biWidth,1);
	mtheight = FixRatio(bmpInfo.bmiHeader.biHeight,1);

	DBG_WARNING4("MOV - InitGraphicsWorld(%dx%d)(%dx%d)", (int)mtwidth, (int)mtheight, bmpInfo.bmiHeader.biWidth, bmpInfo.bmiHeader.biHeight );

	//bga - removing this call, it seems like the earlier call got all the info we needed
	// and this call to GetDIBits is stepping on memory in non-managed code. 
	//GetDIBits(hBackDC, hBackBitmap, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS);
	m_hBackDC = hBackDC;
	m_hBackBitmap = hBackBitmap;

	//	create the GWorld
	if (NewGWorldFromHBITMAP(&m_gWorld,NULL,NULL,NULL,hBackBitmap,hBackDC) != noErr)
	{
		_tcscpy(m_szErrMsg,_T("Unable to Create New Quick Time World From the Bitmap"));
		goto TerminateQuickTimeConstructor;
	}

	GetGWorld(&m_savedPort, &m_savedGD);
	SetGWorld(m_gWorld,NULL);

	c2pstrcpy(m_FileName, szAbsFileName);
	FSMakeFSSpec(0, 0, m_FileName, &fileSpec);

	//	create the movie file
	err = CreateMovieFile( &fileSpec,
							FOUR_CHAR_CODE('TVOD'),
							smCurrentScript,createMovieFileDeleteCurFile | createMovieFileDontCreateResFile,
							&m_resRefNum,&m_pMovie);
	if (err != noErr)
	{
		_tcscpy(m_szErrMsg,_T("Unable to Create Movie File"));
		goto TerminateQuickTimeConstructor;		
	}

	//	create a track for the video
	m_pVideoTrack = NewMovieTrack( m_pMovie, 
									mtwidth,			//Do not cast to (short) - Overflow occurs
									mtheight, 
									0);
	if (m_pVideoTrack==NULL)
	{
        _tcscpy(m_szErrMsg,_T("Unable to Create Video Track"));
		goto TerminateQuickTimeConstructor;		
	}

	m_trackFrame.left = 0;
	m_trackFrame.top = 0; 
	m_trackFrame.right = bmpInfo.bmiHeader.biWidth;		//Do not cast to (short) - Overflow occurs
	m_trackFrame.bottom = bmpInfo.bmiHeader.biHeight;

	m_pVideoMedia = NewTrackMedia( m_pVideoTrack, 
									VideoMediaType,
									(int)(kVideoTimeScale),		// video time scale
									nil, 0);
	if (m_pVideoMedia == NULL)
	{
		_tcscpy(m_szErrMsg,_T("Unable to Create Video Track Media"));
		goto TerminateQuickTimeConstructor;		
	}

	//
	nAppendFuncSelector=1;		//0=Dummy	1=FirstTime	2=Usual
	return S_OK;

TerminateQuickTimeConstructor:

	ReleaseMemory();
	MessageBox(NULL,m_szErrMsg,_T("Error"),MB_OK|MB_ICONERROR);
	return E_FAIL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CQTMovieFile::~CQTMovieFile(void)
{
	Close();

	ExitMovies();
	TerminateQTML();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CQTMovieFile::Close(void)
{
	if (nAppendFuncSelector == 2)
	{
		short resId = movieInDataForkResID;
		if (m_pVideoMedia)  
			EndMediaEdits(m_pVideoMedia);

		if (m_pVideoTrack)  
			InsertMediaIntoTrack(m_pVideoTrack, 
									kTrackStart,		// track start time
									kMediaStart,		// media start time
									GetMediaDuration(m_pVideoMedia),
									fixed1);
		if (m_pMovie)  
			AddMovieResource(m_pMovie, m_resRefNum, &resId,m_FileName);

		//--------------------
		// create a track reference from the target track to the timecode track
		//--------------------
		OSErr myErr = noErr;
		myErr = AddTrackReference(m_pTypeTrack, m_pTimeCodeTrack, kTrackReferenceTimeCode /*TimeCodeMediaType*/, NULL);
	}

	nAppendFuncSelector=0;

	ReleaseMemory();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CQTMovieFile::ReleaseMemory()
{
	nAppendFuncSelector = 0;		//Point to Dummy
	if (m_imageDesc)
	{
		DisposeHandle((Handle)m_imageDesc);
		m_imageDesc = NULL;
	}
	if (m_compressedData)
	{
		DisposeHandle(m_compressedData);
		m_compressedData = NULL;
	}
	if (m_resRefNum)
	{
		CloseMovieFile(m_resRefNum);
		m_resRefNum=0;
	}		
	if (m_pMovie)
	{	
		DisposeMovie(m_pMovie);
		m_pMovie = NULL;
	}
	if (m_savedPort)
	{
		SetGWorld(m_savedPort,m_savedGD);
		m_savedPort = NULL;
	}
	if (m_gWorld)
	{
		DisposeGWorld(m_gWorld);
		m_gWorld = NULL;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
HRESULT	CQTMovieFile::AppendFrameFirstTime(g2dImage* i_pSurface)
{
	//	add the first frame of the video track
	long maxCompressedSize;
	OSErr err = noErr;

	Rect rect;
	rect.left = m_trackFrame.left;
	rect.top = m_trackFrame.top;
	rect.right = m_trackFrame.right;
	rect.bottom = m_trackFrame.bottom;

	if (GetSystemMetrics(SM_CXSCREEN) > rect.right) 
		rect.right = GetSystemMetrics(SM_CXSCREEN);
	if (GetSystemMetrics(SM_CYSCREEN) > rect.bottom) 
		rect.bottom = GetSystemMetrics(SM_CYSCREEN);

	DBG_WARNING4("MOV - FirstFrame ud(%dx%d) lr(%dx%d)", rect.top, rect.bottom, rect.left, rect.right );

	//
	err = GetMaxCompressionSize(m_gWorld->portPixMap,
								&rect, 
								kMgrChoose, // let ICM choose depth
								m_CodecQuality, 
								m_CodecType, 
								(CompressorComponent) bestFidelityCodec, //bestCompressionCodec, //bestSpeedCodec, //anyCodec,
								&maxCompressedSize);
	if (err != noErr)	// see bottom of this doc for errors
	{
		_tcscpy(m_szErrMsg,_T("Unable to Get Compression Size"));

		switch (err)
		{
			case noCodecErr: _tcscat(m_szErrMsg, "\nCodec not installed"); break;
			case codecUnimpErr: _tcscat(m_szErrMsg, "\nUnimp Err"); break;
		}

		goto TerminateQuickTimeAppendFirstFrame;
	}

	m_compressedData = NewHandle(maxCompressedSize);
	if (m_compressedData == NULL)
	{
		_tcscpy(m_szErrMsg,_T("Unable to Allocate Memory"));
		goto TerminateQuickTimeAppendFirstFrame;
	}

	MoveHHi( m_compressedData );
	HLock( m_compressedData );
	m_compressedDataPtr = StripAddress( *m_compressedData );

	m_imageDesc = (ImageDescriptionHandle)NewHandle(4);
	if (m_imageDesc == NULL)
	{
		_tcscpy(m_szErrMsg,_T("Unable to Allocate Memory Handle"));
		goto TerminateQuickTimeAppendFirstFrame;
	}

	err = BeginMediaEdits (m_pVideoMedia);
	if (err != noErr)
	{
		_tcscpy(m_szErrMsg,_T("Unable to Begin Editing"));
		goto TerminateQuickTimeAppendFirstFrame;	
	}

	nAppendFuncSelector=2;

	HRESULT result = AppendFrameUsual(i_pSurface);

	return result;

TerminateQuickTimeAppendFirstFrame:
	ReleaseMemory();
	MessageBox(NULL,m_szErrMsg,_T("Error"),MB_OK|MB_ICONERROR);

	return E_FAIL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
HRESULT CQTMovieFile::AppendFrameUsual(g2dImage* i_pSurface)
{
	HBITMAP ihbm;
	g2dImage* pImg = NULL;
	RECT rect;
	HRESULT result = S_OK;

	SelectObject(m_hBackDC, m_hBackBitmap);

	if (i_pSurface == NULL)
	{
		g2dScreenCaptureUtil::CaptureWindowToImage(*m_pWindow, pImg);
		ihbm = pImg->GetHBMP(1);	// sampling
		delete pImg;

		int w=0,h=0;
		m_pWindow->GetDimensions(w,h);
		rect.left = rect.top = 0;
		rect.right = w;
		rect.bottom = h;
	}
	else
	{
		ihbm = i_pSurface->GetHBMP(1);

		rect.top = 0;
		rect.left = 0;
		rect.right = i_pSurface->GetWidth();
		rect.bottom = i_pSurface->GetHeight();
	}

	HDC newdc = CreateCompatibleDC(m_hBackDC);
	SelectObject(newdc, ihbm);

	bool bval = BitBlt(m_hBackDC, 0, 0, rect.right, rect.bottom, newdc, 0, 0, SRCCOPY);
	if (!bval)
	{
		LPVOID lpMsgBuf;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			0,
			(LPTSTR)&lpMsgBuf,
			0,
			NULL );
		MessageBox(NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
	}

	OSErr err = noErr;
	err = CompressImage(m_gWorld->portPixMap, 
						&m_trackFrame, 
						m_CodecQuality,
						m_CodecType,
						m_imageDesc, 
						m_compressedDataPtr );
	if (err != noErr)
	{
		result = E_FAIL;
		goto bail;
	}

	err = AddMediaSample(m_pVideoMedia, 
						m_compressedData,
						kNoOffset,	// no offset in data
						(**m_imageDesc).dataSize, 
						(float)((float)kVideoTimeScale / m_fFramesPerSecond),	// fps 
						(SampleDescriptionHandle)m_imageDesc, 
						kAddOneVideoSample,	// one sample
						kSyncSample,		// self-contained samples
						nil);
	if (err != noErr)
	{
		result = E_FAIL;
		goto bail;
	}

bail:
	::DeleteObject(ihbm);
	ReleaseDC(NULL, newdc);
	::DeleteDC(newdc);
	return result;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
HRESULT CQTMovieFile::AppendDummy(g2dImage* i_pSurface)
{
	return E_FAIL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
HRESULT CQTMovieFile::AppendNewFrame(float i_fCurrentTime, g2dImage* i_pSurface)
{
	m_fCurrentTime = i_fCurrentTime;
	return (this->*pAppendFrame[nAppendFuncSelector])(i_pSurface);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CQTMovieFile::AppendTimeCodeFrame(float i_fStartTime, float i_fEndTime)
{
#ifdef USE_QUICKTIME_TIMECODE
	QTTC_AddTimeCodeToMovie( this->m_pMovie, this->m_pTypeTrack, i_fStartTime, i_fEndTime );
#endif
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CQTMovieFile::SetCodec(const std::string& i_CodeType)
{
	char strcode[4];
	strncpy(strcode, i_CodeType.c_str(), 4);
	m_CodecType = strcode[3] << 0 | strcode[2] << 8 | strcode[1] << 16 | strcode[0] << 24;

	m_CodecQuality = codecHighQuality; //codecNormalQuality;

	//m_CodecType = kRawCodecType;
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void CQTMovieFile::SetFramesPerSecond(const float i_FramesPerSecond)
{
	m_fFramesPerSecond = i_FramesPerSecond;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
Movie CQTMovieFile::GetMovie()
{
	return this->m_pMovie;
}

//----------------------------------------------------------------------------
//
// QTTC_AddTimeCodeToMovie
// Add a timecode track to the specified movie.
//
//----------------------------------------------------------------------------
#ifdef USE_QUICKTIME_TIMECODE
OSErr CQTMovieFile::QTTC_AddTimeCodeToMovie(Movie theMovie, Track i_pTypeTrack, float i_fStartTime, float i_fEndTime)
{
	TimeCodeDef			myTCDef;
	TimeCodeRecord		myTCRec;
	Str63				myString;
	TimeValue			myDuration;
	MatrixRecord		myMatrix;
	Fixed				myWidth;
	Fixed				myHeight;
	Fixed				myTCHeight;
	long				myFlags = 0L;
	TCTextOptions		myTextOptions;
	FontInfo			myFontInfo;
	long				**myFrameHandle;
	TimeValue			sample_time;
	TimeCodeDescriptionHandle	myDesc = NULL;
	OSErr				myErr = noErr;
	
#ifdef USE_QUICKTIME_TIMECODE
	OSErr		err = noErr;
	// get the (first) track of the specified type; this track determines the width of the new timecode track
	m_pTypeTrack = GetMovieIndTrackType(m_pMovie, 1, VideoMediaType, movieTrackMediaType);
	if (m_pTypeTrack == NULL) 
	{
		err = trackNotInMovie;
		goto bail;
	}

	// get the dimensions of the target track
	//Fixed myWidth, myHeight;
	GetTrackDimensions(m_pTypeTrack, &myWidth, &myHeight);
	
	//	create a track for the timecode
	//
	m_pTimeCodeTrack = NewMovieTrack( m_pMovie, 
										myWidth,			//mtwidth,		//Do not cast to (short) - Overflow occurs
										kTimeCodeTrackSize,	//mtheight, 
										kNoVolume);
	if (m_pTimeCodeTrack == NULL)
	{
        _tcscpy(m_szErrMsg,_T("Unable to Create TimeCode Track"));
		goto bail;		
	}

	m_pTimeCodeMedia = NewTrackMedia( m_pTimeCodeTrack, 
									TimeCodeMediaType,
									GetMovieTimeScale(m_pMovie), //(int)(kVideoTimeScale),		// video time scale
									NULL, 0);
	if (m_pTimeCodeMedia == NULL)
	{
		_tcscpy(m_szErrMsg,_T("Unable to Create TimeCode Track Media"));
		goto bail;		
	}

	m_pTimeCodeMediaHandler = GetMediaHandler(m_pTimeCodeMedia);
	if (m_pTimeCodeMediaHandler == NULL)
	{
		_tcscpy(m_szErrMsg,_T("Unable to Create TimeCode Track Media"));
		goto bail;		
	}
#endif

	GetTrackDimensions(i_pTypeTrack, &myWidth, &myHeight);

	//--------------------
	// fill in a timecode definition structure; this becomes part of the timecode description
	//--------------------
	
	//myFlags = tcCounter;	// if display as a counter instead of time HH:MM:SS:FF
	myTCDef.flags = myFlags;
	myTCDef.fTimeScale = kVideoTimeScale;
	myTCDef.frameDuration = (float)((float)kVideoTimeScale / m_fFramesPerSecond);	// how long each frame lasts
	myTCDef.numFrames = (UInt8)m_fFramesPerSecond;	// number of frames per second

	//	run some calculations
	int hr,mn,sc,fr, total_frames;
	float ms;
	appTimeUtils::ConvertTimeToHMSM( i_fStartTime, hr, mn, sc, ms );
	fr = (int)(ms / myTCDef.frameDuration);	// convert milliseconds to frames;
	total_frames = (int)(m_fFramesPerSecond * (i_fEndTime - i_fStartTime));

	//--------------------
	// fill in a timecode record
	//--------------------
	//DBG_LOG3("total frames = %d  sec = %d  start time = %6.3f", total_frames, sc, i_fStartTime );
	myTCRec.t.hours		= (UInt8)hr;
	myTCRec.t.minutes	= (UInt8)mn;
	myTCRec.t.seconds	= (UInt8)sc;
	myTCRec.t.frames	= (UInt8)fr;

	//--------------------
	// figure out the timecode track geometry
	//--------------------

	// get display options to calculate box height
	TCGetDisplayOptions(m_pTimeCodeMediaHandler, &myTextOptions);
	Str255 gFontName;						// name of current font
	GetFNum(gFontName, &myTextOptions.txFont);
	TCSetDisplayOptions(m_pTimeCodeMediaHandler, &myTextOptions);
	
	// use the starting time to figure out the dimensions of track	
	TCTimeCodeToString(m_pTimeCodeMediaHandler, &myTCDef, &myTCRec, myString);
	TextFont(myTextOptions.txFont);
	TextFace(myTextOptions.txFace);
	TextSize(myTextOptions.txSize);
	GetFontInfo(&myFontInfo);
	
	// calculate track width and height based on text	
	myTCHeight = FixRatio(myFontInfo.ascent + myFontInfo.descent + 2, 1);
	SetTrackDimensions(m_pTimeCodeTrack, myWidth, myTCHeight);

	GetTrackMatrix(m_pTimeCodeTrack, &myMatrix);
	if (false)
		TranslateMatrix(&myMatrix, 0, myHeight);
	
	SetTrackMatrix(m_pTimeCodeTrack, &myMatrix);	
	const bool gDisplayTimeCode = false;
	SetTrackEnabled(m_pTimeCodeTrack, gDisplayTimeCode ? true : false);
		
	TCSetTimeCodeFlags(m_pTimeCodeMediaHandler, gDisplayTimeCode ? tcdfShowTimeCode : 0, tcdfShowTimeCode);
	
	//--------------------
	// edit the track media
	//--------------------
	myErr = BeginMediaEdits(m_pTimeCodeMedia);	
	if (myErr == noErr) 
	{
		long		mySize;
		UserData	myUserData;
		
		//--------------------
		// create and configure a new timecode description handle
		//--------------------
		mySize = sizeof(TimeCodeDescription);
		myDesc = (TimeCodeDescriptionHandle)NewHandleClear(mySize);
		if (myDesc == NULL)
			goto bail;
		
		(**myDesc).descSize = mySize;
		(**myDesc).dataFormat = TimeCodeMediaType;
		(**myDesc).timeCodeDef = myTCDef;
		
		//--------------------
		// set the source identification information
		//--------------------

		// the source identification information for a timecode track is stored
		// in a user data item of type TCSourceRefNameType
		myErr = NewUserData(&myUserData);
		if (myErr == noErr) 
		{
			Handle myNameHandle = NULL;
			
			char gSrcName[256];
			myErr = PtrToHand(&gSrcName[1], &myNameHandle, gSrcName[0]);
			if (myErr == noErr) 
			{
				myErr = AddUserDataText(myUserData, myNameHandle, TCSourceRefNameType, 1, langEnglish);
				if (myErr == noErr)
					TCSetSourceRef(m_pTimeCodeMediaHandler, myDesc, myUserData);
			}
			
			if (myNameHandle != NULL)
				DisposeHandle(myNameHandle);
				
			DisposeUserData(myUserData);
		}

		//--------------------
		// add a sample to the timecode track
		//
		// each sample in a timecode track provides timecode information for a span of movie time;
		// here, we add a single sample that spans the entire movie duration
		//--------------------

		// the sample data contains a frame number that identifies one or more content frames
		// that use the timecode; this value (a long integer) identifies the first frame that
		// uses the timecode
		myFrameHandle = (long **)NewHandle(sizeof(long));
		if (myFrameHandle == NULL)
			goto bail;

		myErr = TCTimeCodeToFrameNumber(m_pTimeCodeMediaHandler, &(**myDesc).timeCodeDef, &myTCRec, *myFrameHandle);

		// the data in the timecode track must be big-endian		
		**myFrameHandle = EndianS32_NtoB(**myFrameHandle);

		// since we created the track with the same timescale as the movie,
		// we don't need to convert the duration
		//
		myDuration = (total_frames) * myTCDef.frameDuration;
		DBG_LOG1("2) curr ams dur(%d)", myDuration );
		
		int offset_in_data = 0;
		myErr = AddMediaSample(	m_pTimeCodeMedia, 
								(Handle)myFrameHandle, 
								offset_in_data, 
								GetHandleSize((Handle)myFrameHandle), 
								myDuration, 
								(SampleDescriptionHandle)myDesc, 
								1, 
								0, 
								&sample_time);
		if (myErr != noErr)
			goto bail;	

		int trackstart	= (m_fFramesPerSecond * (i_fStartTime));		// Contains a time value specifying where the segment is to be inserted.
		int mediatime	= (m_fFramesPerSecond * (i_fStartTime));		// Contains a time value specifying the starting point of the segment in the media.
		int tmp_tcdur	= GetMediaDuration(m_pTimeCodeMedia);
		int tmp_vdur	= GetMediaDuration(m_pVideoMedia);
		DBG_LOG6("sampletime=%d  tcmdur = %d   tvdur = %d   duration = %d  trackstart=%d   mediatime=%d", sample_time, tmp_tcdur, tmp_vdur, myDuration, trackstart, mediatime);

		myErr = InsertMediaIntoTrack(m_pTimeCodeTrack, tmp_vdur, sample_time, myDuration, fixed1);
		if (myErr != noErr)
			goto bail;
	}

	//	close up the media edits
	myErr = EndMediaEdits(m_pTimeCodeMedia);	
	//if (myErr != noErr)
	//	goto bail;

bail:
	if (myDesc != NULL)
		DisposeHandle((Handle)myDesc);
		
	if (myFrameHandle != NULL)
		DisposeHandle((Handle)myFrameHandle);

	return(myErr);
}
#endif



//============================================================================
//============================================================================
namespace
{
	CQTMovieFile l_MovieFile;
}

//============================================================================
//
//	mnmQuickTimeUtil namespace
//
//		Interface to the quicktime functions
//
//============================================================================
namespace mnmQuickTimeUtil
{

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void SetFramesPerSecond(const float i_FramesPerSecond)
{
	l_MovieFile.SetFramesPerSecond( i_FramesPerSecond );
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void SetCodec(const std::string& i_CodeType)
{
	l_MovieFile.SetCodec( i_CodeType );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
HRESULT InitGraphicsWorld(HDC hBackDC,HBITMAP hBackBitmap,LPCTSTR lpszFileName, g2dWindow* i_pWindow)
{
	return l_MovieFile.InitGraphicsWorld(hBackDC, hBackBitmap, lpszFileName, i_pWindow);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
HRESULT	AppendNewFrame(float i_fCurrentTime, g2dImage* i_pSurface)
{
	return l_MovieFile.AppendNewFrame(i_fCurrentTime, i_pSurface);
}

//------------------------------------------------------------------------
//	WriteTimeCode() - write the timecode into the movie.
//------------------------------------------------------------------------
void WriteTimeCode( const float i_fStartTime, const float i_fEndTime )
{
	l_MovieFile.AppendTimeCodeFrame(i_fStartTime, i_fEndTime);
}

//------------------------------------------------------------------------
//	WriteWavAudio() - write an audio track into the movie
//------------------------------------------------------------------------
HRESULT WriteWavAudio(const char* i_AudioFile)
{
//	Track sndDest = l_MovieFile.addTrack(0, 0, kFullVolume);
	
	mnmQuickTimeSound::QTSound_CreateMySoundTrack(l_MovieFile.GetMovie(), i_AudioFile);

	return 0;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
HRESULT Close()
{
	l_MovieFile.Close();
	return 0;
}
}

///* QuickTime errors (Image Compression Manager) */
//enum {
//  codecErr                      = -8960,
//  noCodecErr                    = -8961,
//  codecUnimpErr                 = -8962,
//  codecSizeErr                  = -8963,
//  codecScreenBufErr             = -8964,
//  codecImageBufErr              = -8965,
//  codecSpoolErr                 = -8966,
//  codecAbortErr                 = -8967,
//  codecWouldOffscreenErr        = -8968,
//  codecBadDataErr               = -8969,
//  codecDataVersErr              = -8970,
//  codecExtensionNotFoundErr     = -8971,
//  scTypeNotFoundErr             = codecExtensionNotFoundErr,
//  codecConditionErr             = -8972,
//  codecOpenErr                  = -8973,
//  codecCantWhenErr              = -8974,
//  codecCantQueueErr             = -8975,
//  codecNothingToBlitErr         = -8976,
//  codecNoMemoryPleaseWaitErr    = -8977,
//  codecDisabledErr              = -8978, /* codec disabled itself -- pass codecFlagReenable to reset*/
//  codecNeedToFlushChainErr      = -8979,
//  lockPortBitsBadSurfaceErr     = -8980,
//  lockPortBitsWindowMovedErr    = -8981,
//  lockPortBitsWindowResizedErr  = -8982,
//  lockPortBitsWindowClippedErr  = -8983,
//  lockPortBitsBadPortErr        = -8984,
//  lockPortBitsSurfaceLostErr    = -8985,
//  codecParameterDialogConfirm   = -8986,
//  codecNeedAccessKeyErr         = -8987, /* codec needs password in order to decompress*/
//  codecOffscreenFailedErr       = -8988,
//  codecDroppedFrameErr          = -8989, /* returned from ImageCodecDrawBand */
//  directXObjectAlreadyExists    = -8990,
//  lockPortBitsWrongGDeviceErr   = -8991,
//  codecOffscreenFailedPleaseRetryErr = -8992,
//  badCodecCharacterizationErr   = -8993,
//  noThumbnailFoundErr           = -8994
//}
