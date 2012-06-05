/*****************************************************************************
**  effShaderParams.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effShaderParams.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/Fs/fsAbsolutePathMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/Fs/fsResourceFinder.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/prty/prtyTextureFileChooserUIInfo.hpp"
#include "Graphics/g3d/g3dExceptionX.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"


//------------------------------------------------------------------------
// abstract copy 
//------------------------------------------------------------------------
effParamFloat* effParamFloat::Clone()
{
	effParamFloat* clone = new effParamFloat(m_Name, m_Property.GetPropertyName(), m_Property.GetValue());
	return clone;
}

//------------------------------------------------------------------------
// abstract copy 
//------------------------------------------------------------------------
effParamBool* effParamBool::Clone()
{
	effParamBool* clone = new effParamBool(m_Name, m_Property.GetPropertyName(), m_Property.GetValue());
	return clone;
}

//------------------------------------------------------------------------
// abstract copy 
//------------------------------------------------------------------------
effParamInt* effParamInt::Clone()
{
	effParamInt* clone = new effParamInt(m_Name, m_Property.GetPropertyName(), m_Property.GetValue());
	return clone;
}

//------------------------------------------------------------------------
// abstract copy 
//------------------------------------------------------------------------
effParamColor* effParamColor::Clone()
{
	effParamColor* clone = new effParamColor(m_Name, m_Property.GetPropertyName(), m_Property.GetValue());
	return clone;
}


//------------------------------------------------------------------------
// To support old code, this constructor is kept around in order
// to check for empty strings in the itString, which cannot
// be Push'ed onto the fsLocator.
//------------------------------------------------------------------------
effParamTexture::effParamTexture(const std::string& i_Name, 
								 const std::string& i_PrtyName, 
								 const itString& i_Default)
: effShaderParam(i_Name), 
  m_Property(i_PrtyName, (i_Default.GetLength()>0) ? fsLocator(i_Default) : fsLocator()), 
  m_Texture(NULL), m_pRampTexture(NULL), m_Type( TEXTURE_TYPE_UNKNOWN )
{
}

//------------------------------------------------------------------------
// abstract copy 
//------------------------------------------------------------------------
effParamTexture* effParamTexture::Clone()
{
	effParamTexture* clone = new effParamTexture(m_Name, m_Property.GetPropertyName(), m_Property.GetValue());
	clone->m_Texture = m_Texture;
	/*if (m_pRampTexture)
		clone->m_pRampTexture = matTextureMgr::CreateRenderTargetTexture(m_pRampTexture->GetWidth(), m_pRampTexture->GetHeight(), false, NULL);
	else
		clone->m_pRampTexture = m_pRampTexture;*/
	clone->m_TextureName = m_TextureName;
	clone->m_RampObject = m_RampObject;
	clone->Property() = m_Property;
	return clone;
}


