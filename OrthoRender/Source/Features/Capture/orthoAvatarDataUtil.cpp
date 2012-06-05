/*****************************************************************************
**	orthoAvatarDataUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/orthoAvatarDataUtil.hpp"


//	library
#include "Core/Dbg/dbgLog.hpp"
#include "Core/Env/envSTLHelpers.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Gf/gfPaths.hpp"
#include "Tool/gui/guiXMLTextReader.hpp"
#include "Tool/gui/guiXMLTextWriter.hpp"

#include <string>


//============================================================================
//============================================================================
namespace orthoAvatarDataUtil
{
	namespace
	{
		const char* l_cOrthoKey_PartList	= "Object";
		const char* l_cOrthoKey_ObjectName	= "ObjectName";
		const char* l_cOrthoKey_ObjectFileName = "ObjectFilename";
		const char* l_cOrthoKey_PartName	= "PartName";
		const char* l_cOrthoKey_Category	= "Category";
		const char* l_cOrthoKey_JointName	= "JointName";
		const char* l_cOrthoKey_Icon		= "Icon";

		static orthoAvatarListData l_AvatarData;
		static std::vector<orthoAvatarDataInterest*> m_Interests;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Initialize()
	{
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DeInitialize()
	{
		envSTLHelpers::DeleteContainer( m_Interests );
	}

	//------------------------------------------------------------------------
	//	this function takes only the filename.  The path is assumed
	//	to be ~/Data/Objects
	//------------------------------------------------------------------------
	bool ReadData(const std::string& i_ConfigFileName)
	{
		fsLocator PartsFile;
		PartsFile = gfPaths::GetAppPath();
		PartsFile.Push("Data");
		PartsFile.Push("Objects");
		std::string fname = i_ConfigFileName;
		fname.append("-Parts.txt");
		PartsFile.Push( fname.c_str() );
		return orthoAvatarDataUtil::ReadData( PartsFile, orthoAvatarDataUtil::Data() );
	}

	//------------------------------------------------------------------------
	//	this function takes only the filename with the full path.
	//------------------------------------------------------------------------
	bool ReadData(const fsLocator& i_ConfigFile, orthoAvatarListData& o_Data)
	{
		std::string cfgpath;
		fsFileUtil::LocatorToANSIFilename(i_ConfigFile,cfgpath);

		if (!fsFileUtil::FileExists( i_ConfigFile ))
		{
			DBG_ERROR1("Cannot find the file (%s)!", cfgpath.c_str());
			return false;
		}

		//DBG_LOG1("Reading config file (%s)", cfgpath.c_str());

		if (!guiXMLTextReader::Open(cfgpath.c_str()))
		{
			DBG_ERROR1("Cannot open the file (%s)!", cfgpath.c_str());
			return false;
		}

		//	read in the data
		//
		l_AvatarData.m_Parts.clear();
		l_AvatarData.m_Parts.resize(20);
		int index = 0;
		orthoAvatarItemData tempdata;
		guiXMLTextReader::gui_Node_Type node_type;
		std::string keyname, strvalue;
		while ((node_type = guiXMLTextReader::ReadNode(keyname,strvalue)) != guiXMLTextReader::e_EOF)
		{
			//DBG_LOG3("RO file - key [%s]  type [%d]  value [%s]", keyname.c_str(), node_type, strvalue.c_str());

			if ((node_type == guiXMLTextReader::e_Text) && (keyname.length() > 0))
			{
				if (keyname == l_cOrthoKey_ObjectName)
				{
					std::string value; guiXMLTextReader::Convert(strvalue, value); l_AvatarData.m_Name.SetValue(value);
				}
				else if (keyname == l_cOrthoKey_ObjectFileName)
				{
					std::string value; guiXMLTextReader::Convert(strvalue, value); l_AvatarData.m_FileName.SetValue(value);
				}
				else if (keyname == l_cOrthoKey_PartName)
				{
					std::string value; guiXMLTextReader::Convert(strvalue, value); tempdata.m_PartName.SetValue(value);
				}
				else if (keyname == l_cOrthoKey_Category)
				{
					std::string value; guiXMLTextReader::Convert(strvalue, value); tempdata.m_Category.SetValue(value);
				}
				else if (keyname == l_cOrthoKey_Icon)
				{
					// skip this for now...
					std::string value; guiXMLTextReader::Convert(strvalue, value); /*tempdata.m_Icon.SetValue(value);*/
				}
				else if (keyname == l_cOrthoKey_JointName)
				{
					std::string value; guiXMLTextReader::Convert(strvalue, value); tempdata.m_JointName.SetValue(value);

					// add the data
					if (index >= l_AvatarData.m_Parts.size())
					{
						l_AvatarData.m_Parts.resize( l_AvatarData.m_Parts.size() + 20 );
					}

					//DBG_LOG3("%02d.  object (%s) joint (%s)", index, tempdata.m_PartName.GetValue().c_str(), tempdata.m_JointName.GetValue().c_str());
					l_AvatarData.m_Parts[index++] = tempdata;
				}
			}
		}

		l_AvatarData.m_Parts.resize(index);

		guiXMLTextReader::Close();

		//AvatarChanged();

		//Debug();
		return true;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	orthoAvatarListData& Data()
	{
		return l_AvatarData;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void AddInterest( orthoAvatarDataInterest* i_pInterest )
	{
		m_Interests.push_back( i_pInterest );
	}

	//------------------------------------------------------------------------
	//	notify interests
	//------------------------------------------------------------------------
	void AvatarChanged()
	{
		for (int i=0; i < m_Interests.size(); ++i)
		{
			m_Interests[i]->AvatarChanged();
		}
	}

	//------------------------------------------------------------------------
	//	notify interests part visibility has changed
	//------------------------------------------------------------------------
	void PartChanged(const std::string& i_JointName, bool i_bVisible )
	{
		for (int i=0; i < m_Interests.size(); ++i)
		{
			m_Interests[i]->PartChanged(i_JointName, i_bVisible);
		}
	}

	//------------------------------------------------------------------------
	//	notify interests part material has changed
	//------------------------------------------------------------------------
	void MaterialChanged(const std::string& i_PartName, const maFloatRGBA& i_Color )
	{
		for (int i=0; i < m_Interests.size(); ++i)
		{
			m_Interests[i]->MaterialChanged(i_PartName, i_Color);
		}
	}

	//------------------------------------------------------------------------
	//	notify interests a part gets attached/removed
	//------------------------------------------------------------------------
	void AttachPart( const std::string& i_Name, const std::string& i_PartName, const std::string& i_AttachFile, const bool i_bAttach)
	{
		for (int i=0; i < m_Interests.size(); ++i)
		{
			m_Interests[i]->AttachPart(i_Name, i_PartName, i_AttachFile, i_bAttach);
		}
	}

	//------------------------------------------------------------------------
	//	change the texture of a material
	//------------------------------------------------------------------------
	void ChangeTexture(const std::string& i_MaterialName, const std::string& i_TextureLayer, const std::string& i_TextureName)
	{
		for (int i=0; i < m_Interests.size(); ++i)
		{
			m_Interests[i]->ChangeTexture(i_MaterialName, i_TextureLayer, i_TextureName);
		}
	}

	//------------------------------------------------------------------------
	//	notify interests to change the material
	//------------------------------------------------------------------------
	void ChangeMaterial(const std::string& i_MaterialName, const std::string& i_TextureName)
	{
		for (int i=0; i < m_Interests.size(); ++i)
		{
			m_Interests[i]->ChangeMaterial(i_MaterialName, i_TextureName);
		}
	}

	//------------------------------------------------------------------------
	//	Add Animation
	//------------------------------------------------------------------------
	void AddAnimation(const std::string& i_AnimFile, const int i_Divisions, const std::vector<std::string>& i_Cameras )
	{
		for (int i=0; i < m_Interests.size(); ++i)
		{
			m_Interests[i]->AddAnimation( i_AnimFile, i_Divisions, i_Cameras );
		}
	}

	//------------------------------------------------------------------------
	//	Deletes all drivers
	//------------------------------------------------------------------------
	void DeleteSceneDrivers()
	{
		for (int i=0; i < m_Interests.size(); ++i)
		{
			m_Interests[i]->DeleteSceneDrivers();
		}
	}

	//------------------------------------------------------------------------
	//	Scales all the models
	//------------------------------------------------------------------------
	void ScaleCharacter( const maVector3d& i_Scale )
	{
		for (int i=0; i < m_Interests.size(); ++i)
		{
			m_Interests[i]->ScaleCharacter( i_Scale );
		}
	}

	//------------------------------------------------------------------------
	//	Rotate all the models
	//------------------------------------------------------------------------
	void RotateCharacter( float angle )
	{
		for (int i=0; i < m_Interests.size(); ++i)
		{
			m_Interests[i]->RotateCharacter( angle );
		}
	}

	//------------------------------------------------------------------------
	//	Load a single character
	//------------------------------------------------------------------------
	void LoadCharacter( const std::string& i_Name, const std::string& i_FileName, const bool i_bIsBaseModel )
	{
		for (int i=0; i < m_Interests.size(); ++i)
		{
			m_Interests[i]->LoadCharacter( i_Name, i_FileName, i_bIsBaseModel );
		}
	}

	//------------------------------------------------------------------------
	//	Load new characters, don't load already loaded ones, 
	//	and delete ones not in this list
	//------------------------------------------------------------------------
	void LoadCharacters( const std::vector<std::string>& i_Names, const std::vector<std::string>& i_FileNames, const std::vector<bool>& i_bIsBaseModels )
	{
		for (int i=0; i < m_Interests.size(); ++i)
		{
			m_Interests[i]->LoadCharacters( i_Names, i_FileNames, i_bIsBaseModels );
		}
	}

	//------------------------------------------------------------------------
	//	delete a character
	//------------------------------------------------------------------------
	void DeleteCharacter( const std::string& i_FileName )
	{
		for (int i=0; i < m_Interests.size(); ++i)
		{
			m_Interests[i]->DeleteCharacter( i_FileName );
		}
	}

	//------------------------------------------------------------------------
	//	replace a character model with another
	//------------------------------------------------------------------------
	void ReplaceCharacter( const std::string& i_CurrentFileName, const std::string& i_NewFileName )
	{
		for (int i=0; i < m_Interests.size(); ++i)
		{
			m_Interests[i]->ReplaceCharacter( i_CurrentFileName, i_NewFileName );
		}
	}

	//------------------------------------------------------------------------
	//	show or hide a character
	//------------------------------------------------------------------------
	void ShowCharacter( const std::string& i_FileName, bool i_bShow )
	{
		for (int i=0; i < m_Interests.size(); ++i)
		{
			m_Interests[i]->ShowCharacter( i_FileName, i_bShow );
		}
	}

	//------------------------------------------------------------------------
	//	add an expression to a character
	//------------------------------------------------------------------------
	void AddExpression( const std::string& i_ObjectName, const std::string& i_ExpressionName, const itString& i_ExpressionFileName, const float i_ExpressionValue )
	{
		for (int i=0; i < m_Interests.size(); ++i)
		{
			m_Interests[i]->AddExpression( i_ObjectName, i_ExpressionName, i_ExpressionFileName, i_ExpressionValue );
		}
	}

	//------------------------------------------------------------------------
	//	Save a character as XML - the filename is the model file in the scene
	//------------------------------------------------------------------------
	void SaveXMLCharacter( const std::string& i_FileName )
	{
		for (int i=0; i < m_Interests.size(); ++i)
		{
			m_Interests[i]->SaveXMLCharacter( i_FileName );
		}
	}

	//------------------------------------------------------------------------
	//	output the struct values at anytime
	//------------------------------------------------------------------------
	void Debug()
	{
		DBG_LOG0( "Avatar Data" );
		DBG_LOG0( "---------" );

		DBG_LOG1( "Avatar (%s)", l_AvatarData.m_Name.GetValue().c_str() );
		DBG_LOG1( "       (%s)", l_AvatarData.m_FileName.GetValue().c_str() );

		for (int i=0; i < l_AvatarData.m_Parts.size(); ++i)
		{
			DBG_LOG1( "Part: Name	= %s", l_AvatarData.m_Parts[i].m_PartName.GetValue().c_str() );
			DBG_LOG1( "      Cat.	= %s", l_AvatarData.m_Parts[i].m_Category.GetValue().c_str() );
			DBG_LOG1( "      Joint	= %s", l_AvatarData.m_Parts[i].m_JointName.GetValue().c_str() );
			DBG_LOG3( "      Color	= %d %d %d", l_AvatarData.m_Parts[i].m_Color.GetValue().GetRed(), l_AvatarData.m_Parts[i].m_Color.GetValue().GetGreen(), l_AvatarData.m_Parts[i].m_Color.GetValue().GetBlue() );
		}
	}


	//------------------------------------------------------------------------
	//	add the struct values to the render request response
	//------------------------------------------------------------------------
	void AppendXMLDescription(LightWaitResponse *response) 
	{
		if (response == NULL) {
			return ;
		}

		string descriptor ;

		descriptor.append("<ModelDescriptions>") ;

		char tempStr[256] ;
		
		descriptor.append("<FileName>") ;
		descriptor.append(l_AvatarData.m_FileName.GetValue().c_str()) ;
		descriptor.append("</FileName>") ;

		descriptor.append("<Name>") ;
		descriptor.append(l_AvatarData.m_Name.GetValue().c_str()) ;
		descriptor.append("</Name>") ;

		descriptor.append("<DescriptionCount>") ;
		sprintf(tempStr,"%d",l_AvatarData.m_Parts.size()) ;
		descriptor.append(tempStr) ;
		descriptor.append("</DescriptionCount>") ;

		for (int i=0; i < l_AvatarData.m_Parts.size(); i++)
		{
			descriptor.append("<Description>") ;

			descriptor.append("<Index>") ;
			sprintf(tempStr,"%d",i) ;
			descriptor.append(tempStr) ;
			descriptor.append("</Index>") ;

			descriptor.append("<Name>") ;
			descriptor.append(l_AvatarData.m_Parts[i].m_PartName.GetValue().c_str()) ;
			descriptor.append("</Name>") ;

			descriptor.append("<Category>") ;
			descriptor.append(l_AvatarData.m_Parts[i].m_Category.GetValue().c_str()) ;
			descriptor.append("</Category>") ;

			descriptor.append("<Joint>") ;
			descriptor.append(l_AvatarData.m_Parts[i].m_JointName.GetValue().c_str()) ;
			descriptor.append("</Joint>") ;

			descriptor.append("<Color>") ;
			sprintf(tempStr,"%f, %f, %f",
				l_AvatarData.m_Parts[i].m_Color.GetValue().GetRed(), 
				l_AvatarData.m_Parts[i].m_Color.GetValue().GetGreen(), 
				l_AvatarData.m_Parts[i].m_Color.GetValue().GetBlue()) ;
			descriptor.append(tempStr) ;
			descriptor.append("</Color>") ;

			descriptor.append("</Description>") ;
		}

		descriptor.append("</ModelDescriptions>") ;

		response->setModelDescriptor(descriptor) ;
	}

	//------------------------------------------------------------------------
	//	Changes camera properties
	//------------------------------------------------------------------------
	void CameraChange( const std::string& i_Camera, const cmraCameraChangeData& i_Data )
	{
		for (int i=0; i < m_Interests.size(); ++i)
		{
			m_Interests[i]->CameraChange( i_Camera, i_Data );
		}
	}
}

