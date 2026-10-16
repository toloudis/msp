/****************************************************************************\
**  fsAviFilePACWin.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/
#include "Core/fs/private/fsAviFilePACWin.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsLocator.hpp"

#include <filesystem>
#include <fstream>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vfw.h>

#undef CreateFile
#undef DeleteFile
#undef CreateDirectory
#undef CopyFile


//==============================================================================
//	library pragmas
//==============================================================================
#pragma comment(lib,"Vfw32.lib")


//==============================================================================
// This is the internal structure represented by the Handle:
//==============================================================================
struct fsAviFile::File
{
	IAVIFile *pfile;				// created by Create
	WAVEFORMATEX wfx;				// .nChannels=0 until audio is added. Used when audio stream is first created.
	int period;						// specified in Create, used when the video stream is first created
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
};


//==============================================================================
//==============================================================================
namespace fsAviFilePAC
{

namespace
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

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
fsAviFile::File* create_file()
{
	fsAviFile::File* avi = new fsAviFile::File;
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
	avi->hdc = NULL;
	avi->hBitmap = NULL;
	return avi;
}

//----------------------------------------------------------------------------
//	The header of an uncompressed, bottom-up frame.
//----------------------------------------------------------------------------
BITMAPINFOHEADER frame_header(int i_Width, int i_Height, int i_BitsPerPixel, int i_ImageSize)
{
	BITMAPINFOHEADER header;
	ZeroMemory(&header, sizeof(header));
	header.biSize			= sizeof( BITMAPINFOHEADER );
	header.biWidth			= i_Width;
	header.biHeight			= i_Height;
	header.biPlanes			= 1;
	header.biBitCount		= (unsigned short)i_BitsPerPixel;
	header.biCompression	= BI_RGB;
	header.biSizeImage		= i_ImageSize;
	header.biXPelsPerMeter	= i_Width;
	header.biYPelsPerMeter	= i_Height;
	header.biClrUsed		= 0;
	header.biClrImportant	= 0;
	return header;
}

//----------------------------------------------------------------------------
//	Create the video stream, if it isn't there yet
//----------------------------------------------------------------------------
HRESULT create_video_stream(fsAviFile::File* au, const BITMAPINFOHEADER* i_pFormat)
{
	if (au->ps!=0)
		return AVIERR_OK;

	AVISTREAMINFO strhdr; ZeroMemory(&strhdr,sizeof(strhdr));
	strhdr.fccType = streamtypeVIDEO;// stream type
	strhdr.fccHandler = 0;
	strhdr.dwScale = au->period;
	strhdr.dwRate = 1000;
	if (i_pFormat)
	{
		strhdr.dwSuggestedBufferSize  = i_pFormat->biSizeImage;
		SetRect(&strhdr.rcFrame, 0, 0, i_pFormat->biWidth, i_pFormat->biHeight);
	}
	HRESULT result = AVIFileCreateStream(au->pfile, &au->ps, &strhdr);
	if (result!=AVIERR_OK) au->iserr=true;
	return result;
}

//----------------------------------------------------------------------------
//	Create the audio stream, if it isn't there yet
//----------------------------------------------------------------------------
HRESULT create_audio_stream(fsAviFile::File* au)
{
	if (au->as!=0)
		return AVIERR_OK;

	AVISTREAMINFO ahdr; ZeroMemory(&ahdr,sizeof(ahdr));
	ahdr.fccType=streamtypeAUDIO;
	ahdr.dwScale=au->wfx.nBlockAlign;
	ahdr.dwRate=au->wfx.nSamplesPerSec*au->wfx.nBlockAlign;
	ahdr.dwSampleSize=au->wfx.nBlockAlign;
	ahdr.dwQuality=(DWORD)-1;
	HRESULT result = AVIFileCreateStream(au->pfile, &au->as, &ahdr);
	if (result!=AVIERR_OK) {au->iserr=true; return result;}
	LONG pos = 0;
	result = AVIStreamSetFormat(au->as,pos,&au->wfx,sizeof(WAVEFORMATEX));
	if (result!=AVIERR_OK) au->iserr=true;
	return result;
}

//----------------------------------------------------------------------------
//	The file name of a locator, for the Video for Windows functions
//----------------------------------------------------------------------------
itString filename(const fsLocator& i_Locator)
{
	itString name;
	fsFileUtil::LocatorToUnicodeString(i_Locator, name);
	return name;
}

} // anonymous namespace


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
Handle Create(const fsLocator& i_Filename, int i_FramePeriod)
{
	itString fn = filename(i_Filename);
	IAVIFile *pfile;
	AVIFileInit();
	LastError = AVIFileOpenW(&pfile, fn.GetString(), OF_WRITE|OF_CREATE, NULL);
	if (LastError!=AVIERR_OK)
	{
		AVIFileExit();
		DBG_ERROR("Could not create AVI (" << fn << " Error code (" << LastError << ")");
		return NULL;
	}
	fsAviFile::File *au = create_file();
	au->pfile = pfile;
	au->period = i_FramePeriod;
	return au;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool Close(Handle avi)
{
	if (avi==NULL){ LastError = AVIERR_BADHANDLE; return false; }
	fsAviFile::File *au = avi;
	if (au->as!=0) AVIStreamRelease(au->as); au->as=0;
	if (au->psCompressed!=0) AVIStreamRelease(au->psCompressed); au->psCompressed=0;
	if (au->ps!=0) AVIStreamRelease(au->ps); au->ps=0;
	if (au->pfile!=0) AVIFileRelease(au->pfile); au->pfile=0;
	if( au->pgf != 0) AVIStreamGetFrameClose(au->pgf);
	if( au->hdd != 0) DrawDibClose(au->hdd);
	if( au->hBitmap != 0) DeleteObject(au->hBitmap);
	if( au->hdc != 0) DeleteDC(au->hdc);
	AVIFileExit();
	delete au;

	LastError = S_OK;
	return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool SetVideoCompression(Handle avi, const char* i_FourCC,
						 bool i_ShowDialog, void* i_ParentWindow,
						 int i_Width, int i_Height, int i_BitsPerPixel,
						 int i_ImageSize)
{
	if (avi==NULL){ LastError = AVIERR_BADHANDLE; return false; }
	if (i_FourCC==NULL){ LastError = AVIERR_BADPARAM; return false; }
	fsAviFile::File *au = avi;
	if (au->iserr){ LastError = AVIERR_ERROR; return false; }
	if (au->psCompressed!=0){ LastError = AVIERR_COMPRESSOR; return false; }

	BITMAPINFOHEADER format = frame_header(i_Width, i_Height, i_BitsPerPixel, i_ImageSize);
	LastError = create_video_stream(au, &format);
	if (LastError!=AVIERR_OK) return false;

	// set the compression, prompting dialog if necessary
	AVICOMPRESSOPTIONS opts; ZeroMemory(&opts,sizeof(opts));
	opts.fccHandler=mmioFOURCC( i_FourCC[0], i_FourCC[1], i_FourCC[2], i_FourCC[3] );
	AVICOMPRESSOPTIONS *aopts[1] = { &opts };
	if (i_ShowDialog)
	{
		BOOL res = (BOOL)AVISaveOptions((HWND)i_ParentWindow,0,1,&au->ps,aopts);
		if (!res) {AVISaveOptionsFree(1,aopts); au->iserr=true; LastError = AVIERR_USERABORT; return false; }
	}
	CoInitialize(NULL);
	LastError = AVIMakeCompressedStream(&au->psCompressed, au->ps, aopts[0], NULL);
	AVISaveOptionsFree(1,aopts);
	if (LastError != AVIERR_OK) {CoUninitialize(); au->iserr=true; return false;}
	LastError = AVIStreamSetFormat(au->psCompressed, 0, &format, format.biSize+format.biClrUsed*sizeof(RGBQUAD));
	CoUninitialize();
	if (LastError!=AVIERR_OK) {au->iserr=true; return false;}

	LastError = AVIERR_OK;
	return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool Append(Handle avi1, Handle avi2, const fsLocator& i_SavePath)
{
	if (avi1==NULL){ LastError = AVIERR_BADHANDLE; return false; }
	if (avi2==NULL){ LastError = AVIERR_BADHANDLE; return false; }
	fsAviFile::File *au1 = avi1;
	fsAviFile::File *au2 = avi2;
	PAVISTREAM pEditableStream = NULL;
	if (au1->iserr){ LastError = AVIERR_ERROR; return false; }
	if (au2->iserr){ LastError = AVIERR_ERROR; return false; }
	long Length  = 1;
	long Buf;
	CreateEditableStream(&pEditableStream,NULL);

	LastError = create_video_stream(au1, NULL);
	if (LastError!=AVIERR_OK) return false;
	LastError = create_video_stream(au2, NULL);
	if (LastError!=AVIERR_OK) return false;

	LastError = EditStreamPaste(pEditableStream, &Length, &Buf, au1->ps, 1, -1);
	if (LastError!=AVIERR_OK) {au1->iserr=true; return false;}

	Length = (long)fsAviFilePAC::GetLength(avi1);
	long buf1;
	LastError = EditStreamPaste(pEditableStream, &Length, &buf1, au2->ps, 1, -1);
	if (LastError!=AVIERR_OK) {au1->iserr=true; return false;}

	AVICOMPRESSOPTIONS aco;
	ZeroMemory(&aco, sizeof(AVICOMPRESSOPTIONS));
	aco.fccType = au2->psi.fccType;
	aco.fccHandler = au2->psi.fccHandler;
	aco.dwKeyFrameEvery = 1;
	aco.dwQuality = au2->psi.dwQuality;

	itString save_path = filename(i_SavePath);
	AVISaveW(save_path.GetString(), NULL, NULL, 1, pEditableStream, &aco);

	LastError = S_OK;
	return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool AddFrame(Handle avi, const envType::UInt8* i_Pixels, int i_ImageSize,
			  int i_Width, int i_Height, int i_BitsPerPixel)
{
	if (avi==NULL){ LastError = AVIERR_BADHANDLE; return false; }
	if (i_Pixels==NULL){ LastError = AVIERR_BADPARAM; return false; }
	fsAviFile::File *au = avi;
	if (au->iserr){ LastError = AVIERR_ERROR; return false; }

	BITMAPINFOHEADER format = frame_header(i_Width, i_Height, i_BitsPerPixel, i_ImageSize);
	LastError = create_video_stream(au, &format);
	if (LastError!=AVIERR_OK) return false;

	// create an empty compression, if the user hasn't set any
	if (au->psCompressed==0)
	{
		AVICOMPRESSOPTIONS opts; ZeroMemory(&opts,sizeof(opts));
		opts.fccHandler=mmioFOURCC('D','I','B',' ');
		CoInitialize(NULL);
		LastError = AVIMakeCompressedStream(&au->psCompressed, au->ps, &opts, NULL);
		CoUninitialize();
		if (LastError != AVIERR_OK) {au->iserr=true; return false;}
		LastError = AVIStreamSetFormat(au->psCompressed, 0, &format, format.biSize+format.biClrUsed*sizeof(RGBQUAD));
		if (LastError!=AVIERR_OK) {au->iserr=true; return false;}
	}

	//Now we can add the frame
	LastError = AVIStreamWrite(au->psCompressed, au->nframe, 1, (void*)i_Pixels, i_ImageSize, AVIIF_KEYFRAME, NULL, NULL);
	if (LastError!=AVIERR_OK) {au->iserr=true; return false;}
	au->nframe++;
	LastError = S_OK;
	return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool AddWav(Handle avi, const fsLocator& i_WavFile)
{
	if (avi==NULL){ LastError = AVIERR_BADHANDLE; return false; }
	fsAviFile::File *au = avi;
	if (au->iserr){ LastError = AVIERR_ERROR; return false; }

	std::ifstream file(std::filesystem::path(filename(i_WavFile).GetString()), std::ios::binary | std::ios::ate);
	if (!file)
	{
		au->iserr=true;
		LastError =AVIERR_FILEOPEN;
		return false;
	}
	std::vector<char> buf((size_t)file.tellg());
	file.seekg(0);
	file.read(buf.data(), buf.size());
	const WavChunk *wav = (const WavChunk*)buf.data();

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
	if (badformat) { LastError = AVIERR_BADFORMAT; return false; }

	LastError = create_audio_stream(au);
	if (LastError!=AVIERR_OK) return false;

	// now we can write the data
	unsigned long numbytes = wav->dat.size;
	unsigned long numsamps = numbytes*8 / au->wfx.wBitsPerSample;
	DWORD aviflags = 0;
	LastError = AVIStreamWrite(au->as,au->nsamp,numsamps,(void*)wav->dat.data,numbytes,aviflags,NULL,NULL);
	if (LastError!=AVIERR_OK) {au->iserr=true; return false;}
	au->nsamp+=numsamps;
	LastError = S_OK;
	return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
std::string GetLastErrorMessage()
{
	const char *msg="unknown avi result code";
	switch (LastError)
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
		case AVIERR_NODATA: msg="AVIERR_NODATA"; break;
		case AVIERR_BUFFERTOOSMALL: msg="AVIERR_BUFFERTOOSMALL"; break;
		case AVIERR_CANTCOMPRESS: msg="AVIERR_CANTCOMPRESS"; break;
		case AVIERR_USERABORT: msg="AVIERR_USERABORT"; break;
		case AVIERR_ERROR: msg="AVIERR_ERROR"; break;
	}
	return msg;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
Handle Open(const fsLocator& i_Filename)
{
	itString fn = filename(i_Filename);
	AVIFileInit();

	fsAviFile::File *au = create_file();

	// Opens The AVI Stream
	LastError = AVIStreamOpenFromFileW( &au->ps, fn.GetString(), streamtypeVIDEO, 0, OF_READ, NULL);
	if( LastError != AVIERR_OK)
	{
		HRESULT error = LastError;
		au->iserr=true;
		fsAviFilePAC::Close(au);
		LastError = error;
		return NULL;
	}

	LastError = AVIStreamInfo( au->ps, &au->psi, sizeof(AVISTREAMINFO));				// Reads Information About The Stream Into psi
	if( LastError != AVIERR_OK)
	{
		HRESULT error = LastError;
		au->iserr=true;
		fsAviFilePAC::Close(au);
		LastError = error;
		return NULL;
	}

	int width = au->psi.rcFrame.right - au->psi.rcFrame.left;			// Width Is Right Side Of Frame Minus Left
	int height = au->psi.rcFrame.bottom - au->psi.rcFrame.top;			// Height Is Bottom Of Frame Minus Top

	BITMAPINFOHEADER	bmih;
	ZeroMemory(&bmih, sizeof(bmih));
	bmih.biSize		= sizeof (BITMAPINFOHEADER);		// Size Of The BitmapInfoHeader
	bmih.biPlanes		= 1;					// Bitplanes
	bmih.biBitCount		= 32;					// Bits Format We Want (32 Bit, 4 Bytes)
	bmih.biWidth		= width;
	bmih.biHeight		= -height;				// top-down rows
	bmih.biCompression	= BI_RGB;				// Requested Mode = RGB

	au->pgf = AVIStreamGetFrameOpen( au->ps, (LPBITMAPINFOHEADER)AVIGETFRAMEF_BESTDISPLAYFMT );
	if( !au->pgf )
	{
		au->iserr=true;
		fsAviFilePAC::Close(au);
		LastError = AVIERR_COMPRESSOR;
		return NULL;
	}

	au->hdc = CreateCompatibleDC(0);
	au->hdd = DrawDibOpen();
	au->hBitmap = CreateDIBSection (au->hdc, (BITMAPINFO*)(&bmih), DIB_RGB_COLORS, (void**)(&au->pdata), NULL, NULL);
	SelectObject (au->hdc, au->hBitmap);

	return au;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float GetLength( Handle avi )
{
	fsAviFile::File *au = avi;
	if( au && au->ps )
	{
		return (float)AVIStreamLength(au->ps);
	}
	return 0;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void GetDimensions( Handle avi, int& o_Width, int& o_Height )
{
	fsAviFile::File *au = avi;
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

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool GetFrame( Handle avi, int i_Frame, envType::UInt8*& o_Data, int& o_Size )
{
	if (avi==NULL){ LastError = AVIERR_BADHANDLE; return false; }
	fsAviFile::File *au = avi;
	if (au->iserr){ LastError = AVIERR_ERROR; return false; }
	if (!au->pgf){ LastError = AVIERR_ERROR; return false; }

	LPBITMAPINFOHEADER lpbi;					// Holds The Bitmap Header Information
	lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(au->pgf, i_Frame);	// Grab Data From The AVI Stream
	if( !lpbi )
	{
		au->iserr = true;
		LastError = AVIERR_NODATA;
		return false;
	}

	char* pdata = (char*)lpbi+lpbi->biSize+lpbi->biClrUsed * sizeof(RGBQUAD);	// Pointer To Data Returned By AVIStreamGetFrame

	// Convert Data To Requested Bitmap Format
	int width, height;
	fsAviFilePAC::GetDimensions( avi, width, height );
	SelectObject (au->hdc, au->hBitmap);
	DrawDibDraw (au->hdd, au->hdc, 0, 0, -1, -1, lpbi, pdata, 0, 0, width, height, 0);

	o_Data = au->pdata;	//set through dib device context
	o_Size = width*height*4;

	LastError = S_OK;
	return true;
}

}	// end of namespace
