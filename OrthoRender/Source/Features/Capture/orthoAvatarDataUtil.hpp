/*****************************************************************************
**	orthoAvatarDataUtil.hpp
**
**		Interface to Avatar Data
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef ORTHO_AVATARDATAUTIL_HPP
#error orthoAvatarDataUtil.hpp multiply included
#endif
#define ORTHO_AVATARDATAUTIL_HPP

#ifndef ORTHO_AVATARDATA_HPP
#include "Features/Capture/orthoAvatarData.hpp"
#endif
#ifndef ORTHO_AVATARDATAINTEREST_HPP
#include "Features/Capture/orthoAvatarDataInterest.hpp"
#endif
#ifndef LIGHTWAIT_RESPONSE_HPP
#include "MainApp/LightWaitResponse.hpp"
#endif


//============================================================================
//============================================================================


//============================================================================
//============================================================================
namespace orthoAvatarDataUtil
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Initialize();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DeInitialize();

	//------------------------------------------------------------------------
	//	this function takes only the filename.  The path is assumed
	//	to be ~/Data/Objects
	//------------------------------------------------------------------------
	bool ReadData(const std::string& i_ConfigFileName);

	//------------------------------------------------------------------------
	//	this function takes only the filename with the full path.
	//------------------------------------------------------------------------
	bool ReadData(const fsLocator& i_ConfigFile, orthoAvatarListData& o_Data);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	orthoAvatarListData& Data();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void AddInterest( orthoAvatarDataInterest* i_pInterest );

	//------------------------------------------------------------------------
	//	notify interests
	//------------------------------------------------------------------------
	void AvatarChanged();

	//------------------------------------------------------------------------
	//	notify interests part visibility has changed
	//------------------------------------------------------------------------
	void PartChanged(const std::string& i_JointName, bool i_bVisible );

	//------------------------------------------------------------------------
	//	notify interests part material has changed
	//------------------------------------------------------------------------
	void MaterialChanged(const std::string& i_PartName, const maFloatRGBA& i_Color );

	//------------------------------------------------------------------------
	//	notify interests a part gets attached/removed
	//------------------------------------------------------------------------
	void AttachPart( const std::string& i_Name, const std::string& i_PartName, const std::string& i_AttachFile, const bool i_bAttach = true);

	//------------------------------------------------------------------------
	//	change the texture of a material
	//------------------------------------------------------------------------
	void ChangeTexture(const std::string& i_MaterialName, const std::string& i_TextureLayer, const std::string& i_TextureName);

	//------------------------------------------------------------------------
	//	notify interests to change the material
	//------------------------------------------------------------------------
	void ChangeMaterial(const std::string& i_MaterialName, const std::string& i_TextureName);

	//------------------------------------------------------------------------
	//	output the struct values at anytime
	//------------------------------------------------------------------------
	void Debug();

	//------------------------------------------------------------------------
	//	add the struct values to the render request response
	//------------------------------------------------------------------------
	void AppendXMLDescription(LightWaitResponse *response) ;

	//------------------------------------------------------------------------
	//	Adds an animation sequence
	//------------------------------------------------------------------------
	void AddAnimation(const std::string& i_AnimFile, const int i_Divisions, const std::vector<std::string>& i_Cameras );

	//------------------------------------------------------------------------
	//	Deletes all drivers
	//------------------------------------------------------------------------
	void DeleteSceneDrivers();

	//------------------------------------------------------------------------
	//	Scales all the models
	//------------------------------------------------------------------------
	void ScaleCharacter( const maVector3d& i_Scale );

	//------------------------------------------------------------------------
	//	Rotate all the models
	//------------------------------------------------------------------------
	void RotateCharacter( float angle );

	//------------------------------------------------------------------------
	//	Load a new character
	//------------------------------------------------------------------------
	void LoadCharacter( const std::string& i_Name, const std::string& i_FileName, const bool i_bIsBaseModel );

	//------------------------------------------------------------------------
	//	Load new characters, don't load already loaded ones, 
	//	and delete ones not in this list
	//------------------------------------------------------------------------
	void LoadCharacters( const std::vector<std::string>& i_Names, const std::vector<std::string>& i_FileNames, const std::vector<bool>& i_bIsBaseModels );

	//------------------------------------------------------------------------
	//	delete a character
	//------------------------------------------------------------------------
	void DeleteCharacter( const std::string& i_FileName );

	//------------------------------------------------------------------------
	//	replace a character model with another
	//------------------------------------------------------------------------
	void ReplaceCharacter( const std::string& i_CurrentFileName, const std::string& i_NewFileName );

	//------------------------------------------------------------------------
	//	show or hide a character
	//------------------------------------------------------------------------
	void ShowCharacter( const std::string& i_FileName, bool i_bShow );

	//------------------------------------------------------------------------
	//	add an expression to a character
	//------------------------------------------------------------------------
	void AddExpression( const std::string& i_ObjectName, const std::string& i_ExpressionName, const itString& i_ExpressionFileName, const float i_ExpressionValue );

	//------------------------------------------------------------------------
	//	Save a character as XML - the filename is the model file in the scene
	//------------------------------------------------------------------------
	void SaveXMLCharacter( const std::string& i_FileName );

	//------------------------------------------------------------------------
	//	Changes camera properties
	//------------------------------------------------------------------------
	void CameraChange( const std::string& i_Camera, const cmraCameraChangeData& i_Data );

}	// end of namespace
