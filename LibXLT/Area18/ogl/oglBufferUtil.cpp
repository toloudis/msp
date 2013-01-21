/*****************************************************************************
**  g3dDX11BufferUtil.cpp
**
**      g3dDX11BufferUtil has functions for dealing with vertex and index
**		buffers
**
** Area17
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "Area18/ogl/oglBufferUtil.hpp"

#include "Area18/ogl/oglDevice.hpp"
#include "Area18/ogl/oglBuffer.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g3d/g3dType.hpp"

//--------------------------------------------------------------------
// Anonymous Namespace for local variables and functions
//--------------------------------------------------------------------
namespace
{
	bool l_bGeometryInVideoMemory = true;

	unsigned int l_TotalVertexBufferMemory = 0;
	unsigned int l_TotalIndexBufferMemory = 0;
	unsigned int l_NumVertexBuffers = 0;
	unsigned int l_NumIndexBuffers = 0;
}

//--------------------------------------------------------------------
// Set flag for whether to store geometry in video or system memory
//--------------------------------------------------------------------
void oglBufferUtil::StoreGeometryInVideoMemory(bool i_bVideoMem)
{
	l_bGeometryInVideoMemory = i_bVideoMem;
}

//--------------------------------------------------------------------
// Get current video mem usage totals
//--------------------------------------------------------------------
float oglBufferUtil::GetTotalVertexBufferMemory()
{
	return (float)(l_TotalVertexBufferMemory)/1024.0f;
}
float oglBufferUtil::GetTotalIndexBufferMemory()
{
	return (float)(l_TotalIndexBufferMemory)/1024.0f;
}
unsigned int oglBufferUtil::GetNumVertexBuffers()
{
	return l_NumVertexBuffers;
}
unsigned int oglBufferUtil::GetNumIndexBuffers()
{
	return l_NumIndexBuffers;
}

//--------------------------------------------------------------------
//	CreateVertexBuffer
//--------------------------------------------------------------------
void oglBufferUtil::CreateVertexBuffer(	oglDevice* i_pDevice,
										   int i_nBufferSize,
											DWORD i_VertexShader,
											oglBufferHandle& io_pVertexBuffer,
											bool i_bMorphable,
											void* i_VertexData /*= NULL*/,
											int i_VertexDataSize /*= 0*/)
{
	GLuint usage = ( i_bMorphable ) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
	// no cpu read access for now?
	//UINT cpuAccess = ( i_bMorphable ) ? D3D11_CPU_ACCESS_WRITE : 0; 

	GLenum target = GL_ARRAY_BUFFER;

	// Create the vertex buffer
	io_pVertexBuffer = i_pDevice->CreateBuffer(target, i_nBufferSize, i_VertexData);
	if( !io_pVertexBuffer )
	{
		DBG_ASSERT( false, "Error creating vertex buffer" );
	}

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(g3dType::BumpTex1Vertex), 0); 
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(g3dType::BumpTex1Vertex), (GLvoid*)(sizeof(float)*3)); 
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(g3dType::BumpTex1Vertex), (GLvoid*)(sizeof(float)*6)); 
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(g3dType::BumpTex1Vertex), (GLvoid*)(sizeof(float)*8)); 
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(g3dType::BumpTex1Vertex), (GLvoid*)(sizeof(float)*11)); 

	l_TotalVertexBufferMemory += i_nBufferSize;
//	DBG_LOG("[++]Adding vertex memory, " << l_TotalVertexBufferMemory << " total.");
	l_NumVertexBuffers++;
}

