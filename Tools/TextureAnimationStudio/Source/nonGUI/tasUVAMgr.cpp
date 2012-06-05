/********************************************************************************************\
**  tasUVAMgr.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#include "tasUVAMgr.hpp"

#include "tasUVADataParser.hpp"
#include "tasUVADataUtil.hpp"

#include "dbgLog.hpp"
#include "fsFileX.hpp"
#include "fsFileUtil.hpp"
#include "g2dDX9Types.hpp"
#include "g2dExceptionX.hpp"
#include "g2dImageCreate.hpp"
#include "g2dImageDX9.hpp"
#include "g2dImageDrawUtilDX9.hpp"
#include "itStringUtil.hpp"
#include "matTextureMgr.hpp"
#include "matUVATexture.hpp"
#include "muiMessageBox.hpp"

#include <d3dx9tex.h>
#include <ddraw.h>
//#include c_g2dD3DX9TEX_H

#undef DeleteFile


//------------------------------------------------------------------------
// Local variables and functions
//------------------------------------------------------------------------
namespace
{
	tasAnimatedTextureChangedCallback *	l_ChangedCallback = NULL;

	//matUVATexture*	l_pUVATexture;
}


//------------------------------------------------------------------------
//	Initialize()
//------------------------------------------------------------------------
void tasUVAMgr::Initialize()
{
	//l_pUVATexture = matTextureMgr::CreateUVATexture();
}

//------------------------------------------------------------------------
//	DeInitialize()
//------------------------------------------------------------------------
void tasUVAMgr::DeInitialize()
{
	//delete l_pUVATexture;
	//l_pUVATexture = 0;

	Clear();
}

//----------------------------------------------------------------------------
//	SetAnimatedTextureChangedCallback
//----------------------------------------------------------------------------
void tasUVAMgr::SetAnimatedTextureChangedCallback( tasAnimatedTextureChangedCallback *i_Callback )
{
	l_ChangedCallback = i_Callback;
}

//----------------------------------------------------------------------------
//	Think - Handle material animation timing
//----------------------------------------------------------------------------
void tasUVAMgr::Think()
{
}

//------------------------------------------------------------------------
//	Clear
//------------------------------------------------------------------------
void tasUVAMgr::Clear()
{
	// Call user callback to update interface
	//if (l_ChangedCallback)
	//	l_ChangedCallback->ModelChange();
}

//------------------------------------------------------------------------
//	SaveUVA() - Save saves a level to a given locator
//------------------------------------------------------------------------
void tasUVAMgr::SaveUVA(const fsLocator& i_Locator)
{
	//
	try
	{
		tasUVAData& data = tasUVADataUtil::Data();
		data.m_UVAFilename = i_Locator;

		tasUVADataParser::Write(i_Locator, data);
	}
	catch( const itString& /*i_String*/ )
	{
		muiMessageBox::Show("Problem saving file","Save Error");
	}
	catch( const fsReadOnlyX& )
	{
		muiMessageBox::Show("File is Read Only.  Data not saved.","Read Only Error");
	}
}

//------------------------------------------------------------------------
//	LoadUVA()	
//------------------------------------------------------------------------
void tasUVAMgr::LoadUVA(const fsLocator& i_Locator)
{
	try
	{
		tasUVAData& data = tasUVADataUtil::Data();
		data.m_UVAFilename = i_Locator;

		tasUVADataParser::Read(i_Locator, data);
	}
	catch( const itString& /*i_String*/ )
	{
		muiMessageBox::Show("Problem loading file","Load Error");
	}
	catch( const fsFileDoesntExistX& )
	{
		muiMessageBox::Show("File does not exist.","Read Error");
	}
}

//------------------------------------------------------------------------
// Will the given Image fit in the last texture page?
//------------------------------------------------------------------------
bool CheckImageInTexturePage(int width, int height)
{
	tasUVAData& data = tasUVADataUtil::Data();

	// If we haven't created the first texture page yet,
	if (data.m_TexturePages.size() <= 0) 
		return false;

	if ((width > data.m_TexturePageWidth) || (height > data.m_TexturePageWidth)) return false;

	// Can the bitmap fit in the current position?:
	if (data.m_CurX + width > data.m_TexturePageWidth)
	{
		// Drop down under lastHeight:
		data.m_CurX = 0;
		data.m_CurY += data.m_LastHeight;
	}

	// No more room in the bitmap:
	if (data.m_CurY + height > data.m_TexturePageWidth) 
	{
		return false;
	}

	// Check if it can fit completely in the most recent section
	//if ((m_LastHeight == height) && (height < 32))  // 32 = 256/4
	//{
	//}

	// Are all of the sections full?
	//if (m_SectionMask == 0xffff) return false;
	
	return true;
}


