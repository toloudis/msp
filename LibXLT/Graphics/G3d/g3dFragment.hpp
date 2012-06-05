/****************************************************************************\
**	g3dFragment.hpp
**
**		A g3dFragment is the base class for the bit of geometry
**	which can be drawn in a scene when it is referenced by a scene node.
**	A given g3dFragment can be referenced by multiple g3dSceneNodes.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_FRAGMENT_HPP
#error g3dFragment.hpp multiply included
#endif
#define G3D_FRAGMENT_HPP

#ifndef G3D_TYPE_HPP
#include "Graphics/g3d/g3dType.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class effOcclusionData;
class effTexturedData;
class matMaterial;
class matTexture;


//============================================================================
//============================================================================
class g3dFragment
{
	public:
		//--------------------------------------------------------------------
		//	Constructor
		//--------------------------------------------------------------------
		g3dFragment( matMaterial* i_pMaterial, 
					 int i_nRenderMode,
					 bool i_bMorphable = false,
					 bool i_bComponentSort = false );

		//--------------------------------------------------------------------
		//  Copy Constructor
		//--------------------------------------------------------------------
		g3dFragment(const g3dFragment& i_Frag);

		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		virtual ~g3dFragment() = 0;

		//------------------------------------------------------------------------
		//	Deallocate - called when all device dependent resources should be
		//	released.
		//------------------------------------------------------------------------
		virtual void Deallocate() = 0;

		//------------------------------------------------------------------------
		//	Reallocate - called when the device has been Reset and resources can
		//	be reloaded again.
		//------------------------------------------------------------------------
		virtual void Reallocate() = 0;

		//--------------------------------------------------------------------
		//	SetRenderMode sets the rendering mode for the fragment.
		//--------------------------------------------------------------------
		void SetRenderMode( int i_nRenderMode );

		//--------------------------------------------------------------------
		//	GetRenderMode returns an integer representing the rendering mode
		//	for the fragment.
		//--------------------------------------------------------------------
		inline int GetRenderMode() const;

		//--------------------------------------------------------------------
		//	GetMaterial returns a pointer to the material that will be used
		//	to render the fragment.  The g3dFragment does not own this
		//	material.
		//--------------------------------------------------------------------
		inline matMaterial* GetMaterial();
		inline const matMaterial* GetMaterial() const;

		//--------------------------------------------------------------------
		//	Get Original Material, before any highlight
		//--------------------------------------------------------------------
		inline const matMaterial* GetOriginalMaterial() const;

		//--------------------------------------------------------------------
		//	SetMaterial sets the material that will be used to render
		//  the fragment.  The g3dFragment does not own this material.
		//--------------------------------------------------------------------
		void SetMaterial( matMaterial* i_pMaterial );

		//--------------------------------------------------------------------
		//	SetOriginalMaterial sets the material that existed before the highlight
		//--------------------------------------------------------------------
		void SetOriginalMaterial( matMaterial* i_pOriginalMaterial );

		//----------------------------------------------------------------------------
		//	GetBoundingBox
		//----------------------------------------------------------------------------
		inline const maAxisBox& GetBoundingBox() const;

		//----------------------------------------------------------------------------
		//	SetBoundingBox - Objects with a dynamic vertex buffer should set the
		//  the bounding box after modifying the buffer
		//----------------------------------------------------------------------------
		inline void SetBoundingBox( const maAxisBox& i_BoundingBox );

		//----------------------------------------------------------------------------
		//	GetMorphable - return whether the vertex buffer is dynamic and can
		//  be modified
		//----------------------------------------------------------------------------
		inline bool GetMorphable() const;

		//----------------------------------------------------------------------------
		//	GetComponentSort - return whether the fragment needs to sort its
		//	components before rendering
		//----------------------------------------------------------------------------
		inline bool GetComponentSort() const;

		//----------------------------------------------------------------------------
		//	IsModelSpaceBox - Returns true if bounding box is in model space (default)
		//	or false if it is in world space.  SpriteGroupFrag is in world space
		//----------------------------------------------------------------------------
		inline bool IsModelSpaceBox() const;

		//----------------------------------------------------------------------------
		//	These functions control if the fragment casts a shadow.
		//----------------------------------------------------------------------------
		inline bool GetCastsShadow() const;
		inline void SetCastsShadow(bool i_bShadow);

		//--------------------------------------------------------------------
		// DitherAlphaBias is value from 0-1 to shift the shadows toward more
		// or less translucency (using random dither pattern in the shader)
		//--------------------------------------------------------------------
		void SetShadowDithering(bool i_bUseDither, float i_DitherAlphaBias);
		void GetShadowDithering(bool& o_bUseDither, float& o_DitherAlphaBias) const;

		//----------------------------------------------------------------------------
		//	These functions control if the fragment receives shadows.
		//----------------------------------------------------------------------------
		inline bool GetReceivesShadow() const;
		inline void SetReceivesShadow(bool i_bShadow);

		//----------------------------------------------------------------------------
		//	These functions control if the fragment casts ambient occlusion.
		//----------------------------------------------------------------------------
		inline bool GetCastsOcclusion() const;
		inline void SetCastsOcclusion(bool i_bOcclusion);

		//----------------------------------------------------------------------------
		//	These functions control if the fragment receives ambient occlusion.
		//----------------------------------------------------------------------------
		inline bool GetReceivesOcclusion() const;
		inline void SetReceivesOcclusion(bool i_bOcclusion);

		//----------------------------------------------------------------------------
		//	These functions control if the fragment receives GI.
		//----------------------------------------------------------------------------
		inline bool GetReceivesGI() const;
		inline void SetReceivesGI(bool i_bGI);

		//----------------------------------------------------------------------------
		//	These functions control if the fragment is a shadow hull
		//  (invisible but casts shadow)
		//----------------------------------------------------------------------------
		inline bool IsShadowHull() const;
		inline void SetShadowHull(bool i_bShadow);


		//----------------------------------------------------------------------------
		//	These functions control if the fragment is hair
		//----------------------------------------------------------------------------
		inline bool IsHair() const;
		inline void SetHair(bool i_bHair);

		//----------------------------------------------------------------------------
		//	These functions control if the fragment is double-sided
		//----------------------------------------------------------------------------
		inline bool GetDoubleSided() const;
		inline void SetDoubleSided(bool i_bVal);

		//----------------------------------------------------------------------------
		//	These functions control if the fragment is using baked texture
		//----------------------------------------------------------------------------
		inline bool g3dFragment::GetUseBakedTexture() const;
		inline void g3dFragment::SetUseBakedTexture(bool i_bVal);

		//----------------------------------------------------------------------------
		//	These functions control if the fragment has been baked
		//----------------------------------------------------------------------------
		inline bool g3dFragment::GetHasBeenBaked() const;
		inline void g3dFragment::SetHasBeenBaked(bool i_bVal);

		//----------------------------------------------------------------------------
		//	If the fragment is transparent, render it in a second transparency pass.
		//	e.g. particles, where z-ordering is more important than alpha blending.
		//----------------------------------------------------------------------------
		inline bool GetDeferTransparency() const;
		inline void SetDeferTransparency(bool i_bVal);

		//----------------------------------------------------------------------------
		//	These functions control if the fragment is draw as wireframe
		//----------------------------------------------------------------------------
		//bga - Use g3dSceneNode::SetDrawStyle() to set wireframe
		//inline bool GetDrawWireframe() const;
		//inline void SetDrawWireframe(bool i_bVal);

		//----------------------------------------------------------------------------
		//	GetCount returns the number of g3dFragments in existence, primarily for
		//	leak checking.
		//----------------------------------------------------------------------------
		static int GetCount();

		//----------------------------------------------------------------------------
		//	SetModelSpaceBox - set true if bounding box is in model space (default) or
		//	false if it is in world space
		//----------------------------------------------------------------------------
		void SetModelSpaceBox( bool i_bModelBox );

		//----------------------------------------------------------------------------
		//	SetModelSpaceVertices - set true if vertices need a obj-to-world space
		//	transform (default) or false if vertices are in world space
		//----------------------------------------------------------------------------
		void SetModelSpaceVertices( bool i_bModelVertices );
		bool IsModelSpaceVertices(  ) const;

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

		//----------------------------------------------------------------------------
		//	GetNumIndices - the number of indices in the index buffer
		//----------------------------------------------------------------------------
		virtual int GetNumIndices() const = 0;

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
		// LockIndices()
		//----------------------------------------------------------------------------
		virtual unsigned char* LockIndices() = 0;

		//----------------------------------------------------------------------------
		// UnlockIndices()
		//----------------------------------------------------------------------------
		virtual void UnlockIndices() = 0;

		//--------------------------------------------------------------------
		//  ReadOnlyLock - DO NOT CALL THIS EVER. It's temporary for Renderman
		//--------------------------------------------------------------------
		virtual unsigned char* ReadOnlyLock() = 0;

		//--------------------------------------------------------------------
		//  ReadOnlyUnlock - DO NOT CALL THIS EVER. It's temporary for Renderman
		//--------------------------------------------------------------------
		virtual void ReadOnlyUnlock() = 0;

		//----------------------------------------------------------------------------
		// ReadOnlyLockIndices() - DO NOT CALL THIS EVER. It's temporary for Renderman
		//----------------------------------------------------------------------------
		virtual unsigned char* ReadOnlyLockIndices() = 0;

		//----------------------------------------------------------------------------
		// ReadOnlyUnlockIndices() - DO NOT CALL THIS EVER. It's temporary for Renderman
		//----------------------------------------------------------------------------
		virtual void ReadOnlyUnlockIndices() = 0;

		//---------------------------------------------------------------------------
		// ComponentSort - let the fragment sort its internal components before
		// rendering. The current model to world transformation and the camera 
		// position are passed as arguments in order to do the sorting.
		//---------------------------------------------------------------------------
		virtual void ComponentSort(const maMatrix4x4& i_Transorm,
								   const maPoint3d& i_CameraPos) = 0;

		//---------------------------------------------------------------------------
		//---------------------------------------------------------------------------
		virtual void Split(const maAxisBox& i_camSpaceBox) {} //= 0;
		// these are only used in transparency sorting (see g3dTransparencySortDX11)
		struct sSubFrag
		{
			maAxisBox m_bounds;
			int m_startIndex;
			int m_numTris;
			sSubFrag():m_startIndex(0),m_numTris(0) {}
		};
		//---------------------------------------------------------------------------
		//---------------------------------------------------------------------------
		std::vector<sSubFrag>& SubFragments() {return m_subFragments;}
		//---------------------------------------------------------------------------
		//---------------------------------------------------------------------------
		const std::vector<sSubFrag>& GetSubFragments() const {return m_subFragments;}

		//---------------------------------------------------------------------------
		// GetFaceInfo - return a triangle from the underlying mesh, if one exists.
		//	If no such triangle can be found, return false. This call can be expensive
		//	as it may involve locking a vertex buffer.
		//---------------------------------------------------------------------------
		virtual bool GetFaceInfo(int i_Face, maPoint3d o_Points[3], maVector3d o_Normals[3]) {return false;} /* = 0; */

		//---------------------------------------------------------------------------
		//---------------------------------------------------------------------------
		//effOcclusionData* GetOcclusionData() const;

		//----------------------------------------------------------------------------
		//	These functions control if the fragment receives texture baking.
		//----------------------------------------------------------------------------
		inline bool GetReceivesBake() const;
		inline void SetReceivesBake(bool i_bBake);

		//----------------------------------------------------------------------------
		//	Return uv scaling factors to keep uvs within 0..1
		//----------------------------------------------------------------------------
		virtual void GetUVBakeFactors(maPoint2d& o_Scale, maPoint2d& o_Translate) const;

		//----------------------------------------------------------------------------
		//	Return uv overlap factor
		//----------------------------------------------------------------------------
		virtual float GetUVOverlapFactor() const;

		//----------------------------------------------------------------------------
		//	GetSize returns approximate amount of memory (in bytes) being
		// used by this fragment
		//----------------------------------------------------------------------------
		virtual unsigned int GetSize() const;

		//----------------------------------------------------------------------------
		// Should the fragment be hardware tesellated (if possible)?
		//----------------------------------------------------------------------------
		inline bool GetHardwareTesselate() const;
		inline void SetHardwareTesselate(bool i_bTesselate);

		//----------------------------------------------------------------------------
		//	GetHardwareTessellateVal - returns the value of hardware tessellation for this fragment.
		//----------------------------------------------------------------------------
		inline float GetHardwareTesselateVal() const;

		//----------------------------------------------------------------------------
		//	SetHardwareTessellateVal - Sets the value for hardware tessellation for this fragment.
		//  If not supported defaults to 0.
		//----------------------------------------------------------------------------
		inline void SetHardwareTesselateVal( float val );

		inline void SetHardwareTessellateMeshTexture( matTexture* i_pTexture );
		inline matTexture* GetHardwareTesselateMeshTexture() const;

		//----------------------------------------------------------------------------
		// Should the fragment be animated with hardware skinning (if possible)?
		//----------------------------------------------------------------------------
		inline bool GetHasSkinning() const;
		virtual void SetHasSkinning(bool i_bSkinning);

		//----------------------------------------------------------------------------
		// Should the fragment be animated with hardware skinning (if possible)?
		//----------------------------------------------------------------------------
		inline bool GetHasVelocityBuffer() const;
		virtual void SetHasVelocityBuffer(bool i_bVelocity);

		//----------------------------------------------------------------------------
		// Avoid copying all these matrices an extra time when the shader needs them
		//----------------------------------------------------------------------------
		inline const std::vector<maMatrix4x4>& GetSkinningPalette() const;
		inline std::vector<maMatrix4x4>& GetSkinningPalette();
		inline void SetSkinningPalette(const std::vector<maMatrix4x4>& i_Matrices);

		//--------------------------------------------------------------------
		//  LockSkinning - returns the pointer to the skinning buffer copy in 
		//	system memory. This should be called when you need to modify the 
		//	vertices.  ONLY should be called on morphable fragments
		//--------------------------------------------------------------------
		virtual unsigned char* LockSkinning() { return NULL; }

		//--------------------------------------------------------------------
		//  UnlockSkinning - updates the skinning buffer in VRAM. This should 
		//	be called when you are done modifying the data.
		//	ONLY should be called on morphable fragments
		//--------------------------------------------------------------------
		virtual void UnlockSkinning() {}

		//--------------------------------------------------------------------
		// GetFragmentName()
		//--------------------------------------------------------------------
		std::string GetFragmentName();

		//--------------------------------------------------------------------
		// SetFragmentName()
		//--------------------------------------------------------------------
		void SetFragmentName( std::string i_Name );

	private:
		matMaterial* m_pMaterial;
		matMaterial* m_pOriginalMaterial; //placeholder when highlight is applied
		int m_nRenderMode;
		maAxisBox m_BoundingBox;
		struct
		{
			bool m_bMorphable : 1;		// fragment's data can be changed
			bool m_bComponentSort : 1;	// this fragment needs to be sorted internally before rendering
			bool m_bModelSpaceBox : 1;	// bounding box in model space
			bool m_bCastsShadow : 1;	// casts shadows
			bool m_bReceivesShadow : 1;	// receives light from shadow sources
			bool m_bShadowHull : 1;		// fragment is invisible, but casts shadow
			bool m_bDoubleSided : 1;	// render both sides of triangles
			bool m_bUseBakedTexture : 1;// use baked texture
//			bool m_bDrawWireframe : 1;	//to enable wireframe drawing otherwise solid
			bool m_bHair : 1;			// fragment is hair geometry
		} m_Flags;

		// do we want the object to occlude others for ambient occlusion
		bool m_bIsOccluder;
		// do we want to compute occlusion for this object
		bool m_bReceivesOcclusion;
		bool m_bDeferTransparency;

		bool m_bReceivesGI;

		// data for randomly dithering shadows where the fragment has transparency
		// this is currently only used by particles(?)
		bool m_bUseDitheredShadow;
		float m_DitherAlphaBias;

		bool m_bModelSpaceVertices;

		//effOcclusionData* m_pOcclusionData;
		bool m_bReceivesBake;

		// A flag for initial implementation of hardware tesellation.
		// This could be expanded into a more complicated level of detail
		// level later.
		bool m_bHardwareTesselate;
		float m_fHardwareTessellateVal;
		matTexture* m_pTessellateMeshTexture;

		// Flag to enable/disable sending skinning data to hardware.
		// Note that the shaders have to be aware of whether skin
		// info is part of the vertex data!
		bool m_bHasSkinning;
		bool m_bHasVelocityBuffer;
		std::vector<maMatrix4x4> m_SkinningPalette;

		std::string m_FragmentName;

