/****************************************************************************\
**	mainResolvePath.hpp
**
**		Gives user options on how to resolve missing files.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef MAIN_RESOLVEPATH_HPP
#error mainResolvePath.hpp multiply included
#endif
#define MAIN_RESOLVEPATH_HPP

#include <string>

class fsLocator;

//============================================================================
//============================================================================
namespace mainResolvePath
{
	//------------------------------------------------------------------------
	// Add our function as handler
	//------------------------------------------------------------------------
	void Init();

	//------------------------------------------------------------------------
	// Clear out mapping data from the current file load
	//------------------------------------------------------------------------
	void LoadFinished();

	//------------------------------------------------------------------------
	// ResolvePath() - the given filename cannot be found, ask user how to
	//	handle it. Returns true if a new file is chosen, returning the new
	//	filename in o_LocalFilename.
	//------------------------------------------------------------------------
	bool ResolvePath(const fsLocator& i_OrigFilename, 
					 fsLocator& o_LocalFilename,
					 const std::string &i_Category);

	//------------------------------------------------------------------------
	//	If set to true (this is default) the app will NOT allow any scene to 
	//	load if an asset is missing.
	//------------------------------------------------------------------------
	void SetStrictMode(const bool i_bStrictMode);

};
