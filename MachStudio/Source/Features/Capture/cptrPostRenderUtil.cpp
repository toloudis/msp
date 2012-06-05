/*****************************************************************************
**  cptrPostRenderUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrPostRenderUtil.hpp"

#include "Features/Capture/cptrRenderStatsDataUtil.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Support/capt/captRenderOutputData.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/pyth/pythUtil.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/gf/gfFileTxt.hpp"
#include "Core/gf/gfPaths.hpp"

#include <string>
#include <windows.h>

#undef CreateFile
#undef DeleteFile


//============================================================================
//============================================================================
namespace cptrPostRenderUtil
{
//--------------------------------------------------------------------
// Execute - execute the post commands
//--------------------------------------------------------------------
void Execute()
{
	// TODO [rjk] have different post "commands" as modules that can
	//	be added.  For instance, i have a "send user email" command.
	//	currently i use an app called bmail to send the email, but
	//	we could use others.

	//	notify the user via email
	//
	captRenderOutputData& theData = captRenderOutputDataUtil::Data();
	if (theData.m_bSendPostEmailAddress.GetValue())
	{
		std::string output_text;
		cptrRenderStatsDataUtil::BuildOutputString(output_text);

		//	send it to the debug log also
		//
		DBG_LOG(output_text.c_str());

		//	write this output to a file
		//
		const char* c_TEMPEMAILFILENAME = "~temp_email_body.txt";
		fsLocator temp_email_filename;
		temp_email_filename = gfPaths::GetPath( gfPaths::e_ExePath );
		temp_email_filename.Push(c_TEMPEMAILFILENAME);
		if( fsFileUtil::FileExists(temp_email_filename) )
			fsFileUtil::DeleteFile(temp_email_filename);
		fsFileUtil::CreateFile(temp_email_filename);
		gfFileTxt output_file(temp_email_filename, fsFileStream::e_WriteOnly);

		output_file.Write(output_text.length(), output_text.c_str());
		std::string fullpath;
		fsFileUtil::LocatorToANSIFilename(temp_email_filename,fullpath);

		//	send the email!
		//char cmd[2048];
		std::string from_email;
		from_email = theData.m_PostEmailAddress.GetValue().substr(0, theData.m_PostEmailAddress.GetValue().find(","));
		std::string bmail;
		fsFileUtil::LocatorToANSIFilename( PrefsMgr::Data().m_BMailLocation.GetValue(), bmail );
		//sprintf(cmd, "%s -s ExtraLargeTech.com -t %s -f %s -h -a \"Render Complete\" -b \"Your Render completed. %s\" ", (PrefsMgr::Data().m_BMailLocation.c_str()), theData.m_PostEmailAddress.c_str(), theData.m_PostEmailAddress.c_str(), output_text.c_str() );
		//sprintf(cmd, "%s -s ExtraLargeTech.com -t %s -f %s -h -a \"Render Complete\" -m %s ", (bmail.c_str()), theData.m_PostEmailAddress.GetValue().c_str(), from_email.c_str(), fullpath.c_str() );
		std::ostringstream oss;
		oss << (bmail) << "-s ExtraLargeTech.com -t " << theData.m_PostEmailAddress.GetValue()<<"-f"<<from_email<<"-h -a \"Render Complete\" -m"<<fullpath;
		std::string cmd(oss.str());
		DBG_LOG("email command line " << cmd.c_str());
		WinExec(cmd.c_str(), false);
	}

	//	execute a shell command
	//
	if (theData.m_bExecutePostCommand.GetValue())
	{
		//	determine if the command is a python file
		//
		if (theData.m_PostCommand.GetValue().find(".py") == std::string::npos)
		{
			// regular command
			//char cmd[2048];
			//sprintf(cmd, "%s", theData.m_PostCommand.GetValue().c_str());
			std::ostringstream oss;
			oss << theData.m_PostCommand.GetValue();
			std::string cmd(oss.str());
				WinExec(cmd.c_str(), false);
		}
		else
		{
			// python command
			fsLocator file;
			fsFileUtil::ANSIFilenameToLocator( theData.m_PostCommand.GetValue(), file );
			pythUtil::ScriptFile( file );
		}
	}
}

}	// end of namespace
