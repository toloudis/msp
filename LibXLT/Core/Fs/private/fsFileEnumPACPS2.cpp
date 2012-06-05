/****************************************************************************\
**  fsFileEnumPACPS2.cpp
**
**      fsFileEnumPACPS2.cpp supplies functions that enumerate files and
**	directories.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "fsFileEnumPAC.hpp"

#include "fsFileUtilPAC.hpp"
#include "fsFileX.hpp"
#include "itStringUtil.hpp"

#include <libcdvd.h>
#include <sifdev.h>


namespace fsFileEnumPAC
{

	//========================================================================
	//	EnumerateFiles lists all of the files in a directory.  If i_SubString
	//	is not NULL, only files whose names contain the given substring
	//	will be listed. Returns the last return value from fsEnumTarget::Notify
	//========================================================================
	bool EnumerateFiles(const fsLocator& i_Locator, 
						fsFileEnum::EnumTarget& o_Target,
						const itString* i_SubString)
	{
		int i;
		fsLocator locator;
		bool ret_val = true;

		if ( i_Locator.GetName(0) == itString("cdrom0:") )
		{
			std::vector<itString> file_list;
			fsFileUtilPAC::GetCDDirectoryFileInfo( i_Locator, file_list );
			int file_num = file_list.size();
			for ( i = 0; i < file_num; i++ )
			{
				// convert to lower case because the substring is in lower case
				itString file_name = itStringUtil::ToLower( file_list[i] );

//				std::string name = itStringUtil::GetStdString( file_name );
//				std::string name1 = itStringUtil::GetStdString( *i_SubString );

				if( (i_SubString == NULL) || (file_name.HasSubString(*i_SubString)) )
				{
					locator.Clear();
					
					//	remove ";1"
					int len =  file_name.GetLength();
					if( len > 2 && file_name.HasSubString( itString(";1") ) )
					{
						file_name.SetLength( len - 2 );
					}
					
					locator.Push( file_name );
					if( o_Target.Notify( i_Locator, locator ) == false )
					{
						ret_val = false;
						break;
					}
				}
			}

		}
		else
		{
			std::string directory_name;
			fsFileUtilPAC::LocatorToANSIFilename(i_Locator, directory_name, true);
			
			int directory_handle = sceDopen(directory_name.c_str());
			if( directory_handle < 0 )
				throw fsDirectoryDoesntExistX(i_Locator);
			
			sce_dirent directory_info;
			
			while( sceDread(directory_handle, &directory_info) )
			{	
				if( (directory_info.d_stat.st_mode & (1 << 13)) == 0 )	//	13th bit of mode is File Type Normal File
					continue;
				
				if( directory_info.d_name[0] == '.' )
					continue;
					
				itString file_itstring(directory_info.d_name);
				if( (i_SubString == NULL) || (file_itstring.HasSubString(*i_SubString)) )
				{
					locator.Clear();
					
					//	remove ";1"
					int len =  file_itstring.GetLength();
					if( len > 2 && directory_info.d_name[len - 2] == ';' )
					{
						file_itstring.SetLength( len - 2 );
					}
					
					locator.Push(file_itstring);
					if( o_Target.Notify(i_Locator, locator) == false )
					{
						ret_val = false;
						break;
					}
				}
			}
			
			sceDclose(directory_handle);
		}

		return ret_val;
	}

	//========================================================================
	//	EnumerateDirectories lists all of the subdirectories in a directory.
	//	Returns the last return value from fsEnumTarget::Notify
	//========================================================================
	bool EnumerateDirectories(	const fsLocator& i_Locator, 
								fsFileEnum::EnumTarget& o_Target)
	{
		fsLocator locator;
		
		std::string directory_name;
		fsFileUtilPAC::LocatorToANSIFilename(i_Locator, directory_name, true);
		int directory_handle = sceDopen(directory_name.c_str());
		if( directory_handle < 0 )
			throw fsDirectoryDoesntExistX(i_Locator);

		sce_dirent directory_info;
		bool ret_val = true;
		
		while( sceDread(directory_handle, &directory_info) )
		{	
			if( (directory_info.d_stat.st_mode & (1 << 12)) == 0 )	//	12th bit of mode is File Type Directory
				continue;
			
			if( directory_info.d_name[0] == '.' )
				continue;
				
			locator.Clear();
			itString file_itstring(directory_info.d_name);
			locator.Push(file_itstring);
			if( o_Target.Notify(i_Locator, locator) == false )
			{
				ret_val = false;
				break;
			}
		}
		
		sceDclose(directory_handle);
		
		return ret_val;
	}

	//========================================================================
	//	GetDrives lists all of the drives listed by windows
	//========================================================================
	void GetDrives(std::vector<fsLocator>& o_DirList)
	{
		fsLocator cdrom;
		cdrom.Push("cdrom0:");
		o_DirList.push_back(cdrom);
	}
}
