/*****************************************************************************
**  mtrlShaderObject.hpp
**
**      mtrlShaderObject is the base class for shaders with properties
**	and animation channels.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_SHADEROBJECT_HPP
#error mtrlShaderObject.hpp multiply included
#endif
#define MTRL_SHADEROBJECT_HPP

#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

class effParamTexture;
class entModelTemplate;
class matTexture;
class prtyBoolean;
class prtyColor;
class prtyFileName;
class prtyFloat;
class prtyVector3d;
class tmlnChannel;
class tmlnScriptObject;

//============================================================================
//============================================================================
class mtrlShaderObject : public prtyObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mtrlShaderObject();
	mtrlShaderObject(const fsLocator& i_TextureDirectory);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~mtrlShaderObject() = 0;

	//------------------------------------------------------------------------
	// Set model template in order to alter texture dependencies.
	// Just pointed to, not owned.
	//------------------------------------------------------------------------
	void SetModelTemplate(entModelTemplate *i_pModelTemplate);

	//------------------------------------------------------------------------
	// Add channels for this material to the given script object
	//------------------------------------------------------------------------
	virtual void AddChannels(tmlnScriptObject *i_pScriptObject);

	//------------------------------------------------------------------------
	// Remove channels for this material to the given script object
	//------------------------------------------------------------------------
	virtual void RemoveChannels(tmlnScriptObject *i_pScriptObject);

	//------------------------------------------------------------------------
	// Direct access to channels
	//------------------------------------------------------------------------
	std::vector<tmlnChannel*>& Channels();
	const std::vector<tmlnChannel*>& GetChannels() const;

	//------------------------------------------------------------------------
	// Returns true if there are some drivers on the channels
	// for the material.
	//------------------------------------------------------------------------
	bool HasMaterialAnimation() const;

	//------------------------------------------------------------------------
	// Access to texture directory
	//------------------------------------------------------------------------
	const fsLocator& GetTextureDirectory() const;

	//------------------------------------------------------------------------
	// Set a new directory for the material in order to find the textures.
	// Called when the material has been exported to the material library.
	//------------------------------------------------------------------------
	virtual void UpdateTextureDirectory(const fsLocator &i_TextureDir);

	//--------------------------------------------------------------------
	// Get list of used textures in order to support copying them
	// to the material library when exporting.
	//--------------------------------------------------------------------
	virtual void GetTextureList(std::vector<fsLocator>& o_TextureList);

protected:
	//------------------------------------------------------------------------
	// Derived classes should call this function when creating channels
	//	in order to have the channels be managed and submitted to the
	//	script object correctly.
	//------------------------------------------------------------------------
	void AddChannel(tmlnChannel *i_pChannel);

	//------------------------------------------------------------------------
	// Convenience functions for creating a channels for properties
	//------------------------------------------------------------------------
	void AddColorChannel(const std::string &i_MaterialName, prtyColor &i_Property);
	void AddFloatChannel(const std::string &i_MaterialName, prtyFloat &i_Property);
	//void AddVectorChannel(const std::string &i_MaterialName, prtyVector3d &i_Property);
	void AddBooleanChannel(const std::string &i_MaterialName, prtyBoolean &i_Property);
	void AddTextureChannel(const std::string &i_MaterialName, prtyFileName &i_Property);

	//------------------------------------------------------------------------
	// Convenience functions for locating a texture
	//------------------------------------------------------------------------
	fsLocator LocateTexture(const itString& i_Filename);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void UpdateTexture(prtyFileName& i_pProperty, bool i_bDirty,
		std::string& io_NameSlot, matTexture*& io_TextureSlot,
		std::string& io_TemplateNameSlot, matTexture*& io_TemplateTextureSlot );
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void UpdateTexture(effParamTexture* i_pParam, bool i_bDirty);


	//--------------------------------------------------------------------
	// Change the texture dependency in the model template,
	// replacing old texture with new texture.
	//--------------------------------------------------------------------
	void ReplaceTextureInTemplate(matTexture* i_pOldTexture, 
								  matTexture* i_pNewTexture);

private:
	fsLocator m_TextureDirectory;
	std::vector<tmlnChannel*> m_Channels;
	bool m_bChannelsHaveBeenAdded;
	entModelTemplate *m_pModelTemplate;

	//------------------------------------------------------------------------
	// Load a texture. Release old one. Do not change out params if load fails.
	// Return true if successfully updated.
	//------------------------------------------------------------------------
	bool LoadTexture(const itString& i_NewName, 
		std::string& o_NameSlot, matTexture*& io_TextureSlot,
		std::string& o_TemplateNameSlot, matTexture*& io_TemplateTextureSlot,
		bool i_bDirty);
	//------------------------------------------------------------------------
	// Load a texture.  Do not change out params if load fails.
	//------------------------------------------------------------------------
	bool LoadTexture(effParamTexture* i_pParam, bool i_bDirty);
};