protected:
		// these are only used in transparency sorting (see g3dTransparencySortDX11 and g3dFragment::Split())
		std::vector<sSubFrag> m_subFragments;
};

//--------------------------------------------------------------------
//	GetRenderMode
//--------------------------------------------------------------------
inline int g3dFragment::GetRenderMode() const
{
	return m_nRenderMode;
}

//--------------------------------------------------------------------
//	GetMaterial
//--------------------------------------------------------------------
inline matMaterial* g3dFragment::GetMaterial()
{
	return m_pMaterial;
}

inline const matMaterial* g3dFragment::GetMaterial() const
{
	return m_pMaterial;
}

//--------------------------------------------------------------------
//	Get Original Material, before any highlight
//--------------------------------------------------------------------
inline const matMaterial* g3dFragment::GetOriginalMaterial() const
{
	return m_pOriginalMaterial;
}

//----------------------------------------------------------------------------
//	GetBoundingBox
//----------------------------------------------------------------------------
inline const maAxisBox& g3dFragment::GetBoundingBox() const
{
	return m_BoundingBox;
}

//----------------------------------------------------------------------------
//	SetBoundingBox
//----------------------------------------------------------------------------
inline void g3dFragment::SetBoundingBox( const maAxisBox& i_BoundingBox )
{
	m_BoundingBox = i_BoundingBox;
}

