/*****************************************************************************
**  mtrStaticCubeSaver.cpp
**
**      The mtrStaticCubeSaver writes static cube filenames into 
**	.scm file.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "mtrStaticCubeSaver.hpp"
#include "mtrErrorHandler.hpp"

#include "dbgLog.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "gfFileTxt.hpp"
#include "gfFileUtil.hpp"
#include "gfPaths.hpp"

namespace
{
	

}	// end of namespace

	
//========================================================================
// Write - writes filenames to text file
//========================================================================
void	mtrStaticCubeSaver::Write(  const fsLocator &i_Locator,
								 const std::string i_Filenames[6])
{
	try 
	{
		// Create new locator
		if( fsFileUtil::FileExists(i_Locator) )
			fsFileUtil::DeleteFile(i_Locator);
		fsFileUtil::CreateFile(i_Locator);

		gfFileTxt ofile(i_Locator, fsFileStream::e_WriteOnly);
		for (int i=0; i<6; i++)
		{
			std::string str = i_Filenames[i];
			str += "\n";
			ofile.WriteLine(str);	// I would have expected WriteLine to add newline
		}
	}
	catch( const fsReadOnlyX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		mtrErrorHandler *handler = mtrErrorHandler::GetErrorHandler();
		if (handler)
			handler->FileReadOnly(filename.c_str());
	}
	catch( ... )
	{
		mtrErrorHandler *handler = mtrErrorHandler::GetErrorHandler();
		if (handler)
			handler->GeneralError();
		throw;
	}
}

//========================================================================
// Read - reads six strings from text file.
//========================================================================
bool	mtrStaticCubeSaver::Read(  const fsLocator &i_Locator,
					std::string o_Filenames[6])
{
	try
	{
		gfFileTxt ofile(i_Locator, fsFileStream::e_ReadOnly);
		for (int i=0; i<6; i++)
		{
			if (!ofile.ReadToken(o_Filenames[i]))
				return false;
		}
	}
	catch( const fsFileDoesntExistX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		mtrErrorHandler *handler = mtrErrorHandler::GetErrorHandler();
		if (handler)
			handler->FileDoesntExist(filename.c_str());

		return false;
	}
	catch( ... )
	{
		mtrErrorHandler *handler = mtrErrorHandler::GetErrorHandler();
		if (handler)
			handler->GeneralError();
		throw;
	}

	return true;
}