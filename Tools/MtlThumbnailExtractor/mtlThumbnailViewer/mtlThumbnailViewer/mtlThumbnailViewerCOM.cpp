// ***************************************************************************
// mtlThumbnailViewerCOM.cpp : Implementation of CmtlThumbnailViewerCOM
//
//	This is the code the dll will run to extract the image data from a .mtl file
//  To load this dll and have it extracted, build the project in Release (smaller file)
//  Make sure the registry has been created, execute the mtlThumbnailViewer.reg file
//  Place the dll file into the registry specified location (currently, C:\Projects)
//
// ***************************************************************************
#include "stdafx.h"
#include "resource.h"

#include "mtlThumbnailViewerCOM.h"

#include "Core/ch/chBinReader.hpp"
#include "Core/fs/fsFileStream.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"

#include <cassert>
// CmtlThumbnailViewerCOM
//----------------------------------------------------------------------------
//
// GetLocation - determines how to grab the thumbnail image, from the dll cache, or from
//				 the image file itself.
//
//----------------------------------------------------------------------------
STDMETHODIMP CmtlThumbnailViewerCOM::GetLocation(LPWSTR pszPathBuffer,
													DWORD cchMax,
													DWORD *pdwPriority,
													const SIZE *prgSize,
													DWORD dwRecClrDepth,
													DWORD *pdwFlags)
{
	//if the image is cached, use the cached data, else pull the data from the mtl file
	*pdwFlags &= IEIFLAG_CACHE;
	*pdwFlags &= IEIFLAG_REFRESH;
	if (*pdwFlags & IEIFLAG_ASYNC) 
	{
		return E_PENDING;
	} 
	return NOERROR;
}

//----------------------------------------------------------------------------
//
// Extract - loads the mtl file and reads the THMB chunk to grab out the bitmap data
//			 If no data found (old mtl file) then the data used is from a default bitmap
//			 When we have data, we load the data into an HBITMAP and pass it out
//
//----------------------------------------------------------------------------
STDMETHODIMP CmtlThumbnailViewerCOM::Extract(HBITMAP* o_Thumbnail)
{
	// Load the icons!
	int bitmapSize = 128;
	fsLocator* locator = new fsLocator();
	char* file = new char[MAX_PATH]; 
	wcstombs(file, m_szFilename,MAX_PATH );
	fsFileUtil::UnicodeStringToLocator(itString(file), *locator);
	
	//locator.Push(itString(file));
	//fsFileStream mtl_fs(locator, fsFileStream::e_ReadOnly);
	gfFileBin* doc_file = new gfFileBin(*locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);
	gfFileBin::Header header;
	doc_file->ReadHeader(&header);
	// could check version here?
	chBinReader* reader = new chBinReader(*doc_file);
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;
	const chDefs::Name c_THMB = chDefs::MakeName('T', 'H', 'M', 'B');	
	char* bmpData = NULL;
	char* bmpHeader = NULL;
	int image_size = 0;
	while( reader->ReadChunkHeader(name, version, size) )
	{
		if (name == c_THMB)
		{
			BITMAPFILEHEADER bfh = {0};
			int image_offset = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
			image_size = size - image_offset;
			
			bmpHeader = new char[image_offset];
			bmpData = new char[image_size];

			reader->Read(bmpHeader, image_offset);
			reader->Read(bmpData, image_size);
		}
		reader->FinishChunk();
	}
	delete[] file;
	//delete locator;
	//delete doc_file;
	//delete reader;
	//if no bmp data was extracted, just load up the default bmp image
	if(bmpData == NULL)
	{
		HBITMAP thumb;
		HMODULE myDll = _AtlBaseModule.GetModuleInstance();
		thumb = LoadBitmap(myDll, MAKEINTRESOURCE(IDB_BITMAP_SGPU));
		*o_Thumbnail = thumb;

		return S_OK;
	}
	
	//Now we must convert the char* data into Bitmap data
	HBITMAP hBitmap = NULL;
	HDC hDC;
	BITMAPFILEHEADER bmfh;
	BITMAPINFO *pbmi;
	DWORD dwColorTableSize;
	void *pbmiBits;

	memcpy(&bmfh, bmpHeader, sizeof(BITMAPFILEHEADER));
	dwColorTableSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);
	pbmiBits = new char[sizeof(BITMAPINFOHEADER)];
	memcpy(pbmiBits, bmpHeader, sizeof(BITMAPINFOHEADER));

	//manually tell explorer how the image should look
	pbmi = (BITMAPINFO*)pbmiBits;
	pbmi->bmiHeader.biClrImportant = 0;
	pbmi->bmiHeader.biBitCount = 24;
    // Use as many colors according to bits per pixel
    pbmi->bmiHeader.biClrUsed = 0;
    // Store as un Compressed
    pbmi->bmiHeader.biCompression = BI_RGB;
    // Set the height in pixels
    pbmi->bmiHeader.biHeight = bitmapSize;
    // Width of the Image in pixels
    pbmi->bmiHeader.biWidth = bitmapSize;
    // Default number of planes
    pbmi->bmiHeader.biPlanes = 1;
    // the image size
    pbmi->bmiHeader.biSizeImage = image_size;
	//structure size
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biXPelsPerMeter = bitmapSize;
	pbmi->bmiHeader.biYPelsPerMeter = bitmapSize;

	pbmiBits = new char[sizeof(BITMAPINFOHEADER) + dwColorTableSize];

	hDC = ::CreateCompatibleDC(NULL);
	//Using CreateDIBSection()
	void *ppvBits;
	hBitmap = ::CreateDIBSection(NULL, pbmi, DIB_RGB_COLORS, &ppvBits, NULL, 0);
	::SetDIBits(hDC, hBitmap, 0, pbmi->bmiHeader.biHeight, bmpData, pbmi, DIB_RGB_COLORS);
	::DeleteDC(hDC);
	delete[] bmpHeader;
	delete[] pbmiBits;
	delete[] pbmi;
	delete[] bmpData;
	
	*o_Thumbnail = hBitmap;
	return S_OK;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
STDMETHODIMP CmtlThumbnailViewerCOM::GetDateStamp(FILETIME *pDateStamp)
{
	FILETIME ftCreationTime,ftLastAccessTime,ftLastWriteTime;
	// open the file and get last write time
	HANDLE hFile = CreateFile(m_szFilename,GENERIC_READ,FILE_SHARE_READ,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(!hFile) 
		return E_FAIL;
	GetFileTime(hFile,&ftCreationTime,&ftLastAccessTime,&ftLastWriteTime);
	CloseHandle(hFile);
	*pDateStamp = ftLastWriteTime;
	return NOERROR; 
}