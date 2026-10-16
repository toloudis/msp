/****************************************************************************\
**  fsAbsolutePathMgrPACWin.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Core/fs/private/fsAbsolutePathMgrPACWin.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsLocator.hpp"

#include <windows.h>
#include <shlobj.h>    // for SHGetFolderPath


//============================================================================
//============================================================================
namespace fsAbsolutePathMgrPAC
{
	namespace
	{
//bga - Need better way to do this. Problem is that gfPaths appends extra subpaths
// to the user data directory and I just want the root paths.
		//------------------------------------------------------------------------
		//	Get the user's "My Documents" directory for storing data.
		//	In Windows XP this is the "Documents and Settings/USER_NAME/My Documents" folder
		//	In Windows Vista this is the "\Users\USER_NAME\Documents" folder
		//------------------------------------------------------------------------
		void GetUserDocumentsDirectory( fsLocator& o_Locator )
		{
			o_Locator.Clear();
			TCHAR path[MAX_PATH];
			HRESULT result = SHGetFolderPath( NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, path );
			fsFileUtil::UnicodeStringToLocator( itString(path), o_Locator );
		}

		//------------------------------------------------------------------------
		//	Get the "All Users" "Shared Documents" directory.
		//	In Windows XP this is the "Documents and Settings/All Users/Documents" folder
		//	In Windows Vista this is the "\Users\Public\Documents" folder
		//------------------------------------------------------------------------
		void GetPublicDocumentsDirectory( fsLocator& o_Locator )
		{
			o_Locator.Clear();
			TCHAR path[MAX_PATH];
			HRESULT result = SHGetFolderPath( NULL, CSIDL_COMMON_DOCUMENTS, NULL, SHGFP_TYPE_CURRENT, path );
			fsFileUtil::UnicodeStringToLocator( itString(path), o_Locator );
		}
	}

	//------------------------------------------------------------------------
	// Look for standard windows "My Documents" directories that
	// include a user name or "All Users".
	//------------------------------------------------------------------------
	bool FindInStandardDirectories(const fsLocator& i_OrigFilename,
								   fsLocator& o_LocalFilename)
	{
		// The standard directories we are looking for here are:
		// Windows XP:
		//	c:\Documents and Settings\All Users\Documents
		//	c:\Documents and Settings\<user>\My Documents
		// Vista and Windows 7:
		//	c:\Users\Public\Documents
		//	c:\Users\<user>\Documents
		//
		// If the original filename has this directory pattern,
		// then look in the local standard directories for the
		// file also. This helps locate material library mappings
		// automatically.
		if (i_OrigFilename.GetNumNames() > 4)
		{
			const int c_UserRootIndex = 1; // second string in locator
			const itString c_UserRootXP("Documents and Settings");
			const itString c_UserRootVista("Users");
			const int c_DocumentIndex = 3; // fourth string in locator
			const itString c_MyDocumentsXP("My Documents");
			const itString c_DocumentsXP("Documents");
			const itString c_DocumentsVista("Documents");

			bool bHaveStandardDirectory = false;
			itString user_root = i_OrigFilename.GetName(c_UserRootIndex);
			if (user_root == c_UserRootVista)
			{
				if (i_OrigFilename.GetName(c_DocumentIndex) == c_DocumentsVista)
					bHaveStandardDirectory = true;
			}
			else if (user_root == c_UserRootXP)
			{
				itString docs_dir = i_OrigFilename.GetName(c_DocumentIndex);
				if ((docs_dir == c_MyDocumentsXP) ||
					(docs_dir == c_DocumentsXP))
					bHaveStandardDirectory = true;
			}

			if (bHaveStandardDirectory)
			{
				fsLocator rest_of_path = i_OrigFilename;
				rest_of_path.RemoveBefore(4);

				fsLocator user_docs, public_docs;

				// Attempt local users directory
				GetUserDocumentsDirectory(user_docs);
				fsLocator attempt_loc = user_docs;
				attempt_loc.Push(rest_of_path);
				if (fsFileUtil::FileExists(attempt_loc))
				{
					o_LocalFilename = attempt_loc;
					return true;

				}

				// Attempt public users directory
				GetPublicDocumentsDirectory(public_docs);
				attempt_loc = public_docs;
				attempt_loc.Push(rest_of_path);
				if (fsFileUtil::FileExists(attempt_loc))
				{
					o_LocalFilename = attempt_loc;
					return true;
				}
			}
		}

		return false;
	}
}
