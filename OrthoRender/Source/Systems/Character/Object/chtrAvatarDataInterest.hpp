/*****************************************************************************
**  chtrAvatarDataInterest.hpp
**
**      the interest for when Avatar data changes
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHTR_AVATARDATAINTEREST_HPP
#error chtrAvatarDataInterest.hpp multiply included
#endif
#define CHTR_AVATARDATAINTEREST_HPP

#ifndef ORTHO_AVATARDATAINTEREST_HPP
#include "Features/Capture/orthoAvatarDataInterest.hpp"
#endif

#include <map>


//----------------------------------------------------------------------------
//	forward references
//----------------------------------------------------------------------------
class chtrScriptObject;
class itString;
class fsLocator;
class mdlMaterialInfo;
class tmlnDriver;


//============================================================================
//============================================================================
class chtrAvatarDataInterest : public orthoAvatarDataInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		chtrAvatarDataInterest();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~chtrAvatarDataInterest();

		//--------------------------------------------------------------------
		//	LoadCharacter
		//--------------------------------------------------------------------
		virtual void LoadCharacter( const std::string& i_Name, const std::string& i_FileName, const bool i_bIsBaseModel );

		//------------------------------------------------------------------------
		//	Load new characters, don't load already loaded ones, 
		//	and delete ones not in this list
		//------------------------------------------------------------------------
		virtual void LoadCharacters( const std::vector<std::string>& i_Names, const std::vector<std::string>& i_FileNames, const std::vector<bool>& i_bIsBaseModels );

		//--------------------------------------------------------------------
		//	DeleteCharacter
		//--------------------------------------------------------------------
		virtual void DeleteCharacter( const std::string& i_FileName );

		//------------------------------------------------------------------------
		//	replace a character model with another
		//------------------------------------------------------------------------
		virtual void ReplaceCharacter( const std::string& i_CurrentFileName, const std::string& i_NewFileName );

		//------------------------------------------------------------------------
		//	show or hide a character
		//------------------------------------------------------------------------
		virtual void ShowCharacter( const std::string& i_FileName, bool i_bShow );

		//------------------------------------------------------------------------
		//	Save a character as XML - the filename is the model file in the scene
		//------------------------------------------------------------------------
		virtual void SaveXMLCharacter( const std::string& i_FileName );

		//--------------------------------------------------------------------
		//	AvatarChanged
		//--------------------------------------------------------------------
		virtual void AvatarChanged();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void AttachPart( const std::string& i_Name, const std::string& i_PartName, const std::string& i_AttachFile, const bool i_bAttach) ;

		//--------------------------------------------------------------------
		//	Add Animation
		//--------------------------------------------------------------------
		virtual void AddAnimation(const std::string& i_AnimFile, const int i_Divisions, const std::vector<std::string>& i_Cameras );

		//--------------------------------------------------------------------
		//	Add Expression
		//--------------------------------------------------------------------
		virtual void AddExpression(const std::string& i_ObjectName, const std::string& i_ExpressionName, const itString& i_ExpressionFileName, const float i_ExpressionValue);

		//--------------------------------------------------------------------
		//	Delete All Drivers (animation and orientation)
		//--------------------------------------------------------------------
		virtual void DeleteSceneDrivers();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void ChangeTexture(const std::string& i_MaterialName, const std::string& i_TextureLayer, const std::string& i_TextureName);

		//--------------------------------------------------------------------
		//	ChangeMaterial
		//--------------------------------------------------------------------
		virtual void ChangeMaterial(const std::string i_PartName, const std::string& i_MaterialFile);

		//--------------------------------------------------------------------
		//	PartChanged
		//--------------------------------------------------------------------
		void PartChanged(const std::string i_JointName, bool i_bVisible );

		//--------------------------------------------------------------------
		//	MaterialChanged - either change the diffuse color or a whole
		//	new MTL file.
		//--------------------------------------------------------------------
		void MaterialChanged(const std::string i_PartName, const maFloatRGBA& i_Color);

		//--------------------------------------------------------------------
		//	ScaleCharacter
		//--------------------------------------------------------------------
		void ScaleCharacter( const maVector3d& i_Scale );

		//--------------------------------------------------------------------
		//	RotateCharacter
		//--------------------------------------------------------------------
		void RotateCharacter( float angle );

private:
		//--------------------------------------------------------------------
		//	Creates a new driver on the current selected object
		//--------------------------------------------------------------------
		tmlnDriver*  CreateDriver( chtrScriptObject* pAvatar, const std::string& i_driverName );

		//--------------------------------------------------------------------
		//	Add an object to lightsets
		//--------------------------------------------------------------------
		void AddObjectToLightSets(int i_ObjectIndex);

private:
		std::map< std::string, mdlMaterialInfo > m_DefaultMaterials;
		std::string m_BaseModel;
};
