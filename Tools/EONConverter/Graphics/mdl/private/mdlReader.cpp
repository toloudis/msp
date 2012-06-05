/****************************************************************************\
**  mdlReader.cpp
**
**      mdlReader.hpp supplies functions used to import files from Maya
**	(written by our Maya plugin).
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/mdlReader.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/private/mdlDefs.hpp"
//#include "Graphics/mdl/private/mdlHierarchyNodeInfoParser.hpp"
//#include "Graphics/mdl/private/mdlMaterialParser.hpp"
//#include "Graphics/mdl/private/mdlMaterialLegacyParser.hpp"
#include "Graphics/mdl/private/mdlMeshParser.hpp"

#include "Core/ch/chBinReader.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/gf/gfFileBin.hpp"
//#include "Graphics/mat/matMaterial.hpp"

#include <algorithm>

//----------------------------------------------------------------------------
//	Any of these mdlReader functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//----------------------------------------------------------------------------
namespace mdlReader
{
	namespace
	{
		const chDefs::Name c_GFRG = chDefs::MakeName('G', 'F', 'R', 'G');
		const chDefs::Name c_MTBL = chDefs::MakeName('M', 'T', 'B', 'L');

		// Mutex for synchronizing threads so that
		// only one thread is creating materials at the same time.
		//envMutex l_Mutex;
	}

	//----------------------------------------------------------------------------
	// Return mutex to use when reading more than one geoetry file at a time.
	//----------------------------------------------------------------------------
	//envMutex& GetReaderMutex()
	//{
	//	return l_Mutex;
	//}

	//------------------------------------------------------------------------
	// Some apps may want to override what ambient value is in the
	// model file and force all ambient colors to full white
	//------------------------------------------------------------------------
	//void SetAlwaysFullAmbient(bool i_bVal)
	//{
	//	mdlMaterialLegacyParser::SetAlwaysFullAmbient(i_bVal);
	//}
	//bool IsAlwaysFullAmbient()
	//{
	//	return mdlMaterialLegacyParser::IsAlwaysFullAmbient();
	//}

	//----------------------------------------------------------------------------
	// Set the loader to skip the loading of low resolution or high resolution
	//	models. Can save memory when it is known a certain version isn't needed.
	//----------------------------------------------------------------------------
	void SetSkipLowRes(bool i_bSkip)
	{
		mdlMeshParser::SetSkipLowRes(i_bSkip);
	}
	void SetSkipHighRes(bool i_bSkip)
	{
		mdlMeshParser::SetSkipHighRes(i_bSkip);
	}

	//----------------------------------------------------------------------------
	//	Reads fragments from static geometry file (.mx) and returns
	//	fragments and materials read.
	//----------------------------------------------------------------------------
	void ReadWorldFragments(const fsLocator& i_Locator,
							std::vector< shared_ptr<mdlFragInfo> >& o_GeoData
							/*,	mdlMatInfoTable& o_MaterialTable*/ )
	{
		if( mdlDefs::GetVerboseMode() )
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Locator, filename);
			dbgLog::Write("");
			dbgLog::Write("mdlReader::ReadWorldFragments loading %s", filename.c_str());
		}

		gfFileBin file(i_Locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);

		//	If this isn't a real Terawatt/XLT binary file this will throw
		file.ReadHeader();

		chBinReader reader(file);

		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;

		try
		{
			while( reader.ReadChunkHeader(name, version, size) )
			{
				if (name == c_GFRG)
				{
					shared_ptr<mdlFragInfo> new_frag_info(new mdlFragInfo);
					mdlMeshParser::ReadGFRG(	reader,
								version,
								size,
								*new_frag_info,
								false); //,
								//&o_MaterialTable );

					o_GeoData.push_back(new_frag_info);
				}
				//else if (name == c_MTBL)
				//{
				//	// material table has shared materials for
				//	// fragments, has to be read before GFRG chunks
				//	mdlMaterialParser::ReadMaterialTable(  reader,
				//						version,
				//						size,
				//						o_MaterialTable);
				//}
				reader.FinishChunk();
			}
		}
		catch ( const chInvalidChunkX& i_Ex )
		{
			i_Ex;
			DBG_LOG0("Invalid chunk in mdlImport::LoadWorldFragments");
			throw mdlInvalidModelFileX(i_Locator);
		}
	}

	//------------------------------------------------------------------------
	// ReadHierarchicalModel loads a hierarchical model from a file
	// into a tree data structure of nodes and meshes.
	//------------------------------------------------------------------------
	//void ReadHierarchicalModel(	const fsLocator& i_Locator,
	//							shared_ptr<mdlNodeInfo>& o_NodeGraph,
	//							mdlMatInfoTable& o_MaterialTable)
	//{
	//	mdlHierarchyNodeInfoParser parser(i_Locator, o_MaterialTable);
	//	mdlHierarchyParser::LoadModel(i_Locator, parser);
	//	o_NodeGraph = parser.GetRootNode();
	//}

	//----------------------------------------------------------------------------
	//	SetVerboseMode will cause a bunch of debugging information and statistics
	//	about whatever meshes are being loaded to be written to the debug log.
	//----------------------------------------------------------------------------
	void SetVerboseMode(bool i_Verbose)
	{
		mdlDefs::SetVerboseMode(i_Verbose);
	}
}

