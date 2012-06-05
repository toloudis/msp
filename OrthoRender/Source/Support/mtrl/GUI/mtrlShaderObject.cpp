/********************************************************************************************\
**  mtrlShaderObject.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"

#include "Support/mnm/mnmThinkMgr.hpp"
#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnChannelColor.hpp"
#include "Support/tmln/tmlnChannelFileName.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
//#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/fsResourceFinderDir.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/prty/prtyBoolean.hpp"
#include "Core/prty/prtyColor.hpp"
#include "Core/prty/prtyFileName.hpp"
#include "Core/prty/prtyFloat.hpp"
#include "Core/prty/prtyVector3d.hpp"
#include "Graphics/Ent/entModelTemplate.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mtr/mtrTextureFinder.hpp"
#include "Tool/gui/guiMessageBox.hpp"

#include "Graphics/eff/effShaderParams.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlShaderObject::mtrlShaderObject()
:	m_bChannelsHaveBeenAdded(false)
{
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlShaderObject::mtrlShaderObject(const fsLocator& i_TextureDirectory)
:	m_TextureDirectory(i_TextureDirectory),
	m_bChannelsHaveBeenAdded(false)
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
mtrlShaderObject::~mtrlShaderObject()
{
	// We can only delete the channels if they are not attached to the script object.
	//DBG_ASSERT0(!m_bChannelsHaveBeenAdded, "Channels cannot be deleted, still attached.");
	if (!m_bChannelsHaveBeenAdded)
		envSTLHelpers::DeleteContainer(m_Channels);
}

//------------------------------------------------------------------------
// Set model template in order to alter texture dependencies.
// Just pointed to, not owned.
//------------------------------------------------------------------------
void mtrlShaderObject::SetModelTemplate(entModelTemplate *i_pModelTemplate)
{
	m_pModelTemplate = i_pModelTemplate;
}

//------------------------------------------------------------------------
// Add channels for this material to the given script object
//------------------------------------------------------------------------
void mtrlShaderObject::AddChannels(tmlnScriptObject *i_pScriptObject)
{
	DBG_ASSERT0(!m_bChannelsHaveBeenAdded, "Channels were already added.");

	std::vector<tmlnChannel*>::iterator it;
	for (it = m_Channels.begin(); it != m_Channels.end(); ++it)
	{
		i_pScriptObject->AddChannel( *it );
	}
	
	m_bChannelsHaveBeenAdded = true;
}

//------------------------------------------------------------------------
// Remove channels for this material to the given script object
//------------------------------------------------------------------------
void mtrlShaderObject::RemoveChannels(tmlnScriptObject *i_pScriptObject)
{
	DBG_ASSERT0(m_bChannelsHaveBeenAdded, "Removing channels that were not added.");
	
	std::vector<tmlnChannel*>::iterator it;
	for (it = m_Channels.begin(); it != m_Channels.end(); ++it)
	{
		i_pScriptObject->RemoveChannel( *it );
	}
	
	m_bChannelsHaveBeenAdded = false;
}

//------------------------------------------------------------------------
// Direct access to channels
//------------------------------------------------------------------------
std::vector<tmlnChannel*>& mtrlShaderObject::Channels()
{
	return m_Channels;
}
const std::vector<tmlnChannel*>& mtrlShaderObject::GetChannels() const
{
	return m_Channels;
}

//--------------------------------------------------------------------
// Returns true if there are some drivers on the channels
// for the material.
//--------------------------------------------------------------------
bool mtrlShaderObject::HasMaterialAnimation() const
{
	std::vector<tmlnChannel*>::const_iterator it;
	for (it = m_Channels.begin(); it != m_Channels.end(); ++it)
	{
		if ((*it)->GetNumDrivers() > 0)
			return true;
	}
	return false;
}

//------------------------------------------------------------------------
// Access to texture directory
//------------------------------------------------------------------------
const fsLocator& mtrlShaderObject::GetTextureDirectory() const
{
	return m_TextureDirectory;
}

//------------------------------------------------------------------------
// Set a new directory for the material in order to find the textures.
// Called when the material has been exported to the material library.
//------------------------------------------------------------------------
//virtual 
void mtrlShaderObject::UpdateTextureDirectory(const fsLocator &i_TextureDir)
{
	m_TextureDirectory = i_TextureDir;
}

//--------------------------------------------------------------------
// Get list of used textures in order to support copying them
// to the material library when exporting.
//--------------------------------------------------------------------
//virtual 
void mtrlShaderObject::GetTextureList(std::vector<fsLocator>& o_TextureList)
{
	// default implementation does nothing
}


//------------------------------------------------------------------------
// Derived classes should call this function when creating channels
//	in order to have the channels be managed and submitted to the
//	script object correctly.
//------------------------------------------------------------------------
void mtrlShaderObject::AddChannel(tmlnChannel *i_pChannel)
{
	DBG_ASSERT0(!m_bChannelsHaveBeenAdded, "Adding channel too late, cannot be added to script object");
	m_Channels.push_back( i_pChannel );
}

//------------------------------------------------------------------------
// Convenience functions for creating a channels for properties
//------------------------------------------------------------------------
void mtrlShaderObject::AddColorChannel(const std::string &i_MaterialName, prtyColor &i_Property)
{
	std::string channel_name = i_MaterialName + "." + i_Property.GetPropertyName();
	tmlnChannelColor *pChannel = new tmlnChannelColorProperty(channel_name.c_str(), i_Property);
	pChannel->SetOriginalColor( i_Property.GetValue() );
	this->AddChannel(pChannel);
}
void mtrlShaderObject::AddFloatChannel(const std::string &i_MaterialName, prtyFloat &i_Property)
{
	std::string channel_name = i_MaterialName + "." + i_Property.GetPropertyName();
	tmlnChannelFloat *pChannel = new tmlnChannelFloatProperty(channel_name.c_str(), i_Property);
	pChannel->SetOriginalValue( i_Property.GetValue() );
	this->AddChannel(pChannel);
}
//void mtrlShaderObject::AddVectorChannel(const std::string &i_MaterialName, prtyVector3d &i_Property)
//{
//	std::string channel_name = i_MaterialName + "." + i_Property.GetPropertyName();
//	tmlnChannelPosition *pChannel = new tmlnChannelVector3dProperty(channel_name.c_str(), i_Property);
//	pChannel->SetOriginalPosition( i_Property.GetValue() );
//	this->AddChannel(pChannel);
//}
void mtrlShaderObject::AddBooleanChannel(const std::string &i_MaterialName, prtyBoolean &i_Property)
{
	std::string channel_name = i_MaterialName + "." + i_Property.GetPropertyName();
	tmlnChannelBoolean *pChannel = new tmlnChannelBooleanProperty(channel_name.c_str(), i_Property);
	pChannel->SetOriginalState( i_Property.GetValue() );
	this->AddChannel(pChannel);
}
void mtrlShaderObject::AddTextureChannel(const std::string &i_MaterialName, prtyFileName &i_Property)
{
	std::string channel_name = i_MaterialName + "." + i_Property.GetPropertyName();
	tmlnChannelFileName *pChannel = new tmlnChannelFileNameProperty(channel_name.c_str(), i_Property);
	pChannel->SetOriginalValue( i_Property.GetValue() );
	pChannel->SetDirectory(this->m_TextureDirectory);
	this->AddChannel(pChannel);
}

//------------------------------------------------------------------------
// Convenience functions for locating a texture
//------------------------------------------------------------------------
fsLocator mtrlShaderObject::LocateTexture(const itString& i_Filename)
{
	// Construct a fsResourceFinder to use to locate the textures in the object directory
	const bool bStrict = true; // has to return false if not found
	fsResourceFinderDir texture_finder(m_TextureDirectory, bStrict);

	// Use resource finder that also searches through material library
	mtrTextureFinder lib_texture_finder(texture_finder);

	fsLocator found_loc;
	if (lib_texture_finder.FindResource(i_Filename, found_loc))
	{
		return found_loc;
	}

	// Fallback on a guess where it should be	
	fsLocator tex_loc = this->m_TextureDirectory;
	tex_loc.Push(i_Filename);
	return tex_loc;
}

//------------------------------------------------------------------------
// Load a texture.  Do not change out params if load fails.
//------------------------------------------------------------------------
bool mtrlShaderObject::LoadTexture(const itString& i_NewName, 
									 std::string& o_NameSlot, matTexture*& io_TextureSlot,
									 std::string& o_TemplateNameSlot, matTexture*& io_TemplateTextureSlot,
									 bool i_bDirty)
{
	matTexture* originalTexture = io_TextureSlot;

	bool success = false;
	if (i_NewName.GetLength() == 0)
	{
		o_NameSlot = "";
		io_TextureSlot = NULL;
		success = true;
	}
	else
	{
		try
		{
			// Construct a fsResourceFinder to use to locate the textures in the object directory
			const bool bStrict = true; // has to return false if not found
			fsResourceFinderDir texture_finder(m_TextureDirectory, bStrict);

			// Use resource finder that also searches through material library
			mtrTextureFinder lib_texture_finder(texture_finder);

			// load new texture name.
			// if exception thrown, delete texture. use auto_ptr for this.
			std::auto_ptr<matTexture> tex(matTextureMgr::LoadTexture(lib_texture_finder, i_NewName));
			if (tex.get() != NULL)
			{
				// load succeeded, change data.
				o_NameSlot = itStringUtil::GetStdString(i_NewName);
				io_TextureSlot = tex.release();
				success = true;
			}
		}
		catch( const fsFileDoesntExistX& i_Ex )
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

			std::string msg = "Cannot read file " + filename;
			DBG_ERROR1("%s", msg.c_str() );
			guiMessageBox::Show(msg.c_str(), "File does not exist", guiMessageBox::e_OKOnly);
		}
		catch( const fsUnknownX& i_Ex )
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

			std::string msg = "Cannot read file " + filename;
			DBG_ERROR1("%s", msg.c_str() );
			guiMessageBox::Show(msg.c_str(), "File can not be loaded", guiMessageBox::e_OKOnly);
		}
	}

	if (success)
	{
		if (i_bDirty)
		{
			o_TemplateNameSlot = o_NameSlot;
			io_TemplateTextureSlot = io_TextureSlot;
		}
		// need to remove texture from scriptobject's modeltemplate, and add new one to same.
		//mtrlOperations::ReplaceTexture(originalTexture, io_TextureSlot);
		this->ReplaceTextureInTemplate(originalTexture, io_TextureSlot); // handle this here now, not in mtrlOperations
		// i waited until now to release the old texture.
		matTextureMgr::ReleaseTexture(originalTexture);
	}

	return success;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlShaderObject::UpdateTexture(prtyFileName& i_pProperty, bool i_bDirty,
									 std::string& io_NameSlot, matTexture*& io_TextureSlot,
									 std::string& io_TemplateNameSlot, matTexture*& io_TemplateTextureSlot
									 )
{
	// Test to see if we are just restoring from a failed texture change.
	if (i_pProperty.GetValue() == itString(io_NameSlot.c_str()))
		return;

	bool changed = LoadTexture(i_pProperty.GetValue(), 
		io_NameSlot, io_TextureSlot,
		io_TemplateNameSlot, io_TemplateTextureSlot,
		i_bDirty);

	if (!changed)
	{
		// This line isn't changing the shader value back in the
		// user interface because it thinks the callback is from its local change.
		i_pProperty.SetValue(itString(io_NameSlot.c_str()));
		// So, ask for a new refresh from the ObjectDialog
		mnmThinkMgr::CallFunctionDelayed(&cmmObjectDialogUtil::UpdateDialog);
	}
}

//--------------------------------------------------------------------
// Change the texture dependency in the model template,
// replacing old texture with new texture.
//--------------------------------------------------------------------
void mtrlShaderObject::ReplaceTextureInTemplate(matTexture* i_pOldTexture, 
												matTexture* i_pNewTexture)
{
	if (m_pModelTemplate)
	{
		if (i_pOldTexture != NULL)
			envSTLHelpers::RemoveOneValue(m_pModelTemplate->Textures(), i_pOldTexture);
		if (i_pNewTexture != NULL)
			m_pModelTemplate->Textures().push_back(i_pNewTexture);
	}
}

//------------------------------------------------------------------------
// Load a texture.  Do not change out params if load fails.
//------------------------------------------------------------------------
bool mtrlShaderObject::LoadTexture(effParamTexture* i_pParam, bool i_bDirty)
{
	matTexture* originalTexture = i_pParam->GetTexture();

	bool success = false;
	try
	{
		// Construct a fsResourceFinder to use to locate the textures in the object directory
		const bool bStrict = true; // has to return false if not found
		fsResourceFinderDir texture_finder(m_TextureDirectory, bStrict);

		// Use resource finder that also searches through material library
		mtrTextureFinder lib_texture_finder(texture_finder);

		success = i_pParam->Load(lib_texture_finder);
	}
	catch( const fsFileDoesntExistX& i_Ex )
	{
		success = false;

		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

		std::string msg = "Cannot read file " + filename;
		DBG_ERROR1("%s", msg.c_str() );
		guiMessageBox::Show(msg.c_str(), "File does not exist", guiMessageBox::e_OKOnly);
	}
	catch( const fsUnknownX& i_Ex )
	{
		success = false;

		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

		std::string msg = "Cannot read file " + filename;
		DBG_ERROR1("%s", msg.c_str() );
		guiMessageBox::Show(msg.c_str(), "File can not be loaded", guiMessageBox::e_OKOnly);
	}

	if (success)
	{
		// need to remove texture from scriptobject's modeltemplate, and add new one to same.
		//mtrlOperations::ReplaceTexture(originalTexture, io_TextureSlot);
		this->ReplaceTextureInTemplate(originalTexture, i_pParam->GetTexture()); // handle this here now, not in mtrlOperations
		// i waited until now to release the old texture.
		matTextureMgr::ReleaseTexture(originalTexture);
	}

	return success;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlShaderObject::UpdateTexture(effParamTexture* i_pParam, bool i_bDirty)
{
	// Test to see if we are just restoring from a failed texture change.
	if (i_pParam->IsNameCurrent())
		return;

	bool changed = LoadTexture(i_pParam, i_bDirty);

	if (!changed)
	{
		// This line isn't changing the shader value back in the
		// user interface because it thinks the callback is from its local change.
		i_pParam->Property().SetValue(i_pParam->GetRestoreTextureName());
		// So, ask for a new refresh from the ObjectDialog
		mnmThinkMgr::CallFunctionDelayed(&cmmObjectDialogUtil::UpdateDialog);
	}
}