//----------------------------------------------------------------------------
//	GetMorphable - return whether the vertex buffer is dynamic and can
//  be modified
//----------------------------------------------------------------------------
inline bool g3dFragment::GetMorphable() const
{
	return m_Flags.m_bMorphable;
}

//----------------------------------------------------------------------------
//	GetComponentSort - return whether the fragment needs to sort its
//	components before rendering
//----------------------------------------------------------------------------
inline bool g3dFragment::GetComponentSort() const
{
	return m_Flags.m_bComponentSort;
}

//----------------------------------------------------------------------------
//	IsModelSpaceBox
//----------------------------------------------------------------------------
inline bool g3dFragment::IsModelSpaceBox() const
{
	return m_Flags.m_bModelSpaceBox;
}

//----------------------------------------------------------------------------
//	SetModelSpaceVertices - set true if vertices need a obj-to-world space
//	transform (default) or false if vertices are in world space
//----------------------------------------------------------------------------
inline void g3dFragment::SetModelSpaceVertices( bool i_bModelVertices )
{
	m_bModelSpaceVertices = i_bModelVertices;
}
inline bool g3dFragment::IsModelSpaceVertices(  ) const
{
	return m_bModelSpaceVertices;
}

//----------------------------------------------------------------------------
//	These functions control if the fragment casts a shadow.
//----------------------------------------------------------------------------
inline bool g3dFragment::GetCastsShadow() const
{
	return m_Flags.m_bCastsShadow;
}

