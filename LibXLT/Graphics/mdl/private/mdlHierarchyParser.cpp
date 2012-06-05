/****************************************************************************\
**	mdlHierarchyParser.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/private/mdlHierarchyParser.hpp"

#include "Core/ch/chBinReader.hpp"
#include "Core/Ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/fsResourceFinder.hpp"
#include "Core/Gf/gfFileBin.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlHairInfo.hpp"
#include "Graphics/mdl/mdlModelingPackageData.hpp"
#include "Graphics/mdl/mdlModelingPackageListMgr.hpp"
#include "Graphics/mdl/mdlModelingPackageParser.hpp"
#include "Graphics/mdl/mdlNodeInfo.hpp"
#include "Graphics/mdl/private/mdlHairParser.hpp"
#include "Graphics/mdl/private/mdlMaterialParser.hpp"
#include "Graphics/mdl/private/mdlMeshParser.hpp"
#include "Graphics/mdl/private/mdlSkinParser.hpp"
#include "Graphics/smdl/smdlCharacterSkin.hpp"


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	// chunk names that we parse
	//------------------------------------------------------------------------
	const chDefs::Name c_GHIE = chDefs::MakeName('G', 'H', 'I', 'E');
	const chDefs::Name c_MHIE = chDefs::MakeName('M', 'H', 'I', 'E');
	const chDefs::Name c_MCHR = chDefs::MakeName('M', 'C', 'H', 'R');
	const chDefs::Name c_MTBL = chDefs::MakeName('M', 'T', 'B', 'L');
	const chDefs::Name c_GFRG = chDefs::MakeName('G', 'F', 'R', 'G');
	const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
	const chDefs::Name c_HLEV = chDefs::MakeName('H', 'L', 'E', 'V');
	const chDefs::Name c_HTRN = chDefs::MakeName('H', 'T', 'R', 'N');
	const chDefs::Name c_HJNT = chDefs::MakeName('H', 'J', 'N', 'T');
	const chDefs::Name c_HJOR = chDefs::MakeName('H', 'J', 'O', 'R');
	const chDefs::Name c_HPVT = chDefs::MakeName('H', 'P', 'V', 'T');
	const chDefs::Name c_JOIN = chDefs::MakeName('J', 'O', 'I', 'N');
	const chDefs::Name c_JODA = chDefs::MakeName('J', 'O', 'D', 'A');
	const chDefs::Name c_INVB = chDefs::MakeName('I', 'N', 'V', 'B');
	const chDefs::Name c_JSCO = chDefs::MakeName('J', 'S', 'C', 'O');
	const chDefs::Name c_SKIN = chDefs::MakeName('S', 'K', 'I', 'N');
	const chDefs::Name c_GNDP = chDefs::MakeName('G', 'N', 'D', 'P');
	const chDefs::Name c_HRFO = chDefs::MakeName('H', 'R', 'F', 'O');
	const chDefs::Name c_MODV = chDefs::MakeName('M', 'O', 'D', 'V');

	//------------------------------------------------------------------------
	// convert from 3 rotation angles to an orientation matrix
	//------------------------------------------------------------------------
	void construct_orientation_matrix(float i_X, float i_Y, float i_Z,
									  maMatrix4x4 &o_Orientation)
	{
		o_Orientation.MakeRotateX(i_X);
		maMatrix4x4 ry;
		ry.MakeRotateY(i_Y);
		maMatrix4x4 rz;
		rz.MakeRotateZ(i_Z);
		o_Orientation *= ry;
		o_Orientation *= rz;
	}

	//------------------------------------------------------------------------
	// read skinning - deformation animation plus surface information
	//------------------------------------------------------------------------
	void read_SKIN(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					mdlHierarchyParser& io_Parser)
	{
		// A Skin consists of a subdivision or mesh surface with joint influences 
		// and morph targets that is attached to the joint skeleton read in the 
		// JOIN chunk above.
		//
		shared_ptr<mdlSubdivInfo> subdiv_info;
		shared_ptr<mdlFragInfo> frag_info;
		shared_ptr<smdlCharacterSkin> skin(new smdlCharacterSkin());
		mdlSkinParser::ReadSKIN(	i_Reader,
									i_Version,
									i_Size,
									*skin,
									subdiv_info,
									frag_info,
									io_Parser.GetMaterialTable());
		if (subdiv_info)
			io_Parser.AddSkin(subdiv_info, skin);
		else if (frag_info)
			io_Parser.AddSkin(frag_info, skin);
		else
		{
			DBG_WARNING("Expected either subdiv or mesh info in SKIN chunk");
		}
	}

	//------------------------------------------------------------------------
	// forward declaration
	//------------------------------------------------------------------------
	bool handle_scenenode_chunk(	chReader& i_Reader,
									chDefs::Name i_Name,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									mdlHierarchyParser& io_Parser,
									bool	&o_bReadHTRN);


	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void read_level(chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					mdlHierarchyParser& io_Parser)
	{
		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;

		bool read_HTRN = false;

		try
		{
			while( i_Reader.ReadChunkHeader(name, version, size) )
			{
				handle_scenenode_chunk(i_Reader, name, version, size, io_Parser, read_HTRN);

				i_Reader.FinishChunk();
			}
		}
		catch ( const chInvalidChunkX& i_Ex )
		{
			i_Ex;
			DBG_ERROR("Invalid chunk in mdlHierarchyParser::read_level");
			throw mdlInvalidModelFileX(i_Reader.GetLocator());
		}

		DBG_ASSERT(read_HTRN, "Didn't find required HTRN chunk in mdlHierarchyParser::read_MHIE");
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool handle_scenenode_chunk(	chReader& i_Reader,
									chDefs::Name i_Name,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									mdlHierarchyParser& io_Parser,
									bool	&o_bReadHTRN)
	{
		if( i_Name == c_NNAM )
		{
			std::string node_name;
			i_Reader.Read(node_name);
			//DBG_LOG("Read node named, " << node_name.c_str());
			io_Parser.SetNodeName(node_name.c_str());
		}
		else if( i_Name == c_GFRG )
		{
			// Call the virtual function to read in the mesh info,
			// can be overriden by the derived class.
			io_Parser.ReadMesh(i_Reader, i_Name, i_Version, i_Size);
		}
		else if( i_Name == c_GNDP )
		{
			io_Parser.ReadNodeInfoProxy(i_Reader, i_Name, i_Version, i_Size );
		}
		else if (i_Name == c_SKIN)
		{
			// A Skin consists of a subdivision or mesh surface with joint influences 
			// and morph targets that is attached to the joint skeleton read in the 
			// JOIN chunks.
			//
			read_SKIN( i_Reader, i_Version, i_Size, io_Parser );
		}
		else if (i_Name == c_HRFO )
		{
			// Hair strands
			shared_ptr<mdlHairInfo> hair_info(new mdlHairInfo());
			if (mdlHairParser::ReadHRFO( i_Reader, i_Version, i_Size, *hair_info, &io_Parser.GetMaterialTable()))
				io_Parser.AddHair(hair_info);
		}
		else if( i_Name == c_HTRN )
		{
			o_bReadHTRN = true;
			maMatrix4x4 xform;
			chChunkParserUtil::Read(i_Reader, xform);
			io_Parser.SetNodeTransform(xform);
		}
		else if( i_Name == c_HPVT )
		{
			maVector3d rotate_pivot, scale_pivot;
			maVector3d rotate_pivot_trans(0,0,0), scale_pivot_trans(0,0,0);
			chChunkParserUtil::Read(i_Reader, rotate_pivot);
			chChunkParserUtil::Read(i_Reader, scale_pivot);

			if (i_Version >= 1)
			{
				chChunkParserUtil::Read(i_Reader, rotate_pivot_trans);
				chChunkParserUtil::Read(i_Reader, scale_pivot_trans);
			}

			io_Parser.SetNodePivots(rotate_pivot, scale_pivot, 
									rotate_pivot_trans, scale_pivot_trans);
		}
		else if( i_Name == c_HJOR )
		{
			maVector3d orientation;
			chChunkParserUtil::Read(i_Reader, orientation);
			io_Parser.SetNodeOrientation(orientation.GetX(), orientation.GetY(), orientation.GetZ());
		}
		else if( i_Name == c_JSCO )
		{
			maVector3d orientation;
			chChunkParserUtil::Read(i_Reader, orientation);
			io_Parser.SetScaleOrientation(orientation.GetX(), orientation.GetY(), orientation.GetZ());
		}
		else if( i_Name == c_INVB )
		{
			maMatrix4x4 inv_bind_pose;
			chChunkParserUtil::Read(i_Reader, inv_bind_pose);
			io_Parser.SetInverseBindPose(inv_bind_pose);
		}
		else if( i_Name == c_JODA )
		{
			// JODA is from the CHX file format. The newer GHX file format
			// uses individual chunks like INVB and JSCO
			//
			o_bReadHTRN = true;		// JODA also sets the transform for the node
			maVector3d translation, rotation, orientation, init_scale;
			chChunkParserUtil::Read(i_Reader, translation);
			chChunkParserUtil::Read(i_Reader, rotation);
			chChunkParserUtil::Read(i_Reader, orientation);
			chChunkParserUtil::Read(i_Reader, init_scale);

			maMatrix4x4 orient_matx;
			if ( (orientation.m_X != 0) ||
				 (orientation.m_Y != 0) ||
				 (orientation.m_Z != 0) )
			{
				io_Parser.SetNodeOrientation( orientation.m_X, orientation.m_Y, orientation.m_Z );
				construct_orientation_matrix(orientation.m_X, orientation.m_Y, orientation.m_Z, orient_matx);
			}

			maMatrix4x4 bind_pose;
			chChunkParserUtil::Read(i_Reader, bind_pose);
			bind_pose.Invert();
			io_Parser.SetInverseBindPose(bind_pose);

			// The influences have been moved to their own chunk in version 1
			//if (i_Version < 1)
			//{
			//	envType::UInt16 i, num_influences;
			//	i_Reader.Read( num_influences );

			//	for( i = 0 ; i < num_influences ; i++ )
			//	{
			//		JointInfluence joint_infl;
			//		joint_infl.m_nJointIndex = io_nJointIndex;
			//		envType::UInt16 old_index; // old version only does 16-bit
			//		i_Reader.Read( old_index );
			//		joint_infl.m_nVertexIndex = old_index;
			//		i_Reader.Read( joint_infl.m_fWeight );
			//		o_JointInfluences.push_back( joint_infl );
			//	}
			//}

			if (i_Version >= 2)
			{
				// Later versions put the transformation matrix directly at the
				// end of the chunk so we don't have to recompose it.
				maMatrix4x4 transform;
				chChunkParserUtil::Read(i_Reader, transform);
				io_Parser.SetNodeTransform(transform);

				// Also adds scaleOrientation rotation
				maVector3d scaleOrient;
				chChunkParserUtil::Read(i_Reader, scaleOrient);
				if ( (scaleOrient.m_X != 0) ||
					 (scaleOrient.m_Y != 0) ||
					 (scaleOrient.m_Z != 0) )
				{
					io_Parser.SetScaleOrientation( scaleOrient.m_X, scaleOrient.m_Y, scaleOrient.m_Z );
				}
			}
			else
			{
				// Construct the joint's transform
				maMatrix4x4 transform;
				maRotation base_rot;
				base_rot.SetEuler(rotation.m_X, rotation.m_Y, rotation.m_Z);
				//transform.MakeScale( init_scale.m_X, init_scale.m_Y, init_scale.m_Z );
				transform = base_rot.GetMatrix() * orient_matx;
				transform.TranslateBy( translation );
				io_Parser.SetNodeTransform(transform);
			}
		}
		else if( i_Name == c_HLEV || i_Name == c_HJNT)
		{
			//DBG_LOG("Read c_HLEV");
			io_Parser.CreateLevel();
			read_level(	i_Reader,
						i_Version,
						i_Size,
						io_Parser);
			io_Parser.FinishLevel();
			//DBG_LOG("End c_HLEV");
		}
		else if( i_Name == c_JOIN)
		{
			//DBG_LOG("Read c_JOIN");
			const bool bIsJoint = true;
			io_Parser.CreateLevel(bIsJoint);
			read_level(	i_Reader,
						i_Version,
						i_Size,
						io_Parser);
			io_Parser.FinishLevel();
			//DBG_LOG("End c_JOIN");
		}
		else
			return false;

		return true;
	}


}	// end of local namespace

//------------------------------------------------------------------------
//	LoadModel loads a hierarchical model from a file.  It will make 
//	calls to the HierarchyParser object when it reaches each level in
//	in the hierarchy.
//------------------------------------------------------------------------
void mdlHierarchyParser::LoadModel(	const fsLocator& i_File,
									mdlHierarchyParser& io_Parser )
{
	gfFileBin file(i_File, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);

	//	If this isn't a real Terawatt/XLT binary file this will throw
	file.ReadHeader();

	chBinReader reader(file);
		
	std::string filename;
	fsFileUtil::LocatorToANSIFilename(i_File, filename);	

	chDefs::Name name;
	chDefs::Size size;
	chDefs::Version version;

	reader.ReadChunkHeader(name, version, size);
	if ((name != c_MHIE) && (name != c_GHIE) && (name != c_MCHR))
	{
		DBG_ERROR("Expected MHIE or GHIE or MCHR chunk for hierarchical model in file: " << filename.c_str());
		throw mdlInvalidModelFileX(i_File);
	}

	bool bMODV_chunk_read = false;
	try
	{
		while( reader.ReadChunkHeader(name, version, size) )
		{
			if (name == c_MTBL)
			{
				// material table has shared materials for
				// fragments, has to be read before GFRG chunks
				mdlMaterialParser::ReadMaterialTable(   reader,
														version,
														size,
														io_Parser.GetMaterialTable());

				// Notify that the material table has been read
				io_Parser.MaterialTableWasRead();
			}
			else if (name == c_HLEV || name == c_HJNT)
			{
				io_Parser.CreateLevel();
				read_level(	reader,
							version,
							size,
							io_Parser );
				io_Parser.FinishLevel();
			}
			else if (name == c_JOIN)
			{
				const bool bIsJoint = true;
				io_Parser.CreateLevel(bIsJoint);
				read_level(	reader,
							version,
							size,
							io_Parser );
				io_Parser.FinishLevel();
			}
			else if (name == c_SKIN)
			{
				// version 0 was a polygon based character system.
				// That style is completely unsupported now.
				if (version < 1)
					throw mdlInvalidModelFileX(i_File);

				// A Skin consists of a subdivision or mesh surface with joint influences 
				// and morph targets that is attached to the joint skeleton read in the 
				// JOIN chunk above.
				//
				read_SKIN( reader,
							version,
							size,
							io_Parser );
			}
			else if (name == c_MODV)
			{
				bMODV_chunk_read = true;

				mdlModelingPackageData mpdata;
				mdlModelingPackageParser::ReadMODV(	reader, version, size, mpdata );

				//	if the package name isn't in the list throw and exception
				if (!mdlModelingPackageListMgr::IsInList( mpdata.m_ModelingPackageName ))
				{
					DBG_ERROR("Modeling Package " << mpdata.m_ModelingPackageName );
					throw mdlInvalidModelFileX(i_File);
				}
			}

			reader.FinishChunk();
		}
	}
	catch ( const chInvalidChunkX& i_Ex )
	{
		i_Ex;
		DBG_ERROR("Invalid chunk in mdlHierarchyParser::LoadHierarchicalModel, file: " << filename.c_str());
		throw mdlInvalidModelFileX(i_File);
	}
	
	//	if there is a list of valid packages, but the MODV chunk hasn't been read
	//	(true with older exporters), then throw an exception.
	//
	if (!mdlModelingPackageListMgr::IsListEmpty() && (!bMODV_chunk_read))
	{
		throw mdlInvalidModelFileX(i_File);
	}
}

//------------------------------------------------------------------------
// Reads a HLEV chunk of an existing open reader.
//------------------------------------------------------------------------
void mdlHierarchyParser::LoadHierarchy(mdlHierarchyParser& io_Parser,
									   chReader& i_Reader,
									   chDefs::Name i_Name,
									   chDefs::Version i_Version,
									   chDefs::Size i_Size)
{
	// Not sure about assert or exception here.
	DBG_ASSERT(i_Name==c_HLEV, "Expected HLEV chunk for LoadHierarchy");
	if (i_Name != c_HLEV)
	{
		DBG_ERROR("Expected HLEV chunk for LoadHierarchy.");
		throw mdlInvalidModelFileX(i_Reader.GetLocator());
	}

	io_Parser.CreateLevel();

	read_level(	i_Reader,
				i_Version,
				i_Size,
				io_Parser );

	io_Parser.FinishLevel();

}

//------------------------------------------------------------------------
// This is an alternative to the "AddMesh()" function to be used when
//	the derivation wants to control the how the mdlFragInfo structure
//	is created. The default behavior is to make a local mdlFragInfo
//	structure, read the frag info and then call "AddMesh()".
//------------------------------------------------------------------------
void mdlHierarchyParser::ReadMesh(chReader& i_Reader,
								  chDefs::Name i_Name,
								  chDefs::Version i_Version,
								  chDefs::Size i_Size)
{
	// We only need the vertex remap when the mesh will be vertex animated,
	// which is not the case in this file format.
	const bool c_bFillInVertexRemap = false;

	// If mdlMeshParser::ReadGFRG returns false, then it means that the 
	// fragment was skipped because it had a resolution level that we
	// are skipping.
	mdlFragInfo fragInfo;
	if (mdlMeshParser::ReadGFRG(i_Reader, i_Version, i_Size, 
			fragInfo, c_bFillInVertexRemap, &this->GetMaterialTable()))
	{
		this->AddMesh(fragInfo);
	}
}

//------------------------------------------------------------------------
// Similar to REadMesh,
// But uses AddMeshReference
//------------------------------------------------------------------------
void mdlHierarchyParser::ReadNodeInfoProxy(chReader& i_Reader,
								  chDefs::Name i_Name,
								  chDefs::Version i_Version,
								  chDefs::Size i_Size )
{
	// If mdlMeshParser::ReadGNDP returns false, then it means that the 
	// fragment was skipped because it had a resolution level that we
	// are skipping.
	mdlNodeInfoProxy nodeInfoProxy;
	if (mdlMeshParser::ReadGNDP(i_Reader, i_Version, i_Size, nodeInfoProxy) )
	{
		AddMeshReference( nodeInfoProxy );
	}
}
//------------------------------------------------------------------------
// Notification that a Material Table was read.
//------------------------------------------------------------------------
//virtual 
void mdlHierarchyParser::MaterialTableWasRead()
{
	// default does nothing
}

//------------------------------------------------------------------------
// Static write functions to write hierarchical models
//------------------------------------------------------------------------ 
void mdlHierarchyParser::WriteLevelChunkHeader(chWriter& o_Writer)
{
	o_Writer.WriteChunkHeader(c_HLEV, 0, true);
}
void mdlHierarchyParser::WriteJointChunkHeader(chWriter& o_Writer)
{
	o_Writer.WriteChunkHeader(c_JOIN, 0, true);
}
void mdlHierarchyParser::WriteNodeName(chWriter& o_Writer, const std::string& i_NodeName)
{
	o_Writer.WriteChunkHeader(c_NNAM, 0, false);
	o_Writer.Write(i_NodeName);
	o_Writer.FinishChunk();
}
void mdlHierarchyParser::WriteNodeTransform(chWriter& o_Writer, const maMatrix4x4& i_Transform)
{
	o_Writer.WriteChunkHeader(c_HTRN, 0, false);
	chChunkParserUtil::Write(o_Writer, i_Transform);
	o_Writer.FinishChunk();
}
void mdlHierarchyParser::WriteNodePivots(chWriter& o_Writer, 
										 const maVector3d &i_RotatePivot, 
										 const maVector3d &i_ScalePivot, 
										 const maVector3d &i_RotatePivotTranslation, 
										 const maVector3d &i_ScalePivotTranslation)
{
	o_Writer.WriteChunkHeader(c_HPVT, 1, false);
	chChunkParserUtil::Write(o_Writer, i_RotatePivot);
	chChunkParserUtil::Write(o_Writer, i_ScalePivot);

	// Version 1 adds translation for pivots that have been moved during construction
	chChunkParserUtil::Write(o_Writer, i_RotatePivotTranslation);
	chChunkParserUtil::Write(o_Writer, i_ScalePivotTranslation);

	o_Writer.FinishChunk();
}
void mdlHierarchyParser::WriteNodeOrientation(chWriter& o_Writer, const maVector3d &i_JointOrientation)
{
	o_Writer.WriteChunkHeader(c_HJOR, 0, false);
	chChunkParserUtil::Write(o_Writer, i_JointOrientation);
	o_Writer.FinishChunk();
}
void mdlHierarchyParser::WriteJointScaleOrientation(chWriter& o_Writer, const maVector3d &i_JointScaleOrientation)
{
	o_Writer.WriteChunkHeader(c_JSCO, 0, false);
	chChunkParserUtil::Write(o_Writer, i_JointScaleOrientation);
	o_Writer.FinishChunk();
}
void mdlHierarchyParser::WriteInverseBindPose(chWriter& o_Writer, const maMatrix4x4& i_InverseBindPose)
{
	o_Writer.WriteChunkHeader(c_INVB, 0, false);
	chChunkParserUtil::Write(o_Writer, i_InverseBindPose);
	o_Writer.FinishChunk();
}