//------------------------------------------------------------------------
// load texture based on prty value, updating restore name if successful
//------------------------------------------------------------------------
bool effParamTexture::Load(const fsResourceFinder& i_Finder)
{
	bool success = false;
	fsLocator newName = m_Property.GetValue();
	if (newName.GetNumNames() == 0)
	{
		m_TextureName = newName;
		m_Texture = NULL;
		success = true;
	}
	else
	{
		// Load new texture name.
		// Only use ResourceFinder if we only have single filename, otherwise
		// interpret locator as full path.
		// if exception thrown, delete texture. use auto_ptr for this.
		std::auto_ptr<matTexture> tex;
		if (newName.GetNumNames() == 1)
		{
			//tex.reset(matTextureMgr::LoadTexture(i_Finder, newName.GetLastName()));

			// We can't be using the absolute path mgr here to resolve paths
			// because then we would get the skip warning too often.  At this point,
			// all we can do is try to load the texture file and warn when there is an error.
			// We need to distinguish between "file not found" and "error loading texture".
			//
			fsLocator found_loc;
			itString name = newName.GetLastName();
			if (i_Finder.FindResource(name, found_loc))
			{
				// found_loc returns full path including filename
				newName = found_loc;
			}
			else
			{
				//// Give the single filename to the absolute path mgr in order
				//// to ask the user where the texture should be found
				//if (fsAbsolutePathMgr::ResolvePath(newName))
				//	tex.reset(matTextureMgr::LoadTexture(newName));
				//else
				//DBG_ERROR( "Failed to locate Texture " << newName );
			}
		}

		//bga - Can't resolve the path here because the information
		// would not get propogated to the material information
		// that would be displayed and saved. The path resolve
		// happens on file load.
		if (fsFileUtil::FileExists(newName))
		//if (fsAbsolutePathMgr::ResolvePath(newName))
		{
			tex.reset(matTextureMgr::LoadTexture(newName, GetType()));
			if (!tex.get())
			{
				if (matTextureMgr::IsSkipAllTextures())
				{
					// We are skipping textures, use warning, not error here
					DBG_WARNING( "Skipping texture file " << newName );
				}
				else
				{
					// File exists but cannot be loaded
					DBG_ERROR( "Failed to load texture file " << newName );
				}
			}
		}
		else
		{
			// File doesn't exist. At this point, we can only just skip
			// it. The user may have already been displayed the 
			// Locate dialog and chosen Skip, so we shouldn't tell them
			// again.
			//throw fsFileDoesntExistX(newName);
			DBG_ERROR( "Failed to locate texture file " << newName );
		}

		if (tex.get() != NULL)
		{
			// load succeeded, change data.
			m_TextureName = newName;
			m_Texture = tex.release();
			success = true;

			// If we successfully loaded a texture from a single filename,
			// set the fullpath into the property.
			if (m_Property.GetValue().GetNumNames() == 1)
			{
				//preserve callback state
				prtyTextureFileData val;
				val.m_TextureLocator = m_TextureName;
				val.m_CurrentCallback = m_Property.GetFullValue().m_CurrentCallback;
				m_Property.SetValueWithoutNotify( val );
			}
		}
	}
	// if success is false, data did not change.
	return success;
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
effShaderParams::effShaderParams()
:	m_pShader(NULL), m_pShaderBindings(NULL), m_pPrtyUI(NULL),
	m_pDiffuseMap(NULL), m_pTransparencyMap(NULL), 
	m_pDiffuseColor(NULL), m_pTransparency(NULL),
	m_pPathObject(NULL), m_Version(0)
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
effShaderParams::~effShaderParams() 
{
	if (m_pShaderBindings) 
		delete m_pShaderBindings;
	if (m_pPrtyUI)
		delete m_pPrtyUI;
	
	envSTLHelpers::DeleteContainer(m_ColorParams);
	envSTLHelpers::DeleteContainer(m_TextureParams);
	envSTLHelpers::DeleteContainer(m_FloatParams);
	envSTLHelpers::DeleteContainer(m_BoolParams);
	envSTLHelpers::DeleteContainer(m_IntParams);
}

//------------------------------------------------------------------------
// abstract copy
//------------------------------------------------------------------------
effShaderParams* effShaderParams::Clone() const
{
	// does not create new bindings nor prty UI.

	effShaderParams* clone = new effShaderParams;
	clone->m_ShaderName = m_ShaderName;
	clone->m_pShader = m_pShader;
	clone->m_Version = m_Version;

	for (int i = 0; i < m_FloatParams.size(); i++)
	{
		clone->AddParam(m_FloatParams[i]->Clone());
	}
	for (int i = 0; i < m_BoolParams.size(); i++)
	{
		clone->AddParam(m_BoolParams[i]->Clone());
	}
	for (int i = 0; i < m_IntParams.size(); i++)
	{
		clone->AddParam(m_IntParams[i]->Clone());
	}
	for (int i = 0; i < m_ColorParams.size(); i++)
	{
		clone->AddParam(m_ColorParams[i]->Clone());
	}
	for (int i = 0; i < m_TextureParams.size(); i++)
	{
		clone->AddParam(m_TextureParams[i]->Clone());
	}
	return clone;
}

//------------------------------------------------------------------------
// shader access
//------------------------------------------------------------------------
void effShaderParams::SetShaderName(const fsLocator& i_ShaderPath, matShaderEffect* i_pShader /*= NULL*/)
{
	// if the names are the same, then don't modify the object at all.
	if (i_ShaderPath == m_ShaderName)
		return;

	m_ShaderName = i_ShaderPath;
	m_pShader = i_pShader;
}

//------------------------------------------------------------------------
// UpdateShaderFunction
//------------------------------------------------------------------------
void effShaderParams::SetShaderPathObject(prtyTextureFileName* i_Path)
{
	m_pPathObject = i_Path;
}


//------------------------------------------------------------------------
// shader access
//------------------------------------------------------------------------
matShaderEffect* effShaderParams::GetShader()
{
	// this will load shader if it hasn't been instantiated yet!
	if (m_pShader == NULL)
	{
		// Make sure shader is loaded in as a tokenized fsLocator
		itString itName;
		fsLocator shaderLoc;	
		fsFileUtil::LocatorToUnicodeString( m_ShaderName, itName );	
		fsFileUtil::UnicodeStringToLocator( itName, shaderLoc );

		// Get the shader name
		std::string shaderFileName = itStringUtil::GetStdString(shaderLoc.GetLastName());

		//DBG_TRACE("Loading shader " << shaderFileName << " from saved directory.");
		m_pShader = matShaderMgr::GetEffect( shaderLoc );

		// Shader failed to load, try the .mab directory
		if ( m_pShader == NULL )
		{
			//DBG_TRACE("Loading shader " << shaderFileName << " from .mab directory.");
			shaderLoc = gfPaths::GetPath(gfPaths::e_MabPath);
			shaderLoc.Pop();
			shaderLoc.Push( shaderFileName.c_str() );
			m_pShader = matShaderMgr::GetEffect( shaderLoc );

		}

		// Shader failed to load again, try to find using locate dialog
		if (m_pShader == NULL)
		{
			//DBG_TRACE("Loading shader " << shaderFileName << " from user-located directory.");
			fsAbsolutePathMgr::ResolvePath(shaderLoc, "Shaders");
			m_pShader = matShaderMgr::GetEffect( shaderLoc );
		}		

		// Total failure. Revert back to simple.
		if (m_pShader == NULL)
		{
			//DBG_TRACE("Loading shader Simple.fx instead of " << shaderFileName);
			std::string fullpath;
			fsFileUtil::LocatorToANSIFilename(shaderLoc, fullpath);
			DBG_WARNING("Material Shader = " <<  shaderFileName.c_str() << " load FAILED using path: " << fullpath.c_str());
			m_pShader = matShaderMgr::GetSpecialEffect("Simple.fx");
			m_ShaderName = itString("Simple.fx");
		}

		if ( m_pShader )
			m_ShaderName = shaderLoc;

		// Catastrophe. Couldnt even find Simple.fx
		DBG_ASSERT(m_pShader != NULL, "No shader could be loaded for " << m_ShaderName);
		if (!m_pShader)
			return NULL;
	}
	return m_pShader;
}

//------------------------------------------------------------------------
// add parameters
//------------------------------------------------------------------------
void effShaderParams::AddParam(effParamTexture* i_Param)
{
	m_TextureParams.push_back(i_Param);
}
//------------------------------------------------------------------------
// add parameters
//------------------------------------------------------------------------
void effShaderParams::AddParam(effParamColor* i_Param)
{
	m_ColorParams.push_back(i_Param);
}
//------------------------------------------------------------------------
// add parameters
//------------------------------------------------------------------------
void effShaderParams::AddParam(effParamFloat* i_Param)
{
	m_FloatParams.push_back(i_Param);
}
//------------------------------------------------------------------------
// add parameters
//------------------------------------------------------------------------
void effShaderParams::AddParam(effParamBool* i_Param)
{
	m_BoolParams.push_back(i_Param);
}
//------------------------------------------------------------------------
// add parameters
//------------------------------------------------------------------------
void effShaderParams::AddParam(effParamInt* i_Param)
{
	m_IntParams.push_back(i_Param);
}

//------------------------------------------------------------------------
// get parameters in bulk
//------------------------------------------------------------------------
void effShaderParams::GetAllTextureParams(std::vector<effParamTexture*>& o_TextureParams) const
{
	o_TextureParams.insert(o_TextureParams.end(), m_TextureParams.begin(), m_TextureParams.end());
}
//------------------------------------------------------------------------
// get parameters in bulk
//------------------------------------------------------------------------
void effShaderParams::GetAllColorParams(std::vector<effParamColor*>& o_ColorParams) const
{
	o_ColorParams.insert(o_ColorParams.end(), m_ColorParams.begin(), m_ColorParams.end());
}
//------------------------------------------------------------------------
// get parameters in bulk
//------------------------------------------------------------------------
void effShaderParams::GetAllFloatParams(std::vector<effParamFloat*>& o_FloatParams) const
{
	o_FloatParams.insert(o_FloatParams.end(), m_FloatParams.begin(), m_FloatParams.end());
}
//------------------------------------------------------------------------
// get parameters in bulk
//------------------------------------------------------------------------
void effShaderParams::GetAllBoolParams(std::vector<effParamBool*>& o_BoolParams) const
{
	o_BoolParams.insert(o_BoolParams.end(), m_BoolParams.begin(), m_BoolParams.end());
}
//------------------------------------------------------------------------
// get parameters in bulk
//------------------------------------------------------------------------
void effShaderParams::GetAllIntParams(std::vector<effParamInt*>& o_IntParams) const
{
	o_IntParams.insert(o_IntParams.end(), m_IntParams.begin(), m_IntParams.end());
}


//------------------------------------------------------------------------
// find parameters by name and type
//------------------------------------------------------------------------
effShaderParam* effShaderParams::FindParam(const std::string& i_Name) const
{
	effShaderParam* p = NULL;
	p = FindFloatParam(i_Name);
	if (p)
		return p;
	p = FindTextureParam(i_Name);
	if (p)
		return p;
	p = FindColorParam(i_Name);
	if (p)
		return p;
	p = FindBoolParam(i_Name);
	if (p)
		return p;
	p = FindIntParam(i_Name);
	if (p)
		return p;

	return NULL;
}

//------------------------------------------------------------------------
// find parameters by name and type
//------------------------------------------------------------------------
effParamTexture* effShaderParams::FindTextureParam(const std::string& i_Name) const
{
	int n = m_TextureParams.size();
	effParamTexture* curParam = NULL;
	for (int i = 0; i < n; i++)
	{
		curParam = m_TextureParams[i];
		if (curParam->GetName() == i_Name)
			return curParam;
	}
	return NULL;
}
//------------------------------------------------------------------------
// find parameters by name and type
//------------------------------------------------------------------------
effParamFloat* effShaderParams::FindFloatParam(const std::string& i_Name) const
{
	int n = m_FloatParams.size();
	effParamFloat* curParam = NULL;
	for (int i = 0; i < n; i++)
	{
		curParam = m_FloatParams[i];
		if (curParam->GetName() == i_Name)
			return curParam;
	}
	return NULL;
}
//------------------------------------------------------------------------
// find parameters by name and type
//------------------------------------------------------------------------
effParamColor* effShaderParams::FindColorParam(const std::string& i_Name) const
{
	int n = m_ColorParams.size();
	effParamColor* curParam = NULL;
	for (int i = 0; i < n; i++)
	{
		curParam = m_ColorParams[i];
		if (curParam->GetName() == i_Name)
			return curParam;
	}
	return NULL;
}
//------------------------------------------------------------------------
// find parameters by name and type
//------------------------------------------------------------------------
effParamBool* effShaderParams::FindBoolParam(const std::string& i_Name) const
{
	int n = m_BoolParams.size();
	effParamBool* curParam = NULL;
	for (int i = 0; i < n; i++)
	{
		curParam = m_BoolParams[i];
		if (curParam->GetName() == i_Name)
			return curParam;
	}
	return NULL;
}
//------------------------------------------------------------------------
// find parameters by name and type
//------------------------------------------------------------------------
effParamInt* effShaderParams::FindIntParam(const std::string& i_Name) const
{
	int n = m_IntParams.size();
	effParamInt* curParam = NULL;
	for (int i = 0; i < n; i++)
	{
		curParam = m_IntParams[i];
		if (curParam->GetName() == i_Name)
			return curParam;
	}
	return NULL;
}

//------------------------------------------------------------------------
// load all textures in a loop
//------------------------------------------------------------------------
void effShaderParams::ReloadTextures(const fsResourceFinder& i_Finder) const
{
	for (int i = 0; i < m_TextureParams.size(); i++)
	{
		effParamTexture* param = m_TextureParams[i];
		param->Load(i_Finder);
	}
}

//------------------------------------------------------------------------
// set all textures to null
//------------------------------------------------------------------------
void effShaderParams::RemoveTextures() const
{
	// called after textures have been released externally.
	// can this be changed to release the textures inside here? look for usage by callers.

	for (int i = 0; i < m_TextureParams.size(); i++)
	{
		effParamTexture* param = m_TextureParams[i];
		param->ClearTexture();
		param->ClearRampTexture();
		param->ClearRampProperty();
	}
}

//------------------------------------------------------------------------
// get a list of all non null textures
//------------------------------------------------------------------------
void effShaderParams::GetTextures(std::vector<matTexture*>& o_Textures)
{
	for (int i = 0; i < m_TextureParams.size(); i++)
	{
		effParamTexture* param = m_TextureParams[i];
		matTexture* t = param->GetTexture();
		if (t != NULL)
			o_Textures.push_back(t);

		//add ramp texture to list if it's not null and not equal to the main texture
		matTexture* r = param->GetRampTexture();
		if(r != NULL && r != t)
			o_Textures.push_back(r);
	}
}

//------------------------------------------------------------------------
// debug output
//------------------------------------------------------------------------
void effShaderParams::Dump()
{
	DBG_LOG("BEGIN shader param dump: " << m_ShaderName);
	for (int i = 0; i < m_FloatParams.size(); i++)
	{
		DBG_LOG("  " << m_FloatParams[i]->GetName().c_str() << " : " << m_FloatParams[i]->Property().GetValue());
	}
	for (int i = 0; i < m_ColorParams.size(); i++)
	{
		DBG_LOG("  " << m_ColorParams[i]->GetName().c_str() << " : " << m_ColorParams[i]->Property().GetValue().GetRed() << " " << m_ColorParams[i]->Property().GetValue().GetGreen() << " " << m_ColorParams[i]->Property().GetValue().GetBlue() << " " << m_ColorParams[i]->Property().GetValue().GetAlpha() ); 
	}
	for (int i = 0; i < m_TextureParams.size(); i++)
	{
		DBG_LOG("  " << m_TextureParams[i]->GetName().c_str() << " : " << m_TextureParams[i]->Property().GetString().c_str());
	}
	for (int i = 0; i < m_BoolParams.size(); i++)
	{
		DBG_LOG("  " << m_BoolParams[i]->GetName().c_str() << " : " << (m_BoolParams[i]->Property().GetValue()?"true":"false"));
	}
	for (int i = 0; i < m_IntParams.size(); i++)
	{
		DBG_LOG("  " << m_IntParams[i]->GetName().c_str() << " : " << m_IntParams[i]->Property().GetValue());
	}
	DBG_LOG("END shader param dump");
}

//------------------------------------------------------------------------
// is this considered a transparent material (alpha < 1 for any pixel)
//------------------------------------------------------------------------
bool effShaderParams::HasTransparency()
{
	if( m_pTransparency )
	{
		if (m_pTransparency->GetProperty().GetValue() < 1)
		{
			return true;
		}
	}

	if( m_pTransparencyMap )
	{
		if (m_pTransparencyMap->GetTexture() != NULL)
		{
			//ignore whether the texture has alpha, since only r channel is used
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------
// set default values by matching name and type
//------------------------------------------------------------------------
void effShaderParams::SetMatchingParams(const effShaderParams* i_pOtherParams)
{
	// sanity check. can this ever be called on itself accidentally?
	if (i_pOtherParams == this)
		return;

	int n = 0;

	// all the variables
	n = m_TextureParams.size();
	for (int i = 0; i < n; i++)
	{
		effParamTexture* curParam = m_TextureParams[i];
		effParamTexture* other = i_pOtherParams->FindTextureParam(curParam->GetName());
		if (other)
			curParam->Property() = other->GetProperty();
	}

	n = m_ColorParams.size();
	for (int i = 0; i < n; i++)
	{
		effParamColor* curParam = m_ColorParams[i];
		effParamColor* other = i_pOtherParams->FindColorParam(curParam->GetName());
		if (other)
			curParam->Property().SetValue(other->GetProperty().GetValue());
	}

	n = m_FloatParams.size();
	for (int i = 0; i < n; i++)
	{
		effParamFloat* curParam = m_FloatParams[i];
		effParamFloat* other = i_pOtherParams->FindFloatParam(curParam->GetName());
		if (other)
			curParam->Property().SetValue(other->GetProperty().GetValue());
	}

	n = m_BoolParams.size();
	for (int i = 0; i < n; i++)
	{
		effParamBool* curParam = m_BoolParams[i];
		effParamBool* other = i_pOtherParams->FindBoolParam(curParam->GetName());
		if (other)
			curParam->Property().SetValue(other->GetProperty().GetValue());
	}

	n = m_IntParams.size();
	for (int i = 0; i < n; i++)
	{
		effParamInt* curParam = m_IntParams[i];
		effParamInt* other = i_pOtherParams->FindIntParam(curParam->GetName());
		if (other)
			curParam->Property().SetValue(other->GetProperty().GetValue());
	}

	// "special" shader params override any name matches...
	if (m_pDiffuseMap && i_pOtherParams->m_pDiffuseMap)
		m_pDiffuseMap->Property().SetValue(i_pOtherParams->m_pDiffuseMap->GetProperty().GetFullValue());
	if (m_pTransparencyMap && i_pOtherParams->m_pTransparencyMap)
		m_pTransparencyMap->Property().SetValue(i_pOtherParams->m_pTransparencyMap->GetProperty().GetFullValue());
	if (m_pDiffuseColor && i_pOtherParams->m_pDiffuseColor)
		m_pDiffuseColor->Property().SetValue(i_pOtherParams->m_pDiffuseColor->GetProperty().GetValue());
	if (m_pTransparency && i_pOtherParams->m_pTransparency)
		m_pTransparency->Property().SetValue(i_pOtherParams->m_pTransparency->GetProperty().GetValue());
}

//------------------------------------------------------------------------
// shader version
//------------------------------------------------------------------------
int effShaderParams::GetVersion() const
{
	return m_Version;
}
void effShaderParams::SetVersion(int i_Version)
{
	m_Version = i_Version;
}

void effShaderParams::Clear()
{
	//m_TextureParams.clear();
	//m_ColorParams.clear();
	//m_FloatParams.clear();
	//m_BoolParams.clear();
	//m_IntParams.clear();

	//m_pDiffuseMap = NULL;
	//m_pTransparencyMap = NULL;
	//m_pDiffuseColor = NULL;
	//m_pTransparency = NULL;

	m_pShader = NULL;

	m_pShaderBindings = NULL;
	m_pPrtyUI = NULL;
	//prtyFilePath* m_pPathObject;
	
	//m_ShaderName.Clear();
	//m_TextureParamUIs.clear();
	//m_pPathObject->SetString("");
}
