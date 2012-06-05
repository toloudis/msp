/****************************************************************************\
**	orthoAvatarDataInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/orthoAvatarDataInterest.hpp"

#include "Core/Dbg/dbgLog.hpp"


//============================================================================
//============================================================================
float OrthoAvatarTemporalSpace::animStartTime	= 0;
float OrthoAvatarTemporalSpace::animEndTime		= 0;


//--------------------------------------------------------------------
//--------------------------------------------------------------------
orthoAvatarDataInterest::orthoAvatarDataInterest()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
orthoAvatarDataInterest::~orthoAvatarDataInterest()
{
}

//--------------------------------------------------------------------
//	LoadCharacter
//--------------------------------------------------------------------
void orthoAvatarDataInterest::LoadCharacter( const std::string& i_Name, const std::string& i_FileName, const bool i_bIsBaseModel )
{
}

//------------------------------------------------------------------------
//	Load new characters, don't load already loaded ones, 
//	and delete ones not in this list
//------------------------------------------------------------------------
void orthoAvatarDataInterest::LoadCharacters( const std::vector<std::string>& i_Names, const std::vector<std::string>& i_FileNames, const std::vector<bool>& i_bIsBaseModels )
{
}

//--------------------------------------------------------------------
//	DeleteCharacter
//--------------------------------------------------------------------
void orthoAvatarDataInterest::DeleteCharacter( const std::string& i_FileName )
{
}

//------------------------------------------------------------------------
//	replace a character model with another
//------------------------------------------------------------------------
void orthoAvatarDataInterest::ReplaceCharacter( const std::string& i_CurrentFileName, const std::string& i_NewFileName )
{
}

//------------------------------------------------------------------------
//	show or hide a character
//------------------------------------------------------------------------
void orthoAvatarDataInterest::ShowCharacter( const std::string& i_CurrentFileName, bool i_bShow )
{
}

//------------------------------------------------------------------------
//	Save a character as XML - the filename is the model file in the scene
//------------------------------------------------------------------------
void orthoAvatarDataInterest::SaveXMLCharacter( const std::string& i_FileName )
{
}

//--------------------------------------------------------------------
//	AvatarChanged
//--------------------------------------------------------------------
void orthoAvatarDataInterest::AvatarChanged()
{
}

//--------------------------------------------------------------------
//	PartChanged
//--------------------------------------------------------------------
void orthoAvatarDataInterest::PartChanged(const std::string i_JointName, bool i_bVisible)
{
}

//--------------------------------------------------------------------
//	MaterialChanged
//--------------------------------------------------------------------
void orthoAvatarDataInterest::MaterialChanged(const std::string i_PartName, const maFloatRGBA& i_Color)
{
}

//------------------------------------------------------------------------
//	notify interests a part gets attached/removed
//------------------------------------------------------------------------
void orthoAvatarDataInterest::AttachPart(const std::string& i_Name, const std::string& i_PartName, const std::string& i_AttachFile, const bool i_bAttach)
{
}

//------------------------------------------------------------------------
//	change the texture of a material
//------------------------------------------------------------------------
void orthoAvatarDataInterest::ChangeTexture(const std::string& i_MaterialName, const std::string& i_TextureLayer, const std::string& i_TextureName)
{
}

//--------------------------------------------------------------------
//	ChangeMaterial
//--------------------------------------------------------------------
void orthoAvatarDataInterest::ChangeMaterial(const std::string i_PartName, const std::string& i_MaterialFile)
{
}

//--------------------------------------------------------------------
//	Add Animation
//--------------------------------------------------------------------
void orthoAvatarDataInterest::AddAnimation(const std::string& i_AnimFile, const int i_Divisions, const std::vector<std::string>& i_Cameras )
{
}

//--------------------------------------------------------------------
//	Add Expression
//--------------------------------------------------------------------
void orthoAvatarDataInterest::AddExpression(const std::string& i_ObjectName, const std::string& i_ExpressionName, const itString& i_ExpressionFileName, const float i_ExpressionValue)
{
}

//--------------------------------------------------------------------
//	Delete All Drivers
//--------------------------------------------------------------------
void orthoAvatarDataInterest::DeleteSceneDrivers()
{
}

//--------------------------------------------------------------------
//	Scale all the models
//--------------------------------------------------------------------
void orthoAvatarDataInterest::ScaleCharacter( const maVector3d& i_Scale )
{
}

//--------------------------------------------------------------------
//	Rotate all the models
//--------------------------------------------------------------------
void orthoAvatarDataInterest::RotateCharacter( float angle )
{
}

