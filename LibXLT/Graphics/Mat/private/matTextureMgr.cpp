/****************************************************************************\
**  matTextureMgr.hpp
**
**      The matTextureMgr handles loading, creation, and deletion of
**	textures.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mat/matTextureMgr.hpp"

#include "Core/ch/chBinReader.hpp"
#include "Core/ch/chBinWriter.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/private/fsFileNotifyMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceFinder.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/mat/matUVATexture.hpp"
#include "Graphics/mat/matUVATextureParseUtil.hpp"
//#include "Tool/gpx/gpxRenderControl.hpp"

#include <iomanip>
#include <algorithm>
#include <set>


//============================================================================
//============================================================================
namespace matTextureMgr
{
namespace
{
	matTextureMgrImpl* l_pImpl = NULL;
	bool l_bAllowNullTextures = false;
	bool l_bSkipAllTextures = false;
	bool l_bCompressTextures = false;
	matTextureAdjuster *l_pTextureAdjuster = NULL;
	fsLocator l_TextureFileName;
	fsResourceTrackerInterest* rsrInterest;

	unsigned int l_TextureMemory = 0;

	// used when dumping textures
	int l_nTextureMemoryTally = 0;

	//----------------------------------------------------------------------------
	// Reusable texture info
	//----------------------------------------------------------------------------
	struct TextureInfo
	{
		TextureInfo()
		{
			m_Texture = NULL;
			m_Count = 0;
			m_Size=0;
		}
		TextureInfo(matTexture* i_Texture) : m_Texture(i_Texture)
		{
			m_Count = 1;
			m_Size = i_Texture->GetSize();
		}

		bool operator == (const TextureInfo& i_Info) const { return m_Texture == i_Info.m_Texture; }

		matTexture* m_Texture;
		int m_Count;
		float m_Size; // how much space on the card it takes up
	};

	typedef std::map<fsLocator, TextureInfo> Textures;
	Textures l_Textures;


	//----------------------------------------------------------------------------
	// NonReusable texture info  -- Thus the N R extension
	//----------------------------------------------------------------------------
	struct TextureInfoNR
	{
		TextureInfoNR()
		{
			m_Count = 0;
			m_Size=0;
		}
		TextureInfoNR(const fsLocator& i_Locator, const matTexture* i_Texture)
		{
			fsFileUtil::LocatorToANSIFilename(i_Locator, m_Filename);
			m_Count = 1;
			m_Size = i_Texture->GetSize();
		}
		TextureInfoNR(const char* i_Description, const matTexture* i_Texture)
		{
			m_Filename = i_Description;
			m_Count = 1;
			m_Size = i_Texture->GetSize();
		}


		//bool operator == (const TextureInfo& i_Info) const { return m_Texture == i_Info.m_Texture; }

		std::string m_Filename;
		int m_Count;
		int m_Size; // how much space on the card it takes up
	};

	// These do not have unique fsLocators, but we need to manage them
	typedef std::map<matTexture*, TextureInfoNR> NRTextures;
	NRTextures l_NRTextures;

	void print_texture_info(const std::pair<fsLocator, TextureInfo>& i_Info)
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Info.first, filename);
		l_nTextureMemoryTally += i_Info.second.m_Size;
		if ( i_Info.second.m_Size < 1024 )
			DBG_WARNING("Texture " << filename.c_str() << ": " << i_Info.second.m_Count << " Size(" << i_Info.second.m_Size << " KB)" );
		else
			DBG_WARNING("Texture " << filename.c_str() << ": " << i_Info.second.m_Count << " Size(" << ((float)i_Info.second.m_Size / 1024.0f) << " MB)" );
	}
	void print_texture_info_nr(const std::pair<matTexture*, TextureInfoNR>& i_Info)
	{
		l_nTextureMemoryTally += i_Info.second.m_Size;
		if ( i_Info.second.m_Size < 1024 )
			DBG_WARNING("Texture " << i_Info.second.m_Filename.c_str() << ": Size(" << i_Info.second.m_Size << " KB)" );
		else
			DBG_WARNING("Texture " << i_Info.second.m_Filename.c_str() << ": Size(" << ((float)i_Info.second.m_Size / 1024.0f) << " MB)" );
	}

	void dump_texture_list()
	{
		l_nTextureMemoryTally = 0;
		DBG_WARNING("Texture usage**********************************************");
		// reusable textures
		std::for_each(l_Textures.begin(), l_Textures.end(), print_texture_info);
		// non-reusable textures
		std::for_each(l_NRTextures.begin(), l_NRTextures.end(), print_texture_info_nr);
		float total = ((float)l_nTextureMemoryTally) / 1024.0f;  //KB
		total /= 1024.0f; //MB
		DBG_WARNING("Texture Memory Usage Total:  " << std::setprecision(2) << total << " MB");
	}

	
	//--------------------------------------------------------------------
	// TEXTURE MANAGEMENT
	//--------------------------------------------------------------------
	std::vector<matUVATexture*> l_UVATextures;
	std::set<matUVATexture*> l_AutoCleanupUVATextures;

	void destroy_uva(matUVATexture* i_UVATexture)
	{
		delete i_UVATexture;
	}

	void destroy_uva_and_subtextures(matUVATexture* i_UVATexture)
	{
		int i;
		int num = i_UVATexture->GetNumPages();
		for( i = 0 ; i < num ; i++ )
			ReleaseTexture(i_UVATexture->GetPage(i));

		destroy_uva(i_UVATexture);
	}

	void destroy_texture( matTexture* i_Texture )
	{
		matUVATexture* uva_texture = dynamic_cast<matUVATexture*>(i_Texture);
		if( uva_texture )
		{
			std::set<matUVATexture*>::iterator it = l_AutoCleanupUVATextures.find(uva_texture);

			if( it != l_AutoCleanupUVATextures.end() )
			{
				destroy_uva_and_subtextures(uva_texture);

				l_AutoCleanupUVATextures.erase(it);
			}
			else
			{
				destroy_uva(uva_texture);
			}
		}
		else
		{
			l_pImpl->DestroyTexture(i_Texture);
		}
	}

}	// end of namespace

void GetTextureName(const fsLocator& i_TextureFileName)
{
	l_TextureFileName = i_TextureFileName;
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void Init()
{
	if( NULL == l_pTextureAdjuster ) 
		l_pTextureAdjuster = new matTextureAdjuster;
	rsrInterest = fsResourceTracker::GetInterestList();
	rsrInterest->TextureReloadFunction(&ReloadTexture);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void CleanUp()
{
	delete l_pTextureAdjuster;
	l_pTextureAdjuster = NULL;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void SetImplementation(matTextureMgrImpl* i_pImpl)
{
	l_pImpl = i_pImpl;
}

//------------------------------------------------------------------------
// If AllowNullTextures is true, the Load() function will return
// NULL if the texture isn't found.  If it is false, it will
// through an exception.  The default is false.
//------------------------------------------------------------------------
void SetAllowNullTextures(bool i_bAllow)
{
	l_bAllowNullTextures = i_bAllow;
}
bool IsAllowNullTextures()
{
	return l_bAllowNullTextures;
}

//------------------------------------------------------------------------
// If SkipAllTextures is true, the Load() function will return
// NULL for all textures without even trying to load them.
//------------------------------------------------------------------------
void SetSkipAllTextures(bool i_bSkip)
{
	l_bSkipAllTextures = i_bSkip;
}
bool IsSkipAllTextures()
{
	return l_bSkipAllTextures;
}

//------------------------------------------------------------------------
// AutoCompress Textures
//------------------------------------------------------------------------
void SetCompressTextures(bool i_bCompress)
{
	l_bCompressTextures = i_bCompress;
}
bool IsCompressTextures()
{
	return l_bCompressTextures;
}
//------------------------------------------------------------------------
//	GetTotalTextureMemory returns alleged amount of texture memory
//	made available by the hardware.  Take this number with a grain of
//	salt.
//------------------------------------------------------------------------
unsigned int GetTotalTextureMemory()
{
	if( l_TextureMemory == 0 )
	{
		l_TextureMemory = l_pImpl->GetTotalTextureMemory();
	}

	return l_TextureMemory;
}

//--------------------------------------------------------------------
// LoadTexture will attempt to load a texture based on the given
// locator. The version which takes a directory and itString is the primary
// implementation of this function. The other versions are provided as a
// convenience and simply forward on to the primary version.
// It will attempt to lookup the texture in the loaded list
// if not found it will load from disk.
//--------------------------------------------------------------------
matTexture* LoadTexture( const fsLocator& i_Locator, const TEXTURE_TYPE i_Type, const bool mipmap_if_2D /*= true*/ )
{
	DBG_ASSERT(i_Locator.GetNumNames(), "Locator passed to matTextureMgr::LoadTexture(i_Locator) must have at least one name");
	if (i_Locator.GetNumNames() == 0)
		return NULL;
	fsLocator loc = i_Locator;
	loc.Pop();
	return LoadTexture(loc, i_Locator.GetLastName(), i_Type, mipmap_if_2D);
}
//--------------------------------------------------------------------
matTexture* LoadTexture(const fsLocator& i_Directory, const itString& i_Name, const TEXTURE_TYPE i_Type/* = TEXTURE_TYPE_2D*/, const bool mipmap_if_2D /*= true*/, const bool i_bIgnoreTUVSuffix /*= false*/)
{
	if (l_pImpl == NULL)
		return NULL;

	//we want no extension information to remain in the name of the texture
	itString tex_name(i_Name);
	//tex_name.StripExtension();
	itStringUtil::ToLower(tex_name);

	// Note:  fsLocators use itStrings which are case sensitive, unlike filenames
	// themselves.  Force all the names in the list (unexpanded) to lower case
	// for texture loading
	fsLocator tex_locator(i_Directory), tex_directory;
	int count = tex_locator.GetNumNames();
	int i;
	itString temp;
	for ( i=0; i<count; ++i )
	{
		temp = tex_locator.GetName(i);
		tex_locator.ReplaceName( itStringUtil::ToLower(temp), i );
	}
	tex_directory = tex_locator;
	tex_locator.Push(tex_name);

	//
	std::map<fsLocator, TextureInfo>::iterator it = l_Textures.end();
	it = l_Textures.find(tex_locator);
	if( it == l_Textures.end() )
	{
		// load from disk
		matTexture* new_texture = NULL;

		if (!l_bSkipAllTextures)
		{
			try
			{
				std::vector<itString> names;

				// ACTUALLY LOAD THE TEXTURE!!!!
				new_texture = l_pImpl->LoadTexture(tex_directory, tex_name, names, i_Type, i_bIgnoreTUVSuffix, mipmap_if_2D);

				matUVATexture *uva_texture = dynamic_cast<matUVATexture*>(new_texture);
				if (uva_texture)
				{
					//load all the named textures
					int i, num_names = names.size();
					itString uva_name;
					fsLocator uva_locator = tex_directory;
					std::vector<itString> dummy_names;
					for (i = 0; i < num_names; ++i)
					{
						uva_name = names[i];
						//uva_name.StripExtension();
						itStringUtil::ToLower(uva_name);
						uva_locator.Push(uva_name);
						matTexture *new_texture_page = l_pImpl->LoadTexture(tex_directory,
																			uva_name,
																			dummy_names,
																			i_Type,
																			true, mipmap_if_2D);
		
						l_NRTextures[new_texture_page] = TextureInfoNR(uva_locator, new_texture_page);
						uva_locator.Pop();
						uva_texture->AddTexturePage(new_texture_page);
					}
					l_AutoCleanupUVATextures.insert(uva_texture);
				}
			}
			catch ( const g2dOutOfVideoMemoryX& )
			{
				std::string dbg_filename;
				fsFileUtil::LocatorToANSIFilename( tex_locator, dbg_filename );
				DBG_ERROR("Not enough video memory to load " << dbg_filename.c_str() << "." );
				DumpTextureList();
				
				if (new_texture)
					ReleaseTexture(new_texture);
				throw; // bring down the game
			}
			catch ( const g2dOutOfSystemMemoryX& )
			{
				std::string dbg_filename;
				fsFileUtil::LocatorToANSIFilename( tex_locator, dbg_filename );
				DBG_ERROR("Not enough system memory to load " << dbg_filename.c_str() << "." );
				DumpTextureList();

				if (new_texture)
					ReleaseTexture(new_texture);
				// propagate the out of system memory exception
				throw g2dOutOfSystemMemoryX(); // bring down the game
				//throw;
			}
			catch ( const envExceptionX& i_Ex )

			{
				DBG_WARNING(i_Ex.GetErrorMessage());
				return NULL;
			}
		}

		//DBG_ASSERT(NULL != new_texture, "Invalid NULL texture loaded!");
		if (new_texture)
			l_Textures[tex_locator] = TextureInfo(new_texture);
		return new_texture;
	}
	else
	{
		++(it->second.m_Count);
		return it->second.m_Texture;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
matTexture* LoadTexture(const fsResourceFinder& i_Finder, const itString& i_Name, const TEXTURE_TYPE i_Type/* = TEXTURE_TYPE_2D*/, const bool mipmap_if_2D /*= true */)
{
	fsLocator found_loc;
	if (i_Finder.FindResource(i_Name, found_loc))
	{
		// found_loc returns full path including filename
		fsLocator dir = found_loc;
		itString name = dir.GetLastName();
		dir.Pop();

		return LoadTexture(dir, name, i_Type, mipmap_if_2D);
	}
	return NULL;
}


//--------------------------------------------------------------------
//	WriteUVATexture writes the given UVA to a binary file with the
//	name i_Locator.
//--------------------------------------------------------------------
void WriteUVATexture(	const matUVATexture& i_Texture,
						const fsLocator& i_Locator,
						const itString* i_Textures)
{
	gfFileBin file(i_Locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
	file.WriteHeader(0, 0, 1);
	chBinWriter writer(file);
	matUVATextureParseUtil::WriteUVATexture(writer, i_Texture, i_Textures);
}

//--------------------------------------------------------------------
//	WriteUVATexture writes the given UVA using the given chWriter.
//--------------------------------------------------------------------
void WriteUVATexture(	const matUVATexture& i_Texture,
						chWriter& i_Writer,
						const itString* i_Textures)
{
	matUVATextureParseUtil::WriteUVATexture(i_Writer, i_Texture, i_Textures);
}

//--------------------------------------------------------------------
//	ReadUVATexture reads the given UVA from a binary file with the
//	name i_Locator.  The number of std::strings in the i_Textures
//	array must be the same as the number of texture pages in the
//	texture.
//--------------------------------------------------------------------
matUVATexture* ReadUVATexture(const fsLocator& i_Locator,
							  std::vector<itString>& o_Textures)
{
	gfFileBin file(i_Locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);
	gfFileBin::Header header;
	file.ReadHeader(&header);

	chBinReader Reader(file);
	return matUVATextureParseUtil::ReadUVATexture(Reader, o_Textures);
}

//--------------------------------------------------------------------
//	CreateUVATexture creates a "blank" UVA texture.  When this option
//	is used to create a UVA texture, the textures later added to the
//	UVA must be manually destroyed by the user.
//--------------------------------------------------------------------
matUVATexture* CreateUVATexture()
{
	matUVATexture *new_texture = new matUVATexture;
	l_UVATextures.push_back(new_texture);

	// TODO:  replace with real
	l_NRTextures[new_texture] = TextureInfoNR("UVA on the fly.", new_texture);

	return new_texture;
}


//--------------------------------------------------------------------
//	CreateShadowMap creates a shadow map texture that can 
//	be rendered to. Use the GetRenderTargetAPI() function
//	on the returned texture in order to render to it.
//--------------------------------------------------------------------
matTexture* CreateShadowMap(int i_nTextureWidth, int i_nTextureHeight)
{
	DBG_WARNING("No texture manager implementation yet.");
	if (!l_pImpl)
		return NULL;
	return l_pImpl->CreateShadowMap(i_nTextureWidth, i_nTextureHeight);
}

//--------------------------------------------------------------------
//	CreateShadowMap creates a reflective shadow map texture that can 
//	be rendered to. Use the GetRenderTargetAPI() function
//	on the returned texture in order to render to it.
//--------------------------------------------------------------------
matTexture* CreateReflectiveShadowMap(int i_nTextureWidth, int i_nTextureHeight)
{
	DBG_ASSERT(l_pImpl, "No texture manager implementation yet.");
	if (!l_pImpl)
		return NULL;
	return l_pImpl->CreateReflectiveShadowMap(i_nTextureWidth, i_nTextureHeight);
}

//--------------------------------------------------------------------
//	CreateRenderTargetTexture creates a texture that can 
//	be rendered to. Use the GetRenderTargetAPI() function
//	on the returned texture in order to render to it.
//--------------------------------------------------------------------
matTexture* CreateRenderTargetTexture(int i_nTextureWidth,
										int i_nTextureHeight,
										bool i_bFloatingPoint,
										const g2dPFD* i_PFD,
										bool i_bAutoGenMipmap,
										bool i_bAllocDepthBuffer,
										eResourceCategory i_ResourceCategory, /*= e_SceneTexture*/
										bool i_bFloatDepth,
										bool i_bAntiAlias)
{
//	DBG_ASSERT(l_pImpl, "No texture manager implementation yet.");
	if (!l_pImpl)
		return NULL;
	return l_pImpl->CreateRenderTargetTexture(i_nTextureWidth, i_nTextureHeight, 
		i_bFloatingPoint, i_PFD, i_bAutoGenMipmap, i_bAllocDepthBuffer, i_ResourceCategory, i_bFloatDepth, i_bAntiAlias );
}

//--------------------------------------------------------------------
//	CreateRenderTargetTexture creates a texture that can 
//	be rendered to. Use the GetRenderTargetAPI() function
//	on the returned texture in order to render to it.
//	The texture is initialized with data from the given input texture.
//--------------------------------------------------------------------
matTexture* CreateRenderTargetTexture(matTexture* i_Texture,
									  bool i_bAutoGenMipmap,
									  eResourceCategory i_ResourceCategory /*= e_SceneTexture*/)
{
	DBG_ASSERT(l_pImpl, "No texture manager implementation yet.");
	if (!l_pImpl)
		return NULL;
	return l_pImpl->CreateRenderTargetTexture(i_Texture, i_bAutoGenMipmap, i_ResourceCategory);
}

//--------------------------------------------------------------------
//	CreateCubeRenderTargetTexture creates a cubemap texture that can 
//	be rendered to. 
//--------------------------------------------------------------------
matTexture* CreateCubeRenderTargetTexture(int i_nTextureWidth,
											int i_nTextureHeight,
											g2dPFD* i_PFD)
{
	DBG_ASSERT(l_pImpl, "No texture manager implementation yet.");
	if (!l_pImpl)
		return NULL;
	return l_pImpl->CreateCubeRenderTargetTexture(i_nTextureWidth, i_nTextureHeight, i_PFD);
}

//--------------------------------------------------------------------
//	IncrementReference - adds usage reference to the given texture.
//	A call to ReleaseTexture() will decrement the reference.
//--------------------------------------------------------------------
bool IncrementReference(matTexture* i_Texture)
{
	// check reusable textures first
	Textures::iterator it = l_Textures.begin();
	Textures::iterator end = l_Textures.end();

	for( ; it != end; ++it )
	{
		if( it->second.m_Texture == i_Texture )
		{
			// Increment reference
			++(it->second.m_Count);
			return true;
		}
	}
	// not found
	return false;
}

//--------------------------------------------------------------------
//	ReleaseTexture causes the texture object to be destroyed and
//	the texture information to be removed from memory.
//--------------------------------------------------------------------
void ReleaseTexture(matTexture* i_Texture)
{
	if (!i_Texture)
	{
		//no texture to delete
		return;
	}
	
	// check reusable textures first
	Textures::iterator it = l_Textures.begin();
	Textures::iterator end = l_Textures.end();

	for( ; it != end; ++it )
	{
		
		if( it->second.m_Texture == i_Texture )
			break;
	}

	if ( it != l_Textures.end() )
	{
		fsResourceTracker::Remove(it->first);
		DBG_ASSERT( it->second.m_Count > 0 , "Texture should be gone already");
		if (it->second.m_Count > 0)
			--(it->second.m_Count);

		if( it->second.m_Count == 0 )
		{
			destroy_texture(i_Texture);
			l_Textures.erase(it);
		}
	}
	else
	{

		NRTextures::iterator nr_it = l_NRTextures.find(i_Texture);

		if ( nr_it != l_NRTextures.end() )
		{
			destroy_texture(i_Texture);
			l_NRTextures.erase(nr_it);
		}
		else
		{
			// Render target textures will pass through here.
			// (We may want to keep track of them in order to do error
			// checking on the other types of textures.)
			//DBG_WARNING("Destroying texture that was never mapped!");
			destroy_texture(i_Texture);
		}
	}
}

//--------------------------------------------------------------------
//	DestroyAllTextures causes all textures to be destroyed.
//--------------------------------------------------------------------
void DestroyAllTextures()
{
	std::for_each(l_UVATextures.begin(), l_UVATextures.end(), destroy_uva);
	std::for_each(l_AutoCleanupUVATextures.begin(), l_AutoCleanupUVATextures.end(), destroy_uva_and_subtextures);
	l_pImpl->DestroyAllTextures();

	l_Textures.clear();
	l_NRTextures.clear();
}


//------------------------------------------------------------------------
// DumpTextureList will DBG_WARNING all of the loaded textures
//------------------------------------------------------------------------
void DumpTextureList()
{
	dump_texture_list();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void ReloadTexture( const fsLocator& i_Locator, bool i_bIsMipMap )
{
	// Force all the names in the list (unexpanded) to lower case
	// for texture loading
	//gpxRenderControl::ConfirmSingleThread();
	fsLocator tex_locator(i_Locator);
	int count = tex_locator.GetNumNames();
	itString temp;
	for ( int i = 0; i < count; ++i )
	{
		temp = tex_locator.GetName(i);
		tex_locator.ReplaceName( itStringUtil::ToLower(temp), i );
	}

	std::map<fsLocator, TextureInfo>::iterator it = l_Textures.end();
	it = l_Textures.find(tex_locator);
	if( it != l_Textures.end() )
	{
		UnloadTexture(tex_locator);
		l_pImpl->ReloadTexture(tex_locator, i_bIsMipMap);
	}
	//gpxRenderControl::SetNeedsNewRender();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void UnloadTexture( const fsLocator& i_Locator )
{
	l_pImpl->UnloadTexture(i_Locator);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void SaveTextureToFile(matTexture* i_pTexture, 
	const fsLocator& i_FilePathLocator)
{
	DBG_ASSERT(l_pImpl, "No texture manager implementation yet.");
	if (!l_pImpl)
		return;
	l_pImpl->SaveTextureToFile(i_pTexture, i_FilePathLocator);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void SaveTextureToRgbaTiff(matTexture* i_pTexture, 
	const fsLocator& i_FilePathLocator)
{
	DBG_ASSERT(l_pImpl, "No texture manager implementation yet.");
	if (!l_pImpl)
		return;
	l_pImpl->SaveTextureToRgbaTiff(i_pTexture, i_FilePathLocator);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void SaveTextureToRgbaPNG(matTexture* i_pTexture, 
	const fsLocator& i_FilePathLocator)
{
	DBG_ASSERT(l_pImpl, "No texture manager implementation yet.");
	if (!l_pImpl)
		return;
	l_pImpl->SaveTextureToRgbaPNG(i_pTexture, i_FilePathLocator);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void FillTexture(matTexture* i_pTexture, const maFloatRGBA& i_Color)
{
	DBG_ASSERT(l_pImpl, "No texture manager implementation yet.");
	if (!l_pImpl)
		return;
	l_pImpl->FillTexture(i_pTexture, i_Color);
}

//------------------------------------------------------------------------
// Filltexture fill texture with input pixel data
// Note: i_Size shuold be the byte that i_PixelData contains
//------------------------------------------------------------------------
void FillTexture(matTexture* i_pTexture, void* i_PixelData, int i_nByte)
{
//	DBG_ASSERT(l_pImpl, "No texture manager implementation yet.");
	if (!l_pImpl)
		return;
	l_pImpl->FillTexture(i_pTexture, i_PixelData, i_nByte);
}

void UpdateSurface( matTexture* i_pTexture, unsigned char* i_data, int size, int nMipLevel )
{
	DBG_ASSERT(l_pImpl, "No texture manager implementation yet.");
	if (!l_pImpl)
		return;
	l_pImpl->UpdateSurface( i_pTexture, i_data, size, nMipLevel );
}


//--------------------------------------------------------------------
//	CreateTexture creates a texture 
//  NOTE: The texture will be initialized if i_PixelData and i_Size
//	are set.
//--------------------------------------------------------------------
matTexture* CreateTexture(int i_nTextureWidth,
	int i_nTextureHeight,
	const g2dPFD* i_PFD,
	bool i_bMipmap,
	void* i_PixelData)
{
//	DBG_ASSERT(l_pImpl, "No texture manager implementation yet.");
	if (!l_pImpl)
		return NULL;

	if (i_PixelData == NULL)
	{
		return l_pImpl->CreateTexture(i_nTextureWidth, i_nTextureHeight,
			i_PFD, i_bMipmap);
	}
	else
	{
		return l_pImpl->CreateTexture(i_nTextureWidth, i_nTextureHeight,
			i_PFD, i_bMipmap, i_PixelData);
	}
}

//------------------------------------------------------------------------
//	SetTextureAdjuster replaces the currently used matTextureAdjuster
//	NOTE: a default one is created automatically on Init, this is not
//	necessary for a game to do unless it requires better decision-making
//	than that which is provided by default above
//------------------------------------------------------------------------
void SetTextureAdjuster(matTextureAdjuster *i_pTextureAdjuster)
{
	delete l_pTextureAdjuster;
	l_pTextureAdjuster = i_pTextureAdjuster;
}

const matTextureAdjuster * GetTextureAdjuster()
{
	return l_pTextureAdjuster;
}

//--------------------------------------------------------------------
//	combine 2 textures on render target
//--------------------------------------------------------------------
void MergeTransparentTextures(matTexture* i_pTexO, matTexture* i_pTexI, g2dRenderTarget* io_pTarget)
{
	DBG_ASSERT(l_pImpl, "No texture manager implementation yet.");
	if (!l_pImpl)
		return;
	return l_pImpl->MergeTransparentTextures(i_pTexO, i_pTexI, io_pTarget);
}

matTexture* CreateTexture3D( int i_nTextureWidth, int i_nTextureHeight, int i_nTextureDepth, const g2dPFD* i_PFD /*= NULL*/, BIND_TYPE i_Bindings /*= BIND_SHADER_RESOURCE*/, void* i_PixelData /*= NULL*/ )
{
	DBG_ASSERT(l_pImpl, "No texture manager implementation yet.");
	if (!l_pImpl)
		return NULL;

	if (i_PixelData == NULL)
	{
		return l_pImpl->CreateTexture3D(i_nTextureWidth, i_nTextureHeight, i_nTextureDepth,
			i_PFD, i_Bindings );
	}
	else
	{
		return l_pImpl->CreateTexture3D(i_nTextureWidth, i_nTextureHeight, i_nTextureDepth,
			i_PFD, i_Bindings, i_PixelData);
	}
}

//--------------------------------------------------------------------
//	CopyTexture copy one texture to another
//--------------------------------------------------------------------
void CopyTexture(matTexture* i_SrcTex, matTexture* i_DstTex)
{
	DBG_ASSERT(l_pImpl, "No texture manager implementation yet.");
	if (!l_pImpl)
		return;

	l_pImpl->CopyTexture(i_SrcTex, i_DstTex);
}
}














