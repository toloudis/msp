/*****************************************************************************
**  orthoAvatarDataInterest.hpp
**
**      the interest for when avatar data changes
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef ORTHO_AVATARDATAINTEREST_HPP
#error orthoAvatarDataInterest.hpp multiply included
#endif
#define ORTHO_AVATARDATAINTEREST_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif

#ifndef MA_POINT3D_HPP
#include "Core/Ma/maPoint3d.hpp"
#endif

#ifndef CMRA_DATA_HPP
#include "Systems/Cameras/Data/cmraData.hpp"
#endif


#include <string>
#include <vector>


//============================================================================
//	forward references
//============================================================================
class fsLocator;
class itString;
class maVector3d;


//============================================================================
//============================================================================
class OrthoAvatarTemporalSpace
{
public:
	static float animStartTime;		//start of driven animation
	static float animEndTime;		//end of driven animation
};

//============================================================================
//============================================================================
class orthoAvatarDataInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		orthoAvatarDataInterest();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~orthoAvatarDataInterest();

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
		//	PartChanged
		//--------------------------------------------------------------------
		virtual void PartChanged(const std::string i_JointName, bool i_bVisible);

		//--------------------------------------------------------------------
		//	MaterialChanged
		//--------------------------------------------------------------------
		virtual void MaterialChanged(const std::string i_PartName, const maFloatRGBA& i_Color);

		//------------------------------------------------------------------------
		//	notify interests a part gets attached/removed
		//------------------------------------------------------------------------
		virtual void AttachPart( const std::string& i_Name, const std::string& i_PartName, const std::string& i_AttachFile, const bool i_bAttach);

		//------------------------------------------------------------------------
		//	change the texture of a material
		//------------------------------------------------------------------------
		virtual void ChangeTexture(const std::string& i_MaterialName, const std::string& i_TextureLayer, const std::string& i_TextureName);
		
		//--------------------------------------------------------------------
		//	ChangeMaterial
		//--------------------------------------------------------------------
		virtual void ChangeMaterial(const std::string i_PartName, const std::string& i_MaterialFile);

		//--------------------------------------------------------------------
		//	Add Animation
		//--------------------------------------------------------------------
		virtual void AddAnimation(const std::string& i_AnimFile, const int i_Divisions, const std::vector<std::string>& i_Cameras );

		//--------------------------------------------------------------------
		//	Add Expression
		//--------------------------------------------------------------------
		virtual void AddExpression(const std::string& i_ObjectName, const std::string& i_ExpressionName, const itString& i_ExpressionFileName, const float i_ExpressionValue);

		//--------------------------------------------------------------------
		//	Delete All Drivers
		//--------------------------------------------------------------------
		virtual void DeleteSceneDrivers();

		//--------------------------------------------------------------------
		//	Scale all the models
		//--------------------------------------------------------------------
		virtual void ScaleCharacter( const maVector3d& i_Scale );

		//--------------------------------------------------------------------
		//	Rotate all the models
		//--------------------------------------------------------------------
		virtual void RotateCharacter( float angle );

		//--------------------------------------------------------------------
		//	Change Camera
		//--------------------------------------------------------------------
		virtual void CameraChange( const std::string& i_Camera, const cmraCameraChangeData& i_Data ){}

};