inline void g3dFragment::SetCastsShadow(bool i_bShadow)
{
	m_Flags.m_bCastsShadow = i_bShadow;
}

//----------------------------------------------------------------------------
//	These functions control if the fragment receives light 
//	from shadow sources
//----------------------------------------------------------------------------
inline bool g3dFragment::GetReceivesShadow() const
{
	return m_Flags.m_bReceivesShadow;
}

inline void g3dFragment::SetReceivesShadow(bool i_bShadow)
{
	m_Flags.m_bReceivesShadow = i_bShadow;
}

//----------------------------------------------------------------------------
//	These functions control if the fragment casts ambient occlusion.
//----------------------------------------------------------------------------
inline bool g3dFragment::GetCastsOcclusion() const
{
	return m_bIsOccluder;
}

inline void g3dFragment::SetCastsOcclusion(bool i_bOcclusion)
{
	m_bIsOccluder = i_bOcclusion;
}

//----------------------------------------------------------------------------
//	These functions control if the fragment receives ambient occlusion
//----------------------------------------------------------------------------
inline bool g3dFragment::GetReceivesOcclusion() const
{
	return m_bReceivesOcclusion;
}

inline void g3dFragment::SetReceivesOcclusion(bool i_bOcclusion)
{
	m_bReceivesOcclusion = i_bOcclusion;
}

