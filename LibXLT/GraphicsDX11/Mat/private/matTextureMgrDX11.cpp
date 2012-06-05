/****************************************************************************\
**  matTextureMgrDX11.hpp
**
**      matTextureMgrDX11.hpp is the windows implementation of the
**	Terawatt matTextureMgr.  In addition to just managing the list of
**	textures, the Windows matTextureMgr will reload them from disk
**	and restore the surfaces when the app has been suspended
**	(which usually results in the surfaces being lost).
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/mat/matTextureMgrDX11.hpp"

#include "Core/app/appFlowEventHandler.hpp"
#include "Core/ch/chBinReader.hpp"
//#include "Core/env/envPlatform.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/gf/gfFileTxt.hpp"
#include "Core/gf/gfFileTranslationMgr.hpp"
#include "Core/it/itStringUtil.hpp"
//#include "Core/ma/maFloatRGBA.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g2d/g2dImageSave.hpp"
#include "Graphics/g2d/g2dResetHandler.hpp"
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/mat/matExceptionX.hpp"
#include "Graphics/mat/matTextureTracking.hpp"
#include "Graphics/mat/matUVATexture.hpp"
#include "Graphics/mat/matUVATextureParseUtil.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/g2dImageDX11.hpp"
#include "GraphicsDX11/g2d/private/g2dImageDrawUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "GraphicsDX11/G3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/mat/matCubeRenderTargetTexture.hpp"
#include "GraphicsDX11/mat/matDX11GlobalWin.hpp"
#include "GraphicsDX11/mat/matMipTexture.hpp"
#include "GraphicsDX11/mat/matPlainTexture.hpp"
#include "GraphicsDX11/mat/matShadowMap.hpp"
#include "GraphicsDX11/mat/matReflectiveShadowMap.hpp"
#include "GraphicsDX11/mat/matStaticCubeTexture.hpp"
#include "GraphicsDX11/mat/matVolumeTexture.hpp"
#include "GraphicsDX11/mat/private/matTextureLoaderWin.hpp"
#include "GraphicsDX11/mat/private/matTextureLoaderDDSWin.hpp"
#include "GraphicsDX11/mat/private/matTextureLoaderHDRWin.hpp"
#include "GraphicsDX11/mat/private/matTextureLoaderTGAWin.hpp"

#include <map>
#include <memory>

//	The reason a macro is used here (instead of a function, an inline function, or a template inline function)
//	is that I need the __FILE__ and __LINE__ macros to resolve to useful values
#define CHECK_D3D(hr, msg) \
	do { if( !SUCCEEDED(hr) )				\
	{									\
		g2dDX11Global::PrintDXError(hr);	\
		DBG_ASSERT(SUCCEEDED(hr), (msg));\
	} \
	} while(0);
//		DBG_ASSERT(SUCCEEDED(hr), __FILE__ << "(" << __LINE__ << ") : " << (msg));\

namespace
{
	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dBlendStateMgr::BlendState* st_Blend = NULL;

	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;

enum FileType
{
	e_BMP,
	e_PNG,
	e_DDS,
	e_TGA,
	e_JPG,
	e_HDR,
	e_TIF,
	e_RESOURCE
};

struct FileInfo
{
	FileInfo();
	FileInfo(FileType i_Type, const fsLocator& i_Locator, int i_WidthReduce, int i_HeightReduce);
	FileInfo(int i_Width, int i_Height, const g2dPFD& i_Format);

	FileType m_Type;
	fsLocator m_Locator;
	int m_WidthReduce;
	int m_HeightReduce;

	//	stuff only used for render target textures
	int m_Width;
	int m_Height;
	g2dPFD m_Format;
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline FileInfo::FileInfo()
:	m_WidthReduce(0),
	m_HeightReduce(0)
{
}

inline FileInfo::FileInfo(FileType i_Type, const fsLocator& i_Locator, int i_WidthReduce, int i_HeightReduce)
:	m_Type(i_Type), m_Locator(i_Locator), m_WidthReduce(i_WidthReduce), m_HeightReduce(i_HeightReduce)
{
}

FileInfo::FileInfo(int i_Width, int i_Height, const g2dPFD& i_Format)
:	m_Width(i_Width),
	m_Height(i_Height),
	m_Format(i_Format),
	m_WidthReduce(0),
	m_HeightReduce(0)
{
}

std::map<matPlainTexture*, FileInfo> l_PlainTextures;
std::map<matMipTexture*, FileInfo> l_MipTextures;
typedef std::map<matTextureDX11*, matTextureLoaderDDS::DDSInfo*> CompressedTextures;
CompressedTextures l_CompressedTextures;

std::map<matRenderTargetTexture*, FileInfo> l_RenderTargetTextures;
std::map<matTextureDX11*, FileInfo> l_ShadowMapTextures;
std::map<matTextureDX11*, FileInfo> l_RSMTextures;
std::map<matRenderTargetTexture*, FileInfo> l_FramebufferTextures;

g2dResourceCounterDX11::eResourceCategory GetG2DResourceCategory(matTextureMgr::eResourceCategory i_ResourceCategory)
{
	switch(i_ResourceCategory)
	{
	case matTextureMgr::e_Framebuffer:
		return g2dResourceCounterDX11::eRenderTarget;
	case matTextureMgr::e_ShadowMap:
		return g2dResourceCounterDX11::eShadow;
	case matTextureMgr::e_SceneTexture:
		return g2dResourceCounterDX11::eTexture;
	default:
		return g2dResourceCounterDX11::eTexture;
	};
}
float l_PlainTextureMemory = 0;
float l_MipTextureMemory = 0;
float l_CompressedTextureMemory = 0;

float l_RenderTargetTextureMemory = 0;
float l_ShadowMapTextureMemory = 0;
float l_RSMTextureMemory = 0;
float l_FramebufferTextureMemory = 0;

void ShowTextureMemory()
{
	DBG_LOG("Plain           : " << l_PlainTextures.size() << " :: " << (l_PlainTextureMemory) << " kB");
	DBG_LOG("Mip             : " << l_MipTextures.size() << " :: " << (l_MipTextureMemory) << " kB");
	DBG_LOG("Compressed (DDS): " << l_CompressedTextures.size() << " :: " << (l_CompressedTextureMemory) << " kB");
	DBG_LOG("Render target   : " << l_RenderTargetTextures.size() << " :: " << (l_RenderTargetTextureMemory) << " kB");
	DBG_LOG("Shadow map      : " << l_ShadowMapTextures.size() << " :: " << (l_ShadowMapTextureMemory) << " kB");
	DBG_LOG("RS map			 : " << l_RSMTextures.size() << " :: " << (l_RSMTextureMemory) << " kB");
	DBG_LOG("Frame buffer	 : " << l_FramebufferTextures.size() << " :: " << (l_FramebufferTextureMemory) << " kB");
}

int l_MaxTextureSize = -1;

inline int choose_num_mip_levels(int i_Width, int i_Height)
{
	int smallest_dim = (i_Width < i_Height) ? i_Width : i_Height;

	int power_of_2 = 0;

	while( (smallest_dim & 0x01) == 0 )
	{
		smallest_dim >>= 1;
		power_of_2++;
	}

	return power_of_2 + 1;
}

void Warn_TextureType( TEXTURE_TYPE i_Type )
{
	static char label[] = "Texture type incompatible, expecting";
	switch( i_Type )
	{
		case TEXTURE_TYPE_UNKNOWN:{	DBG_WARNING( label << " Unknown" ); break;}
		case TEXTURE_TYPE_1D:{		DBG_WARNING( label << " 1D Texture" ); break;}
		case TEXTURE_TYPE_2D:{		DBG_WARNING( label << " 2D Texture" ); break;}
		case TEXTURE_TYPE_CUBE:{	DBG_WARNING( label << " Cube Texture" ); break;}
		case TEXTURE_TYPE_3D:{		DBG_WARNING( label << " 3D Texture" ); break;}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
FileType pick_format(const fsLocator& i_Locator)
{
	//	decide what kind of file we have
	//
	gfFileBin test_file_stream(i_Locator, fsFileStream::e_ReadOnly);

	// Check header to determine type
	unsigned char header[12];
	test_file_stream.Read(12, header);

	//	The BMP header begins with 0x42 0x4d 'BM'
	if( (header[0] == 0x42) && (header[1] == 0x4d) )
		return e_BMP;
	//	The DDS header begins with 0x44 0x44 'DD'
	else if( (header[0] == 0x44) && (header[1] == 0x44) )
		return e_DDS;
	else if((header[0] == 137)	&&
			(header[1] == 80)	&&
			(header[2] == 78)	&&
			(header[3] == 71)	&&
			(header[4] == 13)	&&
			(header[5] == 10)	&&
			(header[6] == 26)	&&
			(header[7] == 10)	)
		return e_PNG;
	//	The Normal Data header has 8-byte Terawatt header, then
	//else if((header[8] == 0x4E)	 &&	// 'N'
	//		(header[9] == 0x52)	 && // 'R'
	//		(header[10] == 0x4D) && // 'M'
	//		(header[11] == 0x44) )  // 'D'
	//	return e_Normal;
	////	The Reflection Data header has 8-byte Terawatt header, then
	//else if((header[8] == 0x52)	 &&	// 'R'
	//		(header[9] == 0x42)	 && // 'B'
	//		(header[10] == 0x4D) && // 'M'
	//		(header[11] == 0x44) )  // 'D'
	//	return e_Reflection;
	else if((header[0] == 0xFF)	&&
			(header[1] == 0xD8)	&&
			(header[2] == 0xFF) )
		return e_JPG;
	// really crude attempt to Identify a basic TGA file (32 or 24 bit rgb, no RLE):
	else if((header[0] == 0)	&&
			(header[1] == 0)	&&
			(header[2] == 2)	&&
			(header[3] == 0)	&&
			(header[4] == 0)	&&
			(header[5] == 0)	&&
			(header[6] == 0)	&&
			(header[7] == 0)	&&
			(header[8] == 0)	&&
			(header[9] == 0)	&&
			(header[10] == 0)	&&
			(header[11] == 0)	)
		return e_TGA;
	// .HDR format should begin with "#?RADIANCE"
	else if( strncmp( (char*)header, "#?RADIANCE", 10 ) == 0 ) 
		return e_HDR;
	//tif begins with 'II' or 'MM'
	else if( ((header[0] == 0x49) && (header[1] == 0x49)) || ((header[0] == 0x4D) && (header[1] == 0x4D)) )
		return e_TIF;
	else throw matUnknownImageFileTypeX(i_Locator);

}

//--------------------------------------------------------------------
// note:  this will call ::Load for each type, those Loads handle
// reloads.  the matTextureLoaderWin::Reload turn around and call these
// calls anyway.
//--------------------------------------------------------------------
void load_plain_texture(matPlainTexture* io_Texture, const FileInfo& i_FileInfo)
{
	switch( i_FileInfo.m_Type )
	{
		case e_BMP:
			matTextureLoaderWin::LoadPlainTexture(io_Texture, i_FileInfo.m_Locator);
			//matTextureLoaderWin::LoadBMP(io_Texture, i_FileInfo.m_Locator, false, i_FileInfo.m_WidthReduce, i_FileInfo.m_HeightReduce);
		break;

		case e_PNG:
			matTextureLoaderWin::LoadPlainTexture(io_Texture, i_FileInfo.m_Locator);
			//matTextureLoaderWin::LoadPNG(io_Texture, i_FileInfo.m_Locator, false, i_FileInfo.m_WidthReduce, i_FileInfo.m_HeightReduce);
		break;

		case e_DDS:
			matTextureLoaderWin::LoadPlainTexture(io_Texture, i_FileInfo.m_Locator);
		break;

		case e_TGA:
			matTextureLoaderWin::LoadPlainTexture(io_Texture, i_FileInfo.m_Locator);
		break;
		case e_JPG:
			matTextureLoaderWin::LoadPlainTexture(io_Texture, i_FileInfo.m_Locator);
		break;
		case e_HDR:
			matTextureLoaderWin::LoadPlainTexture(io_Texture, i_FileInfo.m_Locator);
		break;
		case e_TIF:
			matTextureLoaderWin::LoadTIFF(io_Texture, i_FileInfo.m_Locator, true, i_FileInfo.m_WidthReduce, i_FileInfo.m_HeightReduce);
		break;
		case e_RESOURCE:
			matTextureLoaderWin::LoadResource(io_Texture, i_FileInfo.m_Locator, i_FileInfo.m_WidthReduce, i_FileInfo.m_HeightReduce);
		break;
	}
}

//--------------------------------------------------------------------
// note:  this will call ::Load for each type, those Loads handle
// reloads.  the matTextureLoaderWin::Reload turn around and call these
// calls anyway.
//--------------------------------------------------------------------
void load_mip_texture(matMipTexture* io_Texture, const FileInfo& i_FileInfo, bool i_bIsMipMap)
{
	switch( i_FileInfo.m_Type )
	{
		case e_BMP:
			matTextureLoaderWin::LoadBMP(io_Texture, i_FileInfo.m_Locator, true, i_FileInfo.m_WidthReduce, i_FileInfo.m_HeightReduce, i_bIsMipMap);
		break;

		case e_PNG:
			matTextureLoaderWin::LoadPNG(io_Texture, i_FileInfo.m_Locator, true, i_FileInfo.m_WidthReduce, i_FileInfo.m_HeightReduce, i_bIsMipMap);
		break;

		case e_JPG:
			matTextureLoaderWin::LoadTGA(io_Texture, i_FileInfo.m_Locator, true, i_FileInfo.m_WidthReduce, i_FileInfo.m_HeightReduce, i_bIsMipMap);
		break;
		case e_TGA:
			matTextureLoaderTGA::LoadTGA(io_Texture, i_FileInfo.m_Locator, true, i_FileInfo.m_WidthReduce, i_FileInfo.m_HeightReduce, i_bIsMipMap);
		break;
		case e_HDR:
			// reuse TGA loader since it uses generic D3DX image loading code!
			matTextureLoaderHDR::LoadHDR(io_Texture, i_FileInfo.m_Locator, true, i_FileInfo.m_WidthReduce, i_FileInfo.m_HeightReduce, i_bIsMipMap);
		break;
		case e_TIF:
			matTextureLoaderWin::LoadTIFF(io_Texture, i_FileInfo.m_Locator, true, i_FileInfo.m_WidthReduce, i_FileInfo.m_HeightReduce, i_bIsMipMap);
		break;
		//	never be the case?
		//case e_RESOURCE:
		//	matTextureLoaderWin::LoadResource(io_Texture, i_FileInfo.m_Locator, i_FileInfo.m_WidthReduce, i_FileInfo.m_HeightReduce);
		//break;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void load_render_target_texture(matRenderTargetTexture* io_Texture, const FileInfo& i_FileInfo)
{
	io_Texture->Make( i_FileInfo.m_Width, i_FileInfo.m_Height, i_FileInfo.m_Format );
	io_Texture->ReloadInfo();
}

void load_compressed_texture( matTextureDX11* io_Texture, matTextureLoaderDDS::DDSInfo& i_FileInfo )
{
	fsLocator locator = i_FileInfo.s_Locator;
	DBG_ASSERT(false,"load_compressed_texture not implemented");
	DBG_ERROR("load_compressed_texture not implemented");
	//io_Texture->SetSurface( matTextureLoaderDDS::LoadDDS(i_FileInfo, locator, i_FileInfo.s_nWidthReduce, i_FileInfo.s_nHeightReduce) );
}

//============================================================================
//============================================================================
class matTextureReloader : public g2dResetHandler
{
	public:
		//------------------------------------------------------------------------
		//	Deallocate is called when all device dependent resources should be
		//	released.
		//------------------------------------------------------------------------
		virtual void Deallocate();

		//------------------------------------------------------------------------
		//	Allocate is called when the device has been Reset and resources can
		//	be reloaded again.
		//------------------------------------------------------------------------
		virtual void Reallocate();
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void matTextureReloader::Deallocate()
{
	DBG_LOG("matTextureReloader::Deallocate()");
	DBG_ASSERT(false,"matTextureReloader::Deallocate() not implemented");
	DBG_ERROR("matTextureReloader::Deallocate() not implemented");
/*
	{
		std::map<matPlainTexture*, FileInfo>::iterator it = l_PlainTextures.begin();
		std::map<matPlainTexture*, FileInfo>::iterator end = l_PlainTextures.end();
		while( it != end )
		{
			it->first->SetSurface(NULL);
			++it;
		}
	}

	{
		std::map<matMipTexture*, FileInfo>::iterator it = l_MipTextures.begin();
		std::map<matMipTexture*, FileInfo>::iterator end = l_MipTextures.end();
		while( it != end )
		{
			it->first->SetSurface(NULL);
			++it;
		}
	}

	{
		std::map<matRenderTargetTexture*, FileInfo>::iterator it = l_RenderTargetTextures.begin();
		std::map<matRenderTargetTexture*, FileInfo>::iterator end = l_RenderTargetTextures.end();
		while( it != end )
		{
			it->first->SetSurface(NULL);
			++it;
		}
	}

	{
		std::map<matTextureDX11*, FileInfo>::iterator it = l_ShadowMapTextures.begin();
		std::map<matTextureDX11*, FileInfo>::iterator end = l_ShadowMapTextures.end();
		while( it != end )
		{
			it->first->SetSurface(NULL);
			++it;
		}
	}
	{
		std::map<matTextureDX11*, FileInfo>::iterator it = l_RSMTextures.begin();
		std::map<matTextureDX11*, FileInfo>::iterator end = l_RSMTextures.end();
		while( it != end )
		{
			it->first->SetSurface(NULL);
			++it;
		}
	}
	{
		std::map<matRenderTargetTexture*, FileInfo>::iterator it = l_FramebufferTextures.begin();
		std::map<matRenderTargetTexture*, FileInfo>::iterator end = l_FramebufferTextures.end();
		while( it != end )
		{
			it->first->SetSurface(NULL);
			++it;
		}
	}

	{
		CompressedTextures::iterator it = l_CompressedTextures.begin();
		CompressedTextures::iterator end = l_CompressedTextures.end();
		while( it != end )
		{
			it->first->SetSurface(NULL);
			++it;
		}
	}
*/
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void matTextureReloader::Reallocate()
{
	DBG_LOG("matTextureReloader::Reallocate()");

	//	reload all of our textures
	//
	{
		std::map<matPlainTexture*, FileInfo>::iterator it = l_PlainTextures.begin();
		std::map<matPlainTexture*, FileInfo>::iterator end = l_PlainTextures.end();
		while( it != end )
		{
			load_plain_texture(it->first, it->second);
			++it;
		}
	}

	{
		std::map<matMipTexture*, FileInfo>::iterator it = l_MipTextures.begin();
		std::map<matMipTexture*, FileInfo>::iterator end = l_MipTextures.end();
		while( it != end )
		{
			load_mip_texture(it->first, it->second, true);
			++it;
		}
	}

	{
		std::map<matRenderTargetTexture*, FileInfo>::iterator it = l_RenderTargetTextures.begin();
		std::map<matRenderTargetTexture*, FileInfo>::iterator end = l_RenderTargetTextures.end();
		while( it != end )
		{
			load_render_target_texture(it->first, it->second);
			++it;
		}
	}
	{
		std::map<matTextureDX11*, FileInfo>::iterator it = l_ShadowMapTextures.begin();
		std::map<matTextureDX11*, FileInfo>::iterator end = l_ShadowMapTextures.end();
		while( it != end )
		{
			DBG_ASSERT(false, "Not able to reload shadowmaps yet");break;
			//load_render_target_texture(it->first, it->second);
			++it;
		}
	}
	{
		std::map<matTextureDX11*, FileInfo>::iterator it = l_RSMTextures.begin();
		std::map<matTextureDX11*, FileInfo>::iterator end = l_RSMTextures.end();
		while( it != end )
		{
			DBG_ASSERT(false, "Not able to reload rs maps yet");break;
			//load_render_target_texture(it->first, it->second);
			++it;
		}
	}
	{
		std::map<matRenderTargetTexture*, FileInfo>::iterator it = l_FramebufferTextures.begin();
		std::map<matRenderTargetTexture*, FileInfo>::iterator end = l_FramebufferTextures.end();
		while( it != end )
		{
			load_render_target_texture(it->first, it->second);
			++it;
		}
	}

	{
		CompressedTextures::iterator it = l_CompressedTextures.begin();
		CompressedTextures::iterator end = l_CompressedTextures.end();
		while( it != end )
		{
			load_compressed_texture( it->first, *(it->second) );
			++it;
		}
	}

	DBG_LOG("Finished matTextureReloader::Reallocate");
}

}	// end of namespace


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void FillTexture(matTextureDX11* io_pDestPlainTex, void* i_SrcBuffer, int i_nRows, int i_nRowBytes,
				 g2dD3D11TexturePtr i_SrcSurface)
{
	/////////////////////////////////////////////////////
	// put the bytes in a system mem surface (i_TempTex).
	/////////////////////////////////////////////////////

	HRESULT op_result;

	// lock the source surface
	D3D11_MAPPED_SUBRESOURCE lock_info;
	op_result = g2dDX11Global::g_pDeviceContext->Map(i_SrcSurface,
		D3D11CalcSubresource(0,0,1),
		D3D11_MAP_WRITE,
		0,
		&lock_info
		);
	CHECK_D3D(op_result, "FillTexture: Couldn't lock surface");
	
	// fill data
	for (int i = 0; i < i_nRows; i++)
	{
		::memcpy((BYTE*)lock_info.pData + i*lock_info.RowPitch, (BYTE*)i_SrcBuffer + i*i_nRowBytes, i_nRowBytes);
	}

	// unlock the source surface
	g2dDX11Global::g_pDeviceContext->Unmap(i_SrcSurface, D3D11CalcSubresource(0,0,1));

	/////////////////////////////////////////////////////
	// done with temp surface. now use UpdateSurface to get the bits up to vidmem.
	/////////////////////////////////////////////////////

	g2dDX11Global::g_pDeviceContext->CopyResource(io_pDestPlainTex->GetResource(), i_SrcSurface);

//	char fname[MAX_PATH];
//	sprintf(fname, "gpu_%08x.dds", io_pDestPlainTex);
//	fsLocator loc;
//	loc.Push(fname);
//	matTextureMgr::SaveTextureToFile(io_pDestPlainTex, loc);
}

