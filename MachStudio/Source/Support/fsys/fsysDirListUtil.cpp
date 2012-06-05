/*****************************************************************************
**  fsysDirListUtil.cpp
**
**      fsysDirListUtil holds a list of the props.
**
**	StudioGPU
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/

#include "Core/env/envPlatform.hpp"
#include "Support/fsys/fsysDirListUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/gf/gfFileEnum.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/it/itStringUtil.hpp"


namespace
{
	const itString c_GeneralDir("General");

	//----------------------------------------------------------------------------
	//	DirFilesEnum is used to enumerate the files used for geometry
	//----------------------------------------------------------------------------
	class DirFilesEnum : public fsFileEnum::EnumTarget
	{
		std::vector<itString> &m_DirNames;

	public:
		// constructor takes vector to fill
		DirFilesEnum(std::vector<itString> &i_DirNames)
			: m_DirNames(i_DirNames)
		{

		}

		virtual bool Notify( const fsLocator& i_Directory, const fsLocator& i_File )
		{
			itString dirname;
			fsFileUtil::LocatorToUnicodeString(i_File, dirname);

			// the function puts an extra "\" at end, so need to remove it
			if (dirname[dirname.GetLength()-1] == '\\')
				dirname.RemoveCharAt(dirname.GetLength()-1);

			if (   (dirname != itString("."))
				&& (dirname != itString("..")))
			{
				//	DEBUG only
				//std::string tempname;
				//fsFileUtil::LocatorToANSIFilename(i_File, tempname);
				//DBG_LOG( "    dir(" << tempname.c_str() << ")"  );

				m_DirNames.push_back( dirname );
			}
			return true;
		}
	};
}

//------------------------------------------------------------------------
//	Build the directory list from the full directory passed in.,
//	returning vector of itStrings.
//------------------------------------------------------------------------
std::vector<itString> fsysDirListUtil::BuildDirectoryList(const fsLocator &i_FullDir)
{
	//	DEBUG only
	//std::string data_dir;
	//fsFileUtil::LocatorToANSIFilename( i_FullDir, data_dir );
	//DBG_LOG( "build directory (" << data_dir.c_str() << ")" );

	std::vector<itString> dir_names;

	DirFilesEnum dirs(dir_names);
	gfFileEnum::EnumerateDirectories( i_FullDir, dirs );

	return dir_names;
}

//------------------------------------------------------------------------
//	Builds a short directory list using just the given object directory
//	name and "General".
//------------------------------------------------------------------------
std::vector<itString> fsysDirListUtil::BuildDirectoryList(const fsLocator &i_FullDir,
										 const itString &i_ObjDir)
{
	std::vector<itString> dir_names;
	dir_names.push_back(i_ObjDir);
	if (i_ObjDir != c_GeneralDir)
	{
		dir_names.push_back(c_GeneralDir);
	}
	return dir_names;
}