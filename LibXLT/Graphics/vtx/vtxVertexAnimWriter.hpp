/****************************************************************************\
**	vtxVertexAnimWriter.hpp
**
**		vtxVertexAnimWriter.hpp supplies functions used to export vertex anim data into
**	our animation file formats	
**
**	StudioGPU
**	Copyright(C) 2003-8 - All Rights Reserved
\****************************************************************************/
#ifdef VTX_VERTEXANIMWRITER_HPP
#error vtxVertexAnimWriter.hpp multiply included
#endif
#define VTX_VERTEXANIMWRITER_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#include <map>
#include <string>
#include <vector>


//============================================================================
//	Forward References
//============================================================================
class chWriter;
class vtxVertexAnimKeysStatic;
class vtxVertexFramesStatic;
struct vtxVertexFrame;


//============================================================================
//============================================================================
namespace vtxVertexAnimWriter
{
	//------------------------------------------------------------------------
	//A call back structure that can be operated on each vtxVertexFrame
	//Eg: We could use this to update a progress bar, as we write the vertex data.
	//------------------------------------------------------------------------
	struct WriteVertexAnimForMeshCallback
	{
		virtual void operator()( vtxVertexFrame *i_pVertexFrame ) const {}
	};

	//------------------------------------------------------------------------
	// Write the geoemtry data for a single vertex frame.
	// o_Writer is the binary chunk writer
	// i_FrameData is the animation data
	// If i_bTwoComponentNormals is true, write only the X and Y 
	//	values for the normals (can compute the Z when unit length).
	//------------------------------------------------------------------------	
	void WriteVertexFrameData( chWriter &io_Writer, 
							   const vtxVertexFrame& i_FrameData);
							   //bool i_bTwoComponentNormals = false );

	//------------------------------------------------------------------------
	//write the vertex animation data
	//o_Writer is the binary chunk writer
	//i_Callback, is the callback struct we can call  on each vertexframe
	//i_MeshName, is the name of the mesh that we are animating
	//i_VetexAnimKeys is the animation data
	//------------------------------------------------------------------------	
	void WriteVertexAnimForMesh( chWriter &o_Writer,
								 WriteVertexAnimForMeshCallback &i_Callback,
								 const std::string &i_MeshName,
								 shared_ptr< vtxVertexAnimKeysStatic > &i_VertexAnimkeys);

	//------------------------------------------------------------------------
	// Write encoded deltas as a large buffer of memory,
	// compressed with ZLib compression.
	//------------------------------------------------------------------------
	void WriteDeltas(chWriter &o_Writer,
					 const void* i_Data, 
					 int i_NumBytes);
}

