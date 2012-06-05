/****************************************************************************\
**	sprtSpriteGroupFrag.hpp
**
**	
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef SPRT_SPRITEGROUPFRAG_HPP
#error sprtSpriteGroupFrag.hpp multiply included
#endif
#define SPRT_SPRITEGROUPFRAG_HPP

#ifndef G3D_FRAGMENT_HPP
#include "Graphics/g3d/g3dFragment.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class sprtSpriteData;
class matMaterial;


//============================================================================
//============================================================================
class sprtSpriteGroupFrag : public g3dFragment
{
	public:
		//--------------------------------------------------------------------
		// Set id for fragments in order to choose renderer
		//--------------------------------------------------------------------
		static void SetRendererId(int i_RenderMode);
		static void SetStreakRendererId(int i_RenderMode);

		//--------------------------------------------------------------------
		// Constructor
		//--------------------------------------------------------------------
		sprtSpriteGroupFrag( matMaterial* i_pMaterial );

		//--------------------------------------------------------------------
		// Destructor
		//--------------------------------------------------------------------
		virtual ~sprtSpriteGroupFrag();

		//------------------------------------------------------------------------
		//	Deallocate - called when all device dependent resources should be
		//	released.
		//------------------------------------------------------------------------
		virtual void Deallocate();

		//------------------------------------------------------------------------
		//	Reallocate - called when the device has been Reset and resources can
		//	be reloaded again.
		//------------------------------------------------------------------------
		virtual void Reallocate();

		//--------------------------------------------------------------------
		//	GetHead - returns the head of the linked list of sprite group
		//--------------------------------------------------------------------
		inline const sprtSpriteData* GetHead() const;
		inline sprtSpriteData* GetHead();

		//--------------------------------------------------------------------
		//	SetHead - sets the head of the sprite group.
		//	Does not own the sprite group.
		//--------------------------------------------------------------------
		void SetHead( sprtSpriteData* i_pHead );

		//--------------------------------------------------------------------
		// UpdateVertices - alter the position of the vertices in the
		// given fragment. i_pNormals may be NULL, in which case the
		// normals should remain as before. i_NumVertices should
		// represent the number of positions given and should match the
		// number of vertices in the fragment.
		// This method can only be called on a fragment that was created
		// with the "morphable" flag set to true.
		//--------------------------------------------------------------------
		virtual void UpdateVertices(int i_NumVertices, 
									const maPoint3d* i_pVertices, 
									const maVector3d* i_pNormals );

		//----------------------------------------------------------------------------
		//	GetNumVertices - the number of vertices in the vertex buffer
		//----------------------------------------------------------------------------
		virtual int GetNumVertices() const;

		//----------------------------------------------------------------------------
		//	GetNumIndices - the number of indices in the index buffer
		//----------------------------------------------------------------------------
		virtual int GetNumIndices() const;

		//---------------------------------------------------------------------------
		// GetVertexFormat - returns vertex format of vertex buffer using the
		//	enumeration in g3dType.
		//---------------------------------------------------------------------------
		virtual g3dType::VertexFormat GetVertexFormat() const;

		//--------------------------------------------------------------------
		//  Lock - returns the pointer to the vertex buffer copy in system memory.
		//  This should be called when you need to modify the vertices
		//  ONLY should be called on morphable fragments
		//--------------------------------------------------------------------
		virtual unsigned char* Lock();

		//--------------------------------------------------------------------
		//  Unlock - updates the vertex buffer in VRAM. This should be called
		//  when you are done modifying the vertices.
		//	ONLY should be called on morphable fragments
		//--------------------------------------------------------------------
		virtual void Unlock();		

		//--------------------------------------------------------------------
		//  LockIndices 
		//--------------------------------------------------------------------
		virtual unsigned char* LockIndices();

		//--------------------------------------------------------------------
		//  UnlockLockIndices
		//--------------------------------------------------------------------
		virtual void UnlockIndices();

		//--------------------------------------------------------------------
		//  ReadOnlyLock
		//--------------------------------------------------------------------
		virtual unsigned char* ReadOnlyLock();

		//--------------------------------------------------------------------
		//  ReadOnlyUnlock
		//--------------------------------------------------------------------
		virtual void ReadOnlyUnlock();

		//----------------------------------------------------------------------------
		// ReadOnlyLockIndices()
		//----------------------------------------------------------------------------
		virtual unsigned char* ReadOnlyLockIndices();

		//----------------------------------------------------------------------------
		// ReadOnlyUnlockIndices()
		//----------------------------------------------------------------------------
		virtual void ReadOnlyUnlockIndices();

		//--------------------------------------------------------------------
		// ComponentSort - let the fragment sort its internal components before
		// rendering. The current model to world transformation and the camera 
		// position are passed as arguments in order to do the sorting.
		//--------------------------------------------------------------------
		virtual void ComponentSort(const maMatrix4x4& i_Transorm,
								   const maPoint3d& i_CameraPos);
		
		//--------------------------------------------------------------------
		// Streak rendering renders a stretched polygon to represent the
		// motion of a particle over time.
		//--------------------------------------------------------------------
		void SetRenderStreaks(bool i_bStreaks);
		bool GetRenderStreaks() const;

		//--------------------------------------------------------------------
		// Streak Fade is value from 0-1 to modify the alpha
		// value of the stretched vertices.
		//--------------------------------------------------------------------
		float GetStreakFade() const;
		void SetStreakFade(float i_Fade);

		//--------------------------------------------------------------------
		// Streak Taper is value from 0-1 to modify the scale
		// of the stretched vertices.
		//--------------------------------------------------------------------
		float GetStreakTaper() const;
		void SetStreakTaper(float i_Taper);

	private:
		sprtSpriteData* m_pHead;
		static int sm_RendererId;
		static int sm_StreakRendererId;
		bool m_bRenderStreaks;
		float m_StreakFade;
		float m_StreakTaper;
};

//--------------------------------------------------------------------
//	GetHead - returns the head of the linked list of sprite group
//--------------------------------------------------------------------
inline const sprtSpriteData* sprtSpriteGroupFrag::GetHead() const
{
	return m_pHead;
}

inline sprtSpriteData* sprtSpriteGroupFrag::GetHead()
{
	return m_pHead;
}
