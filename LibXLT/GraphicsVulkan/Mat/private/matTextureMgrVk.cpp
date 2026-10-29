/****************************************************************************\
**  matTextureMgrVk.cpp
**
**	See matTextureMgrVk.hpp.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_MATTEXTUREMGRVK_HPP
#include "GraphicsVulkan/Mat/matTextureMgrVk.hpp"
#endif
#ifndef GVK_MATCUBERENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matCubeRenderTargetTexture.hpp"
#endif
#ifndef GVK_MATMIPTEXTURE_HPP
#include "GraphicsVulkan/Mat/matMipTexture.hpp"
#endif
#ifndef GVK_MATPLAINTEXTURE_HPP
#include "GraphicsVulkan/Mat/matPlainTexture.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif
#ifndef GVK_MATSTATICCUBETEXTURE_HPP
#include "GraphicsVulkan/Mat/matStaticCubeTexture.hpp"
#endif
#ifndef GVK_MATVOLUMETEXTURE_HPP
#include "GraphicsVulkan/Mat/matVolumeTexture.hpp"
#endif
#ifndef GVK_MATREFLECTIVESHADOWMAP_HPP
#include "GraphicsVulkan/Mat/matReflectiveShadowMap.hpp"
#endif
#ifndef GVK_MATVKGLOBAL_HPP
#include "GraphicsVulkan/Mat/matVkGlobal.hpp"
#endif
#ifndef GVK_MATTEXTURELOADER_HPP
#include "GraphicsVulkan/Mat/private/matTextureLoader.hpp"
#endif
#ifndef GVK_MATSHADOWMAP_HPP
#include "GraphicsVulkan/Mat/matShadowMap.hpp"
#endif
#ifndef GVK_G3D_VKUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkUtil.hpp"
#endif
#ifndef GVK_G3DBLENDSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dBlendStateMgr.hpp"
#endif
#ifndef GVK_G3DDEPTHSTENCILSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dDepthStencilStateMgr.hpp"
#endif
#ifndef GVK_G3DRASTERIZERSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dRasterizerStateMgr.hpp"
#endif
#ifndef GVK_G3D_DRAWSTYLEUTILVK_HPP
#include "GraphicsVulkan/G3d/g3dDrawStyleUtilVk.hpp"
#endif
#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/G2d/g2dRenderTarget.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef GVK_VKDFORMAT_HPP
#include "GraphicsVulkan/vkd/vkdFormat.hpp"
#endif
#ifndef CH_BINREADER_HPP
#include "Core/ch/chBinReader.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef FS_FILEX_HPP
#include "Core/fs/fsFileX.hpp"
#endif
#ifndef GF_FILEBIN_HPP
#include "Core/gf/gfFileBin.hpp"
#endif
#ifndef GF_FILETRANSLATIONMGR_HPP
#include "Core/gf/gfFileTranslationMgr.hpp"
#endif
#ifndef ENV_STRING_HPP
#include "Core/env/envString.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif
#ifndef G2D_EXCEPTIONX_HPP
#include "Graphics/G2d/g2dExceptionX.hpp"
#endif
#ifndef MAT_EXCEPTIONX_HPP
#include "Graphics/Mat/matExceptionX.hpp"
#endif
#ifndef MAT_TEXTURETRACKING_HPP
#include "Graphics/Mat/matTextureTracking.hpp"
#endif
#ifndef MAT_UVATEXTUREPARSEUTIL_HPP
#include "Graphics/Mat/matUVATextureParseUtil.hpp"
#endif

#include <algorithm>
#include <cstring>
#include <map>
#include <memory>
#include <mutex>
#include <set>

namespace gvk
{

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
typedef std::map<matTextureVk*, matTextureLoaderDDS::DDSInfo*> CompressedTextures;
CompressedTextures l_CompressedTextures;

std::map<matRenderTargetTexture*, FileInfo> l_RenderTargetTextures;
std::map<matTexture*, FileInfo> l_ShadowMapTextures;
std::map<matTexture*, FileInfo> l_RSMTextures;
std::map<matRenderTargetTexture*, FileInfo> l_FramebufferTextures;
std::set<matTexture*> l_VolumeTextures;			// DX11 doesn't track them (they leak); see DestroyTexture
std::set<matTexture*> l_PendingTextures;		// not ported yet (cube render targets)

g2dResourceCounterVk::eResourceCategory GetG2DResourceCategory(matTextureMgr::eResourceCategory i_ResourceCategory)
{
	switch(i_ResourceCategory)
	{
	case matTextureMgr::e_Framebuffer:
		return g2dResourceCounterVk::eRenderTarget;
	case matTextureMgr::e_ShadowMap:
		return g2dResourceCounterVk::eShadow;
	case matTextureMgr::e_SceneTexture:
		return g2dResourceCounterVk::eTexture;
	default:
		return g2dResourceCounterVk::eTexture;
	};
}
float l_PlainTextureMemory = 0;
float l_MipTextureMemory = 0;
float l_CompressedTextureMemory = 0;

float l_RenderTargetTextureMemory = 0;
float l_ShadowMapTextureMemory = 0;
float l_RSMTextureMemory = 0;
float l_FramebufferTextureMemory = 0;

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
//	decide what kind of file we have, from its first bytes
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
// reloads.
//--------------------------------------------------------------------
void load_plain_texture(matPlainTexture* io_Texture, const FileInfo& i_FileInfo)
{
	switch( i_FileInfo.m_Type )
	{
		case e_BMP:
		case e_PNG:
		case e_DDS:
		case e_TGA:
		case e_JPG:
		case e_HDR:
			matTextureLoader::LoadPlainTexture(io_Texture, i_FileInfo.m_Locator);
		break;
		case e_TIF:
			matTextureLoader::LoadTIFF(io_Texture, i_FileInfo.m_Locator, true, i_FileInfo.m_WidthReduce, i_FileInfo.m_HeightReduce);
		break;
		case e_RESOURCE:
			matTextureLoader::LoadResource(io_Texture, i_FileInfo.m_Locator, i_FileInfo.m_WidthReduce, i_FileInfo.m_HeightReduce);
		break;
	}
}

//--------------------------------------------------------------------
// note:  this will call ::Load for each type, those Loads handle
// reloads.
//--------------------------------------------------------------------
void load_mip_texture(matMipTexture* io_Texture, const FileInfo& i_FileInfo, bool i_bIsMipMap)
{
	switch( i_FileInfo.m_Type )
	{
		case e_BMP:
			matTextureLoader::LoadBMP(io_Texture, i_FileInfo.m_Locator, true, i_FileInfo.m_WidthReduce, i_FileInfo.m_HeightReduce, i_bIsMipMap);
		break;

		case e_PNG:
			matTextureLoader::LoadPNG(io_Texture, i_FileInfo.m_Locator, true, i_FileInfo.m_WidthReduce, i_FileInfo.m_HeightReduce, i_bIsMipMap);
		break;

		case e_JPG:
			matTextureLoader::LoadTGA(io_Texture, i_FileInfo.m_Locator, true, i_FileInfo.m_WidthReduce, i_FileInfo.m_HeightReduce, i_bIsMipMap);
		break;
		case e_TGA:
			matTextureLoaderTGA::LoadTGA(io_Texture, i_FileInfo.m_Locator, true, i_FileInfo.m_WidthReduce, i_FileInfo.m_HeightReduce, i_bIsMipMap);
		break;
		case e_HDR:
			matTextureLoaderHDR::LoadHDR(io_Texture, i_FileInfo.m_Locator, true, i_FileInfo.m_WidthReduce, i_FileInfo.m_HeightReduce, i_bIsMipMap);
		break;
		case e_TIF:
			matTextureLoader::LoadTIFF(io_Texture, i_FileInfo.m_Locator, true, i_FileInfo.m_WidthReduce, i_FileInfo.m_HeightReduce, i_bIsMipMap);
		break;
		default:
		break;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void load_render_target_texture(matRenderTargetTexture* io_Texture, const FileInfo& i_FileInfo)
{
	io_Texture->Make( i_FileInfo.m_Width, i_FileInfo.m_Height, i_FileInfo.m_Format );
	io_Texture->ReloadInfo();
}

//--------------------------------------------------------------------
//	The texture class a DDS file's shape needs, holding its image; NULL
//	(with a warning) if i_Type asks for another shape
//--------------------------------------------------------------------
matTextureVk* make_compressed_texture(const matTextureLoaderDDS::DDSInfo& i_Info, TEXTURE_TYPE i_Type,
									  std::unique_ptr<vkdImage>& io_Image, uint32_t i_Format)
{
	switch ( i_Info.s_eType )
	{
	case matTextureLoaderDDS::e_CubeTexture:
		if( i_Type == TEXTURE_TYPE_UNKNOWN || i_Type == TEXTURE_TYPE_CUBE )
		{
			matStaticCubeTexture* texture = new matStaticCubeTexture;
			texture->SetSurface( std::move(io_Image), i_Format );
			texture->ReloadInfo();
			return texture;
		}
		break;
	case matTextureLoaderDDS::e_MipTexture:
		if( i_Type == TEXTURE_TYPE_UNKNOWN || i_Type == TEXTURE_TYPE_1D || i_Type == TEXTURE_TYPE_2D )
		{
			matMipTexture* texture = new matMipTexture;
			texture->SetSurface( std::move(io_Image), i_Format );
			texture->ReloadInfo();
			return texture;
		}
		break;
	case matTextureLoaderDDS::e_VolumeTexture:
		if( i_Type == TEXTURE_TYPE_UNKNOWN || i_Type == TEXTURE_TYPE_3D )
		{
			matVolumeTexture* texture = new matVolumeTexture;
			texture->SetSurface( std::move(io_Image), i_Format );
			texture->ReloadInfo();
			return texture;
		}
		break;
	default:
		return NULL;
	}
	Warn_TextureType( i_Type );
	return NULL;
}

//--------------------------------------------------------------------
//	CopyResource of mips 0..i_Mips-1, every layer; the images must match
//	in size, samples and texel size (can_copy)
//--------------------------------------------------------------------
void copy_image(vkdImage* i_pSource, vkdImage* i_pDest, uint32_t i_Mips)
{
	const vkdImageDesc& src = i_pSource->Desc();
	std::lock_guard<std::recursive_mutex> lock(vkdGlobal::Device().StreamLock());
	VkCommandBuffer commands = vkdGlobal::Context().Commands();
	for (uint32_t mip = 0; mip < i_Mips; mip++)
	{
		VkImageCopy region = {};
		region.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, mip, 0, src.m_Layers };
		region.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, mip, 0, src.m_Layers };
		region.extent = { std::max(1u, src.m_Width >> mip), std::max(1u, src.m_Height >> mip), std::max(1u, src.m_Depth >> mip) };
		vkCmdCopyImage(commands, i_pSource->Image(), VK_IMAGE_LAYOUT_GENERAL, i_pDest->Image(), VK_IMAGE_LAYOUT_GENERAL, 1, &region);
	}
}

//	Same size, samples and bytes per texel (D3D11's CopyResource rule,
//	without its mip count: o_Mips is the number both have)
bool can_copy(vkdImage* i_pSource, vkdImage* i_pDest, uint32_t& o_Mips)
{
	const vkdImageDesc& src = i_pSource->Desc();
	const vkdImageDesc& dst = i_pDest->Desc();
	o_Mips = std::min(src.m_Mips, dst.m_Mips);
	return src.m_Width == dst.m_Width && src.m_Height == dst.m_Height && src.m_Depth == dst.m_Depth &&
		   src.m_Layers == dst.m_Layers && src.m_Samples == dst.m_Samples &&
		   vkdFormatInfoOf(src.m_Format).m_BlockBytes == vkdFormatInfoOf(dst.m_Format).m_BlockBytes;
}

}	// end of namespace

namespace
{
g2dPFD l_pfdGR32f = g2dPFD(g2dPFD::e_GR32f, 32*2);
matTexture* l_RndTexture = NULL;

//----------------------------------------------------------------------------
//	GraphicsDX11's Poisson disk: 256 samples (x, y) in an R32G32_FLOAT
//	texture, 256x1
//----------------------------------------------------------------------------
void CreatePoissonTexture()
{
#define MAX_POISSON_SAMPLES 256
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
	l_RndTexture = matTextureMgr::CreateTexture(MAX_POISSON_SAMPLES, 1, &l_pfdGR32f, false, poissonDisk);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void DestroyPoissonTexture()
{
	matTextureMgr::ReleaseTexture(l_RndTexture);
	l_RndTexture = NULL;
}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
matTexture* matTextureMgrVk::GetPoisson()
{
	return l_RndTexture;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
matTextureMgrVk::matTextureMgrVk()
{
}

//------------------------------------------------------------------------
//	The textures still alive go too: their images must be freed before
//	the device is (DX11 leaves them to the process exit)
//------------------------------------------------------------------------
matTextureMgrVk::~matTextureMgrVk()
{
	DestroyAllTextures();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void matTextureMgrVk::Initialize()
{
	CreatePoissonTexture();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void matTextureMgrVk::DeInitialize()
{
	DestroyPoissonTexture();
}

//------------------------------------------------------------------------
//	GetTotalTextureMemory returns the device-local memory (DX11: the
//	adapter's dedicated video memory), truncated to 32 bits as there
//------------------------------------------------------------------------
unsigned int matTextureMgrVk::GetTotalTextureMemory()
{
	DBG_ASSERT(vkdGlobal::IsSet(), "Must have initialized the device before calling this function");
	return (unsigned int) vkdGlobal::Device().Adapter().m_DeviceLocalBytes;
}

//--------------------------------------------------------------------
// Get current texture usage totals
//--------------------------------------------------------------------
unsigned int matTextureMgrVk::GetNumRenderTargets()
{
	return (unsigned int)l_RenderTargetTextures.size();
}
float matTextureMgrVk::GetTotalTextureMemoryUsed()
{
	return (l_PlainTextureMemory) +
		(l_MipTextureMemory) +
		(l_CompressedTextureMemory) +
		(l_RenderTargetTextureMemory) +
		(l_ShadowMapTextureMemory) +
		(l_RSMTextureMemory) +
		(l_FramebufferTextureMemory);
}
unsigned int matTextureMgrVk::GetNumTextures()
{
	return (unsigned int)(l_PlainTextures.size() +
		l_MipTextures.size() +
		l_CompressedTextures.size() +
		l_RenderTargetTextures.size() +
		l_ShadowMapTextures.size() +
		l_RSMTextures.size() +
		l_FramebufferTextures.size());
}
unsigned int matTextureMgrVk::GetNumShadowMaps()
{
	return (unsigned int)l_ShadowMapTextures.size();
}
float matTextureMgrVk::GetTotalRSMapMemoryUsed()
{
	return l_RSMTextureMemory;
}
unsigned int matTextureMgrVk::GetNumRSMaps()
{
	return (unsigned int)l_RSMTextures.size();
}
float matTextureMgrVk::GetTotalFramebufferMemoryUsed()
{
	return l_FramebufferTextureMemory;
}
unsigned int matTextureMgrVk::GetNumFramebufferTargets()
{
	return (unsigned int)l_FramebufferTextures.size();
}

//--------------------------------------------------------------------
// LoadTexture will attempt to load a texture based on the given
// locator, appending extensions until a file is found
// i_bIgnoreTUVSuffix makes it skip the .tuv test, used to allow for
// loading of textures w/ the same name as a tuv while maintaining
// a single codebase for this function
//--------------------------------------------------------------------
matTexture* matTextureMgrVk::LoadTexture(const fsLocator& i_Directory,
						const std::string& i_Name,
						std::vector<std::string>& o_UVANames,
						TEXTURE_TYPE i_Type/* = TEXTURE_TYPE_2D*/,
						bool i_bIgnoreTUVSuffix /*= false*/,
						bool mipmap_if_2D /*= true*/)
{
	DBG_ASSERT( i_Name.size() > 0, "No name for texture" );

	int width_reduce, height_reduce;

	fsLocator tex_locator(i_Directory);
	tex_locator.Push(i_Name);

	std::string extension;
	envString::GetExtension(i_Name, extension);
	envString::ToLower(extension);

	if (gfFileTranslationMgr::FileExists(tex_locator))
	{
		if (extension == "dds")
		{
			return this->LoadCompressedTexture( tex_locator, i_Type, mipmap_if_2D );
		}
		else if (extension == "tuv")
		{
			if( i_Type == TEXTURE_TYPE_UNKNOWN || i_Type == TEXTURE_TYPE_2D )
			{
				if (!i_bIgnoreTUVSuffix) // do we need this check anymore?
				{
					gfFileBin texture_file(tex_locator, fsFileStream::e_ReadOnly);
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
//	Mip map levels will be created for the texture.
//--------------------------------------------------------------------
matMipTexture* matTextureMgrVk::LoadMipTexture(	const fsLocator& i_Locator,
								int i_WidthReduce,
								int i_HeightReduce)
{
	//	Find out what the file type is (and if it exists)
	//
	FileType file_type = pick_format(i_Locator);
	FileInfo file_info(file_type, i_Locator, i_WidthReduce, i_HeightReduce);

	//	Setup the matMipTexture
	//
	std::unique_ptr<matMipTexture> ret_val(new matMipTexture);

	load_mip_texture(ret_val.get(), file_info, true);

	if (g2dVkGlobal::RestrictPow2Textures())
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
void matTextureMgrVk::ReloadMipTexture(	const fsLocator& i_Locator, matMipTexture& io_Texture)
{
	FileInfo file_info = l_MipTextures[&io_Texture];
	file_info.m_Locator = i_Locator;

	load_mip_texture(&io_Texture, file_info, true); // supports reload of all types
}

//--------------------------------------------------------------------
//	LoadCompressedTexture creates a texture from the given fsLocator.
//	This can be used to load textures that are in .dds format.
// Note:  The matTexture returned pointer can be a matMipTexture
//		matStaticCubeTexture, or a matVolumeTexture.
//--------------------------------------------------------------------
matTexture* matTextureMgrVk::LoadCompressedTexture( const fsLocator& i_Locator, TEXTURE_TYPE i_Type /*= TEXTURE_TYPE_UNKNOWN*/, bool mipmap_if_2D /*= true */ )
{
	// cleaned up when texture is cleaned up (via local map)
	std::unique_ptr<matTextureLoaderDDS::DDSInfo> pInfo(new matTextureLoaderDDS::DDSInfo);

	int width_reduce = 0, height_reduce = 0;
	if (matTextureMgr::GetTextureAdjuster())
		matTextureMgr::GetTextureAdjuster()->GetReduce(i_Locator, width_reduce, height_reduce);

	uint32_t format = 0;
	std::unique_ptr<vkdImage> image = matTextureLoaderDDS::LoadDDS( *pInfo, i_Locator, width_reduce, height_reduce, format, mipmap_if_2D );
	matTextureVk* pRet = image ? make_compressed_texture(*pInfo, i_Type, image, format) : NULL;

	if( !pRet )
	{
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

	l_CompressedTextures[pRet] = pInfo.release();

	return pRet;
}

//--------------------------------------------------------------------
// ReloadCompressedTexture reloads the given texture file into the
// given texture object.  The new file's shape must match the texture's
// class (it is left as it was if it doesn't).
//--------------------------------------------------------------------
void matTextureMgrVk::ReloadCompressedTexture( const fsLocator& i_Locator, matTextureVk* io_Texture, bool i_bIsMipMap)
{
	matTextureLoaderDDS::DDSInfo* pInfo = l_CompressedTextures[io_Texture];
	pInfo->s_Locator = i_Locator;

	uint32_t format = 0;
	std::unique_ptr<vkdImage> image = matTextureLoaderDDS::LoadDDS( *pInfo, i_Locator, 0, 0, format, i_bIsMipMap );

	bool bError = (image == NULL);
	if( !bError )
	{
		switch ( pInfo->s_eType )
		{
		case matTextureLoaderDDS::e_CubeTexture:
			if (matStaticCubeTexture* texture = dynamic_cast<matStaticCubeTexture*>(io_Texture))
			{
				texture->SetSurface( std::move(image), format );
				texture->ReloadInfo();
			}
			break;
		case matTextureLoaderDDS::e_MipTexture:
			if (matMipTexture* texture = dynamic_cast<matMipTexture*>(io_Texture))
			{
				texture->SetSurface( std::move(image), format );
				texture->ReloadInfo();
			}
			break;
		case matTextureLoaderDDS::e_VolumeTexture:
			if (matVolumeTexture* texture = dynamic_cast<matVolumeTexture*>(io_Texture))
			{
				texture->SetSurface( std::move(image), format );
				texture->ReloadInfo();
			}
			break;
		default:
			bError = true;
			break;
		}
	}

	if( bError )
	{
		std::string fullfilename;
		fsFileUtil::LocatorToANSIFilename(i_Locator,fullfilename);
		if (matTextureMgr::IsAllowNullTextures())
		{
			DBG_WARNING("The texture file could not be loaded at the given path: " << fullfilename);
			matTextureTracking::AddMissingTexture(i_Locator);
		}
		else
		{
			DBG_ERROR("The texture file could not be loaded at the given path: " << fullfilename);
			throw fsUnknownX(i_Locator);
		}
	}

	// reuse the type for our needs
	pInfo->s_eType = e_DDS;
}

//--------------------------------------------------------------------
//	CreateShadowMap creates a shadow map texture that can 
//	be rendered to. Use the GetRenderTargetAPI() function
//	on the returned texture in order to render to it.
//--------------------------------------------------------------------
matTexture* matTextureMgrVk::CreateShadowMap(int i_nTextureWidth, int i_nTextureHeight)
{
	if (g2dVkGlobal::RestrictPow2Textures())
	{
		if (!((i_nTextureWidth & (i_nTextureWidth - 1)) == 0) ||
			!((i_nTextureHeight & (i_nTextureHeight - 1)) == 0))
		{
			throw matInvalidTextureSizeX(fsLocator());
		}
	}

	matShadowMap* pTexture = new matShadowMap();

	i_nTextureWidth = std::max(i_nTextureWidth,1);
	i_nTextureHeight = std::max(i_nTextureHeight,1);

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
		delete pTexture;
		return NULL;
	}

	// update all the core data for the texture
	pTexture->ReloadInfo();

	// add memory count to shadow map category 
	l_ShadowMapTextures[pTexture] = FileInfo(i_nTextureWidth, i_nTextureHeight, pTexture->GetPixelFormat());

	return pTexture;
}

//--------------------------------------------------------------------
//	CreateReflectiveShadowMap creates the position, normal and flux
//	targets LPV GI renders each light into
//--------------------------------------------------------------------
matTexture* matTextureMgrVk::CreateReflectiveShadowMap(int i_nTextureWidth, int i_nTextureHeight)
{
	if (g2dVkGlobal::RestrictPow2Textures())
	{
		if (!((i_nTextureWidth & (i_nTextureWidth - 1)) == 0) ||
			!((i_nTextureHeight & (i_nTextureHeight - 1)) == 0))
		{
			throw matInvalidTextureSizeX(fsLocator());
		}
	}

	matReflectiveShadowMap* pTexture = new matReflectiveShadowMap();

	i_nTextureWidth = std::max(i_nTextureWidth,1);
	i_nTextureHeight = std::max(i_nTextureHeight,1);

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
		delete pTexture;
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
matTexture* matTextureMgrVk::CreateRenderTargetTexture(int i_nTextureWidth,
														int i_nTextureHeight,
														bool i_bFloatingPoint,
														const g2dPFD* i_PFD,
														bool i_bAutoGenMipmap,
														bool i_bAllocDepthBuffer,
														matTextureMgr::eResourceCategory i_ResourceCategory,
														bool i_bFloatDepth,
														bool i_bAntiAlias )
{
	if (g2dVkGlobal::RestrictPow2Textures())
	{
		if (!((i_nTextureWidth & (i_nTextureWidth - 1)) == 0) ||
			!((i_nTextureHeight & (i_nTextureHeight - 1)) == 0))
		{
			throw matInvalidTextureSizeX(fsLocator());
		}
	}

	const g2dPFD& image_format = (i_PFD != NULL) ? (*i_PFD) : matVkGlobal::TrueAlphaTextureFormat();
	matRenderTargetTexture* pTexture = new matRenderTargetTexture(i_bFloatingPoint, i_bFloatDepth);

	i_nTextureWidth = std::max(i_nTextureWidth,1);
	i_nTextureHeight = std::max(i_nTextureHeight,1);

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
		delete pTexture;
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
matTexture* matTextureMgrVk::CreateTexture(int i_nTextureWidth,
	int i_nTextureHeight,
	const g2dPFD* i_PFD,
	bool i_bMipmap)
{
	return CreateTexture(i_nTextureWidth, i_nTextureHeight, i_PFD, i_bMipmap, NULL);
}

//--------------------------------------------------------------------
//	CreateTexture creates an initialized texture
//--------------------------------------------------------------------
matTexture* matTextureMgrVk::CreateTexture(int i_nTextureWidth,
											int i_nTextureHeight,
											const g2dPFD* i_PFD,
											bool i_bMipmap,
											void* i_PixelData)
{
	if (g2dVkGlobal::RestrictPow2Textures())
	{
		if (!((i_nTextureWidth & (i_nTextureWidth - 1)) == 0) ||
			!((i_nTextureHeight & (i_nTextureHeight - 1)) == 0))
		{
			throw matInvalidTextureSizeX(fsLocator());
		}
	}

	const g2dPFD& image_format = (i_PFD != NULL) ? (*i_PFD) : matVkGlobal::NonAlphaTextureFormat();

	matTexture* retval = NULL;

	// use unique_ptr to clean up in case of exception thrown from Make()
	if (i_bMipmap)
	{
		std::unique_ptr<matMipTexture> pTexture(new matMipTexture);
		pTexture->Make( i_nTextureWidth, i_nTextureHeight, image_format, 0, i_PixelData );
		pTexture->ReloadInfo();
		matMipTexture* pRaw = pTexture.release();
		l_MipTextures[pRaw] = FileInfo(i_nTextureWidth, i_nTextureHeight, image_format);
		retval = pRaw;
	}
	else
	{
		std::unique_ptr<matPlainTexture> pTexture(new matPlainTexture);
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
matTexture* matTextureMgrVk::CreateTexture3D(int i_nTextureWidth,
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
matTexture* matTextureMgrVk::CreateTexture3D(int i_nTextureWidth,
											 int i_nTextureHeight,
											 int i_nTextureDepth,
											 const g2dPFD* i_PFD,
											 BIND_TYPE i_Bindings,
											 void* i_PixelData)
{
	if (g2dVkGlobal::RestrictPow2Textures())
	{
		if (!((i_nTextureWidth & (i_nTextureWidth - 1)) == 0) ||
			!((i_nTextureHeight & (i_nTextureHeight - 1)) == 0))
		{
			throw matInvalidTextureSizeX(fsLocator());
		}
	}

	const g2dPFD& image_format = (i_PFD != NULL) ? (*i_PFD) : matVkGlobal::NonAlphaTextureFormat();

	std::unique_ptr<matVolumeTexture> pTexture(new matVolumeTexture);
	pTexture->Make( i_nTextureWidth, i_nTextureHeight, i_nTextureDepth, image_format, i_Bindings, i_PixelData );
	pTexture->ReloadInfo();
	matVolumeTexture* pRaw = pTexture.release();
	l_VolumeTextures.insert(pRaw);

	return pRaw;
}

//--------------------------------------------------------------------
//	CopyTexture copy one texture to another
//--------------------------------------------------------------------
void matTextureMgrVk::CopyTexture(matTexture* i_SrcTex, matTexture* i_DstTex)
{
	// only support matRenderTargetCopying
	matRenderTargetTexture* src = dynamic_cast<matRenderTargetTexture*>(i_SrcTex);
	matRenderTargetTexture* dst = dynamic_cast<matRenderTargetTexture*>(i_DstTex);

	if (!src || !dst)
	{
		DBG_WARNING("matTextureMgr::CopyTexture only supports matRenderTarget texture copying");
		return;
	}
	if (!src->GetImage() || !dst->GetImage())
		return;

	uint32_t mips = 0;
	if (!can_copy(src->GetImage(), dst->GetImage(), mips) || src->GetImage()->Desc().m_Mips != dst->GetImage()->Desc().m_Mips)
	{
		DBG_WARNING("matTextureMgr::CopyTexture: the textures don't match");
		return;
	}
	copy_image(src->GetImage(), dst->GetImage(), mips);
}

//--------------------------------------------------------------------
//	CreateRenderTargetTexture creates a texture that can
//	be rendered to. The texture is initialized using data from the
//	input texture.
//--------------------------------------------------------------------
matTexture* matTextureMgrVk::CreateRenderTargetTexture(matTexture* i_Texture,
														bool i_bAutoGenMipmap,
														matTextureMgr::eResourceCategory i_ResourceCategory)
{
	matTextureVk* source = dynamic_cast<matTextureVk*>(i_Texture);
	DBG_ASSERT(source, "Bad source texture passed in to CreateRenderTargetTexture");

	// note: "floating point" (depth map) source texture not supported!
	matTexture* pTexture = CreateRenderTargetTexture(i_Texture->GetWidth(),
		i_Texture->GetHeight(), false, &i_Texture->GetPixelFormat(), i_bAutoGenMipmap, true, i_ResourceCategory, false);
	matTextureVk* dest = dynamic_cast<matTextureVk*>(pTexture);
	DBG_ASSERT(dest, "Bad destination texture created in CreateRenderTargetTexture");

	// assumes compatibility for this copy operation! (D3D11's CopyResource
	// copies nothing if the mip counts differ; this copies the mips both have)
	uint32_t mips = 0;
	if (source && dest && source->GetImage() && dest->GetImage() && can_copy(source->GetImage(), dest->GetImage(), mips))
	{
		copy_image(source->GetImage(), dest->GetImage(), mips);
	}
	else
	{
		DBG_WARNING("CreateRenderTargetTexture: can't copy the source texture");
	}

	return pTexture;
}

//--------------------------------------------------------------------
//	CreateCubeRenderTargetTexture creates a cubemap texture that can 
//	be rendered to. 
//--------------------------------------------------------------------
matTexture* matTextureMgrVk::CreateCubeRenderTargetTexture(int i_nTextureWidth,
													int i_nTextureHeight,
													g2dPFD* i_PFD)
{
	if (g2dVkGlobal::RestrictPow2Textures())
	{
		if (!((i_nTextureWidth & (i_nTextureWidth - 1)) == 0) ||
			!((i_nTextureHeight & (i_nTextureHeight - 1)) == 0))
		{
			throw matInvalidTextureSizeX(fsLocator());
		}
	}

	const g2dPFD& image_format = (i_PFD != NULL) ? (*i_PFD) : matVkGlobal::TrueAlphaTextureFormat();
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
void matTextureMgrVk::DestroyTexture(matTexture* i_Texture)
{
	// First look it up in the compressed texture map, since a compressed texture
	// can be any one of the ones below.  If it isn't compressed, then we can
	// destroy the normal way.
	matTextureVk* tex_vk = dynamic_cast<matTextureVk*>(i_Texture);
	CompressedTextures::iterator it = l_CompressedTextures.find( tex_vk );
	if ( tex_vk && l_CompressedTextures.end() != it )
	{
		// compressed, destroy it
		delete i_Texture;
		delete it->second; // compressed textures need to delete their file info objects
		l_CompressedTextures.erase(it);
		return;
	}

	if (matPlainTexture* plain_tex = dynamic_cast<matPlainTexture*>(i_Texture))
	{
		std::map<matPlainTexture*, FileInfo>::iterator it = l_PlainTextures.find(plain_tex);
		DBG_ASSERT(it != l_PlainTextures.end(), "Texture not found in list (did you create it with the matTextureMgr?)");
		if (it != l_PlainTextures.end())
			l_PlainTextures.erase(it);
		delete plain_tex;
	}
	else if (matMipTexture* mip_tex = dynamic_cast<matMipTexture*>(i_Texture))
	{
		std::map<matMipTexture*, FileInfo>::iterator it = l_MipTextures.find(mip_tex);
		DBG_ASSERT(it != l_MipTextures.end(), "Texture not found in list (did you create it with the matTextureMgr?)");
		if (it != l_MipTextures.end())
			l_MipTextures.erase(it);
		delete mip_tex;
	}
	else if (l_ShadowMapTextures.count(i_Texture))
	{
		l_ShadowMapTextures.erase(i_Texture);
		delete i_Texture;
	}
	else if (l_RSMTextures.count(i_Texture))
	{
		l_RSMTextures.erase(i_Texture);
		l_RSMTextureMemory -= i_Texture->GetSize();
		delete i_Texture;
	}
	else if (l_VolumeTextures.count(i_Texture))
	{
		//	DX11 never deletes these (it doesn't track them)
		l_VolumeTextures.erase(i_Texture);
		delete i_Texture;
	}
	else if (l_PendingTextures.count(i_Texture))
	{
		l_PendingTextures.erase(i_Texture);
		delete i_Texture;
	}
	else
	{
		// rendertargettex can be in any of 3 categories: framebuffer, rendertarget, or shadowmap.
		matRenderTargetTexture* render_target_tex = dynamic_cast<matRenderTargetTexture*>(i_Texture);
		if( render_target_tex )
		{
			if (l_RenderTargetTextures.erase(render_target_tex) || l_FramebufferTextures.erase(render_target_tex))
			{
				delete render_target_tex;
			}
			else
			{
				DBG_ASSERT(false, "Texture not found in list (did you create it with the matTextureMgr?)");
			}
		}
	}
}

//--------------------------------------------------------------------
//	DestroyAllTextures causes all textures to be destroyed.
//--------------------------------------------------------------------
void matTextureMgrVk::DestroyAllTextures()
{
	for (auto& entry : l_PlainTextures)
		delete entry.first;
	l_PlainTextures.clear();

	for (auto& entry : l_MipTextures)
		delete entry.first;
	l_MipTextures.clear();

	for (auto& entry : l_RenderTargetTextures)
		delete entry.first;
	l_RenderTargetTextures.clear();

	for (auto& entry : l_ShadowMapTextures)
		delete entry.first;
	l_ShadowMapTextures.clear();

	for (auto& entry : l_RSMTextures)
		delete entry.first;
	l_RSMTextures.clear();
	l_RSMTextureMemory = 0;

	for (auto& entry : l_FramebufferTextures)
		delete entry.first;
	l_FramebufferTextures.clear();

	//	DX11 leaves these (it destroys them on device loss); here the
	//	device outlives the manager only for its clean-up
	for (auto& entry : l_CompressedTextures)
	{
		delete entry.first;
		delete entry.second;
	}
	l_CompressedTextures.clear();

	for (matTexture* texture : l_VolumeTextures)
		delete texture;
	l_VolumeTextures.clear();

	for (matTexture* texture : l_PendingTextures)
		delete texture;
	l_PendingTextures.clear();
}

//--------------------------------------------------------------------
//	ReloadTextures reloads the appropriate texture
//--------------------------------------------------------------------
void matTextureMgrVk::ReloadTexture( const fsLocator& i_Locator, bool i_bIsMipMap )
{
	for (auto& entry : l_PlainTextures)
	{
		if( entry.second.m_Locator == i_Locator )
		{
			load_plain_texture(entry.first, entry.second);
			return;
		}
	}

	for (auto& entry : l_MipTextures)
	{
		if( entry.second.m_Locator == i_Locator )
		{
			load_mip_texture(entry.first, entry.second, i_bIsMipMap);
			return;
		}
	}

	for (auto& entry : l_CompressedTextures)
	{
		if( entry.second->s_Locator == i_Locator )
		{
			ReloadCompressedTexture(entry.second->s_Locator, entry.first, i_bIsMipMap);
			return;
		}
	}

	for (auto& entry : l_RenderTargetTextures)
	{
		if( entry.second.m_Locator == i_Locator )
		{
			load_render_target_texture(entry.first, entry.second);
			return;
		}
	}
	for (auto& entry : l_ShadowMapTextures)
	{
		if( entry.second.m_Locator == i_Locator )
		{
			DBG_ASSERT(false, "Not able to reload shadowmaps yet");
			return;
		}
	}
	for (auto& entry : l_RSMTextures)
	{
		if( entry.second.m_Locator == i_Locator )
		{
			DBG_ASSERT(false, "Not able to reload rs maps yet");
			return;
		}
	}
	for (auto& entry : l_FramebufferTextures)
	{
		if( entry.second.m_Locator == i_Locator )
		{
			load_render_target_texture(entry.first, entry.second);
			return;
		}
	}
}

//--------------------------------------------------------------------
//	UnloadTextures: DX11 finds the texture and does nothing
//--------------------------------------------------------------------
void matTextureMgrVk::UnloadTexture( const fsLocator& i_Locator )
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void matTextureMgrVk::FillTexture(matTexture* i_pTexture, const maFloatRGBA& i_Color)
{
	// this might have to get converted to (BYTE)(clamp(color,0,1) * 255)
	// depending on pixelformat

	// expand
	int size = i_pTexture->GetWidth() * i_pTexture->GetHeight() * 4;
	std::vector<float> pixels(size);
	for (int i = 0; i < i_pTexture->GetWidth() * i_pTexture->GetHeight(); i++)
	{
		pixels[i*4 + 0] = i_Color.GetRed();
		pixels[i*4 + 1] = i_Color.GetGreen();
		pixels[i*4 + 2] = i_Color.GetBlue();
		pixels[i*4 + 3] = i_Color.GetAlpha();
	}

	this->UpdateTexture(i_pTexture, (unsigned char*)pixels.data(), size * (int)sizeof(float) );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void matTextureMgrVk::FillTexture(matTexture* i_pTexture, void* i_PixelData, int i_nByte)
{
	// pass in number of bytes
	this->UpdateTexture(i_pTexture, (unsigned char*)i_PixelData, i_nByte );
}

//------------------------------------------------------------------------
//	DX11 copies a staging texture of mip 0's size over the whole resource
//	(CopyResource: right for single-mip, single-sample textures)
//------------------------------------------------------------------------
void matTextureMgrVk::UpdateTexture( matTexture* i_pTexture, unsigned char* i_Data, int i_Size, int nMipLevel )
{
	matTextureVk* texture = dynamic_cast<matTextureVk*>(i_pTexture);
	vkdImage* image = texture ? texture->GetImage() : NULL;
	if (!image)
	{
		DBG_WARNING("UpdateTexture: not a texture with an image");
		return;
	}
	if (image->Samples() != VK_SAMPLE_COUNT_1_BIT || image->Desc().m_Mips != 1)
	{
		DBG_WARNING("UpdateTexture: only single-mip, single-sample textures can be updated (as DX11's CopyResource)");
		return;
	}
	const uint32_t format = g2dVkGlobal::DxgiFormatFromPFD( i_pTexture->GetPixelFormat() );
	matTextureVk::Upload(image, format, i_Data, i_pTexture->GetWidth(), i_pTexture->GetHeight());
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void matTextureMgrVk::MergeTransparentTextures(matTexture* i_pTexO, matTexture* i_pTexI, g2dRenderTarget* io_pTarget)
{
	io_pTarget->MakeCurrent();

	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	g3dVkUtil::CopyTexToTarget( i_pTexO, io_pTarget );						//copy existing texture into target

	g3dBlendStateMgr::SetBlendState(st_Blend);

	g3dVkUtil::CopyTexToTarget( i_pTexI, io_pTarget );						//blend with new texture

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
}

//--------------------------------------------------------------------
//	LoadPlainTexture creates a texture from the given fsLocator.
//--------------------------------------------------------------------
matPlainTexture* matTextureMgrVk::LoadPlainTexture(	const fsLocator& i_Locator,
									int i_WidthReduce,
									int i_HeightReduce)
{
	//	Find out what the file type is (and if it exists)
	//
	FileType file_type = pick_format(i_Locator);
	FileInfo file_info(file_type, i_Locator, i_WidthReduce, i_HeightReduce);

	//	Setup the matPlainTexture
	//
	std::unique_ptr<matPlainTexture> ret_val(new matPlainTexture);

	load_plain_texture(ret_val.get(), file_info);

	if (g2dVkGlobal::RestrictPow2Textures())
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
//	the given i_Texture.
//--------------------------------------------------------------------
void matTextureMgrVk::ReloadPlainTexture(matPlainTexture* io_Texture,
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
matPlainTexture* matTextureMgrVk::LoadResourceTexture(const fsLocator& i_Locator,
														int i_WidthReduce,
														int i_HeightReduce)
{
	//	Find out what the file type is (and if it exists)
	//
	FileType file_type = e_RESOURCE;
	FileInfo file_info(file_type, i_Locator, i_WidthReduce, i_HeightReduce);

	//	Setup the matPlainTexture
	//
	std::unique_ptr<matPlainTexture> ret_val(new matPlainTexture);

	load_plain_texture(ret_val.get(), file_info);

	matPlainTexture* texture = ret_val.release();
	l_PlainTextures[texture] = file_info;

	return texture;
}

//--------------------------------------------------------------------
//	DX11's blend states: copy, then multiply (dest * src)
//--------------------------------------------------------------------
void matTextureMgrVk::InitStates()
{
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_SRC_COLOR,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );
	st_Blend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_SRC_COLOR,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void matTextureMgrVk::CleanupStates()
{
	delete st_NoBlend;				st_NoBlend = NULL;
	delete st_Blend;				st_Blend = NULL;
	delete ds_Disable_NS;			ds_Disable_NS = NULL;
	delete ds_Test_Write_LessE_NS;	ds_Test_Write_LessE_NS = NULL;
}

}
