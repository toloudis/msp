/****************************************************************************\
**  mnmAviUtil.cpp
**
**      mnmAviUtil.cpp supplies routines for saving AVI movie files.
**
**	StudioGPU
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/
#include "Support/mnm/mnmAviUtil.hpp"
#include "Core/it/itStringUtil.hpp"


//==============================================================================
//	library pragmas
//==============================================================================
#pragma comment(lib,"Vfw32.lib")

//==============================================================================
//==============================================================================
namespace mnmAviUtil
{
	HRESULT LastError = S_OK;

// First, we'll define the WAV file format.
#include <pshpack1.h>

typedef struct
{ char id[4];						//="fmt "
  unsigned long size;				//=16
  short wFormatTag;					//=WAVE_FORMAT_PCM=1
  unsigned short wChannels;			//=1 or 2 for mono or stereo
  unsigned long dwSamplesPerSec;	//=11025 or 22050 or 44100
  unsigned long dwAvgBytesPerSec;	//=wBlockAlign * dwSamplesPerSec
  unsigned short wBlockAlign;		//=wChannels * (wBitsPerSample==8?1:2)
  unsigned short wBitsPerSample;	//=8 or 16, for bits per sample
} FmtChunk;

typedef struct
{ char id[4];            //="data"
  unsigned long size;    //=datsize, size of the following array
  unsigned char data[1]; //=the raw data goes here
} DataChunk;

typedef struct
{ char id[4];			//="RIFF"
  unsigned long size;	//=datsize+8+16+4
  char type[4];			//="WAVE"
  FmtChunk fmt;
  DataChunk dat;
} WavChunk;
#include <poppack.h>

// This is the internal structure represented by the HAVI handle:
typedef struct
{ 
	IAVIFile *pfile;				// created by CreateAvi
	WAVEFORMATEX wfx;				// as given to CreateAvi (.nChanels=0 if none was given). Used when audio stream is first created.
	int period;						// specified in CreateAvi, used when the video stream is first created
	IAVIStream *as;					// audio stream, initialised when audio stream is first created
	IAVIStream *ps, *psCompressed;  // video stream, when first created
	unsigned long nframe, nsamp;    // which frame will be added next, which sample will be added next
	bool iserr;						// if true, then no function will do anything
	AVISTREAMINFO psi;				// Stream info for reading
	PGETFRAME	  pgf;				// Info for a single frame
	unsigned char* pdata;			//Decompressed and formatted data
	HDRAWDIB hdd;
	HDC hdc;
	HBITMAP hBitmap;
} TAviUtil;

TAviUtil* CreateTAviUtil()
{
	TAviUtil* avi = new TAviUtil;
	avi->pfile = NULL;
	ZeroMemory(&avi->wfx,sizeof(WAVEFORMATEX));
	avi->period = 0;
	avi->as = NULL;
	avi->ps = NULL;
	avi->psCompressed = NULL;
	avi->nframe = 0;
	avi->nsamp = 0;
	avi->iserr = false;
	ZeroMemory(&avi->psi,sizeof(AVISTREAMINFO));
	avi->pgf = NULL;
	avi->pdata = NULL;
	avi->hdd = NULL;
	avi->hBitmap = NULL;
	return avi;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
HAVI CreateAvi(const TCHAR *fn, int frameperiod, const WAVEFORMATEX *wfx)
{ 
	IAVIFile *pfile;
	AVIFileInit();
	LastError = AVIFileOpen(&pfile, fn, OF_WRITE|OF_CREATE, NULL);
	if (LastError!=AVIERR_OK)
	{
		AVIFileExit();
		DBG_ERROR("Could not create AVI (" << itString(fn) << " Error code (" << LastError << ")");
		return NULL;
	}
	TAviUtil *au = CreateTAviUtil();
	au->pfile = pfile;
	if( wfx )CopyMemory(&au->wfx,wfx,sizeof(WAVEFORMATEX));
	au->period = frameperiod;
	return (HAVI)au;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
HRESULT CloseAvi(HAVI avi)
{ 
	if (avi==NULL){ LastError = AVIERR_BADHANDLE; return LastError; }
	TAviUtil *au = (TAviUtil*)avi;
	if (au->as!=0) AVIStreamRelease(au->as); au->as=0;
	if (au->psCompressed!=0) AVIStreamRelease(au->psCompressed); au->psCompressed=0;
	if (au->ps!=0) AVIStreamRelease(au->ps); au->ps=0;
	if (au->pfile!=0) AVIFileRelease(au->pfile); au->pfile=0;
	if( au->pgf != 0) AVIStreamGetFrameClose(au->pgf);
	if( au->hdd != 0) DrawDibClose(au->hdd);
	if( au->hBitmap != 0) DeleteObject(au->hBitmap);
	AVIFileExit();
	delete au;

	LastError = S_OK;
	return LastError;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
HRESULT SetAviVideoCompression(HAVI avi, HBITMAP hbm, AVICOMPRESSOPTIONS *opts, bool ShowDialog, HWND hparent)
{ 
	if (avi==NULL){ LastError = AVIERR_BADHANDLE; return LastError; }
	if (hbm==NULL){ LastError = AVIERR_BADPARAM; return LastError; }
	DIBSECTION dibs; int sbm = GetObject(hbm,sizeof(dibs),&dibs);
	if (sbm!=sizeof(DIBSECTION)){ LastError = AVIERR_BADPARAM; return LastError; }
	TAviUtil *au = (TAviUtil*)avi;
	if (au->iserr){ LastError = AVIERR_ERROR; return LastError; }
	if (au->psCompressed!=0){ LastError = AVIERR_COMPRESSOR; return LastError; }
	//
	if (au->ps==0) // create the stream, if it wasn't there before
	{ 
		AVISTREAMINFO strhdr; ZeroMemory(&strhdr,sizeof(strhdr));
		strhdr.fccType = streamtypeVIDEO;// stream type
		strhdr.fccHandler = 0;
		strhdr.dwScale = au->period;
		strhdr.dwRate = 1000;
		strhdr.dwSuggestedBufferSize  = dibs.dsBmih.biSizeImage;
		SetRect(&strhdr.rcFrame, 0, 0, dibs.dsBmih.biWidth, dibs.dsBmih.biHeight);
		LastError=AVIFileCreateStream(au->pfile, &au->ps, &strhdr);
		if (LastError!=AVIERR_OK) {au->iserr=true; return LastError;}
	}
	//
	if (au->psCompressed==0) // set the compression, prompting dialog if necessary
	{ 
		AVICOMPRESSOPTIONS myopts; ZeroMemory(&myopts,sizeof(myopts));
		AVICOMPRESSOPTIONS *aopts[1];
		if (opts!=NULL) aopts[0]=opts; else aopts[0]=&myopts;
		if (ShowDialog)
		{ 
			BOOL res = (BOOL)AVISaveOptions(hparent,0,1,&au->ps,aopts);
			if (!res) {AVISaveOptionsFree(1,aopts); au->iserr=true; LastError = AVIERR_USERABORT; return LastError; }
		}
#if 0
		DBG_TRACE("fccType:         " << static_cast<int>(myopts.fccType));
		DBG_TRACE("fccHandler:      " << static_cast<int>(myopts.fccHandler));
		DBG_TRACE("dwKeyFrameEvery: " << static_cast<int>(myopts.dwKeyFrameEvery));
		DBG_TRACE("dwQuality:       " << static_cast<int>(myopts.dwQuality));
		DBG_TRACE("dwFlags          " << static_cast<int>(myopts.dwFlags));
		DBG_TRACE("dwBytesPerSecond " << static_cast<int>(myopts.dwBytesPerSecond));
		//DBG_TRACE("lpFormat:        " << static_cast<int>(myopts.lpFormat));
		DBG_TRACE("cbFormat:        " << static_cast<int>(myopts.cbFormat));
		//DBG_TRACE("lpParms:         " << static_cast<int>(myopts.lpParms ));
		DBG_TRACE("cbParms:         " << static_cast<int>(myopts.cbParms ));
		DBG_TRACE("dwInterleaveEvery" << static_cast<int>(myopts.dwInterleaveEvery ));
#endif
		CoInitialize(NULL);
		LastError = AVIMakeCompressedStream(&au->psCompressed, au->ps, aopts[0], NULL);
		AVISaveOptionsFree(1,aopts);
		if (LastError != AVIERR_OK) {au->iserr=true; return LastError;}
		DIBSECTION dibs; GetObject(hbm,sizeof(dibs),&dibs);
		LastError = AVIStreamSetFormat(au->psCompressed, 0, &dibs.dsBmih, dibs.dsBmih.biSize+dibs.dsBmih.biClrUsed*sizeof(RGBQUAD));
		CoUninitialize();
		if (LastError!=AVIERR_OK) {au->iserr=true; return LastError;}
	}
	//
	LastError = AVIERR_OK;
	return LastError;
}

HRESULT AppendAvi(HAVI avi1, HAVI avi2, const itString i_SavePath)
{
	if (avi1==NULL){ LastError = AVIERR_BADHANDLE; return LastError; }
	if (avi2==NULL){ LastError = AVIERR_BADHANDLE; return LastError; }
	TAviUtil *au1 = (TAviUtil*)avi1;
	PAVISTREAM pEditableStream = NULL;
	PAVISTREAM pEditableStream2 = NULL;
	//TAviUtil *au1e = (TAviUtil*)avi1;
	TAviUtil *au2 = (TAviUtil*)avi2;
	if (au1->iserr){ LastError = AVIERR_ERROR; return LastError; }
	if (au2->iserr){ LastError = AVIERR_ERROR; return LastError; }
	long Length  = 1;
	long Buf;
	CreateEditableStream(&pEditableStream,NULL);
	
	AVICOMPRESSOPTIONS aco;
	if (au1->ps==0) // create the stream, if it wasn't there before
	{ 
		AVISTREAMINFO strhdr; ZeroMemory(&strhdr,sizeof(strhdr));
		strhdr.fccType = streamtypeVIDEO;// stream type
		strhdr.fccHandler = 0;
		strhdr.dwScale = au1->period;
		strhdr.dwRate = 1000;
		LastError=AVIFileCreateStream(au1->pfile, &au1->ps, &strhdr);
		if (LastError!=AVIERR_OK) {au1->iserr=true; return LastError;}
	}
	if (au2->ps==0) // create the stream, if it wasn't there before
	{ 
		AVISTREAMINFO strhdr; ZeroMemory(&strhdr,sizeof(strhdr));
		strhdr.fccType = streamtypeVIDEO;// stream type
		strhdr.fccHandler = 0;
		strhdr.dwScale = au2->period;
		strhdr.dwRate = 1000;
		LastError=AVIFileCreateStream(au2->pfile, &au2->ps, &strhdr);
		if (LastError!=AVIERR_OK) {au2->iserr=true; return LastError;}
	}
	LastError = EditStreamPaste(pEditableStream, &Length, &Buf, au1->ps, 1.0, -1);
	
	if (LastError!=AVIERR_OK) {au1->iserr=true; return LastError;}
	Length = GetAviLength(avi1) ;
	long buf1;
	
	LastError = EditStreamPaste(pEditableStream, &Length, &buf1, au2->ps, 1.0, -1);
	if (LastError!=AVIERR_OK) {au1->iserr=true; return LastError;}

	ZeroMemory(&aco, sizeof(AVICOMPRESSOPTIONS));
	aco.fccType = au2->psi.fccType;
	aco.fccHandler = au2->psi.fccHandler;
	aco.dwKeyFrameEvery = 1;
	aco.dwQuality = au2->psi.dwQuality;

	int res = AVISave(i_SavePath.GetString(), NULL, NULL, 1, pEditableStream, &aco);
	
	LastError = S_OK;
	return LastError;


}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
HRESULT AddAviFrame(HAVI avi, HBITMAP hbm)
{ 
	if (avi==NULL){ LastError = AVIERR_BADHANDLE; return LastError; }
	if (hbm==NULL){ LastError = AVIERR_BADPARAM; return LastError; }
	DIBSECTION dibs; int sbm = GetObject(hbm,sizeof(dibs),&dibs);
	if (sbm!=sizeof(DIBSECTION)){ LastError = AVIERR_BADPARAM; return LastError; }
	TAviUtil *au = (TAviUtil*)avi;
	if (au->iserr){ LastError = AVIERR_ERROR; return LastError; }
	//
	if (au->ps==0) // create the stream, if it wasn't there before
	{ 
		AVISTREAMINFO strhdr; ZeroMemory(&strhdr,sizeof(strhdr));
		strhdr.fccType = streamtypeVIDEO;// stream type
		strhdr.fccHandler = 0;
		strhdr.dwScale = au->period;
		strhdr.dwRate = 1000;
		strhdr.dwSuggestedBufferSize  = dibs.dsBmih.biSizeImage;
		SetRect(&strhdr.rcFrame, 0, 0, dibs.dsBmih.biWidth, dibs.dsBmih.biHeight);
		LastError=AVIFileCreateStream(au->pfile, &au->ps, &strhdr);
		if (LastError!=AVIERR_OK) {au->iserr=true; return LastError;}
	}
	//
	// create an empty compression, if the user hasn't set any
	if (au->psCompressed==0)
	{ 
		AVICOMPRESSOPTIONS opts; ZeroMemory(&opts,sizeof(opts));
		opts.fccHandler=mmioFOURCC('D','I','B',' ');
		CoInitialize(NULL);
		LastError = AVIMakeCompressedStream(&au->psCompressed, au->ps, &opts, NULL);
		CoUninitialize();
		if (LastError != AVIERR_OK) {au->iserr=true; return LastError;}
		LastError = AVIStreamSetFormat(au->psCompressed, 0, &dibs.dsBmih, dibs.dsBmih.biSize+dibs.dsBmih.biClrUsed*sizeof(RGBQUAD));
		if (LastError!=AVIERR_OK) {au->iserr=true; return LastError;}
	}
	//
	//Now we can add the frame
	LastError = AVIStreamWrite(au->psCompressed, au->nframe, 1, dibs.dsBm.bmBits, dibs.dsBmih.biSizeImage, AVIIF_KEYFRAME, NULL, NULL);
	if (LastError!=AVIERR_OK) {au->iserr=true; return LastError;}
	au->nframe++;
	LastError = S_OK;
	return LastError;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
HRESULT AddAviAudio(HAVI avi, void *dat, unsigned long numbytes)
{ 
	if (avi==NULL){ LastError = AVIERR_BADHANDLE; return LastError; }
	if (dat==NULL || numbytes==0){ LastError = AVIERR_BADPARAM; return LastError; }
	TAviUtil *au = (TAviUtil*)avi;
	if (au->iserr){ LastError = AVIERR_ERROR; return LastError; }
	if (au->wfx.nChannels==0){ LastError = AVIERR_BADFORMAT; return LastError; }
	unsigned long numsamps = numbytes*8 / au->wfx.wBitsPerSample;
	if ((numsamps*au->wfx.wBitsPerSample/8)!=numbytes){ LastError = AVIERR_BADPARAM; return LastError; }
	//
	if (au->as==0) // create the stream if necessary
	{ 
		AVISTREAMINFO ahdr; ZeroMemory(&ahdr,sizeof(ahdr));
		ahdr.fccType=streamtypeAUDIO;
		ahdr.dwScale=au->wfx.nBlockAlign;
		ahdr.dwRate=au->wfx.nSamplesPerSec*au->wfx.nBlockAlign;
		ahdr.dwSampleSize=au->wfx.nBlockAlign;
		ahdr.dwQuality=(DWORD)-1;
		LastError = AVIFileCreateStream(au->pfile, &au->as, &ahdr);
		if (LastError!=AVIERR_OK) {au->iserr=true; return LastError;}
		LastError = AVIStreamSetFormat(au->as,0,&au->wfx,sizeof(WAVEFORMATEX));
		if (LastError!=AVIERR_OK) {au->iserr=true; return LastError;}
	}
	//
	// now we can write the data
	LastError = AVIStreamWrite(au->as,au->nsamp,numsamps,dat,numbytes,0,NULL,NULL);
	if (LastError!=AVIERR_OK) {au->iserr=true; return LastError;}
	au->nsamp+=numsamps;
	LastError = S_OK;
	return LastError;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
HRESULT AddAviWav(HAVI avi, const TCHAR *src, DWORD flags)
{ 
	if (avi==NULL){ LastError = AVIERR_BADHANDLE; return LastError; }
	if (flags!=SND_MEMORY && flags!=SND_FILENAME){ LastError = AVIERR_BADFLAGS; return LastError; }
	if (src==0){ LastError = AVIERR_BADPARAM; return LastError; }
	TAviUtil *au = (TAviUtil*)avi;
	if (au->iserr){ LastError = AVIERR_ERROR; return LastError; }
	//
	char *buf=0; WavChunk *wav = (WavChunk*)src;
	if (flags==SND_FILENAME)
	{ 
		HANDLE hf=CreateFile(src,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if (hf==INVALID_HANDLE_VALUE)
		{
			au->iserr=true; 
			LastError =AVIERR_FILEOPEN;
			return LastError;
		}
		DWORD size = GetFileSize(hf,NULL);
		buf = new char[size];
		DWORD red; ReadFile(hf,buf,size,&red,NULL);
		CloseHandle(hf);
		wav = (WavChunk*)buf;
	}

	// check that format doesn't clash
	bool badformat=false;
	if (au->wfx.nChannels==0)
	{ 
		au->wfx.wFormatTag=wav->fmt.wFormatTag;
		au->wfx.cbSize=0;
		au->wfx.nAvgBytesPerSec=wav->fmt.dwAvgBytesPerSec;
		au->wfx.nBlockAlign=wav->fmt.wBlockAlign;
		au->wfx.nChannels=wav->fmt.wChannels;
		au->wfx.nSamplesPerSec=wav->fmt.dwSamplesPerSec;
		au->wfx.wBitsPerSample=wav->fmt.wBitsPerSample;
	}
	else
	{ 
		if (au->wfx.wFormatTag!=wav->fmt.wFormatTag) badformat=true;
		if (au->wfx.nAvgBytesPerSec!=wav->fmt.dwAvgBytesPerSec) badformat=true;
		if (au->wfx.nBlockAlign!=wav->fmt.wBlockAlign) badformat=true;
		if (au->wfx.nChannels!=wav->fmt.wChannels) badformat=true;
		if (au->wfx.nSamplesPerSec!=wav->fmt.dwSamplesPerSec) badformat=true;
		if (au->wfx.wBitsPerSample!=wav->fmt.wBitsPerSample) badformat=true;
	}
	if (badformat) {if (buf!=0) delete[] buf; LastError = AVIERR_BADFORMAT; return LastError; }

	//
	if (au->as==0) // create the stream if necessary
	{ 
		AVISTREAMINFO ahdr; ZeroMemory(&ahdr,sizeof(ahdr));
		ahdr.fccType=streamtypeAUDIO;
		ahdr.dwScale=au->wfx.nBlockAlign;
		ahdr.dwRate=au->wfx.nSamplesPerSec*au->wfx.nBlockAlign;
		ahdr.dwSampleSize=au->wfx.nBlockAlign;
		ahdr.dwQuality=(DWORD)-1;
		LastError = AVIFileCreateStream(au->pfile, &au->as, &ahdr);
		if (LastError!=AVIERR_OK) {if (buf!=0) delete[] buf; au->iserr=true; return LastError;}
		LONG pos = 0;
		LastError = AVIStreamSetFormat(au->as,pos,&au->wfx,sizeof(WAVEFORMATEX));
		if (LastError!=AVIERR_OK) {if (buf!=0) delete[] buf; au->iserr=true; return LastError;}
	}
	//
	// now we can write the data
	unsigned long numbytes = wav->dat.size;
	unsigned long numsamps = numbytes*8 / au->wfx.wBitsPerSample;
	DWORD aviflags = 0;
	LastError = AVIStreamWrite(au->as,au->nsamp,numsamps,wav->dat.data,numbytes,aviflags,NULL,NULL);
	if (buf!=0) delete[] buf;
	if (LastError!=AVIERR_OK) {au->iserr=true; return LastError;}
	au->nsamp+=numsamps;
	LastError = S_OK;
	return LastError;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
unsigned int FormatAviMessage(HRESULT code, char *buf,unsigned int len)
{ 
	const char *msg="unknown avi result code";
	switch (code)
	{ 
		case S_OK: msg="Success"; break;
		case AVIERR_BADFORMAT: msg="AVIERR_BADFORMAT: corrupt file or unrecognized format"; break;
		case AVIERR_MEMORY: msg="AVIERR_MEMORY: insufficient memory"; break;
		case AVIERR_FILEREAD: msg="AVIERR_FILEREAD: disk error while reading file"; break;
		case AVIERR_FILEOPEN: msg="AVIERR_FILEOPEN: disk error while opening file"; break;
		case REGDB_E_CLASSNOTREG: msg="REGDB_E_CLASSNOTREG: file type not recognized"; break;
		case AVIERR_READONLY: msg="AVIERR_READONLY: file is read-only"; break;
		case AVIERR_NOCOMPRESSOR: msg="AVIERR_NOCOMPRESSOR: a suitable compressor could not be found"; break;
		case AVIERR_UNSUPPORTED: msg="AVIERR_UNSUPPORTED: compression is not supported for this type of data"; break;
		case AVIERR_INTERNAL: msg="AVIERR_INTERNAL: internal error"; break;
		case AVIERR_BADFLAGS: msg="AVIERR_BADFLAGS"; break;
		case AVIERR_BADPARAM: msg="AVIERR_BADPARAM"; break;
		case AVIERR_BADSIZE: msg="AVIERR_BADSIZE"; break;
		case AVIERR_BADHANDLE: msg="AVIERR_BADHANDLE"; break;
		case AVIERR_FILEWRITE: msg="AVIERR_FILEWRITE: disk error while writing file"; break;
		case AVIERR_COMPRESSOR: msg="AVIERR_COMPRESSOR"; break;
		case AVIERR_NODATA: msg="AVIERR_READONLY"; break;
		case AVIERR_BUFFERTOOSMALL: msg="AVIERR_BUFFERTOOSMALL"; break;
		case AVIERR_CANTCOMPRESS: msg="AVIERR_CANTCOMPRESS"; break;
		case AVIERR_USERABORT: msg="AVIERR_USERABORT"; break;
		case AVIERR_ERROR: msg="AVIERR_ERROR"; break;
	}
	unsigned int mlen=(unsigned int)strlen(msg);
	if (buf==0 || len==0) return mlen;
	unsigned int n=mlen; if (n+1>len) n=len-1;
	strncpy(buf,msg,n); buf[n]=0;
	return mlen;
}

mnmAviUtil::HAVI OpenAVI( const TCHAR *fn )
{
	AVIFileInit();

	TAviUtil *au = CreateTAviUtil();

	// Opens The AVI Stream
	LastError = AVIStreamOpenFromFile( &au->ps, fn, streamtypeVIDEO, 0, OF_READ, NULL);
	if( LastError != AVIERR_OK)
	{
		au->iserr=true;
		CloseAvi((HAVI)au);
		return NULL;
	}

	LastError = AVIStreamInfo( au->ps, &au->psi, sizeof(AVISTREAMINFO));				// Reads Information About The Stream Into psi
	if( LastError != AVIERR_OK)
	{
		au->iserr=true;
		CloseAvi((HAVI)au);
		return NULL;
	}

	int width = au->psi.rcFrame.right - au->psi.rcFrame.left;			// Width Is Right Side Of Frame Minus Left
	int height = au->psi.rcFrame.bottom - au->psi.rcFrame.top;			// Height Is Bottom Of Frame Minus Top

	int lastframe = AVIStreamLength( au->ps );				// The Last Frame Of The Stream

	int mpf = AVIStreamSampleToTime( au->ps, lastframe) / lastframe;		// Calculate Rough Milliseconds Per Frame

	BITMAPINFOHEADER	bmih;
	bmih.biSize		= sizeof (BITMAPINFOHEADER);		// Size Of The BitmapInfoHeader
	bmih.biPlanes		= 1;					// Bitplanes
	bmih.biBitCount		= 32;					// Bits Format We Want (32 Bit, 4 Bytes)
	bmih.biWidth		= width;					// Width We Want (256 Pixels)
	bmih.biHeight		= -height;					// Height We Want (256 Pixels)
	bmih.biCompression	= BI_RGB;				// Requested Mode = RGB

	au->pgf = AVIStreamGetFrameOpen( au->ps, (LPBITMAPINFOHEADER)AVIGETFRAMEF_BESTDISPLAYFMT );
//	au->pgf = AVIStreamGetFrameOpen( au->ps, &bmih);
	if( !au->pgf )
	{
		au->iserr=true;
		CloseAvi((HAVI)au);
		LastError = AVIERR_COMPRESSOR;
		return NULL;
	}

	au->hdc = CreateCompatibleDC(0);
	au->hdd = DrawDibOpen();
	au->hBitmap = CreateDIBSection (au->hdc, (BITMAPINFO*)(&bmih), DIB_RGB_COLORS, (void**)(&au->pdata), NULL, NULL);
	SelectObject (au->hdc, au->hBitmap);

	return (HAVI)au;
}

float GetAviLength( HAVI avi )
{
	TAviUtil *au = (TAviUtil*)avi;
	if( au && au->ps )
	{
		return (float)AVIStreamLength(au->ps);
	}
	return 0;
}

void GetAviDimensions( HAVI avi, int& o_Width, int& o_Height )
{
	TAviUtil *au = (TAviUtil*)avi;
	if( au )
	{
		o_Width = au->psi.rcFrame.right - au->psi.rcFrame.left;			// Width Is Right Side Of Frame Minus Left
		o_Height = au->psi.rcFrame.bottom - au->psi.rcFrame.top;			// Height Is Bottom Of Frame Minus Top
	}
	else
	{
		o_Width = 0;
		o_Height = 0;
	}
}

HRESULT GetAviFrame( HAVI avi, int i_Frame, unsigned char*& o_Data, int& o_Size )
{
	if (avi==NULL){ LastError = AVIERR_BADHANDLE; return LastError; }
	TAviUtil *au = (TAviUtil*)avi;
	if (au->iserr){ LastError = AVIERR_ERROR; return LastError; }
	if (!au->pgf){ LastError = AVIERR_ERROR; return LastError; }

	LPBITMAPINFOHEADER lpbi;					// Holds The Bitmap Header Information
	lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(au->pgf, i_Frame);	// Grab Data From The AVI Stream
	if( !lpbi )
	{
		au->iserr = true;
		LastError = AVIERR_NODATA;
		return LastError;
	}

	char* pdata = (char*)lpbi+lpbi->biSize+lpbi->biClrUsed * sizeof(RGBQUAD);	// Pointer To Data Returned By AVIStreamGetFrame

	// Convert Data To Requested Bitmap Format
	int width, height;
	GetAviDimensions( avi, width, height );
	SelectObject (au->hdc, au->hBitmap);
	DrawDibDraw (au->hdd, au->hdc, 0, 0, -1, -1, lpbi, pdata, 0, 0, width, height, 0);

	o_Data = au->pdata;	//set through dib device context
	o_Size = width*height*4;

	LastError = S_OK;
	return LastError;
}
}	// end of namespace