//--------------------------------------------------------------------
//	ReleaseVertexBuffer
//--------------------------------------------------------------------
void oglBufferUtil::ReleaseVertexBuffer(oglDevice* i_pDevice,
											oglBufferHandle i_pVertexBuffer)
{
	if (i_pVertexBuffer != NULL)
	{
		// get the size.
		// alternative, could store the size as member var
		UINT size = i_pVertexBuffer->GetSize();

		i_pVertexBuffer.reset();

		l_TotalVertexBufferMemory -= size;
//		DBG_LOG("[--]Remove vertex memory, " << l_TotalVertexBufferMemory << " remains.");
		l_NumVertexBuffers--;
	}
}
//--------------------------------------------------------------------
//	LockVertexBuffer
//--------------------------------------------------------------------
BYTE* oglBufferUtil::LockVertexBuffer(	oglBufferHandle io_pVertexBuffer,
											bool i_bMorphable  )
{

	GLbitfield flags = ( i_bMorphable ) ? GL_WRITE_ONLY : GL_READ_WRITE;
	glBindBuffer(GL_ARRAY_BUFFER, io_pVertexBuffer->GetBuffer());
	void* p = glMapBuffer(GL_ARRAY_BUFFER, flags );
	if( !p )
	{
		DBG_ASSERT( false, "Error locking vertex buffer" );
	}
	return (BYTE*)p;
}

//--------------------------------------------------------------------
//	UnlockVertexBuffer
//--------------------------------------------------------------------
void oglBufferUtil::UnlockVertexBuffer( oglBufferHandle i_pVertexBuffer )
{
	GLboolean ok = glUnmapBuffer(GL_ARRAY_BUFFER);
}

//--------------------------------------------------------------------
//	CreateAndFillIndexBuffer - decides if 16 or 32 indices are 
//	needed, creates the appropriate format of index buffer
//	and returns the total size of the buffer in bytes.
//--------------------------------------------------------------------
int oglBufferUtil::CreateAndFillIndexBuffer(oglDevice* i_pDevice,
												const g3dIndexPtr& i_Indices,
												int i_NumIndices,
												int i_NumVertices,
												oglBufferHandle& io_pIndexBuffer,
												bool i_bMorphable )
{
	GLuint usage = ( i_bMorphable ) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
	// no cpu read access for now?
	//UINT cpuAccess = ( i_bMorphable ) ? D3D11_CPU_ACCESS_WRITE : 0; 

	GLenum target = GL_ELEMENT_ARRAY_BUFFER;

	// force 32 bit for shaderresrouceview (ray trace) reasons
	//bool bNeed32Bit = true;
	bool bNeed32Bit = g3dIndexPtr::Needs32Bit(i_NumVertices);
	int nSizeOfIndex = bNeed32Bit? 4 : 2;
	int nBufferSize = i_NumIndices * nSizeOfIndex;

	// get the system mem data ptr squared away.
	envType::UInt16* buffer_16 = NULL;
	envType::UInt32* buffer_32 = NULL;
	void* pSysMem = NULL;
	if (i_Indices.Is32BitIndices())
	{
		if (bNeed32Bit)
		{
			// Need 32-bit and have 32-bit
			pSysMem = (void*)i_Indices.GetIndices32();
		}
		else
		{
			// Have 32-bit, but only need 16-bit

			buffer_16 = new envType::UInt16[i_NumIndices];
			for (int i=0; i<i_NumIndices; i++)
			{
				buffer_16[i] = i_Indices.GetIndices32()[i];
			}
			pSysMem = buffer_16;
		}
	}
	else
	{
		// Have 16-bit, but need 32-bit

		/*buffer_32 = new envType::UInt32[i_NumIndices];
		for (int i=0; i<i_NumIndices; i++)
		{
			buffer_32[i] = i_Indices.GetIndices16()[i];
		}
		pSysMem = buffer_32;*/

		// If we only have 16-bit vertices, there shouldn't be a case where
		// we needed 32-bit. That would mean that the given indices can't
		// handle the vertices.
		DBG_ASSERT(!bNeed32Bit, "Need 32-bit indices, but are only given 16-bit");
		pSysMem = (void*)i_Indices.GetIndices16();
	}

	// Create the index buffer
	io_pIndexBuffer = i_pDevice->CreateBuffer(target, nBufferSize, pSysMem);
	if( !io_pIndexBuffer )
	{
		DBG_ASSERT( false, "Error creating index buffer" );
	}

	if (buffer_16)
		delete [] buffer_16;
	if (buffer_32)
		delete [] buffer_32;
	return nBufferSize;
}