void matTextureMgrDX11::UpdateSurface( matTexture* i_pTexture, unsigned char* i_Data, int i_Size, int nMipLevel )
{
	g2dD3D11TexturePtr l_LocalMemSurfaceRGBA32f = NULL;

	// sysmem tex for temp upload.
	int width = i_pTexture->GetWidth();
	int height = i_pTexture->GetHeight();

	DXGI_FORMAT fmt = g2dDX11Global::D3DFormatFromPFD( i_pTexture->GetPixelFormat() );

	CD3D11_TEXTURE2D_DESC desc( fmt, width, height, 
		1, 1, 0, D3D11_USAGE_STAGING, D3D11_CPU_ACCESS_WRITE  );
	D3D11_SUBRESOURCE_DATA data = {i_Data,width*i_pTexture->GetPixelFormat().BitsPerPixel()/8,0};
	HRESULT hr = g2dDX11Global::g_pDevice->CreateTexture2D(&desc, &data, &l_LocalMemSurfaceRGBA32f);

	/////////////////////////////////////////////////////
	// done with temp surface. now use UpdateSurface to get the bits up to vidmem.
	/////////////////////////////////////////////////////

	// go from matPlainTexture to g2dD3DBaseTexturePtr (no need to release this one)
	ID3D11ShaderResourceView* pDestBaseTex = g3dDX11TextureUtil::GetD3DTexture(i_pTexture);
	ID3D11Resource* resource = NULL;
	pDestBaseTex->GetResource(&resource);

	g2dDX11Global::g_pDeviceContext->CopyResource(resource, l_LocalMemSurfaceRGBA32f);

	resource->Release();
	if (l_LocalMemSurfaceRGBA32f != NULL)
		l_LocalMemSurfaceRGBA32f->Release();
}


