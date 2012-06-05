/*****************************************************************************
**	cmprZipUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "Support/cmpr/private/cmprZipUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include <zlib\contrib\minizip\zip.h>

#define WRITEBUFFERSIZE (16384)

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace cmprZipUtil
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	namespace
	{
		bool l_bFirstZip;
		zipFile l_ZipFile;
		
		//--------------------------------------------------------------------
		//open the zip file using the minizip functions
		//--------------------------------------------------------------------
		void open_zip_file(std::string i_Path)
		{
			//if this is the first open operation, create the zip file
			//if it is not the first make sure the zip file is opened to allow
			//other files to be added to it
			if (l_bFirstZip)
			{
				l_ZipFile = zipOpen(i_Path.c_str(), APPEND_STATUS_CREATE);
				l_bFirstZip = false;
			}
			else
			{
				l_ZipFile = zipOpen(i_Path.c_str(), APPEND_STATUS_ADDINZIP);
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void increment_destination(fsLocator& io_DestLocator)
		{
			fsLocator new_dest = io_DestLocator;
			itString dest_file = io_DestLocator.GetLastName();
			itString ext;
			dest_file.GetExtension(ext);
			dest_file.StripExtension();
			
			if(!dest_file.HasSubString(itString("_part")))
			{
				std::string dest_string = itStringUtil::GetStdString(dest_file);
				dest_string += "_part1";
				itString dest(dest_string.c_str());
				dest_file = dest;
				new_dest.ReplaceLastName(dest);
				new_dest.ReplaceExtension(ext);
				if(fsFileUtil::FileExists(new_dest))
					fsFileUtil::DeleteFile(new_dest);
				fsFileUtil::RenameFile(io_DestLocator, new_dest);
			}
			//get the current part number
			int length = dest_file.GetLength();
			itString::CharType counter_char = dest_file[length - 1];
			int counter = _ttoi(&counter_char);
			counter++;
			::_itot(counter, &counter_char, 10);
			dest_file[length - 1] = counter_char;
			new_dest.ReplaceLastName(dest_file);
			new_dest.ReplaceExtension(ext);
			if(fsFileUtil::FileExists(new_dest))
				fsFileUtil::DeleteFile(new_dest);
			fsFileUtil::CreateFile(new_dest);
			io_DestLocator = new_dest;
			l_bFirstZip = true;
		}

	} // end anonymous namespace

	//------------------------------------------------------------------------
	// Do the necessary preperations before compressing a file.  
	// Based on current compression type
	//------------------------------------------------------------------------
	void PrepareZip()
	{
		l_bFirstZip = true;
	}

	//------------------------------------------------------------------------
	// Do the file compression
	//------------------------------------------------------------------------
	bool ZipFile( const fsLocator& i_SourceFile, fsLocator& io_DestPath )
	{
		std::string src_folder_string, dest_zip_path, file_string;
		//get string values for the source and destination locations, also the file itself
		fsFileUtil::LocatorToANSIFilename(io_DestPath, dest_zip_path, true);
		fsFileUtil::LocatorToANSIFilename(i_SourceFile, src_folder_string);

		file_string = itStringUtil::GetStdString(i_SourceFile.GetLastName());

		/*
			We need to pop up a warning if the file path has any unicode characters in it because 
			zlib, v1.2.3, only accepts const char* as the filename to create the zip file.
		*/
		if(!fsFileUtil::IsFilenameASCII(i_SourceFile))
		{
			//ask if the user wants to continue exporting the scene
			std::string unicode_file("Unable to zip files with Unicode characters in its path; ");
			unicode_file += src_folder_string.c_str();
			unicode_file += " cannot be zipped.  Continue exporting next asset?";
			int retval = guiMessageBox::Show(unicode_file.c_str(), 
											 "Unicode File Path", guiMessageBox::e_YesNo);
			bool decision = true;
			if ( retval == guiMessageBox::e_No )
				decision = false;
			return decision;
		}

		if(!fsFileUtil::IsFilenameASCII(io_DestPath))
		{
			//ask if the user wants to continue exporting the scene
			std::string unicode_file("Unable to create a zip file on a Unicode file path; ");
			unicode_file += dest_zip_path.c_str();
			unicode_file += " cannot be created";
			int retval = guiMessageBox::Show(unicode_file.c_str(), 
											"Unicode Zip Path", guiMessageBox::e_OK);
			return false;  //quit package
		}

		//open the zip file object, there are 2 ways to open it depending on whether the archive is empty or not
		open_zip_file(dest_zip_path);

		//set the zip file info
		zip_fileinfo zi;
        int opt_compress_level=Z_BEST_COMPRESSION;
        //Best compression to save bandwidth at a maximum.
		
		zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour = 
        zi.tmz_date.tm_mday = zi.tmz_date.tm_min = zi.tmz_date.tm_year = 0;
        zi.dosDate = 0;
        zi.internal_fa = 0;
        zi.external_fa = 0;
		
		int zipErr;
		FILE * fin;
		void* buf=NULL;
        int size_buf=0;
        size_buf = WRITEBUFFERSIZE;
        buf = (void*)malloc(size_buf);
		int size_read;
		
		if(!fsFileUtil::FileExists(i_SourceFile))
		{
			//ask if the user wants to continue exporting the scene
			std::string file_missing("File: ");
			file_missing += src_folder_string.c_str();
			file_missing += " was not found and will not be added to the archive.  Continue?";
			int retval = guiMessageBox::Show(file_missing.c_str(), 
											 "File Missing", guiMessageBox::e_YesNo);
			bool decision = true;
			if ( retval == guiMessageBox::e_No )
				decision = false;
			free (buf);
			//close the zip file instance
			zipErr = zipClose(l_ZipFile, NULL);
			return decision;
		}
		//initialize the new file name in the zip archive
		zipErr = zipOpenNewFileInZip(l_ZipFile, file_string.c_str(), 
									&zi,NULL,0,NULL,0,NULL,
									(opt_compress_level != 0) ? Z_DEFLATED : 0,opt_compress_level);

		if (zipErr < 0)
        {
			if (zipErr == ZIP_PARAMERROR)
			{
				free (buf);
				//close the zip file instance
				zipErr = zipClose(l_ZipFile, NULL);
				DBG_LOG("Zip Archive: " << io_DestPath << " has reached its capacity limit.  Creating a new archive to finish the export.");
				increment_destination(io_DestPath);
				return ZipFile(i_SourceFile, io_DestPath);
			}
			else
			{
				DBG_ERROR("Unable to add file: " << file_string << " to archive");
				free (buf);
				//close the zip file instance
				zipErr = zipClose(l_ZipFile, NULL);
				return false;
			}
        }

		//open the source file for reading
		fin = fopen(src_folder_string.c_str(), "rb");
		if (fin==NULL)
        {
			zipErr = ZIP_ERRNO;
			DBG_ERROR("error in opening export file: " << src_folder_string);
			free (buf);
			//close the zip file instance
			zipErr = zipClose(l_ZipFile, NULL);
			//return true so that we move to the next file instead of stopping the entire export process
			return true;
        }

		//until we reach the end of the file, read it into the zip archive
		while (zipErr == ZIP_OK)
		{
			zipErr = ZIP_OK;
            size_read = fread(buf,1,size_buf,fin);

            if (zipErr == ZIP_OK && size_read > 0)
            {
                zipErr = zipWriteInFileInZip (l_ZipFile,buf,size_read);
                if (zipErr < 0)
                {
					DBG_ERROR("Error when writing to zip file during export");
					fclose(fin);
					free (buf);
					//close the zip file instance
					zipErr = zipClose(l_ZipFile, NULL);
					return false;
                }
            }

			//if we reach the end of the file, stop reading
			else if (size_read <= 0)
			{
				break;
			}
		}
		fclose(fin);
        free (buf);
		//close the zip file instance
		zipErr = zipClose(l_ZipFile, NULL);
		
		return true;
	}


} // end cmprZipUtil namespace