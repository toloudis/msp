/****************************************************************************\
**	api3dResourceFinder.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Tool/api3d/api3dResourceFinder.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/gf/gfPaths.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
api3dResourceFinder::api3dResourceFinder(const fsLocator &i_Directory)
	: m_RootDirectory(i_Directory)
{
}

//----------------------------------------------------------------------------
//	Trying to find resource i_FileName.  If this finder
//	knows where it is, it should return true and
//	put the full path to the resource (including filename)
//	into o_FoundLocator.
//----------------------------------------------------------------------------
//virtual
bool api3dResourceFinder::FindResource( const itString& i_FileName,
					fsLocator& o_FoundLocator) const
{
	o_FoundLocator = m_RootDirectory;

	if (   i_FileName.HasSubString(itString(".jpg"))
		|| i_FileName.HasSubString(itString(".bmp"))
		|| i_FileName.HasSubString(itString(".scm"))
		|| i_FileName.HasSubString(itString(".iff"))
		|| i_FileName.HasSubString(itString(".dds")) )
	{
		o_FoundLocator.Push( gfPaths::GetSubPath(gfPaths::e_Textures) );
	}
	o_FoundLocator.Push( i_FileName );

	//debug output
	std::string StrLocF;
	fsFileUtil::LocatorToANSIFilename(o_FoundLocator, StrLocF);
	//DBG_LOG("ResourceFinder: Found(" << StrLocF.c_str() << ")" );

	return true;
}

//----------------------------------------------------------------------------
// This function works like the other FindResource, but this
//	one provides a locator of the resource that is asking
//	for the file to be found, which may be important for
//	some implementations.
//----------------------------------------------------------------------------
//virtual
bool api3dResourceFinder::FindResource(const itString& i_FileName,
					const fsLocator& i_AskingLocator,
					fsLocator& o_FoundLocator) const
{
	o_FoundLocator = i_AskingLocator;
	o_FoundLocator.Pop();
	o_FoundLocator.Pop();
	o_FoundLocator.Push(gfPaths::GetSubPath(gfPaths::e_Models));
	o_FoundLocator.Push(i_FileName);

	//debug output
	std::string StrLocA;
	std::string StrLocF;
	fsFileUtil::LocatorToANSIFilename(i_AskingLocator, StrLocA);
	fsFileUtil::LocatorToANSIFilename(o_FoundLocator, StrLocF);
	//DBG_LOG2("ResourceFinder: Asking(%s) Found(%s)", StrLocA.c_str(), StrLocF.c_str() );

	return true;
}

