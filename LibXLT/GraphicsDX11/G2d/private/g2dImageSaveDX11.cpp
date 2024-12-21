/*****************************************************************************
**  g2dImageSaveDX11.cpp
**
**      g2dImageSaveDX11 contains the windows implementation of the
**	g2dImageSave.
**
**	http://msdn.microsoft.com/archive/default.asp?url=/archive/en-us/directx9_c_Feb_2006/D3DXSaveSurfaceToFile.asp
** 
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g2d/private/g2dImageSaveDX11.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g2d/g2dHDRUtil.hpp"
#include "Graphics/g2d/g2dScanlineOpenEXRUtil.hpp"
#include "Graphics/g2d/g2dTGAUtil.hpp"
#include "Graphics/g2d/g2dTiledOpenEXRUtil.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/g2dImageDX11.hpp"
#include "GraphicsDX11/g2d/g2dWindowDX11.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"

#include <DirectXTex.h>
#include <wincodec.h>

#include <string>

namespace
{
    enum FileType {
        e_BMP,
        e_JPG,
        e_TGA,
        e_PNG,
        e_DDS,
        e_PPM,
        e_DIB,
        e_HDR,
        e_PFM,
        e_EXR,
        e_TIFF
    };
//------------------------------------------------------------------------
//------------------------------------------------------------------------
FileType pick_format(const fsLocator& i_FileName)
{
	//	decide what kind of file we have
	itString fname = i_FileName.GetLastName();
	itString ext;
	fname.GetExtension(ext);

	if ((ext == itString("bmp")) || (ext == itString("BMP")))
	{
		return e_BMP;
	}
	else if ((ext == itString("jpg")) || (ext == itString("JPG")) || (ext == itString("JPEG")) || (ext == itString("jpeg")))
	{
		return e_JPG;
	}
	else if ((ext == itString("tga")) || (ext == itString("TGA")))
	{
		return e_TGA;
	}
	else if ((ext == itString("png")) || (ext == itString("PNG")))
	{
		return e_PNG;
	}
	else if ((ext == itString("dds")) || (ext == itString("DDS")))
	{
		return e_DDS;
	}
	else if ((ext == itString("ppm")) || (ext == itString("PPM")))
	{
		return e_PPM;
	}
	else if ((ext == itString("dib")) || (ext == itString("DIB")))
	{
		return e_DIB;
	}
	else if ((ext == itString("hdr")) || (ext == itString("HDR")))
	{
		return e_HDR;
	}
	else if ((ext == itString("pfm")) || (ext == itString("PFM")))
	{
		return e_PFM;
	}
	else if ((ext == itString("exr")) || (ext == itString("EXR")))
	{
		return e_EXR;
	}
	else if ((ext == itString("tif")) || (ext == itString("TIF")) || (ext == itString("Tif")))
	{
		return e_TIFF;
	}
	else if ((ext == itString("tiff")) || (ext == itString("TIFF")) || (ext == itString("Tiff")))
	{
		return e_TIFF;
	}
	throw g2dUnknownImageFileTypeX();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
FileType pick_format(const std::string& i_FileName)
{
	//	decide what kind of file we have
	//
	int index = i_FileName.find_last_of(".");
	DBG_ASSERT( index != std::string::npos, "Could not find . in filename" );

	std::string ext;
	ext = i_FileName.substr(index+1, i_FileName.size()-index);

	if ((ext == std::string("bmp")) || (ext == std::string("BMP")))
	{
		return e_BMP;
	}
	else if ((ext == std::string("jpg")) || (ext == std::string("JPG")) || (ext == std::string("JPEG")) || (ext == std::string("jpeg")))
	{
		return e_JPG;
	}
	else if ((ext == std::string("tif")) || (ext == std::string("TIF")) || (ext == std::string("TIFF")) || (ext == std::string("tiff")))
	{
		return e_TIFF;
	}
	else if ((ext == std::string("tga")) || (ext == std::string("TGA")))
	{
		return e_TGA;
	}
	else if ((ext == std::string("png")) || (ext == std::string("PNG")))
	{
		return e_PNG;
	}
	else if ((ext == std::string("dds")) || (ext == std::string("DDS")))
	{
		return e_DDS;
	}
	else if ((ext == std::string("ppm")) || (ext == std::string("PPM")))
	{
		return e_PPM;
	}
	else if ((ext == std::string("dib")) || (ext == std::string("DIB")))
	{
		return e_DIB;
	}
	else if ((ext == std::string("hdr")) || (ext == std::string("HDR")))
	{
		return e_HDR;
	}
	else if ((ext == std::string("pfm")) || (ext == std::string("PFM")))
	{
		return e_PFM;
	}
	else if ((ext == std::string("exr")) || (ext == std::string("EXR")))
	{
		return e_EXR;
	}
	else if ((ext == std::string("tif")) || (ext == std::string("TIF")) || (ext == std::string("Tif")))
	{
		return e_TIFF;
	}
	else if ((ext == std::string("tiff")) || (ext == std::string("TIFF")) || (ext == std::string("Tiff")))
	{
		return e_TIFF;
	}
	throw g2dUnknownImageFileTypeX();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
DirectX::WICCodecs get_d3d_file_format( FileType i_Type )
{
	switch( i_Type )
	{
		case e_BMP:
			return DirectX::WICCodecs::WIC_CODEC_BMP;
		case e_JPG:
			return DirectX::WICCodecs::WIC_CODEC_JPEG;
		case e_PNG:
			return DirectX::WICCodecs::WIC_CODEC_PNG;
		case e_TIFF:
			return DirectX::WICCodecs::WIC_CODEC_TIFF;
        case e_DDS:
        case e_TGA:
		case e_PPM:
		case e_DIB:
		case e_HDR:
		case e_PFM:
			throw g2dUnknownImageFileTypeX();
	}

	throw g2dUnknownImageFileTypeX();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void SaveToFile(const itString& i_Filename, const g2dD3D11TexturePtr i_pSurface, DirectX::WICCodecs i_Format)
{
    DirectX::ScratchImage s;
    HRESULT hr = DirectX::CaptureTexture(g2dDX11Global::g_pDevice, g2dDX11Global::g_pDeviceContext, i_pSurface, s);
    if (!SUCCEEDED(hr))
    {
        DBG_TRACE("D3DX11SaveTextureToFile Return Error result:" << hr);
        throw g2dImageSaveX();
    }

    const DirectX::Image* img = s.GetImage(0, 0, 0);
    assert(img);
    hr = DirectX::SaveToWICFile(*img, DirectX::WIC_FLAGS::WIC_FLAGS_NONE,
        DirectX::GetWICCodec(i_Format), i_Filename.GetString());
    if (!SUCCEEDED(hr))
    {
        DBG_TRACE("D3DX11SaveTextureToFile Return Error result:" << hr);
        throw g2dImageSaveX();
    }
}


//----------------------------------------------------------------------------
// capture back buffer to surface in memory
// the returned value must be released() by the caller.
//----------------------------------------------------------------------------
g2dD3D11TexturePtr capture_back_buffer(g2dWindowDX11 *i_pWindow)
{
	HRESULT op_result;

	int width=0, height=0;
	i_pWindow->GetDimensions(width, height);

	g2dD3D11RenderTargetPtr back_buffer = i_pWindow->GetBackBuffer();
	DBG_ASSERT(back_buffer, "error getting back buffer");

	D3D11_RENDER_TARGET_VIEW_DESC bbdesc;
	back_buffer->GetDesc(&bbdesc);

	g2dD3D11TexturePtr lpSurface = NULL;

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = bbdesc.Format;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.BindFlags = 0;//D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ; // | D3D11_CPU_ACCESS_WRITE
    desc.MiscFlags = 0;

	op_result = g2dDX11Global::g_pDevice->CreateTexture2D(&desc, NULL, &lpSurface);
	if ( !SUCCEEDED(op_result) )
	{
		g2dDX11Global::PrintDXError(op_result);
		DBG_ASSERT(SUCCEEDED(op_result), "error creating image surface");
	}

	ID3D11Resource* pResource = NULL;
	back_buffer->GetResource(&pResource);
	g2dDX11Global::g_pDeviceContext->CopyResource(lpSurface, pResource);
	pResource->Release();
	back_buffer->Release();
	return lpSurface;
}

} //namespace


//------------------------------------------------------------------------
//	Save will attempt to figure out the type based on the filename 
//	passed in.
//
//	If the file format is not known the function will throw 
//	g2dUnknownImageFileTypeX.
//------------------------------------------------------------------------
void g2dImageSaveDX11::Save(const fsLocator& i_FileName, g2dWindow* i_pWin)
{
	g2dWindowDX11* pWindowD3D = dynamic_cast<g2dWindowDX11*>(i_pWin);
	if (pWindowD3D)
	{
		g2dD3D11TexturePtr pSurface = capture_back_buffer(pWindowD3D);

		FileType type = pick_format( i_FileName );

		itString filename;
		fsFileUtil::LocatorToUnicodeString(i_FileName, filename);

		SaveToFile( filename, pSurface, get_d3d_file_format( type ) );

		pSurface->Release();
	}
}

//------------------------------------------------------------------------
//	Save will attempt to figure out the type based on the filename 
//	passed in.
//
//	If the file format is not known the function will throw 
//	g2dUnknownImageFileTypeX.
//------------------------------------------------------------------------
void g2dImageSaveDX11::Save(const fsLocator& i_FileName, const g2dImage* i_pImage)
{
	const g2dImageDX11* pD3DImage = dynamic_cast<const g2dImageDX11*>(i_pImage);
	DBG_ASSERT(pD3DImage != 0, "Not a valid image pointer");

	FileType type = pick_format( i_FileName );

	int rendertype = g3dPrefs::CurrentPrefs().m_RendererType;
	int pixelFormat = i_pImage->GetPixelFormat().GetPixelFormat();

	if ( type == e_EXR ) {
		g2dScanlineOpenEXRUtil::WriteToScanlineEXR(i_FileName, i_pImage);		
		//g2dTiledOpenEXRUtil::WriteToTiledEXR(i_FileName, i_pImage);
	}else if (type == e_HDR)
	{
		g2dHDRUtil::WriteToHDR(i_FileName, i_pImage);
	}else if (type == e_TGA)
	{
		g2dTGAUtil::WriteToTGA(i_FileName, i_pImage);
	} else {
		itString filename;
		fsFileUtil::LocatorToUnicodeString(i_FileName, filename);
		SaveToFile( filename, pD3DImage->GetSurface(), get_d3d_file_format( type ) );
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void g2dImageSaveDX11::Save(const itString& i_FileName, const g2dImage* i_pImage)
{
	const g2dImageDX11* pD3DImage = dynamic_cast<const g2dImageDX11*>(i_pImage);
	DBG_ASSERT(pD3DImage != 0, "Not a valid image pointer");

	FileType type = pick_format( i_FileName );

	SaveToFile( i_FileName, pD3DImage->GetSurface(), get_d3d_file_format( type ) );
}


