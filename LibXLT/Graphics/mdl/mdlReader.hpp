/****************************************************************************\
**	mdlReader.hpp
**
**		mdlReader.hpp supplies functions used to import files from Maya
**	(written by our Maya plugin).
**
**		When refactored correctly, mdlReader will specialize in reading
**	files into data structures for processing and mayImport will specialize 
**	in reading files into our graphics objects in order to be rendered.
**
**	StudioGPU
**	Copyright(C) 2003-8 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_READER_HPP
#error mdlReader.hpp multiply included
#endif
#define MDL_READER_HPP

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
class chReader;
class fsLocator;
class matMaterial;
class mdlFragInfo;
class mdlNodeInfo;
class mdlSubdivInfo;


//============================================================================
//	Any of these mdlReader functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//============================================================================
namespace mdlReader
{
	//----------------------------------------------------------------------------
	// Return mutex to use when reading more than one geoetry file at a time.
	//----------------------------------------------------------------------------
	//envMutex& GetReaderMutex();

	//----------------------------------------------------------------------------
	// Some apps may want to override what ambient value is in the
	// model file and force all ambient colors to full white
	//----------------------------------------------------------------------------
	void SetAlwaysFullAmbient(bool i_bVal);
	bool IsAlwaysFullAmbient();

	//----------------------------------------------------------------------------
	// Set the loader to skip the loading of low resolution or high resolution
	//	models. Can save memory when it is known a certain version isn't needed.
	//----------------------------------------------------------------------------
	void SetSkipLowRes(bool i_bSkip);
	void SetSkipHighRes(bool i_bSkip);

	//----------------------------------------------------------------------------
	//	Reads fragments from static geometry file (.mx) and returns
	//	fragments and materials read as data structures.
	//----------------------------------------------------------------------------
	void ReadWorldFragments(const fsLocator& i_Locator,
							std::vector< shared_ptr<mdlFragInfo> >& o_GeoData,
							mdlMatInfoTable& o_MaterialTable );

	//------------------------------------------------------------------------
	// ReadHierarchicalModel loads a hierarchical model from a file (.mhx)
	// into a tree data structure of nodes and meshes.
	//------------------------------------------------------------------------
	void ReadHierarchicalModel(	const fsLocator& i_Locator,
								shared_ptr<mdlNodeInfo>& o_NodeGraph,
								mdlMatInfoTable& o_MaterialTable);

	//----------------------------------------------------------------------------
	//	SetVerboseMode will cause a bunch of debugging information and statistics
	//	about whatever meshes are being loaded to be written to the debug log.
	//----------------------------------------------------------------------------
	void SetVerboseMode(bool i_Verbose);
}

