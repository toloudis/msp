/*****************************************************************************
**  AnimCompressMain.cpp
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
#include "Core/Ch/chExceptionX.hpp"
#include "Core/Dbg/dbgLog.hpp"
#include "Core/Env/envSTLHelpers.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Fs/fsFileX.hpp"
#include "Core/Gf/gfFileBin.hpp"
#include "Core/Gf/gfFileX.hpp"
#include "Core/Gf/gfPackage.hpp"
#include "Core/Gf/gfPaths.hpp"
#include "Core/Ma/maConstants.hpp"
#include "Graphics/mdl/mdlAnimImport.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Graphics/mdl/private/mdlVertexAnimImport.hpp"

#include <iostream>
#include <list>
#include <time.h>

namespace
{
	// Version string of the tool to write in chunk at end of file
	const char* c_CompressVersionStr = "1.0.0";
	const char* c_ExeName = "AnimCompress";
		
	// Chunk version of the PRUN stamp at end of file. This tool
	// will not compress a file that is already marked as compressed with 
	// the same chunk version, so if there is an improvement to the algorithm 
	// that needs to be run on old files also, increment this version number:
	const int c_PruneAlgorithmVersion = 0;

	const chDefs::Name c_ACHR = chDefs::MakeName('A', 'C', 'H', 'R');
	const chDefs::Name c_VTXA = chDefs::MakeName('V', 'T', 'X', 'A');
	const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
	const chDefs::Name c_FRAM = chDefs::MakeName('F', 'R', 'A', 'M');
	const chDefs::Name c_TIME = chDefs::MakeName('T', 'I', 'M', 'E');
	const chDefs::Name c_GVER = chDefs::MakeName('G', 'V', 'E', 'R');
	const chDefs::Name c_NVER = chDefs::MakeName('N', 'V', 'E', 'R');
	const chDefs::Name c_PRUN = chDefs::MakeName('P', 'R', 'U', 'N');

	//========================================================================
	//========================================================================
	bool vec3_equal(const maPoint3d& i_A,
					const maPoint3d& i_B)
	{
		return ((i_A - i_B).LengthSqr() < maConstants::c_fEpsilon);
	}

	//========================================================================
	// return true if the frames are equal within an epsilon
	//========================================================================
	bool compare_frames(const smdlVertexFrame& i_Frame1, 
						const smdlVertexFrame& i_Frame2)
	{
		// exact comparison
		//return (i_Frame1.m_Positions == i_Frame2.m_Positions);

		// epsilon comparison
		return (std::equal(i_Frame1.m_Positions.begin(), 
						   i_Frame1.m_Positions.end(), 
						   i_Frame2.m_Positions.begin(), 
						   vec3_equal));
	}

	//========================================================================
	//========================================================================
	void prune_frames(const anKeyDataBase<smdlVertexFrame*>& i_Frames,
					  anKeyDataBase<smdlVertexFrame*>& o_NewFrames)
	{

		const int num_keys = i_Frames.GetNumKeys();
		if (num_keys > 3)
		{
			int num_frames_removed = 0;
			float time1 = 0, time2 = 0, time3 = 0;

			smdlVertexFrame *pPrevFrame = NULL;
			smdlVertexFrame *pCurFrame = NULL;
			smdlVertexFrame *pNextFrame = NULL;
			
			int i=0;
			i_Frames.GetKeyData(i, time1, pPrevFrame);
			if (!pPrevFrame) // frame zero is null
				i_Frames.GetKeyData(++i, time1, pPrevFrame);

			for (; i<num_keys; i++)
			{
				i_Frames.GetKeyData(i, time3, pNextFrame);

				// Try to get 3 frames in a row that are the same.
				// If so, remove the middle frame
				if (pCurFrame == NULL)
				{
					if (compare_frames(*pPrevFrame,*pNextFrame))
					{
						pCurFrame = pNextFrame;	 // 2 frames match now, prev and cur
						time2 = time3;
					}
					else
					{
						// frames did not match, export "prev" frame and
						// start trying to match 3 again
						o_NewFrames.AddKey(time1, pPrevFrame);
						pPrevFrame = pNextFrame; 
						time1 = time3;
					}
				}
				else
				{
					if (compare_frames(*pCurFrame,*pNextFrame))
					{
						// now 3 frames in a row match, we can remove the middle one
						// removing it means to skip over it without adding it to o_NewFrames
						num_frames_removed++;

						// make the frame we just read the new candidate to remove
						pCurFrame = pNextFrame;	 
						time2 = time3;
					}
					else
					{
						// frames did not match, start trying to match 3 again
						o_NewFrames.AddKey(time1, pPrevFrame);
						o_NewFrames.AddKey(time2, pCurFrame);
						pPrevFrame = pNextFrame;
						time1 = time3;
						pCurFrame = NULL;
					}
				}
			}
			if (pPrevFrame)
				o_NewFrames.AddKey(time1, pPrevFrame);
			if (pCurFrame)
				o_NewFrames.AddKey(time2, pCurFrame);

			DBG_LOG2("Removed %d frames of animation out of %d", num_frames_removed, num_keys);
		}
	}

	//========================================================================
	//========================================================================
	void write_vertex_animation(chWriter& o_Writer,
								const std::string& i_SurfaceName,
								const smdlVertexAnimKeys& i_VertexAnim)
	{
		o_Writer.WriteChunkHeader(c_NNAM, 0, false);
		o_Writer.Write(i_SurfaceName);
		o_Writer.FinishChunk();

		const int num_frames = i_VertexAnim->GetFrames().GetNumKeys();
		smdlVertexFrame *pFrame = NULL;
		float time = 0;
		for (int i=0; i<num_frames; i++)
		{
			i_VertexAnim->GetFrames().GetKeyData(i, time, pFrame);

			// Have to skip first NULL frame.
			if (pFrame)
			{
				o_Writer.WriteChunkHeader(c_FRAM, 0, true);

				o_Writer.WriteChunkHeader(c_TIME, 0, false);
				o_Writer.Write(time);
				o_Writer.FinishChunk();

				envType::Int32 nVerts = pFrame->m_Positions.size();
				o_Writer.WriteChunkHeader(c_GVER, 0, false);
				o_Writer.Write(nVerts);
				if (nVerts > 0)
				{
					// Write the whole array at once. 
					o_Writer.Write(&pFrame->m_Positions[0], nVerts * sizeof(maPoint3d));
				}
				o_Writer.FinishChunk();

				nVerts = pFrame->m_Normals.size();
				if (nVerts > 0)
				{
					o_Writer.WriteChunkHeader(c_NVER, 0, false);
					o_Writer.Write(nVerts);
					// Write the whole array at once. 
					o_Writer.Write(&pFrame->m_Normals[0], nVerts * sizeof(maVector3d));
					o_Writer.FinishChunk();
				}

				o_Writer.FinishChunk(); // c_FRAM
			}
		}
	}

	//========================================================================
	//========================================================================
	void process_ACHR(chBinReader& i_Reader, 
					  gfFileBin& i_File,
					  chBinWriter& o_Writer, 
					  gfFileBin& o_File,
					  int &io_AnimCount)
	{

		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;

		// try to get child chunks
		while( i_Reader.ReadChunkHeader(name, version, size) )
		{
			if ( name == c_VTXA )
			{
				DBG_LOG0("Got baked vertex animation");

				// baked vertex animation
				std::string surface_name;
				smdlVertexAnimKeys keys;
				smdlVertexFrames vertex_frames;
				mdlVertexAnimImport::ReadVertexAnimation(i_Reader, version, size, 
									surface_name, keys, vertex_frames);

				// remove unneeded frames
				anKeyDataBase<smdlVertexFrame*> pruned_frames;
				prune_frames(keys->Frames(), pruned_frames);
				keys->Frames() = pruned_frames;

				//Note: some VTXA chunks have AFPS - frame rate chunk in them,
				// but mdlAnimImport does not parse them. It isn't needed for 
				// a character animation, so we won't output it.
				// I think it is a bug in the exporter anyway.

				// Write the VTXA chunk ourselves now
				o_Writer.WriteChunkHeader(name, version, false);

				write_vertex_animation(o_Writer, surface_name, keys);

				o_Writer.FinishChunk();

				//envSTLHelpers::DeleteContainer(vertex_frames);
				io_AnimCount++;
			}
			else
			{
				char *name_str = (char*)(&name);
				DBG_LOG4("Copying: %c%c%c%c", name_str[0], name_str[1], name_str[2], name_str[3]);

				// copy whole size of chunk from input file to
				// 
				std::auto_ptr<char> buffer(new char [size]);

				o_Writer.WriteChunkHeader(name, version, false);
				i_File.Read(size, buffer.get());
				o_File.Write(size, buffer.get());
				o_Writer.FinishChunk();
			}

			i_Reader.FinishChunk();
		}
	}

	//========================================================================
	//========================================================================
	void process_top_level(chBinReader& i_Reader, 
					  gfFileBin& i_File,
					  chBinWriter& o_Writer, 
					  gfFileBin& o_File,
					  int &io_AnimCount)
	{

		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;

		// try to get child chunks
		while( i_Reader.ReadChunkHeader(name, version, size) )
		{
			if ( name == c_ACHR )
			{		
				o_Writer.WriteChunkHeader(c_ACHR, version, true);
				process_ACHR(i_Reader, i_File, o_Writer, o_File, io_AnimCount);
				o_Writer.FinishChunk();
			}
			else if ( name == c_PRUN )
			{		
				// Already pruned this file, don't write the chunk, we will write it again later.
			}
			else
			{
				char *name_str = (char*)(&name);
				DBG_LOG4("Copying: %c%c%c%c", name_str[0], name_str[1], name_str[2], name_str[3]);

				// copy whole size of chunk from input file to output
				// 
				std::auto_ptr<char> buffer(new char [size]);

				o_Writer.WriteChunkHeader(name, version, true);
				i_File.Read(size, buffer.get());
				o_File.Write(size, buffer.get());
				o_Writer.FinishChunk();
			}

			i_Reader.FinishChunk();
		}

		// Add a chunk at the end saying that we have processed this file.
		// Include the version of the tool and the date.
		// Write as strings so readable from bin viewer
		char date_string[64];
		char time_string[64];
		_strdate(date_string);
		_strtime(time_string);
		o_Writer.WriteChunkHeader(c_PRUN, c_PruneAlgorithmVersion, false);
		o_Writer.Write(c_CompressVersionStr);
		o_Writer.Write(date_string);
		o_Writer.Write(time_string);
		o_Writer.FinishChunk();
	}

	//========================================================================
	// Returns true if the file is a character animation file.
	// VersionsMatch returns true if this file has been processed already
	// and the version of the processing tool matches.
	//========================================================================
	bool has_char_animation(const fsLocator &i_Locator,
							 bool &o_VersionsMatch)
	{
		gfFileBin file(i_Locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);
		//	If this isn't a real Terawatt/XLT binary file this will throw
		file.ReadHeader();
		chBinReader reader(file);

		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;

		// try to get child chunks
		bool read_ACHR = false;
		o_VersionsMatch = false;
		while( reader.ReadChunkHeader(name, version, size) )
		{
			if ( name == c_ACHR )
			{		
				read_ACHR = true;
			}
			else if ( name == c_PRUN )
			{		
				// Already pruned this file, check if versions match
				o_VersionsMatch = (version == c_PruneAlgorithmVersion);
			}

			// skip over other chunks

			reader.FinishChunk();
		}
		return read_ACHR;
	}

	//========================================================================
	// Write - copies chunks from one file to another, but changes
	//		materials based on passed list.
	//========================================================================
	bool	CompressAnim( const fsLocator &i_NewLocator,
						  const fsLocator &i_OldLocator )
	{
		// First of all, determine if this is a character animation file
		// that we can compress.
		bool bVersionsMatch = false;
		if (!has_char_animation(i_OldLocator, bVersionsMatch))
		{
			DBG_WARNING0("File is not a character animation file.");
			std::cerr << "File is not a character animation file." << std::endl;
			return false;
		}

		// Determine if old and new files are the same.
		// Convert to ascii to do case-independent comparison
		std::string old_fname, new_fname;
		fsFileUtil::LocatorToANSIFilename(i_OldLocator, old_fname);
		fsFileUtil::LocatorToANSIFilename(i_NewLocator, new_fname);
		bool bSame = !::_stricmp(old_fname.c_str(), new_fname.c_str());

		fsLocator input_locator = i_OldLocator;
		if (bSame)
		{
			// If we the file has already been processed, then we don't have to 
			// do it again.
			if (bVersionsMatch)
			{
				DBG_WARNING0("File has already been processed, not compressing again.");
				std::cerr << "File has already been processed, not compressing again." << std::endl;
				return true;
			}

			// Need to check for read-only first, otherwise
			// our temporary file will become read-only
			if (fsFileUtil::IsReadOnly(i_OldLocator))
				throw fsReadOnlyX(i_OldLocator);

			// Copy old locator to temporary location in order to
			// have backup in case we mess up file, or to make
			// sure we don't try to overwrite file we are reading
			fsLocator temp_file = gfPaths::GetPath(gfPaths::e_ExePath);
			temp_file.Push(itString("anim.bak"));
			if (i_NewLocator == temp_file)
				return false;	// can't save to our temp file name
			
			if( fsFileUtil::FileExists(temp_file) )
				fsFileUtil::DeleteFile(temp_file);
			fsFileUtil::CopyFile(i_OldLocator, temp_file);

			input_locator = temp_file;
		}


		// Set up input and output files
		gfFileBin::Header header;
		gfFileBin ifile(input_locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);
		ifile.ReadHeader(&header);
		chBinReader reader(ifile);

		// Create new locator
		if( fsFileUtil::FileExists(i_NewLocator) )
			fsFileUtil::DeleteFile(i_NewLocator);
		fsFileUtil::CreateFile(i_NewLocator);

		gfFileBin ofile(i_NewLocator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
		ofile.WriteHeader(header);
		chBinWriter writer(ofile);

		int anim_counter = 0;
		process_top_level(reader, ifile, writer, ofile, anim_counter);

		if (anim_counter == 0)
		{
			DBG_WARNING0("No vertex animations found.");
		}
		std::cout << "Found " << anim_counter << " animations." << std::endl;
		return true;
	}

}


int main(int argc, char** argv)
{
	gfPackage::Init();
	CoreLayer::Init();

	if (argc == 1 || argc > 3)
	{
		std::cout << argv[0] << " version " << c_CompressVersionStr << std::endl;
		std::cout << "Syntax: " << c_ExeName << " filename.chx [output_filename.chx]" << std::endl;
		std::cout << "   If used with a single argument, compresses the file in place" << std::endl;
		std::cout << "   and puts a backup file, anim.bak, in the exe directory." << std::endl;
	}
	else
	{
		fsLocator anim_file, out_file;
		fsFileUtil::ANSIFilenameToLocator(argv[1], anim_file);
		if (argc == 3)
		{
			fsFileUtil::ANSIFilenameToLocator(argv[2], out_file);
			std::cout << "Compressing file: " << argv[1] << " to file " << argv[2] << std::endl;
			DBG_WARNING2("Compressing file: %s to file %s", argv[1], argv[2]);
		}
		else 
		{
			out_file = anim_file;
			std::cout << "Running " << argv[0] << " on file " << argv[1] << std::endl;
			DBG_WARNING2("Running %s on file: %s", argv[0], argv[1]);
		}

		try
		{
			if (CompressAnim( out_file, anim_file  ))
			{
				std::cout << "File processed successfully." << std::endl;
			}
			else
			{
				std::cout << "File could not be compressed." << std::endl;
			}
		}
		catch (fsReadOnlyX& i_Ex)
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
			std::cerr << "File is read only: " << filename << std::endl;
			DBG_ERROR1("File is read only: %s", filename.c_str());
		}
		catch (fsFileDoesntExistX& i_Ex)
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
			std::cerr << "File doesn't exist: " << filename << std::endl;
			DBG_ERROR1("File doesn't exist: %s", filename.c_str());
		}
		catch (fsInvalidLocatorX& i_Ex)
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
			std::cerr << "File doesn't exist: " << filename << std::endl;
			DBG_ERROR1("File doesn't exist: %s", filename.c_str());
		}
		catch (gfInvalidFileBinX& i_Ex)
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
			std::cerr << "File is not character animation file: " << filename << std::endl;
			DBG_ERROR1("File is not character animation file: %s", filename.c_str());
		}
		catch (chInvalidChunkX& )
		{
			std::cerr << "File format error in chunk parsing." << std::endl;
			DBG_ERROR0("File format error in chunk parsing.");
		}
		catch (mdlInvalidModelFileX& i_Ex)
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
			std::cerr << "File format error in file: " << filename << std::endl;
			DBG_ERROR1("File format error in file: %s", filename.c_str());
		}
		catch (...)
		{
			std::cerr << "Unknown error while processing." << std::endl;
			DBG_ERROR0("Unknown error while processing.");
		}
	}

	CoreLayer::CleanUp();
	gfPackage::CleanUp();

	return 0;
}