g2dPFD l_pfdGR32f = g2dPFD(g2dPFD::e_GR32f, 32*2);
int l_bytesGR32f = l_pfdGR32f.BitsPerPixel()/8;;
matTextureDX11* l_RndTexture = NULL;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CreatePoissonTexture()
{
#define MAX_POISSON_SAMPLES 256//128 //81
	// NOTE ONLY 81 SAMPLES! ALLOC'ED SPACE FOR 128. 

/*	// DONT SAMPLE PAST 81!
float poissonDisk[MAX_POISSON_SAMPLES*2] = {
	-0.304967f, -0.058754f,
	-0.043598f, -0.452767f,
	-0.642527f, 0.799442f,
	0.640218f, 0.596450f,
	0.659291f, -0.773008f,
	-0.178993f, -0.872160f,
	0.276105f, -0.820578f,
	0.580867f, -0.117841f,
	-0.522690f, -0.634022f,
	0.089258f, 0.226401f,
	0.928508f, 0.051883f,
	-0.868488f, 0.350548f,
	0.941204f, 0.957318f,
	-0.495066f, 0.394927f,
	-0.125758f, 0.984469f,
	-0.955218f, -0.603685f,
	0.795363f, -0.541541f,
	0.934481f, 0.482255f,
	-0.755124f, -0.059173f,
	0.311857f, 0.783972f,
	-0.959199f, -0.966749f,
	-0.065384f, 0.641868f,
	0.923424f, -0.935969f,
	0.040538f, -0.118669f,
	-0.996426f, 0.082208f,
	-0.613901f, -0.982514f,
	0.236256f, -0.397797f,
	0.609993f, 0.962312f,
	-0.950827f, 0.948278f,
	0.490153f, 0.189333f,
	0.304875f, 0.546167f,
	-0.263125f, 0.281158f,
	-0.572705f, 0.141030f,
	0.954722f, -0.313642f,
	-0.346067f, -0.330249f,
	-0.723442f, -0.474052f,
	-0.383928f, 0.651586f,
	0.286045f, 0.018058f,
	-0.942373f, 0.674203f,
	0.084795f, -0.954489f,
	-0.948848f, -0.187677f,
	0.535156f, -0.407324f,
	0.659832f, 0.351442f,
	-0.254076f, -0.534739f,
	0.896457f, 0.264414f,
	-0.396225f, -0.983286f,
	-0.417622f, 0.924533f,
	0.911242f, 0.718191f,
	0.976459f, -0.699264f,
	-0.116580f, 0.108363f,
	0.319926f, -0.220776f,
	-0.712503f, 0.505410f,
	-0.774058f, -0.812883f,
	0.090694f, 0.829392f,
	0.330998f, -0.608571f,
	-0.548603f, -0.384875f,
	0.118297f, 0.550209f,
	-0.919727f, -0.400404f,
	0.024899f, -0.646427f,
	-0.732525f, 0.986321f,
	-0.060302f, 0.361266f,
	0.178028f, 0.986945f,
	0.527828f, -0.931745f,
	-0.233099f, 0.799864f,
	0.297159f, 0.248822f,
	0.756461f, -0.305501f,
	0.456517f, 0.387692f,
	-0.533354f, -0.140567f,
	0.739327f, 0.111377f,
	-0.339785f, -0.741848f,
	-0.755231f, -0.263203f,
	0.489712f, 0.714908f,
	-0.236110f, 0.472629f,
	0.547581f, -0.613419f,
	-0.643593f, 0.332322f,
	0.736407f, -0.057415f,
	0.064937f, -0.287273f,
	-0.320082f, 0.112393f,
	-0.992819f, 0.524451f,
	-0.762316f, 0.123410f,
	0.731807f, -0.986733f
	};*/
	float poissonDisk[MAX_POISSON_SAMPLES*2] = {
		-0.415757954121f, 	-0.307007312775f,
		-0.999989032745f, 	0.123504042625f,
		-0.324011802673f, 	0.33845436573f,
		-0.969086050987f, 	-0.997798800468f,
		-0.181707203388f, 	-0.0354183316231f,
		0.9620013237f, 	0.746612906456f,
		0.634776711464f, 	-0.359547495842f,
		0.686866641045f, 	0.570747971535f,
		-0.00195515155792f, 	-0.605298280716f,
		0.0248702764511f, 	0.37431037426f,
		-0.0493814945221f, 	0.33611869812f,
		0.402388095856f, 	-0.887425601482f,
		0.920067548752f, 	0.192143321037f,
		-0.360320627689f, 	-0.868100583553f,
		0.0381577014923f, 	-0.111639559269f,
		0.539649248123f, 	0.668017029762f,
		0.0927656888962f, 	0.384810805321f,
		0.651272773743f, 	0.127217888832f,
		0.206308484077f, 	0.607260704041f,
		0.79118001461f, 	0.677464842796f,
		0.139808416367f, 	-0.963199913502f,
		-0.22441393137f, 	-0.851792991161f,
		0.0195223093033f, 	-0.482260942459f,
		0.342718839645f, 	0.930730938911f,
		-0.0997076034546f, 	0.204538822174f,
		-0.378837943077f, 	0.659482836723f,
		0.00894856452942f, 	0.505936384201f,
		0.651379942894f, 	-0.00585699081421f,
		-0.837081551552f, 	0.951209783554f,
		-0.290645718575f, 	-0.701874494553f,
		0.0917477607727f, 	0.152590155602f,
		-0.503503918648f, 	-0.72034406662f,
		0.703035116196f, 	0.0749887228012f,
		-0.96193087101f, 	0.780337452888f,
		-0.770724475384f, 	-0.285424351692f,
		-0.521964728832f, 	0.859279155731f,
		-0.264045059681f, 	0.0244901180267f,
		-0.287628412247f, 	-0.221044540405f,
		-0.726094305515f, 	0.877244234085f,
		-0.0579364299774f, 	0.575763344765f,
		-0.89625442028f, 	0.213267683983f,
		0.921410679817f, 	0.599874258041f,
		0.405699491501f, 	0.167470216751f,
		-0.266678452492f, 	-0.486743867397f,
		-0.200808227062f, 	-0.28875541687f,
		-0.407266914845f, 	0.879252076149f,
		-0.610522627831f, 	-0.502714037895f,
		0.190292000771f, 	-0.375916957855f,
		0.432513713837f, 	0.337543964386f,
		-0.599826455116f, 	-0.362500607967f,
		0.122844338417f, 	-0.674452006817f,
		-0.0707055330276f, 	-0.804758310318f,
		0.561969161034f, 	0.534954190254f,
		-0.706166863441f, 	0.713750720024f,
		0.50043463707f, 	-0.15829706192f,
		-0.931690633297f, 	0.597908616066f,
		-0.0521476864815f, 	0.0349020957947f,
		-0.997243881226f, 	-0.141711473465f,
		-0.962580561638f, 	0.935882329941f,
		0.4426176548f, 	-0.195207297802f,
		0.749297738075f, 	-0.0625938177109f,
		0.908566951752f, 	-0.0246162414551f,
		-0.637861430645f, 	-0.923464179039f,
		-0.79311645031f, 	0.809995889664f,
		0.887435913086f, 	0.544613480568f,
		0.946038246155f, 	0.121113061905f,
		-0.593244731426f, 	0.739417433739f,
		0.46822810173f, 	-0.0841932892799f,
		0.529536962509f, 	-0.901707589626f,
		0.92825114727f, 	-0.577435731888f,
		0.600801944733f, 	0.924322366714f,
		0.665665864944f, 	0.993400335312f,
		0.774976491928f, 	0.0103375911713f,
		0.62262237072f, 	-0.705139458179f,
		0.493190407753f, 	0.288145065308f,
		-0.864136457443f, 	-0.975541055202f,
		0.625094771385f, 	-0.61847358942f,
		0.324367523193f, 	0.309258699417f,
		-0.658737182617f, 	-0.149027884007f,
		-0.476232171059f, 	-0.254612147808f,
		0.449486494064f, 	0.591151356697f,
		0.315091848373f, 	-0.569881975651f,
		0.325016498566f, 	-0.0642792582512f,
		0.484638929367f, 	0.160881996155f,
		-0.73237580061f, 	0.206281661987f,
		0.111445307732f, 	0.0115730762482f,
		-0.457200288773f, 	-0.359537184238f,
		0.752270579338f, 	0.145184874535f,
		-0.26561653614f, 	-0.290171980858f,
		-0.86328625679f, 	-0.506230592728f,
		0.908035159111f, 	-0.188288152218f,
		-0.353170394897f, 	-0.747117161751f,
		0.189489364624f, 	0.399418950081f,
		-0.872706890106f, 	0.0921521186829f,
		-0.727744817734f, 	-0.860645651817f,
		-0.948987066746f, 	-0.441547632217f,
		0.401406764984f, 	-0.469781160355f,
		-0.26779872179f, 	-0.0837929844856f,
		0.937297105789f, 	-0.730389237404f,
		0.0744278430939f, 	0.0924748182297f,
		-0.704310297966f, 	0.354593992233f,
		0.155883431435f, 	0.644702553749f,
		-0.636761069298f, 	-0.0448909401894f,
		0.420086026192f, 	0.921164751053f,
		-0.251735448837f, 	-0.144667387009f,
		0.189654707909f, 	-0.225541591644f,
		0.501892805099f, 	0.495574474335f,
		0.346584320068f, 	0.539728045464f,
		0.141563415527f, 	0.998720407486f,
		-0.506407439709f, 	-0.885527908802f,
		-0.540439724922f, 	0.238392114639f,
		-0.305917322636f, 	0.130444765091f,
		0.17865550518f, 	0.905961751938f,
		-0.514184236526f, 	0.381422758102f,
		0.0286765098572f, 	0.179683923721f,
		-0.712139964104f, 	-0.478293836117f,
		-0.521578788757f, 	-0.358527898788f,
		0.323919296265f, 	-0.903955638409f,
		-0.250787854195f, 	-0.774267852306f,
		-0.958730041981f, 	0.393280744553f,
		-0.635471224785f, 	-0.309784889221f,
		-0.248264908791f, 	0.699278116226f,
		0.250646710396f, 	-0.535455822945f,
		0.521033525467f, 	0.373806238174f,
		0.348887324333f, 	-0.270278334618f,
		-0.394953966141f, 	0.139647841454f,
		0.0216785669327f, 	0.919688224792f,
		0.371087551117f, 	0.46440577507f,
		0.556820988655f, 	-0.339622676373f,
		0.738476872444f, 	0.477137684822f,
		0.771810650826f, 	0.93575835228f,
		-0.205457806587f, 	0.182102441788f,
		-0.903442919254f, 	-0.576662361622f,
		-0.421976387501f, 	-0.214109063148f,
		-0.3824852705f, 	0.41473531723f,
		0.88427066803f, 	0.845234274864f,
		-0.597764253616f, 	-0.794075012207f,
		0.679982304573f, 	-0.802298903465f,
		-0.238126754761f, 	0.403813362122f,
		0.6050812006f, 	-0.543026208878f,
		-0.145884990692f, 	-0.211347818375f,
		-0.745605826378f, 	-0.999535262585f,
		-0.384266376495f, 	0.295287370682f,
		0.271909594536f, 	-0.0186420679092f,
		0.0944821834564f, 	0.740437626839f,
		-0.266018867493f, 	-0.962322890759f,
		0.0610929727554f, 	-0.268881261349f,
		-0.697465002537f, 	-0.720154285431f,
		-0.308974266052f, 	0.838204264641f,
		0.582984209061f, 	0.220297574997f,
		0.484161615372f, 	0.869464635849f,
		0.835884928703f, 	-0.217734217644f,
		0.866134524345f, 	0.962750196457f,
		0.359741687775f, 	-0.157439529896f,
		-0.978330790997f, 	0.503557443619f,
		0.515821576118f, 	0.0977878570557f,
		-0.781891405582f, 	0.660185813904f,
		-0.467448890209f, 	0.0832816362381f,
		-0.502683162689f, 	-0.151012897491f,
		0.876625180244f, 	-0.895189762115f,
		-0.900877118111f, 	-0.658530831337f,
		-0.367899179459f, 	0.79533624649f,
		0.32302069664f, 	-0.365221679211f,
		0.470696091652f, 	0.72997879982f,
		-0.167205512524f, 	-0.500283718109f,
		-0.93359130621f, 	0.148563742638f,
		0.395768165588f, 	-0.0961967110634f,
		0.395884156227f, 	0.74974834919f,
		0.080939412117f, 	0.599899888039f,
		-0.0910460948944f, 	-0.67810177803f,
		0.524116873741f, 	-0.644667506218f,
		0.672092199326f, 	-0.292624533176f,
		0.76543033123f, 	0.551385998726f,
		0.443885326385f, 	-0.527751803398f,
		0.301817536354f, 	0.707025885582f,
		0.107479453087f, 	-0.202602922916f,
		-0.160126268864f, 	-0.582996606827f,
		0.402836322784f, 	-0.633915960789f,
		-0.523296117783f, 	-0.95643222332f,
		0.452896356583f, 	-0.841628909111f,
		-0.865230441093f, 	0.794190526009f,
		0.522953748703f, 	-0.397499680519f,
		-0.717108011246f, 	-0.234159350395f,
		0.0981377363205f, 	0.841750502586f,
		-0.810242652893f, 	0.19634604454f,
		-0.905021548271f, 	0.730925798416f,
		0.211004734039f, 	0.215678215027f,
		-0.441374003887f, 	0.262136340141f,
		0.0325330495834f, 	0.0389763116837f,
		-0.812033832073f, 	0.536602258682f,
		0.0334894657135f, 	-0.386205136776f,
		0.200131177902f, 	0.0127446651459f,
		-0.027800142765f, 	0.781478524208f,
		0.415264368057f, 	0.258855342865f,
		0.306766033173f, 	0.480510234833f,
		0.349022507668f, 	-0.77649140358f,
		0.136532664299f, 	0.520867466927f,
		0.812962174416f, 	-0.559707403183f,
		-0.0205968022346f, 	0.986451745033f,
		-0.509413957596f, 	0.560806512833f,
		-0.550288438797f, 	0.476644039154f,
		0.939857363701f, 	0.456780433655f,
		-0.898642957211f, 	0.863899350166f,
		0.786417841911f, 	-0.890314102173f,
		-0.076176404953f, 	0.922913551331f,
		-0.860582351685f, 	-0.122878789902f,
		0.600135326385f, 	0.861961245537f,
		0.698167085648f, 	0.89713537693f,
		0.492275953293f, 	-0.0186278223991f,
		-0.841658473015f, 	-0.596030235291f,
		0.905914902687f, 	0.337806820869f,
		-0.721285104752f, 	-0.0184351801872f,
		-0.0693228840828f, 	-0.993405759335f,
		0.715800404549f, 	-0.203863024712f,
		0.117646455765f, 	-0.843482613564f,
		-0.443656802177f, 	-0.815229892731f,
		0.597274661064f, 	-0.766075611115f,
		-0.762543559074f, 	0.747245788574f,
		0.193806886673f, 	0.733397245407f,
		-0.985407471657f, 	-0.356724023819f,
		-0.738691449165f, 	-0.371147453785f,
		-0.537099838257f, 	-0.810829877853f,
		0.740815758705f, 	-0.994248151779f,
		0.807778120041f, 	-0.664702653885f,
		0.582011938095f, 	0.792075037956f,
		0.136256337166f, 	0.264344453812f,
		-0.18073785305f, 	-0.428484141827f,
		0.00223731994629f, 	-0.956182062626f,
		-0.00850087404251f, 	-0.251703977585f,
		0.756610989571f, 	-0.620826601982f,
		-0.911600470543f, 	-0.279290378094f,
		-0.653744339943f, 	0.645545363426f,
		0.657974839211f, 	0.625242829323f,
		0.58411192894f, 	0.132586479187f,
		0.879118680954f, 	0.107514619827f,
		-0.133203148842f, 	-0.907719731331f,
		0.583663225174f, 	-0.442635238171f,
		0.918936729431f, 	-0.316513895988f,
		-0.244584262371f, 	-0.625973284245f,
		-0.808946669102f, 	-0.828397154808f,
		0.271717905998f, 	-0.104681670666f,
		-0.229069411755f, 	-0.698700726032f,
		0.267337203026f, 	-0.172531247139f,
		0.671711683273f, 	-0.569589614868f,
		0.477365732193f, 	-0.997455060482f,
		0.292757153511f, 	-0.426412165165f,
		0.393433690071f, 	-0.725052118301f,
		-0.469008982182f, 	-0.452587723732f,
		-0.594199061394f, 	0.0838865041733f,
		0.171688318253f, 	-0.522022247314f,
		0.991615891457f, 	0.225111961365f,
		0.370167851448f, 	0.396544337273f,
		-0.218688964844f, 	0.797135591507f,
		-0.57398968935f, 	0.347099304199f,
		0.68114900589f, 	0.757608890533f,
		0.745979785919f, 	-0.698154687881f,


	};
	// hw tex
	matTextureMgr::ReleaseTexture(l_RndTexture);
	l_RndTexture = dynamic_cast<matTextureDX11*>(matTextureMgr::CreateTexture(MAX_POISSON_SAMPLES, 1, &l_pfdGR32f, false, poissonDisk));
//	FillTexture(l_RndTexture, poissonDisk, 1, MAX_POISSON_SAMPLES*l_bytesGR32f, l_LocalMemSurfaceGR32f);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void DestroyPoissonTexture()
{
	matTextureMgr::ReleaseTexture(l_RndTexture);
	l_RndTexture = NULL;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
matTexture* matTextureMgrDX11::GetPoisson()
{
	return l_RndTexture;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
matTextureMgrDX11::matTextureMgrDX11()
{
	l_MaxTextureSize = -1;
	g2dResetHandler::AddResetHandler(new matTextureReloader);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
matTextureMgrDX11::~matTextureMgrDX11()
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void matTextureMgrDX11::Initialize()
{
	CreatePoissonTexture();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void matTextureMgrDX11::DeInitialize()
{
	DestroyPoissonTexture();
}

//------------------------------------------------------------------------
//	GetTotalTextureMemory returns alleged amount of texture memory
//	made available by the hardware.  Take this number with a grain of
//	salt.
//------------------------------------------------------------------------
unsigned int matTextureMgrDX11::GetTotalTextureMemory()
{
	DBG_ASSERT(g2dDX11Global::g_pAdapter, "Must have initialized g2dSystem before calling this function");
	DXGI_ADAPTER_DESC desc;
	g2dDX11Global::g_pAdapter->GetDesc(&desc);
	return (unsigned int) desc.DedicatedVideoMemory;
}

//--------------------------------------------------------------------
// Get current texture usage totals
//--------------------------------------------------------------------
unsigned int matTextureMgrDX11::GetNumRenderTargets()
{
	return l_RenderTargetTextures.size();
}
float matTextureMgrDX11::GetTotalTextureMemoryUsed()
{
	return (l_PlainTextureMemory) + 
		(l_MipTextureMemory) +
		(l_CompressedTextureMemory) +
		(l_RenderTargetTextureMemory) +
		(l_ShadowMapTextureMemory) + 
		(l_RSMTextureMemory) + 
		(l_FramebufferTextureMemory);
}
unsigned int matTextureMgrDX11::GetNumTextures()
{
	return l_PlainTextures.size() +
		l_MipTextures.size() +
		l_CompressedTextures.size() +
		l_RenderTargetTextures.size() +
		l_ShadowMapTextures.size() +
		l_RSMTextures.size() +
		l_FramebufferTextures.size();
}
unsigned int matTextureMgrDX11::GetNumShadowMaps()
{
	return l_ShadowMapTextures.size();
}
float matTextureMgrDX11::GetTotalRSMapMemoryUsed()
{
	return l_RSMTextureMemory;
}
unsigned int matTextureMgrDX11::GetNumRSMaps()
{
	return l_RSMTextures.size();
}
float matTextureMgrDX11::GetTotalFramebufferMemoryUsed()
{
	return l_FramebufferTextureMemory;
}
unsigned int matTextureMgrDX11::GetNumFramebufferTargets()
{
	return l_FramebufferTextures.size();
}

//--------------------------------------------------------------------
// LoadTexture will attempt to load a texture based on the given
// locator, appending extensions until a file is found
// i_bIgnoreTUVSuffix makes it skip the .tuv test, used to allow for
// loading of textures w/ the same name as a tuv while maintaining
// a single codebase for this function
//--------------------------------------------------------------------
matTexture* matTextureMgrDX11::LoadTexture(const fsLocator& i_Directory,
						const itString& i_Name,
						std::vector<itString>& o_UVANames,
						TEXTURE_TYPE i_Type/* = TEXTURE_TYPE_2D*/,
						bool i_bIgnoreTUVSuffix /*= false*/,
						bool mipmap_if_2D /*= true*/)
{
	DBG_ASSERT( i_Name.GetLength() > 0, "No name for texture" );

	int width_reduce, height_reduce;

	fsLocator tex_locator(i_Directory);
	tex_locator.Push(i_Name);

	itString extension;
	i_Name.GetExtension(extension);
	itStringUtil::ToLower(extension);

	std::string fnamestr;
	fsFileUtil::LocatorToANSIFilename( tex_locator, fnamestr );	//debugging only

	if (gfFileTranslationMgr::FileExists(tex_locator))
	{
		if (extension == itString("dds"))
		{
			return this->LoadCompressedTexture( tex_locator, i_Type, mipmap_if_2D );
		}
		else if (extension == itString("tuv"))
		{
			if( i_Type == TEXTURE_TYPE_UNKNOWN || i_Type == TEXTURE_TYPE_2D )
			{
				if (!i_bIgnoreTUVSuffix) // do we need this check anymore?
				{
					gfFileBin texture_file(tex_locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);
					texture_file.ReadHeader();
					chBinReader reader(texture_file);

					return matUVATextureParseUtil::ReadUVATexture(reader, o_UVANames);
				}
			}
			else
			{
				Warn_TextureType( i_Type );//only 2d textures supported
			}
		}
		else
		{
			if( i_Type == TEXTURE_TYPE_UNKNOWN || i_Type == TEXTURE_TYPE_2D )
			{
				matTextureMgr::GetTextureAdjuster()->GetReduce(tex_locator, width_reduce, height_reduce);
				if ( matTextureMgr::GetTextureAdjuster()->GetUseMip(tex_locator) && mipmap_if_2D)
				{
					return this->LoadMipTexture(tex_locator, width_reduce, height_reduce);
				}
				else
				{
					return this->LoadPlainTexture(tex_locator, width_reduce, height_reduce);
				}
			}
			else
			{
				Warn_TextureType( i_Type );//only 2d textures supported
			}
		}
	}
	else
	{
		//	check if it is a resource, if so, return it.
		matTextureMgr::GetTextureAdjuster()->GetReduce(tex_locator, width_reduce, height_reduce);
		matPlainTexture* pPT = this->LoadResourceTexture(tex_locator, width_reduce, height_reduce);
		if (pPT != NULL)
			return pPT;
	}

	if (matTextureMgr::IsAllowNullTextures())
	{
		DBG_WARNING("No texture file could be found with a known extension at the given path: " << tex_locator);
		matTextureTracking::AddMissingTexture(tex_locator);
		return NULL;
	}
	else
	{
		DBG_ERROR("No texture file could be found with a known extension at the given path: " << tex_locator);
		throw fsFileDoesntExistX(tex_locator);
	}
}

//--------------------------------------------------------------------
//	LoadMipTexture creates a texture from the given fsLocator.
//	This can be used to load textures that are not in any type of
//	pak file (ie usual image file formats - bmp, png, tga, jpg...).
//	Mip map levels will be created for the texture.
//--------------------------------------------------------------------
matMipTexture* matTextureMgrDX11::LoadMipTexture(	const fsLocator& i_Locator,
								int i_WidthReduce,
								int i_HeightReduce)
{
	//	Find out what the file type is (and if it exists)
	//
	FileType file_type = pick_format(i_Locator);
	FileInfo file_info(file_type, i_Locator, i_WidthReduce, i_HeightReduce);

	//	Setup the matMipTexture
	//
	std::auto_ptr<matMipTexture> ret_val(new matMipTexture);

	load_mip_texture(ret_val.get(), file_info, true);

	if (g2dDX11Global::RestrictPow2Textures())
	{
 		if (!((ret_val->GetWidth() & (ret_val->GetWidth() - 1)) == 0) ||
			!((ret_val->GetHeight() & (ret_val->GetHeight() - 1)) == 0))
		{
			throw matInvalidTextureSizeX(i_Locator);
		}
	}
	matMipTexture* texture = ret_val.release();
	l_MipTextures[texture] = file_info;

	return texture;
}
void matTextureMgrDX11::ReloadMipTexture(	const fsLocator& i_Locator, matMipTexture& io_Texture)
{
	FileInfo file_info = l_MipTextures[&io_Texture];
	file_info.m_Locator = i_Locator;

	load_mip_texture(&io_Texture, file_info, true); // supports reload of all types
}

//--------------------------------------------------------------------
//	LoadCompressedTexture creates a texture from the given fsLocator.
//	This can be used to load textures that are in .dds format.
//	Mip map levels will be created for the texture.  See
//	LoadPlainTexture for a discussion of i_WidthReduce and
//	i_HeightReduce.
// Note:  The matTexture returned pointer can be a matMipTexture
//		matStaticCubeTexture, or a matVolumeTexture (which doesn't exist yet).
//--------------------------------------------------------------------
matTexture* matTextureMgrDX11::LoadCompressedTexture( const fsLocator& i_Locator, TEXTURE_TYPE i_Type /*= TEXTURE_TYPE_UNKNOWN*/, bool mipmap_if_2D /*= true */ )
{
	bool bError = false;

	// Create the texture and load the information from the g2dSurfaceLoader
	matTextureDX11 * pRet = NULL;

	// cleaned up when texture is cleaned up (via local map)
	matTextureLoaderDDS::DDSInfo* pInfo = new matTextureLoaderDDS::DDSInfo;

	int width_reduce = 0, height_reduce = 0;
	if (matTextureMgr::GetTextureAdjuster())
		matTextureMgr::GetTextureAdjuster()->GetReduce(i_Locator, width_reduce, height_reduce);

	ID3D11Resource* pTexture = NULL;
	try
	{
		pTexture = matTextureLoaderDDS::LoadDDS( *pInfo, i_Locator, width_reduce, height_reduce, mipmap_if_2D );
	}
	catch (const g2dOutOfVideoMemoryX&)
	{
		if (pInfo)
			delete pInfo;
		pInfo = NULL;
		throw g2dOutOfVideoMemoryX();
	}

	if (pTexture == NULL)
	{
		bError = true;
	}

	if( !bError )
	{
		switch ( pInfo->s_eType )
		{
		case matTextureLoaderDDS::e_CubeTexture:
			{
				if( i_Type == TEXTURE_TYPE_UNKNOWN || i_Type == TEXTURE_TYPE_CUBE )
				{
					matStaticCubeTexture* texture = new matStaticCubeTexture;

					// Set the new surface
					texture->SetSurface( pTexture );

					// set size info into the texture
					texture->ReloadInfo();

					pRet = texture;
				}
				else
				{
					Warn_TextureType( i_Type );
					bError = true;
				}
			}
			break;
		case matTextureLoaderDDS::e_MipTexture:
			{
				if( i_Type == TEXTURE_TYPE_UNKNOWN || i_Type == TEXTURE_TYPE_1D || i_Type == TEXTURE_TYPE_2D )
				{
					matMipTexture* texture = new matMipTexture;

					// Set the new surface
					texture->SetSurface( pTexture );

					// set size info into the texture
					texture->ReloadInfo();

					pRet = texture;
				}
				else
				{
					Warn_TextureType( i_Type );
					bError = true;
				}
			}
			break;
		case matTextureLoaderDDS::e_VolumeTexture:
			{
				if( i_Type == TEXTURE_TYPE_UNKNOWN || i_Type == TEXTURE_TYPE_3D )
				{
					matVolumeTexture* texture = new matVolumeTexture;

					// Set the new surface
					texture->SetSurface( pTexture );

					// set size info into the texture
					texture->ReloadInfo();

					pRet = texture;
				}
				else
				{
					Warn_TextureType( i_Type );
					bError = true;
				}
			}
			break;
		default:
			{
				bError = true;
			}
			break;
		}
	}

	if( bError )
	{
		if (pTexture)
			pTexture->Release();

		delete pInfo;	//make sure to delete this as we aren't adding a texture reference.
		std::string fullfilename;
		fsFileUtil::LocatorToANSIFilename(i_Locator,fullfilename);
		if (matTextureMgr::IsAllowNullTextures())
		{
			DBG_WARNING("The texture file could not be loaded at the given path: " << fullfilename);
			matTextureTracking::AddMissingTexture(i_Locator);
			return NULL;
		}
		else
		{
			DBG_ERROR("The texture file could not be loaded at the given path: " << fullfilename);
			throw fsUnknownX(i_Locator);
		}
	}

	// reuse the type for our needs
	pInfo->s_eType = e_DDS;

	// TODO:  Texture reduction

	// TODO:  do we care about pixel format information?

	l_CompressedTextures[pRet] = pInfo;

	return pRet;
}

//--------------------------------------------------------------------
// ReloadCompressedTexture reloads the given texture file into the
// given texture object.  If the attributes don't match up, this will
// crash.
//--------------------------------------------------------------------
void matTextureMgrDX11::ReloadCompressedTexture( const fsLocator& i_Locator, matTextureDX11* io_Texture, bool i_bIsMipMap)
{
	bool bError = false;

	//DBG_ASSERT(false, "matTextureMgrDX11::ReloadCompressedTexture not implemented");
	//DBG_ERROR("matTextureMgrDX11::ReloadCompressedTexture not implemented");
	matTextureLoaderDDS::DDSInfo* pInfo = l_CompressedTextures[io_Texture];
	pInfo->s_Locator = i_Locator;

	ID3D11Resource* pTexture = NULL;
	try
	{
		pTexture = matTextureLoaderDDS::LoadDDS( *pInfo, i_Locator, 0, 0, i_bIsMipMap );
	}
	catch (const g2dOutOfVideoMemoryX&)
	{
		if (pInfo)
			delete pInfo;
		pInfo = NULL;
		throw g2dOutOfVideoMemoryX();
	}

	if (pTexture == NULL)
	{
		bError = true;
	}

	if( !bError )
	{
		switch ( pInfo->s_eType )
		{
		case matTextureLoaderDDS::e_CubeTexture:
			{
				//if( i_Type == TEXTURE_TYPE_UNKNOWN || i_Type == TEXTURE_TYPE_CUBE )
				{
					matStaticCubeTexture* texture = dynamic_cast<matStaticCubeTexture*>(io_Texture);

					if (texture)
					{
						// Set the new surface
						texture->SetSurface( pTexture );

						// set size info into the texture
						texture->ReloadInfo();

						//pRet = texture;
					}
				}
				/*else
				{
					Warn_TextureType( i_Type );
					bError = true;
				}*/
			}
			break;
		case matTextureLoaderDDS::e_MipTexture:
			{
				//if( i_Type == TEXTURE_TYPE_UNKNOWN || i_Type == TEXTURE_TYPE_1D || i_Type == TEXTURE_TYPE_2D )
				{
					matMipTexture* texture = dynamic_cast<matMipTexture*>(io_Texture);

					if (texture)
					{
						// Set the new surface
						texture->SetSurface( pTexture );

						// set size info into the texture
						texture->ReloadInfo();

						//pRet = texture;
					}
				}
				/*else
				{
					Warn_TextureType( i_Type );
					bError = true;
				}*/
			}
			break;
		case matTextureLoaderDDS::e_VolumeTexture:
			{
				//if( i_Type == TEXTURE_TYPE_UNKNOWN || i_Type == TEXTURE_TYPE_3D )
				{
					matVolumeTexture* texture = dynamic_cast<matVolumeTexture*>(io_Texture);

					if (texture)
					{
						// Set the new surface
						texture->SetSurface( pTexture );

						// set size info into the texture
						texture->ReloadInfo();

						//pRet = texture;
					}
				}
				/*else
				{
					Warn_TextureType( i_Type );
					bError = true;
				}*/
			}
			break;
		default:
			{
				bError = true;
			}
			break;
		}
	}

	if( bError )
	{
		if (pTexture)
			pTexture->Release();

		delete pInfo;	//make sure to delete this as we aren't adding a texture reference.
		std::string fullfilename;
		fsFileUtil::LocatorToANSIFilename(i_Locator,fullfilename);
		if (matTextureMgr::IsAllowNullTextures())
		{
			DBG_WARNING("The texture file could not be loaded at the given path: " << fullfilename);
			matTextureTracking::AddMissingTexture(i_Locator);
			//return NULL;
		}
		else
		{
			DBG_ERROR("The texture file could not be loaded at the given path: " << fullfilename);
			throw fsUnknownX(i_Locator);
		}
	}

	// reuse the type for our needs
	pInfo->s_eType = e_DDS;

	// TODO:  Texture reduction

	// TODO:  do we care about pixel format information?

	l_CompressedTextures[io_Texture] = pInfo;
#if 0
	// cleaned up when texture is cleaned up (via local map)
	matTextureLoaderDDS::DDSInfo* pInfo = l_CompressedTextures[&io_Texture];
	pInfo->s_Locator = i_Locator;

	struct ID3D11Resource* pTexture = matTextureLoaderDDS::LoadDDS( *pInfo, i_Locator, 0, 0 );  // no texture reduction

	DBG_ASSERT(NULL != pTexture, "Unhandled NULL texture returned from DDS Loader.");

	switch ( pInfo->s_eType )
	{
	case matTextureLoaderDDS::e_CubeTexture:
		DBG_ASSERT( NULL != dynamic_cast<matStaticCubeTexture*>(&io_Texture), "Invalid texture type in reloaded file, types must match!");
		break;
	case matTextureLoaderDDS::e_MipTexture:
		DBG_ASSERT( NULL != dynamic_cast<matMipTexture*>(&io_Texture), "Invalid texture type in reloaded file, types must match!");
		break;
	default:
		DBG_ASSERT(false, "Unhandled DDS texture type!");
		// TODO:  throw invalid format or similar
		break;
	}

	// Set the new surface, replaces the old one
	io_Texture.SetSurface( pTexture );

	// TODO:  Texture reduction

	// TODO:  do we care about pixel format information?
#endif
}

//--------------------------------------------------------------------
//	CreateShadowMap creates a shadow map texture that can 
//	be rendered to. Use the GetRenderTargetAPI() function
//	on the returned texture in order to render to it.
//--------------------------------------------------------------------
matTexture* matTextureMgrDX11::CreateShadowMap(int i_nTextureWidth, int i_nTextureHeight)
{
	if (g2dDX11Global::RestrictPow2Textures())
	{
		if (!((i_nTextureWidth & (i_nTextureWidth - 1)) == 0) ||
			!((i_nTextureHeight & (i_nTextureHeight - 1)) == 0))
		{
			throw matInvalidTextureSizeX(fsLocator());
		}
	}

	matShadowMap* pTexture = new matShadowMap();
	
	i_nTextureWidth = max(i_nTextureWidth,1);
	i_nTextureHeight = max(i_nTextureHeight,1);

	try
	{
		pTexture->Make( i_nTextureWidth, i_nTextureHeight );
	}
	catch (const g2dOutOfVideoMemoryX& )
	{
		delete pTexture;
		throw;
	}
	catch ( const envExceptionX& i_Ex )
	{
		DBG_ERROR(i_Ex.GetErrorMessage());
		return NULL;
	}

	// update all the core data for the texture
	pTexture->ReloadInfo();

	// add memory count to shadow map category 
	l_ShadowMapTextures[pTexture] = FileInfo(i_nTextureWidth, i_nTextureHeight, pTexture->GetPixelFormat());

	return pTexture;
}

//--------------------------------------------------------------------
//	CreateShadowMap creates a reflective shadow map texture that can 
//	be rendered to. Use the GetRenderTargetAPI() function
//	on the returned texture in order to render to it.
//--------------------------------------------------------------------
matTexture* matTextureMgrDX11::CreateReflectiveShadowMap(int i_nTextureWidth, int i_nTextureHeight)
{
	if (g2dDX11Global::RestrictPow2Textures())
	{
		if (!((i_nTextureWidth & (i_nTextureWidth - 1)) == 0) ||
			!((i_nTextureHeight & (i_nTextureHeight - 1)) == 0))
		{
			throw matInvalidTextureSizeX(fsLocator());
		}
	}

	matReflectiveShadowMap* pTexture = new matReflectiveShadowMap();
	
	i_nTextureWidth = max(i_nTextureWidth,1);
	i_nTextureHeight = max(i_nTextureHeight,1);

	try
	{
		pTexture->Make( i_nTextureWidth, i_nTextureHeight );
	}
	catch (const g2dOutOfVideoMemoryX& )
	{
		delete pTexture;
		throw;
	}
	catch ( const envExceptionX& i_Ex )
	{
		DBG_ERROR(i_Ex.GetErrorMessage());
		return NULL;
	}

	// update all the core data for the texture
	pTexture->ReloadInfo();

	// add memory count to shadow map category 
	l_RSMTextures[pTexture] = FileInfo(i_nTextureWidth, i_nTextureHeight, pTexture->GetPixelFormat());
	l_RSMTextureMemory += pTexture->GetSize();

	return pTexture;
}

//--------------------------------------------------------------------
//	CreateRenderTargetTexture creates a texture that can 
//	be rendered to
//--------------------------------------------------------------------
matTexture* matTextureMgrDX11::CreateRenderTargetTexture(int i_nTextureWidth,
														int i_nTextureHeight,
														bool i_bFloatingPoint,
														const g2dPFD* i_PFD,
														bool i_bAutoGenMipmap,
														bool i_bAllocDepthBuffer,
														matTextureMgr::eResourceCategory i_ResourceCategory,
														bool i_bFloatDepth,
														bool i_bAntiAlias )
{
	if (g2dDX11Global::RestrictPow2Textures())
	{
		if (!((i_nTextureWidth & (i_nTextureWidth - 1)) == 0) ||
			!((i_nTextureHeight & (i_nTextureHeight - 1)) == 0))
		{
			throw matInvalidTextureSizeX(fsLocator());
		}
	}

	const g2dPFD& image_format = (i_PFD != NULL) ? (*i_PFD) : matD3DGlobal::TrueAlphaTextureFormat();
	matRenderTargetTexture* pTexture = new matRenderTargetTexture(i_bFloatingPoint, i_bFloatDepth);
	
	i_nTextureWidth = max(i_nTextureWidth,1);
	i_nTextureHeight = max(i_nTextureHeight,1);

	try
	{
		pTexture->Make( i_nTextureWidth, i_nTextureHeight, image_format, i_bAllocDepthBuffer, i_bAutoGenMipmap,
			GetG2DResourceCategory(i_ResourceCategory), i_bAntiAlias );
	}
	catch (const g2dOutOfVideoMemoryX& )
	{
		delete pTexture;
		throw;
	}
	catch ( const envExceptionX& i_Ex )
	{
		DBG_ERROR(i_Ex.GetErrorMessage());
		return NULL;
	}

	// update all the core data for the texture
	pTexture->ReloadInfo();

	switch(i_ResourceCategory)
	{
	case matTextureMgr::e_SceneTexture:
		l_RenderTargetTextures[pTexture] = FileInfo(i_nTextureWidth, i_nTextureHeight, image_format);
		break;
	case matTextureMgr::e_Framebuffer:
		l_FramebufferTextures[pTexture] = FileInfo(i_nTextureWidth, i_nTextureHeight, image_format);
		break;
	case matTextureMgr::e_ShadowMap:
		l_ShadowMapTextures[pTexture] = FileInfo(i_nTextureWidth, i_nTextureHeight, image_format);
		break;
	};

	return pTexture;
}

//--------------------------------------------------------------------
//	CreateTexture creates a uninitialized texture 
//--------------------------------------------------------------------
matTexture* matTextureMgrDX11::CreateTexture(int i_nTextureWidth,
	int i_nTextureHeight,
	const g2dPFD* i_PFD,
	bool i_bMipmap)
{
	return CreateTexture(i_nTextureWidth, i_nTextureHeight, i_PFD, i_bMipmap, NULL);
}

//--------------------------------------------------------------------
//	CreateTexture creates an initialized texture 
//--------------------------------------------------------------------
matTexture* matTextureMgrDX11::CreateTexture(int i_nTextureWidth,
											int i_nTextureHeight,
											const g2dPFD* i_PFD,
											bool i_bMipmap,
											void* i_PixelData)
{
	if (g2dDX11Global::RestrictPow2Textures())
	{
		if (!((i_nTextureWidth & (i_nTextureWidth - 1)) == 0) ||
			!((i_nTextureHeight & (i_nTextureHeight - 1)) == 0))
		{
			throw matInvalidTextureSizeX(fsLocator());
		}
	}

	const g2dPFD& image_format = (i_PFD != NULL) ? (*i_PFD) : matD3DGlobal::NonAlphaTextureFormat();

	matTexture* retval = NULL;

	// use auto_ptr to clean up in case of exception thrown from Make()
	if (i_bMipmap)
	{
		std::auto_ptr<matMipTexture> pTexture(new matMipTexture);
		pTexture->Make( i_nTextureWidth, i_nTextureHeight, image_format, 0, i_PixelData );
		pTexture->ReloadInfo();
		matMipTexture* pRaw = pTexture.release();
		l_MipTextures[pRaw] = FileInfo(i_nTextureWidth, i_nTextureHeight, image_format);
		retval = pRaw;
	}
	else
	{
		std::auto_ptr<matPlainTexture> pTexture(new matPlainTexture);
		pTexture->Make( i_nTextureWidth, i_nTextureHeight, image_format, i_PixelData );
		pTexture->ReloadInfo();
		matPlainTexture* pRaw = pTexture.release(); 
		l_PlainTextures[pRaw] = FileInfo(i_nTextureWidth, i_nTextureHeight, image_format);
		retval = pRaw;
	}

	return retval;
}

//--------------------------------------------------------------------
//	CreateTexture3D creates a uninitialized volume texture 
//--------------------------------------------------------------------
matTexture* matTextureMgrDX11::CreateTexture3D(int i_nTextureWidth,
											   int i_nTextureHeight,
											   int i_nTextureDepth,
											   const g2dPFD* i_PFD,
											   BIND_TYPE i_Bindings )
{
	return CreateTexture3D(i_nTextureWidth, i_nTextureHeight, i_nTextureDepth, i_PFD, i_Bindings, NULL);
}

//--------------------------------------------------------------------
//	CreateTexture3D creates an initialized texture 
//--------------------------------------------------------------------
matTexture* matTextureMgrDX11::CreateTexture3D(int i_nTextureWidth,
											 int i_nTextureHeight,
											 int i_nTextureDepth,
											 const g2dPFD* i_PFD,
											 BIND_TYPE i_Bindings,
											 void* i_PixelData)
{
	if (g2dDX11Global::RestrictPow2Textures())
	{
		if (!((i_nTextureWidth & (i_nTextureWidth - 1)) == 0) ||
			!((i_nTextureHeight & (i_nTextureHeight - 1)) == 0))
		{
			throw matInvalidTextureSizeX(fsLocator());
		}
	}

	const g2dPFD& image_format = (i_PFD != NULL) ? (*i_PFD) : matD3DGlobal::NonAlphaTextureFormat();

	matTexture* retval = NULL;

	std::auto_ptr<matVolumeTexture> pTexture(new matVolumeTexture);
	pTexture->Make( i_nTextureWidth, i_nTextureHeight, i_nTextureDepth, image_format, i_Bindings, i_PixelData );
	pTexture->ReloadInfo();
	matVolumeTexture* pRaw = pTexture.release(); 
	retval = pRaw;

	return retval;
}

//--------------------------------------------------------------------
//	CopyTexture copy one texture to another
//--------------------------------------------------------------------
void matTextureMgrDX11::CopyTexture(matTexture* i_SrcTex, matTexture* i_DstTex)
{
	// only support matRenderTargetCopying
	matRenderTargetTexture* src = dynamic_cast<matRenderTargetTexture*>(i_SrcTex);
	matRenderTargetTexture* dst = dynamic_cast<matRenderTargetTexture*>(i_DstTex);

	if (!src || !dst)
	{
		DBG_WARNING("matTextureMgr::CopyTexture only supports matRenderTarget texture copying");
		return;
	}
	if (!src->GetResource() || !dst->GetResource())
		return;
	
	D3DX11_TEXTURE_LOAD_INFO load_info;
	HRESULT op_result;
	op_result = D3DX11LoadTextureFromTexture(g2dDX11Global::g_pDeviceContext,
												src->GetResource(),
												&load_info,
												dst->GetResource());

	if ( op_result != S_OK )
	{
		g2dDX11Global::PrintDXError(op_result);
		DBG_ASSERT(op_result == S_OK, "Error creating temporary texture");
	}
}

//--------------------------------------------------------------------
//	CreateRenderTargetTexture creates a texture that can 
//	be rendered to. The texture is initialized using data from the 
//	input texture.
//--------------------------------------------------------------------
matTexture* matTextureMgrDX11::CreateRenderTargetTexture(matTexture* i_Texture,
														bool i_bAutoGenMipmap,
														matTextureMgr::eResourceCategory i_ResourceCategory)
{
	matTextureDX11* textured3d = dynamic_cast<matTextureDX11*>(i_Texture);
	DBG_ASSERT(textured3d, "Bad source texture passed in to CreateRenderTargetTexture");
	
	// note: "floating point" (depth map) source texture not supported!
	matTexture* pTexture = CreateRenderTargetTexture(i_Texture->GetWidth(),
		i_Texture->GetHeight(), false, &i_Texture->GetPixelFormat(), i_bAutoGenMipmap, true, i_ResourceCategory, false);
	matTextureDX11* dsttextured3d = dynamic_cast<matTextureDX11*>(pTexture);
	DBG_ASSERT(dsttextured3d, "Bad destination texture created in CreateRenderTargetTexture");

	// assumes compatibility for this copy operation!
	g2dD3D11ResourcePtr dstSurface = dsttextured3d->GetResource();
	g2dD3D11ResourcePtr srcSurface = textured3d->GetResource();

	g2dDX11Global::g_pDeviceContext->CopyResource(dstSurface, srcSurface);

	return pTexture;
}

//--------------------------------------------------------------------
//	CreateCubeRenderTargetTexture creates a cubemap texture that can 
//	be rendered to. 
//--------------------------------------------------------------------
matTexture* matTextureMgrDX11::CreateCubeRenderTargetTexture(int i_nTextureWidth,
													int i_nTextureHeight,
													g2dPFD* i_PFD)
{
	if (g2dDX11Global::RestrictPow2Textures())
	{
		if (!((i_nTextureWidth & (i_nTextureWidth - 1)) == 0) ||
			!((i_nTextureHeight & (i_nTextureHeight - 1)) == 0))
		{
			throw matInvalidTextureSizeX(fsLocator());
		}
	}

	const g2dPFD& image_format = (i_PFD != NULL) ? (*i_PFD) : matD3DGlobal::TrueAlphaTextureFormat();
	matCubeRenderTargetTexture* pTexture = new matCubeRenderTargetTexture();
	
	try
	{
		pTexture->Make(i_nTextureWidth, i_nTextureHeight, image_format);
	}
	catch (const g2dOutOfVideoMemoryX& )
	{
		delete pTexture;
		throw;
	}
	catch ( const envExceptionX& i_Ex )
	{
		DBG_ERROR(i_Ex.GetErrorMessage());
		return NULL;
	}

	l_RenderTargetTextures[pTexture] = FileInfo(i_nTextureWidth, i_nTextureHeight, image_format);
	pTexture->ReloadInfo();

	return pTexture;
}

//--------------------------------------------------------------------
//	DestroyTexture causes the texture object to be destroyed and
//	the texture information to be removed from memory.
//--------------------------------------------------------------------
void matTextureMgrDX11::DestroyTexture(matTexture* i_Texture)
{
	// First look it up in the compressed texture map, since a compressed texture
	// can be any one of the ones below.  If it isn't compressed, then we can
	// destroy the normal way.
	matTextureDX11* tex_d3d = dynamic_cast<matTextureDX11*>(i_Texture);
	CompressedTextures::iterator it = l_CompressedTextures.find( tex_d3d );
	if ( l_CompressedTextures.end() != it )
	{
		// compressed, destroy it
		delete i_Texture;
		delete it->second; // compressed textures need to delete their file info objects
		l_CompressedTextures.erase(it);
	}
	else
	{
		matPlainTexture* plain_tex = dynamic_cast<matPlainTexture*>(i_Texture);
		if( plain_tex )
		{
			std::map<matPlainTexture*, FileInfo>::iterator it = l_PlainTextures.find(plain_tex);
			DBG_ASSERT(it != l_PlainTextures.end(), "Texture not found in list (did you create it with the matTextureMgr?)");
			l_PlainTextures.erase(it);
			delete plain_tex;
		}
		else
		{
			matMipTexture* mip_tex = dynamic_cast<matMipTexture*>(i_Texture);
			if( mip_tex )
			{
				std::map<matMipTexture*, FileInfo>::iterator it = l_MipTextures.find(mip_tex);
				DBG_ASSERT(it != l_MipTextures.end(), "Texture not found in list (did you create it with the matTextureMgr?)");
				l_MipTextures.erase(it);
				delete mip_tex;
			}
			else
			{
				matShadowMap* shadow_map_tex = dynamic_cast<matShadowMap*>(i_Texture);
				matReflectiveShadowMap* reflective_shadow_map_tex = dynamic_cast<matReflectiveShadowMap*>(i_Texture);
				if (shadow_map_tex)
				{
					std::map<matTextureDX11*, FileInfo>::iterator itSh;
					itSh = l_ShadowMapTextures.find(shadow_map_tex);
					if (itSh != l_ShadowMapTextures.end())
					{
						l_ShadowMapTextures.erase(itSh);
						delete shadow_map_tex;
					}
				}
				else if(reflective_shadow_map_tex)
				{
					std::map<matTextureDX11*, FileInfo>::iterator itSh;
					itSh = l_RSMTextures.find(reflective_shadow_map_tex);
					if (itSh != l_RSMTextures.end())
					{
						l_RSMTextures.erase(itSh);
						l_RSMTextureMemory -= tex_d3d->GetSize();
						delete reflective_shadow_map_tex;
					}
				}
				else
				{
					// rendertargettex can be in any of 3 categories: framebuffer, rendertarget, or shadowmap.
					matRenderTargetTexture* render_target_tex = dynamic_cast<matRenderTargetTexture*>(i_Texture);
					if( render_target_tex )
					{
						std::map<matRenderTargetTexture*, FileInfo>::iterator it;
						it = l_RenderTargetTextures.find(render_target_tex);
						if (it != l_RenderTargetTextures.end())
						{
							l_RenderTargetTextures.erase(it);
							delete render_target_tex;
						}
						else
						{
							std::map<matTextureDX11*, FileInfo>::iterator itSh;
							itSh = l_ShadowMapTextures.find(render_target_tex);
							if (itSh != l_ShadowMapTextures.end())
							{
								l_ShadowMapTextures.erase(itSh);
								delete render_target_tex;
							}
							else
							{
								it = l_FramebufferTextures.find(render_target_tex);
								if (it != l_FramebufferTextures.end())
								{
									l_FramebufferTextures.erase(it);
									delete render_target_tex;
								}
								else
								{
									DBG_ASSERT(false, "Texture not found in list (did you create it with the matTextureMgr?)");
								}
							}
						}
					}
				}
			}
		}
	} // else, not compressed
}

//--------------------------------------------------------------------
//	DestroyAllTextures causes all textures to be destroyed.
//--------------------------------------------------------------------
void matTextureMgrDX11::DestroyAllTextures()
{
	{
		std::map<matPlainTexture*, FileInfo>::iterator it = l_PlainTextures.begin();
		std::map<matPlainTexture*, FileInfo>::iterator end = l_PlainTextures.end();

		while( it != end )
		{
			delete (*it).first;
			++it;
		}

		l_PlainTextures.erase(l_PlainTextures.begin(), end);
	}

	{
		std::map<matMipTexture*, FileInfo>::iterator it = l_MipTextures.begin();
		std::map<matMipTexture*, FileInfo>::iterator end = l_MipTextures.end();

		while( it != end )
		{
			delete (*it).first;
			++it;
		}

		l_MipTextures.erase(l_MipTextures.begin(), end);
	}

	{
		std::map<matRenderTargetTexture*, FileInfo>::iterator it = l_RenderTargetTextures.begin();
		std::map<matRenderTargetTexture*, FileInfo>::iterator end = l_RenderTargetTextures.end();

		while( it != end )
		{
			delete (*it).first;
			++it;
		}

		l_RenderTargetTextures.erase(l_RenderTargetTextures.begin(), end);

	}
	{
		std::map<matTextureDX11*, FileInfo>::iterator it = l_ShadowMapTextures.begin();
		std::map<matTextureDX11*, FileInfo>::iterator end = l_ShadowMapTextures.end();

		while( it != end )
		{
			delete (*it).first;
			++it;
		}

		l_ShadowMapTextures.erase(l_ShadowMapTextures.begin(), end);
	}
	{
		std::map<matTextureDX11*, FileInfo>::iterator it = l_RSMTextures.begin();
		std::map<matTextureDX11*, FileInfo>::iterator end = l_RSMTextures.end();

		while( it != end )
		{
			delete (*it).first;
			++it;
		}

		l_RSMTextures.erase(l_RSMTextures.begin(), end);
		l_RSMTextureMemory = 0;
	}
	{
		std::map<matRenderTargetTexture*, FileInfo>::iterator it = l_FramebufferTextures.begin();
		std::map<matRenderTargetTexture*, FileInfo>::iterator end = l_FramebufferTextures.end();

		while( it != end )
		{
			delete (*it).first;
			++it;
		}

		l_FramebufferTextures.erase(l_FramebufferTextures.begin(), end);
	}
}

//--------------------------------------------------------------------
//	ReloadTextures reloads the appropriate texture
//--------------------------------------------------------------------
void matTextureMgrDX11::ReloadTexture( const fsLocator& i_Locator, bool i_bIsMipMap )
{
	{
		std::map<matPlainTexture*, FileInfo>::iterator it = l_PlainTextures.begin();
		std::map<matPlainTexture*, FileInfo>::iterator end = l_PlainTextures.end();
		while( it != end )
		{
			if( it->second.m_Locator == i_Locator )
			{
				load_plain_texture(it->first, it->second);
				return;
			}
			++it;
		}
	}

	{
		std::map<matMipTexture*, FileInfo>::iterator it = l_MipTextures.begin();
		std::map<matMipTexture*, FileInfo>::iterator end = l_MipTextures.end();
		while( it != end )
		{
			if( it->second.m_Locator == i_Locator )
			{
				load_mip_texture(it->first, it->second,i_bIsMipMap);
				return;
			}
			++it;
		}
	}

	{
		CompressedTextures::iterator it = l_CompressedTextures.begin();
		CompressedTextures::iterator end = l_CompressedTextures.end();
		while( it != end )
		{
			if( it->second->s_Locator == i_Locator )
			{
				ReloadCompressedTexture(it->second->s_Locator, (it->first),i_bIsMipMap);
				return;
			}
			++it;
		}
	}

	{
		std::map<matRenderTargetTexture*, FileInfo>::iterator it = l_RenderTargetTextures.begin();
		std::map<matRenderTargetTexture*, FileInfo>::iterator end = l_RenderTargetTextures.end();
		while( it != end )
		{
			if( it->second.m_Locator == i_Locator )
			{
				load_render_target_texture(it->first, it->second);
				return;
			}
			++it;
		}
	}
	{
		std::map<matTextureDX11*, FileInfo>::iterator it = l_ShadowMapTextures.begin();
		std::map<matTextureDX11*, FileInfo>::iterator end = l_ShadowMapTextures.end();
		while( it != end )
		{
			if( it->second.m_Locator == i_Locator )
			{
				DBG_ASSERT(false, "Not able to reload shadowmaps yet");
				//load_render_target_texture(it->first, it->second);
				return;
			}
			++it;
		}
	}
	{
		std::map<matTextureDX11*, FileInfo>::iterator it = l_RSMTextures.begin();
		std::map<matTextureDX11*, FileInfo>::iterator end = l_RSMTextures.end();
		while( it != end )
		{
			if( it->second.m_Locator == i_Locator )
			{
				DBG_ASSERT(false, "Not able to reload rs maps yet");
				//load_render_target_texture(it->first, it->second);
				return;
			}
			++it;
		}
	}
	{
		std::map<matRenderTargetTexture*, FileInfo>::iterator it = l_FramebufferTextures.begin();
		std::map<matRenderTargetTexture*, FileInfo>::iterator end = l_FramebufferTextures.end();
		while( it != end )
		{
			if( it->second.m_Locator == i_Locator )
			{
				load_render_target_texture(it->first, it->second);
				return;
			}
			++it;
		}
	}
}


//--------------------------------------------------------------------
//	UnloadTextures resets the d3d surface of a texture
//--------------------------------------------------------------------
void matTextureMgrDX11::UnloadTexture( const fsLocator& i_Locator )
{
	{
		std::map<matPlainTexture*, FileInfo>::iterator it = l_PlainTextures.begin();
		std::map<matPlainTexture*, FileInfo>::iterator end = l_PlainTextures.end();
		while( it != end )
		{
			if( it->second.m_Locator == i_Locator )
			{
//				it->first->UnloadSurface();
				return;
			}
			++it;
		}
	}
	
	{
		std::map<matMipTexture*, FileInfo>::iterator it = l_MipTextures.begin();
		std::map<matMipTexture*, FileInfo>::iterator end = l_MipTextures.end();
		while( it != end )
		{
			if( it->second.m_Locator == i_Locator )
			{
//				it->first->UnloadSurface();
				return;
			}
			++it;
		}
	}
	{
		CompressedTextures::iterator it = l_CompressedTextures.begin();
		CompressedTextures::iterator end = l_CompressedTextures.end();
		while( it != end )
		{
			if( it->second->s_Locator == i_Locator )
			{
//				it->first->UnloadSurface();
				return;
			}
			++it;
		}
	}

	{
		std::map<matRenderTargetTexture*, FileInfo>::iterator it = l_RenderTargetTextures.begin();
		std::map<matRenderTargetTexture*, FileInfo>::iterator end = l_RenderTargetTextures.end();
		while( it != end )
		{
			if( it->second.m_Locator == i_Locator )
			{
//				it->first->UnloadSurface();
				return;
			}
			++it;
		}
	}
	{
		std::map<matTextureDX11*, FileInfo>::iterator it = l_ShadowMapTextures.begin();
		std::map<matTextureDX11*, FileInfo>::iterator end = l_ShadowMapTextures.end();
		while( it != end )
		{
			if( it->second.m_Locator == i_Locator )
			{
//				it->first->UnloadSurface();
				return;
			}
			++it;
		}
	}
	{
		std::map<matTextureDX11*, FileInfo>::iterator it = l_RSMTextures.begin();
		std::map<matTextureDX11*, FileInfo>::iterator end = l_RSMTextures.end();
		while( it != end )
		{
			if( it->second.m_Locator == i_Locator )
			{
//				it->first->UnloadSurface();
				return;
			}
			++it;
		}
	}
	{
		std::map<matRenderTargetTexture*, FileInfo>::iterator it = l_FramebufferTextures.begin();
		std::map<matRenderTargetTexture*, FileInfo>::iterator end = l_FramebufferTextures.end();
		while( it != end )
		{
			if( it->second.m_Locator == i_Locator )
			{
//				it->first->UnloadSurface();
				return;
			}
			++it;
		}
	}

}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void matTextureMgrDX11::SaveTextureToRgbaTiff(matTexture* i_pTexture, 
	const fsLocator& i_FilePathLocator)
{
	matTextureDX11* textured3d = dynamic_cast<matTextureDX11*>(i_pTexture);
	DBG_ASSERT(textured3d, "Bad source texture passed in to SaveTextureToFile");

	itString filename;
	fsFileUtil::LocatorToUnicodeString(i_FilePathLocator, filename);

	HRESULT hr;
	D3D11_SHADER_RESOURCE_VIEW_DESC srcDesc;
	
	if ( !textured3d->GetSurface() ) 
	{
		DBG_TRACE( "Texture not found in memory. Aborting save. ");
		return;
	}
	textured3d->GetSurface()->GetDesc(&srcDesc);

	ID3D11Texture2D * tex = NULL;
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = textured3d->GetWidth();
	desc.Height = textured3d->GetHeight();
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	desc.MiscFlags = 0;
	hr = g2dDX11Global::g_pDevice->CreateTexture2D( &desc, NULL, &tex );

	D3DX11_TEXTURE_LOAD_INFO texInfo;
	texInfo.pSrcBox = NULL;
	texInfo.pDstBox = NULL;
	texInfo.SrcFirstMip = 0;
	texInfo.DstFirstMip = 0;
	texInfo.NumMips = D3DX11_DEFAULT;
	texInfo.SrcFirstElement = 0;
	texInfo.DstFirstElement = 0;
	texInfo.NumElements = D3DX11_DEFAULT;
	texInfo.Filter = D3DX11_FILTER_TRIANGLE;
	texInfo.MipFilter = D3DX11_DEFAULT;
	hr = D3DX11LoadTextureFromTexture(g2dDX11Global::g_pDeviceContext,textured3d->GetResource(),&texInfo,tex);
	hr = D3DX11SaveTextureToFile(g2dDX11Global::g_pDeviceContext, tex, D3DX11_IFF_TIFF, filename.GetString());

	tex->Release();
	tex = NULL;

	if (!SUCCEEDED(hr))
	{
		DBG_TRACE( "D3DX11SaveTextureToFile Return Error result: " << hr );
		//throw g2dImageSaveX();
	}

}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void matTextureMgrDX11::SaveTextureToRgbaPNG(matTexture* i_pTexture, 
	const fsLocator& i_FilePathLocator)
{
	matTextureDX11* textured3d = dynamic_cast<matTextureDX11*>(i_pTexture);
	DBG_ASSERT(textured3d, "Bad source texture passed in to SaveTextureToFile");

	itString filename;
	fsFileUtil::LocatorToUnicodeString(i_FilePathLocator, filename);

	HRESULT hr;
	D3D11_SHADER_RESOURCE_VIEW_DESC srcDesc;
	
	if ( !textured3d->GetSurface() ) 
	{
		DBG_TRACE( "Texture not found in memory. Aborting save. ");
		return;
	}
	textured3d->GetSurface()->GetDesc(&srcDesc);

	ID3D11Texture2D * tex = NULL;
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = textured3d->GetWidth();
	desc.Height = textured3d->GetHeight();
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	desc.MiscFlags = 0;
	hr = g2dDX11Global::g_pDevice->CreateTexture2D( &desc, NULL, &tex );

	D3DX11_TEXTURE_LOAD_INFO texInfo;
	texInfo.pSrcBox = NULL;
	texInfo.pDstBox = NULL;
	texInfo.SrcFirstMip = 0;
	texInfo.DstFirstMip = 0;
	texInfo.NumMips = D3DX11_DEFAULT;
	texInfo.SrcFirstElement = 0;
	texInfo.DstFirstElement = 0;
	texInfo.NumElements = D3DX11_DEFAULT;
	texInfo.Filter = D3DX11_FILTER_TRIANGLE;
	texInfo.MipFilter = D3DX11_DEFAULT;
	hr = D3DX11LoadTextureFromTexture(g2dDX11Global::g_pDeviceContext,textured3d->GetResource(),&texInfo,tex);
	hr = D3DX11SaveTextureToFile(g2dDX11Global::g_pDeviceContext, tex, D3DX11_IFF_PNG, filename.GetString());

	tex->Release();
	tex = NULL;

	if (!SUCCEEDED(hr))
	{
		DBG_TRACE( "D3DX11SaveTextureToFile Return Error result: " << hr );
		//throw g2dImageSaveX();
	}

}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void matTextureMgrDX11::SaveTextureToFile(matTexture* i_pTexture, 
	const fsLocator& i_FilePathLocator)
{
	matTextureDX11* textured3d = dynamic_cast<matTextureDX11*>(i_pTexture);
	DBG_ASSERT(textured3d, "Bad source texture passed in to SaveTextureToFile");

	//	decide what kind of file format to output
	itString fname = i_FilePathLocator.GetLastName();
	itString ext;
	fname.GetExtension(ext);
	D3DX11_IMAGE_FILE_FORMAT format = D3DX11_IFF_DDS;
	if ((ext == itString("bmp")) || (ext == itString("BMP")))
	{
		format = D3DX11_IFF_BMP;
	}
	else if ((ext == itString("jpg")) || (ext == itString("JPG")) || (ext == itString("JPEG")) || (ext == itString("jpeg")))
	{
		format = D3DX11_IFF_JPG;
	}
	else if ((ext == itString("png")) || (ext == itString("PNG")))
	{
		format = D3DX11_IFF_PNG;
	}
	else if ((ext == itString("dds")) || (ext == itString("DDS")))
	{
		format = D3DX11_IFF_DDS;
	}
	else if ((ext == itString("tif")) || (ext == itString("TIF")) || (ext == itString("TIFF")) || (ext == itString("tiff")))
	{
		format = D3DX11_IFF_TIFF;
		SaveTextureToRgbaTiff(i_pTexture, i_FilePathLocator);
		return;
	}

	itString filename;
	fsFileUtil::LocatorToUnicodeString(i_FilePathLocator, filename);

	HRESULT hr;
	D3D11_SHADER_RESOURCE_VIEW_DESC srcDesc;
	textured3d->GetSurface()->GetDesc(&srcDesc);

	if ( g2dDX11Global::IsFormatCompressed( srcDesc.Format ) )
	{
		ID3D11Texture2D * tex = NULL;
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = textured3d->GetWidth();
		desc.Height = textured3d->GetHeight();
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		desc.MiscFlags = 0;
		hr = g2dDX11Global::g_pDevice->CreateTexture2D( &desc, NULL, &tex );

		D3DX11_TEXTURE_LOAD_INFO texInfo;
		texInfo.pSrcBox = NULL;
		texInfo.pDstBox = NULL;
		texInfo.SrcFirstMip = 0;
		texInfo.DstFirstMip = 0;
		texInfo.NumMips = D3DX11_DEFAULT;
		texInfo.SrcFirstElement = 0;
		texInfo.DstFirstElement = 0;
		texInfo.NumElements = D3DX11_DEFAULT;
		texInfo.Filter = D3DX11_DEFAULT;
		texInfo.MipFilter = D3DX11_DEFAULT;
		hr = D3DX11LoadTextureFromTexture(g2dDX11Global::g_pDeviceContext,textured3d->GetResource(),&texInfo,tex);
		hr = D3DX11SaveTextureToFile(g2dDX11Global::g_pDeviceContext, tex, format, filename.GetString());

		tex->Release();
		tex = NULL;
	}
	else
	{
		hr = D3DX11SaveTextureToFile(g2dDX11Global::g_pDeviceContext, textured3d->GetResource(), format, filename.GetString());
	}

	if (!SUCCEEDED(hr))
	{
		DBG_TRACE( "D3DX11SaveTextureToFile Return Error result: " << hr );
		//throw g2dImageSaveX();
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void matTextureMgrDX11::FillTexture(matTexture* i_pTexture, const maFloatRGBA& i_Color)
{
	// this might have to get converted to (BYTE)(clamp(color,0,1) * 255) 
	// depending on pixelformat 

	// expand
	int size = i_pTexture->GetWidth() * i_pTexture->GetHeight() * 4;
	float* pixels = new float[size];
	for (int i = 0; i < i_pTexture->GetWidth() * i_pTexture->GetHeight(); i++)
	{
		pixels[i*4 + 0] = i_Color.GetRed();
		pixels[i*4 + 1] = i_Color.GetGreen();
		pixels[i*4 + 2] = i_Color.GetBlue();
		pixels[i*4 + 3] = i_Color.GetAlpha();
	}

	// pass in number of bytes
	this->UpdateSurface(i_pTexture, (unsigned char*)pixels, size * sizeof(float) );

	delete [] pixels;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void matTextureMgrDX11::FillTexture(matTexture* i_pTexture, void* i_PixelData, int i_nByte)
{
	// pass in number of bytes
	this->UpdateSurface(i_pTexture, (unsigned char*)i_PixelData, i_nByte );
}

void matTextureMgrDX11::MergeTransparentTextures(matTexture* i_pTexO, matTexture* i_pTexI, g2dRenderTarget* io_pTarget)
{
	io_pTarget->MakeCurrent();
	
	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	g3dDX11Util::CopyTexToTarget( i_pTexO, io_pTarget );						//copy existing texture into target

	g3dBlendStateMgr::SetBlendState(st_Blend);
	
	g3dDX11Util::CopyTexToTarget( i_pTexI, io_pTarget );						//blend with new texture

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
}

//--------------------------------------------------------------------
//	LoadPlainTexture creates a texture from the given fsLocator.
//	This can be used to load textures that are not in any type of
//	pak file (ie usual image file formats - bmp, png, tga, jpg...).
//--------------------------------------------------------------------
matPlainTexture* matTextureMgrDX11::LoadPlainTexture(	const fsLocator& i_Locator,
									int i_WidthReduce,
									int i_HeightReduce)
{
	//	Find out what the file type is (and if it exists)
	//
	FileType file_type = pick_format(i_Locator);
	FileInfo file_info(file_type, i_Locator, i_WidthReduce, i_HeightReduce);

	//	Setup the matPlainTexture
	//
	std::auto_ptr<matPlainTexture> ret_val(new matPlainTexture);

	load_plain_texture(ret_val.get(), file_info);

	if (g2dDX11Global::RestrictPow2Textures())
	{
		if (!((ret_val->GetWidth() & (ret_val->GetWidth() - 1)) == 0) ||
			!((ret_val->GetHeight() & (ret_val->GetHeight() - 1)) == 0))
		{
			throw matInvalidTextureSizeX(i_Locator);
		}
	}

	matPlainTexture* texture = ret_val.release();
	l_PlainTextures[texture] = file_info;
	return texture;
}

//--------------------------------------------------------------------
//	ReloadPlainTexture will load the data in the i_Locator file into
//	the given i_Texture.  The dimensions must be the same or an
//	exception will be thrown.  Also, there should be no width
//	or height reduction on the original texture.
//--------------------------------------------------------------------
void matTextureMgrDX11::ReloadPlainTexture(matPlainTexture* io_Texture,
						const fsLocator& i_Locator)
{
	//	find the record of the texture
	std::map<matPlainTexture*, FileInfo>::iterator it = l_PlainTextures.find(io_Texture);
	DBG_ASSERT(it != l_PlainTextures.end(), "Couldn't find existing texture to load into");
	FileInfo& file_info = it->second;

	file_info.m_Locator = i_Locator;

	// All reloads for all types are supported via load
	load_plain_texture(io_Texture, file_info);
}

//--------------------------------------------------------------------
//	LoadResourceTexture creates a texture from the given fsLocator.
//	The locator is assumed to be the ID number in string form (e.g. "102")
//--------------------------------------------------------------------
//static 
matPlainTexture* matTextureMgrDX11::LoadResourceTexture(const fsLocator& i_Locator,
														int i_WidthReduce, 
														int i_HeightReduce)
{
	//	Find out what the file type is (and if it exists)
	//
	FileType file_type = e_RESOURCE;
	FileInfo file_info(file_type, i_Locator, i_WidthReduce, i_HeightReduce);

	//	Setup the matPlainTexture
	//
	std::auto_ptr<matPlainTexture> ret_val(new matPlainTexture);

	load_plain_texture(ret_val.get(), file_info);

	if (g2dDX11Global::RestrictPow2Textures())
	{
		if (!((ret_val->GetWidth() & (ret_val->GetWidth() - 1)) == 0) ||
			!((ret_val->GetHeight() & (ret_val->GetHeight() - 1)) == 0))
		{
			throw matInvalidTextureSizeX(i_Locator);
		}
	}

	matPlainTexture* texture = ret_val.release();
	l_PlainTextures[texture] = file_info;

	return texture;
}

//--------------------------------------------------------------------
//	ReloadResourceTexture will load the data in the i_Locator file into
//	the given i_Texture.  The dimensions must be the same or an
//	exception will be thrown.  Also, there should be no width
//	or height reduction on the original texture.
//--------------------------------------------------------------------
//static 
void matTextureMgrDX11::ReloadResourceTexture(matPlainTexture* io_Texture,
											 const fsLocator& i_Locator)
{
	ReloadPlainTexture( io_Texture, i_Locator );
}

void matTextureMgrDX11::InitStates()
{
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_ZERO,D3D11_BLEND_SRC_COLOR,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ZERO,D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL);
	st_Blend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ZERO,D3D11_BLEND_SRC_COLOR,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ZERO,D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL);

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void matTextureMgrDX11::CleanupStates()
{
	SAFE_DELETE( st_NoBlend );
	SAFE_DELETE( st_Blend );
	SAFE_DELETE( ds_Disable_NS );
	SAFE_DELETE( ds_Test_Write_LessE_NS );
}