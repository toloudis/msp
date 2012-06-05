/*****************************************************************************
**	pathDirectoryParser.hpp
**
**		Walks to a directory and returns all files in the end path
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef PATH_DIRECTORYPARSER_HPP
#error pathDirectoryParser.hpp multiply included
#endif
#define PATH_DIRECTORYPARSER_HPP

#include <vector>


//============================================================================
// class forwards
//============================================================================
class fsLocator;
class itString;

//============================================================================
//============================================================================
class pathDirectoryParser
{
public:
	//------------------------------------------------------------------------
	// constructor
	//------------------------------------------------------------------------
	pathDirectoryParser();

	//------------------------------------------------------------------------
	// destructor
	//------------------------------------------------------------------------
	//~pathDirectoryParser();

	//------------------------------------------------------------------------
	// Returns a list of files that are in the given directory
	//------------------------------------------------------------------------
	void GetDirectoryFiles(const fsLocator& i_Directory, 
						   std::vector<fsLocator>& o_Files);

	//------------------------------------------------------------------------
	// Returns a list of files with the given extension that are in 
	// the given directory
	//------------------------------------------------------------------------
	void GetDirectoryFilesWithExt(const fsLocator& i_Directory, 
								  std::vector<fsLocator>& o_Files, 
								  const itString& i_Ext);
	//----------------------------------------------------------------------------
	// Returns a list of sub directories that are in the given directory
	//----------------------------------------------------------------------------
	void GetSubDirectories(const fsLocator& i_Directory, 
						   std::vector<fsLocator>& o_Directories);

	//----------------------------------------------------------------------------
	// Returns a list of sub directories that are in the given directory. Snips the 
	// numbers in the beginning of the directory names
	//----------------------------------------------------------------------------
	void GetClippedSubDirectories(const fsLocator& i_Directory, 
						   std::vector<fsLocator>& o_Directories);

};