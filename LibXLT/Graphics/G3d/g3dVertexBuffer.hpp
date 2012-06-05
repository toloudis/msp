/****************************************************************************\
**	g3dVertexBuffer.hpp
**
**		A g3dVertexBuffer is the base class for a buffer of vertex data.
**	It can be altered by using the UpdateVertices() functions or
**	with the Lock() and Unlock() functions.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_VERTEXBUFFER_HPP
#error g3dVertexBuffer.hpp multiply included
#endif
#define G3D_VERTEXBUFFER_HPP

#ifndef G3D_TYPE_HPP
#include "Graphics/g3d/g3dType.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/Ma/maAxisBox.hpp"
#endif


//============================================================================
//============================================================================
class g3dVertexBuffer
{
	public:
		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		virtual ~g3dVertexBuffer() {};

		//---------------------------------------------------------------------------
		// UpdateVertices - alter the position of the vertices in the
		// given fragment. i_pNormals may be NULL, in which case the
		// normals should remain as before. i_NumVertices should
		// represent the number of positions given and should match the
		// number of vertices in the fragment.
		// This method can only be called on a fragment that was created
		// with the "morphable" flag set to true.
		//---------------------------------------------------------------------------
		virtual void UpdateVertices( int i_NumVertices, 
									 const maPoint3d* i_pVertices, 
									 const maVector3d* i_pNormals = NULL ) = 0;

		//----------------------------------------------------------------------------
		//	GetNumVertices - the number of vertices in the vertex buffer
		//----------------------------------------------------------------------------
		virtual int GetNumVertices() const = 0;

		//---------------------------------------------------------------------------
		// GetVertexFormat - returns vertex format of vertex buffer using the
		//	enumeration in g3dType.
		//---------------------------------------------------------------------------
		virtual g3dType::VertexFormat GetVertexFormat() const = 0;

		//--------------------------------------------------------------------
		//  Lock - returns the pointer to the vertex buffer copy in system memory.
		//  This should be called when you need to modify the vertices
		//  ONLY should be called on morphable fragments
		//--------------------------------------------------------------------
		virtual unsigned char* Lock() = 0;

		//--------------------------------------------------------------------
		//  Unlock - updates the vertex buffer in VRAM. This should be called
		//  when you are done modifying the vertices.
		//	ONLY should be called on morphable fragments
		//--------------------------------------------------------------------
		virtual void Unlock() = 0;

		//----------------------------------------------------------------------------
		//	SetBoundingBox - Objects with a dynamic vertex buffer should set the
		//  the bounding box after modifying the buffer
		//----------------------------------------------------------------------------
		virtual void SetBoundingBox( const maAxisBox& i_BoundingBox ) = 0;
};

