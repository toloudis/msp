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

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
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
		g3dIndexPtr(const envType::UInt16* i_pIndices);
		g3dIndexPtr(const envType::UInt32* i_pIndices);

		//----------------------------------------------------------------------------
		// Query type of pointer
		//----------------------------------------------------------------------------
		bool Is16BitIndices() const;
		bool Is32BitIndices() const;

		//----------------------------------------------------------------------------
		// Access pointers
		//----------------------------------------------------------------------------
		inline const envType::UInt16* GetIndices16() const;
		inline const envType::UInt32* GetIndices32() const;

		//----------------------------------------------------------------------------
		// Get size of an index in bytes (16 -> 2, 32 -> 4)
		//----------------------------------------------------------------------------
		int GetSizeOfIndex() const;

		//----------------------------------------------------------------------------
		// Slow individual index access, try to use the pointers when you can
		//----------------------------------------------------------------------------
		envType::UInt32 GetIndex(int i) const;

	private:
		const envType::UInt16* m_pIndices16;
		const envType::UInt32* m_pIndices32;
};

//----------------------------------------------------------------------------
// Access pointers
//----------------------------------------------------------------------------
inline const envType::UInt16* g3dIndexPtr::GetIndices16() const
{
	return m_pIndices16;
}
inline const envType::UInt32* g3dIndexPtr::GetIndices32() const
{
	return m_pIndices32;
}


