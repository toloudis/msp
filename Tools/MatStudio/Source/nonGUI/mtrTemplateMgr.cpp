/*****************************************************************************
**  mtrTemplateMgr.hpp
**
**		Manages templates loaded from special directory that define
**	common configurations of material settings and texture layers.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "mtrTemplateMgr.hpp"
#include "mtrMaterialSaver.hpp"

#include "dbgLog.hpp"
#include "gfFileEnum.hpp"
#include "fsFileUtil.hpp"
#include "fsLocator.hpp"

#include <vector>


//====================================================================
//====================================================================
namespace mtrTemplateMgr
{

	//====================================================================
	// Local variables and functions
	//====================================================================
	namespace
	{
		struct sTemplate
		{
			itString m_Name;
			mtrMaterialTemplate m_Material;
		};
		std::vector<sTemplate> l_Templates;

		
	//----------------------------------------------------------------------------
	//	TemplateFileEnum is used to enumerate the files used for shader effects
	//----------------------------------------------------------------------------
	class TemplateFileEnum : public fsFileEnum::EnumTarget
	{
		virtual bool Notify( const fsLocator& i_Directory, const fsLocator& i_File )
		{
			fsLocator full_name = i_Directory;
			full_name.Push(i_File);

			std::vector<mtrMaterialTemplate> materials;
			mtrMaterialSaver::Read( full_name, materials );

			if (materials.size() >= 1)
			{
				sTemplate temp;
				temp.m_Name = i_File.GetLastName();
				temp.m_Name.StripExtension();
				temp.m_Material = materials[0];
				l_Templates.push_back(temp);
			}

			return true;
		}
	};
	}	// end of namespace

	//============================================================================
	//	Initialize()
	//============================================================================
	void Initialize(const fsLocator &i_TemplateDir)
	{
		TemplateFileEnum mx_enum;
		itString file_extension("mx");

		std::string mx_dir;
		fsFileUtil::LocatorToANSIFilename( i_TemplateDir, mx_dir );
		DBG_LOG1( "Searching template files (.mx) in directory = %s", mx_dir.c_str() );

		gfFileEnum::EnumerateFiles( i_TemplateDir, mx_enum, &file_extension );
	}

	//============================================================================
	//	DeInitialize()
	//============================================================================
	void DeInitialize()
	{
		l_Templates.clear();
	}

	//============================================================================
	//	GetNumTemplates()
	//============================================================================
	int GetNumTemplates()
	{
		return l_Templates.size();
	}

	//============================================================================
	//	GetTemplateName()
	//============================================================================
	itString GetTemplateName(int i_Index)
	{
		DBG_ASSERT0(i_Index < l_Templates.size(), "Index out of range");
		return l_Templates[i_Index].m_Name;
	}

	//============================================================================
	//	GetTemplateMaterial()
	//============================================================================
	mtrMaterialTemplate GetTemplateMaterial(int i_Index)
	{
		DBG_ASSERT0(i_Index < l_Templates.size(), "Index out of range");
		return l_Templates[i_Index].m_Material;
	}

}