//------------------------------------------------------------------------
//	SaveImage() - this function saves an Image
//------------------------------------------------------------------------
void tasUVAMgr::SaveImage(const fsLocator& i_Locator)
{
	tasUVAData& data = tasUVADataUtil::Data();

	data.m_CurTexPage = 0;
	data.m_CurX = 0;
	data.m_CurY = 0;
	data.m_LastHeight = 0;
	data.m_ImagesInCol = 0;
	data.m_ImagesInRow = 0;

	// 1) create the image to write to
	AddUVATexturePage(i_Locator);

	g2dImage* pImage;
	try
	{
		const int l_cColor_Bits = 32;
		//g2dPFD bm_pfd( 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000, l_cColor_Bits );
		//g2dPFD hdrPFD(g2dPFD::e_RGBA16f, 16*4);
		//g2dPFD hdrPFD(g2dPFD::e_RGBA32f, l_cColor_Bits*4);	// blend won't work
		//WORKING g2dPFD pfd(g2dPFD::e_Color,l_cColor_Bits * 1);
		g2dPFD pfd(g2dPFD::e_RGBA32f,l_cColor_Bits * 4);
		pImage = g2dImageCreate::Make(data.m_TexturePageWidth, data.m_TexturePageWidth, pfd, g2dImage::e_ScratchMemory);//e_SystemMemory);
	}
	catch (g2dUnsupportedPixelFormatX&)
	{
		DBG_ASSERT0(false, "Unsupported Pixel Format");
	}

	g2dD3D9SurfacePtr pMainSrfc = dynamic_cast<g2dImageDX9*>(pImage)->GetSurface();
	D3DSURFACE_DESC ddsd1;
	pMainSrfc->GetDesc(&ddsd1);

	// 2) loop through and add each image
	int num_images = data.m_Images.size();
	try
	{
		for (int i = 0; i < num_images; ++i)
		{
			fsLocator locator = data.m_Images[i];

			g2dImage* pTxImage;
			try
			{
				pTxImage = g2dImageCreate::Load(locator);
			}
			catch (g2dUnknownImageFileTypeX&)
			{
				muiMessageBox::Show("Unsupported File Format", "Error");
				//DBG_ASSERT0(false, "Unsupported File Format");
				return;
			}
			catch (g2dUnsupportedPixelFormatX&)
			{
				muiMessageBox::Show("Unsupported Pixel Format", "Error");
				//DBG_ASSERT0(false, "Unsupported Pixel Format");
				return;
			}
			catch (fsFileDoesntExistX&)
			{
				muiMessageBox::Show("File Doesn't Exist", "Error");
				//DBG_ASSERT0(false, "File Doesn't Exist");
				return;
			}

			//	put the image into the main image
			g2dD3D9SurfacePtr pSrfc = dynamic_cast<g2dImageDX9*>(pTxImage)->GetSurface();

			// Get size of surface:
			D3DSURFACE_DESC ddsd;
			pSrfc->GetDesc(&ddsd);
			DWORD width  = ddsd.Width; //(m_bDoScaling) ? m_GoalSizeX : ddsd.dwWidth;
			DWORD height = ddsd.Height; //(m_bDoScaling) ? m_GoalSizeY : ddsd.dwHeight;

			//DBG_LOG2("ddsd w=%d h=%d", ddsd.Width, ddsd.Height );

			// Check for too big for one texture page
			if ((width > data.m_TexturePageWidth) || (height > data.m_TexturePageWidth))
			{
				muiMessageBox::Show("Texture too big for Texture Page", "Error");
				return;
			}

			// Can this size image fit in the texture page?
			if (!CheckImageInTexturePage(width, height))
			{
				muiMessageBox::Show("Texture too big for Texture Page", "Error");
				delete pTxImage;
				return;
			}

			//	calculate the images in the row/col
			if (data.m_ImagesInRow == 0)
			{
				data.m_ImagesInRow = (data.m_TexturePageWidth / width);
				data.m_ImagesInCol = data.m_ImagesInRow;	// for now, make col same as row
				
				//DBG_LOG2("images rowxcol = %d x %d", data.m_ImagesInRow, data.m_ImagesInCol);
			}

			//
			// Make sure this surface is restored. (bga - necessary?)
			//
			RECT rectDest;
			rectDest.left	= data.m_CurX;
			rectDest.top	= data.m_CurY;
			rectDest.right  = data.m_CurX + width;
			rectDest.bottom = data.m_CurY + height;

			//DBG_LOG3( "right  %d + %d = %d", width, data.m_CurX, rectDest.right );
			//DBG_LOG3( "bottom %d + %d = %d", height, data.m_CurY, rectDest.bottom );

			RECT srcrect;
			srcrect.top = 0; srcrect.left = 0; srcrect.bottom = height; srcrect.right = width;
			POINT destpt;
			destpt.x = data.m_CurX;
			destpt.y = data.m_CurY;
			HRESULT result = g2dImageDrawUtilDX9::UpdateSurface( pSrfc, &srcrect, pMainSrfc, &destpt );

			float u0, u1, v0, v1;
			u0 = (float)rectDest.left / (float)data.m_TexturePageWidth;
			v0 = (float)rectDest.top / (float)data.m_TexturePageWidth;
			u1 = (float)rectDest.right / (float)data.m_TexturePageWidth;
			v1 = (float)rectDest.bottom / (float)data.m_TexturePageWidth;

			DBG_LOG1("texture page width = %d", data.m_TexturePageWidth);
			DBG_LOG5("UV%d [L%d, R%d, T%d, B%d]", data.m_CurTexPage, rectDest.left, rectDest.right, rectDest.top, rectDest.bottom );
			DBG_LOG4("uv0(%6.3f,%6.3f) uv1(%6.3f,%6.3f)", u0,v0,u1,v1);
			
			data.m_TexturePages[data.m_CurTexPage].m_FrameLocs.push_back(u0);
			data.m_TexturePages[data.m_CurTexPage].m_FrameLocs.push_back(v0);
			data.m_TexturePages[data.m_CurTexPage].m_FrameLocs.push_back(u1);
			data.m_TexturePages[data.m_CurTexPage].m_FrameLocs.push_back(v1);

			// Move our current X and Y Position along to next position:
			data.m_CurX += width;	

			// Update most recent height to be what we just Blit:
			data.m_LastHeight = height;

			//	dispose of the image
			delete pTxImage;
		}
	}
	catch( const itString& /*i_String*/ )
	{
		muiMessageBox::Show("Problem saving file","Save Error");
	}
	catch( const fsReadOnlyX& )
	{
		muiMessageBox::Show("File is Read Only.  Data not saved.","Read Only Error");
	}

	//	3) write out the texture
	//
	std::string destfilename;
	fsFileUtil::LocatorToANSIFilename(i_Locator, destfilename);

	if( fsFileUtil::FileExists(i_Locator) )
	{
		try
		{
			fsFileUtil::DeleteFile(i_Locator);
		}
		catch (fsFileInUseX& i_Ex)
		{
			DBG_ERROR1("Cannot delete file (%s)", destfilename.c_str());
			muiMessageBox::Show("Cannot delete BMP before writing, file is in use", "Error");
		}
	}

	//DBG_LOG1("Writing Image [%s]", destfilename.c_str());
	D3DXIMAGE_FILEFORMAT iif = D3DXIFF_BMP;
	if (data.m_ImageFormat == std::string("BMP"))
		iif = D3DXIFF_BMP;
	else if (data.m_ImageFormat == std::string("PNG"))
		iif = D3DXIFF_PNG;
	else if (data.m_ImageFormat == std::string("DDS"))
		iif = D3DXIFF_DDS;
	else if (data.m_ImageFormat == std::string("JPG"))
		iif = D3DXIFF_JPG;
	else if (data.m_ImageFormat == std::string("TGA"))
		iif = D3DXIFF_TGA;

	HRESULT saveresult = D3DXSaveSurfaceToFile(destfilename.c_str(), iif, pMainSrfc, NULL, NULL);
	if (saveresult == D3DERR_INVALIDCALL)
	{
		DBG_ASSERT0(false, "Invalid call when saving image");
	}
	else if (saveresult != D3D_OK)
	{
		DBG_ASSERT0(false, "Save of image did not work");
	}

	//	clean-up
	//
	delete pImage;	// also releases pMainSrc
}