//----------------------------------------------------------------------------
//	These functions control if the fragment receives global illumination
//----------------------------------------------------------------------------
inline bool g3dFragment::GetReceivesGI() const
{
	return m_bReceivesGI;
}

inline void g3dFragment::SetReceivesGI(bool i_bGI)
{
	m_bReceivesGI = i_bGI;
}

//----------------------------------------------------------------------------
//	These functions control if the fragment is a shadow hull
//  (invisible but casts shadow)
//----------------------------------------------------------------------------
inline bool g3dFragment::IsShadowHull() const
{
	return m_Flags.m_bShadowHull;
}

inline void g3dFragment::SetShadowHull(bool i_bShadow)
{
	m_Flags.m_bShadowHull = i_bShadow;
}
//----------------------------------------------------------------------------
//	These functions control if the fragment is double-sided
//----------------------------------------------------------------------------
inline bool g3dFragment::GetDoubleSided() const
{
	return m_Flags.m_bDoubleSided;
}

inline void g3dFragment::SetDoubleSided(bool i_bVal)
{
	m_Flags.m_bDoubleSided = i_bVal;
}
//----------------------------------------------------------------------------
//	These functions control if the fragment is using baked texture
//----------------------------------------------------------------------------
inline bool g3dFragment::GetUseBakedTexture() const
{
	return m_Flags.m_bUseBakedTexture;
}

