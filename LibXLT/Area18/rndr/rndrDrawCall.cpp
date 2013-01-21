#include "rndrDrawCall.h"

#include "Area18/mesh/meshTriMeshFrag.hpp"
#include "Area18/ogl/oglBuffer.hpp"
#include "Area18/shdr/shdrPipeline.hpp"

rndrDrawCall::rndrDrawCall(void)
{
}


rndrDrawCall::~rndrDrawCall(void)
{
}

void rndrDrawCall::run(oglContext* iDevice)
{
	// Set up D3D device

	DBG_ASSERT(mFrag->GetVertexStride() == sizeof(g3dType::BumpTex1Vertex), "Bad vertex format/layout.");
	glBindVertexArray(mFrag->m_VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFrag->GetIndexBuffer()->GetIndexBuffer()->GetBuffer());
	GLenum indType = (mFrag->GetIndexBuffer()->GetSizeOfIndex() == 2) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
	int numInds = 0;
	if( mFrag->GetPrimitiveType() == meshTriMeshFrag::e_TriangleList )
	{
		numInds = mFrag->GetNumNonShadowIndices();
	}
	else
	{
		numInds = mFrag->GetNumIndices();
	}
	int startIndex = 0;
	// ability to limit to a certain number of primitives per draw call:
	int drawCallSize = 60000;// multiple of 3 and of 2 for tris and lines!!
	if (drawCallSize > numInds) 
		drawCallSize = numInds;
	int i = 0;
	// draw in increments of drawCallSize indices.
	while (i <= numInds-drawCallSize)
	{
		glDrawRangeElements((GLenum)mFrag->GetPrimitiveType(), 
 			i+startIndex, 
 			i+startIndex+drawCallSize,
 			drawCallSize,
 			indType, 
 			0);
		CHECKGLERROR();
		i += drawCallSize;
	}
	// draw any remaining.
	if (i < numInds)
	{
		glDrawRangeElements((GLenum)mFrag->GetPrimitiveType(), 
 			i+startIndex, 
 			i+startIndex+numInds-i,
 			numInds-i,
 			indType, 
 			0);
	}
}