//--------------------------------------------------------------------
//	ReleaseIndexBuffer
//--------------------------------------------------------------------
void oglBufferUtil::ReleaseIndexBuffer(oglDevice* i_pDevice,
										   oglBufferHandle i_pIndexBuffer)
{
	if (i_pIndexBuffer != NULL)
	{
		// get the size.
		// alternative, could store the size as member var
		UINT size = i_pIndexBuffer->GetSize();

		i_pIndexBuffer.reset();

		l_TotalIndexBufferMemory -= size;
		l_NumIndexBuffers--;
	}
}

//--------------------------------------------------------------------
//	LockIndexBuffer
//--------------------------------------------------------------------
BYTE* oglBufferUtil::LockIndexBuffer(	oglBufferHandle io_pIndexBuffer,
											bool i_bMorphable  )
{
	GLbitfield flags = ( i_bMorphable ) ? GL_WRITE_ONLY : GL_READ_WRITE;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, io_pIndexBuffer->GetBuffer());
	void* p = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, flags );
	if( !p )
	{
		DBG_ASSERT( false, "Error locking vertex buffer" );
	}
	return (BYTE*)p;
}

//--------------------------------------------------------------------
//	UnlockIndexBuffer
//--------------------------------------------------------------------
void oglBufferUtil::UnlockIndexBuffer( oglBufferHandle i_pIndexBuffer )
{
	GLboolean ok = glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

//--------------------------------------------------------------------
//	CreateBufferCopy
//--------------------------------------------------------------------
BYTE* oglBufferUtil::CreateBufferCopy( int i_nBufferSize, const BYTE* i_pBuffer )
{
	BYTE* buffer_copy = new BYTE[ i_nBufferSize ];
	memcpy( buffer_copy, i_pBuffer, i_nBufferSize );
	return buffer_copy;
}

BYTE* oglBufferUtil::CreateBufferCopy( int i_nBufferSize, const g3dIndexPtr& i_Indices, int i_indexSize, int i_nIndices )
{
	BYTE* buffer_copy = new BYTE[ i_nBufferSize ];
	envType::UInt16* indices16 = reinterpret_cast<envType::UInt16*>(buffer_copy);
	envType::UInt32* indices32 = reinterpret_cast<envType::UInt32*>(buffer_copy);
	DBG_ASSERT(i_nBufferSize == i_indexSize * i_nIndices, "Bad inputs to CreateBufferCopy");
	if (i_Indices.Is32BitIndices())
	{
		// 32 bit
		if (i_indexSize == 4)
		{
			memcpy( buffer_copy, i_Indices.GetIndices32(), i_nBufferSize );
		}
		else
		{
			// type conversion - truncation may occur here if not careful
			//DBG_WARNING("Trying to copy 32 bit indices into 16 bit index buffer");
			for (int i = 0; i < i_nIndices; i++)
			{
				indices16[i] = i_Indices.GetIndex(i);
			}
		}
	}
	else 
	{
		// 16 bit
		if (i_indexSize == 2)
		{
			memcpy( buffer_copy, i_Indices.GetIndices16(), i_nBufferSize );
		}
		else
		{
			// type conversion
			for (int i = 0; i < i_nIndices; i++)
			{
				indices32[i] = i_Indices.GetIndex(i);
			}
		}
	}
	return buffer_copy;
}

//--------------------------------------------------------------------
//	GetIndexBufferSize
//--------------------------------------------------------------------
UINT oglBufferUtil::GetIndexBufferSize( oglBufferHandle i_pIndexBuffer )
{
	if (i_pIndexBuffer)
	{
		return i_pIndexBuffer->GetSize();
	}
	return 0;
}

//--------------------------------------------------------------------
//	GetVertexBufferSize
//--------------------------------------------------------------------
UINT oglBufferUtil::GetVertexBufferSize( oglBufferHandle i_pVertexBuffer )
{
	if (i_pVertexBuffer)
	{
		return i_pVertexBuffer->GetSize();
	}
	return 0;
}