inline void g3dFragment::SetUseBakedTexture(bool i_bVal)
{
	m_Flags.m_bUseBakedTexture = i_bVal;
}

//----------------------------------------------------------------------------
//	These functions control if the fragment is hair geometry
//----------------------------------------------------------------------------
inline bool g3dFragment::IsHair() const
{
	return m_Flags.m_bHair;
}

inline void g3dFragment::SetHair(bool i_bHair)
{
	m_Flags.m_bHair = i_bHair;
}

//----------------------------------------------------------------------------
//	If the fragment is transparent, render it in a second transparency pass.
//	e.g. particles, where z-ordering is more important than alpha blending.
//----------------------------------------------------------------------------
inline bool g3dFragment::GetDeferTransparency() const
{
	return m_bDeferTransparency;
}
inline void g3dFragment::SetDeferTransparency(bool i_bVal)
{
	m_bDeferTransparency = i_bVal;
}

//----------------------------------------------------------------------------
//	These functions control if the fragment is draw in wireframe
//----------------------------------------------------------------------------
//inline bool g3dFragment::GetDrawWireframe() const
//{
//	return m_Flags.m_bDrawWireframe;
//}
//
//inline void g3dFragment::SetDrawWireframe(bool i_bVal)
//{
//	m_Flags.m_bDrawWireframe = i_bVal;
//}


//----------------------------------------------------------------------------
//	These functions control if the fragment receives texture baking
//----------------------------------------------------------------------------
inline bool g3dFragment::GetReceivesBake() const
{
	return m_bReceivesBake;
}

inline void g3dFragment::SetReceivesBake(bool i_bBake)
{
	m_bReceivesBake = i_bBake;
}

//----------------------------------------------------------------------------
// Should the fragment be hardware tesellated (if possible)?
//----------------------------------------------------------------------------
inline bool g3dFragment::GetHardwareTesselate() const
{
	return m_bHardwareTesselate;
}
inline void g3dFragment::SetHardwareTesselate(bool i_bTesselate)
{
	m_bHardwareTesselate = i_bTesselate;
}


//----------------------------------------------------------------------------
//	GetHardwareTessellate - returns the value of hardware tessellation for this fragment.
//----------------------------------------------------------------------------
inline float g3dFragment::GetHardwareTesselateVal() const
{
	return m_fHardwareTessellateVal;
}

//----------------------------------------------------------------------------
//	SetHardwareTessellate - Sets the value for hardware tessellation for this fragment.
//  If not supported defaults to 0.
//----------------------------------------------------------------------------
inline void g3dFragment::SetHardwareTesselateVal( float val )
{
	m_fHardwareTessellateVal = val;
}

inline void g3dFragment::SetHardwareTessellateMeshTexture( matTexture* i_pTexture )
{
	m_pTessellateMeshTexture = i_pTexture;
}

inline matTexture* g3dFragment::GetHardwareTesselateMeshTexture() const
{
	return m_pTessellateMeshTexture;
}

//----------------------------------------------------------------------------
// Should the fragment be animated with hardware skinning (if possible)?
//----------------------------------------------------------------------------
inline bool g3dFragment::GetHasSkinning() const
{
	return m_bHasSkinning;
}

inline bool g3dFragment::GetHasVelocityBuffer() const
{
	return m_bHasVelocityBuffer;
}

//----------------------------------------------------------------------------
// Avoid copying all these matrices an extra time when the shader needs them
//----------------------------------------------------------------------------
inline const std::vector<maMatrix4x4>& g3dFragment::GetSkinningPalette() const
{
	return m_SkinningPalette;
}
inline std::vector<maMatrix4x4>& g3dFragment::GetSkinningPalette() 
{
	return m_SkinningPalette;
}
inline void g3dFragment::SetSkinningPalette(const std::vector<maMatrix4x4>& i_Matrices)
{
	m_SkinningPalette = i_Matrices;
}

