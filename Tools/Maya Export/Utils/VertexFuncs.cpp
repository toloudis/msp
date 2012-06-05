/*****************************************************************************
**  VertexFuncs.cpp
**
**   Namespace for vertex animation related functions. 
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <VertexFuncs.hpp>

#include <AnimFuncs.hpp>

#include <maya/MDoubleArray.h>
#include <maya/MFloatMatrix.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnParticleSystem.h>
#include <maya/MFnSubd.h>
#include <maya/MIntArray.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MMatrix.h>
#include <maya/MPlug.h>
#include <maya/MPointArray.h>
#include <maya/MUint64Array.h>

#include "Core/ch/chDefs.hpp"
#include "Core/ch/chBinWriter.hpp"
#include "Core/Ch/chChunkParserUtil.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/Ma/maAxisBox.hpp"

#include <algorithm>
#include <vector>
#include <math.h>

namespace
{
	//=============================================================================
	//	Chunk types
	//=============================================================================
	const chDefs::Name c_VTXA = chDefs::MakeName('V', 'T', 'X', 'A');
	const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
	const chDefs::Name c_FRAM = chDefs::MakeName('F', 'R', 'A', 'M');
	const chDefs::Name c_TIME = chDefs::MakeName('T', 'I', 'M', 'E');
	const chDefs::Name c_GVER = chDefs::MakeName('G', 'V', 'E', 'R');
	const chDefs::Name c_NVER = chDefs::MakeName('N', 'V', 'E', 'R');
	const chDefs::Name c_BBOX = chDefs::MakeName('B', 'B', 'O', 'X');
	const chDefs::Name c_PTXA = chDefs::MakeName('P', 'T', 'X', 'A');
	const chDefs::Name c_PATT = chDefs::MakeName('P', 'A', 'T', 'T'); // particle attributes

	//=============================================================================
	// assuming that Maya is already at the current time, write mesh's current
	//	state at the given frame to animation file.
	//=============================================================================
	void write_mesh_frame(const MFnMesh &mesh, 
		chWriter &o_Writer,
		float i_Frame,
		float i_TimeOffset,
		bool i_bWriteNormals,
		bool i_bWorldSpace)
	{
		MStatus status;
		int nVerts = mesh.numVertices(&status);

		o_Writer.WriteChunkHeader(c_FRAM, 0, true);

		// Name of mesh
		o_Writer.WriteChunkHeader(c_TIME, 0, false);
		o_Writer.Write(i_Frame - i_TimeOffset); 
		o_Writer.FinishChunk();

		//cout << "Exporting frame #" << frame << endl;

		// Vertices
		o_Writer.WriteChunkHeader(c_GVER, 1, false);
		o_Writer.Write(envType::Int32(nVerts));
		maAxisBox bbox;
		MPoint point;
		std::vector<maPoint3d> points(nVerts);
		std::vector<maPoint3d>::iterator it = points.begin();
		MSpace::Space export_space = (i_bWorldSpace) ? MSpace::kWorld : MSpace::kObject;
		for(int i = 0; i < nVerts; ++i, ++it) 
		{
			mesh.getPoint(i, point, export_space);
			it->Set(float(point[0]),float(point[1]),float(point[2]));
			bbox.Union(*it);
		}
		// Write array all at once, faster than individual writes
		if (nVerts > 0)
			o_Writer.Write(&points[0], nVerts*sizeof(maPoint3d));
		o_Writer.FinishChunk();

		if (i_bWriteNormals)
		{
			int nNormals = mesh.numNormals(&status);

			// Normals
			o_Writer.WriteChunkHeader(c_NVER, 1, false);
			o_Writer.Write(envType::Int32(nNormals));
			std::vector<maPoint3d> normals(nNormals);
			MFloatVectorArray normalArray;
			MFloatVector normal;
			mesh.getNormals(normalArray, export_space);
			std::vector<maPoint3d>::iterator nit = normals.begin();
			for (int i = 0; i < nNormals; ++i, ++nit) 
			{
				normal = normalArray[i];
				nit->Set(float(normal[0]),float(normal[1]),float(normal[2]));
			}
			// Write array all at once, faster than individual writes
			if (nNormals > 0)
				o_Writer.Write(&normals[0], nNormals*sizeof(maPoint3d));
			o_Writer.FinishChunk();
		}

		if (!bbox.IsEmpty())
		{
			o_Writer.WriteChunkHeader(c_BBOX, 0, false);
			maPoint3d min_pt(bbox.GetMinX(), bbox.GetMinY(), bbox.GetMinZ());
			maPoint3d max_pt(bbox.GetMaxX(), bbox.GetMaxY(), bbox.GetMaxZ());
			chChunkParserUtil::Write(o_Writer, min_pt);
			chChunkParserUtil::Write(o_Writer, max_pt);
			o_Writer.FinishChunk();
		}

		o_Writer.FinishChunk();

	}
	void write_subdiv_frame(const MFnSubd &subdiv, 
		chWriter &o_Writer,
		float i_Frame,
		float i_TimeOffset)
	{
		MStatus status;
		MPointArray positions;
		subdiv.vertexBaseMeshGet( positions, MSpace::kWorld );

		int nVerts = positions.length();
		o_Writer.WriteChunkHeader(c_FRAM, 0, true);

		// Name of mesh
		o_Writer.WriteChunkHeader(c_TIME, 0, false);
		o_Writer.Write(i_Frame - i_TimeOffset); 
		o_Writer.FinishChunk();

		//cout << "Exporting frame #" << frame << endl;

		// Vertices
		o_Writer.WriteChunkHeader(c_GVER, 1, false);
		o_Writer.Write(envType::Int32(nVerts));
		MPoint point;
		std::vector<maPoint3d> points(nVerts);
		std::vector<maPoint3d>::iterator it = points.begin();
		for(int i = 0; i < nVerts; ++i,++it) 
		{
			point = positions[i];
			it->Set(float(point[0]),float(point[1]),float(point[2]));
		}
		// Write array all at once, faster than individual writes
		if (nVerts > 0)
			o_Writer.Write(&points[0], nVerts*sizeof(maPoint3d));
		o_Writer.FinishChunk();

		//Note:  No normals written for subdiv vertex animation

		o_Writer.FinishChunk();
	}

	//=============================================================================
	// allocate space in the file for the mesh's vertices over the given times
	//=============================================================================
	void prepare_mesh_times(MFnMesh &mesh, 
		chWriter &o_Writer,
		const gfFileBin& i_File,
		std::list<float> keys,
		std::map<float, int> &o_SeekPos,
		bool i_bWriteNormals)
	{
		MStatus status;

		int nVerts = mesh.numVertices(&status);
		int nNormals = mesh.numNormals(&status);

		// Compute space needed to display to user (12 bytes per vector)
		int num_frames = keys.size();
		int space_needed = nVerts * 12 * num_frames;
		if (i_bWriteNormals)
			space_needed +=  nNormals * 12 * num_frames;
		cout << "Preparing space for vertex animation, ";
		cout << " nVerts: " << nVerts;
		if (i_bWriteNormals)
			cout << " nNormals: " << nNormals;
		cout << " space needed: " << space_needed / 1000 << "KB" << endl;

		std::list<float>::iterator it;
		for (it = keys.begin(); it != keys.end(); ++it)
		{
			float frame = (*it);

			fsFileStream::FilePosType seek_pos = i_File.GetFilePos();
			o_SeekPos[frame] = seek_pos;

			const float c_TempTimeOffset = 0;
			const bool bWorldSpace = true; // irrelevant for prepare step
			write_mesh_frame(mesh, o_Writer, frame, c_TempTimeOffset, i_bWriteNormals, bWorldSpace);
		}
	}
	void prepare_subdiv_times(MFnSubd &subdiv, 
		chWriter &o_Writer,
		const gfFileBin& i_File,
		std::list<float> keys,
		std::map<float, int> &o_SeekPos)
	{
		MStatus status;
		cout << "Preparing space for subdiv vertex animation." << endl;

		std::list<float>::iterator it;
		for (it = keys.begin(); it != keys.end(); ++it)
		{
			float frame = (*it);

			fsFileStream::FilePosType seek_pos = i_File.GetFilePos();
			o_SeekPos[frame] = seek_pos;

			const float c_TempTimeOffset = 0;
			write_subdiv_frame(subdiv, o_Writer, frame, c_TempTimeOffset);
		}
	}

	//=============================================================================
	// alter time in Maya UI and then write out state of mesh at each time.
	//
	// Note: dagPath is used here because an MFnMesh handle seemed to go invalid
	// sometimes when altering the Maya timline.
	//=============================================================================
	void write_mesh_at_times(MDagPath& dagPath, 
		chWriter &o_Writer,
		std::list<float> keys,
		float i_TimeOffset,
		bool i_bWriteNormals,
		bool i_bWorldSpace)
	{
		MStatus status;

		MFnMesh mesh(dagPath, &status);
		if (status)
		{
			int nVerts = mesh.numVertices(&status);
			int nNormals = mesh.numNormals(&status);

			cout << "Writing mesh vertex anim, " << mesh.name() << "nVerts: " << nVerts << " nNormals: " << nNormals << endl;

			std::list<float>::iterator it;
			for (it = keys.begin(); it != keys.end(); ++it)
			{
				//MTime time(*it);
				MTime time(*it, MTime::uiUnit());
				MayaUtil::SetCurrentTime(time);

				float frame = (*it);
				MFnMesh new_time_mesh(dagPath, &status);
				if (status)
					write_mesh_frame(new_time_mesh, o_Writer, frame, i_TimeOffset, i_bWriteNormals, i_bWorldSpace);
				else
				{
					cout << "Error accessing mesh from dagPath after altering Maya time." << endl;
					break;
				}
			}
		}
		else
		{
			cout << "Error accessing mesh from dagPath." << endl;
		}
	}
	void write_subdiv_at_times(MFnSubd &subdiv, 
		chWriter &o_Writer,
		std::list<float> keys,
		float i_TimeOffset)
	{
		MStatus status;

		MPointArray positions;
		//status = subdiv.vertexBaseMeshGet( positions, MSpace::kWorld);
		//int nVerts = positions.length();
		//cout << "nVerts: " << nVerts << endl;

		std::list<float>::iterator it;
		for (it = keys.begin(); it != keys.end(); ++it)
		{
			//MTime time(*it);
			MTime time(*it, MTime::uiUnit());
			MayaUtil::SetCurrentTime(time);

			float frame = (*it);
			write_subdiv_frame(subdiv, o_Writer, frame, i_TimeOffset);
		}
	}


	// sortable struct of particle info. Sorts based on particle id.
	struct sParticleInfo
	{
		int id;
		MPoint position;
		float age;
		float lifespan;
	};
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	bool operator<( const sParticleInfo& a, const sParticleInfo& b )
	{
		return (a.id < b.id);
	}

	void write_particle_at_times(MFnParticleSystem &particle, 
		chWriter &o_Writer,
		std::list<float> keys,
		float i_TimeOffset)
	{
		MStatus status;

		MVectorArray positions;
		MDoubleArray ages, lifespans;
		MIntArray ids;

		std::vector<sParticleInfo> particle_list;

		std::list<float>::iterator it;
		for (it = keys.begin(); it != keys.end(); ++it)
		{
			//MTime time(*it);
			MTime time(*it, MTime::uiUnit());
			MayaUtil::SetCurrentTime(time);

			particle.position( positions );
			int nVerts = positions.length();

			particle.age( ages );
			particle.lifespan( lifespans );

			particle.particleIds( ids );

			// Gather particle info into list for sorting
			particle_list.resize(nVerts);
			for (int i = 0; i < nVerts; i++) 
			{
				particle_list[i].id = ids[i];
				particle_list[i].age = (float) ages[i];
				particle_list[i].lifespan = (float) lifespans[i];
				particle_list[i].position = positions[i];
			}

			// sort the list
			std::sort(particle_list.begin(), particle_list.end());

			// write out particles in sorted order
			o_Writer.WriteChunkHeader(c_FRAM, 0, true);

			// Name of mesh
			o_Writer.WriteChunkHeader(c_TIME, 0, false);
			float frame = (*it);
			o_Writer.Write(frame - i_TimeOffset); 
			o_Writer.FinishChunk();

			//cout << "Exporting frame #" << frame << " num verts: " << nVerts << endl;

			// Vertices
			o_Writer.WriteChunkHeader(c_PATT, 1, false);
			o_Writer.Write(envType::Int32(nVerts));
			for(int i = 0; i < nVerts; i++) 
			{
				//cout << "Particle ID: " << particle_list[i].id << endl;

				// Particle ID
				o_Writer.Write(envType::Int32(particle_list[i].id));

				// Position 
				MPoint point = particle_list[i].position;
				o_Writer.Write(float(point[0]));
				o_Writer.Write(float(point[1]));
				o_Writer.Write(float(point[2]));

				// Age
				o_Writer.Write(float(particle_list[i].age));

				// Lifespan
				o_Writer.Write(float(particle_list[i].lifespan));

			}
			o_Writer.FinishChunk();

			//Note:  No normals written for particle vertex animation

			o_Writer.FinishChunk();
		}
	}

	//=============================================================================
	//=============================================================================
	void bake_subdiv_at_times(MFnSubd &subdiv, 
		int depth,
		chWriter &o_Writer,
		std::list<float> keys,
		MMatrix &matrix,
		float i_TimeOffset)
	{
		MStatus status;
		MFloatMatrix fmatrix(matrix.matrix);
		bool uniform = true;
		int sample = 0;

		MFnMeshData holder;
		MObject holder_obj = holder.create( &status);
		if (status != MS::kSuccess) 
		{
			cout << "Error creating MeshData" << endl;
		}
		else
		{
			std::list<float>::iterator it;
			for (it = keys.begin(); it != keys.end(); ++it)
			{
				//MTime time(*it);
				MTime time(*it, MTime::uiUnit());
				MayaUtil::SetCurrentTime(time);

				MObject obj = subdiv.tesselate (uniform, depth, sample, holder_obj, &status) ;
				if (status == MS::kSuccess) 
				{
					MFnMesh mesh(obj, &status);
					if (status == MS::kSuccess) 
					{
						int nVerts = mesh.numVertices(&status);
						int nNormals = mesh.numNormals(&status);

						std::list<float>::iterator it;
						for (it = keys.begin(); it != keys.end(); ++it)
						{
							//MTime time(*it);
							MTime time(*it, MTime::uiUnit());
							MayaUtil::SetCurrentTime(time);

							o_Writer.WriteChunkHeader(c_FRAM, 0, true);

							// Name of mesh
							o_Writer.WriteChunkHeader(c_TIME, 0, false);
							float frame = (*it);
							o_Writer.Write(frame - i_TimeOffset); 
							o_Writer.FinishChunk();

							//cout << "Exporting frame #" << frame << endl;

							// Vertices
							o_Writer.WriteChunkHeader(c_GVER, 1, false);
							o_Writer.Write(envType::Int32(nVerts));
							for(int i = 0; i < nVerts; i++) 
							{
								MPoint point;
								mesh.getPoint(i, point, MSpace::kWorld);
								point *= matrix;
								o_Writer.Write(float(point[0]));
								o_Writer.Write(float(point[1]));
								o_Writer.Write(float(point[2]));
							}
							o_Writer.FinishChunk();

							// Normals
							o_Writer.WriteChunkHeader(c_NVER, 1, false);
							o_Writer.Write(envType::Int32(nNormals));
							MFloatVectorArray normalArray;
							mesh.getNormals(normalArray, MSpace::kWorld);
							for (int i = 0; i < nNormals; i++) 
							{
								MFloatVector normal = normalArray[i];
								normal *= fmatrix;
								o_Writer.Write(float(normal[0]));
								o_Writer.Write(float(normal[1]));
								o_Writer.Write(float(normal[2]));
							}
							o_Writer.FinishChunk();

							o_Writer.FinishChunk();
						}
					}
				}
			}
		}
	}

}	// end of namespace


//========================================================================
//	PrepareVertexAnimation - allocate enough space in file in order
//		to later write frame data into it. Create a management
//		data structure that tracks the location of each frame
//		within the file so that we can seek to those points later.
//========================================================================
void VertexFuncs::PrepareVertexAnimation(MFnMesh &mesh, 
										  const std::string &i_MeshName,
										  chWriter &o_Writer,
										  const gfFileBin& i_File,
										  const std::list<float> &i_Keys,
										  std::map<float, int> &o_SeekPos,
										  bool i_bWriteNormals)
{
	if (!i_Keys.empty())
	{
		cout << "Have vertex animation on " << mesh.name() << endl;
		o_Writer.WriteChunkHeader(c_VTXA, 0, true);

		// Name of mesh
		o_Writer.WriteChunkHeader(c_NNAM, 0, false);
		//MString name = MayaUtil::PrepareName(mesh.name());
		//o_Writer.Write(name.asUTF8()); 
		o_Writer.Write(i_MeshName); 
		o_Writer.FinishChunk();
		
		// Write in the frame rate in small chunk at top
		//AnimFuncs::WriteCurrentFrameRate(o_Writer);

		prepare_mesh_times(mesh, o_Writer, i_File, i_Keys, o_SeekPos, i_bWriteNormals);

		o_Writer.FinishChunk();
	}
}
//void VertexFuncs::PrepareVertexAnimation(MFnSubd &subdiv, chWriter &o_Writer,
//										  const gfFileBin& i_File,
//										  const std::list<float> &i_Keys,
//										  std::map<float, int> &o_SeekPos)
//{
//	if (!i_Keys.empty())
//	{
//		cout << "Have vertex animation on " << subdiv.name() << endl;
//		o_Writer.WriteChunkHeader(c_VTXA, 0, true);
//
//		// Name of mesh
//		o_Writer.WriteChunkHeader(c_NNAM, 0, false);
//		MString name = MayaUtil::PrepareName(subdiv.name());
//		o_Writer.Write(name.asUTF8()); 
//		o_Writer.FinishChunk();
//		
//		// Write in the frame rate in small chunk at top
//		//AnimFuncs::WriteCurrentFrameRate(o_Writer);
//
//		prepare_subdiv_times(subdiv, o_Writer, i_File, i_Keys, o_SeekPos);
//
//		o_Writer.FinishChunk();
//	}
//}

//========================================================================
//	WriteVertexAnimationFrame - assuming Maya timeline is at the correct
//		time, seek to position in file and write out animation data
//		for this mesh at this time.
//========================================================================
void VertexFuncs::WriteVertexAnimationFrame( const MFnMesh &mesh, 
										     chWriter &o_Writer,
										     gfFileBin& io_File,
										     float i_Frame,
										     float i_TimeOffset,
										     const std::map<float, int> &i_SeekPos,
										     bool i_bWriteNormals,
											 bool i_bWorldSpace)
{
	// Seek to position in file for this frame
	std::map<float, int>::const_iterator it = i_SeekPos.find(i_Frame);
	
	if (it == i_SeekPos.end())
		cout << "Error seeking in vertex animation file." << endl;
	else
	{
		int seek_pos = it->second;
		io_File.SetFilePos(seek_pos);

		// Write actual frame animation data
		write_mesh_frame(mesh, o_Writer, i_Frame, i_TimeOffset, i_bWriteNormals, i_bWorldSpace);
	}
}
//void VertexFuncs::WriteVertexAnimationFrame( const MFnSubd &subdiv, 
//											 chWriter &o_Writer,
//											 gfFileBin& io_File,
//											 float i_Frame,
//											 float i_TimeOffset,
//											 const std::map<float, int> &i_SeekPos )
//{
//	// Seek to position in file for this frame
//	std::map<float, int>::const_iterator it = i_SeekPos.find(i_Frame);
//	
//	if (it == i_SeekPos.end())
//		cout << "Error seeking in vertex animation file." << endl;
//	else
//	{
//		int seek_pos = it->second;
//		io_File.SetFilePos(seek_pos);
//
//		// Write actual frame animation data
//		write_subdiv_frame(subdiv, o_Writer, i_Frame, i_TimeOffset);
//	}
//
//}

//========================================================================
//	WriteMeshVertexAnimation - write animation of vertices to file
//		at the times given in the keys list
//========================================================================
void VertexFuncs::WriteMeshVertexAnimation(MDagPath& dagPath, 
										   MString mesh_name,
										   chWriter &o_Writer,
										   const std::list<float> &i_Keys,
										   float i_TimeOffset,
										   bool i_bWriteNormals,
										   bool i_bWorldSpace)
{
	if (!i_Keys.empty())
	{
		cout << "Have vertex animation on " << mesh_name << endl;
		o_Writer.WriteChunkHeader(c_VTXA, 0, true);

		// Name of mesh
		o_Writer.WriteChunkHeader(c_NNAM, 0, false);
		MString name = MayaUtil::PrepareName(mesh_name);
		o_Writer.Write(name.asUTF8()); 
		o_Writer.FinishChunk();
		
		// Write in the frame rate in small chunk at top
		//AnimFuncs::WriteCurrentFrameRate(o_Writer);

		write_mesh_at_times(dagPath, o_Writer, i_Keys, i_TimeOffset, i_bWriteNormals, i_bWorldSpace);

		o_Writer.FinishChunk();
	}
}

//========================================================================
//	MesHashVertexAnimation - return true if there is 
//	some vertex animation on the given mesh.
//========================================================================
bool VertexFuncs::MeshHashVertexAnimation( MFnMesh& mesh )
{
	MStatus status;

	MPlug pntsPlug = mesh.findPlug("pnts", &status);
	if (status == MS::kSuccess)
	{
		int num_elems = pntsPlug.numElements();
		//cout << "pnts has num elements: " << num_elems << endl;
		if (num_elems > 0)
		{
			// Get anim curve for first vertex
			MPlug elem = pntsPlug.elementByPhysicalIndex(0);
			//cout << "First plug has num elements: " << elem.numElements() << endl;
			//cout << "First plug has num children: " << elem.numChildren() << endl;
			if (elem.numChildren() == 3)
			{
				// The three children are X,Y,Z positions
				MFnAnimCurve animx = AnimFuncs::GetAnimCurve(pntsPlug.child(0), status);
				if (status == MS::kSuccess) return true;
				MFnAnimCurve animy = AnimFuncs::GetAnimCurve(pntsPlug.child(1), status);
				if (status == MS::kSuccess) return true;
				MFnAnimCurve animz = AnimFuncs::GetAnimCurve(pntsPlug.child(2), status);
				if (status == MS::kSuccess) return true;
			}
		}
	}
	return false;
}

//========================================================================
//	WriteMeshVertexAnimation - write animation of vertices to file.
//========================================================================
void VertexFuncs::WriteMeshVertexAnimation(MDagPath& dagPath, chWriter &o_Writer,
							 double i_MinTime, 
							 double i_MaxTime )
{
	MStatus status;

	MFnMesh mesh(dagPath, &status);
	if (status)
	{
		//double minTime, maxTime;
		//AnimFuncs::GetTimeSliderRange(minTime, maxTime);
		
	//	MayaUtil::PrintInputs(mesh);
	//	MayaUtil::PrintPlugs(mesh);

		MPlug pntsPlug = mesh.findPlug("pnts", &status);
		if (status == MS::kSuccess)
		{
			int num_elems = pntsPlug.numElements();
			cout << "pnts has num elements: " << num_elems << endl;
			if (num_elems > 0)
			{
				// Get anim curve for first vertex
				MPlug elem = pntsPlug.elementByPhysicalIndex(0);
				//cout << "First plug has num elements: " << elem.numElements() << endl;
				//cout << "First plug has num children: " << elem.numChildren() << endl;
				if (elem.numChildren() == 3)
				{
					// The three children are X,Y,Z positions
					MFnAnimCurve animx = AnimFuncs::GetAnimCurve(pntsPlug.child(0), status);
					MFnAnimCurve animy = AnimFuncs::GetAnimCurve(pntsPlug.child(1), status);
					MFnAnimCurve animz = AnimFuncs::GetAnimCurve(pntsPlug.child(2), status);
					if (status == MS::kSuccess)
					{
						//cout << "Got anim curves." << endl;
						std::list<float> keys;
						AnimFuncs::GatherKeys(keys, animx, animy, animz, i_MinTime, i_MaxTime);
						WriteMeshVertexAnimation(dagPath, mesh.name(), o_Writer, keys, (float) i_MinTime);
					}
				}
			}
		}
		else
		{
			cout << "No vertex animation." << endl;
		}
	}
	else
	{
		cout << "Could not get mesh object from dagPath" << endl;
	}

}


//========================================================================
//========================================================================
//void VertexFuncs::WriteVertexAnimation(MFnSubd &subd, chWriter &o_Writer,
//									   const std::list<float> &i_Keys,
//									   float i_TimeOffset)
//{
//	if (!i_Keys.empty())
//	{
//		cout << "Have vertex animation on " << subd.name() << endl;
//		o_Writer.WriteChunkHeader(c_VTXA, 0, true);
//
//		// Name of mesh
//		o_Writer.WriteChunkHeader(c_NNAM, 0, false);
//		MString name = MayaUtil::PrepareName(subd.name());
//		o_Writer.Write(name.asUTF8()); 
//		o_Writer.FinishChunk();
//		
//		// Write in the frame rate in small chunk at top
//		//AnimFuncs::WriteCurrentFrameRate(o_Writer);
//
//		write_subdiv_at_times(subd, o_Writer, i_Keys, i_TimeOffset);
//
//		o_Writer.FinishChunk();
//	}
//}

//========================================================================
//========================================================================
//void VertexFuncs::WriteVertexAnimation(MFnSubd &subd, chWriter &o_Writer)
//{
//	MStatus status;
//
//	double minTime, maxTime;
//	AnimFuncs::GetTimeSliderRange(minTime, maxTime);
//	
////	MayaUtil::PrintInputs(subd);
////	MayaUtil::PrintPlugs(subd);
//
//	MPlug pntsPlug = subd.findPlug("cp", &status);
//	if (status == MS::kSuccess)
//	{
//		int num_elems = pntsPlug.numElements();
//		//cout << "pnts has num elements: " << num_elems << endl;
//		if (num_elems > 0)
//		{
//			// Get anim curve for first vertex
//			MPlug elem = pntsPlug.elementByPhysicalIndex(0);
//			//cout << "First plug has num elements: " << elem.numElements() << endl;
//			//cout << "First plug has num children: " << elem.numChildren() << endl;
//			if (elem.numChildren() == 3)
//			{
//				// The three children are X,Y,Z positions
//				MFnAnimCurve animx = AnimFuncs::GetAnimCurve(pntsPlug.child(0), status);
//				MFnAnimCurve animy = AnimFuncs::GetAnimCurve(pntsPlug.child(1), status);
//				MFnAnimCurve animz = AnimFuncs::GetAnimCurve(pntsPlug.child(2), status);
//				if (status == MS::kSuccess)
//				{
//					//cout << "Got anim curves." << endl;
//					std::list<float> keys;
//					AnimFuncs::GatherKeys(keys, animx, animy, animz); 
//					WriteVertexAnimation(subd, o_Writer, keys, (float)minTime);
//				}
//			}
//		}
//	}
//	else
//	{
//		cout << "No vertex animation." << endl;
//	}
//}


//========================================================================
// Write subdivision animation as baked mesh vertex animation
//  VERY SLOW!
//========================================================================
void VertexFuncs::BakeVertexAnimation(MFnSubd &subd, 
									   int depth,
									   chWriter &o_Writer,
									   const std::list<float> &i_Keys,
									   MMatrix &matrix,
									   float i_TimeOffset)
{
	if (!i_Keys.empty())
	{
		cout << "Have vertex animation on " << subd.name() << endl;
		o_Writer.WriteChunkHeader(c_VTXA, 0, true);

		// Name of mesh
		o_Writer.WriteChunkHeader(c_NNAM, 0, false);
		MString name = MayaUtil::PrepareName(subd.name());
		o_Writer.Write(name.asUTF8()); 
		o_Writer.FinishChunk();
		
		// Write in the frame rate in small chunk at top
		//AnimFuncs::WriteCurrentFrameRate(o_Writer);

		bake_subdiv_at_times(subd, depth, o_Writer, i_Keys, matrix, i_TimeOffset);

		o_Writer.FinishChunk();
	}
}


//========================================================================
//	WriteVertexAnimation - write animation of particles to file.
//========================================================================
void VertexFuncs::WriteVertexAnimation(MFnParticleSystem &particle, 
									   chWriter &o_Writer,
									   const std::list<float> &i_Keys,
									   float i_TimeOffset)
{
	MStatus status;
	
//	MayaUtil::PrintInputs(mesh);
//	MayaUtil::PrintPlugs(mesh);

	if (!particle.hasLifespan())
	{
		MayaUtil::DisplayError("Cannot export particle " + particle.name() + ", lifespan per particle is required.");
	}

	if (!i_Keys.empty())
	{
		cout << "Have vertex animation on " << particle.name() << endl;
		o_Writer.WriteChunkHeader(c_PTXA, 0, true);

		// Name of mesh
		o_Writer.WriteChunkHeader(c_NNAM, 0, false);
		MString name = MayaUtil::PrepareName(particle.name());
		o_Writer.Write(name.asUTF8()); 
		o_Writer.FinishChunk();
		
		// Write in the frame rate in small chunk at top
		//AnimFuncs::WriteCurrentFrameRate(o_Writer);

		write_particle_at_times(particle, o_Writer, i_Keys, i_TimeOffset);

		o_Writer.FinishChunk();
	}
	else
	{
		cout << "No keys for animation." << endl;
	}
}
