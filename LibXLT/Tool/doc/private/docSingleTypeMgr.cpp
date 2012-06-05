/*****************************************************************************
**	docSingleTypeMgr.cpp
**
**		SingleTypeMgr handles the details of the single type
**	used by a single type/single document application
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Tool/doc/docSingleTypeMgr.hpp"

#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Gf/gfPaths.hpp"
#include "docDocumentType.hpp"
#include "Tool/doc/docDocumentInterest.hpp"
#include "Tool/doc/docDocumentWithChunks.hpp"
#include "Tool/gui/guiXMLTextReader.hpp"
#include "Tool/gui/guiXMLTextWriter.hpp"

#include <list>


//============================================================================
//============================================================================
namespace docSingleTypeMgr
{
	namespace
	{
		const char * lc_MRU_Group = "MostRecentlyUsed";
		const char * lc_MRU_Entry = "Entry";

		fsLocator l_MRUFile(itString("RecentFiles.cfg"));
		std::string l_Filter;
		docDocumentType l_DocumentType;
		std::list<fsLocator> l_MRUDocs;

		int l_MaxFileList = 10;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void read_from_XML()
		{
			std::string cfgpath;
			fsFileUtil::LocatorToANSIFilename(l_MRUFile,cfgpath);

			guiXMLTextReader::Open(cfgpath.c_str());

			//	read in the preferences
			//
			guiXMLTextReader::gui_Node_Type node_type;
			std::string keyname, strvalue;
			while ((node_type = guiXMLTextReader::ReadNode(keyname,strvalue)) != guiXMLTextReader::e_EOF)
			{
				if ((node_type == guiXMLTextReader::e_Text) && (keyname.length() > 0))
				{
					if (keyname == lc_MRU_Entry)	
					{
						fsLocator value; 
						guiXMLTextReader::Convert(strvalue, value);
						l_MRUDocs.push_back(value);
						if (l_MRUDocs.size() == l_MaxFileList)
							break;
					};
				}
			}

			guiXMLTextReader::Close();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void write_to_XML()
		{
			std::string cfgpath;
			fsFileUtil::LocatorToANSIFilename(l_MRUFile,cfgpath);

			//	if the path doesn't exist, create it
			if (!fsFileUtil::FileExists(l_MRUFile))
			{
				fsLocator newdir(l_MRUFile);
				if (newdir.GetNumNames() > 1)
					newdir.Pop();
				else
				{
					if (gfPaths::IsPathValid(gfPaths::e_AppPath))
						newdir = gfPaths::GetPath(gfPaths::e_AppPath);
					else
						newdir = gfPaths::GetPath(gfPaths::e_ExePath);
				}
				fsFileUtil::CreateDirectory(newdir);
			}

			//	open and start the XML file
			guiXMLTextWriter::Open(cfgpath.c_str());
			guiXMLTextWriter::WriteStartElement(lc_MRU_Group);

			int ind = 0;
			std::list<fsLocator>::iterator it, end = l_MRUDocs.end();
			for (it = l_MRUDocs.begin(); it != end; ++it)
			{
				if (ind >= l_MaxFileList) break;

				ind++;

				//DBG_TRACE("    entry " << *it);

				//	write the actual values
				std::string filepath;
				fsFileUtil::LocatorToANSIFilename(*it, filepath);
				guiXMLTextWriter::WriteElement(lc_MRU_Entry, filepath);
			}

			//	finish it up
			guiXMLTextWriter::WriteEndElement();
			guiXMLTextWriter::Close();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void readMRU()
		{
			DBG_TRACE("Reading MRU file " << l_MRUFile);

			//	TODO - remove this transitional test to keep users MRU lists
			//
			if (fsFileUtil::FileExists(l_MRUFile))
			{
				read_from_XML();
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void writeMRU()
		{
			write_to_XML();
		}

	}	// end of namespace


	//--------------------------------------------------------------------
	// Gets MRU
	//--------------------------------------------------------------------
	void Init( int i_MRUMaxEntries )
	{
		l_MaxFileList = i_MRUMaxEntries;

		readMRU();
	}

	//--------------------------------------------------------------------
	// Removes MRU
	//--------------------------------------------------------------------
	void Close()
	{
		//delete all interests
		envSTLHelpers::DeleteContainer( l_DocumentType.m_Interests );
	}

	//--------------------------------------------------------------------
	//  Set filter for filenames based on the extension and a
	// string description of the file type.  The extension should
	// include the period.
	//--------------------------------------------------------------------
	void  SetFilter(const std::string &i_Ext, const std::string &i_Description)
	{
		std::string extension = i_Ext;
		std::string descrip = i_Description;

		std::string filter = "";
		std::string ext, des, temp;

		// Look for "|" in ext and description in order to have multiple filters
		int ext_off = 0, des_off = 0;
		int ext_find = extension.find('|');
		int des_find = descrip.find('|');
		while (ext_find >= 0)
		{
			ext = extension.substr(0, ext_find);
			des = descrip.substr(0, des_find);

			if (ext[0] != '.') ext = std::string(".") + ext;
			filter += (des + "(*" + ext + ")|*" + ext + "|");

			// shorten base strings
			extension = extension.substr(ext_find+1);
			descrip = descrip.substr(des_find+1);

			ext_find = extension.find('|');
			des_find = descrip.find('|');
		}


		ext = extension;
		if (ext[0] != '.') ext = std::string(".") + ext;

		// Create filter format like:
		// "My Files (*.ext)|*.ext|All Files (*.*)|*.*"
		l_Filter = filter + descrip + "(*" + ext + ")|*" + ext + "|All Files (*.*)|*.*";

	}

	//--------------------------------------------------------------------
	//  Get filter in format for Open and Save FileDialogs
	//--------------------------------------------------------------------
	std::string  GetFilter()
	{
		return l_Filter;
	}


	//--------------------------------------------------------------------
	//  Add interest in the document type.
	// AddDocumentInterest - adds interest in order,
	// PrependDocumentInterest - puts document interest first in list
	//--------------------------------------------------------------------
	void  AddDocumentInterest(docDocumentInterest *i_Interest)
	{
		l_DocumentType.m_Interests.push_back(i_Interest);
	}
	void  PrependDocumentInterest(docDocumentInterest *i_Interest)
	{
		l_DocumentType.m_Interests.insert(l_DocumentType.m_Interests.begin(), i_Interest);
	}

	//--------------------------------------------------------------------
	//  Create document holding instances of document chunks for
	// each interest registered
	//--------------------------------------------------------------------
	docDocument * CreateDocument(int i_WriteFormat)
	{
		docDocumentWithChunks* doc = new docDocumentWithChunks( i_WriteFormat );
		int num = l_DocumentType.m_Interests.size();
		DBG_ASSERT(num>0, "No document interests registered.");
		for (int i=0; i<num; i++)
		{
			docDocumentInterest *interest = l_DocumentType.m_Interests[i];
			doc->AddDocumentChunk( interest->CreateDocumentChunk() );
		}
		return doc;
	}

	//--------------------------------------------------------------------
	//	send the "root" directory for this application to each
	//	doc interest.
	//--------------------------------------------------------------------
	void SetAppDirectory( const fsLocator& i_Locator )
	{
		docDocumentInterest *interest;

		int num = l_DocumentType.m_Interests.size();
		for (int i=0; i<num; i++)
		{
			interest = l_DocumentType.m_Interests[i];
			interest->SetAppDirectory( i_Locator );
		}
	}

	//--------------------------------------------------------------------
	//  Add the given filename to the MRU list
	//--------------------------------------------------------------------
	void  AddToMRU(const fsLocator &i_Locator)
	{
		// if this locator is in the list, remove it and add to front
		// otherwise just add to front
		envSTLHelpers::RemoveAllValues(l_MRUDocs,i_Locator);

		l_MRUDocs.push_front(i_Locator);

		 writeMRU();
	}

	//--------------------------------------------------------------------
	//  Get vector of MRU filenames
	//--------------------------------------------------------------------
	void  GetMRUList(std::vector<fsLocator> &o_Locators)
	{
		std::list<fsLocator>::const_iterator it = l_MRUDocs.begin();
		for (; it != l_MRUDocs.end(); ++it)
			o_Locators.push_back(*it);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetMRUFile(const fsLocator& i_File)
	{
		//DBG_TRACE("Setting MRU file " << i_File );

		l_MRUFile = i_File;
	}

}	// end of namespace
