/****************************************************************************\
**  fsAbsolutePathMgrPACWin.hpp
**
**      fsAbsolutePathMgrPACWin.hpp defines the absolute path manager PAC
**	for windows.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef FS_ABSOLUTEPATHMGRPACWIN_HPP
#error fsAbsolutePathMgrPACWin.hpp multiply included
#endif
#define FS_ABSOLUTEPATHMGRPACWIN_HPP


//============================================================================
//============================================================================
class fsLocator;


//============================================================================
//============================================================================
namespace fsAbsolutePathMgrPAC
{
	//------------------------------------------------------------------------
	// Look for standard windows "My Documents" directories that
	// include a user name or "All Users".  If i_OrigFilename is in one of
	// them, look for the same file in this machine's standard directories.
	// Returns true if the file was found.
	//------------------------------------------------------------------------
	bool FindInStandardDirectories(const fsLocator& i_OrigFilename,
								   fsLocator& o_LocalFilename);
}
