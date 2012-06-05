/****************************************************************************\
**	g3dIndexPtr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dIndexPtr.hpp"


//----------------------------------------------------------------------------
// Static helper function that returns true if the number of vertices
// requires the use of 32 bit indices.
//----------------------------------------------------------------------------
//static 
bool g3dIndexPtr::Needs32Bit(int i_NumVertices)
{
	return (i_NumVertices > 0xffff);
}


//----------------------------------------------------------------------------
// Constructors recognize and store pointer type
//----------------------------------------------------------------------------
g3dIndexPtr::g3dIndexPtr(const envType::UInt16* i_pIndices)
:	m_pIndices16(i_pIndices), m_pIndices32(NULL)
{
}
g3dIndexPtr::g3dIndexPtr(const envType::UInt32* i_pIndices)
:	m_pIndices16(NULL), m_pIndices32(i_pIndices)
{
}

//----------------------------------------------------------------------------
// Query type of pointer
//----------------------------------------------------------------------------
bool g3dIndexPtr::Is16BitIndices() const
{
	return (m_pIndices16 != NULL);
}
bool g3dIndexPtr::Is32BitIndices() const
{
	return (m_pIndices32 != NULL);
}

//----------------------------------------------------------------------------
// Get size of an index in bytes (16 -> 2, 32 -> 4)
//----------------------------------------------------------------------------
int g3dIndexPtr::GetSizeOfIndex() const
{
	if (Is32BitIndices())
		return 4;
	else
		return 2;
}

//----------------------------------------------------------------------------
// Slow individual index access, try to use the pointers when you can
//----------------------------------------------------------------------------
envType::UInt32 g3dIndexPtr::GetIndex(int i) const
{
	return (m_pIndices16 != NULL) ? m_pIndices16[i] : m_pIndices32[i];
}
