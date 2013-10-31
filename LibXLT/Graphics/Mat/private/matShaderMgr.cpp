/****************************************************************************\
**  matShaderMgr.hpp
**
**      matShaderMgr manages an array of shaders that chooses the 
**	appropriate shader based on the combination of texture layers.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mat/matShaderMgr.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/gf/gfFileEnum.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Graphics/eff/effShaderParams.hpp"
#include "Graphics/mat/matMaterial.hpp"

#include <map>


//============================================================================
//============================================================================
namespace matShaderMgr
{

namespace
{
	matShaderMgrImpl* l_pImpl = NULL;

	bool l_bEnabled = false;

	fsLocator l_ShadersRoot;

	const std::string l_PostEffectFolder("PostEffect");

	// the built-in shaders here, keyed by name.
	// since these are built-ins, we could use enums for faster access.
	std::map<std::string, matShaderInfo> l_ShaderMap;

	// cache of loaded user shaders
	std::map<fsLocator, matShaderEffect*> l_LoadedShaders;

	const int c_NumBitsPerLayer = 3; // holds 8 types (Nothing, Error, plus 6 others)
	const int c_MaxNumLayers = 4; // could be 8 later

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	enum Codes
	{
		e_Nothing = 0,
		e_Decal = 1,
		e_Alpha = 2,
		e_Bump = 3,
		e_Specular = 4,
		e_EnvMap = 5,
		e_Gloss = 6,
		e_Error = 7
	};

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void clear_map()
	{
		std::map<std::string, matShaderInfo>::iterator it = l_ShaderMap.begin();
		for (; it != l_ShaderMap.end(); ++it)
		{
			delete it->second.m_DataTemplate;
			delete it->second.m_pEffect;
		}
		l_ShaderMap.clear();

		std::map<fsLocator, matShaderEffect*>::iterator it2 = l_LoadedShaders.begin();
		for (; it2 != l_LoadedShaders.end(); ++it2)
		{
			delete it2->second;
		}
		l_LoadedShaders.clear();
	}

}	// end of namespace

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void SetImplementation(matShaderMgrImpl* i_pImpl)
{
	l_pImpl = i_pImpl;
}

//------------------------------------------------------------------------
// uppercase()
//------------------------------------------------------------------------
std::string uppercase(std::string arg)
{
   for (int x=0; x<arg.length(); x++)
   {
	   if (arg[x] >= 'a' && arg[x] <= 'z') arg[x]+= ('A'-'a');
   }
   return arg;
}

//------------------------------------------------------------------------
// Initialize system
//------------------------------------------------------------------------
void Initialize()
{
	if (l_pImpl)
	{
		fsLocator fx_dir = gfPaths::GetPath(gfPaths::e_ExePath);
		fx_dir.Push("Shaders");

		if (!fsFileUtil::DirectoryExists(fx_dir))
		{
			// If the directory does not exist, look for
			// an environment variable
			char *shdr_dir = getenv("LIBXLT_SHADER_DIR");
			if (shdr_dir)
			{
				fsFileUtil::ANSIFilenameToLocator(shdr_dir, fx_dir);
			}
		}

		// Ensure drive letter is in upper case
		itString fx_dir_itStr;
		fsFileUtil::LocatorToUnicodeString( fx_dir.GetName(0) , fx_dir_itStr );
		std::string driveLetter = uppercase( itStringUtil::GetStdString(fx_dir_itStr) );
		fx_dir.ReplaceName( itString(driveLetter.c_str()) , 0 );

		l_ShadersRoot = fx_dir;

		clear_map();
		l_pImpl->RegisterEffects(fx_dir, l_ShaderMap);
		//l_pImpl->RegisterUserShaders(fx_dir, l_UserShaders);
		
	}
}

//------------------------------------------------------------------------
// Clean up system
//------------------------------------------------------------------------
void DeInitialize()
{
	clear_map();
}

//------------------------------------------------------------------------
// Get the main shaders root path
//------------------------------------------------------------------------
fsLocator GetDefaultShaderPath()
{
	return l_ShadersRoot;
}

//------------------------------------------------------------------------
// Get the main shaders root path
//------------------------------------------------------------------------
std::string GetPostShaderFolder()
{
	return l_PostEffectFolder;
}

//------------------------------------------------------------------------
// ResolveShaderPath
//------------------------------------------------------------------------
fsLocator ResolveShaderPath(const fsLocator& i_ShaderName)
{
	fsLocator shaderLoc = i_ShaderName;	

	// If its MachStudio shader, change shaderLoc to point to default shader path
	if ( shaderLoc.GetNumNames() == 1 )
	{
		// Get the shader name
		std::string shaderFileName = itStringUtil::GetStdString(shaderLoc.GetLastName());

		shaderLoc = matShaderMgr::GetDefaultShaderPath();
		shaderLoc.Push( shaderFileName.c_str() );
	} 

	return shaderLoc;
}

effShaderData* CreateData(const std::string& i_EffectID)
{
	// maps a data type to a shader id name

	if (!l_bEnabled) 
		return NULL;

	std::map<std::string, matShaderInfo>::iterator it = l_ShaderMap.find( i_EffectID );
	if (it != l_ShaderMap.end())
	{
//		DBG_ASSERT(it->second.m_DataTemplate != NULL, "bad shader info data template");
		if (!it->second.m_DataTemplate)
			return NULL;
		return it->second.m_DataTemplate->Clone();
	}
	else
	{
		// shader not registered in map? check initialization and id.
		// assert here?
		return NULL;
	}
}

//------------------------------------------------------------------------
// GetEffect - return effect for given material. This will return NULL
//	if no effect is needed.
//------------------------------------------------------------------------
matShaderEffect* GetEffect(const matMaterial &i_Material, int i_MaterialLayerIndex)
{
	if (!l_bEnabled) 
		return NULL;

	shared_ptr<effShaderParams> pParams = i_Material.GetShaderParams(i_MaterialLayerIndex);
	if (pParams)
	{
		matShaderEffect* eff = NULL;
		eff = pParams->GetShader();
		if (eff != NULL)
			return eff;
	}

	if (i_Material.GetEffectID().empty())
	{
		DBG_ASSERT(false, "material has no shader effect associated");
	}
	else
	{
		return GetSpecialEffect(i_Material.GetEffectID());
	}

	// Note: could return error shader here?
	// This line is never reached.
	return NULL;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
matShaderEffect* GetSpecialEffect(const std::string& i_Name)
{
	if (!l_bEnabled) 
		return NULL;

	// assuming special effect shaders are already loaded.
	std::map<std::string, matShaderInfo>::iterator it = l_ShaderMap.find( i_Name );
	if (it == l_ShaderMap.end()) {
		DBG_WARNING("Failed shader lookup due to missing entry for " << i_Name);
		return NULL;
	}
	DBG_ASSERT(it->second.m_pEffect != NULL, "Failed shader lookup due to NULL shader entry for " << i_Name);
	if (!it->second.m_pEffect)
		return NULL;
	return it->second.m_pEffect;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
matShaderEffect* GetEffect(const fsLocator& i_PathToShader)
{
	if (!l_bEnabled) 
		return NULL;

	if (l_pImpl == NULL) {
		return NULL;
	}

	std::map<fsLocator, matShaderEffect*>::iterator it = l_LoadedShaders.find( i_PathToShader );
	if ( it == l_LoadedShaders.end() )
	{
		// load if not found!
		return l_pImpl->LoadEffect(i_PathToShader, l_LoadedShaders);
	}
	else
	{
		return it->second;
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
matShaderEffect* GetPostEffect(const fsLocator& i_PathToShader)
{
	if (!l_bEnabled) 
		return NULL;

	fsLocator path = i_PathToShader;
	std::string lastName = itStringUtil::GetStdString(path.GetLastName());

	std::map<std::string, matShaderInfo>::iterator it = l_ShaderMap.find( lastName );
	if (it == l_ShaderMap.end() || it->second.m_pEffect == NULL)
	{	
		// dealing with naming of post effect
		if (i_PathToShader.GetNumNames() == 2 && 
			i_PathToShader.GetName(0) == itString(GetPostShaderFolder().c_str()))
		{
			std::string tmp_s;
			fsFileUtil::LocatorToANSIFilename(i_PathToShader, tmp_s);
			path = fsLocator(itString(tmp_s.c_str()));
		}

		std::map<fsLocator, matShaderEffect*>::iterator it = l_LoadedShaders.find( path );
		if ( it == l_LoadedShaders.end() )
		{
			// load if not found!
			if (l_pImpl)
				return l_pImpl->LoadEffect(path, l_LoadedShaders);
			else
				return NULL;
		}
		else
		{
			return it->second;
		}
	}
	else
	{
		return it->second.m_pEffect;
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void ReloadShader( matMaterial * i_pMaterial )
{
	fsLocator shaderLoc = i_pMaterial->GetShaderParams()->GetShaderName();

	// Clear params
	shared_ptr<effShaderParams> params = i_pMaterial->GetShaderParams();
	params->Clear();
	i_pMaterial->SetShaderParams(params);

	// Reload
	l_pImpl->UnloadEffect(shaderLoc, l_LoadedShaders);
	l_pImpl->LoadEffect(shaderLoc, l_LoadedShaders);

}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool IsMachStudioShader(const fsLocator& i_ShaderName)
{	
	fsFileEnum::fsFileList flist;
	std::vector<itString> searchStrings;
	searchStrings.push_back(itString(".fx"));
	fsFileEnum::EnumerateFiles(matShaderMgr::GetDefaultShaderPath(), flist, searchStrings);
	
	int i;
	for ( i = 0 ; i < flist.size() ; i++ )
	{
		// Filenames match
		if ( flist[i] == i_ShaderName )
		{
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------
// Turn on/off use of the shader array.  If not enabled, the function
//	"GetEffect" will return NULL for all materials.
//------------------------------------------------------------------------
bool GetUseShaderArray()
{
	return l_bEnabled;
}
void SetUseShaderArray(bool i_bEnable)
{
	l_bEnabled = i_bEnable;
}

void visitShaders(matShaderMapVisitor* visitor)
{
	std::map<std::string, matShaderInfo>::iterator it = l_ShaderMap.begin();
	for (; it != l_ShaderMap.end(); ++it)
	{
		visitor->visit(it->first, it->second.m_pEffect);
	}
}

}	// end of namespace
