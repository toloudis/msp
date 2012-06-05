/****************************************************************************\
**  mdlWriter.cpp
**
**      mdlWriter.hpp supplies functions used to export geometry data into
**	our geometry file formats.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/mdlWriter.hpp"

#include "Core/ch/chBinWriter.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/Ma/maConstants.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlModelingPackageData.hpp"
#include "Graphics/mdl/mdlNodeInfo.hpp"
#include "Graphics/mdl/private/mdlDefs.hpp"
#include "Graphics/mdl/private/mdlHairParser.hpp"
#include "Graphics/mdl/private/mdlHierarchyParser.hpp"
#include "Graphics/mdl/private/mdlMeshParser.hpp"
#include "Graphics/mdl/private/mdlModelingPackageWriter.hpp"
#include "Graphics/mdl/private/mdlSkinParser.hpp"
#include "Graphics/mtr/mtrMaterialSaver.hpp"

#include <algorithm>
#include <time.h>


//============================================================================
//	Any of these mdlWriter functions might throw one of the fs exceptions.
//============================================================================
namespace mdlWriter
{
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	namespace
	{
		const chDefs::Name c_GFRG = chDefs::MakeName('G', 'F', 'R', 'G');
		const chDefs::Name c_MTBL = chDefs::MakeName('M', 'T', 'B', 'L');
		const chDefs::Name c_GHIE = chDefs::MakeName('G', 'H', 'I', 'E');
		const chDefs::Name c_EXPV = chDefs::MakeName('E', 'X', 'P', 'V');

		//----------------------------------------------------------------------------
		// Write material table to file. Surfaces later refer to the materials
		//	by using the material name.
		//----------------------------------------------------------------------------
		void write_material_table(chWriter& o_Writer,
								  const mdlMatInfoTable& i_MaterialTable)
		{
			// Write material table
			DBG_ASSERT(!i_MaterialTable.empty(), "Need to group materials in material table in order to write to file.");
			if (i_MaterialTable.empty())
				return;
			
			// Material table now needs extra info about fullpaths
			// in order to locate textures.
			mtrMaterialSaver::BeginMaterialTable(o_Writer);
			//o_Writer.WriteChunkHeader(c_MTBL, 0, true);
			mdlMatInfoTable::const_iterator it;
			for (it = i_MaterialTable.begin(); it != i_MaterialTable.end(); ++it)
			{
				mtrMaterialSaver::WriteMaterialData(o_Writer, it->second->m_Info);
			}
			o_Writer.FinishChunk();
		}

		//----------------------------------------------------------------------------
		// recursive write of a level of the hierarchy
		//----------------------------------------------------------------------------
		void write_level(chWriter& o_Writer,
						 const shared_ptr<mdlNodeInfo>& i_NodeGraph)
		{
			if (i_NodeGraph->m_bIsJoint)
				mdlHierarchyParser::WriteJointChunkHeader(o_Writer);
			else
				mdlHierarchyParser::WriteLevelChunkHeader(o_Writer);

			mdlHierarchyParser::WriteNodeName(o_Writer, i_NodeGraph->m_NodeName);
			mdlHierarchyParser::WriteNodeTransform(o_Writer, i_NodeGraph->m_Transform);

			// Use epsilon to check for very nearly zero. If all are zero, we can
			// save a lot of computation when animating.
			if (	(i_NodeGraph->m_RotatePivot.LengthSqr() > maConstants::c_fEpsilon)
				 ||	(i_NodeGraph->m_ScalePivot.LengthSqr() > maConstants::c_fEpsilon)
				 ||	(i_NodeGraph->m_RotatePivotTranslation.LengthSqr() > maConstants::c_fEpsilon)
				 ||	(i_NodeGraph->m_ScalePivotTranslation.LengthSqr() > maConstants::c_fEpsilon))
			{
				mdlHierarchyParser::WriteNodePivots(o_Writer, 
													i_NodeGraph->m_RotatePivot, 
													i_NodeGraph->m_ScalePivot, 
													i_NodeGraph->m_RotatePivotTranslation, 
													i_NodeGraph->m_ScalePivotTranslation);
			}

			if (i_NodeGraph->m_JointOrientation != maVector3d(0,0,0))
				mdlHierarchyParser::WriteNodeOrientation(o_Writer, i_NodeGraph->m_JointOrientation);

			if (i_NodeGraph->m_bIsJoint)
			{
				if (i_NodeGraph->m_JointScaleOrientation != maVector3d(0,0,0))
					mdlHierarchyParser::WriteJointScaleOrientation(o_Writer, i_NodeGraph->m_JointScaleOrientation);

				mdlHierarchyParser::WriteInverseBindPose(o_Writer, i_NodeGraph->m_InverseBindPose);
			}

			if (i_NodeGraph->m_SkinInfo)
			{
				// If there is skin information, then the surface information is written
				// within that skin chunk
				DBG_ASSERT(i_NodeGraph->m_MeshInfo||i_NodeGraph->m_SubdivInfo, "Need surface information when writing skinning information.");
				DBG_ASSERT(!i_NodeGraph->m_MeshInfo||!i_NodeGraph->m_SubdivInfo, "Can have only one surface information when writing skinning information.");
				if (i_NodeGraph->m_MeshInfo)
					mdlSkinParser::WriteSKIN(o_Writer, *i_NodeGraph->m_SkinInfo, *i_NodeGraph->m_MeshInfo);	
				else if (i_NodeGraph->m_SubdivInfo)
					mdlSkinParser::WriteSKIN(o_Writer, *i_NodeGraph->m_SkinInfo, *i_NodeGraph->m_SubdivInfo);
			}
			else if (i_NodeGraph->m_MeshInfo)
			{
				mdlMeshParser::WriteGFRG(o_Writer, *i_NodeGraph->m_MeshInfo);
			}
			else if (i_NodeGraph->m_HairInfo)
			{
				mdlHairParser::WriteHRFO(o_Writer, *i_NodeGraph->m_HairInfo);
			}
			else if ( i_NodeGraph->m_InstanceInfo )
			{
				mdlNodeInfoProxy *pNodeInfoProxy = dynamic_cast< mdlNodeInfoProxy * > ( i_NodeGraph->m_InstanceInfo.get() );
				if( NULL != pNodeInfoProxy )
				{
					mdlMeshParser::WriteGNDP(o_Writer, *pNodeInfoProxy );
				} 
			}
			//Note: do we want to allow subdiv surfaces without skinning information?

			for (int i=0; i<i_NodeGraph->m_Children.size(); i++)
			{
				write_level(o_Writer, i_NodeGraph->m_Children[i]);
			}

			o_Writer.FinishChunk();
		}
	}

	//----------------------------------------------------------------------------
	//	Writes fragments to static geometry file (.mx)
	//----------------------------------------------------------------------------
	void WriteWorldFragments(const fsLocator& i_Locator,
							const std::vector< shared_ptr<mdlFragInfo> >& i_GeoData,
							const mdlMatInfoTable& i_MaterialTable )
	{
		if( fsFileUtil::FileExists(i_Locator) )
			fsFileUtil::DeleteFile(i_Locator);
		fsFileUtil::CreateFile(i_Locator);

		gfFileBin file(i_Locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
		file.WriteHeader();
	
		chBinWriter writer(file);

		// Write material table
		write_material_table(writer, i_MaterialTable);

		// Write geometry fragments
		DBG_ASSERT(!i_GeoData.empty(), "No fragment infos to write to file.");
		if (!i_GeoData.empty())
		{
			std::vector< shared_ptr<mdlFragInfo> >::const_iterator it;
			for (it = i_GeoData.begin(); it != i_GeoData.end(); ++it)
			{
				mdlMeshParser::WriteGFRG(writer, **it);
			}
		}
	}

	//------------------------------------------------------------------------
	//	WriteExporterVersionStamp - write version, date, time
	//	to chunk writer
	//------------------------------------------------------------------------
	void WriteExporterVersionStamp(chWriter &o_Writer,
						const std::string &i_VersionString)
	{
		char date_string[64];
		char time_string[64];
		_strdate(date_string);
		_strtime(time_string);

		// Write as strings so readable from bin viewer
		//
		o_Writer.WriteChunkHeader(c_EXPV, 0, false);
		o_Writer.Write(i_VersionString);
		o_Writer.Write(date_string);
		o_Writer.Write(time_string);
		o_Writer.FinishChunk();
	}

	//------------------------------------------------------------------------
	//	WriteModelPackageVersionStamp - write version, date, time
	//	to chunk writer
	//------------------------------------------------------------------------
	void WriteModelPackageVersionStamp(chWriter &o_Writer,
										const mdlModelingPackageData& o_Data)
	{
		mdlModelingPackageWriter::WriteData( o_Writer, o_Data );
	}

	//------------------------------------------------------------------------
	// WriteHierarchicalModel writes a a tree data structure of nodes and 
	// meshes into a generalized hierarchical model file (.gxb)
	//------------------------------------------------------------------------
	void WriteHierarchicalModel(const fsLocator& i_Locator,
								const shared_ptr<mdlNodeInfo>& i_NodeGraph,
								const mdlMatInfoTable& i_MaterialTable,
								const std::string &i_VersionString)
	{
		if( fsFileUtil::FileExists(i_Locator) )
			fsFileUtil::DeleteFile(i_Locator);
		fsFileUtil::CreateFile(i_Locator);

		gfFileBin file(i_Locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
		file.WriteHeader();
	
		chBinWriter writer(file);
		WriteHierarchicalModel(writer, i_NodeGraph, i_MaterialTable);

		WriteExporterVersionStamp(writer, i_VersionString);
	}
	void WriteHierarchicalModel(chWriter &o_Writer,
								const shared_ptr<mdlNodeInfo>& i_NodeGraph,
								const mdlMatInfoTable& i_MaterialTable)
	{
		// GHIE is generalized hierarchical format
		o_Writer.WriteChunkHeader(c_GHIE, 0, true);

		// Write material table
		write_material_table(o_Writer, i_MaterialTable);

		// Recursively write hierarchy
		write_level(o_Writer, i_NodeGraph);

		o_Writer.FinishChunk(); // c_GHIE
	}
}
