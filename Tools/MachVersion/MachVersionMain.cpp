/*****************************************************************************
**  MachVersionMain.cpp
**
**      Console program that reads character animation files and
**	removes frames that aren't necessary because the surface
**	is not moving.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
  
#include "Core/CoreLayer.hpp"
#include "Core/Ch/chBinReader.hpp"
#include "Core/Ch/chBinWriter.hpp"
#include "Core/Ch/chChunkParserUtil.hpp"
#include "Core/Ch/chExceptionX.hpp"
#include "Core/Dbg/dbgMsg.hpp"
#include "Core/Env/envSTLHelpers.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Fs/fsFileX.hpp"
#include "Core/Gf/gfFileBin.hpp"
#include "Core/Gf/gfFileX.hpp"
#include "Core/Gf/gfPackage.hpp"
#include "Core/Gf/gfPaths.hpp"

#include <iostream>

namespace
{
	// Version string of the tool to write in chunk at end of file
	const char* c_ExeVersionStr = "1.0.0";
	const char* c_ExeName = "MachVersion";

	const chDefs::Name c_EXPV = chDefs::MakeName('E', 'X', 'P', 'V');	// executable version info

	//========================================================================
	// Prints version information to standard output,
	// returns false if could not find version chunk.
	//========================================================================
	bool PrintVersion(const fsLocator &i_Locator)
	{
		gfFileBin file(i_Locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);
		//	If this isn't a real Terawatt/XLT binary file this will throw
		file.ReadHeader();
		chBinReader reader(file);

		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;

		// try to get child chunks
		while( reader.ReadChunkHeader(name, version, size) )
		{
			if ( name == c_EXPV )
			{		
				itString savefilename;
				if (version == 1)
				{
					chChunkParserUtil::Read( reader, savefilename );
				}

				std::string scene_version;
				std::string date_string;
				std::string time_string;
				reader.Read(scene_version);
				reader.Read(date_string);
				reader.Read(time_string);

				std::string filename;
				fsFileUtil::LocatorToANSIFilename(i_Locator, filename);

				// Write comma separated values in order to read into Excel
				std::cout << filename << "," << scene_version << "," << date_string << "," << time_string << std::endl;

				return true;
			}

			// skip over other chunks

			reader.FinishChunk();
		}

		return false;
	}

}


int main(int argc, char** argv)
{
	gfPackage::Init();
	CoreLayer::Init();

	if (argc == 1 || argc > 2)
	{
		std::cout << argv[0] << " version " << c_ExeVersionStr << std::endl;
		std::cout << "Syntax: " << c_ExeName << " filename.mab" << std::endl;
		std::cout << "   Prints version information about scene file to standard output." << std::endl;
	}
	else
	{
		fsLocator scene_file;
		fsFileUtil::ANSIFilenameToLocator(argv[1], scene_file);

		try
		{
			if (!PrintVersion( scene_file ))
			{
				std::cerr << "Could not find version for file: " << scene_file << std::endl;
			}
		}
		catch (fsReadOnlyX& i_Ex)
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
			std::cerr << "File is read only: " << filename << std::endl;
			DBG_ERROR("File is read only: " << i_Ex.GetLocator());
		}
		catch (fsFileDoesntExistX& i_Ex)
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
			std::cerr << "File doesn't exist: " << filename << std::endl;
			DBG_ERROR("File doesn't exist: " << filename);
		}
		catch (fsInvalidLocatorX& i_Ex)
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
			std::cerr << "File doesn't exist: " << filename << std::endl;
			DBG_ERROR("File doesn't exist: " << filename);
		}
		catch (gfInvalidFileBinX& i_Ex)
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
			std::cerr << "File is not character animation file: " << filename << std::endl;
			DBG_ERROR("File is not character animation file: " << filename);
		}
		catch (chInvalidChunkX& )
		{
			std::cerr << "File format error in chunk parsing." << std::endl;
			DBG_ERROR("File format error in chunk parsing.");
		}
		catch (...)
		{
			std::cerr << "Unknown error while processing." << std::endl;
			DBG_ERROR("Unknown error while processing.");
		}
	}

	CoreLayer::CleanUp();
	gfPackage::CleanUp();

	return 0;
}
