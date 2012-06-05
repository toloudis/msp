/****************************************************************************\
**	mdlWriter.hpp
**
**		mdlWriter.hpp supplies functions used to export geometry data into
**	our geometry file formats.
**
**	StudioGPU
**	Copyright(C) 2003-8 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_WRITER_HPP
#error mdlWriter.hpp multiply included
#endif
#define MDL_WRITER_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef MDL_MATINFOTABLE_HPP
#include "Graphics/mdl/mdlMatInfoTable.hpp"
#endif 

#include <map>
#include <vector>


//============================================================================
//	Forward References
//============================================================================
class chWriter;
class fsLocator;
class matMaterial;
class mdlFragInfo;
class mdlNodeInfo;
class mdlSubdivInfo;
struct mdlModelingPackageData;


//============================================================================
//	Any of these mdlWriter functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//============================================================================
namespace mdlWriter
{
	//----------------------------------------------------------------------------
	//	Writes fragments to static geometry file (.mx)
	//----------------------------------------------------------------------------
	void WriteWorldFragments(const fsLocator& i_Locator,
							const std::vector< shared_ptr<mdlFragInfo> >& i_GeoData,
							const mdlMatInfoTable& i_MaterialTable );

	//------------------------------------------------------------------------
	//	WriteExporterVersionStamp - write version, date, time
	//	to chunk writer
	//------------------------------------------------------------------------
	void WriteExporterVersionStamp(chWriter &o_Writer,
								   const std::string &i_VersionString);

	//------------------------------------------------------------------------
	//	WriteModelPackageVersionStamp - write version, date, time
	//	to chunk writer
	//------------------------------------------------------------------------
	void WriteModelPackageVersionStamp(	chWriter &o_Writer,
										const mdlModelingPackageData& o_Data);

	//------------------------------------------------------------------------
	// WriteHierarchicalModel writes a a tree data structure of nodes and 
	// meshes into a generalized hierarchical model file (.gxb)
	//------------------------------------------------------------------------
	void WriteHierarchicalModel(const fsLocator& i_Locator,
								const shared_ptr<mdlNodeInfo>& i_NodeGraph,
								const mdlMatInfoTable& i_MaterialTable,
								const std::string &i_VersionString);
	void WriteHierarchicalModel(chWriter &o_Writer,
								const shared_ptr<mdlNodeInfo>& i_NodeGraph,
								const mdlMatInfoTable& i_MaterialTable);
}

