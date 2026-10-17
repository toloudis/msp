/****************************************************************************\
**	g3dIndexPtr.hpp
**
**		g3dIndexPtr defines a class for tracking 16 and 32 bit indices
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_INDEXPTR_HPP
#error g3dIndexPtr.hpp multiply included
#endif
#define G3D_INDEXPTR_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif


//============================================================================
// Class tracks which type of index pointer is being used. The pointer
// is held, but not owned by this class.
//============================================================================
class g3dIndexPtr
{
	public:
		//----------------------------------------------------------------------------
		// Static helper function that returns true if the number of vertices
		// requires the use of 32 bit indices.
		//----------------------------------------------------------------------------
		static bool Needs32Bit(int i_NumVertices);

		//----------------------------------------------------------------------------
		// Constructors recognize and store pointer type
		//----------------------------------------------------------------------------
		g3dIndexPtr(const uint16_t* i_pIndices);
		g3dIndexPtr(const uint32_t* i_pIndices);

		//----------------------------------------------------------------------------
		// Query type of pointer
		//----------------------------------------------------------------------------
		bool Is16BitIndices() const;
		bool Is32BitIndices() const;

		//----------------------------------------------------------------------------
		// Access pointers
		//----------------------------------------------------------------------------
		inline const uint16_t* GetIndices16() const;
		inline const uint32_t* GetIndices32() const;

		//----------------------------------------------------------------------------
		// Get size of an index in bytes (16 -> 2, 32 -> 4)
		//----------------------------------------------------------------------------
		int GetSizeOfIndex() const;

		//----------------------------------------------------------------------------
		// Slow individual index access, try to use the pointers when you can
		//----------------------------------------------------------------------------
		uint32_t GetIndex(int i) const;

	private:
		const uint16_t* m_pIndices16;
		const uint32_t* m_pIndices32;
};

//----------------------------------------------------------------------------
// Access pointers
//----------------------------------------------------------------------------
inline const uint16_t* g3dIndexPtr::GetIndices16() const
{
	return m_pIndices16;
}
inline const uint32_t* g3dIndexPtr::GetIndices32() const
{
	return m_pIndices32;
}