//------------------------------------------------------------------------
//	LoadImage()
//------------------------------------------------------------------------
//void tasUVAMgr::LoadImage(const fsLocator& i_Locator)
//{
//	// is this function needed?
//	//
//	try
//	{
//	}
//	catch( const itString& /*i_String*/ )
//	{
//		muiMessageBox::Show("Problem loading file","Load Error");
//	}
//	catch( const fsFileDoesntExistX& )
//	{
//		muiMessageBox::Show("File does not exist.","Read Error");
//	}
//}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tasUVAMgr::AddUVAImage(const fsLocator& i_TFilename)
{
	tasUVAData& data = tasUVADataUtil::Data();
	int index = data.m_Images.size();
	data.m_Images.resize(index+1);
	data.m_Images[index] = i_TFilename;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tasUVAMgr::ClearUVAImages()
{
	tasUVAData& data = tasUVADataUtil::Data();
	data.m_Images.clear();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tasUVAMgr::AddUVATexturePage(const fsLocator& i_TFilename)
{
	tasUVAData& data = tasUVADataUtil::Data();
	int index = data.m_TexturePages.size();
	data.m_TexturePages.resize(index+1);
	data.m_TexturePages[index].m_Filename = i_TFilename;
	data.m_NumberOfTexturePages = data.m_TexturePages.size();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tasUVAMgr::ClearUVATexturePages()
{
	tasUVAData& data = tasUVADataUtil::Data();
	data.m_TexturePages.clear();
	data.m_NumberOfTexturePages = 0;
}

